
#ifndef _ADS1299_DRIVER
#define _ADS1299_DRIVER

#include "Ads1299LowDriver.h"

#include <SpiDriver.h>
#include <PinControl.h>

class Ads1299Driver {

    public:

        typedef enum _MONTAGE
        {
            Sequential,
            Referential
        }eMontage;

        Ads1299Driver();
        Ads1299Driver(SpiDriver * spi, PinControl * pins, eMontage montage);

        void StartProducingData(Ads1299LowDriver::eSampleRate rate); 
        void StopProducingData(); 

        typedef struct _ADS_SAMPLE_DATA
        {
            int16_t SampleData[Ads1299LowDriver::MAX_CHANNELS];
        }sAds1299SampleData;

        Ads1299Driver::sAds1299SampleData GetLatestSampleData();
        
    protected:

    private:

        Ads1299LowDriver _Ads1299LowDriver;

        uint8_t _SupportedChannels;
};

#endif
