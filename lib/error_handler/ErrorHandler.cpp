
#include "ErrorHandler.h"

//
// Constructor
//
ErrorHandler::ErrorHandler() :
    _ErrorsRaisedBits(NO_ERRORS)
{

}

void ErrorHandler::RaiseError(eError error)
{
    _ErrorsRaisedBits |= (uint8_t)error; 
}   

bool ErrorHandler::IsErrorPresent(void)
{
    return (NO_ERRORS != _ErrorsRaisedBits);
}
