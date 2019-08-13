/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Mandrel.h"

/* Test Case: validate equation in Mandrel.set_step_count_at_start_of_pass_by_layer() is implemented correctly

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "tests/mandrel_set_step_count_at_start_of_pass_by_layer/mandrel_set_step_count_at_start_of_pass_by_layer.ino"

  Units Under Test: Mandrel.set_step_count_at_start_of_pass_by_layer()

  Insight: Used for regression / sanity testing of the Mandrel.set_step_count_at_start_of_pass_by_layer() method

  Preconditions:
    - config::mandrel_steps_per_rev = 850
    - config::mandrel_step_count_at_start_of_first_pass = 425
    for config::pass_offset_steps
      - config::deg_wrap_angle = 50
      - config::filament_width = 1
      - config::filament_percent_overlap = 0
      - config::mandrel_radius = 2.24806
    such that config::pass_offset_steps = 93
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

int test_mandrel_set_step_count_at_start_of_pass_by_layer_()
{
  Serial.begin(115200);
  Mandrel Mandrel;

  Serial.print("\nAssert layer 1 yields 471");
  Mandrel.set_step_count_at_start_of_pass_by_layer(1);
  if (Mandrel.get_step_count_at_start_of_pass() == 471)
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  Serial.print("\nAssert layer 2 yields 518");
  Mandrel.set_step_count_at_start_of_pass_by_layer(2);
  if (Mandrel.get_step_count_at_start_of_pass() == 518)
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  Serial.print("\nAssert layer 3 yields 564");
  Mandrel.set_step_count_at_start_of_pass_by_layer(3);
  if (Mandrel.get_step_count_at_start_of_pass() == 564)
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  Serial.print("\nAssert layer 11 yields 86");
  Mandrel.set_step_count_at_start_of_pass_by_layer(11);
  if (Mandrel.get_step_count_at_start_of_pass() == 86)
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  Serial.print("\nAssert layer 99 yields 778");
  Mandrel.set_step_count_at_start_of_pass_by_layer(99);
  if (Mandrel.get_step_count_at_start_of_pass() == 778)
    Serial.print(" -> PASS\n");
  else
    Serial.print(" -> FAIL\n");

  return 0;
}

void setup() { test_mandrel_set_step_count_at_start_of_pass_by_layer_(); }

void loop() {}
