// #include "/Users/i/Documents/Arduino/fw_controls/src/include/Switch.h"
// #include "/Users/i/Documents/Arduino/fw_controls/src/include/Config.h"

// /* Test Case: Rising edge when limit switch pressed properly detected by
//               Switch::is_rising_edge() method

//   Setup: Ensure that the arduino.json file in the .vscode folder is loading
//          this test file by setting: "sketch": "tests/switch_rising_edge.ino"

//   Unit Under Test: Switch::is_rising_edge()

//   Background: This test ensures that the primary method used by limit switches
//               on the machine to set direction of motors is working robustly

//   Insight: Want to ensure that the UUT is functioning correctly and
//            is robustly debounced such that any ambient noise or flickering
//            of the switch does not falsely trigger the is_rising_edge()
//            method to return true

//   Procedure:
//     1. Wire arduino and breadboard as seen in the attatched schematic. The
//        limit switch should be normally open as the input port expects 5V in
//        when switch is pressed. It is critical the circuit is setup correctly
//        as this will directly effect test results.
//     2. Press and hold limit switch. The built in LED should change from on
//        to off.
//     3. Repeat this process with quick and slow presses making sure that the
//        LED only flips state on a rising edge (ie. when the limit switch is
//        initially pressed down and NOT when it is released).

//   Expected Results:
//     - Every time the limit switch is depressed the limit switch should change
//       from on-to-off or vice-versa regardless of number, speed, or force of
//       button press
//  */

// int test_switch_rising_edge_()
// {
//   Serial.begin(115200);
//   pinMode(LED_BUILTIN, OUTPUT);
//   unsigned int LED_VAL = HIGH;
//   digitalWrite(LED_BUILTIN, LED_VAL);

//   Switch s(config::test_pin);

//   while (1)
//   {
//     /* Flip LED_VAL on every rising edge */
//     if (s.is_rising_edge())
//     {
//       LED_VAL == LOW ? LED_VAL = HIGH : LED_VAL = LOW;
//       digitalWrite(LED_BUILTIN, LED_VAL);

//       Serial.print("Rising Edge Occured!\n");
//     }
//   }
//   return 0;
// }

// void setup()
// {
//   test_switch_rising_edge_();
// }
