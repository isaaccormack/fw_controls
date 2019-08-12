#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Rotator.h"
#include "include/Switch.h"
#include "include/Util.h"

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int main_()
{
  Serial.begin(115200);

  Carriage Carriage(config::carriage_step_pin, config::carriage_dir_pin);
  Mandrel Mandrel;
  Rotator Rotator;

  Switch C_Home_Switch(config::c_home_switch_pin);
  Switch C_Far_Switch(config::c_far_switch_pin);
  Switch M_Encoder_Switch(config::m_encoder_switch_pin, HIGH);
  Switch Rotator_Switch(config::rotator_switch_pin);

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
  Rotator.set_rev_per_sec(config::rotator_homing_velocity);

  // Could make these happen simultaneously

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
  Carriage.set_home_dir_flip_flag();

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

  /* Home Rotator */
  bool rotator_switch_pressed = false;
  while (Rotator.get_step_count() < config::rotator_steps_to_home || !rotator_switch_pressed)
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

  // update this below to describe how it is integrated as first pass of wind
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

  // Serial.print(Mandrel.get_far_end_wait_steps());
  // Serial.print(" <- Initial far end wait steps\n");

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
          Mandrel.sync_step_count_with_backup();
          Rotator.enable();
        }
      }
      else if (Carriage.is_home_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() == Mandrel.get_step_count_at_start_of_pass())
        {
          Carriage.clear_home_dir_flip_flag();
          Rotator.enable();
        }
      }
      else
      {
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

      if (Rotator.get_step_count() > config::rotator_steps_for_wrap_angle)
      {
        Rotator.clear_step_count();
        Rotator.disable();
      }
    }

    if (C_Far_Switch.is_rising_edge())
    {
      Mandrel.set_step_count_at_far_dir_flip();

      Carriage.flip_dir();
      Carriage.set_far_dir_flip_flag();
      Rotator.flip_dir();
      Rotator.enable();
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      if (!Carriage.is_far_dir_flip_flag_set())
        Mandrel.clear_step_count();

      Mandrel.clear_backup_step_count();
    }
  }
  // When C_Home_Switch.is_rising_edge()
  Carriage.flip_dir();
  Carriage.set_home_dir_flip_flag();
  Rotator.flip_dir();
  Rotator.enable();

  // need to put this above serial print
  // Embed calibration pass as first pass of wind by incrementing step count at start of next pass and delaying accordingly if necessary
  Mandrel.inc_step_count_at_start_of_pass();

  Serial.print("\nInitially\n");
  Serial.print(config::min_wait_steps);
  Serial.print(" <- min wait steps\n");
  Serial.print(Mandrel.get_step_count());
  Serial.print(" <- Initial steps when arriving at home\n");
  Serial.print(Mandrel.get_step_count_at_start_of_pass());
  Serial.print(" <- Steps when leaving home on next pass\n");

  // int_type start_of_next_pass_steps = (Mandrel.get_step_count_at_start_of_pass() + config::pass_offset_steps) % config::mandrel_steps_per_rev;

  if (util::too_few_wait_steps_on_home_side(Mandrel.get_step_count(), Mandrel.get_step_count_at_start_of_pass()))
  {
    Serial.print(" <- Initially im in too few wait steps!\n");
    Mandrel.add_to_far_end_wait_steps(Mandrel.get_far_end_wait_steps() + 20);
    util::delay_for_mandrel_rev(Mandrel, Rotator, M_Encoder_Switch);
  }

  Serial.print(Mandrel.get_far_end_wait_steps());
  Serial.print(" <- Adjusted far end wait steps\n");

  /*---------------------------------------------------------------------------
                    <2 Axis Filament Wind for User Defined Passes>
  Main routine which performs a 2 axis filament wind at users specifications defined
  in Config.h under CALIBRATIONS FOR USER.
  ---------------------------------------------------------------------------*/

  // // non-interruptive delay
  // long_int_type delay_usec = 1000000; // us
  // long_int_type start_usec = micros();
  // while (micros() - start_usec < delay_usec)
  // {
  // }

  int_type passes = 1; // by calibration pass
  int_type layers = 0;
  while (layers < config::total_layers)
  {
    if (passes == config::passes_per_layer)
    {
      Serial.print("\nNext layer!\n");

      passes = 0;
      ++layers;
      Mandrel.set_step_count_at_start_of_pass_by_layer(layers);

      Serial.print(config::min_wait_steps);
      Serial.print(" <- min wait steps\n");
      Serial.print(Mandrel.get_step_count());
      Serial.print(" <- Initial steps when arriving at home\n");
      Serial.print(Mandrel.get_step_count_at_start_of_pass());
      Serial.print(" <- Steps when leaving home on next pass\n");

      if (util::too_few_wait_steps_on_home_side(Mandrel.get_step_count(), Mandrel.get_step_count_at_start_of_pass()))
      {
        Serial.print(" <- Im in too few wait steps on new layer!\n");
        util::delay_for_mandrel_rev(Mandrel, Rotator, M_Encoder_Switch);
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
          Carriage.clear_far_dir_flip_flag();
          Mandrel.sync_step_count_with_backup();
          Rotator.enable();
        }
      }
      else if (Carriage.is_home_dir_flip_flag_set())
      {
        if (Mandrel.get_step_count() == Mandrel.get_step_count_at_start_of_pass())
        {
          Carriage.clear_home_dir_flip_flag();
          Rotator.enable();
        }
      }
      else
      {
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

      if (Rotator.get_step_count() > config::rotator_steps_for_wrap_angle)
      {
        Rotator.clear_step_count();
        Rotator.disable();
      }
    }

    if (C_Far_Switch.is_rising_edge())
    {
      Mandrel.set_step_count_at_far_dir_flip();

      Carriage.flip_dir();
      Carriage.set_far_dir_flip_flag();
      Rotator.flip_dir();
      Rotator.enable();
    }

    if (C_Home_Switch.is_rising_edge())
    {
      // Serial.print(Mandrel.get_step_count_at_start_of_pass());
      // Serial.print(" <- Before incremenet steps when leaving home home\n");

      Mandrel.inc_step_count_at_start_of_pass();
      Carriage.flip_dir();
      Carriage.set_home_dir_flip_flag();
      Rotator.flip_dir();
      Rotator.enable();

      ++passes;

      // Serial.print(Mandrel.get_step_count());
      // Serial.print(" <- Steps when arriving at home\n");
      // Serial.print(Mandrel.get_step_count_at_start_of_pass());
      // Serial.print(" <- Supposed steps when leaving home home\n");
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
