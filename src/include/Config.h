#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/*                CALIBRATIONS                */
// Precondition: 10 <= deg_wrap_angle <= 80
const double deg_wrap_angle = 75.0;                    // degrees
const double wrap_angle = deg_wrap_angle * PI / 180.0; // radians
// Use linearization fcn with gain of 120 to keep mandrel velocity relatively stable over different wrap angles
// Gain can vary between 100 (at slowest) and 150 according to best behaving mandrel speeds (wrt slip and torque)
const double carriage_velocity = 200.0 / deg_wrap_angle; // in / s
const double mandrel_radius = 2.24806;                   // inches
// Note that debounce_time of 1000 us works well for Corey's roller limit switch, but does not work for phidgets
// basic limit switch (too little time).
// Not sure if changing the debounce time and it resulting in effecting the accuracy of the mandrel steps per revolution
// is a defect of the switch class itself and the algorithm needs to be redone of if it is a concequence of the nature of the
// use of the method (very likely the first, look into better debounce algoirthms later)
const int_type debounce_time = 1000;               // us
const long_int_type dir_flip_timeout_dur = 300000; // us
const double filament_width = 0.125;               // in, take into account wrap angle later to calc effective filament width on mandrel
const double filament_overlap_percent = 0.2;       // 0 -> 1, percent of overlap between two adjacent strings of filament
const int_type total_layers = 1;

/*                CONSTANTS                */
/* The number of steps per revolution */
const int_type steps_per_rev = 200;
/* - Note that Mandrel motor has 4.25 : 1 gear ratio. The motor itself has 200 steps per
 * revolution -> 200 * 4.25 = 850 steps per revolution
 * - Also note the gear box has 1.5 degrees of blacklash error due to gearbox */
const int_type mandrel_steps_per_rev = 850;

const double carriage_pulley_pitch = 0.195;
const int_type carriage_num_pulley_teeth = 12;

/*                PRIVATE NAMESPACE FOR DERIVING CONSTANTS                */
namespace
{
const double eff_filament_width = filament_width * (1.0 - filament_overlap_percent);
const double calc_total_passes = 2.0 * total_layers * TWO_PI * config::mandrel_radius / eff_filament_width; // use double so no round off error
// steps = (in * (steps / rev) * rev / in) -- divide by 2 since delay by half on each side
// const double calc_filament_offset_delay_steps = eff_filament_width * mandrel_steps_per_rev / (2.0 * TWO_PI * mandrel_radius);
const double calc_filament_offset_delay_steps = mandrel_steps_per_rev / 2;
} // namespace

/*                DERIVED CONSTANTS                */
int_type total_passes = (int_type)calc_total_passes + 1;
// Need additional delay at end becase filament offset delay steps is so small
int_type filament_wait_steps = mandrel_steps_per_rev / 2;
// int_type filament_offset_delay_steps = (int_type)calc_filament_offset_delay_steps + filament_wait_steps;
int_type filament_offset_delay_steps = (int_type)calc_filament_offset_delay_steps; // delay half a rotation to test

/*                I/O PINS                */
const int_type mandrel_step_pin = 8;
const int_type mandrel_dir_pin = 12; // dummy this for now, no dir pin connected
const int_type m_encoder_switch_pin = 5;
const int_type carriage_step_pin = 9;
const int_type carriage_dir_pin = 13;
const int_type c_home_switch_pin = 7;
const int_type c_far_switch_pin = 6;

} // namespace config

#endif