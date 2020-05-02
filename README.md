# fw_controls
A collection of code used to control the UVic Rocketry's Filament Winding Machine.

[![Link to UVR's Filament Winder Video](https://img.youtube.com/vi/rWipQq5OikY/0.jpg)](https://www.youtube.com/watch?v=rWipQq5OikY "UVR's Filament Winder Video")

## Getting Started
Prior to winding:
1. A suitable board must be chosen as the controller
2. The software development environment must be setup on the operators computer
3. The high and low voltage systems must be wired correctly on the winder

### Controller Selection
Any board which Arduino complies for and meets the following requirements can be used:
1. At least 8MHz clock speed
2. At least 13 digital I/O pins

An Arudino Mega 2560 has worked previously. 

### Wiring
** note that it is really important to check and double check the wires

### Development Environment ** change name
This software has been designed to run with Arduino.
1. Install [Arduino](https://www.arduino.cc/en/main/software)
2. Clone this repo `git clone https://github.com/isaaccormack/fw_controls.git`
3. Open `fw_controls/main/main.ino` in the Arduino IDE
4. Plug the Arduino into the computer
5. Select the board being used under `Tools/Board: {$board}`
6. Select the port connecting to the board under `Tools/Port: {$port}`
7. Upload the code!

Although the Arduino IDE is sufficient for building/uploading the software, using VS Code with Microsoft's Arduino Extension is recommended for developing.

To do this:
1. Install [VS Code](https://code.visualstudio.com/)
2. Install the [Microsoft VS Code Arduino Extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) ^v0.3.0
3. Configure the extension in VS Code. [This](https://medium.com/home-wireless/use-visual-studio-code-for-arduino-2d0cf4c1760b) Medium article is really helpful to get your feet on the ground. You _may_ also need to configure:<br/>
a. `Arduino: Path` An error which states "Cannot find Arduino IDE" may arise. In this case, under VS Code settings, navigate to `Extensions/Arduino configurations` and enter the path where Arduino has been downloaded under `Arduino: Path`, on Mac this was `/Users/{$username}/Downloads`<br/>
4. Select the `<Board Type>` and `<Serial Port>` in the blue control bar at the bottom of the editor
5. Navigate to `main/main.ino` and click on the button labeled `Arduino: Upload` in the top-right corner


////////////////////////////////////////////////
It can be run either through the Arduino IDE, or with the Microsoft Arduino Extension in VS Code. For simply running the software, the Arduino IDE is sufficient; however, developing in VS Code is highly recommended. Either way, download [Arduino](https://www.arduino.cc/en/main/software).

For running the software -> Use Arduio IDE

1. Download [Arduino](https://www.arduino.cc/en/main/software)
2. *Optionally, For Development* <br/>
a. Install [Visual Studio Code](https://code.visualstudio.com/) and the [Microsoft VS Code Arduino Extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) ^v0.3.0<br/>
b. Follow this [tutorial](https://medium.com/home-wireless/use-visual-studio-code-for-arduino-2d0cf4c1760b) to configure the extension in VS Code<br/>


// there are two parts to this, the software, electrical and hardware
// for software you have set up the develop environemnt 

// for hardware you must meet the hardware requirements defined below, then wire the board to the winder following the wiring // diagram in docs 

### Software
This software has been designed to run in the Arduino ecosystem. 
1. Download [Arduino](https://www.arduino.cc/en/main/software)
2. 

This software is designed to be run with either the [Arduino IDE](https://www.arduino.cc/en/main/software) or with [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) for [Visual Studio Code](https://code.visualstudio.com/). 

## Environment Setup
This software is designed to be run with either the [Arduino IDE](https://www.arduino.cc/en/main/software) or with the [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) for [Visual Studio Code](https://code.visualstudio.com/). Note the Arduino IDE must be installed to use the Arduino extension for VS Code. It is recommended for non-developers that the software be run with the Arduino IDE as further configurations are required to develop in VS Code as described [here](https://medium.com/home-wireless/use-visual-studio-code-for-arduino-2d0cf4c1760b).

## Controller Requirements
 - Arduino compatible microcontroller (Arudino Mega 2560 recommended)
 - At least 8MHz clock speed
 - At least 6 digital I/O pins

## Usage
To perform a 2 axis filament wind, upload `main/main.ino` to the controller. Common calibratible parameters changed on a wind-by-wind basis such as wrap angle and mandrel radius can be modified in `config.h`. Constants such as I/O pins and phsyical parameters can also be changed there.

## Tests
Various unit and system tests are provided as regression tests to validate changes made to the software over time. These tests also serve to validate the configuration of the physical machine after any modifications have been made. The tests found in `tests/` must be run manually due to the physical nature of the testing. For each test these is a write up giving backgound information, expected results and last executed results along with the accompanying test code. These tests cover basic functionality such as positional and velocity control of motors, basic switch sanity and more. System tests can be found in `tests/system_tests/`. Theses tests validate `main.ino` by successively testing functionality with greater dependencies until all major components of the system are covered by tests.
