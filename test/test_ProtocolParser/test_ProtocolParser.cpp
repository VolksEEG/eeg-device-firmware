
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

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ProtocolParserDefaultsToWaitingForSync);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync1);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync2);
    RUN_TEST(test_ProtocolParserRemainsWaitingForSyncWithInvalidSync3);
    RUN_TEST(test_ProtocolParserGoesToGetProtocolVersionWithValidSync);
    RUN_TEST(test_ProtocolParserReturnsToWaitingForSyncWithInvalidProtocolVersion);
    RUN_TEST(test_ProtocolParserGoesToGetPayloadLengthWithValidProtocolVersion);
    UNITY_END();

    return 0;
}
