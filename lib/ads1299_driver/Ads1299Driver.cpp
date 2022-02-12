
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
Ads1299Driver::Ads1299Driver(SpiDriver& spi, PinControl& pins) :
    _Ads1299LowDriver(Ads1299LowDriver(spi, pins))
{
    // first reset the device.
    _Ads1299LowDriver.ResetDevice();

    _SupportedChannels = _Ads1299LowDriver.GetNumberOfSupportedChannels();
}
