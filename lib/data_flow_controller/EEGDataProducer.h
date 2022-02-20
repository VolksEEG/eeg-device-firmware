
#ifndef _EEG_DATA_PRODUCER
#define _EEG_DATA_PRODUCER

#include "EegData.h"

class EegDataProducer {

    public:

        virtual EegData::sEegSamples GetLatestSample() {
            EegData::sEegSamples dummy;

            return dummy;
        }

    protected:

    private:

};

#endif