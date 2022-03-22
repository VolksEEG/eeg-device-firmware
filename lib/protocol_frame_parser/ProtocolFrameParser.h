
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

        static const uint8_t START_OF_FRAME = 0xA5;
        static const uint8_t FRAME_VERSION = 0x01;
        static const uint8_t FRAME_HEADER_LENGTH = 0x05;

    private:

};

#endif
