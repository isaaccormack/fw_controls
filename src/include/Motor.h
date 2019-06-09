#ifndef MOTOR_H
#define MOTOR_H

#include "Config.h"

class Motor
{
public:
  typedef unsigned int int_type;
  typedef unsigned long int long_int_type;

  /* Dummy default constructor - all values are overwritten by derived class */
  Motor() : step_pin_(0), dir_pin_(0), pulley_pitch_(0), num_pulley_teeth_(0) {}

  void set_step_freq_from_ang_vel(const int_type &ang_vel)
  {
    step_freq_ = ang_vel * config::steps_per_rev /
                 (pulley_pitch_ * num_pulley_teeth_);
  }

  /* For testing, should be private */
  void set_step_freq(long_int_type step_freq) { step_freq_ = step_freq; }

  long_int_type get_step_freq() const { return step_freq_; }

  void flip_dir() { digitalWrite(dir_pin_, dir_ = !dir_); }

  void step() { digitalWrite(step_pin_, step_ind_ = !step_ind_); }

  void set_last_step_time(const unsigned long &last_step_time) { last_step_time_ = last_step_time; }

  long_int_type get_last_step_time() const { return last_step_time_; }

protected:
  /* Constructor invoked by derived class during its construction */
  Motor(const int_type &step_pin,
        const int_type &dir_pin,
        const float &pulley_pitch,
        const int_type &num_pulley_teeth) : step_pin_(step_pin),
                                            dir_pin_(dir_pin),
                                            pulley_pitch_(pulley_pitch),
                                            num_pulley_teeth_(num_pulley_teeth)
  {
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
  }

private:
  /* Motor specifiic variables which are defined during construction */
  const int_type step_pin_;
  const int_type dir_pin_;

  const float pulley_pitch_;
  const int_type num_pulley_teeth_;

  /* Default values for all motors */
  int_type step_ind_ = LOW;
  int_type dir_ = LOW;
  long_int_type step_freq_ = 0;
  long_int_type last_step_time_ = 0;
};

#endif