#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

int main_()
{
  Carriage Carriage;
  // Carriage.set_step_freq(100); // for test
  Carriage.set_velocity(config::carriage_velocity); // in/s

  /* Vm,tan = Vc / tan(wrap angle) */
  float mandrel_velocity = config::carriage_velocity / tan(config::wrap_angle);

  Mandrel Mandrel;
  // Mandrel.set_step_freq(1000); // for test
  Mandrel.set_velocity(mandrel_velocity);

  // Switch C_home_switch(config::c_home_switch_pin);
  // Switch C_end_switch(config::c_end_switch_pin);

  // Carriage.home();

  // Create array of references to class instances
  Motor *Motors[2] = {&Mandrel, &Carriage};

  while (1)
  {
    for (Motor *m : Motors)
    {
      if ((millis() - m->get_last_step_time()) > m->get_step_freq())
      {
        m->step();
        m->set_last_step_time(millis());
      }
    }
  }
  return 0;
}

void setup()
{
  // Serial.begin(115200);
  main_();
}
