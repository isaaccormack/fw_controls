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

  // Calibration sub-routine to find minimal possible required wait time at far end given wrap_angle and wrap_length

  // Set Carriage and Mandrel velocity at speed they run at during wind
  Carriage.set_velocity(config::carriage_velocity);
  /* Vm / Vc = tan(wrap_angle)   =>   Vm,tan = Vc * tan(wrap angle) */
  double mandrel_velocity = config::carriage_velocity * tan(config::wrap_angle);
  Mandrel.set_velocity(mandrel_velocity);

  long_int_type mandrel_steps_at_far_side = 0;
  long_int_type mandrel_steps_on_one_pass = 0;
  long_int_type mandrel_steps_on_one_pass_counter = 0;
  while (!C_Home_Switch.is_rising_edge())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
      Mandrel.inc_backup_step_count();
      ++mandrel_steps_on_one_pass_counter;
    }

    curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      if (Carriage.is_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() - Mandrel.get_step_count_at_dir_flip() >= Mandrel.get_far_end_wait_steps())
        {
          Carriage.clear_dir_flip_flag();
          // Guarantees that step count after direction change is always reset to a sane value -> [0, 850]
          Mandrel.restore_step_count_with_backup();
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

      // Record steps taken over one pass
      mandrel_steps_on_one_pass = mandrel_steps_on_one_pass_counter;
      mandrel_steps_at_far_end = mandrel_steps_on_one_pass_counter;
      mandrel_steps_on_one_pass_counter = 0;
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

  // Take average of mandrel steps on one pass
  mandrel_steps_on_one_pass += mandrel_steps_on_one_pass_counter;
  mandrel_steps_on_one_pass /= 2;

  /* The below logic compensates for the possibility that the mandrel step count was never cleared, due to a sufficiently 
   * small wrap_length and wrap_angle, such that at this point the value of Mandrel.get_step_count() > 850. The compensation
   * algorithm below this cannot be equipped to deal with this scenario, for reasons described in the docs, instead the 
   * step_count_at_wind_start for the mandrel is increased (from well defined intial value) to a value that guarantees the
   * mandrel step count will be able to rollover at a time other than during the far end direction flip delay. */

  // If it is unlikely that the mandrel enocder switch was never pressed because the number of steps taken on
  // a single pass was too low. Pick 860 arbitrarily for now just to account for fact that mandrel could rotate
  // ~852 steps > 850.
  if (mandrel_steps_on_one_pass < 860)
  {
    // If the mandrel steps at far end was such that the mandrel step counter didn't clear (ie. step counter became > 850)
    // because mandrel encoder switch was hit during turn around period
    int_type mandrel_start_shift_factor = (3 * config::wrap_angle) + 20;
    if (mandrel_steps_at_far_end > (850 - mandrel_start_shift_factor))
    {
      // Add 3*config::wrap_angle + 100 to the mandrels initial step count at wind start such that the mandrels step count
      // will have rolled over before it reaches far end so mandrel_steps_at_far_end will become < 850 - mandrel_start_shift_factor
      Mandrel.add_to_step_count_at_wind_start(mandrel_start_shift_factor + 100);
    }
  }

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
    if (mandrel_offset_steps < Mandrel.get_far_end_wait_steps())
    {
      Mandrel.add_to_far_end_wait_steps(1.2 * mandrel_offset_steps);
    }
  }
  else
  {
    /* Move below to docs */
    // Due to Mandrel step count not being reset when carriage is at end of travel, the mandrel step count can grow signficantly
    // larger than 850 given that the wrap_length is sufficiently small to give a small amount of time between carriage ends such
    // that the mandrel step count would not ever reset during the calibration period, resulting in a value larger than 850.
    // Statistically speaking, the value of the mandrel is very unlikely to be higher than 1500 (in total). Add 2000 to be safe.

    /* Consider the case where mandrel step count at wind start is 0 and we are at the instant immediately following the mandrel
     * encoder switch rising edge. Assume the mandrel steps between home and far side is 850 and the mandrel reaches the other 
     * side before the mandrel encoder switch rising edge is seen, then the mandrel rotate at maximum (for high wrap angle of ~ 80) 
     * 250 steps then continues back to home side. The mandrel would then be able to step (850 - 250) 600 steps before the mandrel
     * limit switch clears the step count given the mandrel limit switch was hit immediately after the carriage hit the far end switch.
     * The the max value of the mandrels step counter is 850 + 250 + 600 = 2 * 850 = 1700. Worst case is 2 * 850 - (~250). */

    // the below assumes that step count is reset at 850 steps (hence 2000 - 850 = 1150) -> could make this dynamic, or
    // just impose restrictions on conditions that would lead to likely / problematic rollover at home end (ie. assert
    // that wrap_length > 10in and theta > 20deg OR combo assertion, ie. assert wrap_length * theta > 200)

    // Do above compensation such that at this point we can be guaranteed 0 <= Mandrel.get_step_count() <= 800
    int_type compensated_mandrel_offset_steps = (Mandrel.get_step_count_at_wind_start() + 20) - Mandrel.get_step_count();
    if (compensated_mandrel_offset_steps < (Mandrel.get_far_end_wait_steps() + 20))
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
