#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/* HOMING CALIBRATIONS                                                       */
constexpr double c_homing_velocity = 1.0; // in/s
constexpr double m_homing_velocity = 5.0; // in/s
constexpr double r_homing_velocity = 0.4; // rev/s
constexpr int_type r_steps_to_home = 103; // steps

/* CONSTANTS                                                                 */
constexpr int_type steps_per_rev = 200;     // step/rev
constexpr int_type m_steps_per_rev = 850;   // step/rev
constexpr double c_pulley_pitch = 0.195;    // in/step
constexpr int_type c_num_pulley_teeth = 12; // teeth/rev
constexpr double r_steps_per_deg = 1.1111;  // steps/deg

/* CALIBRATIONS FOR USER                                                     */
constexpr double deg_wrap_angle = 70.0;          // defined on [30, 88.5] for 3 axis
constexpr double m_radius = 2.24806;             // in
constexpr double filament_width = 0.125;         // in
constexpr double filament_percent_overlap = 0.0; // % in range [0, 1]
constexpr int_type total_layers = 2;             // 1 layer is 2 physical layers due to nature of winding

/* CALIBRATIONS FOR DEVELOPER                                                */
constexpr double c_max_velocity = 18.0;             // in/s
constexpr double m_max_rev_per_sec = 1.0;           // rev/s
constexpr double m_min_resonant_rev_per_sec = 0.21; // rev/s
constexpr double m_max_resonant_rev_per_sec = 0.3;  // rev/s
constexpr int_type max_wait_steps = 350;            // in
constexpr double min_wait_dist = 3.25;              // in

/* PRIVATE HELPER CONSTANTS                                                  */
namespace
{
constexpr double pass_offset_length = filament_width / cos(deg_wrap_angle * PI / 180.0);
constexpr double eff_pass_offset_length = pass_offset_length * (1.0 - filament_percent_overlap);
} // namespace

/* DERIVED CONSTANTS                                                         */
constexpr int_type pass_offset_steps = (double)(eff_pass_offset_length * m_steps_per_rev) /
                                       (TWO_PI * m_radius);
constexpr int_type passes_per_layer = (double)1 + ((TWO_PI * m_radius) / eff_pass_offset_length);

/* I/O PINS                                                                  */
constexpr int_type c_far_switch_pin = 13;
constexpr int_type c_home_switch_pin = 12;
constexpr int_type m_encoder_switch_pin = 11;
constexpr int_type r_switch_pin = 10;
constexpr int_type r_home_switch_pin = 9;
constexpr int_type r_far_switch_pin = 8;
constexpr int_type c_step_pin = 7;
constexpr int_type c_dir_pin = 6;
constexpr int_type m_step_pin = 5;
constexpr int_type radial_dir_pin = 2;
constexpr int_type radial_step_pin = 14;
constexpr int_type r_dir_pin = 4;
constexpr int_type r_step_pin = 3;

} // namespace config

#endif