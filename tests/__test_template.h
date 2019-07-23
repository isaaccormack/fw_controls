#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

/* Any includes required go here */

/* Test Case: <Descriptive title goes here>

  Setup: <Required steps before test case can be executed>

  Unit Under Test: <Classes and corresponding methods or functions being tested>

  Background: <Why this test case is important>

  Insight: <A detailed look into why this test case exists>

  Test Parameters: <Variables directly effecting test output which should be varied over a 
                    a defined range to yield comprehensive testing of software>

  Procedure:
    1. <A numbered series of detailed steps and corresponding expected results
        which covers all required steps in test>

  Expected Results:
    - <Bullet points outlining overall expected results>

  Last Exectued Results:
    - <(Month DD, YYYY) The results of the testing completed with respect to the expected
       results, being sure to note any unexpected behaviour or problems>
 */

int test_case_name_()
{
  /* Setup for test here */
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  /* Execute test here */
  while (1)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    Serial.print("I Blinked!\n");
  }
  return 0;
}

#endif
