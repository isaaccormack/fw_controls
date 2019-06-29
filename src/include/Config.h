#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/*                CALIBRATIONS                */
const float wrap_angle = 45 * 3.14159 / 180; // degrees * pi / 180 = radians
const float carriage_velocity = 5;           // in/s
const float mandrel_radius = 2;              // inches
const int_type debounce_time = 50;           // ms

/*                CONSTANTS                */
/* The number of steps per revolution */
const int_type steps_per_rev = 200;
/* - Note that Mandrel motor has 4.25 : 1 gear ratio. The motor itself has 200 steps per
 * revolution -> 200 * 4.25 = 850 steps per revolution
 * - Also note the gear box has 1.5 degrees of blacklash error due to gearbox */
const int_type mandrel_steps_per_rev = 860; // bump up to 860 since usually only 850 steps done @ high speed

const float carriage_pulley_pitch = 0.195f;
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