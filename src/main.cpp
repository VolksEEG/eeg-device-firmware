
#include <PinControl.h>
#include <ProtocolFrameParser.h>
#include <ProtocolParser.h>
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
ProtocolFrameParser protocolFrameParser;
ProtocolParser protocolParser;
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
  protocolFrameParser = ProtocolFrameParser(&serialPort);
  spiDriver = SpiDriver();
  ads1299Driver = Ads1299Driver(&spiDriver, &pinControl);
  ads1299DataProcessor = Ads1299DataProcessor(&ads1299Driver, &eventHandler);
  fakeDataProducer = FakeDataProducer(&eventHandler);
  dataFlowController = DataFlowController(&fakeDataProducer, NEvent::eEvent::Event_EDFDataReady,
                                            &ads1299DataProcessor, NEvent::eEvent::Event_ProcessedADS1299DataReady,
                                            &protocolParser,
                                            &protocolParser);
  ledControl = LedControl(&errorHandler, &pinControl);
  protocolParser = ProtocolParser(&protocolFrameParser, &serialPort, &dataFlowController);

  // Add handlers for the 1mS timeout event
  eventHandler.AddEventHandler(&ledControl, NEvent::Event_1mSTimeout);
  eventHandler.AddEventHandler(&fakeDataProducer, NEvent::eEvent::Event_1mSTimeout);

  // Add data ready event handlers
  eventHandler.AddEventHandler(&dataFlowController, NEvent::eEvent::Event_ProcessedADS1299DataReady);
  eventHandler.AddEventHandler(&dataFlowController, NEvent::eEvent::Event_EDFDataReady);

  eventHandler.AddEventHandler(&ads1299DataProcessor, NEvent::Event_ADS1299DataReady);

  // Add Data rx from PC event handlers  
  eventHandler.AddEventHandler(&protocolParser, NEvent::Event_DataRxFromPC);
  eventHandler.AddEventHandler(&serialPort, NEvent::Event_DataRxFromPC);  // Serial port must be the last event handler for the DataRxFromPc Event

  // TODO - these should be done elswhere.
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
}

//
//  Function to raise the 1mS timeout event every 1mS.
//
void eventTimerUpdate()
{
  // 1mS timeout elapsed so raise the event
  eventHandler.SignalEvent(NEvent::Event_1mSTimeout);
}