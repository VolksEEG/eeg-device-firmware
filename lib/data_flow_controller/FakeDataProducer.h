
#ifndef _FAKE_DATA_PRODUCER
#define _FAKE_DATA_PRODUCER

#include "EEGDataProducer.h"

#include <CanProcesEvents.h>
#include <EventHandler.h>

class FakeDataProducer : public EegDataProducer, public CanProcessEvents {

    public:

        FakeDataProducer();
        explicit FakeDataProducer(EventHandler * eh);

        EegData::sEegSamples GetLatestSample() override;

        void ProcessEvent(NEvent::eEvent event) override;

    protected:

    private:

        EventHandler * _EventHandlerPtr;

        uint8_t _TimerDivider;

        EegData::sEegSamples _SampleData;
};

#endif