# Data Flow Controller
This module is responsible for controlling the flow of EEG Samples between:
* EDF File parser to PC
* ADS1299 Driver to PC
* ADS1299 Driver to EDF File Parser

It shall respond to Data ready events and request the latest data from the relevant source.
