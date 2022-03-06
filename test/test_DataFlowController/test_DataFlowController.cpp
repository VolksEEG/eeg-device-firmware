
#include <DataFlowController.h>
#include <unity.h>

#include "ProducerMock.h"
#include "ConsumerMock.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const NEvent::eEvent PrimaryProducerEvent = NEvent::eEvent::Event_ADS1299DataReady;
static const NEvent::eEvent SecondaryProducerEvent = NEvent::eEvent::Event_EDFDataReady;

//
//  Test that the primary producer is called by default
//
void test_DataFlowController_PrimaryProducerIsCalledByDefault(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(PrimaryProducerEvent);

    // assert that only the primary producer was called.
    TEST_ASSERT_EQUAL(0, secondaryProducer.GetTimesCalled());
    TEST_ASSERT_EQUAL(1, primaryProducer.GetTimesCalled());
}

//
//  Test that switching to secondary works.
//
void test_DataFlowController_SecondaryProducerIsCalledWhenProducerIsChanged(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // change the producer.
    uut.SetProducer(DataFlowController::eProducerConsumer::secondary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(SecondaryProducerEvent);

    // assert that only the primary producer was called.
    TEST_ASSERT_EQUAL(1, secondaryProducer.GetTimesCalled());
    TEST_ASSERT_EQUAL(0, primaryProducer.GetTimesCalled());
}

//
//  Tests that switching to secondary and back to primary works
//
void test_DataFlowController_PrimaryProducerIsCalledWhenProducerIsChangedBack(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // change the producer.
    uut.SetProducer(DataFlowController::eProducerConsumer::secondary);

    // change the producer back.
    uut.SetProducer(DataFlowController::eProducerConsumer::primary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(PrimaryProducerEvent);

    // assert that only the primary producer was called.
    TEST_ASSERT_EQUAL(0, secondaryProducer.GetTimesCalled());
    TEST_ASSERT_EQUAL(1, primaryProducer.GetTimesCalled());
}

//
//  Test that the primary consumer is called by default
//
void test_DataFlowController_PrimaryConsumerIsCalledByDefault(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(PrimaryProducerEvent);

    // assert that only the primary producer was called.
    TEST_ASSERT_EQUAL(0, secondaryConsumer.GetTimesCalled());
    TEST_ASSERT_EQUAL(1, primaryConsumer.GetTimesCalled());
}

//
//  Test that switching to secondary works.
//
void test_DataFlowController_SecondaryConsumerIsCalledWhenProducerIsChanged(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // change the consumer.
    uut.SetConsumer(DataFlowController::eProducerConsumer::secondary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(PrimaryProducerEvent);

    // assert that only the primary producer was called.
    TEST_ASSERT_EQUAL(1, secondaryConsumer.GetTimesCalled());
    TEST_ASSERT_EQUAL(0, primaryConsumer.GetTimesCalled());
}

//
//  Tests that switching to secondary and back to primary works
//
void test_DataFlowController_PrimaryConsumerIsCalledWhenProducerIsChangedBack(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // change the consumer.
    uut.SetConsumer(DataFlowController::eProducerConsumer::secondary);

    // change the consumer back.
    uut.SetConsumer(DataFlowController::eProducerConsumer::primary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(PrimaryProducerEvent);

    // assert that only the primary producer was called.
    TEST_ASSERT_EQUAL(0, secondaryConsumer.GetTimesCalled());
    TEST_ASSERT_EQUAL(1, primaryConsumer.GetTimesCalled());
}

//
//  Tests that the data is passed from producer to consumer
//
void test_DataFlowController_ProducerDataIsPassedToConsumer(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, PrimaryProducerEvent, &secondaryProducer, SecondaryProducerEvent, &primaryConsumer, &secondaryConsumer);

    // select a producer and consumer to use.
    ProducerMock * producerInstance;
    ConsumerMock * consumerInstance;
    NEvent::eEvent producerEvent;

    // initialize random seed
    srand (time(NULL));

    const int CONFIGURATION = rand() % 4;

    switch(CONFIGURATION)
    {
        case 0:
            // primary producer - primary consumer
            producerInstance = &primaryProducer;
            consumerInstance = &primaryConsumer;
            producerEvent = PrimaryProducerEvent;
            uut.SetProducer(DataFlowController::eProducerConsumer::primary);
            uut.SetConsumer(DataFlowController::eProducerConsumer::primary);
            break;
        case 1:
            // primary producer - secondary consumer
            producerInstance = &primaryProducer;
            consumerInstance = &secondaryConsumer;
            producerEvent = PrimaryProducerEvent;
            uut.SetProducer(DataFlowController::eProducerConsumer::primary);
            uut.SetConsumer(DataFlowController::eProducerConsumer::secondary);
            break;
        case 2:
            // secondary producer - primary consumer
            producerInstance = &secondaryProducer;
            consumerInstance = &primaryConsumer;
            producerEvent = SecondaryProducerEvent;
            uut.SetProducer(DataFlowController::eProducerConsumer::secondary);
            uut.SetConsumer(DataFlowController::eProducerConsumer::primary);
            break;
        case 3:
            // secondary producer - secondary consumer
            producerInstance = &secondaryProducer;
            consumerInstance = &secondaryConsumer;
            producerEvent = SecondaryProducerEvent;
            uut.SetProducer(DataFlowController::eProducerConsumer::secondary);
            uut.SetConsumer(DataFlowController::eProducerConsumer::secondary);
            break;
    }

    EegData::sEegSamples samples;
    samples.channel_1 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_2 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_3 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_4 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_5 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_6 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_7 = (int16_t)(rand() % 32767) - 16383;
    samples.channel_8 = (int16_t)(rand() % 32767) - 16383;

    // show output values and 

    producerInstance->SetOutputValues(samples);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent(producerEvent);

    // print the settings used incase we fail.
    char message[100];
    sprintf(message, "Cfg = %u S = %d, %d, %d, %d, %d, %d, %d, %d", 
                        CONFIGURATION, 
                        samples.channel_1,
                        samples.channel_2,
                        samples.channel_3,
                        samples.channel_4,
                        samples.channel_5,
                        samples.channel_6,
                        samples.channel_7,
                        samples.channel_8);

    TEST_MESSAGE(message);

    // assert that the producer and were called.
    TEST_ASSERT_EQUAL(1, producerInstance->GetTimesCalled());
    TEST_ASSERT_EQUAL(1, consumerInstance->GetTimesCalled());

    // and the values received by the consumer are as expected.
    const EegData::sEegSamples RESULT = consumerInstance->GetReceivedSamples();

    TEST_ASSERT_EQUAL(samples.channel_1, RESULT.channel_1);
    TEST_ASSERT_EQUAL(samples.channel_2, RESULT.channel_2);
    TEST_ASSERT_EQUAL(samples.channel_3, RESULT.channel_3);
    TEST_ASSERT_EQUAL(samples.channel_4, RESULT.channel_4);
    TEST_ASSERT_EQUAL(samples.channel_5, RESULT.channel_5);
    TEST_ASSERT_EQUAL(samples.channel_6, RESULT.channel_6);
    TEST_ASSERT_EQUAL(samples.channel_7, RESULT.channel_7);
    TEST_ASSERT_EQUAL(samples.channel_8, RESULT.channel_8);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_DataFlowController_PrimaryProducerIsCalledByDefault);
    RUN_TEST(test_DataFlowController_SecondaryProducerIsCalledWhenProducerIsChanged);
    RUN_TEST(test_DataFlowController_PrimaryProducerIsCalledWhenProducerIsChangedBack);
    RUN_TEST(test_DataFlowController_PrimaryConsumerIsCalledByDefault);
    RUN_TEST(test_DataFlowController_SecondaryConsumerIsCalledWhenProducerIsChanged);
    RUN_TEST(test_DataFlowController_PrimaryConsumerIsCalledWhenProducerIsChangedBack);
    RUN_TEST(test_DataFlowController_ProducerDataIsPassedToConsumer);
    UNITY_END();

    return 0;
}
