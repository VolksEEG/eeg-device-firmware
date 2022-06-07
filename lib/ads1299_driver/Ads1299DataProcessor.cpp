#include "Ads1299DataProcessor.h"

//
// Constructor
//
Ads1299DataProcessor::Ads1299DataProcessor()
{
    
}

//
// Constructor
//
Ads1299DataProcessor::Ads1299DataProcessor(Ads1299Driver * ads, EventHandler * eh) :
    _Ads1299Driver_ptr(ads),
    _EventHandler_ptr(eh),
    _SamplesCaptured(0),
    _SumOfLastSamples1(0),
    _SumOfLastSamples2(0),
    _SumOfLastSamples3(0),
    _SumOfLastSamples4(0),
    _SumOfLastSamples5(0),
    _SumOfLastSamples6(0),
    _SumOfLastSamples7(0),
    _SumOfLastSamples8(0)
{
    _AveragedSamples.channel_1 = 0;
    _AveragedSamples.channel_2 = 0;
    _AveragedSamples.channel_3 = 0;
    _AveragedSamples.channel_4 = 0;
    _AveragedSamples.channel_5 = 0;
    _AveragedSamples.channel_6 = 0;
    _AveragedSamples.channel_7 = 0;
    _AveragedSamples.channel_8 = 0;
}

//
// Overriden function from CanProcessEvents
//
void Ads1299DataProcessor::ProcessEvent(NEvent::eEvent event)
{
    if (NEvent::Event_ADS1299DataReady != event)
    {
        return;
    }

    const EegData::sEegSamples SAMPLES = _Ads1299Driver_ptr->GetLatestSample();

    _SumOfLastSamples1 += SAMPLES.channel_1;
    _SumOfLastSamples2 += SAMPLES.channel_2;
    _SumOfLastSamples3 += SAMPLES.channel_3;
    _SumOfLastSamples4 += SAMPLES.channel_4;
    _SumOfLastSamples5 += SAMPLES.channel_5;
    _SumOfLastSamples6 += SAMPLES.channel_6;
    _SumOfLastSamples7 += SAMPLES.channel_7;
    _SumOfLastSamples8 += SAMPLES.channel_8;

    if (++_SamplesCaptured == 4)
    {
        _EventHandler_ptr->SignalEvent(NEvent::Event_ProcessedADS1299DataReady);

        _SamplesCaptured = 0;

        _AveragedSamples.channel_1 = (int16_t)_SumOfLastSamples1 / 4;
        _AveragedSamples.channel_2 = (int16_t)_SumOfLastSamples2 / 4;
        _AveragedSamples.channel_3 = (int16_t)_SumOfLastSamples3 / 4;
        _AveragedSamples.channel_4 = (int16_t)_SumOfLastSamples4 / 4;
        _AveragedSamples.channel_5 = (int16_t)_SumOfLastSamples5 / 4;
        _AveragedSamples.channel_6 = (int16_t)_SumOfLastSamples6 / 4;
        _AveragedSamples.channel_7 = (int16_t)_SumOfLastSamples7 / 4;
        _AveragedSamples.channel_8 = (int16_t)_SumOfLastSamples8 / 4;
            
        _SumOfLastSamples1 = 0;
        _SumOfLastSamples2 = 0;
        _SumOfLastSamples3 = 0;
        _SumOfLastSamples4 = 0;
        _SumOfLastSamples5 = 0;
        _SumOfLastSamples6 = 0;
        _SumOfLastSamples7 = 0;
        _SumOfLastSamples8 = 0;
    }
}

//
//  Overridden function from EegDataProducer to start producing EEG data
//
void Ads1299DataProcessor::StartProducingData()
{
    _Ads1299Driver_ptr->StartProducingData();
}

//
//  Overridden function from EegDataProducer to stop producing EEG data
//
void Ads1299DataProcessor::StopProducingData()
{
    _Ads1299Driver_ptr->StopProducingData();
}

//
//  Overriden function from EegDataProducer to get the latest samples.
//
EegData::sEegSamples Ads1299DataProcessor::GetLatestSample()
{
    return _AveragedSamples;
}
