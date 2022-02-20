
#include <DataFlowController.h>
#include <unity.h>

#include "ProducerMock.h"
#include "ConsumerMock.h"

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

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_DataFlowController_PrimaryProducerIsCalledByDefault);
    RUN_TEST(test_DataFlowController_SecondaryProducerIsCalledWhenProducerIsChanged);
    RUN_TEST(test_DataFlowController_PrimaryProducerIsCalledWhenProducerIsChangedBack);
    UNITY_END();

    return 0;
}
