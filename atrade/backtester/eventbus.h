#ifndef ATRADE_EVENTBUS_H
#define ATRADE_EVENTBUS_H

#include "../headers.h"

namespace atrade {
namespace backtester {

struct DelayedEvent
{
    pt::ptime datetime;
    std::function<void()> event;

    DelayedEvent() {}
    DelayedEvent(const pt::ptime& _datetime,
                 const std::function<void()>& _event) :
        datetime(_datetime),
        event(_event)
    {}
};

class EventBus
{
public:
    EventBus(IMarket& market, pt::time_duration delay, std::function<pt::ptime()> get_now);
    void add_event(const std::function<void()>& event);
    void process();
    pt::ptime datetime() const;

private:
    IMarket* m_market;
    pt::time_duration m_delay;
    std::function<pt::ptime()> m_get_now;
    std::queue<DelayedEvent> m_events;

    void next_process();
    pt::ptime next_datetime() const;
};

}
}

#endif // ATRADE_EVENTBUS_H
