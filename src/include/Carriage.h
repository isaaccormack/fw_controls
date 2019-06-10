#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Config.h"
#include "Motor.h"

class Carriage : public Motor
{
public:
  /* Implicitly construct motor with config variables */
  Carriage() : Motor(config::carriage_step_pin, config::carriage_dir_pin) {}

  /* Need to debug this equation as is not giving expected  */
  void set_velocity(const float &velocity)
  {
    // have checks for velocity == 0?
    long_int_type msec_per_step = (1000 * config::carriage_pulley_pitch * config::carriage_num_pulley_teeth) /
                                  (velocity * config::steps_per_rev);
    set_msec_per_step(msec_per_step);
  }

  // ... WIP ..., something like this
  // /* Linearly increase the speed of a motor */
  // void ramp_up_velocity(const float &init_vel = 0, const float &final_vel, const long_int_type &time)
  // {
  //   float delta_vel = final_vel - init_vel;
  //   set_velocity(delta_vel * (millis() - start_time) / time);
  // }
};

#endif