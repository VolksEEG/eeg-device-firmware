
#ifndef _ADS1299_LOW_DRIVER
#define _ADS1299_LOW_DRIVER

#include <stdint.h>
#include <SpiDriver.h>
#include <PinControl.h>
#include <EegData.h>

class Ads1299LowDriver {

    public:

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
        EegData::sEegSamples GetEEGData(void);

    protected:

    private:

        // instance of the SPI Scheduler and Pin Control.
        SpiDriver * _SpiDriverInstance;
        PinControl * _PinControlInstance;

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
            REGISTER_CH8SET = 0x0C
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

        // used to clock the SPI data out of the ADS1299
        static const uint8_t BLANK_DATA = 0x00;
        

        static constexpr float DEFAULT_CHANNEL_GAIN = 24.0f;
        static constexpr float INTERNAL_VREF_VOLTAGE = 4.5f;
        static constexpr float EXTERNAL_VREF_VOLTAGE = 5.0f;    // Modify this value to suit your circuit.
        static constexpr float MICROVOLTS_IN_A_VOLT = 1000000.0f;

        typedef struct _VOLTAGE_CONVERSION_SPECS
        {
            float gainCh1;
            float multiplierCh1;
            float gainCh2;
            float multiplierCh2;
            float gainCh3;
            float multiplierCh3;
            float gainCh4;
            float multiplierCh4;
            float gainCh5;
            float multiplierCh5;
            float gainCh6;
            float multiplierCh6;
            float gainCh7;
            float multiplierCh7;
            float gainCh8;
            float multiplierCh8;
            float vref;

            void SetToDefaults()
            {
                gainCh1 = DEFAULT_CHANNEL_GAIN;
                gainCh2 = DEFAULT_CHANNEL_GAIN;
                gainCh3 = DEFAULT_CHANNEL_GAIN;
                gainCh4 = DEFAULT_CHANNEL_GAIN;
                gainCh5 = DEFAULT_CHANNEL_GAIN;
                gainCh6 = DEFAULT_CHANNEL_GAIN;
                gainCh7 = DEFAULT_CHANNEL_GAIN;
                gainCh8 = DEFAULT_CHANNEL_GAIN;
                vref = EXTERNAL_VREF_VOLTAGE;

                RecalculateMultipliers();
            }

            void RecalculateMultipliers()
            {
                multiplierCh1 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh1) / 8388607.0f);
                multiplierCh2 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh2) / 8388607.0f);
                multiplierCh3 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh3) / 8388607.0f);
                multiplierCh4 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh4) / 8388607.0f);
                multiplierCh5 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh5) / 8388607.0f);
                multiplierCh6 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh6) / 8388607.0f);
                multiplierCh7 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh7) / 8388607.0f);
                multiplierCh8 = (((vref * MICROVOLTS_IN_A_VOLT) / gainCh8) / 8388607.0f);
            }
        }sVOLTAGE_CONVERSION_SPECS;

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

        // private member variables
        sVOLTAGE_CONVERSION_SPECS _VoltageConversionSpecs;

        // private member functions 
        eRegisters GetChannelRegisterFromChannelIdEnum(eChannelId chan);
        uint8_t ReadRegister(eRegisters reg);
        void WriteRegister(eRegisters reg, uint8_t newValue);
        void ModifyRegister(eRegisters reg, uint8_t mask, uint8_t newValue);

        float GetMultiplierFromChannelGain(eChannelGain gain);

};

#endif
