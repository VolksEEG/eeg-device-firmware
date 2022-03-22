
#include "ProtocolParser.h"

//
// Constructor
//
ProtocolParser::ProtocolParser()
{
    
}

ProtocolParser::ProtocolParser(ProtocolFrameParser * pfp) :
    _ProtocolFrameParser(pfp)
{

}

//
//  Overridden function from EegDataConsumer used to push EEG samples to the PC.
//
void ProtocolParser::PushLatestSample(EegData::sEegSamples samples)
{
    uint8_t payload[16] = {
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

    _ProtocolFrameParser->AddFrameToPayloadAndSendToPc(payload, 16);
}
