#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Config.h"
#include "Motor.h"

class Carriage : public Motor
{
public:
  Carriage() : Motor(config::carriage_step_pin, config::carriage_dir_pin) {}

  void set_velocity(const float &velocity)
  {
    long_int_type usec_per_step = 1000000; // Convert s -> us
    usec_per_step *= config::carriage_pulley_pitch;
    usec_per_step *= config::carriage_num_pulley_teeth;
    usec_per_step /= velocity;
    usec_per_step /= config::steps_per_rev;

    set_usec_per_step(usec_per_step);
  }

  void set_home_dir_flip_flag() { home_dir_flip_flag_ = HIGH; }
  void clear_home_dir_flip_flag() { home_dir_flip_flag_ = LOW; }
  int_type is_home_dir_flip_flag_set() const { return home_dir_flip_flag_; }

  void set_far_dir_flip_flag() { far_dir_flip_flag_ = HIGH; }
  void clear_far_dir_flip_flag() { far_dir_flip_flag_ = LOW; }
  int_type is_far_dir_flip_flag_set() const { return far_dir_flip_flag_; }

private:
  int_type home_dir_flip_flag_ = LOW;
  int_type far_dir_flip_flag_ = LOW;
};

#endif