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
    long_int_type usec_per_step = 1000000; // convert s to us
    usec_per_step *= TWO_PI;
    usec_per_step *= config::mandrel_radius;
    usec_per_step /= (tan_velocity);
    usec_per_step /= config::mandrel_steps_per_rev;

    set_usec_per_step(usec_per_step);
  }

  void inc_step_count() { ++step_count_; }
  void clear_step_count() { step_count_ = 0; }
  long_int_type get_step_count() { return step_count_; }

  void set_step_count_at_dir_flip() { step_count_at_dir_flip_ = step_count_; }
  long_int_type get_step_count_at_dir_flip() { return step_count_at_dir_flip_; }

private:
  long_int_type step_count_ = 0;
  long_int_type step_count_at_dir_flip_ = 0;
};

#endif