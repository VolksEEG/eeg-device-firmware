
#ifndef _CONSUMER_MOCK
#define _CONSUMER_MOCK

#include <EEGDataConsumer.h>

class ConsumerMock : public EegDataConsumer {

    public:

        void PushLatestSample(EegData::sEegSamples samples) {

        }
        
    protected:

    private:

};

#endif
