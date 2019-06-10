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

  Switch C_home_switch(config::c_home_switch_pin);

  // Could declare swtich like:
  // Switch C_home_switch(config::c_home_switch_pin, &Carriage);
  // such that the switch holds a reference to motor and can envoke functions on its behalf

  // Switch C_end_switch(config::c_end_switch_pin);

  // Carriage.home();

  // Create array of references to class instances
  Motor *Motors[2] = {&Mandrel, &Carriage};

  while (1)
  {
    for (Motor *m : Motors)
    {
      // // check which state motor is in and handle that accordingly
      // if (m.get_state() == RAMP_UP) // use enumeration to define state??
      // {
      // }
      // else if (m.get_state() == RAMP_DOWN)
      // {
      // }
      // else // it must be travelling linearly so just step it regularly
      // {
      // }
      /* This might need to account for 2 steps per freq */
      if ((millis() - m->get_last_step_time()) > m->get_msec_per_step())
      {
        m->step();
        m->set_last_step_time(millis());
      }
    }
    if (C_home_switch.is_rising_edge())
    {
      // what you actually want to call is some kind of turn around functions
      // this can ALL be generic such that when

      // turn around function can just linearly increase the msec per step until it is rougly 0 (ie. msec_per_step += dt)
      // every <define what time period here this would happen at> then change direction, then ramp back up

      // ie. if delta time (constant) has passed that is required between incrememntally changing msec per step
      // then change msec per step
      // if msec per step is ~= 0, flip dir then repeat this process to ramp up

      // *** would need to define states in main such that each motor is checked if it is in such and such state to
      // then handle whether it should be stepper normally or ramped up / down

      Carriage.flip_dir();
      Serial.print("In func\n");
    }
    // for (Switch *s : Switches)
    // {
    //   if (s.is_rising_edge())
    //   {
    //     // turn_around() handles things such as direction change, ramp down and ramp up speeds and what equation
    //     // they ramp according to
    //     s.motor.turn_around(); // call turn around function of motor associated with that limit switch
    //   }
    // }
  }
  return 0;
}

void setup()
{
  // Serial.begin(115200);
  main_();
}
