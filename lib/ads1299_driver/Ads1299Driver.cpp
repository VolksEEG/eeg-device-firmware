
#include "Ads1299Driver.h"

#include "Ads1299DataConverter.h"

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
Ads1299Driver::Ads1299Driver(SpiDriver * spi, PinControl * pins) :
    _Ads1299LowDriver(Ads1299LowDriver(spi, pins))
{
    // Device defaults to capturing at 250SPS so stop this.
    _Ads1299LowDriver.StopContinuousDataCapture();

    // first reset the device.
    _Ads1299LowDriver.ResetDevice();

    // Again device defaults to capturing at 250SPS so stop this.
    _Ads1299LowDriver.StopContinuousDataCapture();

    // Get the number of channels, just because.
    _SupportedChannels = _Ads1299LowDriver.GetNumberOfSupportedChannels();

    // We will be using the internal reference, this could be passed as a parameter
    // to the constructor in future.
    _Ads1299LowDriver.SetReferenceSource(Ads1299LowDriver::eReferenceSource::Internal);
}

//
//  Overridden function from EegDataProducer to start producing EEG data
//
void Ads1299Driver::StartProducingData()
{
    // ensure the gain is set correctly
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelGain::X12);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelGain::X12);

    // Ensure the channel states are correct
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelState::OnNormal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelState::OnNormal);

    // And start data capture
    _Ads1299LowDriver.StartContinuousDataCapture(Ads1299LowDriver::eSampleRate::SPS_500);    
}

//
//  Overridden function from EegDataProducer to stop producing EEG data
//
void Ads1299Driver::StopProducingData()
{
    // Stop data capture
    _Ads1299LowDriver.StopContinuousDataCapture();

    // Ensure the channel states are Off 
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelState::OffAndShorted);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelState::OffAndShorted);   
}

//
//  Function to get the latest sample data from the ADS1299
//
Ads1299Driver::sAds1299SampleData Ads1299Driver::GetLatestSampleData()
{
    const Ads1299LowDriver::sAds1299AllChannelsData RAW_DATA = _Ads1299LowDriver.GetAds1299Data();

    Ads1299Driver::sAds1299SampleData convertedData;

    // Convert the ADS1299 Sample counts to micro Volts.
    Ads1299DataConverter converter = Ads1299DataConverter();

    for (int i = 0; i < _Ads1299LowDriver.MAX_CHANNELS; ++i)
    {
        convertedData.SampleData[i] = converter.ConvertAds1299CountToMicroVolts(RAW_DATA.channels[i].count, RAW_DATA.channels[i].gain, RAW_DATA.vrefVoltage);
    }

    return convertedData;
}
