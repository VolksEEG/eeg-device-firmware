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
