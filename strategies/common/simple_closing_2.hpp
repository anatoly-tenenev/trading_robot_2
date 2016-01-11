#pragma once

#include <atrade/atrade.hpp>
#include <functions/functions.hpp>
#include <iostream>

class SimpleClosing2 : public atrade::async2::Strategy
{
public:
    SimpleClosing2(boost::asio::io_service& ios, atrade::async2::IAsyncMarket* market, int64_t isin_id) :
        Strategy(ios, market),
        m_isin_id(isin_id)
    {
    }

    SimpleClosing2(boost::asio::io_service& ios, atrade::async2::IAsyncMarket& market, int64_t isin_id) :
        SimpleClosing2(ios, &market, isin_id)
    {
    }

    SimpleClosing2(boost::asio::io_service& ios, int64_t isin_id) :
        SimpleClosing2(ios, nullptr, isin_id)
    {
    }

    void operator() (atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        using namespace atrade;
        using namespace functions;

        auto order_ids = async2::get_add_order_pendings(ios());

        async2::Portfolio& portfolio = async2::get_portfolio(ios());
        auto orders = portfolio.orders(m_isin_id);

        for (const auto& order : orders)
        {
            order_ids.insert(order.order_id);
        }

        async2::Spawner spawner(ios(), ctx);
        for (int64_t order_id : order_ids)
        {
            spawner.spawn([&, order_id](async2::TradingContext& ctx, asio::yield_context yield) {
                boost::system::error_code errc;
                async_cancel_order(ctx, order_id, yield[errc]);
            });
        }
        spawner.async_join(yield);

        std::vector<Order> position = portfolio.position(m_isin_id);
        if (!position.size())
        {
            return;
        }

        auto closing_order = portfolio.closing_order(m_isin_id);
        int64_t order_id = async_add_order(ctx, closing_order, yield);
        async2::WaitDeal wait_deal(ios(), ctx, order_id);
        wait_deal.async_wait(true, yield);

        std::cout << "closing_order.volume=" << closing_order.volume << "\n";
    }

    void run(atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        (*this)(ctx, yield);
    }

private:
    int64_t m_isin_id;
};




