
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
                                uint8_t dataCount,
                                uint32_t speed)
{

    SPI.beginTransaction(SPISettings(speed, MSBFIRST, SPI_MODE1));

    // Assert chip select
    (pinControl->*chipSelectFptr)(PinControl::eSetPinState::SetActive);

    // transfer the data
    SPI.transfer(dataTxRx, dataCount); 

    // deassert the chip select
    (pinControl->*chipSelectFptr)(PinControl::eSetPinState::SetInactive);

    SPI.endTransaction();
}
