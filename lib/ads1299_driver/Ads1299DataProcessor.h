
#ifndef _ADS1299_DATA_PROCESSOR
#define _ADS1299_DATA_PROCESSOR

#include "Ads1299DataProcessor.h"

#include "Ads1299Driver.h"

#include <EEGDataProducer.h>
#include <EegData.h>

class Ads1299DataProcessor : public EegDataProducer, public CanProcessEvents  {

    public:

        Ads1299DataProcessor();
        Ads1299DataProcessor(Ads1299Driver * ads, EventHandler * eh);

        void StartProducingData() override; 
        void StopProducingData() override; 

        EegData::sEegSamples GetLatestSample() override;
        
        void ProcessEvent(NEvent::eEvent event) override;
        
    protected:

    private:

        Ads1299Driver * _Ads1299Driver_ptr;
        EventHandler * _EventHandler_ptr;

        int _SamplesCaptured;

        int32_t _SumOfLastSamples1;
        int32_t _SumOfLastSamples2;
        int32_t _SumOfLastSamples3;
        int32_t _SumOfLastSamples4;
        int32_t _SumOfLastSamples5;
        int32_t _SumOfLastSamples6;
        int32_t _SumOfLastSamples7;
        int32_t _SumOfLastSamples8;

        EegData::sEegSamples _AveragedSamples;

};

#endif
