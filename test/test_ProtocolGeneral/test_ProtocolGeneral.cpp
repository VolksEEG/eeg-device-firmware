
#include <ProtocolGeneral.h>

#include <unity.h>

ProtocolGeneral pg;

void setUp(void) {
    pg = ProtocolGeneral();
}

void tearDown(void) {
}

void test_ProtocolGeneralHeaderChecksumCalculatedCorrectly(void)
{
    uint8_t message[pg._MAX_MESSAGE_LENGTH];

    message[pg._SYNC_WORD_MSB_INDEX] = pg._SYNC_WORD_MSB;
    message[pg._SYNC_WORD_LSB_INDEX] = pg._SYNC_WORD_LSB;
    message[pg._PROTOCOL_VERSION_INDEX] = pg._IMPLEMENTED_PROTOCOL_VERSION;
    message[pg._PAYLOAD_LENGTH_INDEX] = 10;
    message[pg._ID_NUMBER_INDEX] = 0;
    message[pg._ID_ACKNOWLEDGE_INDEX] = 0;
    message[pg._PAYLOAD_CHECKSUM_INDEX] = 0;
    message[pg._HEADER_CHECKSUM_INDEX] = 0;

    TEST_ASSERT_EQUAL(0xFE, pg.CalculateChecksumOfMessageHeader(message));
}

void test_ProtocolGeneralPayloadChecksumCalculatedCorrectly(void)
{
    uint8_t message[pg._MAX_MESSAGE_LENGTH] = {0};

    // give the message a known payload
    for (int i = 0; i < 10; ++i)
    {
        message[pg._PAYLOAD_START_INDEX + i] = i;
    }

    // with a known length
    message[pg._PAYLOAD_LENGTH_INDEX] = 10;

    TEST_ASSERT_EQUAL(0x85, pg.CalculateChecksumOfMessagePayload(message));
}

void test_ProtocolGeneralHeaderChecksumCalculationDoesNotUpdateTheMessage(void)
{
    uint8_t message[pg._MAX_MESSAGE_LENGTH];

    message[pg._SYNC_WORD_MSB_INDEX] = pg._SYNC_WORD_MSB;
    message[pg._SYNC_WORD_LSB_INDEX] = pg._SYNC_WORD_LSB;
    message[pg._PROTOCOL_VERSION_INDEX] = pg._IMPLEMENTED_PROTOCOL_VERSION;
    message[pg._PAYLOAD_LENGTH_INDEX] = 10;
    message[pg._ID_NUMBER_INDEX] = 0;
    message[pg._ID_ACKNOWLEDGE_INDEX] = 0;
    message[pg._HEADER_CHECKSUM_INDEX] = 0;

    // set the header checksum to known incorrect value
    const uint8_t SET_CHECKSUM = 0x12;
    message[pg._HEADER_CHECKSUM_INDEX] = SET_CHECKSUM;

    const uint8_t CALC_CHECKSUM = pg.CalculateChecksumOfMessageHeader(message);

    TEST_ASSERT_NOT_EQUAL(SET_CHECKSUM, CALC_CHECKSUM);                     // double check that the set checksum is incorrect
    TEST_ASSERT_EQUAL(SET_CHECKSUM, message[pg._HEADER_CHECKSUM_INDEX]);   // and confirm that the correct checksum has not been populated to the message
}

void test_ProtocolGeneralPayloadChecksumCalculationDoesNotUpdateTheMessage(void)
{
    uint8_t message[pg._MAX_MESSAGE_LENGTH] = {0};

    // give the message a known payload
    for (int i = 0; i < 10; ++i)
    {
        message[pg._PAYLOAD_START_INDEX + i] = i;
    }

    // set the payload checksum to known incorrect value
    const uint8_t SET_CHECKSUM = 0x12;
    message[pg._PAYLOAD_CHECKSUM_INDEX] = SET_CHECKSUM;

    const uint8_t CALC_CHECKSUM = pg.CalculateChecksumOfMessagePayload(message);

    TEST_ASSERT_NOT_EQUAL(SET_CHECKSUM, CALC_CHECKSUM);                     // double check that the set checksum is incorrect
    TEST_ASSERT_EQUAL(SET_CHECKSUM, message[pg._PAYLOAD_CHECKSUM_INDEX]);   // and confirm that the correct checksum has not been populated to the message
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolGeneralHeaderChecksumCalculatedCorrectly);
    RUN_TEST(test_ProtocolGeneralPayloadChecksumCalculatedCorrectly);
    RUN_TEST(test_ProtocolGeneralHeaderChecksumCalculationDoesNotUpdateTheMessage);
    RUN_TEST(test_ProtocolGeneralPayloadChecksumCalculationDoesNotUpdateTheMessage);
    /*RUN_TEST(test_ProtocolReceiverGoesToGetProtocolVersionWithValidSync);
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
