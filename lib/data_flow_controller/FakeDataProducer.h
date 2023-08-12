
#ifndef _FAKE_DATA_PRODUCER
#define _FAKE_DATA_PRODUCER

#include "IEEGDataProducer.h"

#include "../event_handler/ICanProcesEvents.h"
#include "../event_handler/EventHandler.h"

class FakeDataProducer : public IEegDataProducer, public ICanProcessEvents {

    public:

        FakeDataProducer();
        explicit FakeDataProducer(EventHandler * eh);

        void StartProducingData() override; 
        void StopProducingData() override; 

        EegData::sEegSamples GetLatestSample() override;

        void ProcessEvent(NEvent::eEvent event) override;

    protected:

    private:

        EventHandler * _EventHandlerPtr;

        uint8_t _TimerDivider;

        EegData::sEegSamples _SampleData;
};

#endif