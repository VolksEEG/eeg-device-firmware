
#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER

#include "../error_handler/ErrorHandler.h"
#include "CanProcesEvents.h"

#include <stdint.h>

class EventHandler : CanProcessEvents {

    public:

        typedef enum _EVENTS
        {
            Event_ADS1299DataReady = 0x00,
            Event_EDFDataReady = 0x01,
            Event_1mSTimeout = 0x02
        }eEvent;

        static const uint8_t NO_EVENTS = 0;
        static const uint8_t MAX_EVENTS = 3;
        static const uint8_t MAX_PROCESS_HANDLERS = 5;

        EventHandler();
        EventHandler(ErrorHandler * eh);

        void SignalEvent(eEvent event);
        void AddEventHandler(CanProcessEvents * processerInstance, eEvent event);
        void HandleEvents(void);

    protected:

    private:

        typedef struct _EVENT_HANDLERS
        {
            uint8_t setCount;
            CanProcessEvents *processerInstances[MAX_PROCESS_HANDLERS];
        }sEventProcessers;

        sEventProcessers _EventProcessers[MAX_EVENTS];

        uint8_t _EventsSetBits;

        ErrorHandler * _ErrorHandlerInstance;
};

#endif
