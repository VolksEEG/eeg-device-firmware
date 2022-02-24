
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

        typedef struct _EMG_DATA
        {
            uint32_t status;
            int32_t channel1;
            int32_t channel2;
            int32_t channel3;
            int32_t channel4;
            int32_t channel5;
            int32_t channel6;
            int32_t channel7;
            int32_t channel8;
        }sEMGData;

        typedef enum _REFERENCE_SELECT
        {
            Internal,
            External
        }eReferenceSource;

        Ads1299LowDriver();
        Ads1299LowDriver(SpiDriver * spi, PinControl * pins);

        void ResetDevice(void);
        void StartContinuousDataCapture(eSampleRate rate);
        void StopContinuousDataCapture(void);
        uint8_t GetNumberOfSupportedChannels(void);
        void SetChannelState(eChannelId chan, eChannelState state);
        void SetChannelGain(eChannelId chan, eChannelGain gain);
        void SetReferenceSource(eReferenceSource src);
        void ReadBackRegisters(void);       // TODO - remove debugging function.
        void SetTestSignal(void);
        sEMGData GetEMGData(void);

    protected:

    private:

        // instance of the SPI Scheduler and Pin Control.
        SpiDriver * _SpiDriverInstance;
        PinControl * _PinControlInstance;

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
            static const uint8_t ID_NUMBER_OF_CHANNELS_MASK = 0x03;
            static const uint8_t ID_NUMBER_OF_CHANNELS_4 = 0x00;
            static const uint8_t ID_NUMBER_OF_CHANNELS_6 = 0x01;
            static const uint8_t ID_NUMBER_OF_CHANNELS_8 = 0x02;
        static const uint8_t REGISTER_CONFIG_1 = 0x01;
            static const uint8_t CONFIG_1_DATARATE_MASK = 0x07;
            static const uint8_t CONFIG_1_DATARATE_16KSPS = 0x00;
            static const uint8_t CONFIG_1_DATARATE_8KSPS = 0x01;
            static const uint8_t CONFIG_1_DATARATE_4KSPS = 0x02;
            static const uint8_t CONFIG_1_DATARATE_2KSPS = 0x03;
            static const uint8_t CONFIG_1_DATARATE_1KSPS = 0x04;
            static const uint8_t CONFIG_1_DATARATE_500SPS = 0x05;
            static const uint8_t CONFIG_1_DATARATE_250SPS = 0x06;
        static const uint8_t REGISTER_CONFIG_2 = 0x02;
            static const uint8_t CONFIG_2_INTERNAL_CAL_MASK = 0x10;
            static const uint8_t CONFIG_2_INTERNAL_CAL_ENABLE = 0x10;
            static const uint8_t CONFIG_2_INTERNAL_CAL_DISABLE = 0x00;
            static const uint8_t CONFIG_2_CAL_AMPLITUDE_MASK = 0x04;
            static const uint8_t CONFIG_2_CAL_AMPLITUDE_LOW = 0x00;
            static const uint8_t CONFIG_2_CAL_AMPLITUDE_HIGH = 0x04;
            static const uint8_t CONFIG_2_CAL_FREQUENCY_MASK = 0x03;
            static const uint8_t CONFIG_2_CAL_FREQUENCY_DC = 0x03;
        static const uint8_t REGISTER_CONFIG_3 = 0x03;
            static const uint8_t CONFIG_3_REF_BUFFER_MASK = 0x80;
            static const uint8_t CONFIG_3_REF_BUFFER_DISABLE = 0x00;
            static const uint8_t CONFIG_3_REF_BUFFER_ENABLE = 0x80;
        static const uint8_t REGISTER_CH1SET = 0x05;
        static const uint8_t REGISTER_CH2SET = 0x06;
        static const uint8_t REGISTER_CH3SET = 0x07;
        static const uint8_t REGISTER_CH4SET = 0x08;
        static const uint8_t REGISTER_CH5SET = 0x09;
        static const uint8_t REGISTER_CH6SET = 0x0A;
        static const uint8_t REGISTER_CH7SET = 0x0B;
        static const uint8_t REGISTER_CH8SET = 0x0C;
            static const uint8_t CHNSET_GAIN_MASK = 0x70;
            static const uint8_t CHNSET_GAIN_1 = 0x00;
            static const uint8_t CHNSET_GAIN_2 = 0x10;
            static const uint8_t CHNSET_GAIN_4 = 0x20;
            static const uint8_t CHNSET_GAIN_6 = 0x30;
            static const uint8_t CHNSET_GAIN_8 = 0x40;
            static const uint8_t CHNSET_GAIN_12 = 0x50;
            static const uint8_t CHNSET_GAIN_24 = 0x60;
            static const uint8_t CHNSET_STATE_MASK = 0x87;
            static const uint8_t CHNSET_STATE_OFF_SHORTED = 0x81;
            static const uint8_t CHNSET_STATE_ON_NORMAL = 0x00;
            static const uint8_t CHNSET_STATE_ON_SHORTED = 0x01;
            static const uint8_t CHNSET_STATE_ON_MEASURING_SUPPLIES = 0x03;
            static const uint8_t CHNSET_STATE_ON_MEASURING_TEST_SIGNAL = 0x05;

        static const uint8_t BLANK_DATA = 0x00;
        
        // private member functions 
        uint8_t GetChannelRegisterFromChannelIdEnum(eChannelId chan);
        uint8_t GetGainFromFromChannelGainEnum(eChannelGain gain);
        uint8_t GetSampleRateValueFromSampleRateEnum(eSampleRate rate);
        uint8_t GetChannelStateValueFromChannelStateEnum(eChannelState state);
        uint8_t ReadRegister(uint8_t reg);
        void WriteRegister(uint8_t reg, uint8_t newValue);
        void ModifyRegister(uint8_t reg, uint8_t mask, uint8_t newValue);

};

#endif
