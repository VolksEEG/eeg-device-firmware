
#ifndef _ADS1299_DRIVER
#define _ADS1299_DRIVER

#include "Ads1299LowDriver.h"

#include <EEGDataProducer.h>
#include <SpiDriver.h>
#include <PinControl.h>

class Ads1299Driver : EegDataProducer {

    public:

        Ads1299Driver();
        Ads1299Driver(SpiDriver * spi, PinControl * pins);

        void StartDataCapture(void);
        void StopDataCapture(void);

        EegData::sEegSamples GetLatestSample();
        
    protected:

    private:

        Ads1299LowDriver _Ads1299LowDriver;

        uint8_t _SupportedChannels;
};

#endif
