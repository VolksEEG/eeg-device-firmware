
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
Ads1299LowDriver::Ads1299LowDriver(SpiDriver * spi, PinControl * pins) :
    _SpiDriverInstance(spi),
    _PinControlInstance(pins)
{

}

//
//  Sends the command to Reset the ADS1299 device
//
void Ads1299LowDriver::ResetDevice(void)
{
    uint8_t resetData[1] = { (uint8_t)COMMAND_RESET };

    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    resetData,
                                    1);                      
}

//
//  Sets up the Device to start capturing data continuously at the set rate
//
void Ads1299LowDriver::StartContinuousDataCapture(eSampleRate rate)
{
    ModifyRegister(REGISTER_CONFIG_1, CONFIG_1_DATARATE_MASK, (uint8_t)rate);

    uint8_t startData[1] = { (uint8_t)COMMAND_START };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    startData,
                                    1);     

    uint8_t readContinuousData[1] = { (uint8_t)COMMAND_READ_DATA_CONTINUOUSLY };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readContinuousData,
                                    1);     
}

//
//  Instructs the device to stop capturing data continuously
//
void Ads1299LowDriver::StopContinuousDataCapture(void)
{

    uint8_t stopContinuousData[1] = { (uint8_t)COMMAND_STOP_READING_DATA_CONTINUOUSLY };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    stopContinuousData,
                                    1);   

    uint8_t stopData[1] = { (uint8_t)COMMAND_STOP };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
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
    volatile const uint8_t ID = ReadRegister(REGISTER_ID);

    // the 3rd byte of the received data will be the ID register
    switch ((uint8_t)(ID & ID_NUMBER_OF_CHANNELS_MASK))
    {
        case (uint8_t)ID_NUMBER_OF_CHANNELS_4:
            return 4;
            break;
        case (uint8_t)ID_NUMBER_OF_CHANNELS_6:
            return 6;
            break;
        case (uint8_t)ID_NUMBER_OF_CHANNELS_8:
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
    const eRegisters REG = GetChannelRegisterFromChannelIdEnum(chan);
    
    ModifyRegister(REG, CHNSET_STATE_MASK, (uint8_t)state);
}

//
//  Sets the gain for the selected channel
//
void Ads1299LowDriver::SetChannelGain(eChannelId chan, eChannelGain gain)
{
    const eRegisters REG = GetChannelRegisterFromChannelIdEnum(chan);

    ModifyRegister(REG, CHNSET_GAIN_MASK, (uint8_t)gain);
}

//
//  Function to set the conversion reference source
//
void Ads1299LowDriver::SetReferenceSource(eReferenceSource src)
{
    if (Internal == src)
    {
        ModifyRegister(REGISTER_CONFIG_3, CONFIG_3_REF_BUFFER_MASK, CONFIG_3_REF_BUFFER_ENABLE);

        return;
    }
    
    ModifyRegister(REGISTER_CONFIG_3, CONFIG_3_REF_BUFFER_MASK, CONFIG_3_REF_BUFFER_DISABLE);
}

//
//  Function set up a test signal
//
void Ads1299LowDriver::SetTestSignal(void)
{
    ModifyRegister(REGISTER_CONFIG_2, 
        (uint8_t)(CONFIG_2_INTERNAL_CAL_MASK | CONFIG_2_CAL_AMPLITUDE_MASK | CONFIG_2_CAL_FREQUENCY_MASK),
        (uint8_t)((uint8_t)CONFIG_2_INTERNAL_CAL_ENABLE | (uint8_t)CONFIG_2_CAL_AMPLITUDE_HIGH | (uint8_t)CONFIG_2_CAL_FREQUENCY_DC));
}

//
//  Function to request the latest channel data
//
Ads1299LowDriver::sEMGData Ads1299LowDriver::GetEMGData(void)
{
    uint8_t emgData[27] = {0};  // 3 bytes for each of the 8 emg channels and 1 status channel

    // Get the EMG Data
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    emgData,
                                    27);

    // Got the data, now parse it
    sEMGData emg;

    emg.status = (emgData[0] << 16) | (emgData[1] << 8) | (emgData[2]);
    emg.channel1 = (emgData[3] << 16) | (emgData[4] << 8) | (emgData[5]);
    emg.channel2 = (emgData[6] << 16) | (emgData[7] << 8) | (emgData[8]);
    emg.channel3 = (emgData[9] << 16) | (emgData[10] << 8) | (emgData[11]);
    emg.channel4 = (emgData[12] << 16) | (emgData[13] << 8) | (emgData[14]);
    emg.channel5 = (emgData[15] << 16) | (emgData[16] << 8) | (emgData[17]);
    emg.channel6 = (emgData[18] << 16) | (emgData[19] << 8) | (emgData[20]);
    emg.channel7 = (emgData[21] << 16) | (emgData[22] << 8) | (emgData[23]);
    emg.channel8 = (emgData[24] << 16) | (emgData[25] << 8) | (emgData[26]);

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
Ads1299LowDriver::eRegisters Ads1299LowDriver::GetChannelRegisterFromChannelIdEnum(eChannelId chan)
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
//  Helper funtion to read a register from the ADS1299
//
uint8_t Ads1299LowDriver::ReadRegister(eRegisters reg)
{
    // Read the Register.
    uint8_t readRegData[3] = { (uint8_t)((uint8_t)COMMAND_READ_REGISTER | (uint8_t)reg), 0, BLANK_DATA };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readRegData,
                                    3);

    return readRegData[2];
}

//
//  Helper function to overwrite an ADS1299 register
//
void Ads1299LowDriver::WriteRegister(eRegisters reg, uint8_t newValue)
{
    // Write the Register.
    uint8_t writeRegData[3] = { (uint8_t)((uint8_t)COMMAND_WRITE_REGISTER | (uint8_t)reg), 0, newValue };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    writeRegData,
                                    3);
}

//
//  Helper function to modify part of an ADS1299 Register.
//
void Ads1299LowDriver::ModifyRegister(eRegisters reg, uint8_t mask, uint8_t newValue)
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
