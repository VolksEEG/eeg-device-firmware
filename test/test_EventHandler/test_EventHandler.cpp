
#include <EventHandler.h>
#include <unity.h>

class EventProcessTest : public CanProcessEvents {
    public:

    EventProcessTest() : 
        _ProcessEventCalledCount(0)
    {

    }

    void ProcessEvent(NEvent::eEvent event) {
        _ProcessEventCalledCount++;
    }

    int GetProcessedEventCalledTimes() 
    {
        return _ProcessEventCalledCount;
    }

    private:

        int _ProcessEventCalledCount;
};

//
//  confirm that event overruns are detected
//
void test_EventHandler_EventOverrunIsDetected(void) {

    ErrorHandler eh = ErrorHandler();
    EventHandler uut = EventHandler(&eh);

    uut.SignalEvent(NEvent::eEvent::Event_ADS1299DataReady);
    uut.SignalEvent(NEvent::eEvent::Event_ADS1299DataReady);

    TEST_ASSERT_TRUE(eh.IsErrorPresent());
}

//
//  Confirm that an event processing function is called only once.
//
void test_EventHandler_EventProcessedOnlyOnce(void) {

    ErrorHandler eh = ErrorHandler();
    EventHandler uut = EventHandler(&eh);

    EventProcessTest ep = EventProcessTest();

    uut.AddEventHandler(&ep, NEvent::eEvent::Event_ADS1299DataReady);

    // signal the event
    uut.SignalEvent(NEvent::eEvent::Event_ADS1299DataReady);

    // then process the events twice
    uut.HandleEvents();
    uut.HandleEvents();

    TEST_ASSERT_EQUAL(1, ep.GetProcessedEventCalledTimes());
}

//
//  Confirm that an event processing function is called.
//
void test_EventHandler_EventProcessersFullErrorIsRaised(void) {

    ErrorHandler eh = ErrorHandler();
    EventHandler uut = EventHandler(&eh);

    EventProcessTest ep = EventProcessTest();

    for (int i = 0; i < EventHandler::MAX_PROCESS_HANDLERS; ++i)
    {
        // add the same event handler  max times
        uut.AddEventHandler(&ep, NEvent::eEvent::Event_ADS1299DataReady);
    }

    TEST_ASSERT_FALSE(eh.IsErrorPresent());

    // try to add one more
    uut.AddEventHandler(&ep, NEvent::eEvent::Event_ADS1299DataReady);

    TEST_ASSERT_TRUE(eh.IsErrorPresent());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_EventHandler_EventOverrunIsDetected);
    RUN_TEST(test_EventHandler_EventProcessedOnlyOnce);
    RUN_TEST(test_EventHandler_EventProcessersFullErrorIsRaised);
    UNITY_END();

    return 0;
}
