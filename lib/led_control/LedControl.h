
#ifndef _LED_CONTROL
#define _LED_CONTROL

#include <EventHandler.h>
#include <ErrorHandler.h>
#include <PinControl.h>
#include <stdint.h>

class LedControl : CanProcessEvents {

    public:

        LedControl();
        LedControl(ErrorHandler * eh, PinControl * pc, EventHandler * evh);

        void ProcessEvent(NEvent::eEvent event);

    protected:

    private:

        // TIMEOUT VALUES
        static const uint16_t NORMAL_ON_TIMEOUT;
        static const uint16_t NORMAL_OFF_TIMEOUT;
        static const uint16_t ERROR_ON_TIMEOUT;
        static const uint16_t ERROR_OFF_TIMEOUT;

        ErrorHandler * _ErrorHAndlerInstance;
        PinControl * _PinControlInstance;

        uint16_t _LedCounter;
        uint16_t _LedTimeout;

        void (LedControl::*_LedStateFunction)();

        void LedStateOn(void);
        void LedStateOff(void);
};

#endif
