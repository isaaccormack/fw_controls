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

  // void set_filament_offset_delay_time(const double &tan_velocity)
  // {
  //   long_int_type filament_offset_delay_time_ = 1000000; // convert s to us
  //   filament_offset_delay_time_ *= config::filament_width;
  //   filament_offset_delay_time_ *= (1.0 - config::filament_overlap_percent);
  //   filament_offset_delay_time_ /= tan_velocity;
  //   filament_offset_delay_time_ /= 2.0; // divide by 2 because delay occurs on both sides
  // }

  // long_int_type get_filament_offset_delay_time()
  // {
  //   return filament_offset_delay_time_;
  // }

  // void set_rev_start_time(const long_int_type &rev_start_time) { rev_start_time_ = rev_start_time; }
  // long_int_type get_rev_start_time() { return rev_start_time_; }

  void inc_step_count() { ++step_count_; }
  void clear_step_count() { step_count_ = 0; }
  void get_step_count() { return step_count_; }

private:
  long_int_type filament_offset_delay_time_ = 0; // init as 0 for now...
  long_int_type rev_start_time_ = 0;             // init as 0 for now...
  long_int_type step_count_ = 0;                 // init as 0 for now...
};

#endif