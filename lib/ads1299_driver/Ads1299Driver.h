
#ifndef _ADS1299_DRIVER
#define _ADS1299_DRIVER

#include "Ads1299LowDriver.h"
#include <SpiScheduler.h>
#include <PinControl.h>

class Ads1299Driver {

    public:

        Ads1299Driver();
        Ads1299Driver(SpiScheduler& spi, PinControl& pins);

    protected:

    private:

        Ads1299LowDriver _Ads1299LowDriver;
};

#endif
