/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Test Case: Carriage moves correct distance given a number of steps to complete as 
              calculated from an arbitrary velocity

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/carriage_steps_per_distance/carriage_steps_per_distance.ino"

  Units Under Test:
    Physical Parameters:
      - Timing belt pulley pitch

    Software:
      - Motor.step()
      - Motor.get_usec_per_step()
      - Motor.set_last_step_time(...)

  Background: This test case is for sanity of software and physical configuration as well as for
              callibration of necessary config parameters (namely config::carriage_pulley_pitch).

  Insight: Use this test case to manually test the fact that the carriage moves X inches physically
           as specified in the software, and to calibrate parameters as necessary to increase precision
           given that parts have changed or enough time has passed for mechanical wear to be a factor

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
    - The carriage will move X inches +- 1/16in for various speeds and lengths

  LAST EXECUTED RESULTS:
    - (June 28, 2019) The pulley pitch was changed from initially 0.2 in / tooth to 0.195 which yeilds precision
      of within 1/16" over 10 inches
    - (June 30, 2019) Sanity to double check the new method of stepping motor (as described in July 30 log in docs)
      and it was found that carriage is as or more accurate than during last round of testing (no calibrations whether
      changed). It should be noted that position testing at higher speeds > 6in/s was not accurate due to slip of timing belt
      on motor shaft but below these speeds machine was within +- 1/16" and there is no known reason why this tolerance
      would change as speed increases to ~ 10 in/s
    - (July 22, 2019) Test was run once with values as recorded and results were consistent with those expected
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_carriage_steps_per_distance_()
{
  Serial.begin(115200);
  Carriage Carriage;

  /* START TEST PARAMETERS */
  // Choose values that satisfy carriage velocity equation as given in config.h
  // 2 <= vel <= 6.5 for current implementation
  const double vel = 3; // in/s
  // 0.5 <= length <= 20
  const long_int_type length = 1; // in
  /* END TEST PARAMETERS */

  Carriage.set_velocity(vel);

  const long_int_type total_steps = (double)((length * config::steps_per_rev) / config::carriage_pulley_pitch) /
                                    config::carriage_num_pulley_teeth;

  long_int_type step_count = 0;
  while (step_count < total_steps)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
      ++step_count;
    }
  }
  // Optional debug output
  // Serial.print(step_count);
  // Serial.print(" <- Total steps taken\n");
  return 0;
}

void setup() { test_carriage_steps_per_distance_(); }

void loop() {}
