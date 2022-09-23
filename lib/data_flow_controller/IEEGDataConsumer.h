
#ifndef _EEG_DATA_CONSUMER
#define _EEG_DATA_CONSUMER

#include "EegData.h"

class IEegDataConsumer {

    public:

        virtual void PushLatestSample(EegData::sEegSamples samples) {

        }

    protected:

    private:

};

#endif