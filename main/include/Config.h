#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/* HOMING CALIBRATIONS                                                       */
constexpr double c_homing_velocity = 1.0;    // in/s
constexpr double r_homing_rev_per_sec = 0.4; // rev/s
constexpr int_type r_steps_to_home = 103;    // steps

/* CONSTANTS                                                                 */
constexpr int_type steps_per_rev = 200;     // step/rev
constexpr int_type m_steps_per_rev = 850;   // step/rev
constexpr double c_pulley_pitch = 0.195;    // in/step
constexpr int_type c_num_pulley_teeth = 12; // teeth/rev
constexpr double r_steps_per_deg = 1.1111;  // steps/deg

/* CALIBRATIONS FOR USER                                                     */
constexpr double deg_wrap_angle = 55.0; // defined on [30, 80] for 3 axis
constexpr double m_radius = 2.24806;    // in
// constexpr double m_radius = 2.96426;             // in - large mandrel
constexpr double filament_width = 0.25;          // in
constexpr double filament_percent_overlap = 0.0; // % in range [0, 1]
constexpr int_type total_layers = 2;             // 1 layer is 2 physical layers due to nature of winding
constexpr int min_dwell_steps = 50;
constexpr double wind_length = 11.0916;      // 1 layer is 2 physical layers due to nature of winding
constexpr int_type patterns_per_circuit = 2; // [1, 6]
constexpr int_type mandrel_steps = 600;

/* CALIBRATIONS FOR DEVELOPER                                                */
constexpr double c_max_velocity = 18.0; // in/s
// constexpr double m_max_rev_per_sec = 1.0; // rev/s
constexpr double m_max_rev_per_sec = 0.5;           // rev/s
constexpr double m_min_resonant_rev_per_sec = 0.21; // rev/s
constexpr double m_max_resonant_rev_per_sec = 0.3;  // rev/s

/* DERIVED CONSTANTS                                                         */
constexpr double wrap_angle = (PI * deg_wrap_angle) / 180;
constexpr int_type circuits_per_layer = 1 + ((TWO_PI * m_radius * cos(wrap_angle)) / filament_width);

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