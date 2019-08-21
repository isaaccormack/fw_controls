#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Rotator.h"
#include "include/Switch.h"
#include "include/Utils.cpp"

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int main_()
{
  Serial.begin(115200);

  Carriage Carriage(config::c_step_pin, config::c_dir_pin);
  Mandrel Mandrel;
  Rotator Rotator;

  Switch C_Home_Switch(config::c_home_switch_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);
  Switch M_Encoder_Switch(config::m_encoder_switch_pin, HIGH);
  Switch Rotator_Switch(config::r_switch_pin);

  Switch *Carriage_Switches[2] = {&C_Home_Switch, &C_Far_Switch};

  /*---------------------------------------------------------------------------
                          <Home Carriage and Mandrel>
  Precondition: Carriage direction is initialized such that carriage moves
                toward C_Home_Switch.
  
  Initialization sub-routine to bring carriage to home position resting against 
  C_Home_Switch and mandrel to default intialized step_count_at_start_of_pass
  position.
  ---------------------------------------------------------------------------*/

  Carriage.set_velocity(config::c_homing_velocity); // can change this to acceleration once tested
  Mandrel.set_velocity(config::m_homing_velocity);
  Rotator.set_rev_per_sec(config::r_homing_velocity);

  /* Home Mandrel - Guarantees Mandrel starts first pass at correct step count */
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

    /* Note that is_rising_edge() sometimes reports true on falling edge. Think this is because
       of long indeterminate state (~50ms) on falling edge due to RC values in RC circuit
       (ie. a LOW value is read then a HIGH value due to indeterminate state).
       Possible solved by using latched switches? */
    if (M_Encoder_Switch.is_rising_edge())
    {
      Mandrel.clear_step_count();
      Mandrel.clear_backup_step_count();
      mandrel_step_count_flushed = true;
    }
  }

  /* Home Rotator - Guarantees Rotator initially in the 0deg position */
  bool rotator_switch_pressed = false;
  while (Rotator.get_step_count() < config::r_steps_to_home || !rotator_switch_pressed)
  {
    if (Rotator_Switch.is_rising_edge())
    {
      Rotator.flip_dir();
      Rotator.clear_step_count();
      rotator_switch_pressed = true;
    }
    long_int_type curr_usec = micros();
    if (curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      Rotator.inc_step_count();
    }
  }
  Rotator.clear_step_count();
  Rotator.flip_dir();

  /* Required to home carriage the correct steps for acceleration profile */
  util::setup_for_wind_(Mandrel, Carriage, Rotator);

  /* Home Carriage - Guarantees Carriage is the correct steps before C_Home_Switch for acceleration profile */
  int_type step_count = 0;
  bool home_switch_pressed = false;
  while (!home_switch_pressed || step_count < Carriage.get_total_accel_steps())
  {
    long_int_type curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      Carriage.set_last_step_time(curr_usec);
      Carriage.step();
      ++step_count;
    }
    if (C_Home_Switch.is_rising_edge())
    {
      home_switch_pressed = true;
      step_count = 0;
    }
  }
  Carriage.flip_dir();
  Carriage.set_home_dir_flip_flag();

  /*---------------------------------------------------------------------------
                  <Find and Set Minimum Wait Time on Far End>
  Calibration sub-routine to find minimal wait steps at the far end such that
  there are greater than config::min_wait_steps between the time the carriage
  arrives at the home end and it is scheduled to start the next pass. Specifically
  this is done by completing the first pass of the wind and checking if the 
  difference between the mandrel steps at start of next pass and mandrel steps
  completed when the carriage arrived at home position is less than config::min_wait_steps,
  or if the carriage arrived only a few steps after it is scheduled to leave. In
  either case, the wait steps on the far end is incremented such that this will
  not be the case for successive passes on this layer and the carriage performs
  an extra revolution to provide adequate delay. It should be noted that at least
  the minimal wait time on each end is necessary so that the filament can settle 
  before the next pass begins.
  ---------------------------------------------------------------------------*/

  /* Ensures that mandrel far end wait distance is set such that carriage will arrive back at mandrel 
     with atleast min wait steps before next pass */
  while (!Carriage.is_at_home_end() || Carriage.is_decelerating())
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
          Mandrel.sync_step_count_with_backup();

          Carriage.start_accelerating();
          Carriage.clear_far_dir_flip_flag();

          Rotator.enable();
        }
      }
      else if (Carriage.is_home_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() == Mandrel.get_step_count_at_start_of_pass())
        {
          Carriage.start_accelerating();
          Carriage.clear_home_dir_flip_flag();

          Rotator.enable();
        }
      }
      else
      {
        if (Carriage.is_accelerating())
        {
          Carriage.set_next_usec_per_step_accel();
          Carriage.check_accel_finished();
        }
        else if (Carriage.is_decelerating())
        {
          Carriage.set_next_usec_per_step_decel();
          Carriage.check_decel_finished();
        }
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
    }

    curr_usec = micros();
    if (Rotator.is_enabled() && curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      Rotator.inc_step_count();
      Rotator.check_rotation_finished();
    }

    if (C_Home_Switch.is_rising_edge())
    {
      Carriage.set_home_end_flag();
      Carriage.start_decelerating();

      Rotator.flip_dir();
      Rotator.enable();
    }

    if (C_Far_Switch.is_rising_edge())
    {
      Carriage.set_far_end_flag();
      Carriage.start_decelerating();

      Rotator.flip_dir();
      Rotator.enable();
    }

    if (Carriage.is_at_far_end() && !Carriage.is_decelerating())
    {
      Mandrel.set_step_count_at_far_dir_flip();

      Carriage.clear_far_end_flag();
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
  Carriage.clear_home_end_flag();
  Carriage.flip_dir();
  Carriage.set_home_dir_flip_flag();

  // Calibration pass is really first pass of wind
  Mandrel.inc_step_count_at_start_of_pass();

  if (util::too_few_wait_steps_on_home_side(Mandrel.get_step_count(),
                                            Mandrel.get_step_count_at_start_of_pass(),
                                            Mandrel.get_far_end_wait_steps()))
  {
    Mandrel.add_to_far_end_wait_steps(Mandrel.get_far_end_wait_steps() + util::step_tolerance);
    util::delay_for_mandrel_revolution(Mandrel, M_Encoder_Switch);
  }

  /*---------------------------------------------------------------------------
                    <3 Axis Filament Wind for User Defined Passes>
  Main routine which performs a 3 axis filament wind at users specifications defined
  in include/Config.h under CALIBRATIONS FOR USER. Given a user defined wrap angle,
  the carriage and mandrel actuate at a predetermined constant velocity while the
  applicator head rotates as necessary such that the filament is applied parallel
  to the mandrel during a wind.
  ---------------------------------------------------------------------------*/

  int_type passes = 1; // 1 since calibration pass
  int_type layers = 0;
  while (layers < config::total_layers)
  {
    if (passes == config::passes_per_layer)
    {
      passes = 0;
      ++layers;
      Mandrel.set_step_count_at_start_of_pass_by_layer(layers);

      if (util::too_few_wait_steps_on_home_side(Mandrel.get_step_count(),
                                                Mandrel.get_step_count_at_start_of_pass(),
                                                Mandrel.get_far_end_wait_steps()))
      {
        util::delay_for_mandrel_revolution(Mandrel, M_Encoder_Switch);
      }
    }

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
          Mandrel.sync_step_count_with_backup();

          Carriage.start_accelerating();
          Carriage.clear_far_dir_flip_flag();

          Rotator.enable();
        }
      }
      else if (Carriage.is_home_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() == Mandrel.get_step_count_at_start_of_pass())
        {
          Carriage.start_accelerating();
          Carriage.clear_home_dir_flip_flag();

          Rotator.enable();
        }
      }
      else
      {
        if (Carriage.is_accelerating())
        {
          Carriage.set_next_usec_per_step_accel();
          Carriage.check_accel_finished();
        }
        else if (Carriage.is_decelerating())
        {
          Carriage.set_next_usec_per_step_decel();
          Carriage.check_decel_finished();
        }
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
    }

    curr_usec = micros();
    if (Rotator.is_enabled() && curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
    {
      Rotator.set_last_step_time(curr_usec);
      Rotator.step();
      Rotator.inc_step_count();
      Rotator.check_rotation_finished();
    }

    if (C_Home_Switch.is_rising_edge())
    {
      Carriage.set_home_end_flag();
      Carriage.start_decelerating();

      Rotator.flip_dir();
      Rotator.enable();
    }

    if (Carriage.is_at_home_end() && !Carriage.is_decelerating())
    {
      Mandrel.inc_step_count_at_start_of_pass();

      Carriage.clear_home_end_flag();
      Carriage.flip_dir();
      Carriage.set_home_dir_flip_flag();

      // Need to check this every time home is reached as steps taken per pass varies
      if (util::too_few_wait_steps_on_home_side(Mandrel.get_step_count(),
                                                Mandrel.get_step_count_at_start_of_pass(),
                                                Mandrel.get_far_end_wait_steps()))
      {
        util::delay_for_mandrel_revolution(Mandrel, M_Encoder_Switch);
      }

      ++passes;
    }

    if (C_Far_Switch.is_rising_edge())
    {
      Carriage.set_far_end_flag();
      Carriage.start_decelerating();

      Rotator.flip_dir();
      Rotator.enable();
    }

    if (Carriage.is_at_far_end() && !Carriage.is_decelerating())
    {
      Mandrel.set_step_count_at_far_dir_flip();

      Carriage.clear_far_end_flag();
      Carriage.flip_dir();
      Carriage.set_far_dir_flip_flag();
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

void setup() { main_(); }

void loop() {}
