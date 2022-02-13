
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
    _Ads1299LowDriver.StopContinuousDataCapture();

    // first reset the device.
    _Ads1299LowDriver.ResetDevice();

    _Ads1299LowDriver.StopContinuousDataCapture();

    _SupportedChannels = _Ads1299LowDriver.GetNumberOfSupportedChannels();

    _Ads1299LowDriver.SetReferenceSource(Ads1299LowDriver::eReferenceSource::Internal);

    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelGain::X1);

    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);

    _Ads1299LowDriver.SetTestSignal();
    
    _Ads1299LowDriver.StartContinuousDataCapture(Ads1299LowDriver::eSampleRate::SPS_500);

    //_Ads1299LowDriver.StopContinuousDataCapture();

    //_Ads1299LowDriver.ReadBackRegisters();

}

//
//  TODO - replace this with an actual way of scheduling data reads.
//
void Ads1299Driver::ProcessEvents(void)
{
    // data ready line it active
    volatile Ads1299LowDriver::sEMGData emg = _Ads1299LowDriver.GetEMGData();
}
