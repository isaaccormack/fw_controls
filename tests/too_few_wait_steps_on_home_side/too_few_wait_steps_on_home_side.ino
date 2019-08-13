/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/Utils.cpp"

/* Test Case: util::too_few_wait_steps_on_home_side() reports correctly whether the steps between the arrival
              of the carriage at the home end and the scheduled departure for the next pass is too low on
              equivalence partitioned and boundary condition values.

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/too_few_wait_steps_on_home_side/too_few_wait_steps_on_home_side.ino"

  Units Under Test: util::too_few_wait_steps_on_home_side()

  Background: It is important that there is a long enough wait time on each end of the wrap such that the 
              filament has time to settle on the end before the next pass begins. During the first pass of 
              the wind, or anytime when the step count at start of pass is change as the result of a new 
              layer, it is not guaranteed that the carriage will arrive back on the home end of the machine
              meeting both of the following criteria:
                1. Less than config::min_wait_steps steps before the step count at start of next pass
                2. More than step_tolerance (usually 20 steps) after the step count at start of next pass
              
              This util function is used for detection of this so that the control algorithm can handle the
              case accordingly. Due to the mandrel step count rolling over when it passes ~850 (when the 
              mandrel encoder switch is pressed to be exact) the algorithm to detect this has complications
              and edge cases which must be tested.

  Insight: Used for regression / sanity testing of the util::too_few_wait_steps_on_home_side() method

  Preconditions:
    - config::deg_wrap_angle = 50 such that config::min_wait_time = 150
    - step_tolerance = 20 in util::too_few_wait_steps_on_home_side
    If either of the above parameters are changed, hardcoded values in this test case must be updated.

  Procedure:
    1. Upload test case to controller
    2. Open serial output window and read output

  Expected Results:
    - All test report "PASS"

  LAST EXECUTED RESULTS:
    - (Aug 11, 2019) Results were consistent with those expected.
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int test_too_few_wait_steps_on_home_side_()
{
  Serial.begin(115200);

  /* ---------- NON-ROLLOVER TESTS ---------- */

  // INVALID - Arrives too soon before departure
  Serial.print("\n799, 800");
  if (util::too_few_wait_steps_on_home_side(799, 800))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // INVALID - Arrives too soon after departure
  Serial.print("\n810, 800");
  if (util::too_few_wait_steps_on_home_side(810, 800))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // VALID - Arrives with enough steps before
  Serial.print("\n600, 800");
  if (!util::too_few_wait_steps_on_home_side(600, 800))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // VALID - Arrives after outside of tolerance threshold
  Serial.print("\n700, 600");
  if (!util::too_few_wait_steps_on_home_side(700, 600))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  /* ---------- ROLLOVER TESTS ---------- */

  // INVALID - Arrives too soon before departure
  Serial.print("\n825, 25");
  if (util::too_few_wait_steps_on_home_side(825, 25))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // INVALID - Arrives too soon after departure
  Serial.print("\n10, 849");
  if (util::too_few_wait_steps_on_home_side(10, 849))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // VALID - Arrives with enough steps before
  Serial.print("\n800, 100");
  if (!util::too_few_wait_steps_on_home_side(800, 100))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // VALID - Arrives after outside of tolerance threshold
  Serial.print("\n10, 830");
  if (!util::too_few_wait_steps_on_home_side(10, 830))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  /* ---------- BOUNDARY CONDITIONS ---------- */

  // INVALID - Arrival and departure at same time
  Serial.print("\n800, 800");
  if (util::too_few_wait_steps_on_home_side(800, 800))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // INVALID - Check boundary on arrival one step after departure
  Serial.print("\n0, 849");
  if (util::too_few_wait_steps_on_home_side(0, 849))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // INVALID - Check boundary on arrival one step before departure
  Serial.print("\n851, 0");
  if (util::too_few_wait_steps_on_home_side(851, 0))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  /* ---------- RANDOM VALUES ---------- */

  // VALID - Enough steps between arrival and departure
  Serial.print("\n53, 3");
  if (!util::too_few_wait_steps_on_home_side(53, 3))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // VALID - Enough steps between arrival and departure
  Serial.print("\n503, 3");
  if (!util::too_few_wait_steps_on_home_side(503, 3))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  // INVALID - Arrives too soon before departure
  Serial.print("\n0, 1");
  if (util::too_few_wait_steps_on_home_side(0, 1))
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  return 0;
}

void setup() { test_too_few_wait_steps_on_home_side_(); }

void loop() {}
