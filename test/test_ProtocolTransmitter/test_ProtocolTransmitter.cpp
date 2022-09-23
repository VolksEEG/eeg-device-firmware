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

void test_ProtocolTransmitterOnlyAllowsUptoMaxTxMessagesToBeQueued(void)
{
    uint8_t testPayload[1] = {0};
    uint8_t payloadsQueued = 0;

    while(true == uut.SendPayloadToPc(testPayload, 1))
    {
        payloadsQueued++;
    }

    TEST_ASSERT_EQUAL(uut._MAX_TX_MESSAGES, payloadsQueued);
}

void test_ProtocolTransmitterOnlyAllowsMessagesWithUptoMaxPayloadSizeBytesToBeQueued(void)
{
    uint8_t testPayload[ProtocolGeneral::_MAX_PAYLOAD_SIZE + 1] = {0};

    // confirm that a message with the max payload size payload can be added    
    TEST_ASSERT_TRUE(uut.SendPayloadToPc(testPayload, ProtocolGeneral::_MAX_PAYLOAD_SIZE));

    // and 1 byte longer can't be added
    TEST_ASSERT_FALSE(uut.SendPayloadToPc(testPayload, ProtocolGeneral::_MAX_PAYLOAD_SIZE + 1));

    // finally, confirm that the last failure was not a space issue
    TEST_ASSERT_TRUE(uut.SendPayloadToPc(testPayload, ProtocolGeneral::_MAX_PAYLOAD_SIZE));
    TEST_ASSERT_TRUE(uut.SendPayloadToPc(testPayload, ProtocolGeneral::_MAX_PAYLOAD_SIZE));
}

void test_ProtocolTransmitterUpdateAcknowledgedIdForFirstIdRemovesCorrectNumberOfMessagesFromTxQueue()
{
    uint8_t testPayload[1] = {0};

    // Arrange
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 0
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 1
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 2
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 3
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 4

    TEST_ASSERT_EQUAL(5, uut.GetTxCount());

    // Act
    uut.UpdateAcknowledgedId(0);

    // Assert
    TEST_ASSERT_EQUAL(4, uut.GetTxCount());
}

void test_ProtocolTransmitterUpdateAcknowledgedIdForLastIdRemovesCorrectNumberOfMessagesFromTxQueue()
{
    uint8_t testPayload[1] = {0};

    // Arrange
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 0
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 1
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 2
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 3
    uut.SendPayloadToPc(testPayload, 1);    // ID will be 4

    TEST_ASSERT_EQUAL(5, uut.GetTxCount());

    // Act
    uut.UpdateAcknowledgedId(4);

    // Assert
    TEST_ASSERT_EQUAL(0, uut.GetTxCount());
}

void test_ProtocolTransmitterUpdateAcknowledgedIdRemovesCorrectNumberOfMessagesFromTxQueueWhenQueueWrapsAround()
{
    uint8_t testPayload[1] = {0};
    uint8_t expectedID = 0;

    const int INITIAL_MESSAGES_TO_ACK = 1;

    // Arrange

    // fill up the queue with the INITIAL_MESSAGES_TO_ACK
    for (int i = 0; i < INITIAL_MESSAGES_TO_ACK; ++i)
    {
        uut.SendPayloadToPc(testPayload, 1);

        expectedID = i;
    }

    TEST_ASSERT_EQUAL(INITIAL_MESSAGES_TO_ACK, uut.GetTxCount());

    // Acknowledge all these messages in one
    uut.UpdateAcknowledgedId(expectedID);

    TEST_ASSERT_EQUAL(0, uut.GetTxCount());

    // fill up the full queue to cause a wrap around
    for (int i = 0; i < uut._MAX_TX_MESSAGES; ++i)
    {
        uut.SendPayloadToPc(testPayload, 1);

        expectedID++;
    }

    TEST_ASSERT_EQUAL(uut._MAX_TX_MESSAGES, uut.GetTxCount());

    // Acknowledge all these messages in one
    uut.UpdateAcknowledgedId(expectedID);

    TEST_ASSERT_EQUAL(0, uut.GetTxCount());
   
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolTransmitterDataToTxToPCEventIsRaisedWhenFirstPayloadIsToBeTransmitted);
    RUN_TEST(test_ProtocolTransmitterDataToTxToPCEventIsNotRaisedWhenSubsequentPayloadsAreToBeTransmitted);
    RUN_TEST(test_ProtocolTransmitterOnlyAllowsUptoMaxTxMessagesToBeQueued);
    RUN_TEST(test_ProtocolTransmitterOnlyAllowsMessagesWithUptoMaxPayloadSizeBytesToBeQueued);
    RUN_TEST(test_ProtocolTransmitterUpdateAcknowledgedIdForFirstIdRemovesCorrectNumberOfMessagesFromTxQueue);
    RUN_TEST(test_ProtocolTransmitterUpdateAcknowledgedIdForLastIdRemovesCorrectNumberOfMessagesFromTxQueue);
    RUN_TEST(test_ProtocolTransmitterUpdateAcknowledgedIdRemovesCorrectNumberOfMessagesFromTxQueueWhenQueueWrapsAround);
    /*RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncWithInvalidProtocolVersion);
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
