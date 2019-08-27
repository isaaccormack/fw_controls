#include "include/Carriage.h"
#include "include/Mandrel.h"
#include "include/Rotator.h"
#include "include/Switch.h"

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
  Switch Rotator_Switch(config::r_switch_pin);
  Switch M_Encoder_Switch(config::m_encoder_switch_pin);

  /*---------------------------------------------------------------------------
                          <Home Carriage and Mandrel>
  Precondition: Carriage direction is initialized such that carriage moves
                toward C_Home_Switch.
  
  Initialization sub-routine to bring carriage to home position resting against 
  C_Home_Switch and mandrel to default intialized step_count_at_start_of_pass
  position.
  ---------------------------------------------------------------------------*/

  // Current prognosis of system is that it seems everything is working functionally, except either
  // a calculation of the dwell steps or something of the like is either fundamentally wrong,
  // or wrong in implementation (possible rounding error). It seems that somethims multiple patterns
  // per circuit is not respected but instead it acts as one pattern per circuit, also the machine
  // either advances too much between passes or not at all. This has not always been the case, the
  // system has worked more or less with this equation prior giving (more) accurate distances
  // between strands therefore assume error in implementation of some part of initial setup algo.

  // /* Home Rotator - Guarantees Rotator initially in the 0deg position */
  // Rotator.set_rev_per_sec(config::r_homing_rev_per_sec);
  // bool rotator_switch_pressed = false;
  // while (Rotator.get_step_count() < config::r_steps_to_home || !rotator_switch_pressed)
  // {
  //   if (Rotator_Switch.is_rising_edge())
  //   {
  //     Rotator.flip_dir();
  //     Rotator.clear_step_count();
  //     rotator_switch_pressed = true;
  //   }
  //   long_int_type curr_usec = micros();
  //   if (curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
  //   {
  //     Rotator.set_last_step_time(curr_usec);
  //     Rotator.step();
  //     Rotator.inc_step_count();
  //   }
  // }
  // Rotator.clear_step_count();
  // Rotator.flip_dir();

  // Private scope so helper variables are cleared off stack before control algo
  {
    Serial.print("circuits_per_layer: ");
    Serial.print(config::circuits_per_layer);
    Serial.print("\n");

    const double c_accel_dist = (39.375 / config::deg_wrap_angle) - 0.3125; // linear eq. between [30deg -> 1in, 90deg -> 0.125in]
    const int_type c_accel_steps = (c_accel_dist * config::steps_per_rev) / (config::c_pulley_pitch * config::c_num_pulley_teeth);
    Carriage.set_total_accel_steps(c_accel_steps);

    // Set each to max velocity
    double m_velocity = TWO_PI * config::m_radius * config::m_max_rev_per_sec;
    double c_velocity = 8.0 + (2.0 * c_accel_dist); // linear eq. between [0.125in -> 8.25in/s, 1in -> 10in/s]

    const double crit_wrap_angle = atan2f(m_velocity, c_velocity);
    const double deg_crit_wrap_angle = (180.0 * crit_wrap_angle) / PI;
    if ((config::deg_wrap_angle + 0.25) < deg_crit_wrap_angle) // + 0.25 to prevent wrap_angle close to crit_wrap_angle causing error is floating point equation
    {
      // Then max speed of wind is bounded by max carriage velocity
      m_velocity = c_velocity * tan(config::wrap_angle);
      const double mandrel_rev_per_sec = m_velocity / (TWO_PI * config::m_radius);
      if (mandrel_rev_per_sec > config::m_min_resonant_rev_per_sec && mandrel_rev_per_sec < config::m_max_resonant_rev_per_sec)
      {
        // Then rev per sec of mandrel motor is in range of resonant frequency
        m_velocity = TWO_PI * config::m_radius * config::m_min_resonant_rev_per_sec;
        c_velocity = m_velocity / tan(config::wrap_angle);
      }
    }
    else
    {
      // Max speed of wind is bounded by max mandrel rev per sec
      c_velocity = m_velocity / tan(config::wrap_angle);
    }

    Mandrel.set_velocity(m_velocity);
    Carriage.set_init_usec_per_step_accel(c_accel_dist, c_velocity);

    // Calculate all possible offset steps given patterns per circuit
    double offset_steps[config::patterns_per_circuit];
    int_type dwell_steps[config::patterns_per_circuit];
    int_type min_viable_dwell_steps = config::m_steps_per_rev;
    for (int_type m = 0; m < config::patterns_per_circuit; ++m)
    {
      offset_steps[m] = (config::m_steps_per_rev * (m + (1.0 / config::circuits_per_layer))) / config::patterns_per_circuit;
      int_type i = (2 * config::mandrel_steps) / (config::m_steps_per_rev + int_type(offset_steps[m]));
      ++i;
      dwell_steps[m] = (((i * config::m_steps_per_rev) + int_type(offset_steps[m])) - (2 * config::mandrel_steps)) / 2;
      if (dwell_steps[m] >= config::min_dwell_steps && dwell_steps[m] < min_viable_dwell_steps)
      {
        min_viable_dwell_steps = dwell_steps[m];
      }
      // Serial.print("For m = ");
      // Serial.print(m);
      // Serial.print(" offset steps = ");
      // Serial.print(offset_steps[m]);
      // Serial.print("\n");
      // Serial.print("For m = ");
      // Serial.print(m);
      // Serial.print(" dwell_steps = ");
      // Serial.print(dwell_steps[m]);
      // Serial.print("\n");
    }

    Serial.print("min_viable_dwell_steps = ");
    Serial.print(min_viable_dwell_steps);
    Serial.print("\n");

    if (min_viable_dwell_steps == config::m_steps_per_rev)
    {
      // Then there are no viable solutions for the given configuration
    }

    Mandrel.set_dwell_steps(min_viable_dwell_steps);
  }

  // Set beginning of stoke Mandrel step count for all circuits
  int_type start_of_pattern_step_count[config::patterns_per_circuit];
  start_of_pattern_step_count[0] = 0;
  for (int i = 1; i < config::patterns_per_circuit; ++i)
  {
    start_of_pattern_step_count[i] = (start_of_pattern_step_count[i - 1] + (2 * config::mandrel_steps) + (2 * Mandrel.get_dwell_steps())) % config::m_steps_per_rev;
  }

  /* Home Carriage - Guarantees Carriage is the correct steps before C_Home_Switch for acceleration profile */
  Carriage.set_velocity(config::c_homing_velocity);
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
  Carriage.set_dwell_flag();

  /*---------------------------------------------------------------------------
                               <3 Axis Filament Wind>
  Main routine which performs a 3 axis filament wind at users specifications defined
  in include/Config.h under CALIBRATIONS FOR USER. Given a user defined wrap angle,
  the carriage and mandrel actuate at a predetermined constant velocity while the
  applicator head rotates as necessary such that the filament is applied parallel
  to the mandrel during a wind.
  ---------------------------------------------------------------------------*/

  int_type strokes = 0;
  int_type layers = 0;
  int_type m_step_count = 0;
  int_type curr_pattern = 0;

  while (layers < config::total_layers)
  {
    if (strokes == (2 * config::circuits_per_layer))
    {
      strokes = 0;
      ++layers;
    }

    long_int_type curr_usec = micros();
    if (curr_usec - Mandrel.get_last_step_time() > Mandrel.get_usec_per_step())
    {
      Mandrel.set_last_step_time(curr_usec);
      Mandrel.step();
      Mandrel.inc_step_count();
      ++m_step_count;
    }

    curr_usec = micros();
    if (curr_usec - Carriage.get_last_step_time() > Carriage.get_usec_per_step())
    {
      if (Carriage.is_dwell_flag_set())
      {
        if (Mandrel.get_step_count() == start_of_pattern_step_count[curr_pattern])
        {
          m_step_count = 0;

          Carriage.clear_dwell_flag();
          Carriage.start_accelerating();
          Carriage.set_last_step_time(curr_usec);

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
          if (!Carriage.is_decelerating())
          {
            Serial.print("For Calibration - Total Mandrel Steps: ");
            Serial.print(m_step_count % config::m_steps_per_rev);
            Serial.print("\n");
          }
        }
        Carriage.set_last_step_time(curr_usec);
        Carriage.step();
      }
    }

    // curr_usec = micros();
    // if (Rotator.is_enabled() && curr_usec - Rotator.get_last_step_time() > Rotator.get_usec_per_step())
    // {
    //   Rotator.set_last_step_time(curr_usec);
    //   Rotator.step();
    //   Rotator.inc_step_count();
    //   Rotator.check_rotation_finished();
    // }

    if (C_Home_Switch.is_rising_edge() || C_Far_Switch.is_rising_edge())
    {
      Carriage.set_decel_flag();
      Carriage.start_decelerating();

      Rotator.flip_dir();
      Rotator.enable();
    }

    if (Carriage.is_decel_flag_set() && !Carriage.is_decelerating())
    {
      Carriage.clear_decel_flag();
      Carriage.flip_dir();
      Carriage.set_dwell_flag();

      for (int i = 0; i < config::patterns_per_circuit; ++i)
      {
        Serial.print("start of pattern at ");
        Serial.print(i);
        Serial.print(" is ");
        Serial.print(start_of_pattern_step_count[i]);
        Serial.print("\n");
      }

      start_of_pattern_step_count[curr_pattern] = (start_of_pattern_step_count[curr_pattern] + config::mandrel_steps + Mandrel.get_dwell_steps()) % config::m_steps_per_rev;

      ++strokes;
      if (strokes % 2 == 0)
      {
        ++curr_pattern;
        if (curr_pattern == config::patterns_per_circuit)
        {
          curr_pattern = 0;
        }
      }
    }

    if (M_Encoder_Switch.is_rising_edge())
    {
      Serial.print("Mandrel step count at rising edge: ");
      Serial.print(Mandrel.get_step_count());
      Serial.print("\n");
      // Button not being properly debounced currently
      if (Mandrel.get_step_count() > (config::m_steps_per_rev / 2))
      {
        Mandrel.clear_step_count();
      }
    }
  }
  return 0;
}

void setup() { main_(); }

void loop() {}
