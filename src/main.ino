#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int main_()
{
  Serial.begin(115200);

  Carriage Carriage;
  Carriage.set_velocity(config::carriage_velocity);

  /* Vm / Vc = tan(wrap_angle)   =>   Vm,tan = Vc * tan(wrap angle) */
  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);

  Mandrel Mandrel;
  Mandrel.set_velocity(mandrel_velocity);

  Switch C_Home_Switch(config::c_home_switch_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);
  Switch M_Encoder_Switch(config::m_encoder_switch_pin);

  Switch *Carriage_Switches[2] = {&C_Home_Switch, &C_Far_Switch};

  // Carriage.home();

  Serial.print("\n");
  Serial.print(config::filament_offset_delay_steps);
  Serial.print(" <- filament offset delay steps\n");

  int_type passes = 0;
  while (passes < config::total_passes)
  {
    unsigned long int curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
    }

    curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      if (Carriage.is_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() - Mandrel.get_step_count_at_dir_flip() < config::filament_offset_delay_steps)
        {
          continue;
        }
        Carriage.clear_dir_flip_flag();
      }
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
    }

    for (Switch *s : Carriage_Switches)
    {
      if (s->is_rising_edge() && !Carriage.is_dir_flip_flag_set())
      {
        mandrel_step_counter = 0;
        Mandrel.set_step_count_at_dir_flip();

        Carriage.flip_dir();
        Carriage.set_dir_flip_flag();
        ++passes;
      }
    }
    if (M_Encoder_Switch.is_rising_edge())
    {
      // If not in the middle of changing direction
      if (!Carriage.is_dir_flip_flag_set())
        Mandrel.clear_step_count();
    }
  }
  return 0;
}

void setup()
{
  main_();
}

void loop() {}
