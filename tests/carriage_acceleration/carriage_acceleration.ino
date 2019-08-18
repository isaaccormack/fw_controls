/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Switch.h"

/* Test Case: Carriage performs correct number of steps and arrives at correct velocity at the end of 
              acceleration
  
  Precondtion: For EACH pair of config::carriage_accel_dist and config::carriage_velocity values used to
               derive the acceleration for this test, the compile-time constant Carriage::init_usec_per_step_ 
               must be properly calibrated such that the correct velocity at end of acceleration is 
               expected. Otherwise, results of this test will be incorrect due to no fault of the acceleration
               algorithm (ie. due to fault of calibration). See docs/calibration_procedures/intial_time_to_step_for_carriage_accel
               to calibrate this value.

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/carriage_acceleration/carriage_acceleration.ino"

  Test Description: Initially position the carriage config::carriage_accel_steps away from the limit switch.
    Begin acceleration of carriage and assert that the limit switch will be hit approximately at the same
    time as acceleration is complete such that the current number of steps the carriage has traveled is
    very close to config::carriage_accel_steps and Carriage.get_usec_per_step() is very close to the value of
    Carriage.get_usec_per_step() that would exist if Carriage.set_velocity(config::carriage_velocity) had
    occured (ie. current velocity very close to target velocity).

  Test Parametes:
    - config::carriage_accel_dist
    - config::carriage_velocity (must manually override algorithmically derived value to desired test value)

  Units Under Test:
    - Carriage::init_usec_per_step_ (Private compile-time constant)
    - Carriage.set_next_usec_per_step_accel()

  Background: The control algorithm for filament winding assumes that the velocity at the end of the acceleration
              profile is the desired velocity for the wrap angle. Given this test case passes and proper calibration
              of the acceleration profile, this assumption is valid. 

  Procedure:
    1. For most config::carriage_accel_dist in [0.5, 1, 2, 3, 4, 5, 6] and using 
       config::carriage_velocity = min(8 + 2 * config::carriage_accel_dist, 18) as a guide for max values and 
       config::carriage_velocity = 1 as a min value, run the below code for a comprehensive set of pairs of the
       aforementioned values until confidence in acceleration is achieved. 
       
  Expected Results:
    For each pair of values tested:
    - Expect step_count to be +- 1 of config::carriage_accel_steps.
    - Expect Carriage.get_usec_per_step() after accleration to be +- 50 of Carriage.get_usec_per_step() at Carriage
      set to config::carriage_velocity.

  Remedial Action on Test Fail: This test may fail if the acceleration algorithm is changed and undesired non-linearites
    in the acceleration profile arise. As linear acceleration is expected for the control algorithm, it would be
    beneficial to ensure the carriage is infact accelerating linearly. To do this, add the following to the
    Carriage.set_next_usec_per_step_accel() method after the logic:
      Serial.print("\n");
      Serial.print(d_next_usec_per_step);
      Serial.print(" ");
      Serial.print(micros());
      Serial.print("\n");
    This data can then be copied from the serial output and plotted. Plotting d_next_usec_per_step against time
    (ensuring correct spacing of the points on the time axis) should yield a linear plot.

  LAST EXECUTED RESULTS:
    - (Aug 17, 2019) Test was run over comphrensive set of values as described in procedure and results were
      consistent with those expected
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_carriage_acceleration_()
{
  Serial.begin(115200);
  Carriage Carriage(config::carriage_step_pin, config::carriage_dir_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);

  Serial.print("\nExpected step count for accel: ");
  Serial.print(config::carriage_accel_steps);
  Serial.print("\n");
  Carriage.set_velocity(config::carriage_velocity);
  Serial.print("Expected usec per step at accel finished: ");
  Serial.print(Carriage.get_usec_per_step());
  Serial.print("\n");

  Carriage.set_velocity(config::carriage_homing_velocity);
  Carriage.flip_dir(); // Begin by moving toward far end limit switch

  /* Home Carriage */
  int_type step_count = 0;
  bool far_switch_pressed = false;
  while (!far_switch_pressed || step_count < config::carriage_accel_steps)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
      ++step_count;
    }
    if (C_Far_Switch.is_rising_edge())
    {
      delay(500);
      Carriage.flip_dir();
      far_switch_pressed = true;
      step_count = 0;
    }
  }
  delay(500);
  Carriage.flip_dir();

  Carriage.start_accelerating();
  step_count = 0;
  far_switch_pressed = false;
  while (Carriage.is_accelerating() || !far_switch_pressed)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_next_usec_per_step_accel();
      Carriage.check_accel_finished();
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
      ++step_count;
    }

    if (C_Far_Switch.is_rising_edge())
    {
      Serial.print(step_count);
      Serial.print(" <- step count at completed accel\n");
      Serial.print(Carriage.get_usec_per_step());
      Serial.print(" <- usec per step at completed accel\n");
      far_switch_pressed = true;
    }
  }

  // Assume functionality for deceleration works more or less since need to decelerate to test fast velocities
  Carriage.start_decelerating();
  while (Carriage.is_decelerating())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_next_usec_per_step_decel();
      Carriage.check_decel_finished();
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
    }
  }
  // assert distance travelled is correct
  // assert carriage reaches correct speed
  // assert acceleration was linear

  return 0;
}

void setup() { test_carriage_acceleration_(); }

void loop() {}
