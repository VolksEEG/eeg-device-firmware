
#ifndef _PROTOCOL_TRANSMITTER
#define _PROTOCOL_TRANSMITTER

#include "IPcCommunications.h"
#include "IProtocolTransmission.h"
#include "ProtocolGeneral.h"

#include "../data_flow_controller/IEEGDataConsumer.h"
#include "../event_handler/ICanProcesEvents.h"
#include "../event_handler/EventHandler.h"

class ProtocolTransmitter : public IEegDataConsumer, public ICanProcessEvents, public IProtocolTransmission, private ProtocolGeneral  {

    public:

        ProtocolTransmitter();
        ProtocolTransmitter(IPcCommunications * pci, EventHandler * evh);

        void ProcessEvent(NEvent::eEvent event) override;

        bool SendPayloadToPc(uint8_t * payload_ptr, uint8_t payload_length) override;
        void UpdateIdToAcknowledge(uint8_t id) override;
        void UpdateAcknowledgedId(uint8_t id) override;

        void PushLatestSample(EegData::sEegSamples samples) override;

        static const uint8_t _MAX_TX_MESSAGES = 10;

    protected:

    private:

        IPcCommunications * _PcComsInterfaceInstance;
        EventHandler * _EventHandlerInstance;

        typedef struct _TX_MESSAGE
        {
            uint8_t message[ProtocolGeneral::_MAX_MESSAGE_LENGTH];
        }sTxMessageStruct;

        uint8_t _IdToAcknowledge;
        uint8_t _IdToSend;

        // tx fifo variables
        uint8_t _TxIpIndex;
        uint8_t _TxNextUnackedIndex;
        uint8_t _TxNextOpIndex;
        uint8_t _TxCount;
        sTxMessageStruct _TxMessages[_MAX_TX_MESSAGES];

};

#endif