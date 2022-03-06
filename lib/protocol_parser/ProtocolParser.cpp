
#include "ProtocolParser.h"

//
// Constructor
//
ProtocolParser::ProtocolParser() :
    mValue(0)
{
    
}

ProtocolParser::ProtocolParser(ProtocolFrameParser * pfp) :
    mValue(0),
    _ProtocolFrameParser(pfp)
{

}

//
//  GetValues function to test out unit tests
//
int ProtocolParser::GetValue(void)
{
    return this->mValue;
}

//
//  SetValues function to test out unit tests
//
void ProtocolParser::SetValue(int newValue)
{
    this->mValue = newValue;
}

//
//  Overridden function from EegDataConsumer used to push EEG samples to the PC.
//
void ProtocolParser::PushLatestSample(EegData::sEegSamples samples)
{
    (void)samples;
}
