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
    _IdToAcknowledge(_MAX_VALID_ID),
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

    // confirm there are messages to send
    if (0 == _TxCount)
    {
        return;
    }

    // first populate the next message to send with the latest ID to Ack
    _TxMessages[_TxNextOpIndex].message[_ID_ACKNOWLEDGE_INDEX] = _IdToAcknowledge;
    // and update the header checksum as this may have changed
    _TxMessages[_TxNextOpIndex].message[_HEADER_CHECKSUM_INDEX] = CalculateChecksumOfMessageHeader(_TxMessages[_TxNextOpIndex].message);

    // and send it to the PC
    _PcComsInterfaceInstance->TransmitData(_TxMessages[_TxNextOpIndex].message, (uint16_t)(_HEADER_SIZE + _TxMessages[_TxNextOpIndex].message[_PAYLOAD_LENGTH_INDEX]));

    const uint8_t NEXT_NEXT_OP_INDEX = ((_TxNextOpIndex + 1) == _MAX_TX_MESSAGES) ? 0 : (_TxNextOpIndex + 1);

    if (NEXT_NEXT_OP_INDEX == _TxIpIndex)
    {
        // the next index is where the next message will go, therefore there are no more messages to send.
        // do not update the next op index, if this function gets called again, it will resend the last message.
        return;
    }

    // update the op index to send the next message next time
    _TxNextOpIndex = NEXT_NEXT_OP_INDEX;

    // TODO - Event_DataToTxToPC will only be called once, we can't signal it again here, as that will cause an overrun, maybe set up some timer and/or do as in the SerialPortDriver.
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
 * @brief Overridden function to update the id to acknowledge in future transmissions.
 * 
 * @param id The ID to acknowledge in future transmissions.
 */
void ProtocolTransmitter::UpdateIdToAcknowledge(uint8_t id)
{
    _IdToAcknowledge = id;
}

/**
 * @brief Overridden function to process an acknowledged ID.
 * 
 * @param id The ID being acknowledged.
 */
void ProtocolTransmitter::UpdateAcknowledgedId(uint8_t id)
{
    uint8_t newUnackedIndex = _TxNextUnackedIndex;
    uint8_t newCount = _TxCount;
    bool idFound = false;
    
    for (
            uint8_t i = _TxNextUnackedIndex; 
            (newCount > 0) && (newUnackedIndex == _TxNextUnackedIndex);              // end if the end has been found, or we have tested all messages
            i = ((i + 1) == _MAX_TX_MESSAGES) ? 0 : (i + 1)     // Increment and wrap around 
        )
    {
        if (!idFound)
        {
            // is this message the same ID as we have received
            if (_TxMessages[i].message[_ID_NUMBER_INDEX] == id)
            {
                idFound = true;
            }
            
            // this ID is not it so the new count is at least one less if we do find the id in future
            newCount--;
        }
        else
        {
            // the id has been found, so the next id is the newest Unacked Index.
            newUnackedIndex = i;
        }
    }

    if (newUnackedIndex != _TxNextUnackedIndex)
    {
        // the next unacked index needs updating
        _TxNextUnackedIndex = newUnackedIndex;

        // restart message delivery from this point
        _TxNextOpIndex = _TxNextUnackedIndex;

        _TxCount = newCount;
    }
    else if (idFound)
    {
        // if the ID has been found, but the nextUnackedIndex has not changed, then the ID is the latest in the queue.
        // effectively clear the queue
        _TxNextUnackedIndex = _TxIpIndex;
        _TxNextOpIndex = _TxNextUnackedIndex;
        _TxCount = 0;
    }
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

#ifdef PIO_UNIT_TESTING

/**
 * @brief Unit testing helper function to get the number of Tx messages.
 * 
 * @return The local _TxCount variable.
 */
uint8_t ProtocolTransmitter::GetTxCount()
{
    return _TxCount;
}

#endif
