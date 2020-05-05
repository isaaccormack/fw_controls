#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Motor.h"

class Carriage : public Motor
{
public:
  Carriage(const int_type &step_pin, const int_type &dir_pin) : Motor(step_pin, dir_pin) {}

  void set_velocity(const double &velocity)
  {
    set_usec_per_step((1000000 * config::c_pulley_pitch * config::c_num_pulley_teeth) / (config::steps_per_rev * velocity));
  }

  void set_init_usec_per_step_accel(const double &accel_dist, const double &velocity)
  {
    init_usec_per_step_accel_ = 0.957 * 1000000 * sqrt((accel_dist * config::c_pulley_pitch * config::c_num_pulley_teeth) / (100 * sq(velocity)));
  }

  bool is_accelerating() const { return is_accelerating_; }
  bool is_decelerating() const { return is_decelerating_; }
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
      is_accelerating_ = false;
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
      is_decelerating_ = false;
  }
  void set_total_accel_steps(const int_type &accel_steps) { total_accel_steps_ = accel_steps; }
  int_type get_total_accel_steps() const { return total_accel_steps_; }

  void set_decel_flag() { decel_flag_ = true; }
  void clear_decel_flag() { decel_flag_ = false; }
  bool is_decel_flag_set() const { return decel_flag_; }

  void set_dwell_flag() { dwell_flag_ = true; }
  void clear_dwell_flag() { dwell_flag_ = false; }
  bool is_dwell_flag_set() const { return dwell_flag_; }

private:
  bool decel_flag_ = false;
  bool dwell_flag_ = false;

  bool is_accelerating_ = false;
  bool is_decelerating_ = false;

  int_type accel_steps_ = 0;

  // Defined dynamically per pass
  int_type total_accel_steps_;
  double init_usec_per_step_accel_;
  double d_next_usec_per_step_;
};

#endif