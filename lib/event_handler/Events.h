
#ifndef _EVENTS
#define _EVENTS

#include <stdint.h>

namespace NEvent {

    static const uint8_t NO_EVENTS = 0;
    static const uint8_t MAX_EVENTS = 6;

    typedef enum _EVENTS
    {
        Event_ADS1299DataReady = 0x00,
        Event_EDFDataReady = 0x01,
        Event_1mSTimeout = 0x02,
        Event_DataRxFromPC = 0x03,
        Event_BufferedADS1299DataReady = 0x04,
        Event_DataToTxToPC = 0x05,
        Event_InvalidEvent = MAX_EVENTS
    }eEvent;   
}

#endif
