
#ifndef _SPI_SCHEDULER
#define _SPI_SCHEDULER

#include "SpiRxCallback.h"

#include <PinControl.h>

class SpiScheduler {

    public:

        typedef enum _SPI_PRIORITY
        {
            SPI_NORMAL_PRIORITY,
            SPI_HIGH_PRIORITY
        }eSpiPriority;

        SpiScheduler();

        void SchedulAnSpiJob(eSpiPriority priority, 
                                PinControl& pinControl, 
                                void (PinControl::*chipSelectFptr)(PinControl::eSetPinState), 
                                uint8_t txData[], 
                                uint8_t txCount,
                                SpiRxCallback& spiRxCallback
                                );


    protected:

    private:

        typedef struct _SPI_JOB
        {
            
        }eSpiJob;
};

#endif
