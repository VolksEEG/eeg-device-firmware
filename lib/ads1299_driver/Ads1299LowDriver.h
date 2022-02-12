
#ifndef _ADS1299_LOW_DRIVER
#define _ADS1299_LOW_DRIVER

#include <stdint.h>
#include <SpiDriver.h>
#include <PinControl.h>

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
        Ads1299LowDriver(SpiDriver& spi, PinControl& pins);

        void ResetDevice(void);
        void StartContinuousDataCapture(eSampleRate rate);
        void StopContinuousDataCapture(void);
        uint8_t GetNumberOfSupportedChannels(void);
        void SetChannelState(eChannelId chan, eChannelState state);
        void SetChannelGain(eChannelId chan, eChannelGain gain);

    protected:

    private:

        // instance of the SPI Scheduler.
        SpiDriver _SpiDriver;
        PinControl _PinControl;

        // Command Definitions
        static const uint8_t COMMAND_WAKEUP = 0x02;
        static const uint8_t COMMAND_STANDBY = 0x04;
        static const uint8_t COMMAND_RESET = 0x06;
        static const uint8_t COMMAND_START = 0x08;
        static const uint8_t COMMAND_STOP = 0x0A;
        static const uint8_t COMMAND_READ_DATA_CONTINUOUSLY = 0x10;
        static const uint8_t COMMAND_STOP_READING_DATA_CONTINUOUSLY = 0x11;
        static const uint8_t COMMAND_READ_DATA = 0x12;
        static const uint8_t COMMAND_READ_REGISTER = 0x20;
        static const uint8_t COMMAND_WRITE_REGISTER = 0x40;

        // Register definitions
        static const uint8_t REGISTER_ID = 0x00;
            static const uint8_t ID_NUMBER_OF_CHANNELS_4 = 0x00;
            static const uint8_t ID_NUMBER_OF_CHANNELS_6 = 0x01;
            static const uint8_t ID_NUMBER_OF_CHANNELS_8 = 0x02;
        
};

#endif
