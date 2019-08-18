/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Switch.h"

/* Test Case: Carriage performs correct number of steps for deceleration 
  
  Precondtions:
    - tests/carriage_acceleration/ has run and passed
    - For EACH pair of config::carriage_accel_dist and config::carriage_velocity values used to
      derive the acceleration for this test, the compile-time constant Carriage::init_usec_per_step_ 
      must be properly calibrated such that the correct velocity at end of acceleration is 
      expected. Otherwise, results of this test will be incorrect due to no fault of the acceleration
      algorithm (ie. due to fault of calibration). See docs/calibration_procedures/intial_time_to_step_for_carriage_accel
      to calibrate this value.

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/carriage_deceleration/carriage_deceleration.ino"

  Test Description: Initially position the carriage config::carriage_accel_steps + num_buffer_steps away
    from the limit switch. Begin acceleration of the carriage until target velocity is reached and continue
    moving at this speed until C_Far_Switch is pressed at which point begin the deceleration of the 
    carriage. While the carriage is moving at a speed faster than the speed at which it moves for the
    first step in the acceleration profile, continue deceleration. Assert that once the machine has decelerated
    very close to the same number of steps has occured as config::carriage_accel_steps such that acceleration
    and deceleration take place over the same distance.

  Test Parametes:
    - config::carriage_accel_dist for each x in [0.5, 1, 2, 3, 4, 5, 6]
    - config::carriage_velocity in range [d, min(8 + 2 * config::carriage_accel_dist, 18)]
    Note that config::carriage_velocity in Config.h must be manually overriden from the algorithmically derived value

  Units Under Test:
    - Carriage.set_next_usec_per_step_decel()

  Background: The control algorithm for filament winding requires that the same number of steps for acceleration
    and deceleration are completed. As such, it is assumed that given the carriage decelerates to the value of 
    Carriage::get_init_usec_per_step_, it will have travelled very close to the required number of steps for 
    acceleration to the speed just decelerated from.

  Procedure:
    1. Ensure the acceleration of the carriage is calibrated for each pair of test parameter values
    2. Using a comprehensive set of pairs test parameters, run the below code

  Expected Results:
    For each pair of values tested:
    - Expect step_count after decleration to be +- 1 of config::carriage_accel_steps after deceleration

  Remedial Action on Test Fail: This test may fail if the deceleration algorithm is changed and undesired non-linearites
    in the deceleration profile arise. As linear deceleration is expected for the control algorithm, it would be
    beneficial to ensure the carriage is in fact decelerating linearly. To do this, add the following to the
    Carriage.set_next_usec_per_step_decel() method after the logic:
      Serial.print("\n");
      Serial.print(d_next_usec_per_step);
      Serial.print(" ");
      Serial.print(micros());
      Serial.print("\n");
    This data can then be copied from the serial output and plotted. Plotting d_next_usec_per_step against time
    (ensuring correct spacing of the points on the time axis) should yield a linear plot.

  LAST EXECUTED RESULTS:
    - (Aug 18, 2019) Test was run over comphrensive set of values as described in procedure and results were
      consistent with those expected
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_carriage_deceleration_()
{
  Serial.begin(115200);
  Carriage Carriage(config::carriage_step_pin, config::carriage_dir_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);

  Serial.print("\nExpected step count for decel: ");
  Serial.print(config::carriage_accel_steps);
  Serial.print("\n");

  Carriage.set_velocity(config::carriage_homing_velocity);
  Carriage.flip_dir(); // Begin by moving toward far end limit switch

  /* Home Carriage */
  int_type step_count = 0;
  int_type num_buffer_steps = 200; // Extra steps so carriage can travel at constant velocity before decelerating
  bool far_switch_pressed = false;
  while (!far_switch_pressed || step_count < config::carriage_accel_steps + num_buffer_steps)
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
  Serial.print("Expected usec per step at accel finished: ");
  Serial.print(Carriage.get_usec_per_step());
  Serial.print("\n");
  const long_int_type expected_usec_per_step = Carriage.get_usec_per_step(); // Save for use in test
  while (!C_Far_Switch.is_rising_edge())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      if (Carriage.is_accelerating())
      {
        Carriage.set_next_usec_per_step_accel();
        Carriage.check_accel_finished();
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
      else
      {
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
    }
  }

  step_count = 0;

  Carriage.start_decelerating();
  while (Carriage.get_usec_per_step() < expected_usec_per_step)
  // while (step_count < config::carriage_accel_steps) // use if paranoid (ie. to validate correct final speed given steps)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_next_usec_per_step_decel();
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
      ++step_count;

      // Use this with second loop condition (ie. while (step_count < config::carriage_accel_steps)) to see deceleration
      // Serial.print(Carriage.get_usec_per_step());
      // Serial.print(" <- usec per step \n");
    }
  }
  // if first condition used:
  //  assert that step_count ~= config::carriage_accel_steps
  // if second condition used:
  //  assert that at config::carriage_accel_steps - 1 steps, Carriage.get_usec_per_step() < expected_usec_per_step
  //      and that at config::carriage_accel_steps, Carriage.get_usec_per_step() > expected_usec_per_step

  // assert in both cases that deceleration was linear

  Serial.print("Steps completed during deceleration: ");
  Serial.print(step_count);
  Serial.print("\n");

  // Useful when testing with second loop condition
  Serial.print("Carriage.get_usec_per_step() at end of deceleration: ");
  Serial.print(Carriage.get_usec_per_step());
  Serial.print("\n");

  return 0;
}

void setup() { test_carriage_deceleration_(); }

void loop() {}
