
#include "ProtocolReceiver.h"

#include <string.h>

//
// Constructor
//
ProtocolReceiver::ProtocolReceiver()
{
    
}

ProtocolReceiver::ProtocolReceiver(PcCommunicationsInterface * pci, EegDataProducer * edp, EventHandler * evh) :
    _PcComsInterface(pci),
    _EEGDataProducer(edp),
    _EventHandler(evh),
    _TxIpIndex(0),
    _TxNextUnackedIndex(0),
    _TxNextOpIndex(0),
    _TxCount(0),
    _TxNextIdToSend(0)
{
    _RxState = ResetRxStruct(_RxState);

    // the last Valid ID is always MAX and the first expected ID is always 0
    _RxState.lastValidId = _MAX_VALID_ID;
    _RxState.nextExpectedId = 0;

    for (int i = 0; i < _MAX_TX_MESSAGES; ++i)
    {
        ResetTxMessage(&_TxMessages[i]);
    }
}

//
//  Overriden function for processing events.
//
void ProtocolReceiver::ProcessEvent(NEvent::eEvent event)
{
    if (NEvent::Event_DataRxFromPC == event)
    {
        uint8_t buf[1];

        const uint8_t RX_COUNT = _PcComsInterface->GetReceivedBytes(buf, 1);

        if (0 == RX_COUNT)
        {
            return;
        }

        _RxState = _RxState.state_fptr(buf[0], _RxState, this);
    }
    else if (NEvent::Event_DataToTxToPC == event)
    {
        // send all messages from the last acknowledged one
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
        }
    }
}

// 
//  Function to send a payload of data to the PC
//
void ProtocolReceiver::SendPayloadToPc(uint8_t * payload_ptr, uint8_t payloadLength)
{
    if (_TxCount >= _MAX_TX_MESSAGES)
    {
        // no space to transmit
        return;
    }

    if (payloadLength > _MAX_PAYLOAD_SIZE)
    {
        // payload won't fit
        return;
    }

    // Add the message to the Tx Messages fifo
    _TxMessages[_TxIpIndex].idNumber = _TxNextIdToSend++;
    _TxMessages[_TxIpIndex].payloadLength = payloadLength;

    for (int i = 0; i < payloadLength; ++i)
    {
        _TxMessages[_TxIpIndex].payload[i] = payload_ptr[i];
    }

    _TxIpIndex++;
    _TxCount++;

    _EventHandler->SignalEvent(NEvent::Event_DataToTxToPC);
}

// 
//  Data reception state function to wait for the sync sequence
//
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    static const uint8_t SYNC_SEQ_BYTES[] = {0xAA, 0x55};
    static const uint8_t SYNC_SEQ_BYTE_COUNT = 2;

    if (c != SYNC_SEQ_BYTES[state.rxMultiByteCounter])
    {
        return ResetRxStruct(state);
    }

    // store the recieved value
    state.message[state.rxIndex++] = c;

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
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetProtocolVersion(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    if (c != _IMPLEMENTED_PROTOCOL_VERSION)
    {
        return ResetRxStruct(state);
    }

    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to the next field.
    state.state_fptr = RxState_GetPayloadLength;

    return state;
}

// 
//  Data reception state function to process the received payload length
//
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetPayloadLength(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
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

    // store the recieved value
    state.message[state.rxIndex++] = c;

    // store the payload length
    state.payloadLength = c;

    // update state to the next field.
    state.state_fptr = RxState_GetIdNumber;

    return state;
}

// 
//  Data reception state function to process the received message ID
//
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetIdNumber(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to the next field.
    state.messageID = c;

    // update state to get the acknowledge ID
    state.state_fptr = RxState_GetAcknowledgeId;

    return state;
}

// 
//  Data reception state function to process the received acknowledge ID
//
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetAcknowledgeId(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to the next field.
    state.state_fptr = RxState_GetChecksum;

    return state;
}

// 
//  Data reception state function to capture the checksum
//
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetChecksum(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // just store the checksum for now
    state.checksum = c;

    state.state_fptr = RxState_GetPayload;

    return state;
}

ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetPayload(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // have all payload bytes been received.
    if (++state.rxMultiByteCounter < state.payloadLength)
    {
        // no, so wait for the next byte
        return state;
    }

    // process the payload
    const uint8_t CALC_CHECKSUM = CalculateChecksum(state.message, state.payloadLength + _HEADER_SIZE);

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

        protocolReceiver->SendPayloadToPc(ackData, 1);

        return ResetRxStruct(state); 
    }

    // ID is what we expect, so updated the last Valid ID and the next one we expect will be 1 greater.
    state.lastValidId = state.nextExpectedId;
    state.nextExpectedId++;

    return ResetRxStruct(state);
}

//
//  Helper function to get a default rx state struct
//
ProtocolReceiver::sRxStruct ProtocolReceiver::ResetRxStruct(sRxStruct state)
{
    memset(state.message, 0, _MAX_MESSAGE_LENGTH);
    state.payloadLength = 0;
    state.messageID = 255;
    state.checksum = 0;

    state.rxIndex = 0;
    state.rxMultiByteCounter = 0;

    state.state_fptr = RxState_WaitForSyncSequence;

    return state;
}

uint8_t ProtocolReceiver::CalculateChecksum(uint8_t * data, uint16_t count)
{
    return 0x00;
}

void ProtocolReceiver::ResetTxMessage(sTxMessageStruct * message_ptr)
{
    message_ptr->idNumber = 0;
    message_ptr->payloadLength = 0;

    for (int i = 0; i < _MAX_PAYLOAD_SIZE; ++i)
    {
        message_ptr->payload[i] = 0;
    }
}


#ifdef PIO_UNIT_TESTING

ProtocolReceiver::RX_STATE ProtocolReceiver::GetCurrentRxState()
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

uint8_t ProtocolReceiver::GetImplementedProtocolVersion()
{
    return _IMPLEMENTED_PROTOCOL_VERSION;
}

uint8_t ProtocolReceiver::GetMaximumPayloadLength()
{
    return _MAX_PAYLOAD_SIZE;
}

uint8_t ProtocolReceiver::GetNextExpectedId()
{
    return _RxState.nextExpectedId;
}

#endif
