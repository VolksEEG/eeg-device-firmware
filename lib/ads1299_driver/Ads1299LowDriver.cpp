
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
    const uint8_t RATE = GetSampleRateValueFromSampleRateEnum(rate);

    ModifyRegister(REGISTER_CONFIG_1, CONFIG_1_DATARATE_MASK, RATE);

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

    uint8_t stopContinuousData[1] = { COMMAND_STOP_READING_DATA_CONTINUOUSLY };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    stopContinuousData,
                                    1);   

    uint8_t stopData[1] = { COMMAND_STOP };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    stopData,
                                    1);     
}

//
//  Gets the number of channels supported by the ADS1299.
//
uint8_t Ads1299LowDriver::GetNumberOfSupportedChannels(void)
{
    // Read the ID Register.
    const uint8_t ID = ReadRegister(REGISTER_ID);

    // the 3rd byte of the received data will be the ID register
    switch (ID & ID_NUMBER_OF_CHANNELS_MASK)
    {
        case ID_NUMBER_OF_CHANNELS_4:
            return 4;
            break;
        case ID_NUMBER_OF_CHANNELS_6:
            return 6;
            break;
        case ID_NUMBER_OF_CHANNELS_8:
            return 8;
            break;
        default:
            return 0;
            break;
    } 
}

//
//  Configures each channels data capture state
//
void Ads1299LowDriver::SetChannelState(eChannelId chan, eChannelState state)
{
    const uint8_t REG = GetChannelRegisterFromChannelIdEnum(chan);

    const uint8_t STATE = GetChannelStateValueFromChannelStateEnum(state);
    
    ModifyRegister(REG, CHNSET_STATE_MASK, STATE);
}

//
//  Sets the gain for the selected channel
//
void Ads1299LowDriver::SetChannelGain(eChannelId chan, eChannelGain gain)
{
    const uint8_t REG = GetChannelRegisterFromChannelIdEnum(chan);

    const uint8_t GAIN = GetGainFromFromChannelGainEnum(gain);

    ModifyRegister(REG, CHNSET_GAIN_MASK, GAIN);
}

//
//  Function to request the latest channel data
//
Ads1299LowDriver::sEMGData Ads1299LowDriver::GetEMGData(void)
{
    uint8_t emgData[27] = {0};  // 3 bytes for each of the 8 emg channels and 1 status channel

    // Get the EMG Data
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    emgData,
                                    27);

    // Got the data, now parse it
    sEMGData emg;

    emg.status = (emgData[2] << 16) | (emgData[1] << 8) | (emgData[0]);
    emg.channel1 = (emgData[5] << 16) | (emgData[4] << 8) | (emgData[3]);
    emg.channel2 = (emgData[8] << 16) | (emgData[7] << 8) | (emgData[6]);
    emg.channel3 = (emgData[11] << 16) | (emgData[10] << 8) | (emgData[9]);
    emg.channel4 = (emgData[14] << 16) | (emgData[13] << 8) | (emgData[12]);
    emg.channel5 = (emgData[17] << 16) | (emgData[16] << 8) | (emgData[15]);
    emg.channel6 = (emgData[20] << 16) | (emgData[19] << 8) | (emgData[18]);
    emg.channel7 = (emgData[23] << 16) | (emgData[22] << 8) | (emgData[21]);
    emg.channel8 = (emgData[226] << 16) | (emgData[25] << 8) | (emgData[24]);

    // sign extend the values from 24 bit to 32 bit
    emg.channel1 |= (emg.channel1 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel2 |= (emg.channel2 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel3 |= (emg.channel3 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel4 |= (emg.channel4 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel5 |= (emg.channel5 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel6 |= (emg.channel6 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel7 |= (emg.channel7 & 0x00800000) ? 0xFF000000 : 0; 
    emg.channel8 |= (emg.channel8 & 0x00800000) ? 0xFF000000 : 0; 

    return emg;  
}

//
//  Helper function to convert from channel id enum to associated register.
//
uint8_t Ads1299LowDriver::GetChannelRegisterFromChannelIdEnum(eChannelId chan)
{
    switch (chan)
    {
        case CH1: return REGISTER_CH1SET;
        case CH2: return REGISTER_CH2SET;
        case CH3: return REGISTER_CH3SET;
        case CH4: return REGISTER_CH4SET;
        case CH5: return REGISTER_CH5SET;
        case CH6: return REGISTER_CH6SET;
        case CH7: return REGISTER_CH7SET;
        case CH8: return REGISTER_CH8SET;
    }
}

//
//  Helper function to get he gain setting for the passed eChannelGain
//
uint8_t Ads1299LowDriver::GetGainFromFromChannelGainEnum(eChannelGain gain)
{
    switch (gain)
    {
        case X1: return CHNSET_GAIN_1;
        case X2: return CHNSET_GAIN_2;
        case X4: return CHNSET_GAIN_4;
        case X6: return CHNSET_GAIN_6;
        case X8: return CHNSET_GAIN_8;
        case X12: return CHNSET_GAIN_12;
        case X24: return CHNSET_GAIN_24;
    }
}

//
//  Helper function to convert the eSampleRate enum to a register value
//
uint8_t Ads1299LowDriver::GetSampleRateValueFromSampleRateEnum(eSampleRate rate)
{
    switch(rate)
    {
        case SPS_16000: return CONFIG_1_DATARATE_16KSPS;
        case SPS_8000: return CONFIG_1_DATARATE_8KSPS;
        case SPS_4000: return CONFIG_1_DATARATE_4KSPS;
        case SPS_2000: return CONFIG_1_DATARATE_2KSPS;
        case SPS_1000: return CONFIG_1_DATARATE_1KSPS;
        case SPS_500: return CONFIG_1_DATARATE_500SPS;
        case SPS_250: return CONFIG_1_DATARATE_250SPS;
    }
}

//
//  Helper function to get the channel state register value from the passed enum
//
uint8_t Ads1299LowDriver::GetChannelStateValueFromChannelStateEnum(eChannelState state)
{
    switch (state)
    {
        case OffAndShorted: return CHNSET_STATE_OFF_SHORTED;
        case OnNormal: return CHNSET_STATE_ON_NORMAL;
        case OnAndShorted: return CHNSET_STATE_ON_SHORTED;
        case OnAndMeasuringSupplies: return CHNSET_STATE_ON_MEASURING_SUPPLIES;
        case OnAndMeasuringTestSignal: return CHNSET_STATE_ON_MEASURING_TEST_SIGNAL;
    }
}

//
//  Helper funtion to read a register from the ADS1299
//
uint8_t Ads1299LowDriver::ReadRegister(uint8_t reg)
{
    // Read the Register.
    uint8_t readRegData[3] = { (uint8_t)(COMMAND_READ_REGISTER | reg), 0, BLANK_DATA };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readRegData,
                                    3);

    return readRegData[2];
}

//
//  Helper function to overwrite an ADS1299 register
//
void Ads1299LowDriver::WriteRegister(uint8_t reg, uint8_t newValue)
{
    // Write the Register.
    uint8_t writeRegData[3] = { (uint8_t)(COMMAND_WRITE_REGISTER | reg), 0, newValue };
    
    _SpiDriver.TransmitDataOverSPI(_PinControl,
                                    &PinControl::SetADS1299ChipSelectState,
                                    writeRegData,
                                    3);
}

//
//  Helper function to modify part of an ADS1299 Register.
//
void Ads1299LowDriver::ModifyRegister(uint8_t reg, uint8_t mask, uint8_t newValue)
{
    const uint8_t CURRENT_REG_VALUE = ReadRegister(reg);

    // check that the new value isn't already set
    if (newValue == (CURRENT_REG_VALUE & mask))
    {
        return;
    }

    const uint8_t NEW_REG_VALUE = (CURRENT_REG_VALUE & ~mask) | newValue;

    WriteRegister(reg, NEW_REG_VALUE);
}
