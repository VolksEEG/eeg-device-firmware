#ifndef MOCK_PROTOCOL_TRASNMISSION
#define MOCK_PROTOCOL_TRASNMISSION

#include <IProtocolTransmission.h>

class MockIProtocolTransmission : public IProtocolTransmission {

    public:

        MockIProtocolTransmission()
        {

        }

        void SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length) override
        {
            
        }

    protected:

    private:

};

#endif
