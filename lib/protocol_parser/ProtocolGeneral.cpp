/**
 * @file ProtocolGeneral.cpp
 * @author Graham Long (longevoty_software@mail.com)
 * @brief Class which deals with common protocol receive / transmit functions
 * @date 2022-Sept-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ProtocolGeneral.h"

/**
 * @brief Constructor
 * 
 */
ProtocolGeneral::ProtocolGeneral()
{
    
}

/**
 * @brief Function to Calculate the checksum of the passed messages header.
 * 
 * @param message_ptr A pointer to the message to return the header checksum of.
 */
uint8_t ProtocolGeneral::CalculateChecksumOfMessageHeader(uint8_t * message_ptr)
{
    return 0x00;
}

/**
 * @brief Function to Calculate the checksum of the passed messages payload.
 * 
 * @param message_ptr A pointer to the message to return the payloads checksum.
 */
uint8_t ProtocolGeneral::CalculateChecksumOfMessagePayload(uint8_t * message_ptr)
{
    return 0x01;
}

