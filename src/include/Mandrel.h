#ifndef MANDREL_H
#define MANDREL_H

#include "Config.h"
#include "Motor.h"

class Mandrel : public Motor
{
public:
  /* Implicitly construct motor with config variables */
  Mandrel() : Motor(config::mandrel_step_pin, config::mandrel_dir_pin) {}

  void set_velocity(const double &tan_velocity)
  {
    /* T = r / v * step_per_rev,
     * => us/step = 1000 * 1000 * TWO_PI * Mr / (Vm,tan * mandrel steps per rev) */
    long_int_type usec_per_step = 1000000;
    usec_per_step *= TWO_PI;
    usec_per_step *= config::mandrel_radius;
    usec_per_step /= (tan_velocity);
    usec_per_step /= config::mandrel_steps_per_rev;

    set_usec_per_step(usec_per_step);
  }
};

#endif