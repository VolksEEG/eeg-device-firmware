/**
 * @file Ads1299LowDriver.cpp
 * @author Graham Long (longevoty_software@mail.com)
 * @brief Low level driver for the ADS1299 EEG Amplifier.
 * @date 2022-06-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Ads1299LowDriver.h"

/**
 * @brief Constructor
 * 
 */
Ads1299LowDriver::Ads1299LowDriver()
{

}

/**
 * @brief Constructor
 * 
 * @param spi Pointer to the SPI Driver to be used to communicate with the ADS1299 device.
 * @param pins Pointer to the PinControl module which is used to set the chip select line for the ADS1299 Device.
 */
Ads1299LowDriver::Ads1299LowDriver(SpiDriver * spi, PinControl * pins) :
    _SpiDriverInstance(spi),
    _PinControlInstance(pins),
    _VotlageReferenceCache(INTERNAL_VREF_VOLTAGE)       //! @todo Confirm this is the default.
{
    SetChannelGainsToDefault();
}

/**
 * @brief Sends the command to Reset the ADS1299 device over the SPI Interface
 * 
 */
void Ads1299LowDriver::ResetDevice(void)
{
    uint8_t resetData[1] = { (uint8_t)COMMAND_RESET };

    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    resetData,
                                    1,
                                    4000000); 

    // device has been reset, so set stored gains to the default.
    SetChannelGainsToDefault();                  
}

/**
 * @brief Sets up the Device to start capturing data continuously at the set rate
 * 
 * @param rate The data rate to capture samples at.
 */
void Ads1299LowDriver::StartContinuousDataCapture(eSampleRate rate)
{
    ModifyRegister(REGISTER_CONFIG_1, CONFIG_1_DATARATE_MASK, (uint8_t)rate);

    uint8_t startData[1] = { (uint8_t)COMMAND_START };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    startData,
                                    1,
                                    4000000);     

    uint8_t readContinuousData[1] = { (uint8_t)COMMAND_READ_DATA_CONTINUOUSLY };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readContinuousData,
                                    1,
                                    4000000);     
}

/**
 * @brief Instructs the device to stop capturing data continuously
 * 
 */
void Ads1299LowDriver::StopContinuousDataCapture(void)
{

    uint8_t stopContinuousData[1] = { (uint8_t)COMMAND_STOP_READING_DATA_CONTINUOUSLY };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    stopContinuousData,
                                    1,
                                    4000000);   

    uint8_t stopData[1] = { (uint8_t)COMMAND_STOP };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    stopData,
                                    1,
                                    4000000);     
}

/**
 * @brief Gets the number of channels supported by the ADS1299.
 * 
 * @return uint8_t The number of channels supported by the device.
 */
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

/**
 * @brief Configures an ADS1299 EEG channels data capture state
 * 
 * @param chan The channel to configure.
 * @param state The state to set that channel to.
 */
void Ads1299LowDriver::SetChannelState(eChannelId chan, eChannelState state)
{
    const eRegisters REG = GetChannelRegisterFromChannelIdEnum(chan);
    
    ModifyRegister(REG, CHNSET_STATE_MASK, (uint8_t)state);
}

/**
 * @brief Sets the gain for the selected channel
 * 
 * @param chan The channel to set the gain of.
 * @param gain The gain to set on that channel.
 */
void Ads1299LowDriver::SetChannelGain(eChannelId chan, eChannelGain gain)
{
    const eRegisters REG = GetChannelRegisterFromChannelIdEnum(chan);

    ModifyRegister(REG, CHNSET_GAIN_MASK, (uint8_t)gain);

    // Cache the channels gain.
    _ChannelGainsCache[(int)chan] = GetMultiplierFromChannelGain(gain);
}

/**
 * @brief Function to set the voltage reference source
 * 
 * @param src The reference source to use, External or Internal.
 */
void Ads1299LowDriver::SetReferenceSource(eReferenceSource src)
{
    if (Internal == src)
    {
        ModifyRegister(REGISTER_CONFIG_3, CONFIG_3_REF_BUFFER_MASK, CONFIG_3_REF_BUFFER_ENABLE);

        _VotlageReferenceCache = INTERNAL_VREF_VOLTAGE;
        
        return;
    }
    
    ModifyRegister(REGISTER_CONFIG_3, CONFIG_3_REF_BUFFER_MASK, CONFIG_3_REF_BUFFER_DISABLE);

    _VotlageReferenceCache = EXTERNAL_VREF_VOLTAGE;
}

/**
 * @brief Function to set up a test signal
 * 
 */
void Ads1299LowDriver::SetTestSignal(void)
{
    ModifyRegister(REGISTER_CONFIG_2, 
        (uint8_t)(CONFIG_2_INTERNAL_CAL_MASK | CONFIG_2_CAL_AMPLITUDE_MASK | CONFIG_2_CAL_FREQUENCY_MASK),
        (uint8_t)((uint8_t)CONFIG_2_INTERNAL_CAL_ENABLE | (uint8_t)CONFIG_2_CAL_AMPLITUDE_LOW | (uint8_t)CONFIG_2_CAL_FREQUENCY_FAST));
}

/**
 * @brief Function to set whether the Negative input is connected to SRB1
 * 
 */
void Ads1299LowDriver::SetNegativeInputsConnectionToSRB1(bool connect)
{
    if (connect)
    {
        ModifyRegister(REGISTER_MISC1, MISC_1_SRB1_MASK, (uint8_t)MISC_1_SRB1_DISABLE);

        return;
    }

    ModifyRegister(REGISTER_MISC1, MISC_1_SRB1_MASK, (uint8_t)MISC_1_SRB1_ENABLE);
}

//
//  Function to request the latest channel data
//
/**
 * @brief Get the latest EEG Channel data and sign extend it to 32 bits.
 * 
 * @return A structure holding the channel data. 
 */
Ads1299LowDriver::sAds1299AllChannelsData Ads1299LowDriver::GetAds1299Data(void)
{
    //! @todo Make 27 variable based on the number of channels supported by the device.
    uint8_t eegRawData[27] = {0};  // 3 bytes for each of the 8 eeg channels and 1 status channel

    // Get the EMG Data
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    eegRawData,
                                    27,
                                    12000000);

    Ads1299LowDriver::sAds1299AllChannelsData eegData;

    eegData.vrefVoltage = _VotlageReferenceCache;
    
    // Loop over the raw data, skipping bytes 0-2 as these are status bytes.
    for (int i = 3, ch = 0; 
            i < 27; 
            i = i + 3)
    {
        // Get the 24 bit count and extend the last bit to make it a 32 bit signed value.
        const int32_t COUNT = (eegRawData[i] << 16) | (eegRawData[i + 1] << 8) | (eegRawData[i + 2]);
        const int32_t EXTENDED_COUNT = COUNT | ((COUNT & 0x00800000) ? 0xFF000000 : 0);   // if bit 23 is set, then or it with bits 24 and above set to sign extend.

        eegData.channels[ch].count = EXTENDED_COUNT;
        eegData.channels[ch].gain = _ChannelGainsCache[ch];

        // advance to the next channel
        ch++;
    }

    return eegData;  
}

/**
 * @brief Helper function to convert from channel id enum to associated register.
 * 
 * @param chan The channel to request the register for.
 * @return Ads1299LowDriver::eRegisters The Register associated with that channel
 */
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

/**
 * @brief Helper funtion to read a register from the ADS1299
 * 
 * @param reg The register to read
 * @return uint8_t The value of the read register.
 */
uint8_t Ads1299LowDriver::ReadRegister(eRegisters reg)
{
    // Read the Register.
    uint8_t readRegData[3] = { (uint8_t)((uint8_t)COMMAND_READ_REGISTER | (uint8_t)reg), 0, BLANK_DATA };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    readRegData,
                                    3,
                                    4000000);

    return readRegData[2];
}

/**
 * @brief Helper function to overwrite an ADS1299 register
 * 
 * @param reg The register to overwrite.
 * @param newValue The new value to write to that register.
 */
void Ads1299LowDriver::WriteRegister(eRegisters reg, uint8_t newValue)
{
    // Write the Register.
    uint8_t writeRegData[3] = { (uint8_t)((uint8_t)COMMAND_WRITE_REGISTER | (uint8_t)reg), 0, newValue };
    
    _SpiDriverInstance->TransmitDataOverSPI(_PinControlInstance,
                                    &PinControl::SetADS1299ChipSelectState,
                                    writeRegData,
                                    3,
                                    4000000);
}

/**
 * @brief Helper function to modify part of an ADS1299 Register.
 * 
 * @param reg The register to modify.
 * @param mask The mask of values to change.
 * @param newValue The new values to set/clear.
 */
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

/**
 * @brief Helper function to get the multiplier value from a channel gain enum
 * 
 * @param gain The gain enumeration.
 * @return int32_t The Multiplier for that gain.
 */
int32_t Ads1299LowDriver::GetMultiplierFromChannelGain(eChannelGain gain)
{
    switch (gain)
    {
        case X1: return 1;
        case X2: return 2;
        case X4: return 4;
        case X6: return 6;
        case X8: return 8;
        case X12: return 12;
        case X24: return 24;
        default: return DEFAULT_CHANNEL_GAIN;
    }
}

/**
 * @brief Helper function to set the cached channel gain values to default.
 * 
 */
void Ads1299LowDriver::SetChannelGainsToDefault(void)
{
    for (int i = 0; i < MAX_CHANNELS; ++i)
    {
        _ChannelGainsCache[i] = DEFAULT_CHANNEL_GAIN;
    }
}
