#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"
#include "Motor.h"

class Switch
{
public:
  typedef unsigned int int_type;
  typedef unsigned long int long_int_type;

  Switch(const int_type &pin,
         Motor &m,
         const long_int_type &debounce_time = config::debounce_time) : pin_(pin),
                                                                       motor(m),
                                                                       debounce_time_(debounce_time)
  {
    pinMode(pin_, INPUT);
  }

  Motor &motor; // store reference to motor

  /* Method assumes the button is wired normally open */
  bool is_rising_edge()
  {
    /* Debounce by polling at least every debounce_time_ (ie. 50ms) */
    if ((millis() - lastMillis_) > debounce_time_)
    {
      lastMillis_ = millis();

      if (digitalRead(pin_) == HIGH)
      {
        if (last_val_ == LOW)
        {
          last_val_ = HIGH;
          return true;
        }
      }
      else
      {
        last_val_ = LOW;
      }
    }
    return false;
  }

private:
  const int_type pin_;
  const long_int_type debounce_time_;

  long_int_type lastMillis_ = 0;
  int_type last_val_ = LOW;
};

#endif