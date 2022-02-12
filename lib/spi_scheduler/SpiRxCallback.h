
#ifndef _SPI_RX_CALLBACK
#define _SPI_RX_CALLBACK

#include <stdint.h>

class SpiRxCallback {

    public:
        
        virtual void SpiRxDataProcess(uint8_t rxData[], uint8_t rxCount) = 0;

    protected:

    private:

};

#endif