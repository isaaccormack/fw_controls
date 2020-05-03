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
The high voltage system is the electrical powertrain for the machine. It consists of the power supply, stepper motors, and stepper motor drivers (drivers for short).

### Stepper Motors
Stepper motors were chosen for this application due to their high torque-to-size ratio, high positional precision, and ease of control via software. On board the filament winder there are four motors. All but the mandrel motor were chosen to be NEMA 23 stepper motors from [Phidgets](https://www.phidgets.com/), the specs of which are shown below. ![Gearless NEMA 23 Stepper Motor from Phidgets](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/NEMA_23_carriage_motor_specs.png)

<talk about specifics to the gearless motors here, ie. how they are suitable for their applications, bring up the specific for each one, talk about gear ratios and precision requirements and the torque of the motor to verify decision>

<then talk about the mandrel motor specs and why mandrel motor was used with diagram>

<finally talk about how using all the same motors with drivers enables simple control, replacement of parts and whatever else>



Using the same motors for three applications enables the same driver :
1. Uniform control
2. Uniform drivers

On board the filament winder there are four motors. All but the mandrel motor were chosen to be NEMA For this machine, using motors which all use the same voltage was desirable.
The limiting factor on the stepper motors is their torque. For the mechanical application of the gearless motors (pulling loads along linear bearings), these motors were adequate. This was determined through some _rough_ mechanical calculations and simple testing. It should be noted that the faster the motors are spun, the less torque they offer (on an inverse? logarithmic curve), so specing the motors for the gear ratios and operational speeds of the machine is important. Of course the gear ratios also play into the precision which can be achieved from the machine, but pairing the stepper motors 200 step / rev resolution with near 1:1 gear ratios (for instance 

high positional control for the carriages was acheived by using a small gear, this worked because there wasn't much load on the motor since linear bearings

before handwhich determine how fast

This section will discuss only the electrical aspects of the stepper motors, consult the software readme for detail on their control. 

### Power Supply
A 12v-30A, or 360W power supply is used. The full specs are shown below.
![Power supply specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/12V-30A_power_supply_specs.png)

As each stepper motor is 12v, and all together they pull ~10A. There are only 3 output ports on the supply, so typically the two stepper mottors on the carraige are wired out of one port. 

There _must_ exist a kill switch between the power supply and the wall outlet as a means to disable power to the high voltage system. This is useful for regular operation and necessary for emergencies.

### Stepper Motor Drivers

## Low Voltage System
