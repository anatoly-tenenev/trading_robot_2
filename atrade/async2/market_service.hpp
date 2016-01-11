#ifndef ATRADE_ASYNC_2_MARKET_SERVICE_HPP
#define ATRADE_ASYNC_2_MARKET_SERVICE_HPP

#include "../headers.h"
#include "asyncmarket.hpp"
#include "portfolio.hpp"
#include "task.hpp"

namespace atrade {
namespace async2 {

class MarketService : public asio::io_service::service
{
public:
    MarketService(asio::io_service& ios) :
        service(ios),
        m_datetime(pt::not_a_date_time)
    {
    }

    ~MarketService()
    {
    }

    void init_market(IMarket& market)
    {
        m_root_market = std::make_unique<RootAsyncMarket>(market);
        m_portfolio = std::make_unique<Portfolio>(*m_root_market);
        m_datetime = m_root_market->datetime();
    }

    void process_market()
    {
        if (m_tasks.datetime() <= m_root_market->datetime())
        {
            if (m_datetime < m_tasks.datetime())
            {
                m_datetime = m_tasks.datetime();
            }
            m_tasks.execute_first();
        }
        else
        {
            m_datetime = m_root_market->datetime();
            m_root_market->process();
        }
    }

    pt::ptime datetime() const
    {
        if (!m_datetime.is_not_a_date_time())
        {
            return m_datetime;
        }
        throw UndefinedDatetimeException();
    }

    template <typename Function>
    Task add_task(const pt::ptime& datetime, Function&& func)
    {
        return Task(m_tasks, datetime, std::forward<Function>(func));
    }

    template <typename Function>
    Task add_task(const pt::time_duration& duration, Function&& func)
    {
        return Task(m_tasks, m_root_market->datetime() + duration, std::forward<Function>(func));
    }

    void add_order(const Order& order)
    {
        m_root_market->add_order(order);
    }

    void del_order(int64_t order_id)
    {
        m_root_market->del_order(order_id);
    }

    void move_order(int64_t order_id, int64_t new_order_id, double price, int volume)
    {
        m_root_market->move_order(order_id, new_order_id, price, volume);
    }

    void req_securities()
    {
        m_root_market->req_securities();
    }

    Subscription<OrdLogHandler> subscribe_ordlog(const OrdLogHandler& handler)
    {
        return m_root_market->subscribe_ordlog(handler);
    }

    Subscription<OrderHandler> subscribe_order(const OrderHandler& handler)
    {
        return m_root_market->subscribe_order(handler);
    }

    Subscription<OrderErrorHandler> subscribe_order_error(const OrderErrorHandler& handler)
    {
        return m_root_market->subscribe_order_error(handler);
    }

    Subscription<TickHandler> subscribe_tick(const TickHandler& handler)
    {
        return m_root_market->subscribe_tick(handler);
    }

    Subscription<StockHandler> subscribe_stock(const StockHandler& handler)
    {
        return m_root_market->subscribe_stock(handler);
    }

    Subscription<SecurityHandler> subscribe_security(const SecurityHandler& handler)
    {
        return m_root_market->subscribe_security(handler);
    }

    Portfolio& get_portfolio()
    {
        return *m_portfolio;
    }

    History& get_history()
    {
        return m_root_market->history();
    }

    MarketData& get_market_data()
    {
        return m_root_market->market_data();
    }

    std::unordered_set<int64_t> get_add_order_pendings() const
    {
        return m_root_market->add_order_pendings();
    }

    IAsyncMarket& get_market()
    {
        return *m_root_market;
    }

    static asio::io_service::id id;

    struct implementation_type
    {
    };

    void construct(implementation_type&)
    {
    }

    void destroy(implementation_type&)
    {
    }

private:
    std::unique_ptr<RootAsyncMarket> m_root_market;
    std::unique_ptr<Portfolio> m_portfolio;
    TaskService m_tasks;
    pt::ptime m_datetime;

    void shutdown_service()
    {
    }
};

void init_market(asio::io_service& ios, IMarket& market);
void process_market(asio::io_service& ios);
pt::ptime current_datetime(asio::io_service& ios);
pt::ptime _current_datetime(asio::io_service* ios);

template <typename Function>
Task add_task(asio::io_service& ios, const pt::ptime& datetime, Function&& task)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.add_task(datetime, std::forward<Function>(task));
}

template <typename Function>
Task add_task(asio::io_service& ios, const pt::time_duration& duration, Function&& task)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.add_task(duration, std::forward<Function>(task));
}

void add_order(asio::io_service& ios, const Order& order);
void del_order(asio::io_service& ios, int64_t order_id);
void move_order(asio::io_service& ios, int64_t order_id, int64_t new_order_id, double price, int volume);
void req_securities(asio::io_service& ios);

Subscription<OrdLogHandler> subscribe_ordlog(asio::io_service& ios, const OrdLogHandler& handler);
Subscription<OrdLogHandler> subscribe_ordlog(asio::io_service& ios, int64_t isin_id, const OrdLogHandler& handler);
Subscription<OrderHandler> subscribe_order(asio::io_service& ios, const OrderHandler& handler);
Subscription<OrderErrorHandler> subscribe_order_error(asio::io_service& ios, const OrderErrorHandler& handler);
Subscription<TickHandler> subscribe_tick(asio::io_service& ios, const TickHandler& handler);
Subscription<StockHandler> subscribe_stock(asio::io_service& ios, const StockHandler& handler);
Subscription<SecurityHandler> subscribe_security(asio::io_service& ios, const SecurityHandler& handler);

Portfolio& get_portfolio(asio::io_service& ios);
History& get_history(asio::io_service& ios);
MarketData& get_market_data(asio::io_service& ios);
std::unordered_set<int64_t> get_add_order_pendings(asio::io_service& ios);

IAsyncMarket& get_market(asio::io_service& ios);

}
}

#endif // ATRADE_ASYNC_2_MARKET_SERVICE_HPP

