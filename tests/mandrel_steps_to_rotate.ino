#include "/Users/i/Documents/Arduino/fw_controls/src/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Switch.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* This is a simple sanity test to ensure that the step count of the mandrel is properly found to be 850 +- 1 step.
 * 
 * This tes 
 * 
 * To execute the test simply run the code and open a serial monitor and ensure that the number of mandrel steps being outputed
 * is correct within 1 step. A tolerance of steps, say +- 2 or greater indicates a problem in the system. */

/* Test Case: Step counter for mandrel records 850 +- 1 steps between each mandrel limit switch rising edge (once per revolution)

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/mandrel_steps_to_rotate.ino"

  Units Under Test:
    Physical Parameters:
      - Physcial debouncing of Mandrel encoder limit switch

    Software:
      - Switch.is_rising_edge()

  Background: This test case was historcially used to check if the debounce time of the switch in the software was too long such
              it would cause value for the number of steps of the mandrel does per revolution to be inaccurate.
              Now that the switch is physically debounced, this test is used for sanity of the debouncing circuit such that it can
              be validated the switch does not bounce. Also this does a sanity check of the is_rising_edge() method in the switch class. 

  Insight: Use this test case to see that the number of steps per revolution output on the serial monitor is 850 +- 1

  Procedure:
    1. Vary the speed and number of revolutions as to test on an appropriate spectrum of values and long enough length of time
    2. Push test case to controller
    3. Open serial montior and read values

  Expected Results:
    - For each revolution the number of steps per revolution output on the serial montitor will be 850 +- 1

    LAST EXECUTED RESULTS:
      - (July 11, 2019) After adding an RC circuit to debounce the switch the experimental results match expected and
        fit within the tolerance range (+- 1 step per 850 steps).
 */

int test_mandrel_steps_to_rotate_()
{
  Serial.begin(115200);
  Mandrel Mandrel;
  Switch M_Encoder_Switch(config::m_encoder_switch_pin);

  /* START TEST PARAMETERS */
  // Using fcn of 200/deg_wrap_angle to define carriage vel, 'tan_vel' is between 3.5 -> 14 in/s for 15 <= deg_wrap_angle <= 80
  double tan_vel = 8;
  unsigned long int revolutions = 20;
  /* END TEST PARAMETERS */

  Mandrel.set_velocity(tan_vel);

  // The two below variables are helpers in testing s.t. revolutions can be specified by velocity will be tested
  double ang_vel = tan_vel;
  ang_vel /= TWO_PI;
  ang_vel /= config::mandrel_radius;

  // Initialize test time to 1s = 1,000,000us
  // Test time = 1s * (X rev) / (Y rev/s) = s
  unsigned long int test_time = 1000000;
  test_time *= revolutions;
  test_time /= ang_vel;

  // Set test_time to time required to do X revolutions
  unsigned long int start_time = micros();
  unsigned long int end_time = test_time + start_time;

  Mandrel.clear_step_count();
  while (micros() < end_time)
  {
    unsigned long int curr_usec = micros();
    if ((curr_usec - Mandrel.get_last_step_time()) > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      Serial.print(Mandrel.get_step_count());
      Serial.print("\n");

      Mandrel.clear_step_count();
    }
  }

  return 0;
}

void setup()
{
  test_mandrel_steps_to_rotate_();
}

void loop() {}
