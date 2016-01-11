#pragma once

#include <atrade/atrade.hpp>
#include <functions/functions.hpp>
#include <iostream>

class IsinIdException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Isin Id exception";
    }
};

class SimpleClosing3 : public atrade::async2::Strategy
{
public:
    SimpleClosing3(boost::asio::io_service& ios, atrade::async2::IAsyncMarket* market, int64_t isin_id) :
        Strategy(ios, market),
        m_isin_id(isin_id)
    {
    }

    SimpleClosing3(boost::asio::io_service& ios, atrade::async2::IAsyncMarket& market, int64_t isin_id) :
        SimpleClosing3(ios, &market, isin_id)
    {
    }

    SimpleClosing3(boost::asio::io_service& ios, int64_t isin_id) :
        SimpleClosing3(ios, nullptr, isin_id)
    {
    }

    void operator() (atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        using namespace atrade;
        using namespace functions;

        atrade::Security security = async2::get_security(ios(), ctx, m_isin_id, yield);
        double step = security.step();

        boost::system::error_code errc;
        int tries = 0;

        async2::Portfolio* portfolio;
        std::unique_ptr<async2::Portfolio> _portfolio;
        if (market())
        {
            _portfolio = std::make_unique<async2::Portfolio>(*market());
            portfolio = _portfolio.get();
        }
        else
        {
            portfolio = &async2::get_portfolio(ios());
        }

        do {

            std::unordered_set<int64_t> order_ids;
            std::vector<atrade::Order> orders;

            if (market())
            {
                order_ids = market()->add_order_pendings();
            }
            else
            {
                order_ids = async2::get_add_order_pendings(ios());
            }
            orders = portfolio->orders(m_isin_id);

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

            std::vector<Order> position = portfolio->position(m_isin_id);
            if (!position.size())
            {
                return;
            }

            OrderBook orderbook(ios(), m_isin_id);

            auto closing_order = portfolio->closing_order(m_isin_id);
            if (tries < 3)
            {
                closing_order.type = atrade::OrderType::Limit;
                closing_order.price = ((closing_order.dir == OrderDir::Bid) ?
                                           orderbook.best_bid() + 1*step :
                                           orderbook.best_ask() - 1*step);
            }
            else
            {
                std::cout << "closing by market order\n";
            }

            int64_t order_id = async_add_order(ctx, closing_order, yield);
            async2::WaitDeal wait_deal(ios(), ctx, order_id);

            errc = boost::system::error_code();
            wait_deal.async_wait(true, pt::seconds(10), yield[errc]);

            ++tries;

        } while (errc);

        std::cout << "closing tries = " << tries << "\n";
    }

    void run(atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        (*this)(ctx, yield);
    }

private:
    int64_t m_isin_id;
};


