
#ifndef _ADS1299_DATA_PROCESSOR
#define _ADS1299_DATA_PROCESSOR

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
};

#endif
