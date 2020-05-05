#ifndef ROTATOR_H
#define ROTATOR_H

#include "Motor.h"

class Rotator : public Motor
{
public:
  Rotator() : Motor(config::r_step_pin, config::r_dir_pin) {}

  void set_rev_per_sec(const double &rev_per_sec)
  {
    // Divide by 2 since 2:1 gear ratio
    set_usec_per_step(1000000 / (2 * config::steps_per_rev * rev_per_sec));
  }

  void set_rev_per_sec(const double &c_accel_dist, const double &c_velocity)
  {
    set_rev_per_sec((c_velocity * (90.0 - config::deg_wrap_angle)) / (2.0 * c_accel_dist * 360));
  }

  void check_rotation_finished()
  {
    if (step_count_ > steps_for_wrap_angle_)
    {
      step_count_ = 0;
      enabled = false;
    }
  }

  void inc_step_count() { ++step_count_; }
  int_type get_step_count() { return step_count_; }
  void clear_step_count() { step_count_ = 0; }

  bool enable() { enabled = true; }
  bool is_enabled() { return enabled; }

private:
  int_type step_count_ = 0;
  bool enabled = false;

  constexpr static int_type steps_for_wrap_angle_ = (double)(90.0 - config::deg_wrap_angle) * config::r_steps_per_deg;
};

constexpr config::int_type Rotator::steps_for_wrap_angle_;

#endif