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
  int_type get_step_count() const { return step_count_; }
  void sync_step_count_with_backup() { step_count_ = backup_step_count_; }

  void inc_backup_step_count() { ++backup_step_count_; }
  void clear_backup_step_count() { backup_step_count_ = 0; }

  void set_step_count_at_dir_flip() { step_count_at_dir_flip_ = step_count_; }
  long_int_type get_step_count_at_dir_flip() { return step_count_at_dir_flip_; }

  void set_step_count_at_wind_start(const int_type &step_count_at_wind_start) { step_count_at_wind_start_ = step_count_at_wind_start; }
  void add_to_step_count_at_wind_start(const int_type &additional_steps) { step_count_at_wind_start_ += additional_steps; }
  int_type get_step_count_at_wind_start() const { return step_count_at_wind_start_; }

  void add_to_far_end_wait_steps(const int_type &additional_steps) { far_end_wait_steps_ += additional_steps; }
  int_type get_far_end_wait_steps() const { return far_end_wait_steps_; }

private:
  int_type step_count_ = 0;
  int_type backup_step_count_ = 0;
  long_int_type step_count_at_dir_flip_ = 0;

  int_type step_count_at_wind_start_ = 425; // DO NOT TOUCH - configuration algorithm assumes this value
  // Linearly increase wait time with wrap angle since mandrel speed increases with wrap angle.
  int_type far_end_wait_steps_ = 3 * config::deg_wrap_angle;
};

#endif