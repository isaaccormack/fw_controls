# User Manual

The 3-Axis filament winding machine is able to wind on the angles within [30, 80] degrees. For angles > 80 degrees, a special
case of the 3-Axis winding algorithm, called a hoop wind, should be employed. (Software to be developed, details in ...).

## Physical Configuration
First start by adjusting the carriages limit switches to set the start and end position of the wind. For testing, duct tape was
used to position the limit switches on the wood blocks. This has proven to work well enough to support multiple layer winds, a
more permanent solution may envolve wood screws. The wooden blocks themselves can be unbolted from the chassis and moved
accordingly. Ensure the limit switch makes good contact with the linear bearing such that it is unlikely for the linear bearing
to move past the limit switch without fully depressing it. <2 imgs here, 1 for layout from above, 1 is good contact>

Ensure that the mandrel limit switch is adequately depressed by the key on the mandrel shaft and that it is secure such
that it will be consistantly depressed in the same location every mandrel revolution.

Ensure that the limit switch on the applicator head contacts the applicator head upon rotation. 

Finally, tension the carriage, mandrel, and rotator head timing belts if necessary. 

## Software Configuration
Open the user configuration file, main/include/Config.h, and navigate to the CALIBRATIONS FOR USER section. First input the
desired wrap angle for the wind as the value of deg_wrap_angle. Next input the mandrel's radius. The easiest way to find the
radius of the mandrel being wound is to measure the cirumference of the mandrel, the calculate the radius by diving that number
by 2*pi. This is a crucial value in the control algorithm so precision is necessary (3 decimal places on this value is ideal).
Next, measure the width of the filament and input to at least 2 decimal places. Input a desired percentage, in the range [0.0, 1.0]
of which the strands of filament should overlap. Specify the number of layers the wind should be. A layer is defined as the
point at which the mandrel has been completely covered in filament once. Input the number of patterns per circuit. 

Finally, the mandrel_steps must be set. The mandrel steps is the total number of steps the mandrel makes as the carriage moves
from the rest at the home end to the far end, modulus the mandrels steps per revolution. To find this number, first navigate to
main/main.ino and uncomment the code which prints this value to the serial output (around line 185). Upload the code to the
filament winding machines control board and ensure the carriage is able to move and that neither limit switches are already depressed.
Open the Arduino serial monitor, flip the breaker switch allowing power to the machine and wait for the carriage to do at least
4 passes. You should see output such as <> on the serial monitor. Take the average of these numbers and use that as the value
for mandrel_steps. If the value is inconsistent, ensure the carriages limit switches are securly fastened, the carriages timing
belt is tight, and that neither of timing belt pulley for the carriage or mandrel are slipping on the motor shaft. This value must
be updated every time the winding angle is changed or the position of the carriages limit switches are moved.

## For Winding Angles > 65 Degrees
Ensure that the space between strands of filament between sucessive circuits is the same as that specified in the config file.
Due to inaccuracies of this open control system, there is a tendency for spacing to be less than that desired (by up to 1/8").
To compensate for this, repeatedly define the filament width as slightly larger than the actual width in the config file and 
test until the correct physical width is found.

## Gotcha's
- Ensure the mandrel's radius is used, not the diameter
- Enter the winding angle and fix the limit switch positions before running the calibration routine to find mandrel_steps.
  Remember to re-upload the code to the arduino with this updated value before doing a wind.
