
#include <ErrorHandler.h>
#include <unity.h>

void setUp(void) {
}

void tearDown(void) {
}

//
//  Tests that initially no errors are raised
//
void test_ErrorHandler_InitialyNoErrorsAreRaised(void) {

    ErrorHandler eh = ErrorHandler();

    TEST_ASSERT_FALSE(eh.IsErrorPresent());
}

//
//  Tests that initially no errors are raised
//
void test_ErrorHandler_WhenAnErrorIsRaisedItIsReported(void) {

    ErrorHandler eh = ErrorHandler();

    eh.RaiseError(ErrorHandler::eError::Error_EventOverrun);

    TEST_ASSERT_TRUE(eh.IsErrorPresent());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_ErrorHandler_InitialyNoErrorsAreRaised);
    RUN_TEST(test_ErrorHandler_WhenAnErrorIsRaisedItIsReported);
    UNITY_END();

    return 0;
}
