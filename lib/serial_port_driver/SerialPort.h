
#ifndef _SERIAL_PORT_DRIVER
#define _SERIAL_PORT_DRIVER

#include <EventHandler.h>
#include <CanProcesEvents.h>
#include <PcCommunicationsInterface.h>

class SerialPort : public CanProcessEvents, public PcCommunicationsInterface {

    public:

        SerialPort();
        explicit SerialPort(EventHandler * eh);

        void ProcessEvent(NEvent::eEvent event) override;
        
        uint8_t GetReceivedBytes(uint8_t data[], uint8_t max_length) override;
        void TransmitData(uint8_t data[], uint8_t count) override;

    protected:

    private:

        EventHandler * _EventHandlerInstance;
};

#endif
