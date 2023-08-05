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

static const uint8_t CRC_TABLE[256] = {
    0, 213, 127, 170, 254, 43, 129, 84, 41, 252,
    86, 131, 215, 2, 168, 125, 82, 135, 45, 248,
    172, 121, 211, 6, 123, 174, 4, 209, 133, 80,
    250, 47, 164, 113, 219, 14, 90, 143, 37, 240,
    141, 88, 242, 39, 115, 166, 12, 217, 246, 35,
    137, 92, 8, 221, 119, 162, 223, 10, 160, 117,
    33, 244, 94, 139, 157, 72, 226, 55, 99, 182,
    28, 201, 180, 97, 203, 30, 74, 159, 53, 224,
    207, 26, 176, 101, 49, 228, 78, 155, 230, 51,
    153, 76, 24, 205, 103, 178, 57, 236, 70, 147,
    199, 18, 184, 109, 16, 197, 111, 186, 238, 59,
    145, 68, 107, 190, 20, 193, 149, 64, 234, 63,
    66, 151, 61, 232, 188, 105, 195, 22, 239, 58,
    144, 69, 17, 196, 110, 187, 198, 19, 185, 108,
    56, 237, 71, 146, 189, 104, 194, 23, 67, 150,
    60, 233, 148, 65, 235, 62, 106, 191, 21, 192,
    75, 158, 52, 225, 181, 96, 202, 31, 98, 183,
    29, 200, 156, 73, 227, 54, 25, 204, 102, 179,
    231, 50, 152, 77, 48, 229, 79, 154, 206, 27,
    177, 100, 114, 167, 13, 216, 140, 89, 243, 38,
    91, 142, 36, 241, 165, 112, 218, 15, 32, 245,
    95, 138, 222, 11, 161, 116, 9, 220, 118, 163,
    247, 34, 136, 93, 214, 3, 169, 124, 40, 253,
    87, 130, 255, 42, 128, 85, 1, 212, 126, 171,
    132, 81, 251, 46, 122, 175, 5, 208, 173, 120,
    210, 7, 83, 134, 44, 249
};

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
 * 
 * @return The calculated Checksum
 */
uint8_t ProtocolGeneral::CalculateChecksumOfMessageHeader(uint8_t * message_ptr)
{	
    uint8_t crc = 0;

	uint8_t * pos = (uint8_t *)(message_ptr + _PROTOCOL_VERSION_INDEX);
	const uint8_t * END = (uint8_t *)(message_ptr + _HEADER_CHECKSUM_INDEX);

	while (pos < END) {
		crc = CRC_TABLE[crc ^ *pos];
		pos++;
	}

	return crc;
}

/**
 * @brief Function to Calculate the checksum of the passed messages payload.
 * 
 * @param message_ptr A pointer to the message to return the payloads checksum.
 * 
 * @return The calculated Checksum
 */
uint8_t ProtocolGeneral::CalculateChecksumOfMessagePayload(uint8_t * message_ptr)
{
    uint8_t crc = 0;

	uint8_t * pos = (uint8_t *)(message_ptr + _PAYLOAD_START_INDEX);
	const uint8_t * END = (uint8_t *)(pos + *(message_ptr + _PAYLOAD_LENGTH_INDEX));

	while (pos < END) {
		crc = CRC_TABLE[crc ^ *pos];
		pos++;
	}

	return crc;
}

