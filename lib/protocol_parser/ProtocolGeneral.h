
#ifndef PROTOCOl_GENERAL
#define PROTOCOl_GENERAL

#include <stdint.h>

class ProtocolGeneral 
{
    public:

        // Size definitions
#ifndef PIO_UNIT_TESTING
        static const int _MAX_PAYLOAD_SIZE = 255;
#else
        // payload size is 1 less for unit testing to allow payloads greater than the max to be tested
        static const int _MAX_PAYLOAD_SIZE = 254;
#endif
        static const int _HEADER_SIZE = 8;
        static const int _MAX_MESSAGE_LENGTH = _HEADER_SIZE + _MAX_PAYLOAD_SIZE;

        // message value definitions
        static const uint8_t _SYNC_WORD_MSB = 0xAA;
        static const uint8_t _SYNC_WORD_LSB = 0x55;
        static const uint8_t _IMPLEMENTED_PROTOCOL_VERSION = 0x01; // Version 0.1
        static const uint8_t _MAX_VALID_ID = 255;

        // message byte index definitions
        static const uint8_t _SYNC_WORD_MSB_INDEX = 0;
        static const uint8_t _SYNC_WORD_LSB_INDEX = 1;
        static const uint8_t _PROTOCOL_VERSION_INDEX = 2;
        static const uint8_t _PAYLOAD_LENGTH_INDEX = 3;
        static const uint8_t _ID_NUMBER_INDEX = 4;
        static const uint8_t _ID_ACKNOWLEDGE_INDEX = 5;
        static const uint8_t _PAYLOAD_CHECKSUM_INDEX = 6;
        static const uint8_t _HEADER_CHECKSUM_INDEX = 7;
        static const uint8_t _PAYLOAD_START_INDEX = 8;

        // command groups and commands
        typedef enum _COMS_COMMAND_GROUPS
        {
            GROUP_ACKNOWLEDGE = 0x00,
            GROUP_QUERY = 0x20,
            GROUP_WRITE = 0x40
        }eComsCommandGroup;

        typedef enum _COMS_COMMANDS
        {
            CMD_ACKNOWLEDGE = 0x00,
            CMD_NOT_ACKNOWLEDGE = 0x10,
            CMD_EEG_DATA_MODE = 0x01,
            CMD_EEG_DATA_VALUES = 0x02
        }eComsCommands;

        ProtocolGeneral();

        static uint8_t CalculateChecksumOfMessageHeader(uint8_t * message_ptr);
        static uint8_t CalculateChecksumOfMessagePayload(uint8_t * message_ptr);

    protected:

    private:

};

#endif
