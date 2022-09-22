
#ifndef _I_PROTOCOL_TRANSMISSION
#define _I_PROTOCOL_TRANSMISSION

#include <stdint.h>

class IProtocolTransmission {

    public:
        virtual bool SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length) = 0;
        virtual bool ProcessReceivedId(uint8_t id) = 0;
        virtual void ProcessAcknowledgedId(uint8_t id) = 0;
};

#endif
