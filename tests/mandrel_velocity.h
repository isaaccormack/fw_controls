#ifndef TEST_MANDREL_VELOCITY_H
#define TEST_MANDREL_VELOCITY_H

/* MUST GIVE FULL PATH OF INCLUDES GIVEN CURRENT PROJECT STRUCTRE */
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Mandrel performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/test_runner.ino" and that
         test_mandrel_velocity_ is being run by test_runner.ino.

  Units Under Test: Mandrel.set_velocity()

  Insight: Ensure the equation to define the velocity for the mandrel is correct by derivation and
           implementation

  Procedure:
    1. Decide on a range of tan_vel and revolutions values to execute this test case over
       depending on whether this test case is being used for sanity or calibration.
    2. Line the keyway of the drive shaft parallel with eye level s.t. the plane is barely not visible
    3. Execute the test case as to rotate the mandrel X times while leaving eye line in same orientation
    4. Assert that the mandrel stops in the same position as it was initially after X rotations by seeing that the
       flat on the drive shaft is still parallel with your line of sight.
    5. (Optional) Calibrate required parameters itteratively until the precision of the mandrels rotation is within 1 - 3 steps

  Expected Results:
    - The mandrel will rotate exactly X rotations as specified in the software under various mandrel
      mandrel speed within 1 - 3 steps ideally.

  LAST EXECUTED RESULTS:
    - (June 28, 2019) The mandrel rotated within 1 - 3 steps after 10 rotations with ang_vel at 1 rev /s  when
      carriage_pulley_pitch was set to 0.195
    - (June 31, 2019) The pattern to update last_step_time was applied resulting in the mandrels velocity being accurate
      within 1 - 3 steps over the specified tan_vel range between 1 - 10 revolutions.
    - (June 31, 2019) Update range of test velocities to 3.5 -> 14, mandrel is accurate between 3.5 and 10
    - (June 31, 2019) Update range of test velocities to 3.5 -> 10 due to mandrel slipping with speeds > 10in/s
      (and even at 10in/s). Given the machine operates at > 10in/s there would need to be acceleration into this
      to keep system accurate, note this for later development if tan_vel > 10in/s is necessary.
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_mandrel_velocity_()
{
  Serial.begin(115200);
  Mandrel Mandrel;

  /* START TEST PARAMETERS */
  // 3.5 <= tan_vel <= 10 in/s -- for > 10in/s, results are comprimised by slippage of motor due to fast acceleration
  double tan_vel = 10;
  // 1 <= revolutions <= 10
  long_int_type revolutions = 2;
  /* END TEST PARAMETERS */

  double ang_vel = (tan_vel / TWO_PI) / config::mandrel_radius;

  // Set test time to time required to do the number of defined revolutions
  long_int_type test_time = (1000000 * revolutions) / ang_vel; // 1,000,000 us = 1s

  Mandrel.set_velocity(tan_vel);

  long_int_type start_time = micros();
  long_int_type end_time = test_time + start_time;

  long_int_type step_count = 0;
  while (micros() < end_time)
  {
    long_int_type curr_usec = micros();
    if ((curr_usec - Mandrel.get_last_step_time()) > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      ++step_count;
    }
  }
  // Optional debug output
  // Serial.print((micros() - start_time) / 1000);
  // Serial.print(" <- Total time taken (ms)\n");
  // Serial.print(step_count);
  // Serial.print(" <- Total steps taken\n");
  return 0;
}

#endif
