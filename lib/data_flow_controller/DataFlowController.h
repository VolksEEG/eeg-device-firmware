
#ifndef _DATA_FLOW_CONTROLLER
#define _DATA_FLOW_CONTROLLER

#include <stdint.h>

#include "EEGDataProducer.h"
#include "EEGDataConsumer.h"

#include <CanProcesEvents.h>

class DataFlowController : public CanProcessEvents, EegDataProducer, EegDataConsumer {

    public:

        typedef enum _PROD_CONS
        {
            primary,
            secondary
        }eProducerConsumer;

        DataFlowController();
        DataFlowController(EegDataProducer * primaryProducer, 
                            NEvent::eEvent primaryDataReadyEvent,
                            EegDataProducer * secondaryProducer,
                            NEvent::eEvent secondaryDataReadyEvent,
                            EegDataConsumer * primaryConsumer,
                            EegDataConsumer * secondaryConsumer);
        
        void SetProducer(eProducerConsumer priOrSec);
        void SetConsumer(eProducerConsumer priOrSec);

        void ProcessEvent(NEvent::eEvent event) override;

        void PushLatestSample(EegData::sEegSamples samples) override {

        }

        EegData::sEegSamples GetLatestSample() override {
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

        EegDataProducer * _PrimaryProducerInstance;
        NEvent::eEvent _PrimaryDataReadyEvent;
        EegDataProducer * _SecondaryProducerInstance;
        NEvent::eEvent _SecondaryDataReadyEvent;
        EegDataProducer * _CurrentProducerInstance;
        NEvent::eEvent _CurrentDataReadyEvent;

        EegDataConsumer * _PrimaryConsumerInstance;
        EegDataConsumer * _SecondaryConsumerInstance;
        EegDataConsumer * _CurrentConsumerInstance;

};

#endif