
#include "FakeDataProducer.h"

//
// Constructor
//
FakeDataProducer::FakeDataProducer()
{

}

//
// Constructor
//
FakeDataProducer::FakeDataProducer(EventHandler * eh) : 
    _EventHandlerPtr(eh),
    _TimerDivider(0)
{
    _SampleData.channel_1 = 0;
    _SampleData.channel_2 = 1;
    _SampleData.channel_3 = 2;
    _SampleData.channel_4 = 3;
    _SampleData.channel_5 = 4;
    _SampleData.channel_6 = 5;
    _SampleData.channel_7 = 6;
    _SampleData.channel_8 = 7;
}

//
// Overriden function from CanProcessEvents
//
void FakeDataProducer::ProcessEvent(NEvent::eEvent event)
{
    _TimerDivider = (_TimerDivider + 1) % 2;

    // has timer divider "overflowed"
    if (0 ==  _TimerDivider)
    {
        // set the next sample
        _SampleData.channel_1 = _SampleData.channel_2;
        _SampleData.channel_2 = _SampleData.channel_3;
        _SampleData.channel_3 = _SampleData.channel_4;
        _SampleData.channel_4 = _SampleData.channel_5;
        _SampleData.channel_5 = _SampleData.channel_6;
        _SampleData.channel_6 = _SampleData.channel_7;
        _SampleData.channel_7 = _SampleData.channel_8;
        _SampleData.channel_8++;

        if (_SampleData.channel_8 == 100)
        {
            _SampleData.channel_8 = 0;
        }

        // signal the data ready event
        _EventHandlerPtr->SignalEvent(NEvent::Event_EDFDataReady);
    }
}

//
//  Overridden function from EegDataProducer to start producing EEG data
//
void FakeDataProducer::StartProducingData()
{

}

//
//  Overriden function from EegDataProducer to get the latest samples.
//
EegData::sEegSamples FakeDataProducer::GetLatestSample()
{
    return _SampleData;
}
