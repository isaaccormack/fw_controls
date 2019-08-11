/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Mandrel performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/mandrel_velocity/mandrel_velocity.ino"

  Units Under Test: Mandrel.set_velocity()

  Insight: Ensure the equation to define the velocity for the mandrel is correct by derivation and
           implementation

  Procedure:
    1. Execute this test over the range of values at which the mandrel will rotate at according to
       the wrap angle.
    2. Ensure this test is run outside of the wrap angle which cause a resonant frequency in the mandrel 
       as well as within the wrap angle which causes a resonant frequency but with the carriage velocity
       properly calibrated as to make the mandrel rotate slower s.t. the motor does not incur a resonant
       frequency (and especially near boundaries speeds over which this calibration effects). This is to
       validate no steps are missed due to inaccurate calibration near the resonant frequency wrap angle.
       Details on calibrating this carriage velocity to compensate for this are found in the config file.
    3. Line the keyway of the drive shaft parallel with eye level s.t. the plane is barely not visible
    4. Execute the test case as to rotate the mandrel X times while leaving eye line in same orientation
    5. Assert that the mandrel stops in the same position as it was initially after X rotations by seeing that the
       flat on the drive shaft is still parallel with your line of sight.
    6. (Optional) Calibrate required parameters itteratively until the precision of the mandrels rotation is within 1 - 3 steps

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
    - (Aug. 10, 2019) Update test to use mandrel velocity as calculated in 2 axis instead of manual input which 
      slightly redefines purpose of test. Test run with new calibration to compensate for mandrel velocity resonant
      frequency between 52 and 60 degree wrap angle. With the current calibration the motor has accurate velocity
      and does not skips steps at wrap angles in and outside of the values which otherwise cause a resonant frequency.
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_mandrel_velocity_()
{
  Serial.begin(115200);
  Mandrel Mandrel;

  /* START TEST PARAMETERS */
  // config::deg_wrap_angle between 15 and 80 degrees
  // 1 <= revolutions <= 10
  const long_int_type revolutions = 3;
  /* END TEST PARAMETERS */

  /* Vm,tan / Vc = tan(wrap_angle)     =>     Vm,tan = Vc * tan(wrap_angle) */
  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);
  Mandrel.set_velocity(mandrel_velocity);

  double ang_vel = (mandrel_velocity / TWO_PI) / config::mandrel_radius;

  // Set test time to time required to do the number of defined revolutions
  const long_int_type test_time = (1000000 * revolutions) / ang_vel; // 1,000,000 us = 1s

  const long_int_type start_time = micros();
  const long_int_type end_time = test_time + start_time;

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

void setup() { test_mandrel_velocity_(); }

void loop() {}
