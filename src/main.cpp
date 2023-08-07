
#include <PinControl.h>
#include <ProtocolReceiver.h>
#include <ProtocolTransmitter.h>
#include <SpiDriver.h>
#include <Ads1299Driver.h>
#include <Ads1299DataProcessor.h>
#include <ErrorHandler.h>
#include <EventHandler.h>
#include <DataFlowController.h>
#include <LedControl.h>
#include <FakeDataProducer.h>
#include <SerialPort.h>

#include <Arduino.h>
#include <MyDelay.h>

//
//  Class instances
//
PinControl pinControl;
ProtocolReceiver protocolReceiver;
ProtocolTransmitter protocolTransmitter;
SpiDriver spiDriver;
Ads1299Driver ads1299Driver;
Ads1299DataProcessor ads1299DataProcessor;
ErrorHandler errorHandler;
EventHandler eventHandler;
DataFlowController dataFlowController;
LedControl ledControl;
FakeDataProducer fakeDataProducer;
SerialPort serialPort;

//
//  Local function declarations
//
void eventTimerUpdate();

// 
//  Local variables
//
MyDelay eventTimer(1, eventTimerUpdate, MYDELAY_REPEAT_FOREVER ); // 1mS event timer

//
//  Arduino setup function
//
void setup() {
  // instantiate classes
  errorHandler = ErrorHandler();
  eventHandler = EventHandler(&errorHandler);
  pinControl = PinControl(&eventHandler);
  serialPort = SerialPort(&eventHandler);
  spiDriver = SpiDriver();
  ads1299Driver = Ads1299Driver(&spiDriver, &pinControl, Ads1299Driver::eMontage::Referential);
  ads1299DataProcessor = Ads1299DataProcessor(&ads1299Driver, &eventHandler, &errorHandler);
  fakeDataProducer = FakeDataProducer(&eventHandler);
  protocolTransmitter = ProtocolTransmitter(&serialPort, &eventHandler);
  protocolReceiver = ProtocolReceiver(&serialPort, &dataFlowController, &eventHandler, &protocolTransmitter);
  dataFlowController = DataFlowController(&fakeDataProducer, NEvent::eEvent::Event_EDFDataReady,
                                            &ads1299DataProcessor, NEvent::eEvent::Event_BufferedADS1299DataReady,
                                            &protocolTransmitter,
                                            &protocolTransmitter);
  ledControl = LedControl(&errorHandler, &pinControl);

  // Add handlers for the 1mS timeout event
  eventHandler.AddEventHandler(&ledControl, NEvent::Event_1mSTimeout);
  eventHandler.AddEventHandler(&fakeDataProducer, NEvent::eEvent::Event_1mSTimeout);

  // Add data ready event handlers
  eventHandler.AddEventHandler(&dataFlowController, NEvent::eEvent::Event_BufferedADS1299DataReady);
  eventHandler.AddEventHandler(&dataFlowController, NEvent::eEvent::Event_EDFDataReady);

  eventHandler.AddEventHandler(&ads1299DataProcessor, NEvent::Event_ADS1299DataReady);

  // Add Data rx from PC event handlers  
  eventHandler.AddEventHandler(&protocolReceiver, NEvent::Event_DataRxFromPC);
  eventHandler.AddEventHandler(&serialPort, NEvent::Event_DataRxFromPC);  // Serial port must be the last event handler for the DataRxFromPc Event

  // Add Data to tx to PC event handlers
  eventHandler.AddEventHandler(&protocolTransmitter, NEvent::Event_DataToTxToPC);

  //! @todo these should be done elswhere.
  dataFlowController.SetProducer(DataFlowController::eProducerConsumer::secondary);

  // start the event timer.
  eventTimer.start();
}

//
//  Arduino Main loop function
//
void loop() {

  // Handle any system events  
  eventHandler.HandleEvents();
  
  // perform any background tasks after handling events, so that an event raised by the background
  // task handlers is less likely to overrun.
  eventTimer.update();  // Check the event timer - automatically calls eventTimerUpdate if timer has elapsed.
  serialPort.BackgroundTaskHandler(); // handle serial background tasks
  protocolTransmitter.BackgroundTaskHandler(); // Handle repeat transmissions to the PC.
}

//
//  Function to raise the 1mS timeout event every 1mS.
//
void eventTimerUpdate()
{
  // 1mS timeout elapsed so raise the event
  eventHandler.SignalEvent(NEvent::Event_1mSTimeout);
}