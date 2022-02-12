
#include "Ads1299Driver.h"

//
// Constructor
//
Ads1299Driver::Ads1299Driver()
{
    _Ads1299LowDriver = Ads1299LowDriver();
}

//
// Constructor
//
Ads1299Driver::Ads1299Driver(SpiScheduler& spi, PinControl& pins) :
    _Ads1299LowDriver(Ads1299LowDriver(spi, pins))
{
    
}
