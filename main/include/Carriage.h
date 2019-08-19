#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Config.h"
#include "Motor.h"

class Carriage : public Motor
{
public:
  Carriage(const int_type &step_pin, const int_type &dir_pin) : Motor(step_pin, dir_pin) {}

  void set_velocity(const float &velocity) { set_usec_per_step(scaled_inch_per_step_ / velocity); }

  bool is_accelerating() { return is_accelerating_; }
  bool is_decelerating() { return is_decelerating_; }
  void start_accelerating()
  {
    is_accelerating_ = true;
    accel_steps_ = 0;
    d_next_usec_per_step = init_usec_per_step_;
    set_usec_per_step((long_int_type)d_next_usec_per_step);
  }
  void start_decelerating() { is_decelerating_ = true; }
  void set_next_usec_per_step_accel()
  {
    ++accel_steps_;
    d_next_usec_per_step -= 2 * d_next_usec_per_step / (4 * accel_steps_ + 1);
    set_usec_per_step((long_int_type)d_next_usec_per_step);
    // Serial.print(accel_steps_);
    // Serial.print(" ");
    // Serial.print(d_next_usec_per_step);
    // Serial.print(" <- from accel\n");
  }
  void check_accel_finished()
  {
    if (accel_steps_ == config::carriage_accel_steps)
    {
      is_accelerating_ = false;
    }
  }
  void set_next_usec_per_step_decel()
  {
    --accel_steps_;
    d_next_usec_per_step += 2 * d_next_usec_per_step / (4 * accel_steps_ + 1);
    set_usec_per_step((long_int_type)d_next_usec_per_step);
    // Serial.print(accel_steps_);
    // Serial.print(" ");
    // Serial.print(d_next_usec_per_step);
    // Serial.print(" <- from decel\n");
  }
  void check_decel_finished()
  {
    if (accel_steps_ == 0)
    {
      is_decelerating_ = false;
    }
  }

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

  // for testing
  double get_init_usec_per_step() { return init_usec_per_step_; }
  void set_init_usec_per_step()
  {
    init_usec_per_step_ = (double)0.957 * 1000000 * sqrt(config::carriage_accel_dist * config::carriage_pulley_pitch * config::carriage_num_pulley_teeth / (100.0 * sq(config::carriage_velocity)));
  }

private:
  bool home_dir_flip_flag_ = false;
  bool far_dir_flip_flag_ = false;
  bool at_home_end_flag_ = false;
  bool at_far_end_flag_ = false;

  bool is_accelerating_ = false;
  bool is_decelerating_ = false;
  bool at_end_flag_ = false;

  int_type accel_steps_ = 0;

  // keep track of usec per step with floating point precision to improve accuracy of accel / decel with little overhead
  double d_next_usec_per_step = init_usec_per_step_;

  constexpr static long_int_type scaled_inch_per_step_ = (double)(1000000 * config::carriage_pulley_pitch * config::carriage_num_pulley_teeth) / config::steps_per_rev;

  // Use manual calibration procedure to calibrate Cerr as automatic algorithm very complex for compile time
  // Or could, use auxilary program in UI to calculate this
  // To calibrate, calculate the number of steps necessary to reach distance, take the floor of this, change Cerr until expected value occurs at exp num steps
  double init_usec_per_step_;
};

const config::long_int_type Carriage::scaled_inch_per_step_;
// const double Carriage::init_usec_per_step_;

#endif