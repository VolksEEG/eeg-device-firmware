
#ifndef _PROTOCOL_FRAME_PARSER
#define _PROTOCOL_FRAME_PARSER

#include "PcCommunicationsInterface.h"

#include <EventHandler.h>

#include <stdint.h>
#include <cstdlib>

class ProtocolFrameParser : public CanProcessEvents {

    public:

        ProtocolFrameParser();
        explicit ProtocolFrameParser(PcCommunicationsInterface * pci);

        void ProcessEvent(NEvent::eEvent event) override;

        void AddFrameToPayloadAndSendToPc(uint8_t payloadData[], uint8_t payloadLength);

    protected:

        PcCommunicationsInterface * _PcComsInterface;

        static const uint8_t START_OF_FRAME_LSB = 0xFF;
        static const uint8_t START_OF_FRAME_MSB = 0xFF;
        static const uint8_t FRAME_HEADER_LENGTH = 0x02;

    private:

};

#endif
