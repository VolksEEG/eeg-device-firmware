# ADS1299 Driver
## Overview
This driver consists of both a high level driver which exposes the following abstracted functionality:
* Retrieving the number of available channels
* Starting / Stopping continuous data capture.
* Configuring the input channels.
* Enabling / disabling current sources.
* Setting / Getting GPIO levels.

And a low level driver which is called by the high level driver to set the appropriate register values in the ADS1299.