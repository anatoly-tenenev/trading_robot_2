#include "backtester.h"

namespace atrade {
namespace backtester {

Backtester::Backtester(std::unique_ptr<IMarket>&& simulator, pt::time_duration delay, std::function<pt::ptime()> get_now) :
    m_simulator(std::move(simulator)),
    m_eventbus(*m_simulator, delay, get_now)
{

}

Backtester::Backtester(std::unique_ptr<IMarket>&& simulator, std::function<pt::ptime()> get_now) :
    Backtester(std::move(simulator), pt::milliseconds(0), get_now)
{
}

void Backtester::add_order(const Order& order)
{
    m_eventbus.add_event([=]() {
        m_simulator->add_order(order);
    });
}

void Backtester::del_order(int64_t order_id)
{
    m_eventbus.add_event([=]() {
        m_simulator->del_order(order_id);
    });
}

void Backtester::move_order(int64_t order_id, int64_t new_order_id, double price, int volume)
{
    m_eventbus.add_event([=]() {
        m_simulator->move_order(order_id, new_order_id, price, volume);
    });
}

void Backtester::req_securities()
{
    m_eventbus.add_event([=]() {
        m_simulator->req_securities();
    });
}

void Backtester::ordlog_handler(const OrdLogHandler& handler)
{
    m_simulator->ordlog_handler([=](int64_t isin_id, const OrdLogEntry& entry) {
        m_eventbus.add_event([&handler, isin_id, entry]() {
            handler(isin_id, entry);
        });
    });
}

void Backtester::order_handler(const OrderHandler& handler)
{
    m_simulator->order_handler([=](const Order& order) {
        m_eventbus.add_event([&handler, order]() {
            handler(order);
        });
    });
}

void Backtester::order_error_handler(const OrderErrorHandler& handler)
{
    m_simulator->order_error_handler([=](const OrderError& error) {
        m_eventbus.add_event([&handler, error]() {
            handler(error);
        });
    });
}

void Backtester::tick_handler(const TickHandler& handler)
{
    m_simulator->tick_handler([=](const Deal& tick) {
        m_eventbus.add_event([&handler, tick]() {
            handler(tick);
        });
    });
}

void Backtester::stock_handler(const StockHandler& handler)
{
    m_simulator->stock_handler([=](int64_t isin_id, const std::vector<Quote>& stocks) {
        m_eventbus.add_event([&handler, isin_id, stocks]() {
            handler(isin_id, stocks);
        });
    });
}

void Backtester::security_handler(const SecurityHandler& handler)
{
    m_simulator->security_handler([=](const std::vector<Security>& securities) {
        m_eventbus.add_event([&handler, securities]() {
            handler(securities);
        });
    });
}

std::vector<Quote> Backtester::stocks(int64_t isin_id)
{
    return m_simulator->stocks(isin_id);
}

boost::posix_time::ptime Backtester::datetime()
{
    return m_eventbus.datetime();
}

void Backtester::process()
{
    m_eventbus.process();
}

}
}
