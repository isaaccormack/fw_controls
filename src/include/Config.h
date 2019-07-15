#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/* HOMING CALIBRATIONS                                                       */
const double carriage_homing_velocity = 2; // in/s
const double mandrel_homing_velocity = 5;  // in/s

/* PUBLIC CALIBRATIONS - FOR USER                                            */
const double deg_wrap_angle = 25;            // [10, 80]
const double mandrel_radius = 2.24806;       // in
const double filament_width = 0.125;         // in
const double filament_percent_overlap = 0.2; // [0, 1]
const int_type total_layers = 1;

/* PRIVATE CALIBRATIONS - FOR DEVELOPER                                      */
const double wrap_angle = deg_wrap_angle * PI / 180.0; // rad
/* Define inverse relation such that mandrel velocity remains relatively
   stable over different wrap angles. Gain is typically in the range
   [100, 150] where 150 is fastest. */
const double carriage_velocity = 120.0 / deg_wrap_angle; // in / s

/* CONSTANTS                                                                 */
const int_type steps_per_rev = 200;
const int_type mandrel_steps_per_rev = 850;
const double carriage_pulley_pitch = 0.195;
const int_type carriage_num_pulley_teeth = 12;

/* PRIVATE NAMESPACE FOR DERIVING CONSTANTS                                  */
namespace
{
/* Constants which must be derived in floating point then typecasted to
   integral type to combat round off error are created here. */
const double eff_filament_width = filament_width * (1.0 - filament_percent_overlap);
const double calc_total_passes = 2.0 * total_layers * TWO_PI * mandrel_radius /
                                 eff_filament_width;
} // namespace

/* DERIVED CONSTANTS                                                         */
int_type total_passes = (int_type)calc_total_passes + 1;

/* I/O PINS                                                                  */
const int_type c_far_switch_pin = 13;
const int_type c_home_switch_pin = 12;
const int_type m_encoder_switch_pin = 11;
const int_type carriage_step_pin = 7;
const int_type carriage_dir_pin = 6;
const int_type mandrel_step_pin = 5;
} // namespace config

#endif