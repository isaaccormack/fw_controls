#include "Config.h"
#include "Mandrel.h"
#include "Rotator.h"
#include "Switch.h"

namespace util
{
typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

/* Note step_tolerance must exist as factors such as:
  - Loose carriage timing belt
  - Loose carriage or mandrel encoder limit switches
  - Slipping of timing belt pulley on motor shaft
  can cause the number of steps the mandrel has taken when the carriage returns to the home end
  after a pass to vary by at most step_tolerance. To ensure that carriage never returns when the
  mandrel step count is only a few steps before it is scheduled to leave for the next pass, the
  far end delay steps are incremented as necessary. */
int_type step_tolerance = 20;

bool too_few_wait_steps_on_home_side(const int_type curr_steps, const int_type start_of_next_pass_steps)
{
  if (curr_steps >= start_of_next_pass_steps)
  {
    if (curr_steps + config::min_wait_steps > config::mandrel_steps_per_rev && start_of_next_pass_steps < config::min_wait_steps)
    {
      if (curr_steps + config::min_wait_steps - config::mandrel_steps_per_rev > start_of_next_pass_steps)
      {
        return true;
      }
    }
    else if (curr_steps - start_of_next_pass_steps < step_tolerance)
    {
      return true;
    }
  }
  else // curr_steps < start_next_pass_steps
  {
    if (start_of_next_pass_steps + step_tolerance > config::mandrel_steps_per_rev && curr_steps < step_tolerance)
    {
      if (start_of_next_pass_steps + step_tolerance - config::mandrel_steps_per_rev > curr_steps)
      {
        return true;
      }
    }
    else if (start_of_next_pass_steps - curr_steps < config::min_wait_steps)
    {
      return true;
    }
  }
  return false;
}

/* Delay carriage while mandrel rotates config::min_wait_steps + step_tolerance */
void delay_for_mandrel_revolution(Mandrel &Mandrel, Rotator &Rotator, Switch &M_Encoder_Switch)
{
  int_type mandrel_step_count = 0;
  while (mandrel_step_count < config::min_wait_steps + step_tolerance)
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
      Mandrel.inc_backup_step_count();
      ++mandrel_step_count;
    }

    curr_usec = micros();
    if (Rotator.is_enabled() && curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      Rotator.inc_step_count();

      if (Rotator.get_step_count() > config::rotator_steps_for_wrap_angle)
      {
        Rotator.clear_step_count();
        Rotator.disable();
      }
    }
    if (M_Encoder_Switch.is_rising_edge())
    {
      Mandrel.clear_step_count();
      Mandrel.clear_backup_step_count();
    }
  }
}

} // namespace util