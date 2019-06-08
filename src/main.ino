#include "include/Mandrel.h"

/* Black Green Blue Red */
/* 1ms works well for fast setting */

int main_()
{
  Mandrel b;
  Switch s;
  // b.set_step_freq_from_ang_vel(10);
  b.set_step_freq(300);
  while (1)
  {
    unsigned long i = micros();
    if ((i - b.get_last_step_time()) > b.get_step_freq())
    {
      b.step();
      b.set_last_step_time(i);
      Serial.print(i);
      Serial.print("\n");
    }
    if (s.is_pressed_debounced())
    {
      b.flip_dir();
    }
  }
  return 0;
}

void setup()
{
  Serial.begin(115200);
  main_();
}
