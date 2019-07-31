/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Radial Carriage performs correct number of steps given a velocity and length of time

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/radial_velocity/radial_velocity.ino"

  This is a sanity test to check phsyical configuration of radial carriage motor. As the radial motor is 
  only an instantiation of the Carriage class, it does not require as comprehensive of testing. The test
  procedure for this test case follows that of carriage velocity but modified for the changed physical
  constraints. See that test case for further details.

  LAST EXECUTED RESULTS:
    - (July 30, 2019) Results were accurate within 1/32" and reproducable for the parameters as specified
    less vel > 4 in/s where results were accurate within 1/16".
  */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_axial_velocity_()
{
  Serial.begin(115200);
  Carriage Radial(config::radial_step_pin, config::radial_dir_pin);

  /* START TEST PARAMETERS */
  // 0.25 <= vel <= 5
  const float vel = 5.0f; // in/s
  // Note: Max travel length is ~5in - set test_time accordingly
  const double test_time = 1; // s
  /* END TEST PARAMETERS */

  Radial.set_velocity(vel);

  const long_int_type start_time = micros();
  const long_int_type end_time = (test_time * 1000 * 1000) + start_time;

  long_int_type step_count = 0;
  long_int_type counter = 0;
  while (micros() < end_time)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Radial.get_last_step_time() > Radial.get_usec_per_step())
    {
      Radial.set_last_step_time(curr_usec);
      Radial.step();
      ++step_count;
    }
    ++counter;
  }
  // Optional debug output
  // Serial.print((micros() - start_time) / 1000);
  // Serial.print(" <- Total time taken (ms)\n");
  // Serial.print(step_count);
  // Serial.print(" <- Total steps taken\n");
  // Serial.print(counter);
  // Serial.print(" <- Total loops\n");
  return 0;
}

void setup() { test_axial_velocity_(); }

void loop() {}
