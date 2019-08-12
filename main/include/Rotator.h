#ifndef ROTATOR_H
#define ROTATOR_H

#include "Config.h"
#include "Motor.h"

class Rotator : public Motor
{
public:
  Rotator() : Motor(config::rotator_step_pin, config::rotator_dir_pin) {}

  void set_rev_per_sec(const float &rev_per_sec)
  {
    set_usec_per_step(scaled_step_per_rev_ / rev_per_sec);
  }

  void inc_step_count() { ++step_count_; }
  void clear_step_count() { step_count_ = 0; }
  int_type get_step_count() const { return step_count_; }

  bool enable() { enabled = true; }
  void disable() { enabled = false; }
  bool is_enabled() { return enabled; }

private:
  int_type step_count_ = 0;
  bool enabled = false;

  // Divide by 2 since 2:1 gear ratio
  constexpr static long_int_type scaled_step_per_rev_ = (1000000 / config::steps_per_rev) / 2;
};

#endif