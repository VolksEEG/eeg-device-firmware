
#include "EventHandler.h"

//
// Constructor
//
EventHandler::EventHandler()
{

}

//
// Constructor
//
EventHandler::EventHandler(ErrorHandler& eh) :
    _EventsSetBits(0),
    _ErrorHandler(eh)
{

}

void EventHandler::SignalEvent(eEvent event)
{
    if ((uint8_t)event == (_EventsSetBits & (uint8_t)event))
    {
        // the even has already been signalled so raise an error.
        _ErrorHandler.RaiseError(ErrorHandler::eError::Error_EventOverrun);

        return;
    }   

    _EventsSetBits |= (uint8_t)event;
}   

void EventHandler::AddEventHandler(CanProcessEvents processer, eEvent event)
{

}

void EventHandler::HandleEvents(void)
{

}
