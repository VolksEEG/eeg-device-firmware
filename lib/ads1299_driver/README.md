# ADS1299 Driver
## Overview
This driver consists of both a high level driver which exposes the following abstracted functionality:
* Starting / Stopping continuous data capture.

And will expose the following functionality in future:
* Retrieving the number of available channels
* Configuring the input channels.
* Enabling / disabling current sources.
* Setting / Getting GPIO levels.

And a low level driver which is called by the high level driver to set the appropriate register values in the ADS1299.

**Note** The low level driver does not and will not expose the complete functionality of the ADS1299 Registers, and will not be the most optimised solution e.g. writing multiple sequential registers at the same time is not intended to be implemented.

## Inheritance 
The high level "ads1299Driver" class inherits from the EEGDataProducer base class and overrides the "GetLatestSample" function. 

This allows the DataFlowController class to use it as one of it's possible sources of EMG data.
