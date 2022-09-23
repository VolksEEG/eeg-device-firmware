
#include "DataFlowController.h"

//
// Constructor
//
DataFlowController::DataFlowController() :
    _PrimaryProducerInstance(this),
    _SecondaryProducerInstance(this),
    _CurrentProducerInstance(this),
    _PrimaryConsumerInstance(this),
    _CurrentDataReadyEvent(NEvent::eEvent::Event_ADS1299DataReady),
    _SecondaryConsumerInstance(this),
    _CurrentConsumerInstance(this),
    _DataProductionInProgress(false)
{

}

//
// Constructor
//
DataFlowController::DataFlowController(IEegDataProducer * primaryProducer, 
                            NEvent::eEvent primaryDataReadyEvent,
                            IEegDataProducer * secondaryProducer,
                            NEvent::eEvent secondaryDataReadyEvent,
                            IEegDataConsumer * primaryConsumer,
                            IEegDataConsumer * secondaryConsumer) :
    _PrimaryProducerInstance(primaryProducer),
    _PrimaryDataReadyEvent(primaryDataReadyEvent),
    _SecondaryProducerInstance(secondaryProducer),
    _SecondaryDataReadyEvent(secondaryDataReadyEvent),
    _CurrentProducerInstance(primaryProducer),
    _CurrentDataReadyEvent(primaryDataReadyEvent),
    _PrimaryConsumerInstance(primaryConsumer),
    _SecondaryConsumerInstance(secondaryConsumer),
    _CurrentConsumerInstance(primaryConsumer),
    _DataProductionInProgress(false)
{
    
}
        
//
// Sets the producer to either be the primary or secondary 
//
void DataFlowController::SetProducer(eProducerConsumer priOrSec)
{
    // make sure producing is stopped on the current producer
    _CurrentProducerInstance->StopProducingData();

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
// Overriden function from ICanProcessEvents
//
void DataFlowController::ProcessEvent(NEvent::eEvent event)
{
    if (event != _CurrentDataReadyEvent)
    {
        // this is the other data ready event so ignore it.
        return;
    }

    if (false == _DataProductionInProgress)
    {
        // data is not being produced so ignore this event.
        return;
    }

    // get the samples from the producer
   const EegData::sEegSamples SAMPLES = _CurrentProducerInstance->GetLatestSample();

   // pass them on the the consumer
   _CurrentConsumerInstance->PushLatestSample(SAMPLES);
}


//
//  Overridden function from IEegDataProducer to start producing EEG data
//
void DataFlowController::StartProducingData()
{
    _CurrentProducerInstance->StartProducingData();
    _DataProductionInProgress = true;
}

//
// Overridden function from IEegDataProducer to stop producing EEG data
//
void DataFlowController::StopProducingData()
{
    _CurrentProducerInstance->StopProducingData();
    _DataProductionInProgress = false;
}
