
#ifndef _EVENTS
#define _EVENTS

#include <stdint.h>

namespace NEvent {

    typedef enum _EVENTS
    {
        Event_ADS1299DataReady = 0x00,
        Event_EDFDataReady = 0x01,
        Event_1mSTimeout = 0x02,
        Event_DataRxFromPC = 0x03,
        Event_ProcessedADS1299DataReady = 0x04
    }eEvent;

    static const uint8_t NO_EVENTS = 0;
    static const uint8_t MAX_EVENTS = 5;
    
}

#endif
