
#ifndef _SERIAL_PORT_DRIVER
#define _SERIAL_PORT_DRIVER

#include <EventHandler.h>
#include <CanProcesEvents.h>
#include <PcCommunicationsInterface.h>

class SerialPort : public CanProcessEvents, PcCommunicationsInterface {

    public:

        SerialPort();
        explicit SerialPort(EventHandler * eh);

        void ProcessEvent(NEvent::eEvent event) override;
        
        int8_t GetReceivedByte() override;
        void TransmitData(int8_t * dataPtr, int8_t count) override;

    protected:

    private:

        EventHandler * _EventHandlerInstance;
};

#endif
