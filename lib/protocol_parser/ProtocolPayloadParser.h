
#ifndef _PROTOCOL_PAYLOAD_PARSER
#define _PROTOCOL_PAYLOAD_PARSER

#include "IProtocolTransmission.h"

#include "../data_flow_controller/IEEGDataProducer.h"

class ProtocolPayloadParser {

    public:

        ProtocolPayloadParser();
        ProtocolPayloadParser(IEegDataProducer * edp, IProtocolTransmission * pti);

    protected:

    private:

        IEegDataProducer * _EEGDataProducer;
        IProtocolTransmission * _ProtocolTransmissionInstance;
};

#endif
