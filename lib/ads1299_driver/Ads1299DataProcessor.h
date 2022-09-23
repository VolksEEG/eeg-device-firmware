
#ifndef _ADS1299_DATA_PROCESSOR
#define _ADS1299_DATA_PROCESSOR

#include "Ads1299Driver.h"

#include <IEEGDataProducer.h>
#include <EegData.h>
#include <ErrorHandler.h>

class Ads1299DataProcessor : public IEegDataProducer, public ICanProcessEvents  {

    public:

        Ads1299DataProcessor();
        Ads1299DataProcessor(Ads1299Driver * ads, EventHandler * eh, ErrorHandler * erh);

        void StartProducingData() override; 
        void StopProducingData() override; 

        EegData::sEegSamples GetLatestSample() override;
        
        void ProcessEvent(NEvent::eEvent event) override;
        
    protected:

    private:

        Ads1299Driver * _Ads1299Driver_ptr;
        EventHandler * _EventHandler_ptr;
        ErrorHandler * _ErrorHandler_ptr;

        static const uint16_t _MAX_SAMPLES_TO_BUFFER = 50;      // 100 milliSeconds of data

        EegData::sEegSamples _AdsBufferedData[_MAX_SAMPLES_TO_BUFFER];
        uint16_t _AdsSampleBufferInputIndex;
        uint16_t _AdsSampleBufferOutputIndex;
        uint16_t _AdsSampleBufferCount;
};

#endif
