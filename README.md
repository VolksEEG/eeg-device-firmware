# Initial Prototype EEG Data Acquisition firmware.
This is the development repository for the initial prototype of the VolksEEG Data Acquisition firmware.

Written in/using Arduino for the NRF52840 Feather sense development board from Adafruit.

The firmware uses an event system to trigger changes in the state of the libraries and the capturing of data from the EEG sources (ADS1299 or EDF File on an SD Card).

Where a library or class is required by another library or class, a pointer to the dependency class shall be passed into the dependent classes constructor to facilitate unit testing classes.

## src 
The src folder contains the main.cpp file which holds the Arduino "setup" and "loop" functions, all libraries are instantiated in the "setup" function and events/timers are processed in the loop function. 

The src folder also contains ArduinoMock.h which provides empty mocks for Arduino specific functions to allow the unit tests to build on the native platform.

## lib
The majority of the functionality is performed by libraries in the lib folder. 

each library has it's own folder and has it's own README document detailing it's purpose.

## test
All tests are defined in library specific sub folders in the test folder.

To facilitate unit testing, where Arduino specific functions are used, the header file ArduinoMock (defined in the src folder) is included, in place of the Arduino specific header file for tests.
