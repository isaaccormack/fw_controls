# Electrical Overview 
The electronics on board the filament winder can be most easily discussed when divided into high and low voltage systems.

As seen below, the high voltage system is connected by red lines and the low voltage by black lines.
![Electrical Architectural Diagram](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/fw_electrical_architecture.png?raw=true)

# Table of Contents
  * [Low Voltage System](#low-voltage-system)
    * [Microcontroller](#microcontroller)
    * [Limit Switches](#limit-switches)
      * [C_Far_Switch](#c_far_switch)
      * [C_Home_Switch](#c_home_switch)
      * [M_Encoder_Switch](#m_encoder_switch)
      * [Radial_Far_Switch](#radial_far_switch)
      * [Radial_Home_Switch](#radial_home_switch)
      * [Rotator_Switch](#rotator_switch)
    * [Switch Debounce Circuits (SDCs)](#switch-debounce-circuits-sdcs)
    * [Stepper Motor Drivers](#stepper-motor-drivers)
  * [High Voltage System](#high-voltage-system)
    * [Stepper Motors](#stepper-motors)
    * [Stepper Motor Drivers](#stepper-motor-drivers-1)
    * [Power Supply](#power-supply)

## Low Voltage System
The low voltage system consists of all components used to control the machine. Sensors in the form of limit switches are fed into the microcontroller and control signals to the motor drivers are sent out. The low voltage system consists of the microcontroller, limit switches, switch debounce circuits, and stepper motor drivers.

### Microcontroller
The [Arduino Mega 2560 R3](https://store.arduino.cc/usa/mega-2560-r3) microcontroller was chosen to control the machine. The decision for which microcontroller to use was first limited by the desire to develop within the Arduino ecosystem. Among others, the STM32 family of boards was considered, but lack of online help for common bug fixes and the like makes barrier to entry high. The use of Simulink was also considered, but programming in C was preferred due to its ubiquity. The Arduino ecosystem offers tutorials and answers to many FAQs around embedded systems and thus makes itself accessible to the average university student keen to apply their programming knowledge from other domains.

To perform the functions necessary to control the machine, it was found that a 16MHz Arduino board making use of non-performant standard library Arduino functions, like `digitalWrite()`, was acceptable, however, when designing key aspects of the systems software architecture the programmer must still be of course cognizant of the limited CPU speed. The Mega was chosen as it was on hand at the time. The Uno is also a viable choice. Specs for the Mega 2560 R3 are below. ![Arduino Mega 2560 R3 Specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/arduino_mega_2560_r3_specs.png?raw=true)<br>
The board is typically powered by the computer it's connected to, but a standard 5V power supply can be used for longer winds once the software has been uploaded.

### Limit Switches
Limit switches are used to provide feedback to the microcontroller regarding physical positioning of components aboard the filament winder. All switches must be securely fastened with no play as they are used as physical reference points. The movement of these switches during winding will cause inaccuracies of varying magnitudes, from small tolerance losses, to incoherent winding patterns.

All but the M_Encoder_Switch are simple limit switches from Phidgets. The specs are shown below. ![Phidgets Limit Switch Specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/limit_switch_specs.png?raw=true)<br>
The M_Encoder_Switch is a miscellaneous roller limit switch. This is necessary as the switch is in constant contact with a rotating surface.

The purpose of each switch, action taken by control system when depressed, and mechanical configuration for each switch on board the machine is listed below.

#### C_Far_Switch
_Purpose_ - Detect when the carriage has reached its furthest winding position.<br>
_Action_ - The carriage begins decelerating as to change direction.<br>
_Configuration_ - The placement of the switch determines the length of the wind relative to C_Home_Switch. The carriage will reach a stop 0.25 - 1in after the switch is depressed, due to deceleration, depending on how steep or shallow (respectively) the wrap angle is. The switch must be depressed just enough for it to _click_ upon contact with the linear bearing.

#### C_Home_Switch
_Purpose_ - Detect when the carriage has reached its home position.<br>
_Action_ - The carriage begins decelerating. If the wind is over, the carriage will stop, else it will change direction and begin the next pass.<br>
_Configuration_ - The position of this switch determines roughly where the wind will begin as the carriage accelerates for 0.25 - 1in (again, depending on wrap angle) before the switch is released. The switch must be depressed just enough for it to _click_ upon contact with the linear bearing.

#### M_Encoder_Switch
_Purpose_ - Detect the mandrel has completed a single rotation.<br>
_Action_ - Signal to the microcontroller. This signal is used to calibrate the actual position of the mandrel to the position the control system calculates the mandrel to be in, like a cheap linear encoder.<br>
_Configuration_ - It is _crucial_ the switch is mounted securely to the plate with no play and that it stays in the same position between each revolution, for the entire duration of the wind. The key of the mandrel's drive-shaft should depress the switch just enough for it to _click_ upon contact.

#### Radial_Far_Switch
_This switch is used in 4-axis control and therefore its action and configuration are not currently defined, however, suggestions for how to handle these cases are given below._

_Purpose_ - Detect the applicator head has reached its closest position to the mandrel drive shaft. This occurs when the carriage is at either end of the mandrel and the applicator head moves toward the mandrel drive shaft to wrap the filament around the end cap.<br>
_Action_ - It is likely the applicator head would begin decelerating as to come to a stop or change direction.<br>
_Configuration_ - It is likely the switch would be positioned 0.5in from where the applicator head should come to a stop, to allow for deceleration. The stop position would likely be where the end of the applicator head is 0.5in -- or as close as possible -- to the mandrel drive shaft.

#### Radial_Home_Switch
_This switch is used in 4-axis control and therefore its action and configuration are not currently defined, however, suggestions for how to handle these cases are given below._

_Purpose_ - Detect the applicator head has reached its furthest position away from the mandrel. This occurs when the applicator head has completed winding the end cap and moves away from the mandrel drive shaft to wind the mid-section of the mandrel.<br>
_Action_ - It is likely the applicator head would being decelerating as to come to a stop.<br>
_Configuration_ - It is likely the switch would be positioned 0.5in from where the applicator head should come to a stop to allow for deceleration. The stop position would likely be where the end of the applicator head is 0.5in -- or as close as possible -- to the mid-section of the mandrel.

#### Rotator_Switch
This switch is only used to calibrate the machine before a wind. During start-up, the applicator head simply rotates until the switch is depressed, then rotates in the opposite direction `config::r_steps_to_home` steps until the applicator head is positioned vertically -- its home position. 

### Switch Debounce Circuits (SDCs)
A switch debounce circuit (SDC) is used to physically debounce each limit switch. The schematic is given below. ![Switch Debounce Circuit Schematic](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/debounced_pulldown_switch_schematic.png?raw=true)<br>
Note that the switch must be _wired normally open_. This particular circuit was calibrated using a Phidgets limit switch. Other limit switches may require new time constants for ideal behaviour. To determine the time constant for this circuit, the GPIO output was observed under an oscilloscope while resistance and capacitance values were changed. Calibration was complete once the output curve was smoothed enough to resemble the figure below. ![Physically Debounced Switch Comparison](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/figures/switch_debounce_output.png?raw=true)<br>
Physical debouncing is preferred to software debouncing for its simplicity. While software debouncing a switch, it is common to either poll the switch at some small time period (50ms) or use an interrupt to signal when the switch has been pressed. In short, interrupts introduce more complexity to the software, and polling is unreliable. Interrupts can potentially piggyback on each other, starving other functions of the control system, such as stepping the motor (as the control system is single threaded), producing small inaccuracies. Polling introduces a margin of error between the time the switch is detected to have been pressed, and the time it actually was, as the switch is only polled every 50ms or so. It is most reliable to only read the switch at the point in time when the control algorithm can use its value immediately.

### Stepper Motor Drivers
The TB6600 stepper motor driver allows step level control of the stepper motors, meaning, when the +PUL terminal receives a rising edge, the stepper motor will step once. The wiring diagram is given below. ![TB6600 Wiring Diagram](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/TB6600_driver_wiring_schematic.png?raw=true)<br>
The microcontroller periodically sends pulses to the driver to rotate the motor. The +DIR terminal is used to determine the direction of the motors rotation, and the +ENBL terminal keeps the driver enabled. The +ENBL terminal can be tied to 5V. All low voltage negative terminals are tied to a common ground.

## High Voltage System
The high voltage system is the electrical powertrain for the machine. It consists of the power supply, stepper motors, and stepper motor drivers.

### Stepper Motors
Stepper motors were chosen for this application due to their high torque-to-size ratio, high positional precision, and ease of software control. On board the filament winder there are four motors. All but the mandrel motor were chosen to be gearless NEMA 23 stepper motors from [Phidgets](https://www.phidgets.com/), the specs of which are shown below. ![Gearless NEMA 23 Stepper Motor Specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/NEMA_23_carriage_motor_specs.png?raw=true)<br>
The three gearless motors are used for the following:
- _Carriage motor_ - pulls the carriage along linear rails from end-to-end
- _Radial motor_ - pulls the applicator head along linear rails to and fro the mandrel
- _Rotator motor_ - spins the applicator head to apply filament to the mandrel at varying angles

When selecting stepper motors it is important to ensure the motor has enough precision and torque for the application. At 200 steps/rev paired with a small pulley gear, the motors certainly have enough precision. While all three motors have nearly the same torque requirements, the carriage motor has the highest due to its load. This motor was used as a benchmark. Some _rough_ mechanical calculations were done along with simple testing to ensure the gearless NEMA 23 motor would provide enough torque at operational speeds for the carriage motor. It should be noted that, although the holding torque for this motor is very high, its torque decreases with speed so it is important to have the operation speed well defined when spec'ing the motor for torque.

For uniformity in software control, among other things, all four motors aboard the machine would ideally be the same; however, this is not possible as the mandrel motor has much higher torque requirements. Thus, the mandrel motor uses the same gearless NEMA 23 motor as the others, except with a 4.25:1 gearbox, making it a geared NEMA 23 stepper motor. The specs are given below. ![Geared NEMA 23 Stepper Motor Specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/geared_NEMA_23_mandrel_motor_specs.png?raw=true)<br>
The _mandrel motor_ is used to rotate the mandrel at a constant velocity during winding. As mandrels can be large in length and diameter (4 ft. x 12in. max respectively) and made from heavy metals like stainless steel, this motor sees the highest load. Using a 4.25:1 gear reduction ratio enables the mandrel motor to output a higher torque than the gearless motors, but also means the motor has 850 steps/rev (compared to 200 steps/rev of the gearless) and a max RPM of 375. This must be accounted for in the control software.

### Stepper Motor Drivers
The TB6600 stepper motor driver was chosen as it supports 12V motors of 1 - 3A, meaning it could be used to drive all motors. The specs are shown below. ![TB6600 Stepper Driver Specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/TB6600_stepper_driver_specs.png?raw=true)<br>
See the wiring diagram in the low voltage stepper motor driver [section](#stepper-motor-drivers) for the following discussion. The high voltage interfaces to the driver include 12V and GND wires to the power supply and the four coloured wires to the stepper motor. The stepper motor must be wired to the ports of the driver in the order depicted.

### Power Supply
A 12V-30A, or 360W power supply is used since all stepper motors are 12V, and pull ~10A together. The specs are shown below.
![Power Supply Specs](https://github.com/isaaccormack/fw_controls/blob/master/docs/datasheets/12V-30A_power_supply_specs.png?raw=true)<br>
As there are only 3 output ports on the power supply, the two motors on the carriage are wired out of one port. 

There _must_ exist a kill switch between the power supply and the wall outlet as a means to disable power to the high voltage system. This is useful for regular operation and _necessary_ for emergencies.
