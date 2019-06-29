#include "/Users/i/Documents/Arduino/fw_controls/src/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Given this ensures correct amount of steps given velocity and time length, and the other ensures
 * correct rotation given steps, we ensure correct rotation / time given velocity */

/* Assert that after letting the mandrel rotate for 'test_time' seconds, the mandrel
 * has rotated ang_vel * test_time revolutions */

/* Test Case: Mandrel performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/mandrel_velocity.ino"

  Units Under Test: Mandrel.set_velocity()

  Background: See Insight

  Insight: The equation used to set velocity for the carriage uses both large and small, integer and
           floating point values in multiplation and division and therefore, the order of the equation
           is important. This test case also validates that the precision of the time unit used to
           measure velocity (microseconds) is precise enough to reflect the period at which steps take
           place at different velocities.

  Procedure:
    1. Decide on a range of values over which to execute this test case depending on whether executing
       for sanity or calibration.
    2. Line flat on the keyway with eye level s.t. the plane of the flat is parallel with eye Line
    3. Execute the test case as to rotate the mandrel X times while leaving your eye line in same orientation
    4. Assert that the mandrel stops in the same position as it was initially after X rotations by seeing that the
       flat on the drive shaft is still parallel with your line of sight.
    5. (Optional) Calibrate required parameters itteratively until the precision of the mandrels rotation is within 1 - 3 steps

  Expected Results:
    - The mandrel will rotate exactly X rotations as specified in the software under various mandrel
      mandrel speed within 1 - 3 steps ideally.

  LAST EXECUTED RESULTS:
    - (June 28, 2019) The mandrel rotated within 1 - 3 steps after 10 rotations with ang_vel at 1 rev /s  when
      carriage_pulley_pitch was set to 0.195
 */

int test_mandrel_velocity_()
{
  Serial.begin(115200);
  Mandrel Mandrel;

  /* START TEST PARAMETERS */
  // Test 'ang_vel' between 0.1 -> 1 rev / s
  double ang_vel = 0.5; // rev/s
  // Test 'test_time' between 1 and 10s
  unsigned long int test_time = 10; // s
  /* END TEST PARAMETERS */

  /* tan vel = 2 * PI * w * r = 2 * PI * ang_vel * mandrel_radius */
  double tan_vel = TWO_PI * config::mandrel_radius * ang_vel;
  Mandrel.set_velocity(tan_vel); // in / s

  unsigned long int start_time = micros();
  unsigned long int end_time = (test_time * 1000 * 1000) + start_time;

  unsigned long int step_count = 0;
  while (micros() < end_time)
  {
    if ((micros() - Mandrel.get_last_step_time()) > Mandrel.get_usec_per_step())
    {
      Mandrel.step();
      Mandrel.set_last_step_time(micros());
      ++step_count;
    }
  }
  // Optional debug output
  Serial.print((micros() - start_time) / 1000);
  Serial.print(" <- Total time taken (ms)\n");
  Serial.print(step_count);
  Serial.print(" <- Total steps taken\n");
  return 0;
}

void setup()
{
  test_mandrel_velocity_();
}

void loop() {}
