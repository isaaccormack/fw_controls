#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int main_()
{
  Serial.begin(115200);

  Carriage Carriage;
  Mandrel Mandrel;

  Switch C_Home_Switch(config::c_home_switch_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);
  Switch M_Encoder_Switch(config::m_encoder_switch_pin, HIGH);

  Switch *Carriage_Switches[2] = {&C_Home_Switch, &C_Far_Switch};

  /*---------------------------------------------------------------------------
                        Home Carriage and Mandrel

  Precondition: Carriage direction is initialized such that carriage moves
                toward C_Home_Switch.
  ---------------------------------------------------------------------------*/

  Carriage.set_velocity(config::carriage_home_velocity);
  Mandrel.set_velocity(config::mandrel_home_velocity);

  /* Home Carriage */
  while (!C_Home_Switch.is_rising_edge())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
    }
  }
  Carriage.flip_dir();

  /* Home Mandrel */
  bool mandrel_step_count_flushed = false;
  while (!mandrel_step_count_flushed || Mandrel.get_step_count() != Mandrel.get_step_count_at_wind_start())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
    }
    // Note the the is_rising_edge() is sometimes buggy and reports true on falling edge.
    // I think this is because of length of indeterminate state of switch as capacitor discharges
    // leading to a false low value in the midst of high values. Could be solved by using latched
    // switches.
    if (M_Encoder_Switch.is_rising_edge())
    {
      Mandrel.clear_step_count();
      mandrel_step_count_flushed = true;
    }
  }

  /*---------------------------------------------------------------------------
                        Minimize wait time on ends

  Perform basic calibration to ensure that <yada yada yada>
  ---------------------------------------------------------------------------*/
  // Calibration sub-routine to find minimal possible required wait time at far end given wrap_angle and wrap_length

  // Note that using a backup step counter for mandrel is only required if is is possible the mandrel encoder
  // switch will not be hit during a pass due to sufficiently small wrap_length and/or wrap angle. That is, if the
  // number of steps the mandrel takes during a single pass is < ~860 (860 since can be more than 850 so pick safe
  // value) then we need to sync the mandrel step count with backup every turn around to ensure that when cariage
  // reaches the home end we have a value for the mandrel step count of 0 <= mandrel step count <= 850 so that
  // the wait time on each end is optimized. Namely, the unoptimization occurs when the step count at home is > 850
  // such that the mandrel needs to do an extra loop to clear the step count before it then is able to accurately
  // count up the value it should be at (say 100) for the beggining of the next loop.
  //
  // Specifically, the condition where backup_mandrel_step_count is needed is when:
  //     (mandrel_steps_per_rev * wrap_length * tan(wrap_angle)) / (TWO_PI * mandrel_radius) < ~860
  //
  // For now I leave the backup_mandrel_step_count in all the time, knowing that for most use cases it is extraneous
  // to keep code simple

  // Set Carriage and Mandrel velocity at speed they run at during wind
  Carriage.set_velocity(config::carriage_velocity);
  /* Vm / Vc = tan(wrap_angle)   =>   Vm,tan = Vc * tan(wrap angle) */
  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);
  Mandrel.set_velocity(mandrel_velocity);

  while (!C_Home_Switch.is_rising_edge())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
      Mandrel.inc_backup_step_count();
    }

    curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      if (Carriage.is_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() - Mandrel.get_step_count_at_dir_flip() >= Mandrel.get_far_end_wait_steps())
        {
          Carriage.clear_dir_flip_flag();
          // Guarantees that step count after direction change is always reset to a value between [0, 850]
          Mandrel.sync_step_count_with_backup();
        }
      }
      else
      {
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
    }

    if (C_Far_Switch.is_rising_edge())
    {
      Mandrel.set_step_count_at_dir_flip();

      Carriage.flip_dir();
      Carriage.set_dir_flip_flag();
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      // If not in the middle of changing direction
      if (!Carriage.is_dir_flip_flag_set())
      {
        Mandrel.clear_step_count();
      }
      Mandrel.clear_backup_step_counter();
    }
  }

  // Debug output
  Serial.print(Mandrel.get_step_count_at_wind_start());
  Serial.print(" <- step count at wind start\n");
  Serial.print(Mandrel.get_step_count());
  Serial.print(" <- Mandrel step count at end\n");
  Serial.print(Mandrel.get_far_end_wait_steps());
  Serial.print(" <- initial far end wait steps\n");

  /* Calculate required offset (if any) to far_end_wait_steps() as to make the wait time on the home
   * side greater than minimum amount (which Mandrel.get_far_end_wait_steps() is set to by default). */
  if (Mandrel.get_step_count_at_wind_start() >= Mandrel.get_step_count())
  {

    int_type mandrel_offset_steps = Mandrel.get_step_count_at_wind_start() - Mandrel.get_step_count();
    // If the steps waited on the home side is less than minimal acceptable wait steps
    if (mandrel_offset_steps < Mandrel.get_far_end_wait_steps())
    {
      // Add to far end wait steps s.t. when mandrel reaches home it will wait for a time longer then minimum acceptable wait steps
      Mandrel.add_to_far_end_wait_steps(1.2 * mandrel_offset_steps);
    }
  }
  else
  {
    // Guaranteed 0 <= mandrel_step_count <= ~860 (realistically 853 given machine is well toleranced)
    // Add 870 for safety to compensate for possibility that Mandrel.get_step_count() > 850, say 852
    int_type compensated_mandrel_offset_steps = (Mandrel.get_step_count_at_wind_start() + 870) - Mandrel.get_step_count();
    if (compensated_mandrel_offset_steps < (Mandrel.get_far_end_wait_steps() + 870))
    {
      {
        Mandrel.add_to_far_end_wait_steps(1.2 * (compensated_mandrel_offset_steps - (Mandrel.get_far_end_wait_steps() + 20)));
      }
    }
  }

  Serial.print(Mandrel.get_far_end_wait_steps());
  Serial.print(" <- new far end wait steps\n");

  while (1)
  {
  }

  Serial.print("\n");
  Serial.print(config::filament_offset_delay_steps);
  Serial.print(" <- filament_offset_delay_steps\n");

  int_type mandrel_wait_steps = 0;

  int_type passes = 0;
  while (passes < config::total_passes)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
      ++mandrel_wait_steps;
    }

    curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      if (Carriage.is_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() - Mandrel.get_step_count_at_dir_flip() >= config::filament_offset_delay_steps)
        {
          Carriage.clear_dir_flip_flag();
        }
      }
      else
      {
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
    }

    for (Switch *s : Carriage_Switches)
    {
      if (s->is_rising_edge() && !Carriage.is_dir_flip_flag_set())
      {
        mandrel_wait_steps = 0;
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
