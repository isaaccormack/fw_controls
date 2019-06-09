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
    int_type step_freq = (velocity * config::steps_per_rev) /
                         (config::carriage_pulley_pitch * config::carriage_num_pulley_teeth);
    set_step_freq(step_freq);
  }
};

#endif