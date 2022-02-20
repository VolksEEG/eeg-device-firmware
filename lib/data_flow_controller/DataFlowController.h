
#ifndef _DATA_FLOW_CONTROLLER
#define _DATA_FLOW_CONTROLLER

#include <stdint.h>

#include "EEGDataProducer.h"
#include "EEGDataConsumer.h"

#include <CanProcesEvents.h>

class DataFlowController : CanProcessEvents, EegDataProducer, EegDataConsumer {

    public:

        typedef enum _PROD_CONS
        {
            primary,
            secondary
        }eProducerConsumer;

        DataFlowController();
        DataFlowController(EegDataProducer * primaryProducer, 
                            EegDataProducer * secondaryProducer,
                            EegDataConsumer * primaryConsumer,
                            EegDataConsumer * secondaryConsumer);
        
        void SetProducer(eProducerConsumer priOrSec);
        void SetConsumer(eProducerConsumer priOrSec);

        void ProcessEvent(NEvent::eEvent event);

        void PushLatestSample(EegData::sEegSamples samples) {

        }

        EegData::sEegSamples GetLatestSample() {
            EegData::sEegSamples dummy;
            return dummy;
        }

    protected:

    private:

        EegDataProducer * _PrimaryProducerInstance;
        EegDataProducer * _SecondaryProducerInstance;
        EegDataProducer * _CurrentProducerInstance;

        EegDataConsumer * _PrimaryConsumerInstance;
        EegDataConsumer * _SecondaryConsumerInstance;
        EegDataConsumer * _CurrentConsumerInstance;
};

#endif