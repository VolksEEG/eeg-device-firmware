
#ifndef _PROTOCOL_PAYLOAD_PARSER
#define _PROTOCOL_PAYLOAD_PARSER

#include "IProtocolTransmission.h"

#include "../data_flow_controller/IEEGDataProducer.h"

class ProtocolPayloadParser {

    public:

        ProtocolPayloadParser();
        ProtocolPayloadParser(IEegDataProducer * edp, IProtocolTransmission * pti);

        bool ParsePayload(uint8_t * payload_ptr, uint8_t length);

    protected:

    private:

        IEegDataProducer * _EEGDataProducer;
        IProtocolTransmission * _ProtocolTransmissionInstance;
};

#endif
