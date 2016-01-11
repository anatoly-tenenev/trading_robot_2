#include "eventbus.h"
#include "../imarket.h"

namespace atrade {
namespace backtester {

EventBus::EventBus(IMarket& market, pt::time_duration delay, std::function<pt::ptime()> get_now) :
    m_market(&market),
    m_delay(delay),
    m_get_now(get_now)
{
}

void EventBus::add_event(const std::function<void()>& event)
{
    m_events.emplace(m_get_now() + m_delay, event);
}

void EventBus::process()
{
    if (!m_events.empty() && m_events.front().datetime <= m_market->datetime())
    {
        auto event = m_events.front().event;
        m_events.pop();
        event();
    }
    else
    {
        m_market->process();
    }
}

pt::ptime EventBus::datetime() const
{
    if (!m_events.empty() && m_events.front().datetime <= m_market->datetime())
    {
        return m_events.front().datetime;
    }
    else
    {
        return m_market->datetime();
    }
}

}
}
