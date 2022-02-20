
#include <DataFlowController.h>
#include <unity.h>

#include "ProducerMock.h"
#include "ConsumerMock.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
//  Test that the primary producer is called by default
//
void test_DataFlowController_PrimaryProducerIsCalledByDefault(void) 
{
    ProducerMock primaryProducer = ProducerMock();
    ProducerMock secondaryProducer = ProducerMock();
    ConsumerMock primaryConsumer = ConsumerMock();
    ConsumerMock secondaryConsumer = ConsumerMock();

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

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

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // change the producer.
    uut.SetProducer(DataFlowController::eProducerConsumer::secondary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

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

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // change the producer.
    uut.SetProducer(DataFlowController::eProducerConsumer::secondary);

    // change the producer back.
    uut.SetProducer(DataFlowController::eProducerConsumer::primary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

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

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

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

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // change the consumer.
    uut.SetConsumer(DataFlowController::eProducerConsumer::secondary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

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

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // change the consumer.
    uut.SetConsumer(DataFlowController::eProducerConsumer::secondary);

    // change the consumer back.
    uut.SetConsumer(DataFlowController::eProducerConsumer::primary);

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

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

    DataFlowController uut = DataFlowController(&primaryProducer, &secondaryProducer, &primaryConsumer, &secondaryConsumer);

    // select a producer and consumer to use.
    ProducerMock * producerInstance;
    ConsumerMock * ConsumerMock;

    // initialize random seed
    srand (time(NULL));

    switch(rand() % 4)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
    }

    // process event to make Data Flow Controller call a producer.
    uut.ProcessEvent();

    // assert that only the primary producer was called.
    // TODO ASSERT
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_DataFlowController_PrimaryProducerIsCalledByDefault);
    RUN_TEST(test_DataFlowController_SecondaryProducerIsCalledWhenProducerIsChanged);
    RUN_TEST(test_DataFlowController_PrimaryProducerIsCalledWhenProducerIsChangedBack);
    RUN_TEST(test_DataFlowController_PrimaryConsumerIsCalledByDefault);
    RUN_TEST(test_DataFlowController_SecondaryConsumerIsCalledWhenProducerIsChanged);
    RUN_TEST(test_DataFlowController_PrimaryConsumerIsCalledWhenProducerIsChangedBack);
    UNITY_END();

    return 0;
}
