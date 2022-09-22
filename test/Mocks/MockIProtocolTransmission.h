#ifndef MOCK_PROTOCOL_TRASNMISSION
#define MOCK_PROTOCOL_TRASNMISSION

#include <IProtocolTransmission.h>

class MockIProtocolTransmission : public IProtocolTransmission {

    public:

        MockIProtocolTransmission():
            _NextExpectedId(0)
        {

        }

        bool SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length) override
        {
            return true;
        }

        bool ProcessReceivedId(uint8_t id) override
        {
            if (_NextExpectedId != id)
            {
                return false;
            }

            _NextExpectedId = (_NextExpectedId == ProtocolGeneral::_MAX_VALID_ID) ? 0 : (_NextExpectedId + 1);

            return true;
        }

        void ProcessAcknowledgedId(uint8_t id) override
        {

        }

        uint8_t GetNextExpectedId(void)
        {
            return _NextExpectedId;
        }

    protected:

    private:

        uint8_t _NextExpectedId;
};

#endif
