#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/*                CALIBRATIONS                */
const float wrap_angle = 45 * 3.14159 / 180; // degrees * pi / 180 = radians
const float carriage_velocity = 8;           // in/s
const float mandrel_radius = 2;              // inches
const int_type debounce_time = 50;           // ms

/*                CONSTANTS                */
/* The number of steps per revolution */
const int_type steps_per_rev = 200;

const float carriage_pulley_pitch = 0.2;
const int_type carriage_num_pulley_teeth = 12;

/*                I/O PINS                */
const int_type mandrel_step_pin = 8;
const int_type mandrel_dir_pin = 5; // dummy this for now, no dir pin connected
const int_type carriage_step_pin = 9;
const int_type carriage_dir_pin = 13;
const int_type c_home_switch_pin = 7;

const int_type switch_pin = 6;

} // namespace config

#endif