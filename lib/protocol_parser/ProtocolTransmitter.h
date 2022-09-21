
#ifndef _PROTOCOL_TRANSMITTER
#define _PROTOCOL_TRANSMITTER

#include "PcCommunicationsInterface.h"

#include <EEGDataConsumer.h>
#include <CanProcesEvents.h>

class ProtocolTransmitter : public EegDataConsumer, public CanProcessEvents  {

    public:

        ProtocolTransmitter();
        ProtocolTransmitter(PcCommunicationsInterface * pci);

        void ProcessEvent(NEvent::eEvent event) override;

    protected:

    private:

        PcCommunicationsInterface * _PcComsInterface;
};

#endif
