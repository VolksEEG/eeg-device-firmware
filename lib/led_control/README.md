# LED Control
This module is responsible for controlling the on board LEDs in the following scenarios.

| Scenario          | On Time   | Off Time  |
|-                  |-          |-          |
| Normal Operation  | 200mS     | 1800mS    |
| Error Present     | 1500mS    | 500mS     | 

it shall respond to the 1mS timeout event in order to control this timing.

Each LED State timeout shall be monitored in it's own state function which, once elapsed, will set the counter for the next state depending on whether an error is present or not.  