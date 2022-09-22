
#ifndef _PROTOCOL_RECEIVER
#define _PROTOCOL_RECEIVER

#include "PcCommunicationsInterface.h"
#include "ProtocolGeneral.h"
#include "IProtocolTransmission.h"

#include "../data_flow_controller/EEGDataProducer.h"
#include "../event_handler/CanProcesEvents.h"
#include "../event_handler/EventHandler.h"

class ProtocolReceiver : public CanProcessEvents, private ProtocolGeneral  {

    public:

        ProtocolReceiver();
        ProtocolReceiver(PcCommunicationsInterface * pci, EegDataProducer * edp, EventHandler * evh, IProtocolTransmission * pti);

        void ProcessEvent(NEvent::eEvent event) override;

        #ifdef PIO_UNIT_TESTING

        typedef enum _RX_STATES
        {
            WaitForSequence,
            GetProtocolVersion,
            GetPayloadLength,
            GetIdNumber,
            GetAckId,
            GetPayloadChecksum,
            GetHeaderChecksum,
            GetPayload,
            InvalidState
        }RX_STATE;

        RX_STATE GetCurrentRxState();

        uint8_t GetNextExpectedId();

        #endif
    protected:

    private:

        PcCommunicationsInterface * _PcComsInterface;
        EegDataProducer * _EEGDataProducer;
        EventHandler * _EventHandler;
        IProtocolTransmission * _ProtocolTransmissionInstance;

        typedef struct _RX_STATE
        {
            // reception varaibles
            _RX_STATE (*state_fptr)(uint8_t c, _RX_STATE state, ProtocolReceiver * protocolReceiver);
            uint8_t rxIndex;
            uint8_t rxMultiByteCounter;

            // message values
            uint8_t message[ProtocolGeneral::_MAX_MESSAGE_LENGTH];

            uint8_t lastValidId;
            uint8_t nextExpectedId;
        }sRxStruct;

        sRxStruct _RxState;

        void SendPayloadToPc(uint8_t * payload_ptr, uint8_t payloadLength);

        static ProtocolReceiver * _ProtocolParser;

        static sRxStruct ResetRxStruct(sRxStruct state);

        static sRxStruct RxState_WaitForSyncSequence(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetProtocolVersion(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetPayloadLength(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetIdNumber(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetAcknowledgeId(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetPayloadChecksum(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetHeaderChecksum(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
        static sRxStruct RxState_GetPayloadAndProcessMessageContents(uint8_t c, sRxStruct state, ProtocolReceiver * protocolReceiver);
};

#endif
