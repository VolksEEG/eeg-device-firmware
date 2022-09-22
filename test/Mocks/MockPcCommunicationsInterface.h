#ifndef MOCK_PC_COMMUNICATIONS_INTERFACE
#define MOCK_PC_COMMUNICATIONS_INTERFACE

#include <PcCommunicationsInterface.h>

class MockPcCommunicationsInterface : public PcCommunicationsInterface {
    public:

        MockPcCommunicationsInterface()
        {
            read_index = 0;
            read_count = 0;

            for (int i = 0; i < READ_BYTES_MAX; ++i)
            {
                read_bytes[i] = 0;
            }
        }

        uint8_t GetReceivedBytes(uint8_t data[], uint8_t max_length) override {
            
            uint8_t copied_bytes = 0;

            while   (
                        (max_length > copied_bytes)
                        && (read_index < read_count)
                    )
            {
                data[copied_bytes++] = read_bytes[read_index++];
            }

            return copied_bytes;
        }

        void TransmitData(uint8_t data[], uint16_t count) override {

        }

        void LoadReceiveBytes(uint8_t data[], uint8_t count)
        {
            for (int i = 0; i < count; ++i)
            {
                read_bytes[i] = data[i];
            }

            read_index = 0;
            read_count = count;
        }

    private:

        static const uint8_t READ_BYTES_MAX = 100;
        uint8_t read_index;
        uint8_t read_count;
        uint8_t read_bytes[READ_BYTES_MAX];
};

#endif
