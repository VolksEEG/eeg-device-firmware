
#ifndef _ADS1299_DATA_CONVERTER
#define _ADS1299_DATA_CONVERTER

#include <EegData.h>

class Ads1299DataConverter  {

    public:

        Ads1299DataConverter();

        static int16_t ConvertAds1299CountToMicroVolts(int32_t count, int32_t gain, float ref);
        
    protected:

    private:

        static constexpr float ADS1299_FULL_SCALE_VALUE = 8388607.0f;
        static constexpr float MICROVOLTS_IN_A_VOLT = 1000000.0f;

};

#endif
