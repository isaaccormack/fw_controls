#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"
#include "Motor.h"

/* Precondition: Switch is wired normally open

   Switches are physically debounced with a simple RC circuit. The schematic for this circuit
   can be seen in /docs/debounced_pulldown_switch_schematic.png. The values in the diagram are
   configured for a breadboard layout. */
class Switch
{
public:
  typedef config::int_type int_type;

  Switch(const int_type &pin, const int_type &last_val = LOW) : pin_(pin), last_val_(last_val)
  {
    pinMode(pin_, INPUT);
  }

  bool is_rising_edge()
  {
    int_type this_val = digitalRead(pin_);
    if (this_val == HIGH && last_val_ == LOW)
    {
      last_val_ = this_val;
      return true;
    }
    last_val_ = this_val;
    return false;
  }

private:
  const int_type pin_;
  int_type last_val_;
};

#endif