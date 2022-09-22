
#ifndef _I_PROTOCOL_TRANSMISSION
#define _I_PROTOCOL_TRANSMISSION

#include <stdint.h>

class IProtocolTransmission {

    public:
        virtual void SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length) = 0;
};

#endif
