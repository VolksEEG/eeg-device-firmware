
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
//  Sets all the low level registers to start data capture.
//
void Ads1299Driver::StartDataCapture(void)
{
    // ensure the gain is set correctly
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelGain::X1);
    _Ads1299LowDriver.SetChannelGain(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelGain::X1);

    // Ensure the channel states are correct
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH1, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH2, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH3, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH4, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH5, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH6, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH7, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);
    _Ads1299LowDriver.SetChannelState(Ads1299LowDriver::eChannelId::CH8, Ads1299LowDriver::eChannelState::OnAndMeasuringTestSignal);

    _Ads1299LowDriver.SetTestSignal();
    
    // And start data capture
    _Ads1299LowDriver.StartContinuousDataCapture(Ads1299LowDriver::eSampleRate::SPS_500);
}

//
//  Sets all the low level registers to stop data capture.
//
void Ads1299Driver::StopDataCapture(void)
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
//  Overridden function from EegDataProducer to start producing EEG data
//
void Ads1299Driver::StartProducingData()
{
    
}

//
//  Overridden function from EegDataProducer to get the latest sample
//
EegData::sEegSamples Ads1299Driver::GetLatestSample()
{
    // call the low level driver
    const Ads1299LowDriver::sEMGData EMG = _Ads1299LowDriver.GetEMGData();

    EegData::sEegSamples samples;

    // TODO - Convert to microvolts.
    samples.channel_1 = (int16_t)(EMG.channel1 >> 8);
    samples.channel_2 = (int16_t)(EMG.channel2 >> 8);
    samples.channel_3 = (int16_t)(EMG.channel3 >> 8);
    samples.channel_4 = (int16_t)(EMG.channel4 >> 8);
    samples.channel_5 = (int16_t)(EMG.channel5 >> 8);
    samples.channel_6 = (int16_t)(EMG.channel6 >> 8);
    samples.channel_7 = (int16_t)(EMG.channel7 >> 8);
    samples.channel_8 = (int16_t)(EMG.channel8 >> 8);

    return samples;
}
