
#ifndef _PROTOCOL_PARSER
#define _PROTOCOL_PARSER

#include <ProtocolFrameParser.h>
#include <EEGDataConsumer.h>
#include <SerialPort.h>

class ProtocolParser : public EegDataConsumer, public CanProcessEvents  {

    public:

        ProtocolParser();
        ProtocolParser(ProtocolFrameParser * pfp, SerialPort * sp);

        void PushLatestSample(EegData::sEegSamples samples) override;
        
        void ProcessEvent(NEvent::eEvent event) override;

    protected:

    private:

        static const int _PROTOCOL_PAYLOAD_SIZE = 18;
        static const int _RX_DATA_SIZE = 20;

        ProtocolFrameParser * _ProtocolFrameParser;
        SerialPort * _SerialPort;

        short _SampleSetIndex;

        typedef struct _RX_STATE
        {
            _RX_STATE (*state_fptr)(uint8_t c, _RX_STATE);
            uint8_t rxData[_RX_DATA_SIZE];
            uint8_t rxIndex;
            uint8_t rxMultiByteCounter;
        }sRxStruct;


        sRxStruct _RxState;

        static sRxStruct GetDefaultRxStruct(void);

        static sRxStruct RxState_WaitForSyncSequence(uint8_t c, sRxStruct state);
        static sRxStruct RxState_GetCommand(uint8_t c, sRxStruct state);
};

#endif
