
#ifndef _PROTOCOL_PARSER
#define _PROTOCOL_PARSER

#include <ProtocolFrameParser.h>
#include <EEGDataConsumer.h>

class ProtocolParser : public EegDataConsumer {

    public:

        ProtocolParser();
        explicit ProtocolParser(ProtocolFrameParser * pfp);

        void PushLatestSample(EegData::sEegSamples samples) override;

    protected:

    private:

        ProtocolFrameParser * _ProtocolFrameParser;
};

#endif
