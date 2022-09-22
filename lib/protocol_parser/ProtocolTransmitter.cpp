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
 * @param eh Pointer to the event handler module which is used to set the events raised by this module.
 */
ProtocolTransmitter::ProtocolTransmitter(PcCommunicationsInterface * pci, EventHandler * eh) :
    _PcComsInterfaceInstance(pci),
    _EventHandlerInstance(eh),
    _TxIpIndex(0),
    _TxNextUnackedIndex(0),
    _TxNextOpIndex(0),
    _TxCount(0),
    _TxNextIdToSend(0)
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
 */
void ProtocolTransmitter::SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length)
{

    _TxCount++;

    if (1 == _TxCount)
    {
        // this is the only message in the queue, so kick off processing the messages
        _EventHandlerInstance->SignalEvent(NEvent::Event_DataToTxToPC);
    }
    
/*    if (_TxCount >= _MAX_TX_MESSAGES)
    {
        // no space to transmit
        return;
    }

    if (payloadLength > _MAX_RX_PAYLOAD_SIZE)
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
    */
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
