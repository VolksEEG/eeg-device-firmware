#include <ProtocolTransmitter.h>

#include "../Mocks/MockEventHandler.h"
#include "../Mocks/MockPcCommunicationsInterface.h"

#include <unity.h>

MockPcCommunicationsInterface pci;
MockEvenHandler eh;
ProtocolTransmitter uut;

void setUp(void) {
    pci = MockPcCommunicationsInterface();
    eh = MockEvenHandler();

    uut = ProtocolTransmitter(&pci, &eh);
}

void tearDown(void) {
}

void test_ProtocolTransmitterDataToTxToPCEventIsRaisedWhenFirstPayloadIsToBeTransmitted(void)
{
    uint8_t testPayload[1] = {0};

    uut.SendPayloadToPc(testPayload, 1);

    TEST_ASSERT_EQUAL(1, eh.GetNumberOfEventsSignalled());
    TEST_ASSERT_EQUAL(NEvent::Event_DataToTxToPC, eh.GetLastSignalledEvent());
}

void test_ProtocolTransmitterDataToTxToPCEventIsNotRaisedWhenSubsequentPayloadsAreToBeTransmitted(void)
{
    // send the first one
    test_ProtocolTransmitterDataToTxToPCEventIsRaisedWhenFirstPayloadIsToBeTransmitted();

    uint8_t testPayload[1] = {0};

    uut.SendPayloadToPc(testPayload, 1);

    TEST_ASSERT_EQUAL(1, eh.GetNumberOfEventsSignalled());
    TEST_ASSERT_EQUAL(NEvent::Event_DataToTxToPC, eh.GetLastSignalledEvent());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolTransmitterDataToTxToPCEventIsRaisedWhenFirstPayloadIsToBeTransmitted);
    RUN_TEST(test_ProtocolTransmitterDataToTxToPCEventIsNotRaisedWhenSubsequentPayloadsAreToBeTransmitted);
    /*RUN_TEST(test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync2);
    RUN_TEST(test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync3);
    RUN_TEST(test_ProtocolReceiverGoesToGetProtocolVersionWithValidSync);
    RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncWithInvalidProtocolVersion);
    RUN_TEST(test_ProtocolReceiverGoesToGetPayloadLengthWithValidProtocolVersion);
    RUN_TEST(test_ProtocolReceiverGoesToWaitForSyncIfThePayloadLengthIsTooLarge);
    RUN_TEST(test_ProtocolReceiverGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge);
    RUN_TEST(test_ProtocolReceiverGoesToGetAckIdAfterTheGetIdState);
    RUN_TEST(test_ProtocolReceiverGoesToGetChecksumAfterTheAckIdState);
    RUN_TEST(test_ProtocolReceiverGoesToGetPayloadStateAfterTheGetChecksumState);
    RUN_TEST(test_ProtocolReceiverRemainsInGetPayloadStateBeforeAllPayloadBytesAreReceived);
    RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncAfterAllPayloadBytesAreReceived);
    RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncSequenceIfThePayloadIsZeroLength);
    RUN_TEST(test_ProtocolReceiverInitiallyExpectesAnIdOfZero);
    RUN_TEST(test_ProtocolReceiverIncrementsTheExpectedIdAfterEachMessageWithACorrectId);
    RUN_TEST(test_ProtocolReceiverDoesNoIncrementTheExpectedIdAfterAMessageWithAnIncorrectId);*/
    UNITY_END();

    return 0;
}
