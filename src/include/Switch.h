#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"

/* HIGH = 1
 * LOW = 0 */
/* Make each instantiation of switch serve a different purpose but
 * have there only exist one switch class (ie. Switch mandrel_home;) */

/* Switch must be wired normally open */
class Switch
{
public:
  typedef unsigned int int_type;
  typedef unsigned long int long_int_type;

  Switch(int_type pin) : pin_(pin) { pinMode(pin_, INPUT); }

  /* Method assumes the button has been pulled down */
  bool is_rising_edge()
  {
    /* Debounce by polling every debounce_time_ (ie. 50ms) */
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
  const int_type pin_; // must initialize pin
  const long_int_type debounce_time_ = config::debounce_time;

  long_int_type lastMillis_ = 0;
  int_type last_val_ = LOW;
};

#endif