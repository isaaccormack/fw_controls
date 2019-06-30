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
    - The carriage will move X inches +- 1/16in where X = vel (in/s) * test_time (s) for various velocities and time lengths

  LAST EXECUTED RESULTS:
    - (June 28, 2019) The carriage moved to within 1/16 of the position for various velocities and time lengths
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
  double vel = 5; // in/s
  // Test 'test_time' between 1 -> 10 s
  double test_time = 1; // s
  /* END TEST PARAMETERS */

  // The lag factor is required to compensate for the increased frequency of digital writes at
  // higher speeds which cummulatively begin to lag the setting the last step time s.t. not enough
  // steps occur bc last step time is longer than it should be
  // lag factor associated with cost to read from millis and check condition every loop
  const unsigned long int LAG_FACTOR = (unsigned long int)(vel + vel / 2);

  Carriage.set_velocity(vel);

  const unsigned long int start_time = micros();
  const unsigned long int end_time = (test_time * 1000 * 1000) + start_time;

  unsigned long int error_flag = 0;
  unsigned long int step_count = 0;
  unsigned long int counter = 2;
  unsigned long int prev_counter = 0;
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
      Carriage.set_last_step_time(curr_usec - LAG_FACTOR);
      Carriage.step();
      ++step_count;

      prev_counter = counter;
      if (prev_counter == (counter - 1))
      {
        // then this for loop has been executed twice in a row (which it should never be)
        error_flag = 1;
      }
    }
    ++counter;
  }
  // Optional debug output
  if (error_flag > 0)
  {
    Serial.print("motor was stepped twice in a row due to lag factor\n");
  }
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
