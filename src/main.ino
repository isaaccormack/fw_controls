#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

int main_()
{
  Serial.begin(115200);

  // Determine limits / relationships between wrap angles and carriage velocities (implicitly defining mandrel velocities)

  Carriage Carriage;
  Carriage.set_velocity(config::carriage_velocity);

  /* From Vm / Vc = tan(wrap_angle)
   * then Vm,tan = Vc * tan(wrap angle) */
  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);

  Mandrel Mandrel;
  Mandrel.set_velocity(mandrel_velocity);

  Switch C_home_switch(config::c_home_switch_pin);

  // Could declare swtich like:
  // Switch C_home_switch(config::c_home_switch_pin, &Carriage);
  // such that the switch holds a reference to motor and can envoke functions on its cbehalf

  // Switch C_end_switch(config::c_end_switch_pin);

  // Carriage.home();

  // Serial.print(Carriage.get_dir());
  // Serial.print(" <- Carriage Direction\n");

  // Create array of references to class instances
  Motor *Motors[2] = {&Mandrel, &Carriage};

  while (1)
  {
    for (Motor *m : Motors)
    {
      unsigned long int curr_usec = micros();
      if ((curr_usec - m->get_last_step_time()) > m->get_usec_per_step())
      {
        if (m->is_dir_flip_flag_set())
        {
          if (curr_usec - m->get_last_dir_flip_time() < config::dir_flip_timeout_dur)
          {
            continue;
          }
          Carriage.clear_dir_flip_flag();
        }
        m->set_last_step_time(curr_usec);
        m->step();
      }
    }

    // Second condition makes it such that the direction cant be flipped when machine is timed out
    if (C_home_switch.is_rising_edge() && !Carriage.is_dir_flip_flag_set())
    {
      Carriage.flip_dir();
      Carriage.set_dir_flip_flag();
      Carriage.set_last_dir_flip_time(micros());
    }
  }
  return 0;
}

void setup()
{
  main_();
}
