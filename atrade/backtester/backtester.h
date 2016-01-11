#ifndef ATRADE_BACKTESTER_H
#define ATRADE_BACKTESTER_H

#include "../headers.h"
#include "../imarket.h"
#include "eventbus.h"
#include "simplemarketsimulator.h"
#include "../utils.hpp"

namespace atrade {
namespace backtester {

class Backtester : public IMarket
{
public:
    Backtester(std::unique_ptr<IMarket>&& simulator, pt::time_duration delay, std::function<pt::ptime()> get_now);
    Backtester(std::unique_ptr<IMarket>&& simulator, std::function<pt::ptime()> get_now);
    void add_order(const Order& order) override;
    void del_order(int64_t order_id) override;
    void move_order(int64_t order_id, int64_t new_order_id, double price, int volume) override;
    void req_securities() override;
    void ordlog_handler(const OrdLogHandler& handler) override;
    void order_handler(const OrderHandler& handler) override;
    void order_error_handler(const OrderErrorHandler& handler) override;
    void tick_handler(const TickHandler& handler) override;
    void stock_handler(const StockHandler& handler) override;
    void security_handler(const SecurityHandler& handler) override;
    std::vector<Quote> stocks(int64_t isin_id) override;
    pt::ptime datetime() override;
    void process() override;

protected:
    std::unique_ptr<IMarket> m_simulator;
    EventBus m_eventbus;
};

class SimpleBacktester : public Backtester
{
public:
    SimpleBacktester(pt::time_duration delay, std::function<pt::ptime()> get_now) :
        Backtester(std::make_unique<SimpleMarketSimulator>(), delay, get_now)
    {
    }

    SimpleBacktester(std::function<pt::ptime()> get_now) :
        SimpleBacktester(pt::milliseconds(0), get_now)
    {
    }

    void add_data_source(std::unique_ptr<IMarketDataSource>&& data_source)
    {
        SimpleMarketSimulator& simulator =
                static_cast<SimpleMarketSimulator&>(*m_simulator);
        simulator.add_data_source(std::move(data_source));
    }

    template <typename TDataSource, typename ...Args>
    void add_data_source(const Args&... args)
    {
        add_data_source(std::make_unique<TDataSource>(args...));
    }

    void print_result() const
    {
        std::cout << "Backtester result:\n";
        auto orders = dynamic_cast<MarketSimulator&>(*m_simulator).orders_history();
        for (auto& e : orders)
        {
            std::cout << "isin_id=" << e.first
                      << " deals_amount=" << calc_deals_amount(e.second)
                      << " profit=" << calc_profit(e.second) << "\n";
        }
    }
};

}
}

#endif // ATRADE_BACKTESTER_H
