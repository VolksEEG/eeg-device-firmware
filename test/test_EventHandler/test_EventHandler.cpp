
#include <EventHandler.h>
#include <unity.h>

//
//  Test test to set the value
//
void test_EventHandler_EventOverrunIsDetected(void) {

    ErrorHandler eh = ErrorHandler();
    EventHandler uut = EventHandler(eh);

    uut.SignalEvent(EventHandler::eEvent::Event_ADS1299DataReady);
    uut.SignalEvent(EventHandler::eEvent::Event_ADS1299DataReady);

    TEST_ASSERT_TRUE(eh.IsErrorPresent());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_EventHandler_EventOverrunIsDetected);
    UNITY_END();

    return 0;
}
