# Event Handler
This module is repsonible for allowing for events to be raised by other modules and then processed in priority order by any other module which implements an event handler.

All events are defined in the Events.h file.

Any Class which wishes to be an event handler must publicly inherit the CanProcessEvents class and be passed to the "AddEventHandler" function in main.cpp following initialisation of all the classes.

## Available Events in priority order (Highest first)
1. ADS1299 Data Ready
2. EDF File Data Ready
3. 1mS Timer Event

## Event overrun
In order to prevent an event being missed (not processed) due to another of the same event being signalled, this module **shall** check that the relevent event is not already signalled, before signalling it to be processed, and raising an error in the error module if an overrun has ocurred.