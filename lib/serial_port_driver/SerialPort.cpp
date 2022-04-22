
#include "SerialPort.h"
#include <string.h>

#ifndef UNIT_TEST
#include <Adafruit_TinyUSB.h>
#else
#include <../../src/ArduinoMock.h>
#endif

//
//  Constructor
//
SerialPort::SerialPort()
{

}

//
//  Constructor
//
SerialPort::SerialPort(EventHandler * eh) :
    _EventHandlerInstance(eh),
    _RxInputIndex(0),
    _RxOutputIndex(0),
    _RxCount(0),
    _RxDataProcessingComplete(true),
    _ReSignalDataRxEvent(false)
{
    // baud rate does not seem to matter for USB serial link
    Serial.begin(115200);

    memset(_RxBuffer, 0, _RX_BUFFER_SIZE);
}

//
//  Function to perform serial port related background tasks
//
void SerialPort::BackgroundTaskHandler(void)
{
    if (true == _ReSignalDataRxEvent)
    {
        _EventHandlerInstance->SignalEvent(NEvent::Event_DataRxFromPC);

        _ReSignalDataRxEvent = false;
    }

    if (Serial.available() == 0)
    {
        return;
    }

    if (_RxCount >= _RX_BUFFER_SIZE)
    {
        return;
    }

    if (true == _RxDataProcessingComplete)
    {
        _RxDataProcessingComplete = false;

        _EventHandlerInstance->SignalEvent(NEvent::Event_DataRxFromPC);
    }
    
    _RxBuffer[_RxInputIndex] = (uint8_t)Serial.read();

    // increment input index and check for wrap around.
    if(++_RxInputIndex == _RX_BUFFER_SIZE)
    {
        _RxInputIndex = 0;
    }

    _RxCount++;
}

//
//  Overriden function for processing events
//
void SerialPort::ProcessEvent(NEvent::eEvent event)
{
    if (NEvent::Event_DataRxFromPC != event)
    {
        return;
    }

    if (true == _RxDataProcessingComplete)
    {
        return;
    }

    _ReSignalDataRxEvent = true;
}

//
// Overriden function to get received bytes from the PC
//
uint8_t SerialPort::GetReceivedBytes(uint8_t data[], uint8_t max_length)
{
    if (0 == _RxCount)
    {
        return 0;
    }

    // get the maximum number of bytes we can copy
    const uint8_t COUNT = (max_length > _RxCount) ? _RxCount : max_length;

    // copy bytes to the output. 
    for (int i = 0; i < COUNT; ++i)
    {
        data[i] = _RxBuffer[_RxOutputIndex];

        // increment output index and check for wrap around.
        if(++_RxOutputIndex == _RX_BUFFER_SIZE)
        {
            _RxOutputIndex = 0;
        }

        _RxCount--;
    }

    // has all data been requested
    if (0 == _RxCount)
    {
        _RxDataProcessingComplete = true;
    }

    return COUNT;
}

//
//  Overriden function to transmit a number of bytes to the PC
//
void SerialPort::TransmitData(uint8_t data[], uint8_t count)
{
    Serial.write(data, count);
}
