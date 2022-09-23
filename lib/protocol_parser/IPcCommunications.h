
#ifndef _PC_COMMUNICATIONS_INTERFACE
#define _PC_COMMUNICATIONS_INTERFACE

#include <stdint.h>

class IPcCommunications {

    public:
        // TODO - consider splitting this into two interfaces as no module is likely to need both functions
        virtual uint8_t GetReceivedBytes(uint8_t data[], uint8_t max_length) = 0;
        virtual void TransmitData(uint8_t data[], uint16_t count) = 0;
};

#endif