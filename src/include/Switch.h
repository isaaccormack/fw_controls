#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"
#include "Motor.h"

class Switch
{
public:
  typedef config::int_type int_type;
  typedef config::long_int_type long_int_type;

  Switch(const int_type &pin,
         const long_int_type &debounce_time = config::debounce_time) : pin_(pin),
                                                                       debounce_time_(debounce_time)
  {
    pinMode(pin_, INPUT);
  }

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