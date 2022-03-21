
#ifndef _PC_COMMUNICATIONS_INTERFACE
#define _PC_COMMUNICATIONS_INTERFACE

#include <stdint.h>

class PcCommunicationsInterface {

    public:
        virtual int8_t GetReceivedByte() = 0;
        virtual void TransmitData(int8_t * dataPtr, int8_t count) = 0;
};

#endif
