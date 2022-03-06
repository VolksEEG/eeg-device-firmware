
#include <PinControl.h>
#include <ProtocolFrameParser.h>
#include <ProtocolParser.h>
#include <SpiDriver.h>
#include <Ads1299Driver.h>
#include <ErrorHandler.h>
#include <EventHandler.h>
#include <DataFlowController.h>
#include <LedControl.h>

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
ErrorHandler errorHandler;
EventHandler eventHandler;
DataFlowController dataFlowController;
LedControl ledControl;

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
  protocolFrameParser = ProtocolFrameParser();
  protocolParser = ProtocolParser(&protocolFrameParser);
  spiDriver = SpiDriver();
  ads1299Driver = Ads1299Driver(&spiDriver, &pinControl);
  dataFlowController = DataFlowController(&ads1299Driver, NEvent::eEvent::Event_ADS1299DataReady,
                                            &ads1299Driver, NEvent::eEvent::Event_ADS1299DataReady,
                                            &protocolParser,
                                            &protocolParser);
  ledControl = LedControl(&errorHandler, &pinControl);

  // add handler for the 1mS timeout event
  eventHandler.AddEventHandler(&ledControl, NEvent::Event_1mSTimeout);

  // add data ready event handlers
  eventHandler.AddEventHandler(&dataFlowController, NEvent::eEvent::Event_ADS1299DataReady);
  eventHandler.AddEventHandler(&dataFlowController, NEvent::eEvent::Event_EDFDataReady);

  // start the event timer.
  eventTimer.start();
}

//
//  Arduino Main loop function
//
void loop() {

  // Handle any system events  
  eventHandler.HandleEvents();
  
  // Check the event timer - automatically calls eventTimerUpdate if timer has elapsed.
  eventTimer.update();
}

//
//  Function to raise the 1mS timeout event every 1mS.
//
void eventTimerUpdate()
{
  // 1mS timeout elapsed so raise the event
  eventHandler.SignalEvent(NEvent::Event_1mSTimeout);
}