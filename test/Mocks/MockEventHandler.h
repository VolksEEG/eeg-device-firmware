#ifndef MOCK_EVENT_HANDLER
#define MOCK_EVENT_HANDLER

#include <EventHandler.h>

class MockEvenHandler : public EventHandler {

    public:

        MockEvenHandler()
        {
            _NumberOfEventsSignalled = 0;
            _LastEventSignalled = NEvent::eEvent::Event_InvalidEvent;
        }

        void SignalEvent(NEvent::eEvent event) override 
        {
            _NumberOfEventsSignalled++;
            _LastEventSignalled = event;
        }

        NEvent::eEvent GetLastSignalledEvent()
        {
            return _LastEventSignalled;
        }

        uint8_t GetNumberOfEventsSignalled()
        {
            return _NumberOfEventsSignalled;
        }

    protected:

    private:

        uint8_t _NumberOfEventsSignalled;
        NEvent::eEvent _LastEventSignalled;
};

#endif
