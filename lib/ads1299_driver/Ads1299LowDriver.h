
#ifndef _ADS1299_LOW_DRIVER
#define _ADS1299_LOW_DRIVER

#include <stdint.h>

class Ads1299LowDriver {

    public:

        typedef enum _SAMPLE_RATES
        {
            SPS_250,
            SPS_500,
            SPS_1000,
            SPS_2000,
            SPS_4000,
            SPS_8000,
            SPS_16000
        }eSampleRate;

        typedef enum _CHANNEL_IDS
        {
            CH1,
            CH2,
            CH3,
            CH4,
            CH5,
            CH6,
            CH7,
            CH8
        }eChannelId;

        // leaving out the Bias measuring functionality for now.
        typedef enum _CHANNEL_STATES
        {
            Off,
            OffAndShorted,
            OnNormal,
            OnAndShorted,
            OnAndMeasuringSupplies,
            OnAndMeasuringTestSignal
        }eChannelState;

        typedef enum _CHANNEL_GAINS
        {
            X1,
            X2,
            X4,
            X6,
            X8,
            X12,
            X24
        }eChannelGain;

        Ads1299LowDriver();

        void ResetDevice(void);
        void StartContinuousDataCapture(eSampleRate rate);
        void StopContinuousDataCapture(void);
        uint8_t GetNumberOfSupportedChannels(void);
        void SetChannelState(eChannelId chan, eChannelState state);
        void SetChannelGain(eChannelId chan, eChannelGain gain);

    protected:

    private:

        // instance of the SPI Scheduler.

};

#endif
