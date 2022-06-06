
#include "LedControl.h"

//
//  Constant value definitions
//
const uint16_t LedControl::NORMAL_ON_TIMEOUT = 500;
const uint16_t LedControl::NORMAL_OFF_TIMEOUT = 1500;
const uint16_t LedControl::ERROR_ON_TIMEOUT = 1500;
const uint16_t LedControl::ERROR_OFF_TIMEOUT = 500;

//
// Constructor
//
LedControl::LedControl()
{

}
  
//
// Constructor
//      
LedControl::LedControl(ErrorHandler * eh, PinControl * pc) :
    _ErrorHAndlerInstance(eh),
    _PinControlInstance(pc),
    _LedCounter(0),
    _LedTimeout(NORMAL_OFF_TIMEOUT),
    _LedStateFunction(&LedControl::LedStateOff)
{
    // Set initial state to inactive i.e. Off
    _PinControlInstance->SetHeartbeatLedState(PinControl::eSetPinState::SetInactive);
}

//
// Overridden function from the CanProcessEvents Base Class
//
void LedControl::ProcessEvent(NEvent::eEvent event)
{
    // double check the event
    if (NEvent::Event_1mSTimeout != event)
    {
        return;
    }

    // event is a 1mS timeout so call state machine function
    (this->*_LedStateFunction)();
}

//
//  Controls the timeout and transition from the LED being on
//
void LedControl::LedStateOn(void)
{
    _LedCounter++;

    if (_LedCounter >= _LedTimeout)
    {
        // set the next timeout
        _LedTimeout = (true == _ErrorHAndlerInstance->IsErrorPresent()) 
                            ? ERROR_OFF_TIMEOUT
                            : NORMAL_OFF_TIMEOUT;

        // reset the counter, 
        // turn LED off and, 
        // Set the Off state in both scenarios
        _LedCounter = 0;
        //void (PinControl::*ledStateFptr)(PinControl::eSetPinState) = &PinControl::SetHeartbeatLedState;
        //(_PinControlInstance->*ledStateFptr)(PinControl::eSetPinState::SetInactive);
        _PinControlInstance->SetHeartbeatLedState(PinControl::eSetPinState::SetInactive);
        _LedStateFunction = &LedControl::LedStateOff;
    }
}

//
//  Controls the timeout and transition from the LED being off
//
void LedControl::LedStateOff(void)
{
    _LedCounter++;

    if (_LedCounter >= _LedTimeout)
    {
        // set the next timeout
        _LedTimeout = (true == _ErrorHAndlerInstance->IsErrorPresent()) 
                            ? ERROR_ON_TIMEOUT
                            : NORMAL_ON_TIMEOUT;

        // reset the counter, 
        // turn LED on and, 
        // Set the On state in both scenarios
        _LedCounter = 0;
        //void (PinControl::*ledStateFptr)(PinControl::eSetPinState) = &PinControl::SetHeartbeatLedState;
        //(_PinControlInstance->*ledStateFptr)(PinControl::eSetPinState::SetActive);
        _PinControlInstance->SetHeartbeatLedState(PinControl::eSetPinState::SetActive);
        _LedStateFunction = &LedControl::LedStateOn;
    }
}
