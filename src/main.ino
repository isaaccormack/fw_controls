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

  Serial.print(Carriage.get_dir());
  Serial.print(" <- Carriage Direction\n");

  // Create array of references to class instances
  Motor *Motors[2] = {&Mandrel, &Carriage};

  while (1)
  {
    for (Motor *m : Motors)
    {
      unsigned long int curr_usec = micros();
      if ((curr_usec - m->get_last_step_time()) > m->get_usec_per_step())
      {
        if (m->dir_flip_flag_is_set() && (curr_usec - m->get_dir_flip_time() > m->dir_flip_timeout_length()))
        {
          if (curr_usec - m->get_dir_flip_time() < m->dir_flip_timeout_length())
          {
            continue;
          }
          Carriage.clear_dir_flip_flag();
        }
        m->set_last_step_time(curr_usec);
        m->step();
      }
    }

    if (C_home_switch.is_rising_edge())
    {
      // What I want to do:
      // Flip the motors direction, then the next time it goes to step (ie. next loop) delay the motor from stepping
      // for a timeout period
      Carriage.flip_dir();
      Carriage.set_dir_flip_flag();
      Carriage.set_dir_flip_time(micros());
      // Serial.print("In func\n");
      // Serial.print(Carriage.get_dir());
      // Serial.print(" <- Carriage Direction\n");
    }
    // unsigned long int curr_usec = micros();
    // if (curr_usec - debug_time > 1000000) // if 1s has passed
    // {
    //   debug_time = curr_usec;
    //   Serial.print(Carriage.get_step_counter());
    //   Serial.print(" <- num of steps done by carriage over 1s\n");
    //   Serial.print(Mandrel.get_step_counter());
    //   Serial.print(" <- num of steps done by mandrel over 1s\n");
    //   Carriage.clear_step_counter();
    //   Mandrel.clear_step_counter();
    // }
  }
  return 0;
}

void setup()
{
  main_();
}
