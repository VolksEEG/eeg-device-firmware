
#include "SerialPort.h"

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
int8_t SerialPort::GetReceivedByte()
{
    return 0;
}

//
//  Overriden function to transmit a number of bytes to the PC
//
void SerialPort::TransmitData(int8_t * dataPtr, int8_t count)
{

}
