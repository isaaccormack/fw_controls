#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Switch.h"

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int main_()
{
  Carriage Carriage;
  Mandrel Mandrel;

  Switch C_Home_Switch(config::c_home_switch_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);
  Switch M_Encoder_Switch(config::m_encoder_switch_pin, HIGH);

  Switch *Carriage_Switches[2] = {&C_Home_Switch, &C_Far_Switch};

  /*---------------------------------------------------------------------------
                          <Home Carriage and Mandrel>

  Precondition: Carriage direction is initialized such that carriage moves
                toward C_Home_Switch.
  
  Initialization sub-routine to bring carriage to home position resting against 
  C_Home_Switch and mandrel to default intialized step_count_at_start_of_pass
  position.
  ---------------------------------------------------------------------------*/

  Carriage.set_velocity(config::carriage_homing_velocity);
  Mandrel.set_velocity(config::mandrel_homing_velocity);

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
  while (!mandrel_step_count_flushed || Mandrel.get_step_count() != Mandrel.get_step_count_at_start_of_pass())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
      Mandrel.inc_backup_step_count();
    }

    /* Note that is_rising_edge() sometimes buggy and reports true on falling edge. Think this is 
       because of long indeterminate state (~50ms) on falling edge due to RC values in RC circuit
       (ie. a LOW value is read then a HIGH value due to indeterminate state).
       Possible solved by using latched switches? */
    if (M_Encoder_Switch.is_rising_edge())
    {
      Mandrel.clear_step_count();
      Mandrel.clear_backup_step_count();
      mandrel_step_count_flushed = true;
    }
  }

  /*---------------------------------------------------------------------------
                  <Find and Set Minimum Wait Time on Far End>

  Calibration sub-routine to find minimal wait time at far end such that the home
  end has a wait time greater than the minimal wait time. Algorithm performs a 
  single wrap cycle using user defined wrap parameters to determine the step count
  of the mandrel when home is finally reached. From this, wait time on far end is
  increased, if necessary, such that wait time on home end is at least the
  minumum allowable value. Wait time on ends necessary to reduce wear on machine
  and possible slippage of timing belt pulleys of motor shaft that may occur
  during an immediate direction flip of the carriage due to its momentum.
  ---------------------------------------------------------------------------*/

  Carriage.set_velocity(config::carriage_velocity);

  /* Vm,tan / Vc = tan(wrap_angle)     =>     Vm,tan = Vc * tan(wrap_angle) */
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
      if (Carriage.is_far_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() - Mandrel.get_step_count_at_far_dir_flip() >= Mandrel.get_far_end_wait_steps())
        {
          Carriage.clear_far_dir_flip_flag();
          Mandrel.sync_step_count_with_backup(); // Guarantees step count after direction change always in range [0, ~850]
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
      Mandrel.set_step_count_at_far_dir_flip();

      Carriage.flip_dir();
      Carriage.set_far_dir_flip_flag();
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      if (!Carriage.is_far_dir_flip_flag_set())
        Mandrel.clear_step_count();

      Mandrel.clear_backup_step_count();
    }
  }
  Carriage.flip_dir();
  Carriage.set_home_dir_flip_flag();

  if (Mandrel.get_step_count() < (Mandrel.get_step_count_at_start_of_pass() + 50) &&
      Mandrel.get_step_count() > (Mandrel.get_step_count_at_start_of_pass() - Mandrel.get_far_end_wait_steps()))
  {
    Mandrel.add_to_far_end_wait_steps(Mandrel.get_far_end_wait_steps() + 50);
  }

  /*---------------------------------------------------------------------------
                    <2 Axis Filament Wind for User Defined Passes>

  Main routine which performs a 2 axis filament wind at users specifications defined
  in Config.h under PUBLIC CALIBRATIONS.
  ---------------------------------------------------------------------------*/

  delay(1000);

  int_type passes = 0;
  while (passes < config::total_passes)
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
      if (Carriage.is_far_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() - Mandrel.get_step_count_at_far_dir_flip() >= Mandrel.get_far_end_wait_steps())
        {
          Carriage.clear_far_dir_flip_flag();
          Mandrel.sync_step_count_with_backup(); // Guarantees step count after direction change always in range [0, ~850]
        }
      }
      else if (Carriage.is_home_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() == Mandrel.get_step_count_at_start_of_pass())
        {
          Carriage.clear_home_dir_flip_flag();
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
      Mandrel.set_step_count_at_far_dir_flip();

      Carriage.flip_dir();
      Carriage.set_far_dir_flip_flag();
    }

    if (C_Home_Switch.is_rising_edge())
    {
      Carriage.flip_dir();
      Carriage.set_home_dir_flip_flag();
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      /* Don't clear step count at far end b/c wait criteria relies on difference between initial and
         current step count values. Home end uses comparison against absolute so doesn't matter. */
      if (!Carriage.is_far_dir_flip_flag_set())
        Mandrel.clear_step_count();

      Mandrel.clear_backup_step_count();
    }
  }
  return 0;
}

void setup()
{
  main_();
}

void loop() {}
