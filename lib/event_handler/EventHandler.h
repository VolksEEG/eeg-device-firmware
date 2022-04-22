
#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER

#include "../error_handler/ErrorHandler.h"
#include "CanProcesEvents.h"

#include <stdint.h>

class EventHandler : public CanProcessEvents {

    public:

        static const uint8_t MAX_PROCESS_HANDLERS = 5;

        EventHandler();
        explicit EventHandler(ErrorHandler * eh);

        void SignalEvent(NEvent::eEvent event);
        void AddEventHandler(CanProcessEvents * handler_ptr,
                                NEvent::eEvent event);
        void HandleEvents(void);

        void ProcessEvent(NEvent::eEvent event) override {

        }

    protected:

    private:

        typedef struct _EVENT_HANDLERS
        {
            uint8_t setCount;
            CanProcessEvents * processerInstance_ptrs[MAX_PROCESS_HANDLERS];
        }sEventProcessers;

        sEventProcessers _EventProcessers[NEvent::MAX_EVENTS];

        uint8_t _EventsSetBits;

        ErrorHandler * _ErrorHandlerInstance;
};

#endif
