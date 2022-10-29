
#include <ProtocolReceiver.h>
#include <IEEGDataProducer.h>

#include "../Mocks/MockEventHandler.h"
#include "../Mocks/MockPcCommunicationsInterface.h"
#include "../Mocks/MockIProtocolTransmission.h"

#include <unity.h>

class MockEegDataProducer : public IEegDataProducer {
    public:

        void StartProducingData() override {

        }

        void StopProducingData() override {

        }

    private:

};

MockPcCommunicationsInterface pci;
MockEegDataProducer edp;
MockEvenHandler evh;
MockIProtocolTransmission pti;
ProtocolReceiver uut;

void setUp(void) {
    pci = MockPcCommunicationsInterface();
    edp = MockEegDataProducer();
    evh = MockEvenHandler();
    pti = MockIProtocolTransmission();

    uut = ProtocolReceiver(&pci, &edp, &evh, &pti);
}

void tearDown(void) {
}

//
//  test protocol parser starts in the waiting for sync state
//
void test_ProtocolReceiverDefaultsToWaitingForSync(void) 
{
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser stays in the waiting for sync state for invalid sync patterns
//
void test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync1(void) 
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
void test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync2(void) 
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
void test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync3(void) 
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
void test_ProtocolReceiverGoesToGetProtocolVersionWithValidSync(void) 
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
void test_ProtocolReceiverReturnsToWaitingForSyncWithInvalidProtocolVersion(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetProtocolVersionWithValidSync();

    uint8_t data[1] = {(uint8_t)~ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the Get Payload Length state if the protocol version is the version implemented
//
void test_ProtocolReceiverGoesToGetPayloadLengthWithValidProtocolVersion(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetProtocolVersionWithValidSync();

    uint8_t data[1] = {ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayloadLength, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the Wait for sync state if the payload version is too large
//
void test_ProtocolReceiverGoesToWaitForSyncIfThePayloadLengthIsTooLarge(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetPayloadLengthWithValidProtocolVersion();

    uint8_t data[1] = {(uint8_t)(ProtocolGeneral::_MAX_PAYLOAD_SIZE + 1)};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get Id state if the payload version is not too large
//
void test_ProtocolReceiverGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetPayloadLengthWithValidProtocolVersion();

    uint8_t data[1] = {ProtocolGeneral::_MAX_PAYLOAD_SIZE};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetIdNumber, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the ack Id state after get id state
//
void test_ProtocolReceiverGoesToGetAckIdAfterTheGetIdState(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge();

    uint8_t data[1] = {0x00};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetAckId, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get payload checksum after the ack Id state
//
void test_ProtocolReceiverGoesToGetPayloadChecksumAfterTheAckIdState(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetAckIdAfterTheGetIdState();

    uint8_t data[1] = {0x00};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayloadChecksum, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get header checksum state after the get payload checksum state
//
void test_ProtocolReceiverGoesToGetHeaderChecksumStateAfterTheGetPayloadChecksumState(void) 
{
    // setup
    test_ProtocolReceiverGoesToGetPayloadChecksumAfterTheAckIdState();

    uint8_t data[1] = {0x00};
    pci.LoadReceiveBytes(data, 1);

    // run 
    uut.ProcessEvent(NEvent::Event_DataRxFromPC);

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetHeaderChecksum, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get payload state after the get header checksum state if checksum is correct
//
void test_ProtocolReceiverGoesToGetPayloadStateAfterTheGetHeaderChecksumStateIfHeaderChecksumIsCorrect(void) 
{
    const uint8_t TEST_PAYLOAD_LENGTH = 1;
    const uint8_t TEST_MESSAGE_LENGTH = ProtocolGeneral::_HEADER_SIZE;  // payload not included, as this test is checking state before payload is received
    
    uint8_t message[TEST_MESSAGE_LENGTH] = {
        ProtocolGeneral::_SYNC_WORD_MSB, 
        ProtocolGeneral::_SYNC_WORD_LSB, 
        ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
        TEST_PAYLOAD_LENGTH,    // Payload Length
        0x00,                   // ID Number
        0x00,                   // ID Ack
        0x00,                   // Payload Checksum
        0x00                   // Header Checksum
        };

    // set the checksum to the correct value
    message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessageHeader(message);

    pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

    // run 
    for (int i = 0; i < TEST_MESSAGE_LENGTH; ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayload, uut.GetCurrentRxState());
}

//
//  test protocol parser goes to the get wait for sync state after the get header checksum state if header checksum is incorrect
//
void test_ProtocolReceiverGoesToTheWaitForSyncStateAfterTheGetHeaderChecksumStateIfHeaderChecksumIsNotCorrect(void) 
{
    const uint8_t TEST_PAYLOAD_LENGTH = 1;
    const uint8_t TEST_MESSAGE_LENGTH = ProtocolGeneral::_HEADER_SIZE + TEST_PAYLOAD_LENGTH;
    
    uint8_t message[TEST_MESSAGE_LENGTH] = {
        ProtocolGeneral::_SYNC_WORD_MSB, 
        ProtocolGeneral::_SYNC_WORD_LSB, 
        ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
        TEST_PAYLOAD_LENGTH,    // Payload Length
        0x00,                   // ID Number
        0x00,                   // ID Ack
        0x00,                   // Payload Checksum
        0x00                   // Header Checksum
        };

    // set the checksum to an incorrect value
    message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ~ProtocolGeneral::CalculateChecksumOfMessageHeader(message);

    pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

    // run 
    for (int i = 0; i < TEST_MESSAGE_LENGTH; ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser stays in the get payload state while all bytes are not yet received.
//
void test_ProtocolReceiverRemainsInGetPayloadStateBeforeAllPayloadBytesAreReceived(void) 
{
    // setup
    const uint8_t TEST_PAYLOAD_LENGTH = 10;
    const uint8_t TEST_MESSAGE_LENGTH = ProtocolGeneral::_HEADER_SIZE + TEST_PAYLOAD_LENGTH;
    
    uint8_t message[TEST_MESSAGE_LENGTH] = {
        ProtocolGeneral::_SYNC_WORD_MSB, 
        ProtocolGeneral::_SYNC_WORD_LSB, 
        ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
        TEST_PAYLOAD_LENGTH,    // Payload Length
        0x00,                   // ID Number
        0x00,                   // ID Ack
        0x00,                   // Payload Checksum
        0x00,                   // Header Checksum
        0x01,                   // Start of payload
        0x02, 
        0x03, 
        0x04, 
        0x05, 
        0x06, 
        0x07, 
        0x08, 
        0x09};

    // set the checksums to the correct values
    message[ProtocolGeneral::_PAYLOAD_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessagePayload(message);
    message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessageHeader(message);

    pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

    // run 

    // process all but 1 byte
    for (int i = 0; i < (TEST_MESSAGE_LENGTH - 1); ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::GetPayload, uut.GetCurrentRxState());
}

//
//  test protocol parser returns to looking for the next packet when all bytes are received.
//
void test_ProtocolReceiverReturnsToWaitingForSyncAfterAllPayloadBytesAreReceived(void) 
{
    // setup
    const uint8_t TEST_PAYLOAD_LENGTH = 10;
    const uint8_t TEST_MESSAGE_LENGTH = ProtocolGeneral::_HEADER_SIZE + TEST_PAYLOAD_LENGTH;
    
    uint8_t message[TEST_MESSAGE_LENGTH] = {
        ProtocolGeneral::_SYNC_WORD_MSB, 
        ProtocolGeneral::_SYNC_WORD_LSB, 
        ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
        TEST_PAYLOAD_LENGTH,    // Payload Length
        0x00,                   // ID Number
        0x00,                   // ID Ack
        0x00,                   // Payload Checksum
        0x00,                   // Header Checksum
        0x01,                   // Start of payload
        0x02, 
        0x03, 
        0x04, 
        0x05, 
        0x06, 
        0x07, 
        0x08, 
        0x09};

    // set the checksums to the correct values
    message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessageHeader(message);
    message[ProtocolGeneral::_PAYLOAD_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessagePayload(message);

    pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

    // run 

    // process all but 1 byte
    for (int i = 0; i < TEST_MESSAGE_LENGTH; ++i)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert
    TEST_ASSERT_EQUAL(uut.RX_STATE::WaitForSequence, uut.GetCurrentRxState());
}

//
//  test protocol parser returns to waiting for Sync Sequence if the payload is zero length.
//
void test_ProtocolReceiverReturnsToWaitingForSyncSequenceIfThePayloadIsZeroLength(void) 
{   
    // setup
    uint8_t data[4] = {
        ProtocolGeneral::_SYNC_WORD_MSB, 
        ProtocolGeneral::_SYNC_WORD_LSB, 
        ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
        0};

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
void test_ProtocolReceiverInitiallyExpectesAnIdOfZero(void) 
{
    // assert
    TEST_ASSERT_EQUAL(0, uut.GetNextExpectedId());
}

//
//  test protocol parser increments the expected ID after each message with a correct ID.
//
void test_ProtocolReceiverIncrementsTheExpectedIdAfterEachMessageWithACorrectId(void) 
{
    const uint8_t TEST_PAYLOAD_LENGTH = 1;
    const uint8_t TEST_MESSAGE_LENGTH = ProtocolGeneral::_HEADER_SIZE + TEST_PAYLOAD_LENGTH;
    
    // 256 values from 0 to 255 and an extra 1 to roll back to 0
    for (uint16_t i = 0; i < 257; ++i)
    {
        const uint8_t THIS_ID = (uint8_t)(i % 256);
        const uint8_t NEXT_ID = (uint8_t)(THIS_ID + 1);

        // setup
        uint8_t message[TEST_MESSAGE_LENGTH] = {
            ProtocolGeneral::_SYNC_WORD_MSB, 
            ProtocolGeneral::_SYNC_WORD_LSB, 
            ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
            TEST_PAYLOAD_LENGTH,    // Payload Length
            THIS_ID,                // ID Number
            0x00,                   // ID Ack
            0x00,                   // Payload Checksum
            0x00,                   // Header Checksum
            0x00};      // Dummy Payload

        // set the checksums to the correct values
        message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessageHeader(message);
        message[ProtocolGeneral::_PAYLOAD_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessagePayload(message);

        pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

        // run 
        for (uint8_t j = 0; j < TEST_MESSAGE_LENGTH; ++j)
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
void test_ProtocolReceiverDoesNoIncrementTheExpectedIdAfterAMessageWithAnIncorrectId(void) 
{
    const uint8_t TEST_PAYLOAD_LENGTH = 1;
    const uint8_t TEST_MESSAGE_LENGTH = ProtocolGeneral::_HEADER_SIZE + TEST_PAYLOAD_LENGTH;
    
    const uint8_t TEST_ID = 25;

    // Add messages with the correct ID up to TEST_ID value
    for (uint8_t i = 0; i < TEST_ID; ++i)
    {
        const uint8_t THIS_ID = (uint8_t)(i % 256);
        const uint8_t NEXT_ID = (uint8_t)(THIS_ID + 1);

        // setup
        uint8_t message[TEST_MESSAGE_LENGTH] = {
            ProtocolGeneral::_SYNC_WORD_MSB, 
            ProtocolGeneral::_SYNC_WORD_LSB, 
            ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
            TEST_PAYLOAD_LENGTH,    // Payload Length
            THIS_ID,                // ID Number
            0x00,                   // ID Ack
            0x00,                   // Payload Checksum
            0x00,                   // Header Checksum
            0x00};      // Dummy Payload

        // set the checksums to the correct values
        message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessageHeader(message);
        message[ProtocolGeneral::_PAYLOAD_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessagePayload(message);

        pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

        // run 
        for (uint8_t j = 0; j < TEST_MESSAGE_LENGTH; ++j)
        {
            uut.ProcessEvent(NEvent::Event_DataRxFromPC);
        }

        // assert
        TEST_ASSERT_EQUAL(NEXT_ID, uut.GetNextExpectedId());
    }

    // assert that the next expected ID is TEST_ID
    TEST_ASSERT_EQUAL(TEST_ID, uut.GetNextExpectedId());

    // Send one more with an incorrect ID

    // setup
    uint8_t message[TEST_MESSAGE_LENGTH] = {
        ProtocolGeneral::_SYNC_WORD_MSB, 
        ProtocolGeneral::_SYNC_WORD_LSB, 
        ProtocolGeneral::_IMPLEMENTED_PROTOCOL_VERSION, 
        TEST_PAYLOAD_LENGTH,
        (TEST_ID - 1),         // Next ID should be TEST_ID, therefore (TEST_ID - 1) would be incorrect
        0x00, 
        0x00, 
        0x00};      // Dummy Payload

    // set the checksums to the correct values
    message[ProtocolGeneral::_HEADER_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessageHeader(message);
    message[ProtocolGeneral::_PAYLOAD_CHECKSUM_INDEX] = ProtocolGeneral::CalculateChecksumOfMessagePayload(message);

    pci.LoadReceiveBytes(message, TEST_MESSAGE_LENGTH);

    // run 
    for (uint8_t j = 0; j < TEST_MESSAGE_LENGTH; ++j)
    {
        uut.ProcessEvent(NEvent::Event_DataRxFromPC);
    }

    // assert that the next expected ID is still TEST_ID
    TEST_ASSERT_EQUAL(TEST_ID, uut.GetNextExpectedId());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolReceiverDefaultsToWaitingForSync);
    RUN_TEST(test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync1);
    RUN_TEST(test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync2);
    RUN_TEST(test_ProtocolReceiverRemainsWaitingForSyncWithInvalidSync3);
    RUN_TEST(test_ProtocolReceiverGoesToGetProtocolVersionWithValidSync);
    RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncWithInvalidProtocolVersion);
    RUN_TEST(test_ProtocolReceiverGoesToGetPayloadLengthWithValidProtocolVersion);
    RUN_TEST(test_ProtocolReceiverGoesToWaitForSyncIfThePayloadLengthIsTooLarge);
    RUN_TEST(test_ProtocolReceiverGoesToGetIdNumberIfThePayloadLengthIsNoTooLarge);
    RUN_TEST(test_ProtocolReceiverGoesToGetAckIdAfterTheGetIdState);
    RUN_TEST(test_ProtocolReceiverGoesToGetPayloadChecksumAfterTheAckIdState);
    RUN_TEST(test_ProtocolReceiverGoesToGetHeaderChecksumStateAfterTheGetPayloadChecksumState);
    RUN_TEST(test_ProtocolReceiverGoesToGetPayloadStateAfterTheGetHeaderChecksumStateIfHeaderChecksumIsCorrect);
    RUN_TEST(test_ProtocolReceiverGoesToTheWaitForSyncStateAfterTheGetHeaderChecksumStateIfHeaderChecksumIsNotCorrect);
    RUN_TEST(test_ProtocolReceiverRemainsInGetPayloadStateBeforeAllPayloadBytesAreReceived);
    RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncAfterAllPayloadBytesAreReceived);
    RUN_TEST(test_ProtocolReceiverReturnsToWaitingForSyncSequenceIfThePayloadIsZeroLength);
    RUN_TEST(test_ProtocolReceiverInitiallyExpectesAnIdOfZero);
    RUN_TEST(test_ProtocolReceiverIncrementsTheExpectedIdAfterEachMessageWithACorrectId);
    RUN_TEST(test_ProtocolReceiverDoesNoIncrementTheExpectedIdAfterAMessageWithAnIncorrectId);
    UNITY_END();

    return 0;
}
