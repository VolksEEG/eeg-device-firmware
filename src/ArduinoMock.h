
#ifndef ARDUINO_MOCK
#define ARDUINO_MOCK

#include <stdint.h>

#define LOW             0
#define HIGH            1

#define OUTPUT          0
#define INPUT           1

#define LED_BUILTIN    13

#define pinMode(a, b)
#define digitalWrite(a, b)
#define digitalRead(a)          (0)

#define MSBFIRST        0
#define SPI_MODE1       1

#define noInterrupts()
#define interrupts()

class SPISettings {
    public: 
        SPISettings(long, int, int) {

        }
};

class SPI_mock {
    public:

        void begin() {

        }

        void beginTransaction(SPISettings settings) {

        }

        void endTransaction(void) {

        }

        uint8_t transfer(uint8_t data)
        {
            return 0;
        }
};

static SPI_mock SPI;

#endif