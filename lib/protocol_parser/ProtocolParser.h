
#ifndef _PROTOCOL_PARSER
#define _PROTOCOL_PARSER

#include <../protocol_frame_parser/ProtocolFrameParser.h>
#include <EEGDataConsumer.h>
#include <EEGDataProducer.h>
#include <SerialPort.h>

class ProtocolParser : public EegDataConsumer, public CanProcessEvents  {

    public:

        ProtocolParser();
        ProtocolParser(ProtocolFrameParser * pfp, SerialPort * sp, EegDataProducer * edp);

        void PushLatestSample(EegData::sEegSamples samples) override;
        
        void ProcessEvent(NEvent::eEvent event) override;

    protected:

    private:

        static const int _PROTOCOL_PAYLOAD_SIZE = 18;
        static const int _MAX_PAYLOAD_SIZE = 20;

        static const uint8_t _MAX_VALID_ID = 255;

        ProtocolFrameParser * _ProtocolFrameParser;
        SerialPort * _SerialPort;
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
        }sRxStruct;

        sRxStruct _RxState;
        uint8_t _LastValidId;

        static ProtocolParser * _ProtocolParser;

        static sRxStruct GetDefaultRxStruct();

        static sRxStruct RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetProtocolVersion(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetPayloadLength(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetIdNumber(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
        static sRxStruct RxState_GetAcknowledgeId(uint8_t c, sRxStruct state, ProtocolParser * protocolParser);
};

#endif
