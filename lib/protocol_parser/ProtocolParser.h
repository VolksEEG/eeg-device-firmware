
#ifndef _PROTOCOL_PARSER
#define _PROTOCOL_PARSER

#include "PcCommunicationsInterface.h"

#include <EEGDataConsumer.h>
#include <EEGDataProducer.h>
#include <SerialPort.h>

class ProtocolParser : public EegDataConsumer, public CanProcessEvents  {

    public:

        ProtocolParser();
        ProtocolParser(PcCommunicationsInterface * pci, EegDataProducer * edp);

        void PushLatestSample(EegData::sEegSamples samples) override;
        
        void ProcessEvent(NEvent::eEvent event) override;

        #ifdef PIO_UNIT_TESTING

        typedef enum _RX_STATES
        {
            WaitForSequence,
            GetProtocolVersion,
            GetPayloadLength,
            GetIdNumber,
            GetAckId,
            GetChecksum,
            GetPayload,
            InvalidState
        }RX_STATE;

        RX_STATE GetCurrentRxState();

        uint8_t GetImplementedProtocolVersion();

        uint8_t GetMaximumPayloadLength();

        uint8_t GetNextExpectedId();

        #endif
    protected:

    private:

        static const int _PROTOCOL_PAYLOAD_SIZE = 18;
        static const int _MAX_PAYLOAD_SIZE = 20;

        static const uint8_t _MAX_VALID_ID = 255;

        static const uint8_t _IMPLEMENTED_PROTOCOL_VERSION = 0x01; // Version 0.1

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

        PcCommunicationsInterface * _PcComsInterface;
        EegDataProducer * _EEGDataProducer;

        typedef struct _RX_STATE
        {
            // reception varaibles
            _RX_STATE (*state_fptr)(uint8_t c, _RX_STATE state, ProtocolParser * protocolParser);
            uint8_t rxIndex;
            uint8_t rxMultiByteCounter;

            // message values
            uint8_t payload[_MAX_PAYLOAD_SIZE];
            uint8_t payloadLength;
            uint8_t messageID;
            uint8_t checksum;

            uint8_t nextExpectedId;
        }sRxStruct;

        sRxStruct _RxState;

        void SendPayloadToPc(uint8_t * payload_ptr, uint8_t payloadLength);

        static ProtocolParser * _ProtocolParser;

        static sRxStruct ResetRxStruct(sRxStruct state);
        static uint8_t CalculateChecksum(sRxStruct state);

        static sRxStruct RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetProtocolVersion(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetPayloadLength(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetIdNumber(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetAcknowledgeId(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetChecksum(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetPayload(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
};

#endif
