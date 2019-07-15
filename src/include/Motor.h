#ifndef MOTOR_H
#define MOTOR_H

#include "Config.h"

class Motor
{
public:
  typedef config::int_type int_type;
  typedef config::long_int_type long_int_type;

  /* Dummy default constructor - all values are overwritten by derived class */
  Motor() : step_pin_(0), dir_pin_(0) {}

  void step()
  {
    digitalWrite(step_pin_, step_ind_ = !step_ind_);
    digitalWrite(step_pin_, step_ind_ = !step_ind_);
  }

  void set_last_step_time(const long_int_type &last_step_time) { last_step_time_ = last_step_time; }
  long_int_type get_last_step_time() const { return last_step_time_; }

  long_int_type get_usec_per_step() const { return usec_per_step_; }

  void flip_dir() { digitalWrite(dir_pin_, dir_ = !dir_); }

protected:
  void set_usec_per_step(const long_int_type &usec_per_step) { usec_per_step_ = usec_per_step; }

  /* Constructor invoked by derived class during its construction */
  Motor(const int_type &step_pin,
        const int_type &dir_pin) : step_pin_(step_pin),
                                   dir_pin_(dir_pin)
  {
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    digitalWrite(dir_pin, dir_);
  }

private:
  /* Motor specifiic variables defined during construction */
  const int_type step_pin_;
  const int_type dir_pin_;

  /* Default values for all motors */
  int_type step_ind_ = LOW;
  int_type dir_ = HIGH;
  long_int_type usec_per_step_ = 0;
  long_int_type last_step_time_ = 0;
};

#endif