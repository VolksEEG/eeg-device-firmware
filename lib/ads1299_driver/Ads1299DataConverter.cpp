/**
 * @file Ads1299DataConverter.cpp
 * @author Graham Long (longevoty_software@mail.com)
 * @brief Data convertor helper class
 * @date 2022-06-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Ads1299DataConverter.h"

/**
 * @brief Constructor
 * 
 */
Ads1299DataConverter::Ads1299DataConverter()
{
    
}

/**
 * @brief Converts ADS1299 counts to microvolts.
 * 
 * See ADS1299 Datasheet Section 9.5.1 Data Format (Equation 8)
 * 
 * @param count The ADS1299 Count.
 * @param gain The Gain associated with that count.
 * @param ref The reference voltage
 * @return int16_t The number of micro volts for the passed arguments.
 */
int16_t Ads1299DataConverter::ConvertAds1299CountToMicroVolts(int32_t count, int32_t gain, float ref)
{
    const float VOLTS_PER_COUNT = (ref / gain) / ADS1299_FULL_SCALE_VALUE;

    return (int16_t)(count * VOLTS_PER_COUNT * MICROVOLTS_IN_A_VOLT);
}
