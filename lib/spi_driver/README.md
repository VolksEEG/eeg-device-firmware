# SPI Driver
## Overview
This class allows multiple devices to access the same SPI peripheral.

It simply wraps the Arduino SPI Library to include ability to pass in a chip select setting function from the PinControl class.

## Future improvements
* Allow passing in of SPI Settings to the transfer function to facilitate using with the SD card. 