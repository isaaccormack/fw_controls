#ifndef TEST_SWITCH_RISING_EDGE_H
#define TEST_SWITCH_RISING_EDGE_H

/* MUST GIVE FULL PATH OF INCLUDES GIVEN CURRENT PROJECT STRUCTRE */
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Switch.h"
#include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

/* Test Case: Rising edge of limit switch when pressed is properly detected and
              switch is debounced.

  Setup: Ensure that the arduino.json file in the .vscode folder is loading
         this test file by setting: "sketch": "tests/test_runner.ino" and that
         test_switch_rising_edge_ is being run by the test file.

  Unit Under Test:
    Physical:
      - The RC cirucit which debounces the switch
    Software:
      - Switch::is_rising_edge()

  Background: The reliability of the switches is cricital to the machines function

  Insight: This test ensures that is_rising_edge() method is behaving
           correctly and that the RC circuit is correctly debouncing the switch

  Procedure:
    1. Wire switch into breadboard as seen in docs/debouned_pulldown_switch_schematic.pdf.
       The limit switch should be normally open as the input port expects 5V in
       when switch is pressed.
    2. Press and hold limit switch. The built in LED should change from on
       to off.
    3. Repeat this process varying the technique used to depress the switch (quickly, slowly,
       not fully, etc.).

  Expected Results:
    - Every time the limit switch is depressed the limit switch should change
      from on-to-off or vice-versa regardless of number, speed, frequency, or
      force of button press. A non-rising edge event should never be detected.

  LAST EXECUTED RESULTS:
    (July 22 2019) Switch detects every rising edge regardless of number, speed,
    frequency, or force of button press

 */

int test_switch_rising_edge_()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  unsigned int LED_VAL = HIGH;
  digitalWrite(LED_BUILTIN, LED_VAL);

  Switch s(config::c_home_switch_pin);

  while (1)
  {
    /* Flip LED_VAL on every rising edge */
    if (s.is_rising_edge())
    {
      LED_VAL == LOW ? LED_VAL = HIGH : LED_VAL = LOW;
      digitalWrite(LED_BUILTIN, LED_VAL);

      Serial.print("Rising Edge Occured!\n");
    }
  }
  return 0;
}

#endif
