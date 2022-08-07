#include "Ads1299DataProcessor.h"

#include <string.h>

//
// Constructor
//
Ads1299DataProcessor::Ads1299DataProcessor()
{
    
}

//
// Constructor
//
Ads1299DataProcessor::Ads1299DataProcessor(Ads1299Driver * ads, EventHandler * eh, ErrorHandler * erh) :
    _Ads1299Driver_ptr(ads),
    _EventHandler_ptr(eh),
    _ErrorHandler_ptr(erh),
    _AdsSampleBufferInputIndex(0),
    _AdsSampleBufferOutputIndex(0),
    _AdsSampleBufferCount(0)
{
    for (int i = 0; i < _MAX_SAMPLES_TO_BUFFER; ++i)
    {
        _AdsBufferedData[i].channel_1 = 0;
        _AdsBufferedData[i].channel_2 = 0;
        _AdsBufferedData[i].channel_3 = 0;
        _AdsBufferedData[i].channel_4 = 0;
        _AdsBufferedData[i].channel_5 = 0;
        _AdsBufferedData[i].channel_6 = 0;
        _AdsBufferedData[i].channel_7 = 0;
        _AdsBufferedData[i].channel_8 = 0;
    }
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

    if (_AdsSampleBufferCount >= _MAX_SAMPLES_TO_BUFFER)
    {
        // no space remaining, so raise an error and do not attempt to buffer the sample
        _ErrorHandler_ptr->RaiseError(ErrorHandler::eError::Error_AdsSampleUnableToBeBuffered);

        return;
    }

    EegData::sEegSamples eegSamples;

    // get the latest sample
    Ads1299Driver::sAds1299SampleData adsSamples = _Ads1299Driver_ptr->GetLatestSampleData();

    eegSamples.channel_1 = adsSamples.SampleData[0];
    eegSamples.channel_2 = adsSamples.SampleData[1];
    eegSamples.channel_3 = adsSamples.SampleData[2];
    eegSamples.channel_4 = adsSamples.SampleData[3];
    eegSamples.channel_5 = adsSamples.SampleData[4];
    eegSamples.channel_6 = adsSamples.SampleData[5];
    eegSamples.channel_7 = adsSamples.SampleData[6];
    eegSamples.channel_8 = adsSamples.SampleData[7];
    
    // buffer it
    _AdsBufferedData[_AdsSampleBufferInputIndex++] = eegSamples;

    // wrap around the input index
    if (_AdsSampleBufferInputIndex == _MAX_SAMPLES_TO_BUFFER)
    {
        _AdsSampleBufferInputIndex = 0;
    }

    // track the count
    _AdsSampleBufferCount++;

    if (1 == _AdsSampleBufferCount)
    {
        // if this if the first sample then signal the buffered ADS1299 Data ready event
        _EventHandler_ptr->SignalEvent(NEvent::Event_BufferedADS1299DataReady);        
    }
}

//
//  Overridden function from EegDataProducer to start producing EEG data
//
void Ads1299DataProcessor::StartProducingData()
{
    _Ads1299Driver_ptr->StartProducingData(Ads1299LowDriver::eSampleRate::SPS_500);
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
    EegData::sEegSamples eegSamples = _AdsBufferedData[_AdsSampleBufferOutputIndex++];

    // wrap around the input index
    if (_AdsSampleBufferOutputIndex == _MAX_SAMPLES_TO_BUFFER)
    {
        _AdsSampleBufferOutputIndex = 0;
    }

    // track the count
    _AdsSampleBufferCount--;

    if (0 != _AdsSampleBufferCount)
    {
        _EventHandler_ptr->SignalEvent(NEvent::Event_BufferedADS1299DataReady);
    }

    return eegSamples;
}
