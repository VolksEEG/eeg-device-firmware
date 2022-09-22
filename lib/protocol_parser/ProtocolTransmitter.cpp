/**
 * @file ProtocolTransmitter.cpp
 * @author Graham Long (longevoty_software@mail.com)
 * @brief Class which deals with transmission of payload data to the PC.
 * @date 2022-Sept-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ProtocolTransmitter.h"

/**
 * @brief Constructor
 * 
 */
ProtocolTransmitter::ProtocolTransmitter()
{
    
}


/**
 * @brief Constructor
 * 
 * @param pci Pointer to the PC Communications interface to be used to communicate with the PC.
 * @param evh Pointer to the event handler module which is used to set the events raised by this module.
 */
ProtocolTransmitter::ProtocolTransmitter(IPcCommunications * pci, EventHandler * evh) :
    _PcComsInterfaceInstance(pci),
    _EventHandlerInstance(evh),
    _IdAckToSend(_MAX_VALID_ID),
    _IdToSend(0),
    _TxIpIndex(0),
    _TxNextUnackedIndex(0),
    _TxNextOpIndex(0),
    _TxCount(0)
{
    
}

/**
 * @brief Overridden function to process events.
 * 
 * @param event The event which is being processed.
 */
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

/**
 * @brief Overridden function to send a payload to the PC.
 * 
 * @param payload_ptr A pointer to the payload to be sent.
 * @param payload_length The length of the payload to be sent.
 * 
 * @return true if adding the payload was successful, else false
 * @note this function returning true only means it was successfully added to the queue for transmission
 * not that it was successfully transmitted.
 */
bool ProtocolTransmitter::SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length)
{
    if (_MAX_TX_MESSAGES == _TxCount)
    {
        // the tx messages array is full
        return false;
    }

    if (payload_length > _MAX_PAYLOAD_SIZE)
    {
        // payload can't fit into the allocated memory so return.
        return false;
    }

    // populate the tx message with the payload
    _TxMessages[_TxIpIndex].message[_SYNC_WORD_MSB_INDEX] = _SYNC_WORD_MSB;
    _TxMessages[_TxIpIndex].message[_SYNC_WORD_LSB_INDEX] = _SYNC_WORD_LSB;
    _TxMessages[_TxIpIndex].message[_PROTOCOL_VERSION_INDEX] = _IMPLEMENTED_PROTOCOL_VERSION;
    _TxMessages[_TxIpIndex].message[_PAYLOAD_LENGTH_INDEX] = payload_length;
    _TxMessages[_TxIpIndex].message[_ID_NUMBER_INDEX] = _IdToSend++;

    // add any payload
    for (uint8_t i = 0; i < payload_length; ++i)
    {
        _TxMessages[_TxIpIndex].message[_PAYLOAD_START_INDEX + i] = *(payload_ptr + i);
    }

    _TxMessages[_TxIpIndex].message[_PAYLOAD_CHECKSUM_INDEX] = CalculateChecksumOfMessagePayload(_TxMessages[_TxIpIndex].message);
    
    // ID Acknowledge, and header checksum will be calculated and populated on transmission.
    
    // update the input index
    if (++_TxIpIndex == _MAX_TX_MESSAGES)
    {
        _TxIpIndex = 0;
    }

    _TxCount++;

    if (1 == _TxCount)
    {
        // this is the only message in the queue, so kick off processing the messages
        _EventHandlerInstance->SignalEvent(NEvent::Event_DataToTxToPC);
    }

    return true;
}

/**
 * @brief Overridden function to process the recieved ID.
 * 
 * @param id The ID to process.
 * 
 * @return true if the received ID is valid, else false.
 */
bool ProtocolTransmitter::ProcessReceivedId(uint8_t id)
{
    const uint8_t NEXT_EXPECTED_ID = (_IdAckToSend == _MAX_VALID_ID) ? 0 : (_IdAckToSend + 1);

    if (NEXT_EXPECTED_ID != id)
    {
        // the id is not as expected, so add an ack which will ack the most recent valid ID
        uint8_t ackData[1] = {(uint8_t)GROUP_ACKNOWLEDGE | (uint8_t)CMD_ACKNOWLEDGE};

        SendPayloadToPc(ackData, 1);

        return false;
    }

    // ID is valid so update the ID ack to send
    _IdAckToSend = id;
    
    return true;
}

/**
 * @brief Overridden function to process an acknowledged ID.
 * 
 * @param id The ID being acknowledged.
 */
void ProtocolTransmitter::ProcessAcknowledgedId(uint8_t id)
{
    // TODO - Process acknowledged messages.
}

/**
 * @brief Overridden function to consume the latest EEG samples.
 * 
 * @param samples The latest received samples.
 */
void ProtocolTransmitter::PushLatestSample(EegData::sEegSamples samples)
{
    uint8_t eeg_data_payload[10];       // TODO - actually populate this

    this->SendPayloadToPc(eeg_data_payload, 10);    // TODO - send the correct amount.
}
