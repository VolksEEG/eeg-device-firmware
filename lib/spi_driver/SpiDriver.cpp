
#include "SpiDriver.h"

#ifndef PIO_UNIT_TESTING
#include <SPI.h>
#else
#include <../../src/ArduinoMock.h>
#endif

//
// Constructor
//
SpiDriver::SpiDriver()
{
    SPI.begin();
}

//
//  Function to transmit data over SPI.
//
//  The passed dataTxRx array will be populated with 
//  data recieved during the transmission.
//
void SpiDriver::TransmitDataOverSPI(PinControl * pinControl, 
                                void (PinControl::*chipSelectFptr)(PinControl::eSetPinState), 
                                uint8_t dataTxRx[],
                                uint8_t dataCount)
{

    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE1));

    (pinControl->*chipSelectFptr)(PinControl::eSetPinState::SetActive);

    // transfer each byte and set the dataTxRx array to the received byte.
    for (int i = 0; i < dataCount; ++i)
    {
        dataTxRx[i] = (uint8_t)SPI.transfer(dataTxRx[i]);
    }    

    (pinControl->*chipSelectFptr)(PinControl::eSetPinState::SetInactive);

    SPI.endTransaction();
}
