
#include "ProtocolParser.h"

#include <string.h>

//
// Constructor
//
ProtocolParser::ProtocolParser()
{
    
}

ProtocolParser::ProtocolParser(PcCommunicationsInterface * pci, EegDataProducer * edp) :
    _PcComsInterface(pci),
    _EEGDataProducer(edp),
    _LastValidId(_MAX_VALID_ID)
{
    _RxState = GetDefaultRxStruct();
}

//
//  Overriden function for processing events.
//
void ProtocolParser::ProcessEvent(NEvent::eEvent event)
{
    if (NEvent::Event_DataRxFromPC != event)
    {
        return;
    }

    uint8_t buf[1];

    const uint8_t RX_COUNT = _PcComsInterface->GetReceivedBytes(buf, 1);

    if (0 == RX_COUNT)
    {
        return;
    }

    _RxState = _RxState.state_fptr(buf[0], _RxState, this);
}

//
//  Overridden function from EegDataConsumer used to push EEG samples to the PC.
//
void ProtocolParser::PushLatestSample(EegData::sEegSamples samples)
{
    uint8_t payload[_PROTOCOL_PAYLOAD_SIZE] = {
        (uint8_t)(samples.channel_1 & 0x00FF),
        (uint8_t)((samples.channel_1 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_2 & 0x00FF),
        (uint8_t)((samples.channel_2 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_3 & 0x00FF),
        (uint8_t)((samples.channel_3 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_4 & 0x00FF),
        (uint8_t)((samples.channel_4 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_5 & 0x00FF),
        (uint8_t)((samples.channel_5 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_6 & 0x00FF),
        (uint8_t)((samples.channel_6 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_7 & 0x00FF),
        (uint8_t)((samples.channel_7 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_8 & 0x00FF),
        (uint8_t)((samples.channel_8 & 0xFF00) >> 8)
    };
}

// 
//  Data reception state function to wait for the sync sequence
//
ProtocolParser::sRxStruct ProtocolParser::RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    static const uint8_t SYNC_SEQ_BYTES[] = {0xAA, 0x55};
    static const uint8_t SYNC_SEQ_BYTE_COUNT = 2;

    if (c != SYNC_SEQ_BYTES[state.rxMultiByteCounter])
    {
        return GetDefaultRxStruct();
    }

    if (++state.rxMultiByteCounter < SYNC_SEQ_BYTE_COUNT)
    {
        return state;
    }

    // update state to the next field.
    state.rxMultiByteCounter = 0;
    state.state_fptr = RxState_GetProtocolVersion;

    return state;
}

// 
//  Data reception state function to process the received protocol version
//
ProtocolParser::sRxStruct ProtocolParser::RxState_GetProtocolVersion(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    if (c != IMPLEMENTED_PROTOCOL_VERSION)
    {
        return GetDefaultRxStruct();
    }

    // update state to the next field.
    state.state_fptr = RxState_GetPayloadLength;

    return state;
}

// 
//  Data reception state function to process the received payload length
//
ProtocolParser::sRxStruct ProtocolParser::RxState_GetPayloadLength(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    // quick check that the payload length is not greater than the buffer we have assigned.
    if (c > _MAX_PAYLOAD_SIZE)
    {
        // TODO - what should we do in this case
        return GetDefaultRxStruct();
    }

    // store the payload length
    state.payloadLength = c;

    // update state to the next field.
    state.state_fptr = RxState_GetIdNumber;

    return state;
}

// 
//  Data reception state function to process the received message ID
//
ProtocolParser::sRxStruct ProtocolParser::RxState_GetIdNumber(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    const uint8_t NEXT_EXPECTED_ID_NUMBER = protocolParser->_LastValidId + 1;

    // check the ID to ensure we have not missed a message.
    if (c != NEXT_EXPECTED_ID_NUMBER)
    {
        // TODO - return an ack with the last valid ID number

        return GetDefaultRxStruct();
    }

    // update state to the next field.
    state.messageID = c;

    // update state to get the acknowledge ID
    state.state_fptr = RxState_GetAcknowledgeId;

    return state;
}

// 
//  Data reception state function to process the received acknowledge ID
//
ProtocolParser::sRxStruct ProtocolParser::RxState_GetAcknowledgeId(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    // update state to the next field.
    //state.state_fptr = ;

    return state;
}

//
//  Helper function to get a default rx state struct
//
ProtocolParser::sRxStruct ProtocolParser::GetDefaultRxStruct(void)
{
    ProtocolParser::sRxStruct temp;

    memset(temp.payload, 0, _MAX_PAYLOAD_SIZE);
    temp.payloadLength = 0;
    temp.messageID = 255;
    temp.checksum = 0;

    temp.rxIndex = 0;
    temp.rxMultiByteCounter = 0;

    temp.state_fptr = RxState_WaitForSyncSequence;
    

    return temp;
}


#ifdef PIO_UNIT_TESTING

ProtocolParser::RX_STATE ProtocolParser::GetCurrentRxState()
{
    if (RxState_WaitForSyncSequence == _RxState.state_fptr)
    {
        return WaitForSequence;
    }
    else if (RxState_GetProtocolVersion == _RxState.state_fptr)
    {
        return GetProtocolVersion;
    }
    else if (RxState_GetPayloadLength == _RxState.state_fptr)
    {
        return GetPayloadLength;
    }
    else if (RxState_GetIdNumber == _RxState.state_fptr)
    {
        return GetIdNumber;
    }
    else if (RxState_GetAcknowledgeId == _RxState.state_fptr)
    {
        return GetAckId;
    }

    return InvalidState;
}

uint8_t ProtocolParser::GetImplementedProtocolVersion()
{
    return IMPLEMENTED_PROTOCOL_VERSION;
}

#endif
