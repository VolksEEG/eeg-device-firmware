
#ifndef _DATA_FLOW_CONTROLLER
#define _DATA_FLOW_CONTROLLER

#include <stdint.h>

#include "IEEGDataProducer.h"
#include "IEEGDataConsumer.h"

#include <ICanProcesEvents.h>

class DataFlowController : public ICanProcessEvents, public IEegDataProducer, IEegDataConsumer {

    public:

        typedef enum _PROD_CONS
        {
            primary,
            secondary
        }eProducerConsumer;

        DataFlowController();
        DataFlowController(IEegDataProducer * primaryProducer, 
                            NEvent::eEvent primaryDataReadyEvent,
                            IEegDataProducer * secondaryProducer,
                            NEvent::eEvent secondaryDataReadyEvent,
                            IEegDataConsumer * primaryConsumer,
                            IEegDataConsumer * secondaryConsumer);
        
        void SetProducer(eProducerConsumer priOrSec);
        void SetConsumer(eProducerConsumer priOrSec);

        void ProcessEvent(NEvent::eEvent event) override;

        void PushLatestSample(EegData::sEegSamples samples) override {

        }

        void StartProducingData() override;
        void StopProducingData() override;

        EegData::sEegSamples GetLatestSample() override {
            // Null function
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

        IEegDataProducer * _PrimaryProducerInstance;
        NEvent::eEvent _PrimaryDataReadyEvent;
        IEegDataProducer * _SecondaryProducerInstance;
        NEvent::eEvent _SecondaryDataReadyEvent;
        IEegDataProducer * _CurrentProducerInstance;
        NEvent::eEvent _CurrentDataReadyEvent;

        IEegDataConsumer * _PrimaryConsumerInstance;
        IEegDataConsumer * _SecondaryConsumerInstance;
        IEegDataConsumer * _CurrentConsumerInstance;

        bool _DataProductionInProgress;
};

#endif