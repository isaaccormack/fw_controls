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
    // have checks for tan_velocity == 0?
    /* ms/step = 1000 * 2 * pi * Mr / (Vm,tan * steps per rev) */
    long_int_type msec_per_step = (1000 * 6.2831853 * config::mandrel_radius) /
                                  (tan_velocity * config::steps_per_rev);
    set_msec_per_step(msec_per_step);
  }
};

#endif