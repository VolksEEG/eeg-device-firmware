/**
 * @file ProtocolReceiver.cpp
 * @author Graham Long (longevoty_software@mail.com)
 * @brief Class which deals with common protocol receive functions
 * @date 2022-Sept-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ProtocolReceiver.h"

#include <string.h>

/**
 * @brief Constructor
 * 
 */
ProtocolReceiver::ProtocolReceiver()
{
    
}

/**
 * @brief Constructor
 * 
 * @param pci Pointer to the PC Communications interface to be used to communicate with the PC.
 * @param pti Pointer to the Protocol Transmission Interface which is used for sending responses back to the PC.
 * @param pppi Pointer to the Protocol Payload Parser which is used to process message payloads.
 */
ProtocolReceiver::ProtocolReceiver(IPcCommunications * pci, IProtocolTransmission * pti, ProtocolPayloadParser * ppp) :
    _PcComsInterface(pci),
    _ProtocolTransmissionInstance(pti),
    _ProtocolPayloadParser(ppp)
{
    _RxState = ResetRxStruct(_RxState);

    // first ID is always expected to be 0
    _RxState.nextExpectedID = 0;
}

/**
 * @brief Overridden function to process events.
 * 
 * @param event The event which is being processed.
 */
void ProtocolReceiver::ProcessEvent(NEvent::eEvent event)
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

/**
 * @brief Rx State function which waits for the synchronization sequence to be received.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    static const uint8_t SYNC_SEQ_BYTE_COUNT = 2;
    static const uint8_t SYNC_SEQ_BYTES[SYNC_SEQ_BYTE_COUNT] = {_SYNC_WORD_MSB, _SYNC_WORD_LSB};

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

/**
 * @brief Rx State function which gets and stores the protocol version.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetProtocolVersion(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    if (c != protocolReceiver->_IMPLEMENTED_PROTOCOL_VERSION)
    {
        return ResetRxStruct(state);
    }

    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to the next field.
    state.state_fptr = RxState_GetPayloadLength;

    return state;
}

/**
 * @brief Rx State function which gets the payload length.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetPayloadLength(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // quick check that the payload length is not greater than the buffer we have assigned.
    if (c > protocolReceiver->_MAX_PAYLOAD_SIZE)
    {
        // This should never happen unless we reduce the max payload size below 255
        return ResetRxStruct(state);
    }

    // or the payload length is 0
    if (0 == c)
    {
        return ResetRxStruct(state);
    }

    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to the next field.
    state.state_fptr = RxState_GetIdNumber;

    return state;
}

/**
 * @brief Rx State function which gets the ID Number.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetIdNumber(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to get the acknowledge ID
    state.state_fptr = RxState_GetAcknowledgeId;

    return state;
}

/**
 * @brief Rx State function which gets the Acknowledge ID Number.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetAcknowledgeId(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // update state to the next field.
    state.state_fptr = RxState_GetPayloadChecksum;

    return state;
}

/**
 * @brief Rx State function which Gets the payload checksum.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetPayloadChecksum(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    state.state_fptr = RxState_GetHeaderChecksum;

    return state;
}

/**
 * @brief Rx State function which Gets and checks the header checksum.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetHeaderChecksum(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the received value
    state.message[state.rxIndex++] = c;

    // process the header checksum
    const uint8_t CALC_CHECKSUM = protocolReceiver->CalculateChecksumOfMessageHeader(state.message);
    const uint8_t RX_CHECKSUM = state.message[_HEADER_CHECKSUM_INDEX];

    if (CALC_CHECKSUM != RX_CHECKSUM)
    {
        // header checksum does not match
        return ResetRxStruct(state);
    }

    // checksum is OK so get the payload
    state.state_fptr = RxState_GetPayloadAndProcessMessageContents;

    return state;
}

/**
 * @brief Rx State function which gets the payload number of bytes and processes the message contents.
 * 
 * @param c The character/byte recieved.
 * @param state The current receive state structure.
 * @param protocolReceiver The instance of this class which is calling this state function.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::RxState_GetPayloadAndProcessMessageContents(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver)
{
    // store the recieved value
    state.message[state.rxIndex++] = c;

    // have all payload bytes been received.
    if (++state.rxMultiByteCounter < state.message[_PAYLOAD_LENGTH_INDEX])
    {
        // no, so wait for the next byte
        return state;
    }

    // process the payload checksum
    const uint8_t CALC_CHECKSUM = protocolReceiver->CalculateChecksumOfMessagePayload(state.message);

    if (CALC_CHECKSUM != state.message[_PAYLOAD_CHECKSUM_INDEX])
    {
        return ResetRxStruct(state);
    }

    // checksum is OK, and header checksum will be OK
    // is the ID.
    if (state.nextExpectedID != state.message[_ID_NUMBER_INDEX])
    {
        // the id is not as expected, so add an ack which will ack the most recent valid ID
        uint8_t ackData[1] = {(uint8_t)GROUP_ACKNOWLEDGE | (uint8_t)CMD_ACKNOWLEDGE};

        protocolReceiver->_ProtocolTransmissionInstance->SendPayloadToPc(ackData, 1);

        return ResetRxStruct(state);
    }

    // the recieved ID is as expected, update the next one to expect
    if (state.nextExpectedID++ == _MAX_VALID_ID)
    {
        state.nextExpectedID = 0;
    }

    // The ID is valid, so update the Transmission instance so it can acknowledge it in future messages.
    protocolReceiver->_ProtocolTransmissionInstance->UpdateIdToAcknowledge(state.message[_ID_NUMBER_INDEX]);

    // ID is OK, does this ack any of our ID's
    protocolReceiver->_ProtocolTransmissionInstance->UpdateAcknowledgedId(state.message[_ID_ACKNOWLEDGE_INDEX]);

    //! @todo Process the message contents
    //_ProtocolPayloadParser

    return ResetRxStruct(state);
}

/**
 * @brief helper function to reset the an Rx State structure ready to restart the receiving process.
 * @note Only the fields which are specific to a single isolated message reception are reset.
 * 
 * @param state The receive state structure to reset.
 * 
 * @return The updated recieve state structure.
 */
ProtocolReceiver::sRxStruct ProtocolReceiver::ResetRxStruct(sRxStruct state)
{
    // clear the message
    memset(state.message, 0, _MAX_MESSAGE_LENGTH);

    // and the receive indexes.
    state.rxIndex = 0;
    state.rxMultiByteCounter = 0;

    // reset the state machine
    state.state_fptr = RxState_WaitForSyncSequence;

    // and return the reset state
    return state;
}

#ifdef PIO_UNIT_TESTING

/**
 * @brief Unit testing helper function used to query the current state.
 * 
 * @return An enum representation of the current state.
 */
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
    else if (RxState_GetPayloadChecksum == _RxState.state_fptr)
    {
        return GetPayloadChecksum;
    }
    else if (RxState_GetHeaderChecksum == _RxState.state_fptr)
    {
        return GetHeaderChecksum;
    }
    else if (RxState_GetPayloadAndProcessMessageContents == _RxState.state_fptr)
    {
        return GetPayload;
    }

    return InvalidState;
}

/**
 * @brief Unit testing helper function used to query the next expected ID.
 * 
 * @return The next expected ID to be recieved.
 */
uint8_t ProtocolReceiver::GetNextExpectedId()
{
    return _RxState.nextExpectedID;
}

#endif
