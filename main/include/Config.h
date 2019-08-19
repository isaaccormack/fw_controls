#ifndef CONFIG_H
#define CONFIG_H

#include <math.h>

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/* HOMING CALIBRATIONS                                                       */
constexpr double carriage_homing_velocity = 1;  // in/s
constexpr double mandrel_homing_velocity = 5;   // in/s
constexpr double rotator_homing_velocity = 0.4; // rev/s
constexpr int_type rotator_steps_to_home = 103; // steps

/* CONSTANTS                                                                 */
constexpr int_type steps_per_rev = 200;            // step/rev
constexpr int_type mandrel_steps_per_rev = 850;    // step/rev
constexpr double carriage_pulley_pitch = 0.195;    // in/step
constexpr int_type carriage_num_pulley_teeth = 12; // teeth/rev
constexpr double rotator_steps_per_deg = 1.1111;   // steps/deg

/* CALIBRATIONS FOR USER                                                     */
constexpr double deg_wrap_angle = 5.0;           // Carriage velocity must be recalibrated for values in (51, 60)
constexpr double mandrel_radius = 2.24806;       // in
constexpr double filament_width = 1.0;           // in
constexpr double filament_percent_overlap = 0.0; // % in range [0, 1]
constexpr int_type total_layers = 100;           // 1 layer is 2 physical layers due to nature of winding
constexpr double carriage_accel_dist = 3.0;      // in

/* CALIBRATIONS FOR DEVELOPER                                                */
constexpr double derived_carriage_velocity = 8.0 + 2.0 * carriage_accel_dist;             // in/s
constexpr double max_carriage_velocity = 18.0;                                            // in/s
constexpr double max_mandrel_rev_per_sec = 1.0;                                           // rev/s
constexpr double mandrel_min_resonant_rev_per_sec = 0.21;                                 // rev/s
constexpr double mandrel_max_resonant_rev_per_sec = 0.3;                                  // rev/s
constexpr double wrap_angle = deg_wrap_angle * PI / 180.0;                                // rad
constexpr int_type mandrel_step_count_at_start_of_first_pass = mandrel_steps_per_rev / 2; // Find min wait time on far end calibration algorithm assumes this initial value
// constexpr int_type min_wait_steps = 3 * deg_wrap_angle;                                   // Mandrel speed increases with wrap angle => increase wait steps
// changing for faster profile
constexpr int_type min_wait_steps = 350; // Mandrel speed increases with wrap angle => increase wait steps

/* PRIVATE CONSTANTS                                                         */
namespace
{
constexpr double pass_offset_length = filament_width / cos(wrap_angle);
constexpr double eff_pass_offset_length = pass_offset_length * (1.0 - filament_percent_overlap);
} // namespace

/* DERIVED CONSTANTS                                                         */
constexpr int_type pass_offset_steps = (double)eff_pass_offset_length * mandrel_steps_per_rev /
                                       (TWO_PI * mandrel_radius);
constexpr int_type passes_per_layer = (double)1 + (TWO_PI * mandrel_radius / eff_pass_offset_length);
constexpr int_type rotator_steps_for_wrap_angle = (double)(90 - deg_wrap_angle) * rotator_steps_per_deg;
constexpr int_type carriage_accel_steps = (double)carriage_accel_dist * steps_per_rev /
                                          (carriage_pulley_pitch * carriage_num_pulley_teeth);

/* I/O PINS                                                                  */
constexpr int_type c_far_switch_pin = 13;
constexpr int_type c_home_switch_pin = 12;
constexpr int_type m_encoder_switch_pin = 11;
constexpr int_type rotator_switch_pin = 10;
constexpr int_type r_home_switch_pin = 9;
constexpr int_type r_far_switch_pin = 8;
constexpr int_type carriage_step_pin = 7;
constexpr int_type carriage_dir_pin = 6;
constexpr int_type mandrel_step_pin = 5;
constexpr int_type radial_dir_pin = 2;
constexpr int_type radial_step_pin = 14;
constexpr int_type rotator_dir_pin = 4;
constexpr int_type rotator_step_pin = 3;

/* DYNAMIC CALIBRATION ROUTINE                                                 */
double carriage_velocity = max_carriage_velocity;
double mandrel_velocity = TWO_PI * mandrel_radius * max_mandrel_rev_per_sec;
double rotator_rev_per_sec; // Make variable public
void set_max_operating_speeds()
{
  if (derived_carriage_velocity < max_carriage_velocity)
  {
    carriage_velocity = derived_carriage_velocity;
  }

  double critical_wrap_angle = atan2f(mandrel_velocity, carriage_velocity);

  Serial.print("\nWrap angle: ");
  Serial.print(config::deg_wrap_angle);
  Serial.print("\n");
  Serial.print("Wrap angle radians: ");
  Serial.print(config::wrap_angle);
  Serial.print("\n");
  Serial.print("Max carriage velocity: ");
  Serial.print(carriage_velocity);
  Serial.print("\n");
  Serial.print("Max mandrel velocity: ");
  Serial.print(mandrel_velocity);
  Serial.print("\n");
  Serial.print("Critical wrap angle: ");
  Serial.print(critical_wrap_angle);
  Serial.print("\n");

  if (wrap_angle > critical_wrap_angle) // Then bounded by max mandrel rev per sec
  {
    carriage_velocity = mandrel_velocity / tan(wrap_angle);
  }
  else // Bounded by max carriage velocity
  {
    mandrel_velocity = carriage_velocity * tan(wrap_angle);
  }

  Serial.print("Adjusted carriage velocity: ");
  Serial.print(carriage_velocity);
  Serial.print("\n");
  Serial.print("Adjusted mandrel velocity: ");
  Serial.print(mandrel_velocity);
  Serial.print("\n");

  double min_resonant_velocity = TWO_PI * mandrel_radius * mandrel_min_resonant_rev_per_sec;
  double max_resonant_velocity = TWO_PI * mandrel_radius * mandrel_max_resonant_rev_per_sec;

  Serial.print("Min resonant mandrel velocity: ");
  Serial.print(min_resonant_velocity);
  Serial.print("\n");
  Serial.print("Max resonant mandrel velocity: ");
  Serial.print(max_resonant_velocity);
  Serial.print("\n");

  if (mandrel_velocity > min_resonant_velocity && mandrel_velocity < max_resonant_velocity)
  {
    mandrel_velocity = min_resonant_velocity;
    carriage_velocity = mandrel_velocity / tan(wrap_angle);
  }

  rotator_rev_per_sec = carriage_velocity * (90.0 - deg_wrap_angle) / (2.0 * carriage_accel_dist * 360); // rev/s
}

} // namespace config

#endif