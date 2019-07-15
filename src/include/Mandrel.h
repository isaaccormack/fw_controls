#ifndef MANDREL_H
#define MANDREL_H

#include "Config.h"
#include "Motor.h"

class Mandrel : public Motor
{
public:
  Mandrel() : Motor(config::mandrel_step_pin, 52) {} // No mandrel direction control => direction pin = 52

  void set_velocity(const double &tan_velocity)
  {
    long_int_type usec_per_step = 1000000; // convert s -> us
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

  void set_step_count_at_far_dir_flip() { step_count_at_far_dir_flip_ = step_count_; }
  int_type get_step_count_at_far_dir_flip() { return step_count_at_far_dir_flip_; }

  void set_step_count_at_start_of_pass(const int_type &step_count) { step_count_at_start_of_pass_ = step_count; }
  void add_to_step_count_at_start_of_pass(const int_type &additional_steps) { step_count_at_start_of_pass_ += additional_steps; }
  int_type get_step_count_at_start_of_pass() const { return step_count_at_start_of_pass_; }

  void add_to_far_end_wait_steps(const int_type &additional_steps) { far_end_wait_steps_ += additional_steps; }
  int_type get_far_end_wait_steps() const { return far_end_wait_steps_; }

private:
  int_type step_count_ = 0;
  int_type backup_step_count_ = 0;
  int_type step_count_at_far_dir_flip_ = 0;

  int_type step_count_at_start_of_pass_ = 425;               // DO NOT TOUCH - configuration algorithm assumes initial value
  int_type far_end_wait_steps_ = 3 * config::deg_wrap_angle; // Mandrel speed increases with wrap angle => increase wait steps
};

#endif