
#include "ProtocolFrameParser.h"
#include "Arduino.h"

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
    uint8_t data[FRAME_HEADER_LENGTH + payloadLength] = {
        START_OF_FRAME_LSB,
        START_OF_FRAME_MSB
    };

    memcpy(data + FRAME_HEADER_LENGTH, payloadData, payloadLength);

    _PcComsInterface->TransmitData(data, (FRAME_HEADER_LENGTH + payloadLength));
}
