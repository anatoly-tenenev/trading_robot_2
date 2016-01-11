#pragma once

#include <atrade/atrade.hpp>
#include <iostream>

class SimpleTradingGuard2 : public atrade::async2::Strategy
{
public:
    SimpleTradingGuard2(boost::asio::io_service& ios, int64_t isin_id) :
        Strategy(ios, nullptr),
        m_isin_id(isin_id)
    {
    }

    void operator() (atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        using namespace atrade;

        int i = 0;
        while (true)
        {
            std::vector<atrade::Order> _orders;
            auto subscr = async2::subscribe_order(ios(), [&](const atrade::Order& order) {
                if (order.isin_id == m_isin_id)
                {
                    _orders.push_back(order);
                }
            });

            async2::Watcher<async2::BooleanIndicator> watcher(ios(), ctx, m_time_indicator.get());

            watcher.async_wait(true, yield);

            std::cout << "### 1: " << i << " " << current_datetime() << "\n";
            m_closing_strategy->run(ctx, yield);

            async2::Spawner spawner(ios(), ctx);

            std::cout << "### 2: " << i << " " << current_datetime() << "\n";
            for (auto& strategy : m_strategies)
            {
                spawner.spawn([=](async2::TradingContext& ctx, asio::yield_context yield) {
                    strategy->run(ctx, yield);
                });
            }
            watcher.async_wait(false, yield);
            spawner.reset();

            std::cout << "### 3: " << i << " " << current_datetime() << "\n";
            auto closing_strategy = m_closing_strategy;
            spawner.spawn([=](async2::TradingContext& ctx, asio::yield_context yield) {
                closing_strategy->run(ctx, yield);
            });
            try
            {
                spawner.async_join(pt::seconds(45), yield);
            }
            catch(boost::system::system_error&)
            {
                spawner.reset();
            }

            std::cout << "### 4: " << i << " " << current_datetime()
                      << " profit=" << atrade::calc_profit(_orders) << "\n";
            ++i;
        }
    }

    template <typename TimeIndicatorT, typename ...Args>
    void set_time_indicator(Args&&... args)
    {
        m_time_indicator = std::make_shared<TimeIndicatorT>(std::forward<Args>(args)...);
    }

    template <typename StrategyT, size_t N = 1, typename ...Args>
    void add_strategy(Args&&... args)
    {
        for (size_t i = 0; i < N; ++i)
        {
            m_strategies.emplace_back(std::make_shared<StrategyT>(std::forward<Args>(args)...));
        }
    }

    template <typename StrategyT, typename ...Args>
    void set_closing_strategy(Args&&... args)
    {
        m_closing_strategy = std::make_shared<StrategyT>(std::forward<Args>(args)...);
    }

    void run(atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        (*this)(ctx, yield);
    }

private:
    int64_t m_isin_id;
    std::shared_ptr<atrade::async2::BooleanIndicator> m_time_indicator;
    atrade::vector_sptr<atrade::async2::IStrategy> m_strategies;
    std::shared_ptr<atrade::async2::IStrategy> m_closing_strategy;
};


