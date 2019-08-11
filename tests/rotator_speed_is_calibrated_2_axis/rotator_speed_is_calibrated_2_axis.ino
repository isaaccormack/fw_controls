/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Rotator.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Rotator head has enough time to rotate to necessary position given it has the minimal
              possible wait time at end of pass

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/rotator_speed_is_calibrated_2_axis/rotator_speed_is_calibrated_2_axis.ino"

  Units Under Test: config::rotator_rev_per_sec

  Test Parameters: config::deg_wrap_angle
                   config::carriage_velocity

  Ensure that the config::rotator_rev_per_sec is correctly calibrated such that there is adequate time,
           during the minimal possible delay time between passes, to rotate the amount of steps necessary for
           it to reach its desired position. If this is not the case, then the rotator head wont be rotated
           again as it leaves the end position (as it is already rotating) and thus it will be misaligned for the
           remainder of the passes. 
  
  Procedure:
    1. Run the test over at least 7 wrap angles spaced linearly between 15 and 80 degrees.
    2. Ensure that the config::carriage_velocity is properly calibrated when this test is run over wrap angles
       which cause resonant frequencies

  Expected Results:
    - The rotator head will finish rotating at least 10 mandrel steps before the mandrel has reached the far end
      wait steps (ie. the minimal amount of time it can wait at either end).
  
  Actions on Test Failure:
    - config::rotator_rev_per_sec must be recalibrated (ie. constant changed accordingly) such that the rotator head moves
    as to meet the expected results (or the developer may choose to modifiy the minimum required wait time but this has many
    impacts on the control system which may initially be unclear and therefore must be changed diligently). The equation 
    currently is intended such that the rotator head only moves as fast as it needs to to move to the desired position 
    within the required time. It is likely just the gain will need to be changed to make the rotator head move faster.

  LAST EXECUTED RESULTS:
    - (Aug 10, 2019) Rotator head finishes moving with more than 10 mandrel steps left before the mandrel stops moving. With
      this buffer of steps it is very unlikely the rotator head will miss beggining to step as it leave the end position due
      to not being finished stepping.
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int rotator_speed_is_calibrated_2_axis()
{
  Serial.begin(115200);
  Rotator Rotator;
  Mandrel Mandrel;

  /* NO PARAMETERS */

  Rotator.set_rev_per_sec(config::rotator_rev_per_sec);
  Rotator.enable();

  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);
  Mandrel.set_velocity(mandrel_velocity);

  Serial.print("\nTotal mandrel wait steps: ");
  Serial.print(Mandrel.get_far_end_wait_steps());
  Serial.print("\nWrap angle is: ");
  Serial.print(config::deg_wrap_angle);
  Serial.print("\nMandrel usec per step: ");
  Serial.print(Mandrel.get_usec_per_step());
  Serial.print("\n");
  Serial.print("\n");

  bool test_pass = false;
  while (Mandrel.get_step_count() < Mandrel.get_far_end_wait_steps())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
    }

    curr_usec = micros();
    if (Rotator.is_enabled() && curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      Rotator.inc_step_count();

      if (Rotator.get_step_count() > config::rotator_steps_for_wrap_angle)
      {
        Rotator.disable();

        test_pass = true;
        Serial.print("Time: ");
        Serial.print(millis());
        Serial.print("ms, Mandrel Steps: ");
        Serial.print(Mandrel.get_step_count());
        Serial.print("\nThe rotator head has finished turning!\n");
        Serial.print("\n");
      }
    }
  }
  Serial.print("Time: ");
  Serial.print(millis());
  Serial.print("ms, Mandrel Steps: ");
  Serial.print(Mandrel.get_step_count());
  Serial.print("\nThe mandrel has finished waiting!\n");
  if (!test_pass)
  {
    Serial.print("\n---------- TEST FAILED ----------");
  }
  else
  {
    Serial.print("\n---------- TEST PASSED ----------");
  }
  return 0;
}

void setup() { rotator_speed_is_calibrated_2_axis(); }

void loop() {}
