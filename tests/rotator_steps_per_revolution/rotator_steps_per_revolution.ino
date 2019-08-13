/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Rotator.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Rotator performs correct number of rotations given a the number of steps to compelete 
              this many rotations as calculated from an arbitrary revolutions per second

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/rotator_steps_per_revolution/rotator_steps_per_revolution.ino"

  Units Under Test:
    Physical Parameters:
      - Gear ratio between stepper and shaft
      - Steps per revolution for stepper

    Software:
      - Motor.step()
      - Motor.get_usec_per_step()
      - Motor.set_last_step_time(...)

  Background: This test case is for sanity of software and physical configuration

  Insight: Use this test case to test that the rotator rotates exactly X rotations
           as specified in the software. This test case is similar to rotator_velocity
           but relies on number of steps per revolution to defined number of revolutions
           instead of velocity (which in turn uses number of steps per revolution), thus 
           this is a lower level test and should be executed first to build testing premise

  Procedure:
    1. Decide on a range of values over which to execute this test case depending on whether executing
       for sanity or calibration.
    2. Position the applicator head vertical in its 0 degree position
    3. Standing vertically over the applicator head arm, line the arm of the applicator head with line of sight
       such that the wall of the applicator head perpendicular with line of sight is barely visible
    3. Execute the test case as to rotate the applicator head X times while leaving eye line in same orientation
    4. Assert that the applicator head stops in the same position as it was initially after X rotations

  Expected Results:
    - The rotator will rotate exactly X rotations as specified in the software under various
      speeds within 1 - 3 steps ideally.

    LAST EXECUTED RESULTS:
      - (Aug 11, 2019) The applicator head seems like it is off by at least one step per revolution, possibly
        due to machine tolerance on laser cut pulley, loose timing belt, or misallignment of the motor and 
        rotator pulley. Due to the nature of the rotator (ie. it only rotates a set amount of steps in either
        direction then the opposite direction the same amount of steps), it does not seems that this tolerance
        will be a problem. Infact, a larger deviance of up to a 10 step error could be incurred with minimal impacts.
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_rotator_steps_per_revolution_()
{
  Serial.begin(115200);
  Rotator Rotator;

  /* START TEST PARAMETERS */
  // 0.1 <= rev_per_sec <= 1 rev/s
  const float rev_per_sec = .75f;
  // 1 <= revolutions <= 20
  const long_int_type revolutions = 10;
  /* END TEST PARAMETERS */

  Rotator.set_rev_per_sec(rev_per_sec);

  // steps = 2 * (steps / rev) * rev since gear ratio
  const long_int_type total_steps = 2 * config::steps_per_rev * revolutions;
  long_int_type step_count = 0;
  while (step_count < total_steps)
  {
    long_int_type curr_usec = micros();
    if ((curr_usec - Rotator.get_last_step_time()) > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      ++step_count;
    }
  }
  // Optional debug output
  // Serial.print(step_count);
  // Serial.print(" <- Total steps taken\n");
  return 0;
}

void setup() { test_rotator_steps_per_revolution_(); }

void loop() {}
