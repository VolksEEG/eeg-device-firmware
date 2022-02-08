
#include <Ads1299Driver.h>
#include <unity.h>

void test_Ads1299Driver_DeviceIsResetOnInitialisation(void)
{
    Ads1299Driver uut = Ads1299Driver();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_Ads1299Driver_DeviceIsResetOnInitialisation);
    UNITY_END();

    return 0;
}
