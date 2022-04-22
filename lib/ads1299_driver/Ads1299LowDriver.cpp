
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
    _VoltageConversionSpecs.SetToDefaults();
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

    _VoltageConversionSpecs.SetToDefaults();                  
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

    const float GAIN = GetMultiplierFromChannelGain(gain);

    switch (chan)
    {
        case CH1:
            _VoltageConversionSpecs.gainCh1 = GAIN;
            break;
        case CH2:
            _VoltageConversionSpecs.gainCh2 = GAIN;
            break;
        case CH3:
            _VoltageConversionSpecs.gainCh3 = GAIN;
            break;
        case CH4:
            _VoltageConversionSpecs.gainCh4 = GAIN;
            break;
        case CH5:
            _VoltageConversionSpecs.gainCh5 = GAIN;
            break;
        case CH6:
            _VoltageConversionSpecs.gainCh6 = GAIN;
            break;
        case CH7:
            _VoltageConversionSpecs.gainCh7 = GAIN;
            break;
        case CH8:
            _VoltageConversionSpecs.gainCh8 = GAIN;
            break;
    }

    // one of the multipliers will have changed, so recalculate.
    _VoltageConversionSpecs.RecalculateMultipliers();
}

//
//  Function to set the conversion reference source
//
void Ads1299LowDriver::SetReferenceSource(eReferenceSource src)
{
    if (Internal == src)
    {
        ModifyRegister(REGISTER_CONFIG_3, CONFIG_3_REF_BUFFER_MASK, CONFIG_3_REF_BUFFER_ENABLE);

        _VoltageConversionSpecs.vref = INTERNAL_VREF_VOLTAGE;
        
        // all of the multipliers will have changed, so recalculate.
        _VoltageConversionSpecs.RecalculateMultipliers();

        return;
    }
    
    ModifyRegister(REGISTER_CONFIG_3, CONFIG_3_REF_BUFFER_MASK, CONFIG_3_REF_BUFFER_DISABLE);

    _VoltageConversionSpecs.vref = EXTERNAL_VREF_VOLTAGE;
    
    // all of the multipliers will have changed, so recalculate.
    _VoltageConversionSpecs.RecalculateMultipliers();
}

//
//  Function set up a test signal
//
void Ads1299LowDriver::SetTestSignal(void)
{
    ModifyRegister(REGISTER_CONFIG_2, 
        (uint8_t)(CONFIG_2_INTERNAL_CAL_MASK | CONFIG_2_CAL_AMPLITUDE_MASK | CONFIG_2_CAL_FREQUENCY_MASK),
        (uint8_t)((uint8_t)CONFIG_2_INTERNAL_CAL_ENABLE | (uint8_t)CONFIG_2_CAL_AMPLITUDE_LOW | (uint8_t)CONFIG_2_CAL_FREQUENCY_FAST));
}

//
//  Function to request the latest channel data
//
EegData::sEegSamples Ads1299LowDriver::GetEEGData(void)
{
    uint8_t eegRawData[27] = {0};  // 3 bytes for each of the 8 eeg channels and 1 status channel

    // Get the EMG Data
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    eegRawData,
                                    27);

    // Got the data, now parse it
    sEMGData eegTemp;

    eegTemp.status = (eegRawData[0] << 16) | (eegRawData[1] << 8) | (eegRawData[2]);
    eegTemp.channel1 = (eegRawData[3] << 16) | (eegRawData[4] << 8) | (eegRawData[5]);
    eegTemp.channel2 = (eegRawData[6] << 16) | (eegRawData[7] << 8) | (eegRawData[8]);
    eegTemp.channel3 = (eegRawData[9] << 16) | (eegRawData[10] << 8) | (eegRawData[11]);
    eegTemp.channel4 = (eegRawData[12] << 16) | (eegRawData[13] << 8) | (eegRawData[14]);
    eegTemp.channel5 = (eegRawData[15] << 16) | (eegRawData[16] << 8) | (eegRawData[17]);
    eegTemp.channel6 = (eegRawData[18] << 16) | (eegRawData[19] << 8) | (eegRawData[20]);
    eegTemp.channel7 = (eegRawData[21] << 16) | (eegRawData[22] << 8) | (eegRawData[23]);
    eegTemp.channel8 = (eegRawData[24] << 16) | (eegRawData[25] << 8) | (eegRawData[26]);

    // sign extend the values from 24 bit to 32 bit
    eegTemp.channel1 |= (eegTemp.channel1 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel2 |= (eegTemp.channel2 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel3 |= (eegTemp.channel3 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel4 |= (eegTemp.channel4 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel5 |= (eegTemp.channel5 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel6 |= (eegTemp.channel6 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel7 |= (eegTemp.channel7 & 0x00800000) ? 0xFF000000 : 0; 
    eegTemp.channel8 |= (eegTemp.channel8 & 0x00800000) ? 0xFF000000 : 0; 

    // Now convert to micro Volts.
    EegData::sEegSamples eegData;
    eegData.channel_1 = (int16_t)(eegTemp.channel1 * _VoltageConversionSpecs.multiplierCh1);
    eegData.channel_2 = (int16_t)(eegTemp.channel2 * _VoltageConversionSpecs.multiplierCh2);
    eegData.channel_3 = (int16_t)(eegTemp.channel3 * _VoltageConversionSpecs.multiplierCh3);
    eegData.channel_4 = (int16_t)(eegTemp.channel4 * _VoltageConversionSpecs.multiplierCh4);
    eegData.channel_5 = (int16_t)(eegTemp.channel5 * _VoltageConversionSpecs.multiplierCh5);
    eegData.channel_6 = (int16_t)(eegTemp.channel6 * _VoltageConversionSpecs.multiplierCh6);
    eegData.channel_7 = (int16_t)(eegTemp.channel7 * _VoltageConversionSpecs.multiplierCh7);
    eegData.channel_8 = (int16_t)(eegTemp.channel8 * _VoltageConversionSpecs.multiplierCh8);

    return eegData;  
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
        default: return REGISTER_CH8SET;
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

//
//  Helper function to get the multiplier value from a channel gain enum
//
float Ads1299LowDriver::GetMultiplierFromChannelGain(eChannelGain gain)
{
    switch (gain)
    {
        case X1: return 1.0f;
        case X2: return 2.0f;
        case X4: return 4.0f;
        case X6: return 6.0f;
        case X8: return 8.0f;
        case X12: return 12.0f;
        default: return 24.0f;
    }
}
