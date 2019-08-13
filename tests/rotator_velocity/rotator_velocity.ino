/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Rotator.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Rotator performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/rotator_velocity/rotator_velocity.ino"

  Units Under Test: Rotator.set_velocity()

  Insight: Ensure the equation to define the velocity for the rotator is correct in derivation and
           implementation

  Procedure:
    1. Decide on a range of rev_per_sec and revolutions values to execute this test case over
       depending on whether this test case is being used for sanity or calibration.
    2. Position the applicator head vertical in its 0 degree position
    3. Standing vertically over the applicator head arm, line the arm of the applicator head with line of sight
       such that the wall of the applicator head perpendicular with line of sight is barely visible
    3. Execute the test case as to rotate the applicator head X times while leaving eye line in same orientation
    4. Assert that the applicator head stops in the same position as it was initially after X rotations

  Expected Results:
    - The applicator head will rotate exactly X rotations as specified in the software within 1 - 3 steps ideally.

  LAST EXECUTED RESULTS:
    - (Aug 11, 2019) Using a 2:1 ratio in the derivation of the setting the rotator velocity, the applicator
      head rotates within 1 - 3 steps of a full revolution for multiple revolutions. It should be noted that 
      a loose drive belt can effect the results by a noticable amount of steps. It is best to tension belt by
      hand if necessary.
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_rotator_velocity_()
{
  Serial.begin(115200);
  Rotator Rotator;

  /* START TEST PARAMETERS */
  // 3.5 <= tan_vel <= 10 in/s -- for > 10in/s, results are comprimised by slippage of motor due to fast acceleration
  const float rev_per_sec = 0.7f;
  // 1 <= revolutions <= 10
  const long_int_type revolutions = 10;
  /* END TEST PARAMETERS */

  // Set test time to time required to do the number of defined revolutions
  const long_int_type test_time = (1000000 * revolutions) / rev_per_sec;

  Rotator.set_rev_per_sec(rev_per_sec);

  const long_int_type start_time = micros();
  const long_int_type end_time = test_time + start_time;

  long_int_type step_count = 0;
  while (micros() < end_time)
  {
    long_int_type curr_usec = micros();
    if ((curr_usec - Rotator.get_last_step_time()) > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      ++step_count;
    }
  }

  return 0;
}

void setup() { test_rotator_velocity_(); }

void loop() {}
