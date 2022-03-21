
#include "EventHandler.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#else
#include <../../src/ArduinoMock.h>
#endif

//
// Constructor
//
EventHandler::EventHandler()
{

}

//
// Constructor
//
EventHandler::EventHandler(ErrorHandler * eh) :
    _EventsSetBits(0),
    _ErrorHandlerInstance(eh)
{
    // initialise the event handlers
    for (int event = 0; event < NEvent::MAX_EVENTS; ++event)
    {
        // officially no handlers have been set
        _EventProcessers->setCount = 0;

        const sEventHandlerInstance hInstance = {this, &CanProcessEvents::ProcessEvent};

        for (int handler = 0; handler < MAX_PROCESS_HANDLERS; ++handler)
        {
            // default all the handlers to this class
            _EventProcessers->processerInstances[handler] = hInstance;
        }
    }
}

//
//  Called by other classes to signal that an event has occurred
//
void EventHandler::SignalEvent(NEvent::eEvent event)
{
    if ((uint8_t)(1 << event) == (_EventsSetBits & (uint8_t)(1 << event)))
    {
        // the even has already been signalled so raise an error.
        _ErrorHandlerInstance->RaiseError(ErrorHandler::eError::Error_EventOverrun);

        return;
    }   

    _EventsSetBits |= (uint8_t)(1 << event);
}   

//
//  Called by other classes to add an event handler for an event
//
void EventHandler::AddEventHandler(sEventHandlerInstance handler, NEvent::eEvent event)
{
    const uint8_t SET_COUNT = _EventProcessers[(uint8_t)event].setCount;

    if (SET_COUNT >= MAX_PROCESS_HANDLERS)
    {
        // we have allocated all our available handlers
        _ErrorHandlerInstance->RaiseError(ErrorHandler::eError::Error_EventHandlerUnableToBeAdded);

        return;
    }

    _EventProcessers[(uint8_t)event].processerInstances[SET_COUNT] = handler;

    _EventProcessers[(uint8_t)event].setCount++;
}

//
//  Function to handle the highest priority event which has been signalled.
//
void EventHandler::HandleEvents(void)
{
    noInterrupts();

    // take a copy of the event set bits as these may be modified under interrupt
    const uint8_t EVENT_SET_BITS = _EventsSetBits;

    interrupts();

    if (NEvent::NO_EVENTS == EVENT_SET_BITS)
    {
        return;
    }

    // find the highest priority (least significant set bit).
    uint8_t eventIndex = 0;
    bool eventFound = false;
    
    do 
    {
        const uint8_t EVENT_BIT = (1 << eventIndex);

        // check if the bit corresponding to this event is set
        if (EVENT_BIT == (EVENT_SET_BITS & EVENT_BIT))
        {
            // it is so we have found the highest priority event
            eventFound = true;
        }
        else
        {
            // it's not, so try the next event.
            eventIndex++;
        }

    }while (
        (false == eventFound)
        && (NEvent::MAX_EVENTS > eventIndex)
    );

    // an event should be found but just confirm the variables here
    if  (
            (false == eventFound)
            || (NEvent::MAX_EVENTS <= eventIndex)
        )
    {
        _ErrorHandlerInstance->RaiseError(ErrorHandler::eError::Error_EventHandlingFailure);
        return;
    }

    // call all the set event processers
    for (int i = 0; i < _EventProcessers[eventIndex].setCount; ++i)
    {
        sEventHandlerInstance hInst = _EventProcessers[eventIndex].processerInstances[i];
        
        (hInst.processerInstance->*hInst.eventProcessingFptr)((NEvent::eEvent)eventIndex);
    }

    // this event has been processed, so clear it's bit
    _EventsSetBits &= ~(1 << eventIndex);
}
