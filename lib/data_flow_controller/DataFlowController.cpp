
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
    _CurrentConsumerInstance(this)
{

}

//
// Constructor
//
DataFlowController::DataFlowController(EegDataProducer * primaryProducer, 
                                        EegDataProducer * secondaryProducer,
                                        EegDataConsumer * primaryConsumer,
                                        EegDataConsumer * secondaryConsumer) :
    _PrimaryProducerInstance(primaryProducer),
    _SecondaryProducerInstance(secondaryProducer),
    _CurrentProducerInstance(primaryProducer),
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
    switch (priOrSec)
    {
        case primary:
            _CurrentProducerInstance = _PrimaryProducerInstance;
            break;
        case secondary:
            _CurrentProducerInstance = _SecondaryProducerInstance;
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
void DataFlowController::ProcessEvent()
{
   const EegData::sEegSamples SAMPLES = _CurrentProducerInstance->GetLatestSample();
}