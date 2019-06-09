#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

/* Black Green Blue Red */
/* 1ms works well for fast setting */

int main_()
{
  // Mandrel Mandrel;
  // Mandrel.set_step_freq(800);

  Carriage Carriage;
  Carriage.set_step_freq(100);

  // Switch C_home_switch(config::c_home_switch_pin);
  // Switch C_end_switch(config::c_end_switch_pin);

  // Carriage.home();

  while (1)
  {
    if ((millis() - Carriage.get_last_step_time()) > Carriage.get_step_freq())
    {
      Carriage.step();
      Carriage.set_last_step_time(millis());
    }
    // if ((micros() - Mandrel.get_last_step_time()) > Mandrel.get_step_freq())
    // {
    //   Mandrel.step();
    //   Mandrel.set_last_step_time(micros());
    //   // Serial.print(i);
    //   // Serial.print("\n");
    // }
    // if (c_home_switch.is_rising_edge())
    // {
    //   Mandrel.flip_dir();
    // }
  }
  return 0;
}

void setup()
{
  // Serial.begin(115200);
  main_();
}
