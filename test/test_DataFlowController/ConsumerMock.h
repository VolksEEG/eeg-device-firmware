
#ifndef _CONSUMER_MOCK
#define _CONSUMER_MOCK

#include <EEGDataConsumer.h>

class ConsumerMock : public EegDataConsumer {

    public:

        ConsumerMock() :
            _TimesCalled(0)
        {
            _ReceivedSamples.channel_1 = 0;
            _ReceivedSamples.channel_2 = 0;
            _ReceivedSamples.channel_3 = 0;
            _ReceivedSamples.channel_4 = 0;
            _ReceivedSamples.channel_5 = 0;
            _ReceivedSamples.channel_6 = 0;
            _ReceivedSamples.channel_7 = 0;
            _ReceivedSamples.channel_8 = 0;
        }

        void PushLatestSample(EegData::sEegSamples samples) {
            _ReceivedSamples = samples;
            _TimesCalled++;
        }

        uint8_t GetTimesCalled() {
            return _TimesCalled;
        }

        EegData::sEegSamples GetReceivedSamples()
        {
            return _ReceivedSamples;
        }
        
    protected:

    private:

        uint8_t _TimesCalled;
        EegData::sEegSamples _ReceivedSamples;
};

#endif
