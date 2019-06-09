#ifndef MANDREL_H
#define MANDREL_H

#include "Config.h"

class Mandrel : public Motor
{
public:
  typedef unsigned int int_type;
  typedef unsigned long int long_int_type;

  Mandrel()
  {
    pinMode(step_pin_, OUTPUT);
    pinMode(dir_pin_, OUTPUT);
  }

  /* @param dir The desired direction of mandrel rotation
   *            true == cw and false == ccw */
  void flip_dir()
  {
    dir_ = !dir_;
    digitalWrite(dir_pin_, dir_ ? HIGH : LOW);
  }

  /* @param ang_vel The desired angular velocity of the mandrel in rad / s */
  // this could also be an init func b/c it doesnt actually start the motor stepping??
  void run(int_type ang_vel) { set_step_freq_from_ang_vel(ang_vel); }

  // debugging function
  void set_step_freq(long_int_type step_freq) { step_freq_ = step_freq; }

  /* @return step_freq_ Current step frequency for mandrel */
  long_int_type get_step_freq() const { return step_freq_; }

  /* @param step_ind Flip boolean indication of whether the step pin should be outputting HIGH or LOW.
   *                 true == HIGH and false == LOW */
  void step()
  {
    step_ind_ = !step_ind_;
    digitalWrite(step_pin_, step_ind_ ? HIGH : LOW);
  }

  /* @param last_step_time A value returned by millis() indicating when motor was last stepped */
  void set_last_step_time(const unsigned long &last_step_time) { last_step_time_ = last_step_time; }

  /* @return last_step_time_ The value returned by millis() when motor last stepped */
  long_int_type get_last_step_time() const { return last_step_time_; }

  // look into how to multiply float and int properly and decide on type for freq_ (probably integral type)
  void set_step_freq_from_ang_vel(const int_type &ang_vel)
  {
    step_freq_ = ang_vel * config::steps_per_rev /
                 (pulley_pitch_ * num_pulley_teeth_);
  }

private:
  const int_type step_pin_ = config::mandrel_step_pin;
  const int_type dir_pin_ = config::mandrel_dir_pin;

  const float pulley_pitch_ = 0.2;
  const int_type num_pulley_teeth_ = 20;

  bool step_ind_ = true;
  bool dir_ = true;
  long_int_type step_freq_ = 0;
  long_int_type last_step_time_ = 0;
};

#endif