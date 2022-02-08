
#include <Ads1299Driver.h>
#include <unity.h>

void test_Ads1299Driver_DeviceIsResetAndNumberOfChannelsIsRequestedOnInitialisation(void)
{
    // Given 
    // uut is not instantiated

    // When
    // uut is instantiated

    // then 
    // the following functions will be called in the low level driver

    // Reset Device
    // Get Number Of Channels

    TEST_IGNORE_MESSAGE("Ignoring this test until the low level driver is implemented");
}

void test_Ads1299Driver_SetActiveChannelsAboveMaximumIsIgnored(void)
{
    // Given 
    // The returned number of channels is 4 or 6.

    // when 
    // The user attempts to set a channel above this value.

    // Then
    // Only the lower 4 or 6 channels are passed to the low level driver.

    TEST_IGNORE_MESSAGE("Ignoring this test until the low level driver is implemented");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_Ads1299Driver_DeviceIsResetAndNumberOfChannelsIsRequestedOnInitialisation);
    RUN_TEST(test_Ads1299Driver_SetActiveChannelsAboveMaximumIsIgnored);
    UNITY_END();

    return 0;
}
