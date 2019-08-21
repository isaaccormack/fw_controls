#include "Config.h"
#include "Mandrel.h"
#include "Rotator.h"
#include "Switch.h"

namespace util
{
typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

/* Function to set optimal values for wind parameters based on calibrated values in Config.h */
void setup_for_wind_(Mandrel &Mandrel, Carriage &Carriage, Rotator &Rotator)
{
  const double c_accel_dist = (39.375 / config::deg_wrap_angle) - 0.3125; // linear eq. between [30deg -> 1in, 90deg -> 0.125in]
  Carriage.set_total_accel_steps(c_accel_dist);

  double m_velocity = TWO_PI * config::m_radius * config::m_max_rev_per_sec;
  double c_velocity = 8.0 + (2.0 * c_accel_dist); // linear eq. between [0.125in -> 8.25in/s, 1in -> 10in/s]

  const double wrap_angle = config::deg_wrap_angle * PI / 180.0; // rad
  const double crit_wrap_angle = atan2f(m_velocity, c_velocity);
  const double deg_crit_wrap_angle = 180.0 * crit_wrap_angle / PI;

  if ((config::deg_wrap_angle + 0.25) < deg_crit_wrap_angle) // + 0.25 to prevent wrap_angle close to crit_wrap_angle causing error is floating point equation
  {
    // Then max speed of wind is bounded by max carriage velocity
    m_velocity = c_velocity * tan(wrap_angle);

    // Find minimal wait steps at end of pass given mandrel radius and wrap angle
    const double min_wait_steps = config::m_steps_per_rev * config::min_wait_dist / (TWO_PI * config::m_radius);

    // Find wait steps using linear equation between [30deg -> wait_steps(min_wait_dist), deg_crit_wrap_angle -> max_wait_steps]
    // Hardcode 30 as minimal wrap angle for 3 axis system
    double wait_steps = ((config::deg_wrap_angle * (config::max_wait_steps - min_wait_steps)) + (deg_crit_wrap_angle * min_wait_steps) - (30 * config::max_wait_steps)) / (deg_crit_wrap_angle - 30.0);

    if (wait_steps > config::max_wait_steps)
    {
      wait_steps = config::max_wait_steps;
    }
    Mandrel.set_far_end_wait_steps((int_type)wait_steps);
  }
  else
  {
    // Max speed of wind is bounded by max mandrel rev per sec
    c_velocity = m_velocity / tan(wrap_angle);

    // Set wait steps to max as mandrel is rotating quick enough thaT optimization isn't necessary
    Mandrel.set_far_end_wait_steps(config::max_wait_steps);
  }

  double min_resonant_velocity = TWO_PI * config::m_radius * config::m_min_resonant_rev_per_sec;
  double max_resonant_velocity = TWO_PI * config::m_radius * config::m_max_resonant_rev_per_sec;
  if (m_velocity > min_resonant_velocity && m_velocity < max_resonant_velocity)
  {
    m_velocity = min_resonant_velocity;
    c_velocity = m_velocity / tan(wrap_angle);
  }

  Mandrel.set_velocity(m_velocity);
  Carriage.set_init_usec_per_step_accel(c_accel_dist, c_velocity);
  Rotator.set_rev_per_sec(c_accel_dist, c_velocity);
}

/* Note step_tolerance must exist as factors such as:
  - Loose carriage timing belt
  - Loose carriage or mandrel encoder limit switches
  - Slipping of timing belt pulley on motor shaft
  can cause the number of steps the mandrel has taken when the carriage returns to the home end
  after a pass to vary by at most step_tolerance. To ensure that carriage never returns when the
  mandrel step count is only a few steps before it is scheduled to leave for the next pass, the
  far end delay steps are incremented as necessary. */
constexpr int_type step_tolerance = 20;

// maybe this algorithm can be made more efficient as it is run every pass this would be nice
bool too_few_wait_steps_on_home_side(const int_type curr_steps, const int_type start_of_next_pass_steps, const int_type min_wait_steps)
{
  if (curr_steps >= start_of_next_pass_steps)
  {
    if (curr_steps + min_wait_steps > config::m_steps_per_rev && start_of_next_pass_steps < min_wait_steps)
    {
      if (curr_steps + min_wait_steps - config::m_steps_per_rev > start_of_next_pass_steps)
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
    if (start_of_next_pass_steps + step_tolerance > config::m_steps_per_rev && curr_steps < step_tolerance)
    {
      if (start_of_next_pass_steps + step_tolerance - config::m_steps_per_rev > curr_steps)
      {
        return true;
      }
    }
    else if (start_of_next_pass_steps - curr_steps < min_wait_steps)
    {
      return true;
    }
  }
  return false;
}

/* Delay carriage while mandrel rotates config::min_wait_steps + step_tolerance */
void delay_for_mandrel_revolution(Mandrel &Mandrel, Switch &M_Encoder_Switch)
{
  int_type mandrel_step_count = 0;
  while (mandrel_step_count < Mandrel.get_far_end_wait_steps() + step_tolerance)
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
    if (M_Encoder_Switch.is_rising_edge())
    {
      Mandrel.clear_step_count();
      Mandrel.clear_backup_step_count();
    }
  }
}

} // namespace util