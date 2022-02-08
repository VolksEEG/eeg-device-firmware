
#include "Ads1299LowDriver.h"

//
// Constructor
//
Ads1299LowDriver::Ads1299LowDriver()
{
}

//
//  Sends the command to Reset the ADS1299 device
//
void Ads1299LowDriver::ResetDevice(void)
{
    // TODO - Reset the internal state.
}

//
//  Sets up the Device to start capturing data continuously at the set rate
//
void Ads1299LowDriver::StartContinuousDataCapture(eSampleRate rate)
{

}

//
//  Instructs the device to stop capturing data continuously
//
void Ads1299LowDriver::StopContinuousDataCapture(void)
{

}

//
//  Gets the number of channels supported by the ADS1299.
//
uint8_t Ads1299LowDriver::GetNumberOfSupportedChannels(void)
{

}

//
//  Configures each channels data capture state
//
void Ads1299LowDriver::SetChannelState(eChannelId chan, eChannelState state)
{

}

//
//  Sets the gain for the selected channel
//
void Ads1299LowDriver::SetChannelGain(eChannelId chan, eChannelGain gain)
{

}
