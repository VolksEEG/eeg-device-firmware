
#include <ProtocolParser.h>
#include <EEGDataProducer.h>

#include <unity.h>

class MockPcCommunicationsInterface : public PcCommunicationsInterface {
    public:

        MockPcCommunicationsInterface()
        {
            read_index = 0;
            read_count = 0;

            for (int i = 0; i < READ_BYTES_MAX; ++i)
            {
                read_bytes[i] = 0;
            }
        }

        uint8_t GetReceivedBytes(uint8_t data[], uint8_t max_length) override {
            
            uint8_t copied_bytes = 0;

            while   (
                        (max_length > copied_bytes)
                        && (read_index < read_count)
                    )
            {
                data[copied_bytes++] = read_bytes[read_index++];
            }

            return copied_bytes;
        }

        void TransmitData(uint8_t data[], uint8_t count) override {

        }

        void LoadReceiveBytes(uint8_t data[], uint8_t count)
        {
            for (int i = 0; i < count; ++i)
            {
                read_bytes[i] = data[i];
            }

            read_index = 0;
            read_count = count;
        }

    private:

        static const uint8_t READ_BYTES_MAX = 100;
        uint8_t read_index;
        uint8_t read_count;
        uint8_t read_bytes[READ_BYTES_MAX];
};

class MockEegDataProducer : public EegDataProducer {
    public:

        void StartProducingData() override {

        }

        void StopProducingData() override {

        }

    private:

};

MockPcCommunicationsInterface pci;
MockEegDataProducer edp;
ProtocolParser uut;

void setUp(void) {
    pci = MockPcCommunicationsInterface();
    edp = MockEegDataProducer();

    uut = ProtocolParser(&pci, &edp);
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
void test_ProtocolParserRemainsWaitingForSyncWithInvalidSync1(void) 
{
    // setup
    uint8_t data[2] = {0x55, 0x55};
    pci.LoadReceiveBytes(data, 2);

    // run 
    // process both bytes
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser stays in the waiting for sync state for invalid sync patterns
//
void test_ProtocolParserRemainsWaitingForSyncWithInvalidSync2(void) 
{
    // setup
    uint8_t data[2] = {0x55, 0xAA};
    pci.LoadReceiveBytes(data, 2);

    // run 
    // process both bytes
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser stays in the waiting for sync state for invalid sync patterns
//
void test_ProtocolParserRemainsWaitingForSyncWithInvalidSync3(void) 
{
    // setup
    uint8_t data[2] = {0xAA, 0xAA};
    pci.LoadReceiveBytes(data, 2);

    // run 
    // process both bytes
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the Get Protocol Version state after Valid sync pattern
//
void test_ProtocolParserGoesToGetProtocolVersionWithValidSync(void) 
{
    // setup
    uint8_t data[2] = {0xAA, 0x55};
    pci.LoadReceiveBytes(data, 2);

    // run 
    // process both bytes
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetProtocolVersion, uut.GetCurrentRxState());
}

//
//  test protocol parser goes back to the Wait For Sync state if the protocol version is not the version implemented
//
void test_ProtocolParserReturnsToWaitingForSyncWithInvalidProtocolVersion(void) 
{
    // setup
    test_ProtocolParserGoesToGetProtocolVersionWithValidSync();

    uint8_t data[1] = {(uint8_t)~uut.GetImplementedProtocolVersion()};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the Get Payload Length state if the protocol version is the version implemented
//
void test_ProtocolParserGoesToGetPayloadLengthWithValidProtocolVersion(void) 
{
    // setup
    test_ProtocolParserGoesToGetProtocolVersionWithValidSync();

    uint8_t data[1] = {uut.GetImplementedProtocolVersion()};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayloadLength, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the Wait for sync state if the payload version is too large
//
void test_ProtocolParserGoesToWaitForSyncIfThePayloadLengthIsTooLarge(void) 
{
    // setup
    test_ProtocolParserGoesToGetPayloadLengthWithValidProtocolVersion();

    uint8_t data[1] = {(uint8_t)(uut.GetMaximumPayloadLength() + 1)};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get Id state if the payload version is not too large
//
void test_ProtocolParserGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge(void) 
{
    // setup
    test_ProtocolParserGoesToGetPayloadLengthWithValidProtocolVersion();

    uint8_t data[1] = {uut.GetMaximumPayloadLength()};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetIdNumber, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the ack Id state after get id state
//
void test_ProtocolParserGoesToGetAckIdAfterTheGetIdState(void) 
{
    // setup
    test_ProtocolParserGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge();

    uint8_t data[1] = {0x00};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetAckId, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get checksum after the ack Id state
//
void test_ProtocolParserGoesToGetChecksumAfterTheAckIdState(void) 
{
    // setup
    test_ProtocolParserGoesToGetAckIdAfterTheGetIdState();

    uint8_t data[1] = {0x00};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetChecksum, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get payload state after the get checksum state
//
void test_ProtocolParserGoesToGetPayloadStateAfterTheGetChecksumState(void) 
{
    // setup
    test_ProtocolParserGoesToGetChecksumAfterTheAckIdState();

    uint8_t data[1] = {0x00};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayload, uut.GetCurrentRxState());
}

//
//  test protocol parser stays in the get payload state while all bytes are not yet received.
//
void test_ProtocolParserRemainsInGetPayloadStateBeforeAllPayloadBytesAreReceived(void) 
{
    // setup
    uint8_t data[16] = {0xAA, 0x55, uut.GetImplementedProtocolVersion(), 10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    pci.LoadReceiveBytes(data, 16);

    // run 
    for (int i = 0; i < 16; ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayload, uut.GetCurrentRxState());
}

//
//  test protocol parser returns to looking for the next packet when all bytes are received.
//
void test_ProtocolParserReturnsToWaitingForSyncAfterAllPayloadBytesAreReceived(void) 
{
    // setup
    uint8_t data[16] = {0xAA, 0x55, uut.GetImplementedProtocolVersion(), 9, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
    pci.LoadReceiveBytes(data, 16);

    // run 
    for (int i = 0; i < 16; ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser returns to waiting for Sync Sequence if the payload is zero length.
//
void test_ProtocolParserReturnsToWaitingForSyncSequenceIfThePayloadIsZeroLength(void) 
{   
    // setup
    uint8_t data[4] = {0xAA, 0x55, uut.GetImplementedProtocolVersion(), 0};
    pci.LoadReceiveBytes(data, 4);

    // run 
    for (uint8_t i = 0; i < 4; ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser starts with a next expected ID of 0.
//
void test_ProtocolParserInitiallyExpectesAnIdOfZero(void) 
{
    // assert
    TEST_ASSERT_EQUAL(0, uut.GetNextExpectedId());
}

//
//  test protocol parser increments the expected ID after each message with a correct ID.
//
void test_ProtocolParserIncrementsTheExpectedIdAfterEachMessageWithACorrectId(void) 
{
    // 256 values from 0 to 255 and an extra 1 to roll back to 0
    for (uint16_t i = 0; i < 257; ++i)
    {
        const uint8_t THIS_ID = (uint8_t)(i % 256);
        const uint8_t NEXT_ID = (uint8_t)(THIS_ID + 1);

        // setup
        uint8_t data[8] = {0xAA, 0x55, uut.GetImplementedProtocolVersion(), 1, THIS_ID, 0x00, 0x00, 0x00};
        pci.LoadReceiveBytes(data, 8);

        // run 
        for (uint8_t j = 0; j < 8; ++j)
        {
            uut.ProcessEvent(NEvent::Event_DataRxFromPC);
        }

        // assert
        TEST_ASSERT_EQUAL(NEXT_ID, uut.GetNextExpectedId());
    }
}

//
//  test protocol parser does not increment the expected ID after a message with an incorrect ID.
//
void test_ProtocolParserDoesNoIncrementTheExpectedIdAfterAMessageWithAnIncorrectId(void) 
{
    // 25 values with the correct ID
    for (uint16_t i = 0; i < 25; ++i)
    {
        const uint8_t THIS_ID = (uint8_t)(i % 256);
        const uint8_t NEXT_ID = (uint8_t)(THIS_ID + 1);

        // setup
        uint8_t data[8] = {0xAA, 0x55, uut.GetImplementedProtocolVersion(), 1, THIS_ID, 0x00, 0x00, 0x00};
        pci.LoadReceiveBytes(data, 8);

        // run 
        for (uint8_t j = 0; j < 8; ++j)
        {
            uut.ProcessEvent(NEvent::Event_DataRxFromPC);
        }

        // assert
        TEST_ASSERT_EQUAL(NEXT_ID, uut.GetNextExpectedId());
    }

    // assert that the next expected ID is 25
    TEST_ASSERT_EQUAL(25, uut.GetNextExpectedId());

    // Send one more with an incorrect ID
    // setup
    uint8_t data[8] = {0xAA, 0x55, uut.GetImplementedProtocolVersion(), 1, 24, 0x00, 0x00, 0x00};
    pci.LoadReceiveBytes(data, 8);

    // run 
    for (uint8_t j = 0; j < 8; ++j)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert that the next expected ID is still 25
    TEST_ASSERT_EQUAL(25, uut.GetNextExpectedId());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolParserDefaultsToWaitingForSync);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync1);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync2);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync3);
    RUN_TEST(test_ProtocolParserGoesToGetProtocolVersionWithValidSync);
    RUN_TEST(test_ProtocolParserReturnsToWaitingForSyncWithInvalidProtocolVersion);
    RUN_TEST(test_ProtocolParserGoesToGetPayloadLengthWithValidProtocolVersion);
    RUN_TEST(test_ProtocolParserGoesToWaitForSyncIfThePayloadLengthIsTooLarge);
    RUN_TEST(test_ProtocolParserGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge);
    RUN_TEST(test_ProtocolParserGoesToGetAckIdAfterTheGetIdState);
    RUN_TEST(test_ProtocolParserGoesToGetChecksumAfterTheAckIdState);
    RUN_TEST(test_ProtocolParserGoesToGetPayloadStateAfterTheGetChecksumState);
    RUN_TEST(test_ProtocolParserRemainsInGetPayloadStateBeforeAllPayloadBytesAreReceived);
    RUN_TEST(test_ProtocolParserReturnsToWaitingForSyncAfterAllPayloadBytesAreReceived);
    RUN_TEST(test_ProtocolParserReturnsToWaitingForSyncSequenceIfThePayloadIsZeroLength);
    RUN_TEST(test_ProtocolParserInitiallyExpectesAnIdOfZero);
    RUN_TEST(test_ProtocolParserIncrementsTheExpectedIdAfterEachMessageWithACorrectId);
    RUN_TEST(test_ProtocolParserDoesNoIncrementTheExpectedIdAfterAMessageWithAnIncorrectId);
    UNITY_END();

    return 0;
}
