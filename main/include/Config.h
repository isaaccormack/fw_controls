#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/* HOMING CALIBRATIONS                                                       */
constexpr double carriage_homing_velocity = 2; // in/s
constexpr double mandrel_homing_velocity = 5;  // in/s

/* CALIBRATIONS FOR USER                                                     */
/* The wrap angle is not defined between (53, 60) due to resonance in the mandrel
   stepper motor. To wind at angles on this value, the equation driving 
   carriage_velocity must be recalibrated as to change the speed of mandrel. */
constexpr double deg_wrap_angle = 70;            // Defined between [20, 53] and [60, 80]
constexpr double mandrel_radius = 2.24806;       // in
constexpr double filament_width = 1;             // in - Perpendical distance between angled filament strands
constexpr double filament_percent_overlap = 0.0; // [0, 1]
constexpr int_type total_layers = 4;             // 1 layer is 2 physical layers due to nature of winding

/* CALIBRATIONS FOR DEVELOPER                                                */
/* Define inverse relation such that mandrel velocity remains relatively
   stable over different wrap angles. Gain, given by numerator of expression
   ranges from 98 for 20 deg_wrap_angle to 152 for 80 deg_wrap_angle */
constexpr double carriage_velocity = (80 + (9 * deg_wrap_angle / 10)) / deg_wrap_angle; // in/s
constexpr double wrap_angle = deg_wrap_angle * PI / 180.0;                              // rad

/* CONSTANTS                                                                 */
constexpr int_type steps_per_rev = 200;            // step/rev
constexpr int_type mandrel_steps_per_rev = 850;    // step/rev
constexpr double carriage_pulley_pitch = 0.195;    // in/step
constexpr int_type carriage_num_pulley_teeth = 12; // teeth/rev

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