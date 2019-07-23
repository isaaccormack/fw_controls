#include "carriage_steps_per_distance.h"
#include "carriage_velocity.h"
#include "mandrel_steps_per_revolution.h"
#include "mandrel_steps_to_rotate.h"
#include "mandrel_velocity.h"
#include "switch_rising_edge.h"

/* Arduino doesn't support multiple 'setup' and 'main' functions within a
   directory, so use a test runned to control which tests are run. */

void setup()
{
  // test_carriage_steps_per_distance_();
  // test_carriage_velocity_();
  // test_mandrel_steps_per_revolution_();
  // test_mandrel_steps_to_rotate_();
  // test_mandrel_steps_to_rotate_();
  // test_mandrel_velocity_();
  test_switch_rising_edge_();
}

void loop() {}