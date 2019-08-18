/* MUST GIVE FULL PATH OF INCLUDES -- WILL NEED TO CHANGE PER DEVELOPER */
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Carriage.h"
#include "/Users/i/Documents/Arduino/fw_controls/main/include/Config.h"

/* Calibration Procedure: Calibrate acceleration profile of carriage to ensure that correct velocity is 
                          reached at end of profile.

  Setup: Ensure that the arduino.json file in the .vscode folder is loading this test file 
         by setting: "sketch": "docs/calibration_procedures/carriage_acceleration/carriage_acceleration.ino"

  Unit Under Calibration: Carriage::init_usec_per_step_ (Private member variable)

  Background: The acceleration algorithm relies on a 3rd order taylor series approximation to lower its
              computational complexity. Specifically, additions and subtractions can be performed to change velocity
              compared to alternate methods which use square roots. The acceleration algorithm uses the length of 
              time of the first step in the acceleration (from rest) to set the acceleration (as this value is
              recursively used to set the length for the next step). Due to the approximation made by the taylor
              series, the length of the first step must be calibrated to yield an accurate carriage velocity at
              the end of the acceleration profile (there are no checks in place to ensure that carriage velocity
              is correct after acceleration takes place ie. machine assumed to be calibrated correctly [this is done
              for efficiency]).

  Procedure:
    1. Upload this .ino file to arduino
    2. Open the serial monitor
    3. Adjust the value of the floating point constant at the beginning of the defintion of Carriage::init_usec_per_step_
       such that the experimental carriage velocity at the end of acceleration is within 10 us of the expected value.
       Typically values for the constant will exist in the range [0.94, 1.06] for shorter to longer acceleration
       distances respectively. 
 */

typedef config::int_type int_type;
typedef config::long_int_type long_int_type;

int calibrate_carriage_acceleration_()
{
  Serial.begin(115200);
  Carriage Carriage(config::carriage_step_pin, config::carriage_dir_pin);

  Carriage.set_velocity(Carriage.get_usec_per_step());
  Serial.print("\nCalibration Parameters\n");
  Serial.print("Acceleration distance: ");
  Serial.print(config::carriage_accel_dist);
  Serial.print("\n");
  Serial.print("Carriage velocity: ");
  Serial.print(config::carriage_velocity);
  Serial.print("\n\n");

  Carriage.start_accelerating();
  for (int_type step_count = 0; step_count < config::carriage_accel_steps; ++step_count)
  {
    Carriage.set_next_usec_per_step_accel();
  }

  Serial.print("Carriage velocity at end of acceleration: ");
  Serial.print(Carriage.get_usec_per_step());
  Serial.print("\n");

  Carriage.set_velocity(config::carriage_velocity);
  Serial.print("Expected carriage velocity: ");
  Serial.print(Carriage.get_usec_per_step());
  Serial.print("\n");

  return 0;
}

void setup() { calibrate_carriage_acceleration_(); }

void loop() {}
