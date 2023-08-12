
#include <ProtocolPayloadParser.h>

#include "../Mocks/MockIEegDataProducer.h"
#include "../Mocks/MockIProtocolTransmission.h"

#include <unity.h>

ProtocolPayloadParser ppp;
MockIEegDataProducer medp;
MockIProtocolTransmission mpti;

void setUp(void) {
    medp = MockIEegDataProducer();
    mpti = MockIProtocolTransmission();
    ppp = ProtocolPayloadParser(&medp, &mpti);
}

void tearDown(void) {
}

void test_ProtocolPayloadParser_NullPointerReturnFalse(void)
{
    TEST_ASSERT_FALSE(ppp.ParsePayload((uint8_t*)0, 1));
}

void test_ProtocolPayloadParser_ZeroLengthPayloadReturnFalse(void)
{
    uint8_t payload[] = {
        0x02
    };

    TEST_ASSERT_FALSE(ppp.ParsePayload(payload, 0));
}

void test_ProtocolPayloadParser_InvalidCommandGroupsReturnFalse(void)
{
    // Test each invalid command group returns false
    for (uint16_t command_group = 0x0040; command_group < 0x0100; command_group += 0x20)
    {
        uint8_t payload = (uint8_t)command_group;

        TEST_ASSERT_FALSE(ppp.ParsePayload(&payload, 1));
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolPayloadParser_NullPointerReturnFalse);
    RUN_TEST(test_ProtocolPayloadParser_ZeroLengthPayloadReturnFalse);
    RUN_TEST(test_ProtocolPayloadParser_InvalidCommandGroupsReturnFalse);
    /*
    RUN_TEST(test_ProtocolGeneralPayloadChecksumCalculationDoesNotUpdateTheMessage);
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
