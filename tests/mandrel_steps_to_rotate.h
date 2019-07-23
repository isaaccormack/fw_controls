#ifndef TEST_MANDREL_STEPS_TO_ROTATE_H
#define TEST_MANDREL_STEPS_TO_ROTATE_H

/* MUST GIVE FULL PATH OF INCLUDES GIVEN CURRENT PROJECT STRUCTRE */
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Mandrel.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Switch.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Step counter for mandrel records 850 +- 2 steps between each mandrel limit switch rising edge (once per revolution)

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/test_runner.ino" and that
         test_mandrel_steps_to_rotate_ is being run by the test file.

  Units Under Test:
    Physical Parameters:
      - Physcial debouncing of Mandrel encoder limit switch

    Software:
      - Switch.is_rising_edge()

  Background: This test is used for sanity of the debouncing circuit such that it validates under a normal use case
              the switch does not bouce. The implementation of the step method is also tested as to validate that
              there are no blocking function calls or general design patterns which cause inaccurate steps pre revolution.

  Insight: Use this test case to validate that the number of steps per revolution output on the serial monitor is 850 +- 2

  Procedure:
    1. Vary the speed and number of revolutions as to test on an appropriate spectrum of values and long enough length of time
    2. Push test case to controller
    3. Open serial montior and read values

  Expected Results:
    - For each revolution the number of steps per revolution output on the serial montitor will be 850 +- 2. A tolerance of greater
    then +- 2 step should be seen as symptomatic of an issue within the system as there is no reason this should be acceptable.

    LAST EXECUTED RESULTS:
      - (July 22, 2019) After adding an RC circuit to debounce the switch the experimental results match expected and
        fit within the tolerance range (+- 2 step per 850 steps).
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_mandrel_steps_to_rotate_()
{
  Serial.begin(115200);
  Mandrel Mandrel;
  Switch M_Encoder_Switch(config::m_encoder_switch_pin);

  /* START TEST PARAMETERS */
  // 3.5 <= tan_vel <= 10 in/s -- for > 10in/s, results are comprimised by slippage of motor due to fast acceleration
  double tan_vel = 8;
  // 1 <= revolutions <= 20
  long_int_type revolutions = 20;
  /* END TEST PARAMETERS */

  Mandrel.set_velocity(tan_vel);

  double ang_vel = (tan_vel / TWO_PI) / config::mandrel_radius;

  // Set test time to time required to do the number of defined revolutions
  long_int_type test_time = (1000000 * revolutions) / ang_vel; // 1,000,000 us = 1s

  // Set test_time to time required to do X revolutions
  long_int_type start_time = micros();
  long_int_type end_time = test_time + start_time;

  Mandrel.clear_step_count();
  while (micros() < end_time)
  {
    long_int_type curr_usec = micros();
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

#endif
