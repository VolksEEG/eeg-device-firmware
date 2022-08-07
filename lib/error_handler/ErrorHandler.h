
#ifndef _ERROR_HANDLER
#define _ERROR_HANDLER

#include <stdint.h>

class ErrorHandler {

    public:

        typedef enum _ERRORS
        {
            Error_EventOverrun = 0,
            Error_EventHandlerUnableToBeAdded = 1,
            Error_EventHandlingFailure = 2,
            Error_AdsSampleUnableToBeBuffered = 3
        }eError;

        ErrorHandler();

        void RaiseError(eError error);
        bool IsErrorPresent(void); 

    protected:

    private:

        static const uint8_t NO_ERRORS = 0;

        uint8_t _ErrorsRaisedBits;
};

#endif
