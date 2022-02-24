
#ifndef _SPI_SCHEDULER
#define _SPI_SCHEDULER

#include <PinControl.h>

class SpiDriver {

    public:

        SpiDriver();

        void TransmitDataOverSPI(PinControl * pinControl, 
                                void (PinControl::*chipSelectFptr)(PinControl::eSetPinState), 
                                uint8_t dataTxRx[],
                                uint8_t dataCount);

    protected:

    private:

};

#endif
