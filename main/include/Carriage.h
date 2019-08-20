#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Config.h"
#include "Motor.h"

class Carriage : public Motor
{
public:
  Carriage(const int_type &step_pin, const int_type &dir_pin) : Motor(step_pin, dir_pin) {}

  void set_velocity(const double &velocity)
  {
    set_usec_per_step(1000000 * config::c_pulley_pitch * config::c_num_pulley_teeth / (config::steps_per_rev * velocity));
  }

  void set_init_usec_per_step_accel(const double &accel_dist, const double &velocity)
  {
    init_usec_per_step_accel_ = 0.957 * 1000000 * sqrt(accel_dist * config::c_pulley_pitch * config::c_num_pulley_teeth / (100 * sq(velocity)));
  }

  bool is_accelerating()
  {
    return is_accelerating_;
  }
  bool is_decelerating() { return is_decelerating_; }
  void start_accelerating()
  {
    is_accelerating_ = true;
    accel_steps_ = 0;
    d_next_usec_per_step_ = init_usec_per_step_accel_;
    set_usec_per_step((long_int_type)d_next_usec_per_step_);
  }
  void start_decelerating() { is_decelerating_ = true; }
  void set_next_usec_per_step_accel()
  {
    ++accel_steps_;
    d_next_usec_per_step_ -= 2 * d_next_usec_per_step_ / (4 * accel_steps_ + 1);
    set_usec_per_step((long_int_type)d_next_usec_per_step_);
  }
  void check_accel_finished()
  {
    if (accel_steps_ == total_accel_steps_)
    {
      is_accelerating_ = false;
    }
  }
  void set_next_usec_per_step_decel()
  {
    --accel_steps_;
    d_next_usec_per_step_ += 2 * d_next_usec_per_step_ / (4 * accel_steps_ + 1);
    set_usec_per_step((long_int_type)d_next_usec_per_step_);
  }
  void check_decel_finished()
  {
    if (accel_steps_ == 0)
    {
      is_decelerating_ = false;
    }
  }
  void set_total_accel_steps(const double &accel_dist)
  {
    total_accel_steps_ = (double)accel_dist * config::steps_per_rev / (config::c_pulley_pitch * config::c_num_pulley_teeth);
    Serial.print("total_accel_steps_: ");
    Serial.print(total_accel_steps_);
    Serial.print("\n");
  }
  int_type get_total_accel_steps() { return total_accel_steps_; }

  void set_home_dir_flip_flag() { home_dir_flip_flag_ = true; }
  void clear_home_dir_flip_flag() { home_dir_flip_flag_ = false; }
  bool is_home_dir_flip_flag_set() const { return home_dir_flip_flag_; }

  void set_far_dir_flip_flag() { far_dir_flip_flag_ = true; }
  void clear_far_dir_flip_flag() { far_dir_flip_flag_ = false; }
  bool is_far_dir_flip_flag_set() const { return far_dir_flip_flag_; }

  void set_home_end_flag() { at_home_end_flag_ = true; }
  void clear_home_end_flag() { at_home_end_flag_ = false; }
  bool is_at_home_end() { return at_home_end_flag_; }

  void set_far_end_flag() { at_far_end_flag_ = true; }
  void clear_far_end_flag() { at_far_end_flag_ = false; }
  bool is_at_far_end() { return at_far_end_flag_; }

private:
  bool home_dir_flip_flag_ = false;
  bool far_dir_flip_flag_ = false;
  bool at_home_end_flag_ = false;
  bool at_far_end_flag_ = false;

  bool is_accelerating_ = false;
  bool is_decelerating_ = false;

  int_type accel_steps_ = 0;

  // Defined dynamically per pass
  int_type total_accel_steps_;
  double init_usec_per_step_accel_;
  double d_next_usec_per_step_;
};

#endif