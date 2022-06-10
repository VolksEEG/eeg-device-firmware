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
    _EventHandler_ptr(eh)
{
    
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

    // For now, just signal the Processed ADS1299 Data ready event
     _EventHandler_ptr->SignalEvent(NEvent::Event_ProcessedADS1299DataReady);
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
    EegData::sEegSamples eegSamples;

    Ads1299Driver::sAds1299SampleData adsSamples = _Ads1299Driver_ptr->GetLatestSampleData();

    eegSamples.channel_1 = adsSamples.SampleData[0];
    eegSamples.channel_2 = adsSamples.SampleData[1];
    eegSamples.channel_3 = adsSamples.SampleData[2];
    eegSamples.channel_4 = adsSamples.SampleData[3];
    eegSamples.channel_5 = adsSamples.SampleData[4];
    eegSamples.channel_6 = adsSamples.SampleData[5];
    eegSamples.channel_7 = adsSamples.SampleData[6];
    eegSamples.channel_8 = adsSamples.SampleData[7];
    
    return eegSamples;
}
