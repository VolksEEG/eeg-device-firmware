
#ifndef _ADS1299_LOW_DRIVER
#define _ADS1299_LOW_DRIVER

#include <stdint.h>
#include <SpiDriver.h>
#include <PinControl.h>

class Ads1299LowDriver {

    public:

        static const uint8_t MAX_CHANNELS = 8u;

        typedef enum _SAMPLE_RATES
        {
            SPS_250 = 0x06,
            SPS_500 = 0x05,
            SPS_1000 = 0x04,
            SPS_2000 = 0x03,
            SPS_4000 = 0x02,
            SPS_8000 = 0x01,
            SPS_16000 = 0x00
        }eSampleRate;

        typedef enum _CHANNEL_IDS
        {
            CH1 = 0,
            CH2 = 1,
            CH3 = 2,
            CH4 = 3,
            CH5 = 4,
            CH6 = 5,
            CH7 = 6,
            CH8 = 7
        }eChannelId;

        // leaving out the Bias measuring functionality for now.
        typedef enum _CHANNEL_STATES
        {
            OffAndShorted = 0x81,
            OnNormal = 0x00,
            OnAndShorted = 0x01,
            OnAndMeasuringSupplies = 0x03,
            OnAndMeasuringTestSignal = 0x05
        }eChannelState;

        typedef enum _CHANNEL_GAINS
        {
            X1 = 0x00,
            X2 = 0x10,
            X4 = 0x20,
            X6 = 0x30,
            X8 = 0x40,
            X12 = 0x50,
            X24 = 0x60
        }eChannelGain;
        
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
        void SetTestSignal(void);
        void SetNegativeInputsConnectionToSRB1(bool connect);

        typedef struct _ADS1299_CHANNEL_DATA
        {
            int32_t gain;
            int32_t count;
        }sAds1299ChannelData;

        typedef struct _ADS1299_ALL_CHANNELS_DATA
        {
            Ads1299LowDriver::sAds1299ChannelData channels[MAX_CHANNELS];
            float vrefVoltage;
        }sAds1299AllChannelsData;

        Ads1299LowDriver::sAds1299AllChannelsData GetAds1299Data(void);

    protected:

    private:

        // Command Definitions
        typedef enum _COMMANDS
        {
            COMMAND_WAKEUP = 0x02,
            COMMAND_STANDBY = 0x04,
            COMMAND_RESET = 0x06,
            COMMAND_START = 0x08,
            COMMAND_STOP = 0x0A,
            COMMAND_READ_DATA_CONTINUOUSLY = 0x10,
            COMMAND_STOP_READING_DATA_CONTINUOUSLY = 0x11,
            COMMAND_READ_DATA = 0x12,
            COMMAND_READ_REGISTER = 0x20,
            COMMAND_WRITE_REGISTER = 0x40
        }eCommands;

        // Register definitions
        typedef enum _REGISTERS
        {
            REGISTER_ID = 0x00,
            REGISTER_CONFIG_1 = 0x01,
            REGISTER_CONFIG_2 = 0x02,
            REGISTER_CONFIG_3 = 0x03,
            REGISTER_CH1SET = 0x05,
            REGISTER_CH2SET = 0x06,
            REGISTER_CH3SET = 0x07,
            REGISTER_CH4SET = 0x08,
            REGISTER_CH5SET = 0x09,
            REGISTER_CH6SET = 0x0A,
            REGISTER_CH7SET = 0x0B,
            REGISTER_CH8SET = 0x0C,
            REGISTER_MISC1 = 0x15
        }eRegisters;

        // definitions for determining the number of channels from the ID register.
        static const uint8_t ID_NUMBER_OF_CHANNELS_MASK = 0x03;
        
        typedef enum _NUMBER_OF_CHANNELS
        {
            ID_NUMBER_OF_CHANNELS_4 = 0x00,
            ID_NUMBER_OF_CHANNELS_6 = 0x01,
            ID_NUMBER_OF_CHANNELS_8 = 0x02
        }eNumChannels;

        // definitions for setting sample rate
        static const uint8_t CONFIG_1_DATARATE_MASK = 0x07;

        // definitions for setting internal calibration signal
        static const uint8_t CONFIG_2_INTERNAL_CAL_MASK = 0x10;
        static const uint8_t CONFIG_2_CAL_AMPLITUDE_MASK = 0x04;
        static const uint8_t CONFIG_2_CAL_FREQUENCY_MASK = 0x03;

        typedef enum _CAL_SIGNAL_STATE
        {
            CONFIG_2_INTERNAL_CAL_DISABLE = 0x00,
            CONFIG_2_INTERNAL_CAL_ENABLE = 0x10
        }eCalSignalState;

        typedef enum _CAL_SIGNAL_AMPLITUDE
        {
            CONFIG_2_CAL_AMPLITUDE_LOW = 0x00,
            CONFIG_2_CAL_AMPLITUDE_HIGH = 0x04
        }eCalSignalAmplitude;

        typedef enum _CAL_SIGNAL_FREQUENCY
        {
            CONFIG_2_CAL_FREQUENCY_SLOW = 0x00,
            CONFIG_2_CAL_FREQUENCY_FAST = 0x01,
            CONFIG_2_CAL_FREQUENCY_DC = 0x03
        }eCalSignalFrequency;

        // definitions for setting the reference buffer.
        static const uint8_t CONFIG_3_REF_BUFFER_MASK = 0x80;

        typedef enum _REG_BUFFER_STATE
        {
            CONFIG_3_REF_BUFFER_DISABLE = 0x00,
            CONFIG_3_REF_BUFFER_ENABLE = 0x80
        }eRefBufferState;

        // definitions for channel settings
        static const uint8_t CHNSET_GAIN_MASK = 0x70;
        static const uint8_t CHNSET_STATE_MASK = 0x87;

        // definitions for srb1 connection setting
        static const uint8_t MISC_1_SRB1_MASK = 0x20;

        typedef enum _REG_BUFFER_STATE
        {
            MISC_1_SRB1_DISABLE = 0x00,
            MISC_1_SRB1_ENABLE = 0x20
        }eRefBufferState;


        // used to clock the SPI data out of the ADS1299
        static const uint8_t BLANK_DATA = 0x00;
        

        static const int32_t DEFAULT_CHANNEL_GAIN = 24;
        static constexpr float INTERNAL_VREF_VOLTAGE = 4.5f;
        static constexpr float EXTERNAL_VREF_VOLTAGE = 5.0f;    // Modify this value to suit your circuit.

        // private member variables
        
        // instance of the SPI Scheduler and Pin Control.
        SpiDriver * _SpiDriverInstance;
        PinControl * _PinControlInstance;

        int32_t _ChannelGainsCache[MAX_CHANNELS];
        float _VotlageReferenceCache;

        // private member functions 
        eRegisters GetChannelRegisterFromChannelIdEnum(eChannelId chan);
        uint8_t ReadRegister(eRegisters reg);
        void WriteRegister(eRegisters reg, uint8_t newValue);
        void ModifyRegister(eRegisters reg, uint8_t mask, uint8_t newValue);

        int32_t GetMultiplierFromChannelGain(eChannelGain gain);
        void SetChannelGainsToDefault(void);

};

#endif
