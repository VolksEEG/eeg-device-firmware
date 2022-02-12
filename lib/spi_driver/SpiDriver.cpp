
#include "SpiDriver.h"

//
// Constructor
//
SpiDriver::SpiDriver()
{
    
}

//
//  Function to transmit data over SPI.
//
//  The passed dataTxRx array will be populated with 
//  data recieved during the transmission.
//
void SpiDriver::TransmitDataOverSPI(PinControl& pinControl, 
                                void (PinControl::*chipSelectFptr)(PinControl::eSetPinState), 
                                uint8_t dataTxRx[],
                                uint8_t dataCount)
{
    (pinControl.*chipSelectFptr)(PinControl::eSetPinState::SetActive);

    // Modify the data to show its doing something
    for (int i = 0; i < dataCount; ++i)
    {
        dataTxRx[i] = ~dataTxRx[i];
    }    

    (pinControl.*chipSelectFptr)(PinControl::eSetPinState::SetInactive);
}
