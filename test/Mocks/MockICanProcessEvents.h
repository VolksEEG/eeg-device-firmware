#include <ICanProcesEvents.h>

class MockICanProcessEvents : public ICanProcessEvents {
    public:

    MockICanProcessEvents() : 
        _ProcessEventCalledCount(0)
    {

    }

    void ProcessEvent(NEvent::eEvent event) override {
        _ProcessEventCalledCount++;
    }

    int GetProcessedEventCalledTimes() 
    {
        return _ProcessEventCalledCount;
    }

    private:

        int _ProcessEventCalledCount;
};
