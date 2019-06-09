#ifndef MANDREL_H
#define MANDREL_H

#include "Config.h"
#include "Motor.h"

class Mandrel : public Motor
{
public:
  /* Implicitly construct motor with config variables */
  Mandrel() : Motor(config::mandrel_step_pin, config::mandrel_dir_pin) {}

  void set_velocity(const float &tan_velocity)
  {
    /* f = (Vc * steps per rev) / c where c = 2*pi*r */
    int_type step_freq = (tan_velocity * config::steps_per_rev) /
                         (6.2831853 * config::mandrel_radius);
    set_step_freq(step_freq);
  }
};

#endif