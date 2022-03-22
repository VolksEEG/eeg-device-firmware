
#include "SerialPort.h"

#include <Adafruit_TinyUSB.h>

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
    _EventHandlerInstance(eh)
{
    // baud rate does not seem to matter for USB serial link
    Serial.begin(115200);
}

//
//  Overriden function for processing events
//
void SerialPort::ProcessEvent(NEvent::eEvent event)
{

}

//
// Overriden function to get received bytes from the PC
//
uint8_t SerialPort::GetReceivedBytes(uint8_t data[], uint8_t max_length)
{
    return 0;
}

//
//  Overriden function to transmit a number of bytes to the PC
//
void SerialPort::TransmitData(uint8_t data[], uint8_t count)
{
    Serial.write(data, count);
}
