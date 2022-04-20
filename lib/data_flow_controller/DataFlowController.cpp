
#include "DataFlowController.h"

//
// Constructor
//
DataFlowController::DataFlowController() :
    _PrimaryProducerInstance(this),
    _SecondaryProducerInstance(this),
    _CurrentProducerInstance(this),
    _PrimaryConsumerInstance(this),
    _SecondaryConsumerInstance(this),
    _CurrentConsumerInstance(this),
    _CurrentDataReadyEvent(NEvent::eEvent::Event_ADS1299DataReady)
{

}

//
// Constructor
//
DataFlowController::DataFlowController(EegDataProducer * primaryProducer, 
                            NEvent::eEvent primaryDataReadyEvent,
                            EegDataProducer * secondaryProducer,
                            NEvent::eEvent secondaryDataReadyEvent,
                            EegDataConsumer * primaryConsumer,
                            EegDataConsumer * secondaryConsumer) :
    _PrimaryProducerInstance(primaryProducer),
    _PrimaryDataReadyEvent(primaryDataReadyEvent),
    _SecondaryProducerInstance(secondaryProducer),
    _SecondaryDataReadyEvent(secondaryDataReadyEvent),
    _CurrentProducerInstance(primaryProducer),
    _CurrentDataReadyEvent(primaryDataReadyEvent),
    _PrimaryConsumerInstance(primaryConsumer),
    _SecondaryConsumerInstance(secondaryConsumer),
    _CurrentConsumerInstance(primaryConsumer)
{
    
}
        
//
// Sets the producer to either be the primary or secondary 
//
void DataFlowController::SetProducer(eProducerConsumer priOrSec)
{
    // TODO - Stop producing data on the current producer.

    switch (priOrSec)
    {
        case primary:
            _CurrentProducerInstance = _PrimaryProducerInstance;
            _CurrentDataReadyEvent = _PrimaryDataReadyEvent;
            break;
        case secondary:
            _CurrentProducerInstance = _SecondaryProducerInstance;
            _CurrentDataReadyEvent = _SecondaryDataReadyEvent;
            break;
        default:
            // all cases are covered above.
            break;
    }
}

//
// Sets the consumer to either be the primary or secondary 
//
void DataFlowController::SetConsumer(eProducerConsumer priOrSec)
{
    switch (priOrSec)
    {
        case primary:
            _CurrentConsumerInstance = _PrimaryConsumerInstance;
            break;
        case secondary:
            _CurrentConsumerInstance = _SecondaryConsumerInstance;
            break;
        default:
            // all cases are covered above.
            break;
    }
}

//
// Overriden function from CanProcessEvents
//
void DataFlowController::ProcessEvent(NEvent::eEvent event)
{
    if (event != _CurrentDataReadyEvent)
    {
        // this is the other data ready event so ignore it.
        return;
    }

    // get the samples from the producer
   const EegData::sEegSamples SAMPLES = _CurrentProducerInstance->GetLatestSample();

   // pass them on the the consumer
   _CurrentConsumerInstance->PushLatestSample(SAMPLES);
}


//
//  Overridden function from EegDataProducer to start producing EEG data
//
void DataFlowController::StartProducingData()
{
    // TODO - call the Start Producing Data function on the current producer

}
