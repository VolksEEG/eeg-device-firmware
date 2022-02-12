# SPI Scheduler
## Overview
This class allows multiple devices to access the same SPI peripheral.

## Shcedule priorities
An SPI jobs can be scheduled at any of the following priorities:

### High Priority
Any job scheduled at this priority shall be run prior to any of the other jobs.

### Normal Priority
Any job scheduled at this priority shall be run, unless there are high priority jobs to run.

*Note:* If multiple jobs are scheduled at the same priority, these will be run in the order they were scheduled (FIFO).

## Spi Jobs
Each SPI Job needs to know the following:
* What function to call to control the chip select line.
* What data to transmit.
* What function to call when the data has been transmitted (and consequently recieved).
