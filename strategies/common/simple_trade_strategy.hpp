#pragma once

#include <atrade/atrade.hpp>
#include <functions/functions.hpp>
#include <strategies/common/simple_closing_2.hpp>
#include <strategies/common/simple_closing_3.hpp>
#include <indicators/common/price_indicator.hpp>
#include <indicators/common/price_indicator_2.hpp>
#include <indicators/common/market_order_indicator.hpp>
#include <indicators/common/step_indicator.hpp>
#include <indicators/common/volume_indicator.hpp>
#include <iostream>

class SimpleTradeStrategy : public atrade::async2::Strategy
{
public:
    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;

    SimpleTradeStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket* market, int64_t isin_id, atrade::OrderDir order_dir) :
        Strategy(ios, market),
        m_isin_id(isin_id),
        m_order_dir(order_dir)
    {
    }

    SimpleTradeStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket& market, int64_t isin_id, atrade::OrderDir order_dir) :
        SimpleTradeStrategy(ios, &market, isin_id, order_dir)
    {
    }

    SimpleTradeStrategy(boost::asio::io_service& ios, int64_t isin_id, atrade::OrderDir order_dir) :
        SimpleTradeStrategy(ios, nullptr, isin_id, order_dir)
    {
    }

    ~SimpleTradeStrategy()
    {
    }

    void operator() (atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        using namespace atrade;
        using namespace functions;

        using asio::io_service;
        using async2::TradingContext;
        using asio::yield_context;
        using async2::Timer;
        using Watcher = async2::Watcher<async2::BooleanIndicator>;
        using TimerPtr = std::unique_ptr<Timer>;
        using WatcherPtr = std::unique_ptr<Watcher>;

        Security security = async2::get_security(ios(), ctx, m_isin_id, yield);
        double step = security.step();

        async2::Spawner spawner(ios(), ctx);

        boost::system::error_code errc;

        OrderBook orderbook3(ios(), m_isin_id);

        Order order;
        int order_id;

        std::vector<Order> __orders;
        Order _order;

        order = Order(m_isin_id,
                      m_order_dir,
                      (m_order_dir == OrderDir::Bid) ?
                          orderbook3.best_bid() + 1*step :
                          orderbook3.best_ask() - 1*step,
                      1);
        order_id = async_add_order(ctx, order, yield);
        async2::WaitDeal wait_deal_1(ios(), ctx, order_id);
        __orders = wait_deal_1.async_wait(true, pt::seconds(15), yield[errc]);
        if (errc)
        {
            spawner.spawn(SimpleClosing3(ios(), market(), m_isin_id));
            spawner.async_join(yield);
            return;
        }
        _order = __orders.front();

        const int max_volume = 150;

        order.dir = !order.dir;
        order.type = atrade::OrderType::Limit;
        order.price = find_price(ios(), m_isin_id, order.dir, max_volume);
//        order.price = ((order.dir == OrderDir::Bid) ?
//                           _order.deal_price+5*step :
//                           _order.deal_price-5*step);

        order_id = async_add_order(ctx, order, yield);

        pt::ptime t2;
        std::vector<Order> res;

        auto async_wait_deal = [&]() {
            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
                try
                {
                    async2::WaitDeal wait_deal_2(ios(), ctx, order_id);
                    //                        res = wait_deal_2.async_wait(true, pt::seconds(90), yield);
                    res = wait_deal_2.async_wait(true, yield);
                    spawner.reset();
                }
                catch(const boost::system::system_error& e)
                {
                    if (e.code() == async2::Errc::MakeDealTimeout)
                    {
                        //                            spawner.reset();
                    }
                }
            });
        };

        async_wait_deal();

        DealType _type = ((order.dir == OrderDir::Bid) ?
                              DealType::Sell : DealType::Buy);
        double _price = ((order.dir == OrderDir::Bid) ?
                             _order.deal_price+25*step :
                             _order.deal_price-25*step);
        PriceIndicator price_indicator(ios(), m_isin_id, _type, _price);

        spawner.spawn([&](TradingContext& ctx, yield_context yield) {
            for(;;)
            {
                async2::Watcher<PriceIndicator> price_watcher(ios(), ctx, &price_indicator);
                price_watcher.async_wait(true, yield);
                Timer timer(ios(), ctx, pt::seconds(2));
                timer.async_wait(yield);
                if (price_indicator.value())
                {
                    spawner.reset();
                    break;
                }
            }
        });

        spawner.async_join(yield);
        spawner.spawn(SimpleClosing3(ios(), market(), m_isin_id));
        spawner.async_join(yield);
    }

    void run(atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        (*this)(ctx, yield);
    }

private:
    int64_t m_isin_id;
    atrade::OrderDir m_order_dir;
};


