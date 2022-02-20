
#ifndef _EEG_DATA_CONSUMER
#define _EEG_DATA_CONSUMER

#include "EegData.h"

class EegDataConsumer {

    public:

        virtual void PushLatestSample(EegData::sEegSamples samples) {

        }

    protected:

    private:

};

#endif