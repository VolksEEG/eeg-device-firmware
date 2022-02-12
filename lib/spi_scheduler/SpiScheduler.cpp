
#include "SpiScheduler.h"

//
// Constructor
//
SpiScheduler::SpiScheduler()
{
    
}

void SpiScheduler::SchedulAnSpiJob(eSpiPriority priority, 
                                PinControl& pinControl, 
                                void (PinControl::*chipSelectFptr)(PinControl::eSetPinState), 
                                uint8_t txData[], 
                                uint8_t txCount,
                                SpiRxCallback& spiRxCallback
                                )
{

}