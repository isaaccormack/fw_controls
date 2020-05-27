- [Per-Wind Calibration](#per-wind-calibration)
- [Pre-Wind](#pre-wind)
  * [Initialize Carriage and Mandrel Class States](#initialize-carriage-and-mandrel-class-states)
    + [Carriage Acceleration Distance](#carriage-acceleration-distance)
    + [Carriage and Mandrel Velocity](#carriage-and-mandrel-velocity)
    + [Mandrel Dwell Steps](#mandrel-dwell-steps)
  * [Initialize Start of Pattern Step Count](#initialize-start-of-pattern-step-count)
  * [Homing](#homing)
    + [Rotator Head](#rotator-head)
    + [Carriage](#carriage)
- [Winding Algorithm](#winding-algorithm)
  * [Velocity Control](#velocity-control)
  * [Acceleration Control](#acceleration-control)

The strutcure of this doc parallels the execution in which the code is written for ease of understanding.

This doc talks about the theory behind what needs to happen in a 3 axis wind along with how that is implemented in the source code.

Everything in here wrt main.ino, this should be obvious from the introduction but need to give one stating what is going on here
- could make a dependency chart for class initialization parameters, ie. c_accel_dist depends on config::wrap_angle, etc. 
- 

# Per-Wind Calibration
remember to note that config::mandrel_steps must be calibrated before a wind. maybe in the winding section, why this is and how to do this. for why, refer to mandrel dwell steps section

# Pre-Wind
Before the winding algorithm takes place, the carriage, mandrel, and rotator classes must be initialized with parameters calculated at run time, then each motor homed.

## Initialize Carriage and Mandrel Class States
<need to make this more detailed>Important parameters are explained in their own subsection here. Although not all parameters listed are directly used to initialize a motor, all are used indirectly and thus require an explanation.

Although params are not function the format `param(dependent1, dependent2, ...)` will be used etc. to denote that the param is a function of each dependent

### Carriage Acceleration Distance
The _carriage acceleration distance_, `c_accel_dist`, is the distance over which the carriage accelerates and delecrates. The distance depends only on _wrap angle_, `config::wrap_angle`. The acceleration distance is longer for shallower winding angles as the carriage reaches higher speeds. It has been found experimentally that 1 inch is sufficient for the carriage to reach its fastest speed, during a 30 degree wind, and 0.125 inches is enough for the carriage at its slowest speeds during winds close to 90 degrees. The acceleration distance is calculated by linearly interpolating between these two endpoints using the wrap angle.

Using `c_accel_dist` and some parameters related to the carriage motor's gearing, the _total number of steps_ needed for the carriage to travel the acceleration distance, `c_accel_steps`, is calculated.

The acceleration steps are saved into the state of the Carriage object by calling `Carriage.set_total_accel_steps(c_accel_steps)`.

### Carriage and Mandrel Velocity
The _carriage velocity_, `c_velocity`, refers to the linear velocity of the carriage, and the _mandrel velocity_, `m_velocity`, to the tangential velocity of the mandrel. For a 3 axis wind, the carriage and mandrel velocities can be thought of as perpendicular vectors on the 2D plane such that their mangnitudes are related by the tangent of the the _wrap angle_, `config::wrap_angle` as follows:
```
m_velocity / c_velocity = tan(config::wrap_angle)                           (1)
```
Clearly, for a wrap angle greater than 45 degrees, the mandrel velocity is higher than the carriage velocity. Given each motor has a max speed, there exists a _critical wrap angle_, `crit_wrap_angle`, such that:
```
crit_wrap_angle = atan(m_max_velocity / c_max_velocity)                     (2)
```
Only one motor can be run at its max speed at any wrap angle other than the critical wrap angle. For wrap angles less than the critical wrap angle, the carriage motor operates at its max speed, and for angles greater, the mandrel motor.

In the source code, each motor is initially set to their max value. For the mandrel, this is the cirumference of the mandrel multiplied by the the experimentally determined _max angular speed_,   `config::m_max_rev_per_sec`. Beyond this speed, the mandrel motor lacks sufficient torque. The carriage motors max speed depends only on its acceleration distance. A max speed of 8.25 - 10 in/s given an acceleration distance from 0.125 - 1 inch is possible. Although the carriage can reach speeds of up to 18 in/s, this requires an acceleration distance longer than 1 inch and adds unnecessary wear to the machine. 
 
The critical angle is then calculated using equation 2. If the desired wrap angle is _less_ than the critical wrap angle the carriage velocity is left at max and the mandrel velocity is calculated using equation 1. The mandrel motor experiences resonant frequencies between `config::m_min_resonant_rev_per_sec` and `config::m_max_resonant_rev_per_sec`, in which range, the motor cannot operate. If the mandrel motor is found to be set to a speed in this range, it is instead set to a velocity at the lower bound. The carriage velocity is then recalculated using equation 1.

If the desired wrap angle is _greater_ than the critical wrap angle the mandrel velocity is left at max and the carriage motors speed is calculated using equation 1.
 
The mandrel velocity is saved into the state of the Mandrel object by calling `Mandrel.set_velocity(m_velocity)`. The acceleration state of the Carriage object is set by calling `Carriage.set_init_usec_per_step_accel(c_accel_dist, c_velocity)`. As `Carriage.set_total_accel_steps(...)` has already been called, the carriages acceleration state is now fully initialized.

where to talk about set velocity and carriage

Finally, the rotator head velocity is set using the carriages acceleration distance and velocity by calling `Rotator.set_rev_per_sec(c_accel_dist, c_velocity)`. The logic behind this is discussed in the Rotator section of Classes.md.

### Mandrel Dwell Steps
The _mandrel dwell steps_, `dwell_steps`, are the number of steps the mandrel must spin  while the carriage rests at either end of the mandrel, to complete a wind with the desired _patterns per circuit_, `config::patterns_per_circuit`. The patterns per circuit are the number of distinct filament paths wound around the mandrel. 

Patterns per circuit is best explained by example. Consider the case where the carriage takes a pass from the home end to the far end, waits for the mandrel to rotate some amount of _dwell steps_, then takes a pass back to the home end and waits for an equal number of steps. A _pass_, as mentioned below, is defined to be a trip the carriage takes from one rest position at an end of the mandrel to another, during a wind.

When the carriage goes to take its next pass in a _single pattern per circuit_ wind, the filament would be layed exactly over the filament from the first pass. Thus, one distinct filament path exists. Of course, during a filament wind, the layers are offset from each other so the new filament would only partly overlap the previous layer such that the entire mandrel will eventually be covered in a filament.

In a _two patterns per circuit_ wind, the carriage would lay another distinct filament path on its passes from the home to far end and back, before overlapping the filament from the first pass on its next pass. Thus, two distinct filament paths exist. The controls for this machine support up to 6 patterns per circuit.

The dwell steps depend on _passes per layer_, `config::passes_per_layer`. This is the number of passes the carriage needs to take to cover the mandrel in exactly one layer of filament. The calculation to find passes per layer is described in the config.h section of classes.md.

Half the _offset steps_, `offset_steps / 2`, is the number of steps the mandrel would have to spin while the carriage rests at each end to complete the desired _patterns per circuit_ and yeild the proper offset between filament strands between circuits _iff_ each time the carriage arrives at the end of the mandrel, the mandrel is in the same position as it was when the carriage left the other end (ie. the mandrel _doesn't_ rotate when the carriage makes a pass). Clearly, this is almost never the case, so _offset steps_ is only an intermediate value calculated in the process of finding _dwell steps_. Half the offset steps is refered to here as the carriage will always rest for an equal number of steps at each end, although the _offset steps_ is calculated to be the number of steps the carriage must rest for at two ends, or between two subsequent passes leaving the home end.

The _dwell steps_ account for the mandrels rotation while the carriage makes a pass. It does this by considering _mandrel steps_, `config::mandrel_steps`, the number of steps the mandrel rotates during a pass. _Mandrel steps_ exists in the range [0, `config::mandrel_steps_per_rev`] and must be recalibrated each time any wind parameters are changed. More details about the calibration routine [here](). The _dwell steps_ are calculated by finding the difference between the number of steps the mandrel rotates over two passes and the number of _offset steps_ the mandrel must take between two passes to produce the desired patterns per circuit. This gives the number of steps the mandrel must rotate while the carriage waits, over two subsequent passes. However, this number can be small or even negative if `2*config::mandrel_steps > offset_steps`, so a scaling factor `i` is used to account for this. `i * config::mandrel_steps_per_rev` is a number of steps which can be added to the calculated dwell steps while preserving the patterns per circuit. `i` is a fraction which relates the offset between the mandrels position after two passes and the desired position. The calculated _dwell steps_ are then sufficiently large to ensure the filament doesn't slip off the end during a wind with a small wrap angle. Ideally in this case, the dwell steps would be larger than `config::mandrel_steps_per_rev / 2`. 

Finally, the dwell steps are then saved into the mandrels state by calling `Mandrel.set_dwell_steps(dwell_steps)`.

## Initialize Start of Pattern Step Count
The _start of pattern step count_, `start_of_pattern_step_count[]`, is an array which holds the step count the mandrel must reach before the carriage begins its pass. This ensures that each time the carriage leaves for a pass, it is starting from the correct position. 

Each index maps to a pattern, and because the ps


the mandrel must be at for the carriage to begin its next pattern. leave its .. for the 

holds the mandrel step the mandrel must be at when the carriage leaves the home end to begin a pattern. Each index holds the position for the respective pattern. Using an array of _start of pattern step count_ is simply a way to ensure that each pattern begins from the correct position. This ensures each strand of filament is always positioned the right offset from the one on the circuit below it, necessary for the machines tight tolerances, and that errors don't stack between passes.

Say for instance the mandrel skipped a couple steps during a pass. If the carriage simply waited for _dwell steps_ at the end of the pass before doing the next, the error would propogate for the remainder of the wind leaving a large gap between where the filament should be and where it is now being layed. By using the array of _start of pattern step count_, the carriage will wait until the mandrel is in the correct position, even if it is more of less time than the actual _dwell steps_ to ensure the above case does not happen. This works as the mandrel step count stored in _start of pattern step count_ is calculated using dwell steps.. put this here or below?

The _mandrel steps_ are pretty reliable as they are reset every time the mandrel makes a rotation, via the `Mandrel_Switch`. This means that although the mandrel could skip steps in one revolution, completing only 840 steps while the software has told (and thinks) it did 850 steps, this error will be cleared as the mandrel steps are 0'd for the next revolution. Thus the position where the software think the mandrel is and where the mandrel is is recalibrated every revolution. This mitigates error propogation for obvious reasons. 

... maybe only need to talk about initialization here, talk about what this is and how its updated in the controls, ie in the winding section there can be a section on "Start of pattern step count"

regardless of whether some ... An alternate approach of waiting for _dwell steps_ at each end may allow errors to stack, such that if the 

holds the position the mandrel must be in keeps track of the mandrel step count the mandrel must be at before the carriage begins the pattern.


The _start of pattern step count_, `start_of_pattern_step_count[]`, is used to ensure the mandrel is correctly positioned when the carriage beings its first pass for each pattern. As two passes (from the home to far end and back) are needed to complete a pattern, this value is checked when the used when the ma

is an array of mandrel step counts which the 
why
how


## Homing
Before winding can take place, the rotator head and carriage must be homed.

It should be noted here that the mandrel need not be homed as ... figure out why...
maybe this -> the mandrel position is only used when the mandrel approaches the end of a wind, and since for all...
or its actually a known defect and it needs to be homed but for all the winds we have done this wasnt seen since the wind was long enough for the mandrel to rotate at least once
the idea is that you count your mandrel steps from home position, but it the mandrel hasn't already rotated once then we aren't starting from an accurate 0 steps, ie. 0 steps is wherever the mandrel started from, not right after the limit switch

### Rotator Head
The rotator head must be homed to its 0 degree position, which is where the filament toe is applied perpendicular to the mandrel, as it would be during a 90 degree wind.  To do this, the rotator head is rotated at a constant velocity from whatever position it may be in until it depresses the Rotator_Switch. The rotator head then changes direction and rotates for `config::r_steps_to_home`, until it reaches its home position. The _steps to home_ are found experimentally and vary with the Rotator_Switch position. It is imporant to ensure the direction of the rotator head is set such that it rotates toward the Rotator_Switch.

Lastly and critically, `Rotator.set_rev_per_sec(c_accel_dist, c_velocity)` is called to set the velocity for the rotator head during the wind. This overrides the value of `config::r_homing_rev_per_sec` set previously for homing.

### Carriage
The carriage must be positioned the _total acceleration steps_ before the C_Home_Switch. To do this, the carriage is moved at a constant velocity from whatever position it may be in until it depresses the C_Home_Switch and moves `c_accel_steps` past it. It is imporant to ensure the direction of the carriage is set such that it moves toward its home position. Once the carriage is homed, 

It may be confusing that `Carriage.set_velocity(config::c_homing_velocity)` is called _after_ the carriage's class state has been initialized for a wind; however, only state related to acceleration was initialized before. The set of state variables in the Carriage class which govern acceleration and velocity control are mutually exclusive. When the carriage is controlled with acceleration, as it is during a wind, the velocity the carriage will reach is fully defined by its acceleration state. Here however, the carriage is controlled with constant velocity, so the velocity state variables must be defined by calling `set_velocity(...)`.

The mechanics of how the carriage is controlled at constant velocity and via acceleration are discussed in classes.md/Carriage. -- change this

# Winding Algorithm
The 3-axis winding algorithm is relatively simple once initializtion is complete. During a wind, the mandrel rotates at a constant velocity while the carriage moves from end to end, and the rotator head pivots to apply filament to the mandrel. This continues until the mandrel has been wrapped with the desired layers of filament, `config::total_layers`.



The software works by polling for events, then taking the actions 

## Mandrel Control

## Carriage Control



To give an overview, th


here we are going to talk about how the carriage is controlled with accel / decel, referencing http://annals.fih.upt.ro/pdf-full/2013/ANNALS-2013-3-06.pdf.

First we must discuss the most basic mechanics of how the motors are controlled.



Where to talk about deal with why rotator head moves how much it does during a 3 axis wind, not implementation details but actually why this is the case, seems like it should either be in this doc (which kind of just outlines the code) or another doc called control fundamentals, which takes an even higher level look at what exactly is going on during a 3 axis wind, where as this more talks about how I implement it (or that can be worked into here with description sections, ie. talking about how how the constant velocity vs acceleration word, BUT these are sort of fundamentally different as the latter doesn't require a high level description (people just get it), but the former is not obvious, but its also not a huge point and could quickly be talked over, yeah maybe do that in the section which talks about rotator head in this doc in wind controls)
 

## Velocity Control
As in the mandrel and rotator during winding and the carriage during homing.

## Acceleration Control
As in the carriage during winding. Discuss how this "block" of logic works to accelerate the carriage.
