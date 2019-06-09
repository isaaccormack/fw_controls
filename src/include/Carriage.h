#ifndef CARRIAGE_H
#define CARRIAGE_H

#include "Config.h"
#include "Motor.h"

class Carriage : public Motor
{
public:
  /* Implicitly construct motor with config variables */
  Carriage() : Motor(LED_BUILTIN,
                     config::carriage_dir_pin,
                     config::carriage_pulley_pitch,
                     config::carriage_num_pulley_teeth) {}
};

#endif