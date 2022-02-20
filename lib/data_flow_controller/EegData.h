
#ifndef _EEG_DATA
#define _EEG_DATA

#include <stdint.h>

class EegData
{
    public:
        typedef struct _EEG_SAMPLES
        {
            int16_t channel_1;
            int16_t channel_2;
            int16_t channel_3;
            int16_t channel_4;
            int16_t channel_5;
            int16_t channel_6;
            int16_t channel_7;
            int16_t channel_8;
        }sEegSamples;
};

#endif