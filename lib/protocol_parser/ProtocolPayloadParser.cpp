/**
 * @file ProtocolPayloadParser.cpp
 * @author Graham Long (longevoty_software@mail.com)
 * @brief Class which deals with parsing protocol payloads
 * @date 2023-Aug-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ProtocolPayloadParser.h"

/**
 * @brief Constructor
 * 
 */
ProtocolPayloadParser::ProtocolPayloadParser()
{
    
}

/**
 * @brief Constructor
 * 
 * @param edp Pointer to the Eeg Data Producer which will be configured by this protocol.
 * @param pti Pointer to the Protocol Transmission Interface which is used for sending responses back to the PC.
 */
ProtocolPayloadParser::ProtocolPayloadParser(IEegDataProducer * edp, IProtocolTransmission * pti) :
    _EEGDataProducer(edp),
    _ProtocolTransmissionInstance(pti)
{
    
}

/**
 * @brief Function to parse the passed payload.
 * 
 * @param payload_ptr A pointer to the payload to parse.
 * @param length The length f the payload.
 * 
 * @return whether or not the payload was valid.
 */
bool ProtocolPayloadParser::ParsePayload(uint8_t * payload_ptr, uint8_t length)
{
    // Confirm the payload pointer and length
    if (nullptr == payload_ptr) { return false; }
    if (0 == length) { return false; }
    
    // Confirm the command group is valid
    const uint8_t COMMAND_GROUP = *payload_ptr & 0xE0;
    if (COMMAND_GROUP > 0x20) { return false; }

    return true;
}
