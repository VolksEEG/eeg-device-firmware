
#ifndef _ADS1299_DRIVER
#define _ADS1299_DRIVER

#include "Ads1299LowDriver.h"

#include <EEGDataProducer.h>
#include <SpiDriver.h>
#include <PinControl.h>

class Ads1299Driver : public EegDataProducer {

    public:

        Ads1299Driver();
        Ads1299Driver(SpiDriver * spi, PinControl * pins);

        void StartProducingData() override; 
        void StopProducingData() override; 

        EegData::sEegSamples GetLatestSample() override;
        
    protected:

    private:

        Ads1299LowDriver _Ads1299LowDriver;

        uint8_t _SupportedChannels;
};

#endif
