
#include "ProtocolFrameParser.h"

//
// Constructor
//
ProtocolFrameParser::ProtocolFrameParser()
{

}

//
// Constructor
//
ProtocolFrameParser::ProtocolFrameParser(PcCommunicationsInterface * pci) :
    _PcComsInterface(pci)
{

}

//
//  Overriden function for processing events.
//
void ProtocolFrameParser::ProcessEvent(NEvent::eEvent event)
{

}

//
//
//
void ProtocolFrameParser::AddFrameToPayloadAndSendToPc(uint8_t payloadData[], uint8_t payloadLength)
{
    uint8_t header[FRAME_HEADER_LENGTH] = {
        START_OF_FRAME,
        FRAME_VERSION,
        payloadLength,
        0, 0
    };

    _PcComsInterface->TransmitData(header, FRAME_HEADER_LENGTH);
    _PcComsInterface->TransmitData(payloadData, payloadLength);
}
