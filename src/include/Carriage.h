#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Config.h"
#include "Motor.h"

class Carriage : public Motor
{
public:
  /* Implicitly construct motor with config variables */
  Carriage() : Motor(config::carriage_step_pin, config::carriage_dir_pin) {}

  void set_velocity(const float &velocity)
  {
    long_int_type usec_per_step = 1000000;
    usec_per_step *= config::carriage_pulley_pitch;
    usec_per_step *= config::carriage_num_pulley_teeth;
    usec_per_step /= velocity;
    usec_per_step /= config::steps_per_rev;

    set_usec_per_step(usec_per_step);
  }
};

#endif