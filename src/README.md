# Initial Prototype EEG Data Acquisition firmware.
This is the development repository for the initial prototype of the VolksEEG Data Acquisition firmware.

Written in/using Arduino for the NRF52840 Feather sense development board from Adafruit.

The majority of the functionality is performed by libraries in the lib folder. 

The firmware uses an event system to trigger changes in the state of the libraries and the capturing of data from the EEG sources (ADS1299 or EDF File on an SD Card).

## Unit testing
To facilitate unit testing, where Arduino specific functions are used, the header file ArduinoMock is included, in place of the Arduino specific header file for tests.

All tests are defined in the test folder.