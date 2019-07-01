#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

int main_()
{
  Serial.begin(115200);

  Carriage Carriage;
  Carriage.set_velocity(config::carriage_velocity);

  /* Vm / Vc = tan(wrap_angle)   =>   Vm,tan = Vc * tan(wrap angle) */
  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);

  Mandrel Mandrel;
  Mandrel.set_velocity(mandrel_velocity);

  Switch C_Home_Switch(config::c_home_switch_pin, Carriage);
  Switch C_Far_Switch(config::c_far_switch_pin, Carriage);

  Motor *Motors[2] = {&Mandrel, &Carriage};
  Switch *Dir_Switches[2] = {&C_Home_Switch, &C_Far_Switch};

  // Carriage.home();

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

    for (Switch *s : Dir_Switches)
    {
      if (s->is_rising_edge() && !s->motor.is_dir_flip_flag_set())
      {
        s->motor.flip_dir();
        s->motor.set_dir_flip_flag();
        s->motor.set_last_dir_flip_time(micros());
      }
    }
  }
  return 0;
}

void setup()
{
  main_();
}
