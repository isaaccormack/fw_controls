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
    - The carriage will move X inches +- 1/16in for various velocities and time lengths

  LAST EXECUTED RESULTS:
    - (June 28, 2019) The carriage moved to within 1/16 of the position for various velocities and time lengths
 */

int test_carriage_velocity_()
{
  Serial.begin(115200);
  Carriage Carriage;

  /* START TEST PARAMETERS */
  // Expect mandrel to move length = vel * test_time
  // Test 'vel' between 1 -> 6 in/s
  double vel = 4; // in/s
  // Test 'test_time' between 1 -> 10 s
  unsigned long int test_time = 2; // s
  /* END TEST PARAMETERS */

  Carriage.set_velocity(vel);

  unsigned long int start_time = micros();
  unsigned long int end_time = (test_time * 1000 * 1000) + start_time;

  unsigned long int step_count = 0;
  while (micros() < end_time)
  {
    if ((micros() - Carriage.get_last_step_time()) > Carriage.get_usec_per_step())
    {
      Carriage.step();
      Carriage.set_last_step_time(micros());
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

void setup()
{
  test_carriage_velocity_();
}

void loop() {}
