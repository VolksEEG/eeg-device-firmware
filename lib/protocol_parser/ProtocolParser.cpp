
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
    _EEGDataProducer(edp)
{
    _RxState = ResetRxStruct(_RxState);

    // the first expected ID is always 0
    _RxState.nextExpectedId = 0;
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
//  Function to send a payload of data to the PC
//
void ProtocolParser::SendPayloadToPc(uint8_t * payload_ptr, uint8_t payloadLength)
{
    // TODO - fill this in 
    // TODO - public function to send EEG data.
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
        return ResetRxStruct(state);
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
    if (c != _IMPLEMENTED_PROTOCOL_VERSION)
    {
        return ResetRxStruct(state);
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
        return ResetRxStruct(state);
    }

    // or the payload length is 0
    if (0 == c)
    {
        return ResetRxStruct(state);
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
    state.state_fptr = RxState_GetChecksum;

    return state;
}

// 
//  Data reception state function to capture the checksum
//
ProtocolParser::sRxStruct ProtocolParser::RxState_GetChecksum(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    // just store the checksum for now
    state.checksum = c;

    state.state_fptr = RxState_GetPayload;

    return state;
}

ProtocolParser::sRxStruct ProtocolParser::RxState_GetPayload(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    // append this byte to the payload.
    state.payload[state.rxMultiByteCounter++] = c;

    // have all payload bytes been received.
    if (state.rxMultiByteCounter < state.payloadLength)
    {
        // no, so wait for the next byte
        return state;
    }

    // process the payload
    const uint8_t CALC_CHECKSUM = CalculateChecksum(state);

    if (CALC_CHECKSUM != state.checksum)
    {
        return ResetRxStruct(state);
    }

    // checksum is OK
    // is the ID.
    if (state.messageID != state.nextExpectedId)
    {
        // ID is not what we expect.
        // Send a response with the last Valid ID Acknowledged.
        uint8_t ackData[1] = {(uint8_t)GROUP_ACKNOWLEDGE | (uint8_t)CMD_ACKNOWLEDGE};

        protocolParser->SendPayloadToPc(ackData, 1);

        return ResetRxStruct(state);   
    }

    // ID is what we expect, so the next one we expect will be 1 greater.
    state.nextExpectedId++;

    return ResetRxStruct(state);
}

//
//  Helper function to get a default rx state struct
//
ProtocolParser::sRxStruct ProtocolParser::ResetRxStruct(sRxStruct state)
{
    memset(state.payload, 0, _MAX_PAYLOAD_SIZE);
    state.payloadLength = 0;
    state.messageID = 255;
    state.checksum = 0;

    state.rxIndex = 0;
    state.rxMultiByteCounter = 0;

    state.state_fptr = RxState_WaitForSyncSequence;

    return state;
}

uint8_t ProtocolParser::CalculateChecksum(sRxStruct state)
{
    return 0x00;
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
    else if (RxState_GetChecksum == _RxState.state_fptr)
    {
        return GetChecksum;
    }
    else if (RxState_GetPayload == _RxState.state_fptr)
    {
        return GetPayload;
    }

    return InvalidState;
}

uint8_t ProtocolParser::GetImplementedProtocolVersion()
{
    return _IMPLEMENTED_PROTOCOL_VERSION;
}

uint8_t ProtocolParser::GetMaximumPayloadLength()
{
    return _MAX_PAYLOAD_SIZE;
}

uint8_t ProtocolParser::GetNextExpectedId()
{
    return _RxState.nextExpectedId;
}

#endif
