
#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER

#include "../error_handler/ErrorHandler.h"
#include "ICanProcesEvents.h"

#include <stdint.h>

class EventHandler : public ICanProcessEvents {

    public:

        static const uint8_t MAX_PROCESS_HANDLERS = 5;

        EventHandler();
        explicit EventHandler(ErrorHandler * eh);

        virtual void SignalEvent(NEvent::eEvent event);
        void AddEventHandler(ICanProcessEvents * handler_ptr,
                                NEvent::eEvent event);
        void HandleEvents(void);

        void ProcessEvent(NEvent::eEvent event) override {

        }

    protected:

    private:

        typedef struct _EVENT_HANDLERS
        {
            uint8_t setCount;
            ICanProcessEvents * processerInstance_ptrs[MAX_PROCESS_HANDLERS];
        }sEventProcessers;

        sEventProcessers _EventProcessers[NEvent::MAX_EVENTS];

        uint8_t _EventsSetBits;

        ErrorHandler * _ErrorHandlerInstance;
};

#endif
