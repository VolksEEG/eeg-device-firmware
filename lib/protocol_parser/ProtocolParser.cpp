
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
    (void)samples;
}
