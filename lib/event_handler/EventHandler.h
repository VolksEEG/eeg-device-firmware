
#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER

#include "../error_handler/ErrorHandler.h"

#include <stdint.h>

class CanProcessEvents {

    public:
        virtual void ProcessEvent() {

        }
};

class EventHandler {

    public:

        typedef enum _EVENTS
        {
            Event_ADS1299DataReady = 0x01,
            Event_EDFDataReady = 0x02,
            Event_1mSTimeout = 0x04
        }eEvent;

        EventHandler();
        EventHandler(ErrorHandler& eh);

        void SignalEvent(eEvent event);
        void AddEventHandler(CanProcessEvents processer, eEvent event);
        void HandleEvents(void);

    protected:

    private:

        uint8_t _EventsSetBits;

        ErrorHandler _ErrorHandler;
};

#endif
