
#include "ProtocolTransmitter.h"

//
// Constructor
//
ProtocolTransmitter::ProtocolTransmitter()
{
    
}

ProtocolTransmitter::ProtocolTransmitter(PcCommunicationsInterface * pci) :
    _PcComsInterface(pci)
{
    
}

//
//  Overriden function for processing events.
//
void ProtocolTransmitter::ProcessEvent(NEvent::eEvent event)
{
    if (NEvent::Event_DataToTxToPC != event)
    {
        return;
    }
    
    /*// send all messages from the last acknowledged one
    for (uint8_t i = _TxNextUnackedIndex; i < _TxIpIndex; ++i)
    {
        // TODO - Simplify this to prevent copying so much data.

        uint8_t message[_MAX_MESSAGE_LENGTH];

        // populate the header
        message[0] = 0xAA;
        message[1] = 0x55;
        message[2] = _IMPLEMENTED_PROTOCOL_VERSION;
        message[3] = _TxMessages[i].payloadLength;
        message[4] = _TxMessages[i].idNumber;
        message[5] = _RxState.lastValidId;          // always return the last valid ID we have received.
        // Skip the checksum until we have populated the payload.

        // copy over the payload
        for (int j = 0; j < _TxMessages[i].payloadLength; ++j)
        {
            message[_HEADER_SIZE + j] = _TxMessages[i].payload[j];
        }

        const uint16_t TOTAL_COUNT = _TxMessages[i].payloadLength + _HEADER_SIZE;

        // Add the checksum to the message
        message[6] = CalculateChecksum(message, TOTAL_COUNT);

        // finally sent it to the PC
        _PcComsInterface->TransmitData(message, TOTAL_COUNT);
    }*/
}
