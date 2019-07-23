#ifndef TEST_CARRIAGE_VELOCITY_H
#define TEST_CARRIAGE_VELOCITY_H

/* MUST GIVE FULL PATH OF INCLUDES GIVEN CURRENT PROJECT STRUCTRE */
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Carriage performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/test_runner.ino" and that
         test_carriage_velocity_ is being run by test_runner.ino.

  Units Under Test: Carriage.set_velocity()

  Insight: Ensure the equation to define the velocity for the carriage is correct by derivation and
           implementation

  Procedure:
    1. Decide on a range of values over which to execute this test case depending on whether executing
       for sanity or calibration.
    2. Tape a piece of blank paper down below the carriage on the chassis in an accessible area
    3. Position carriage over paper and place pen against side of the carriage s.t. the pen will draw a
       measurable line across the paper when the carriage goes to move
    4. Execute the software, s.t. the mandrel moves the distance specified and a line is drawn with the pen during
    5. Measure the length of the line drawn on the paper with a ruler
    6. (Optional) Calibrate config::carriage_pulley_pitch (or other required parameters) until the length travelled
       physically is within 1/16 of an inch of the distance in the software, for various speeds and lengths. As the
       machine tolerance of this part is variable and a new belt / belt wear can cary this calibration, it may be 
       necessary to recalibrate over time

  Expected Results:
    - The carriage will move X inches +- (an exceptable tolerance) where X = vel (in/s) * test_time (s) for various velocities
      and time lengths
      - In the past, velocities between 1 -> 8 in/s have been used measuring the machines precision over 1", 5", 10" and 20" sections
        with an exceptable tolerance of +- 1/16 ON AVERAGE, meaning a 1/64" tolerance is seen at speeds of 2 in/s and a 1/8" tolerance
        seen at speeds of 10+ in/s. This is deemed acceptable because the machine will spend most of its operating time at <= 6 in/s.

  LAST EXECUTED RESULTS:
    - (June 28, 2019) The pulley pitch was changed from initially 0.2 in / tooth to 0.195 which yeilds precision
      of within 1/16" over 10 inches
    - (June 31, 2019) At 2in/s for 5s the carriage moved 10" + 3/32"
                      At 6in/s for 1.66666s the carriage moved 10" +- 1/64"
                      This the machine has been optimized between the values of 2 -> 6 in/s and yeilds a tolerance of [-0", +3/32"] over 10"
                      The tolerance at speeds not listed above can be approximated by interpolation of the values listed above.
                      It has been determined that precision above this would require more advanced / complicated compensation techniques
                      to account for timing in accuracies when reading / writing from micros in the manner done here, namely some form of
                      lag factor as described in the docs, and development will continue with this range of precision. However, it has been
                      decided that physical calibrators must be in place on the machine during the execution of the control system or this
                      error will accumulate over time leading to unpredicatable / undesireable results. A physical calibrator in this sense
                      would be something such as a limit switch to indicate the machine has reached an end position. This method of using
                      physical feedback rather than keeping internal state should be used as much as possible to limit error.
    - (June 31, 2019) Change gain on carriage speed calc from 120 to 200 to overall speed up mandrel and the results is that from 30 < theta < 80
                      the carriage vel is accurate within 1/16"
    - (July 22, 2019) Test was run once with values as recorded and results were consistent with those expected
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_carriage_velocity_()
{
  Serial.begin(115200);
  Carriage Carriage;

  /* START TEST PARAMETERS */
  // Choose values that satisfy carriage velocity equation as given in config.h
  // 2 <= vel <= 6.5 for current implementation
  double vel = 2; // in/s
  // 1 <= test_time <= 10
  double test_time = 1.5; // s
  /* END TEST PARAMETERS */

  Carriage.set_velocity(vel);

  const long_int_type start_time = micros();
  const long_int_type end_time = (test_time * 1000 * 1000) + start_time;

  long_int_type step_count = 0;
  long_int_type counter = 0;
  while (micros() < end_time)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
      ++step_count;
    }
    ++counter;
  }
  // Optional debug output
  Serial.print((micros() - start_time) / 1000);
  Serial.print(" <- Total time taken (ms)\n");
  Serial.print(step_count);
  Serial.print(" <- Total steps taken\n");
  Serial.print(counter);
  Serial.print(" <- Total loops\n");
  return 0;
}

#endif
