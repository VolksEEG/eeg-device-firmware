
#ifndef _PROTOCOL_PARSER
#define _PROTOCOL_PARSER

#include <ProtocolFrameParser.h>
#include <EEGDataConsumer.h>

class ProtocolParser : public EegDataConsumer {

    public:

        ProtocolParser();
        explicit ProtocolParser(ProtocolFrameParser * pfp);

        // TODO - remove these.
        int GetValue(void);
        void SetValue(int newValue);

        void PushLatestSample(EegData::sEegSamples samples) override;

    protected:

    private:

        int mValue;

        ProtocolFrameParser * _ProtocolFrameParser;
};

#endif
