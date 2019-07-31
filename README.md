# fw_controls
A collection of code used to control the UVic Rocketry's Filament Winding Machine

## Environment Setup
This software is designed to be run with either the [Arduino IDE](https://www.arduino.cc/en/main/software) or with [Arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino) for [Visual Studio Code](https://code.visualstudio.com/). Note the Arduino IDE must be installed to use the Arduino extension for VS Code. It is recommended for non-developers that the software be run with the Arduino IDE as further configurations are required to develop in VS Code as described [here](https://medium.com/home-wireless/use-visual-studio-code-for-arduino-2d0cf4c1760b).

## Controller Requirements
 - Arduino compatible microcontroller (Arudino Mega 2560 is used)
 - At least 8MHz clock speed
 - At least 6 digital I/O pins

## Usage
To perform a 2 axis filament wind, upload `main/main.ino` to the controller. Common calibratible parameters changed on a wind-by-wind basis such as wrap angle and mandrel radius can be modified in `config.h`. Constants such as I/O pins and phsyical parameters can also be changed there.

## Tests
Various unit and system tests are provided as regression tests to validate changes made to the software over time. These tests also serve to validate the configuration of the physical machine after any modifications have been made. The tests found in `tests/` must be run manually due to the physical nature of the testing. For each test these is a write up giving backgound information, expected results and last executed results along with the accompanying test code. These tests cover basic functionality such as positional and velocity control of motors, basic switch sanity and more. System tests can be found in `tests/system_tests/`. Theses tests validate `main.ino` by successively testing functionality with greater dependencies until all major components of the system are covered by tests.
