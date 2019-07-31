/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Radial Carriage moves correct distance given a number of steps to complete as 
              calculated from an arbitrary velocity

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/radial_steps_per_distance/radial_steps_per_distance.ino"

  
  This is a sanity test to check phsyical configuration of radial carriage motor. As the radial motor is 
  only an instantiation of the Carriage class, it does not require as comprehensive of testing. The test
  procedure for this test case follows that of carriage velocity but modified for the changed physical
  constraints. See that test case for further details.

  LAST EXECUTED RESULTS:
    - (July 30, 2019) Radial axis of carriage moved to desired length within 1/32" for all specified values
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_radial_steps_per_distance_()
{
  Serial.begin(115200);
  Carriage Radial(config::radial_step_pin, config::radial_dir_pin);

  /* START TEST PARAMETERS */
  // 0.5 <= vel <= 5
  const float vel = 0.5f; // in/s
  // 0.25 <= length <= 5
  const float length = 1.0f; // in
  /* END TEST PARAMETERS */

  Radial.set_velocity(vel);

  const long_int_type total_steps = (double)((length * config::steps_per_rev) / config::carriage_pulley_pitch) /
                                    config::carriage_num_pulley_teeth;

  long_int_type step_count = 0;
  while (step_count < total_steps)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Radial.get_last_step_time() > Radial.get_usec_per_step())
    {
      Radial.set_last_step_time(curr_usec);
      Radial.step();
      ++step_count;
    }
  }
  // Optional debug output
  // Serial.print(step_count);
  // Serial.print(" <- Total steps taken\n");
  return 0;
}

void setup() { test_radial_steps_per_distance_(); }

void loop() {}
