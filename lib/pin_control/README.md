# Pin Control
This module provides a layer of abstraction around the already abstracted Arduino GPIO functions.

The main purpose of this is to provide application specific functions e.g. "SetHeartbeatLedState" rather than referencing pin numbers in the modules which need to set GPIO pins.

## Inputs
This module allows getting the level of the following input pins:

* SD Card's Card detect (CD) pin
* SD Card's Write Protect (WP) pin
* ADS1299's Data ready pin

## Outputs
This module allows setting of the following output pins:

* Heartbeat LED
* ADS1299's Reset pin
* ADS1299's Chip Select pin
* SD Card's Chip Select pin

## Interrupts
In order to prompty respond to the ADS1299's data ready input changing state, an interrupt is set up to trigger on the falling edge of this pin and consequently signal the ADS1299 data ready event in the event handler class.
