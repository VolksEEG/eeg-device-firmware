
#ifndef _SERIAL_PORT_DRIVER
#define _SERIAL_PORT_DRIVER

#include <EventHandler.h>
#include <ICanProcesEvents.h>
#include <../protocol_parser/iPcCommunications.h>

class SerialPort : public ICanProcessEvents, public IPcCommunications {

    public:

        SerialPort();
        explicit SerialPort(EventHandler * eh);

        void BackgroundTaskHandler(void);
        void ProcessEvent(NEvent::eEvent event) override;
        
        uint8_t GetReceivedBytes(uint8_t data[], uint8_t max_length) override;
        void TransmitData(uint8_t data[], uint16_t count) override;

    protected:

    private:

        typedef uint8_t RX_BUF_TYPE;

        static const RX_BUF_TYPE _RX_BUFFER_SIZE = 100;

        EventHandler * _EventHandlerInstance;

        uint8_t _RxBuffer[_RX_BUFFER_SIZE];
        RX_BUF_TYPE _RxInputIndex;
        RX_BUF_TYPE _RxOutputIndex;
        RX_BUF_TYPE _RxCount;
        bool _RxDataProcessingComplete;
        bool _ReSignalDataRxEvent;
};

#endif
