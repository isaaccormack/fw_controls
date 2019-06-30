#include "/Users/i/Documents/Arduino/fw_controls/src/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Mandrel performs correct number of rotations at an arbitrary angular velocity

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/mandrel_steps_per_revolution.ino"

  Units Under Test:
    Physical Parameters:
      - Gear ratio between stepper and shaft
      - Steps per revolution for stepper

    Software:
      - Motor.step()
      - Motor.get_usec_per_step()
      - Motor.set_last_step_time(...)

  Background: This test case is for sanity of software and physical configuration as well as for
              callibration of necessary config parameters (namely config::mandrel_steps_per_rev).

  Insight: Use this test case to manually test the fact that the mandrel rotates exactly X
           rotations physically as specified in the software, and adjust parameters to increase
           the precision.

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
      - (June 28, 2019) The mandrel definitely over rotates by more than 3 steps after 10 revolutions, but this is to
        be expected as the mandrel steps per rev is calibrated higher than should be (860 opposed to 850, due to the fact
        that at higher velocities, the mandrel will skips steps, so increase steps per rev by a little to compensate)
        and the code in this test takes the mandrel steps per revolution very litteraly as it determines how many steps
        are executed in the test
        Overall, a compromise is made between accurate mandrel velocity and exact control over position
      - (June 30, 2019) After applying the correction to the way the steps are counted as originated in the carriage 
        testing, and therefore setting the config::mandrel_steps_per_rev back to the proper value of 850, there is no
        visible error in the number of rotations of the mandrel over the test range. Also motor is showing no signs of 
        slipping / stalling while spinning freely or under load.
 */

int test_mandrel_steps_per_revolution_()
{
  Serial.begin(115200);
  Mandrel Mandrel;

  /* START TEST PARAMETERS */
  // Using fcn of 120/deg_wrap_angle to define carriage vel, 'tan_vel' is between 2 -> 6 in/s for 15 <= deg_wrap_angle <= 80
  double tan_vel = 2;
  // Test 'revolutions' between 1 and 10 revs
  unsigned long int revolutions = 5; // Expect mandrel to rotate 10 times
  /* END TEST PARAMETERS */

  Mandrel.set_velocity(tan_vel);

  // steps = (steps / rev) * rev
  unsigned long int total_steps = config::mandrel_steps_per_rev * revolutions;
  unsigned long int step_count = 0;
  while (step_count < total_steps)
  {
    unsigned long int curr_usec = micros();
    if ((curr_usec - Mandrel.get_last_step_time()) > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      ++step_count;
    }
  }
  // Optional debug output
  // Serial.print(step_count);
  // Serial.print(" <- Total steps taken\n");
  return 0;
}

void setup()
{
  test_mandrel_steps_per_revolution_();
}

void loop() {}
