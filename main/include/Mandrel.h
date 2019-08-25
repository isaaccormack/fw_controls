#ifndef MANDREL_H
#define MANDREL_H

#include "Config.h"
#include "Motor.h"

class Mandrel : public Motor
{
public:
  Mandrel() : Motor(config::m_step_pin, 52) {} // No mandrel direction control => direction pin = 52

  void set_velocity(const double &tan_velocity)
  {
    set_usec_per_step((1000000 * TWO_PI * config::m_radius) / (config::m_steps_per_rev * tan_velocity));
  }

  void inc_step_count() { ++step_count_; }
  void clear_step_count() { step_count_ = 0; }
  int_type get_step_count() const { return step_count_; }

  void set_dwell_steps(const int_type &dwell_steps) { dwell_steps_ = dwell_steps; }
  int_type get_dwell_steps() const { return dwell_steps_; }

private:
  int_type step_count_ = 0;
  int_type dwell_steps_;

  int_type step_count_at_pattern_start[config::patterns_per_circuit];
};

#endif