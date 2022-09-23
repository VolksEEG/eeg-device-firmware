
#include <IEEGDataProducer.h>

class MockIEegDataProducer : public IEegDataProducer {

    public:

        MockIEegDataProducer() :
            _TimesCalled(0)
        {
            _Samples.channel_1 = 0;
            _Samples.channel_2 = 0;
            _Samples.channel_3 = 0;
            _Samples.channel_4 = 0;
            _Samples.channel_5 = 0;
            _Samples.channel_6 = 0;
            _Samples.channel_7 = 0;
            _Samples.channel_8 = 0;

        }

        void StartProducingData() override {

        }

        void StopProducingData() override {
            
        }

        EegData::sEegSamples GetLatestSample() override {

            _TimesCalled++;

            return _Samples;
        }

        uint8_t GetTimesCalled() {
            return _TimesCalled;
        }

        void SetOutputValues(EegData::sEegSamples newSamples)
        {
            _Samples = newSamples;
        }
        
    protected:

    private:

        uint8_t _TimesCalled;
        EegData::sEegSamples _Samples;
};
