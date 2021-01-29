# fw_controls
A collection of code used to control the UVic Rocketry's Filament Winding Machine.

## Winding Video
[![filament winder video](http://img.youtube.com/vi/ra9vlA5Vj_g/0.jpg)](http://www.youtube.com/watch?v=ra9vlA5Vj_g "Filament Winder Montage") <br/>
A short montage showcasing the machine, filament winds produced, and some trial runs with various winding patterns.

## Getting Started
Prior to winding:
1. A suitable microcontroller must be chosen
2. The filament winders low and high voltage systems must be wired correctly
3. The software must be uploaded to the microcontroller

### Microcontroller Selection
Any board which Arduino complies for and meets the following requirements can be used:
1. At least 8MHz clock speed
2. At least 13 digital I/O pins

An Arudino Mega 2560 has worked previously. 

### Wiring
The filament winder contains both high and low voltage systems. Please refer to the [electrical architecture](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/electrical_architecture.png) diagram for the following discussion. Below the procedure for ensuring both system are correctly wired is illustrated.

The high voltage system consists of the power supply, stepper motors, and high voltage end-points on the stepper drivers. These are the devices connected by red lines in the architecture diagram. Consult the [stepper driver](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/TB6600_driver_wiring_schematic.png) wiring diagram for wiring connections. The high voltage system should be enabled/disabled via a breaker switch between the power supply and the AC wall outlet.

The low voltage system consists of the microcontroller, switch debounce circuits (SDC), switches, and low voltage end-points on the stepper drivers. These are the devices connected by black lines in the architecture diagram. It is worth mentioning here that SDCs must be used with all switches to provide physical debouncing. This solution was chosen over software debouncing to simplify the software. Each SDC must be wired according to the diagram [here](https://github.com/isaaccormack/fw_controls/blob/master/docs/electrical/schematics/debounced_pulldown_switch_schematic.png).

To wire the low voltage system, all of the switches and drivers must be connected to the Arduino as shown in the architecture diagram. It is important that the devices are wired into the correct GPIO pin of the Arduino as defined in `main/include/Config.h` under I/O PINS.

For troubleshooting electrical connections, it is recommended to check the stepper driver and power supply terminals first, as these can come lose over time.

### Uploading to Microcontroller / Developing
This software has been designed to run with Arduino.
1. Install [Arduino](https://www.arduino.cc/en/main/software)
2. Clone this repo `git clone https://github.com/isaaccormack/fw_controls.git`
3. Open `fw_controls/main/main.ino` in the Arduino IDE
4. Plug the Arduino into the computer
5. Select the board being used under `Tools/Board: {$board}`
6. Select the port connecting to the board under `Tools/Port: {$port}`
7. Upload the code!

Between winds, calibratible parameters such as the wrap angle and mandrel radius can be changed in `main/include/Config.h`. Once the changes are saved, they can be uploaded to the board.


Although the Arduino IDE is sufficient for building and uploading, using VS Code with Microsoft's Arduino Extension is recommended for developing.

To do this:
1. Install [VS Code](https://code.visualstudio.com/)
2. Install the [Microsoft VS Code Arduino Extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) ^v0.3.0
3. Configure the extension in VS Code. [This](https://medium.com/home-wireless/use-visual-studio-code-for-arduino-2d0cf4c1760b) Medium article is really helpful to get your feet on the ground. You _may_ also need to configure:<br/>
a. `Arduino: Path` An error which states "Cannot find Arduino IDE" may arise. In this case, under VS Code settings, navigate to `Extensions/Arduino configurations` and enter the path where Arduino has been downloaded under `Arduino: Path`, on Mac this was `/Users/{$username}/Downloads`<br/>
4. Select the board and port being used by clicking the `<Board Type>` and `<Serial Port>` buttons in the blue control bar at the bottom of the editor
5. Navigate to `main/main.ino` and click on the button labeled `Arduino: Upload` in the top-right corner

## Usage
To perform a 2 axis filament wind, upload `main/main.ino` to the controller. Common calibratible parameters changed on a wind-by-wind basis such as wrap angle and mandrel radius can be modified in `config.h`. Constants such as I/O pins and phsyical parameters can also be changed there.

## Tests
Various unit and system tests are provided as regression tests to validate changes made to the software over time. These tests also serve to validate the configuration of the physical machine after any modifications have been made. The tests found in `tests/` must be run manually due to the physical nature of the testing. For each test these is a write up giving backgound information, expected results and last executed results along with the accompanying test code. These tests cover basic functionality such as positional and velocity control of motors, basic switch sanity and more. System tests can be found in `tests/system_tests/`. Theses tests validate `main.ino` by successively testing functionality with greater dependencies until all major components of the system are covered by tests.
