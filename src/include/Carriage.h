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
    set_usec_per_step(scaled_inch_per_step_ / velocity);
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

  constexpr static long_int_type scaled_inch_per_step_ = (1000000 * config::carriage_pulley_pitch * config::carriage_num_pulley_teeth) /
                                                         config::steps_per_rev;
};

#endif