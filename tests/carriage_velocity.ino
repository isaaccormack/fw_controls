#include "/Users/i/Documents/Arduino/fw_controls/src/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Carriage performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/carriage_velocity.ino"

  Units Under Test: Carriage.set_velocity()

  Background: See Insight

Insight: The equation used to set velocity for the carriage uses both large and small, integer and
         floating point values in multiplication and division and therefore, the order of the equation
         is important. This test case also validates that the precision of the time unit used to
         measure velocity (microseconds) is precise enough to reflect the period at which steps take
         place at different velocities.

  Procedure:
    1. Decide on a range of values over which to execute this test case depending on whether executing
       for sanity or calibration.
    2. Tape a piece of blank paper down below the carriage on the chassis in an accessible area
    3. Position carriage over paper and place pen against side of the carriage s.t. the pen will draw a
       measurable line across the paper when the carriage goes to move
    4. Execute the software, s.t. the mandrel moves the distance specified and a line is drawn with the pen during
    5. Measure the length of the line drawn on the paper with a ruler
    6. (Optional) Calibrate config::carriage_pulley_pitch (or other required parameters) until the length travelled
       physically is within 1/16 of an inch of the distance in the software, for various speeds and lengths

  Expected Results:
    - The carriage will move X inches +- (an exceptable tolerance) where X = vel (in/s) * test_time (s) for various velocities
      and time lengths
      - In the past, velocities between 1 -> 8 in/s have been used measuring the machines precision over 1", 5", 10" and 20" sections
        with an exceptable tolerance of +- 1/16 ON AVERAGE, meaning a 1/64" tolerance is seen at speeds of 2 in/s and a 1/8" tolerance
        seen at speeds of 10+ in/s. This is deemed acceptable because the machine will spend most of its operating time at <= 6 in/s.

  LAST EXECUTED RESULTS:
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
 */

int test_carriage_velocity_()
{
  Serial.begin(115200);
  Carriage Carriage;

  /* START TEST PARAMETERS */
  // With gain at 120: For theta = 80deg, vel = 1.5 in/s
  //                   For theta = 70deg, vel = 1.72 in/s
  //                   For theta = 20deg, vel = 6 in/s
  //                   For theta = 15deg, vel = 8 in/s
  // Thus, optimize between 1.72 -> 6 in/s but test between 1.5 -> 8 in/s
  double vel = 6; // in/s
  // Test 'test_time' between 1 -> 10 s
  double test_time = 1.66666; // s
  /* END TEST PARAMETERS */

  Carriage.set_velocity(vel);

  const unsigned long int start_time = micros();
  const unsigned long int end_time = (test_time * 1000 * 1000) + start_time;

  unsigned long int step_count = 0;
  unsigned long int counter = 0;
  while (micros() < end_time)
  {
    // /* Mock Intensive process */
    // for (int i = 0; i < 20000; ++i)
    // {
    //   int k = 0;
    //   ++k;
    // }
    unsigned long int curr_usec = micros();
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

void setup()
{
  test_carriage_velocity_();
}

void loop() {}
