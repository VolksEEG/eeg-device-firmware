
#include "Ads1299LowDriver.h"

//
// Constructor
//
Ads1299LowDriver::Ads1299LowDriver()
{

}

//
// Constructor
//
Ads1299LowDriver::Ads1299LowDriver(SpiDriver& spi, PinControl& pins) :
    _SpiDriver(spi),
    _PinControl(pins)
{

}

//
//  Sends the command to Reset the ADS1299 device
//
void Ads1299LowDriver::ResetDevice(void)
{
    uint8_t resetData[1] = { COMMAND_RESET };

    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    resetData,
                                    1);                      
}

//
//  Sets up the Device to start capturing data continuously at the set rate
//
void Ads1299LowDriver::StartContinuousDataCapture(eSampleRate rate)
{
    uint8_t startData[1] = { COMMAND_START };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    startData,
                                    1);     

    uint8_t readContinuousData[1] = { COMMAND_READ_DATA_CONTINUOUSLY };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readContinuousData,
                                    1);     
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
    // Read the ID Register.
    uint8_t readIdRegData[2] = { (COMMAND_READ_REGISTER | REGISTER_ID ), 0x00 };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readIdRegData,
                                    2);

    // TODO - parse and return the number of channels.    
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
