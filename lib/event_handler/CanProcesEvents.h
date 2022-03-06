
#ifndef _CAN_PROCESS_EVENTS
#define _CAN_PROCESS_EVENTS

#include "Events.h"

class CanProcessEvents {

    public:
        virtual void ProcessEvent(NEvent::eEvent event) = 0;
};

#endif
