# Data Flow Controller
This module is responsible for controlling the flow of EEG Samples between:
* EDF File parser to PC
* ADS1299 Driver to PC
* ADS1299 Driver to EDF File Parser

It shall respond to Data ready events and request the latest data from the relevant source.

The instantiator can set the primary and secondary data producers and consumers, and then select which ones to use via publicly accessible functions. 

## Inheritance
This class inherits from the following classes:
### CanProcessEvents
This class overrides the "ProcessEvent" function in order to respond to data ready events.
when it recieves an expected event (based on the set producer) it shall get the latest sample from the current producer and pass it onto the current consumer.

### EegDataProducer, EegDataConsumer
This class inherits from these two classes, purely so that, should the producers / consumers not be set, this class can act as both and effectively do nothing quitely.
