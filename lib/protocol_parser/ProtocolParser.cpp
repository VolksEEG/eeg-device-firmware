
#include "ProtocolParser.h"

#include <string.h>

//
// Constructor
//
ProtocolParser::ProtocolParser()
{
    
}

ProtocolParser::ProtocolParser(ProtocolFrameParser * pfp, SerialPort * sp, EegDataProducer * edp) :
    _ProtocolFrameParser(pfp),
    _SerialPort(sp),
    _EEGDataProducer(edp),
    _SampleSetIndex(0)
{
    _RxState = GetDefaultRxStruct();
}

//
//  Overriden function for processing events.
//
void ProtocolParser::ProcessEvent(NEvent::eEvent event)
{
    if (NEvent::Event_DataRxFromPC != event)
    {
        return;
    }

    uint8_t buf[1];

    const uint8_t RX_COUNT = _SerialPort->GetReceivedBytes(buf, 1);

    if (0 == RX_COUNT)
    {
        return;
    }

    _RxState = _RxState.state_fptr(buf[0], _RxState, this);
}

//
//  Overridden function from EegDataConsumer used to push EEG samples to the PC.
//
void ProtocolParser::PushLatestSample(EegData::sEegSamples samples)
{
    uint8_t payload[_PROTOCOL_PAYLOAD_SIZE] = {
        (uint8_t)(_SampleSetIndex & 0x00FF),
        (uint8_t)((_SampleSetIndex & 0xFF00) >> 8),
        (uint8_t)(samples.channel_1 & 0x00FF),
        (uint8_t)((samples.channel_1 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_2 & 0x00FF),
        (uint8_t)((samples.channel_2 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_3 & 0x00FF),
        (uint8_t)((samples.channel_3 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_4 & 0x00FF),
        (uint8_t)((samples.channel_4 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_5 & 0x00FF),
        (uint8_t)((samples.channel_5 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_6 & 0x00FF),
        (uint8_t)((samples.channel_6 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_7 & 0x00FF),
        (uint8_t)((samples.channel_7 & 0xFF00) >> 8),
        (uint8_t)(samples.channel_8 & 0x00FF),
        (uint8_t)((samples.channel_8 & 0xFF00) >> 8)
    };

    _ProtocolFrameParser->AddFrameToPayloadAndSendToPc(payload, _PROTOCOL_PAYLOAD_SIZE);

    _SampleSetIndex++;
}

// 
//  Data reception state function to wait for the sync sequence
//
ProtocolParser::sRxStruct ProtocolParser::RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    static const uint8_t SYNC_SEQ_BYTE = 0xFF;
    static const uint8_t SYNC_SEQ_BYTE_COUNT = 2;

    if (c != SYNC_SEQ_BYTE)
    {
        return GetDefaultRxStruct();
    }

    if (++state.rxMultiByteCounter < SYNC_SEQ_BYTE_COUNT)
    {
        return state;
    }

    state.rxMultiByteCounter = 0;
    state.state_fptr = RxState_GetCommand;

    return state;
}

// 
//  Data reception state function to process the received command
//
ProtocolParser::sRxStruct ProtocolParser::RxState_GetCommand(uint8_t c, sRxStruct state, ProtocolParser * protocolParser)
{
    static const uint8_t CMD_START_DATA_CAPTURE = 0x01;
    static const uint8_t CMD_STOP_DATA_CAPTURE = 0x02;

    switch (c)
    {
        case CMD_START_DATA_CAPTURE:
            protocolParser->_EEGDataProducer->StartProducingData();//_ProtocolParser->_EEGDataProducer->StartProducingData();
            break;
        case CMD_STOP_DATA_CAPTURE:
            protocolParser->_EEGDataProducer->StopProducingData();//_ProtocolParser->_EEGDataProducer->StopProducingData();
            break;
        default: 
            // Do nothing if the command is not recognised.
            break;
    }

    return GetDefaultRxStruct();
}

//
//  Helper function to get a default rx state struct
//
ProtocolParser::sRxStruct ProtocolParser::GetDefaultRxStruct(void)
{
    ProtocolParser::sRxStruct temp;

    memset(temp.rxData, 0, _RX_DATA_SIZE);

    temp.rxIndex = 0;
    temp.rxMultiByteCounter = 0;

    temp.state_fptr = RxState_WaitForSyncSequence;

    return temp;
}
