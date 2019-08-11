#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/* HOMING CALIBRATIONS                                                       */
constexpr double carriage_homing_velocity = 2;  // in/s
constexpr double mandrel_homing_velocity = 5;   // in/s
constexpr double rotator_homing_velocity = 0.4; // rev/s
constexpr int_type rotator_steps_to_home = 103; // steps

/* CALIBRATIONS FOR USER                                                     */
constexpr double deg_wrap_angle = 55;            // Carriage velocity must be recalibrated for values in [51, 60)
constexpr double mandrel_radius = 2.24806;       // in
constexpr double filament_width = 1.0;           // in
constexpr double filament_percent_overlap = 0.0; // % in range [0, 1]
constexpr int_type total_layers = 4;             // 1 layer is 2 physical layers due to nature of winding

/* CALIBRATIONS FOR DEVELOPER                                                */
/* Define inverse relation such that mandrel velocity remains relatively stable over different wrap angles.
   Gain, ie. numerator of expression ranges from 98 @ 20 deg to 152 @ 80 deg. K must be tuned to avoid resonant
   frequency of the mandrel motor s.t. if deg_wrap_angle > 51 && deg_wrap_angle < 60 then k = 50 else k = 80 */
constexpr int_type k = 50;
constexpr double carriage_velocity = (k + (9 * deg_wrap_angle / 10)) / deg_wrap_angle; // in/s

// rotator_rev_per_sec calibrated to satify 0.1 rev/s @ 80 deg and 0.8 rev/s @ 10deg as currently required
constexpr double rotator_rev_per_sec = 8 / deg_wrap_angle; // rev/s
constexpr double wrap_angle = deg_wrap_angle * PI / 180.0; // rad

/* CONSTANTS                                                                 */
constexpr int_type steps_per_rev = 200;            // step/rev
constexpr int_type mandrel_steps_per_rev = 850;    // step/rev
constexpr double carriage_pulley_pitch = 0.195;    // in/step
constexpr int_type carriage_num_pulley_teeth = 12; // teeth/rev
constexpr double rotator_steps_per_deg = 1.1111;   // steps/deg

/* PRIVATE CONSTANTS                                                         */
namespace
{
constexpr double pass_offset_length = filament_width / cos(wrap_angle);
constexpr double eff_pass_offset_length = pass_offset_length * (1.0 - filament_percent_overlap);
} // namespace

/* DERIVED CONSTANTS                                                         */
constexpr int_type pass_offset_steps = (double)eff_pass_offset_length * mandrel_steps_per_rev /
                                       (TWO_PI * mandrel_radius);
constexpr int_type passes_per_layer = (double)1 + (TWO_PI * mandrel_radius /
                                                   eff_pass_offset_length);
constexpr int_type rotator_steps_for_wrap_angle = (double)(90 - deg_wrap_angle) * rotator_steps_per_deg;

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
} // namespace config

#endif