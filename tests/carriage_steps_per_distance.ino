#include "/Users/i/Documents/Arduino/fw_controls/src/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Carriage moves correct distance at an arbitrary velocity 

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/carriage_steps_per_distance.ino"

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
           as speicifed in the software, and calibrate parameters to increase precision
  
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
    - The carriage will move X inches +- 1/16 for various speeds and lengths
  
    LAST EXECUTED RESULTS: 
      - (June 28, 2019) The pulley pitch was changed from initially 0.2 in / tooth to 0.195 which yeilds precision 
        of within 1/16" over 10 inches
 */

int test_carriage_steps_per_distance_()
{
  Serial.begin(115200);
  Carriage Carriage;

  /* START TEST PARAMETERS */
  // Test 'vel' between 1 -> 6 in/s
  double vel = 1; // in/s
  // Test 'length' between 0.5 -> 10 in
  unsigned long int length = 10; // in
  /* END TEST PARAMETERS */

  Carriage.set_velocity(vel);

  unsigned long int total_steps = length;
  total_steps *= config::steps_per_rev;
  total_steps /= config::carriage_pulley_pitch;
  total_steps /= config::carriage_num_pulley_teeth;

  unsigned long int step_count = 0;
  while (step_count < total_steps)
  {
    if ((micros() - Carriage.get_last_step_time()) > Carriage.get_usec_per_step())
    {
      Carriage.step();
      Carriage.set_last_step_time(micros());
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
  test_carriage_steps_per_distance_();
}

void loop() {}
