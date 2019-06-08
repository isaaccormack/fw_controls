#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
typedef unsigned int int_type;
typedef unsigned long int long_int_type;

/*                CONSTANTS                */
/* The number of steps per revolution */
const int_type steps_per_rev = 200;
const int_type debounce_time = 50; // ms

/*                I/O PINS                */
const int_type mandrel_step_pin = 13;
const int_type mandrel_dir_pin = 12;
const int_type switch_pin = 11;

const int_type test_pin = 8;

} // namespace config

#endif