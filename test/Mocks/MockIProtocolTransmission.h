#ifndef MOCK_PROTOCOL_TRASNMISSION
#define MOCK_PROTOCOL_TRASNMISSION

#include <IProtocolTransmission.h>

class MockIProtocolTransmission : public IProtocolTransmission {

    public:

        MockIProtocolTransmission()
        {

        }

        bool SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length) override
        {
            return true;
        }

        void UpdateIdToAcknowledge(uint8_t id) override
        {
            
        }

        void UpdateAcknowledgedId(uint8_t id) override
        {

        }

    protected:

    private:

};

#endif
