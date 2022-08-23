
#include <ProtocolParser.h>
#include <SerialPort.h>
#include <FakeDataProducer.h>

#include <unity.h>

ProtocolParser uut;
ErrorHandler erh;
EventHandler evh;
SerialPort sp;
FakeDataProducer fdp;

void setUp(void) {
    erh = ErrorHandler();
    evh = EventHandler(&erh);
    sp = SerialPort(&evh);
    fdp = FakeDataProducer(&evh);
    uut = ProtocolParser(&sp, &fdp);
}

void tearDown(void) {
}

//
//  test protocol parser starts in the waiting for sync state
//
void test_ProtocolParserDefaultsToWaitingForSync(void) 
{
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser stays in the waiting for sync state for invalid sync patterns
//
void test_ProtocolParserRemainsWaitingForSyncWithInvalidSync(void) 
{
    // TODO - make SerialPort an interface.
    // uut.ProcessEvent(NEvent::Event_DataRxFromPC);


    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolParserDefaultsToWaitingForSync);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync);
    UNITY_END();

    return 0;
}
