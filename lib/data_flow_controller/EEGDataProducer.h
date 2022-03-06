
#ifndef _EEG_DATA_PRODUCER
#define _EEG_DATA_PRODUCER

#include "EegData.h"

class EegDataProducer {

    public:

        virtual EegData::sEegSamples GetLatestSample() {
            EegData::sEegSamples dummy;
            
            dummy.channel_1 = 0;
            dummy.channel_2 = 0;
            dummy.channel_3 = 0;
            dummy.channel_4 = 0;
            dummy.channel_5 = 0;
            dummy.channel_6 = 0;
            dummy.channel_7 = 0;
            dummy.channel_8 = 0;

            return dummy;
        }

    protected:

    private:

};

#endif