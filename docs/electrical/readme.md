# Architectural Overview 
The electronics on board the filament winder can be most easily discussed when divided into high and low voltage systems.

As seen below, the high voltage system is connected by red lines and the low voltage system by black lines.
![Electrical Architectural Diagram](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/fw_electrical_architecture.png?raw=true)

# Table of Contents
- [High Voltage System](#high-voltage-system)
  * [Power Supply](#power-supply)
  * [Stepper Motors](#stepper-motors)
  * [Stepper Motor Drivers](#stepper-motor-drivers)
- [Low Voltage System](#low-voltage-system)

## High Voltage System
The high voltage system is the electrical powertrain for the machine. It consists of the power supply, stepper motors, and stepper motor drivers (motors and drivers for short).

### Stepper Motors
Stepper motors were chosen for this application due to their high torque-to-size ratio, high positional precision, and ease of software control. On board the filament winder there are four motors. All but the mandrel motor were chosen to be gearless NEMA 23 stepper motors from [Phidgets](https://www.phidgets.com/), the specs of which are shown below. ![Gearless NEMA 23 Stepper Motor](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/NEMA_23_carriage_motor_specs.png)

The three gearless motors are the following:
1. Carrige motor - pulls the carriage along the linear bearings from end-to-end
2. Radial motor - pulls the applicator head aboard the carriage along linear rails toward and away from the mandrel
3. Rotator motor - spins the applicator head to apply filament to the mandrel at varying angles

When selecting the stepper motors it was important to ensure the motor had enough precision and torque for the application. At 200 steps/rev paired with a small pulley gear, the motors easily achieved enough precision. While all three motors have nearly the same torque requirements, the carriage motor must have the highest torque due to its load. This motor was used as a benchmark. Some _rough_ mechanical calculations were done along with simple testing to ensure the gearless NEMA 23 motor would provide enough torque at operational speeds for the carriage motor. It should be noted that although the holding torque for this motor is very high, its torque decreases with speed so it is important to have a well defined operational speed to ensure adequate torque.

For uniformity in software control, among other things, ideally all four motors aboard the machine would be the same; however, this is not possible as the mandrel motor has much higher torque requirements. Thus, for the mandrel motor a geared NEMA 23 stepper motor with 4.25:1 ratio was used, the specs seen below. ![Geared NEMA 23 Stepper Motor](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/geared_NEMA_23_mandrel_motor_specs.png)

As the motor is geared down at a 4.25:1 ratio, it can output a much higher torque but also because of this, the motor has 857 steps / rev (compared to 200 steps / rev of the gearless) and a max RPM of 375. This must be acounted for when being controlled.

Although the mandrel motor is geared, it has many of the same specs as the gearless motor. Most importantly, it is also a 12v motor and pulls at most 2.8A. This means a 12v driver for motors of 1 - 3A could be chosen to drive all the motors, enabling a uniform interface for control. This greatly simplifies the software as all motors can inherit from a base `Motor.h` class which performs fundamental motor control.

### Stepper Motor Drivers
The TB6600 stepper motor driver was chosen as it supports 12v motors of 1 - 3A, meaning it could be used to drive all motors. The specs are shown below. ![TB6600 Stepper Driver]()

### Power Supply
A 12v-30A, or 360W power supply is used. The full specs are shown below.
![Power supply specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/12V-30A_power_supply_specs.png)

As each stepper motor is 12v, and all together they pull ~10A. There are only 3 output ports on the supply, so typically the two stepper mottors on the carraige are wired out of one port. 

There _must_ exist a kill switch between the power supply and the wall outlet as a means to disable power to the high voltage system. This is useful for regular operation and necessary for emergencies.

## Low Voltage System
