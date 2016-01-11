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

class MoWatcherStrategy : public atrade::async2::Strategy
{
public:
    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;
    typedef std::function<void(const atrade::Order&)> OnLossHandler;

    MoWatcherStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket* market, int64_t isin_id) :
        Strategy(ios, market),
        m_isin_id(isin_id)
    {
    }

    MoWatcherStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket& market, int64_t isin_id) :
        MoWatcherStrategy(ios, &market, isin_id)
    {
    }

    MoWatcherStrategy(boost::asio::io_service& ios, int64_t isin_id) :
        MoWatcherStrategy(ios, nullptr, isin_id)
    {
    }

    ~MoWatcherStrategy()
    {
        std::cout << "success_start=" << m_success_start
                  << " fail_start=" << m_fail_start
                  << " success_trade=" << m_success_trade
                  << " fail_trade=" << m_fail_trade << "\n";
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

        DealType deal_type = DealType::Undefined;

        bool should_continue = true;
        for (int i=0; i<3; ++i)
        {
            MarketOrderIndicator moi(ios(), m_isin_id, 200, pt::seconds(5));
            async2::Watcher<MarketOrderIndicator> mo_watcher(ios(), ctx, &moi);
            DealType dt;
            if (deal_type == DealType::Undefined)
            {
                dt = mo_watcher.async_wait_not(DealType::Undefined, yield);
            }
            else
            {
                boost::system::error_code errc;
                dt = mo_watcher.async_wait_not(DealType::Undefined, pt::seconds(5), yield[errc]);
                if (errc)
                {
                    should_continue = false;
                    break;
                }
            }
            if (deal_type == DealType::Undefined)
            {
                deal_type = dt;
            }
            else if (deal_type != dt)
            {
                should_continue = false;
                break;
            }
        }
        if (!should_continue)
        {
            return;
        }

        boost::system::error_code errc;

        OrderDir order_dir = (deal_type == DealType::Buy) ? OrderDir::Bid : OrderDir::Ask;

        OrderBook orderbook3(ios(), m_isin_id);

        Order order;
        int order_id;

        std::vector<Order> __orders;
        Order _order;

        order = Order(m_isin_id,
                      order_dir,
                      (order_dir == OrderDir::Bid) ?
                          orderbook3.best_bid() - 1*step :
                          orderbook3.best_ask() + 1*step,
//                          orderbook3.best_ask() - 1*step :
//                          orderbook3.best_bid() + 1*step,
                      1);
        order_id = async_add_order(ctx, order, yield);
        async2::WaitDeal wait_deal_1(ios(), ctx, order_id);
        __orders = wait_deal_1.async_wait(true, pt::seconds(5), yield[errc]);
        if (errc)
        {
            ++m_fail_start;
            spawner.spawn(SimpleClosing3(ios(), market(), m_isin_id));
            spawner.async_join(yield);
            return;
        }
        ++m_success_start;
        _order = __orders.front();

        const int max_volume = 50;

        order.dir = !order.dir;
        order.type = atrade::OrderType::Limit;
        order.price = find_price(ios(), m_isin_id, order.dir, max_volume);
//        order.price = ((order.dir == OrderDir::Bid) ?
//                           _order.deal_price+1*step :
//                           _order.deal_price-1*step);
        order_id = async_add_order(ctx, order, yield);

        auto async_wait_deal = [&]() {
            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
                try
                {
                    async2::WaitDeal wait_deal_2(ios(), ctx, order_id);
//                    wait_deal_2.async_wait(true, pt::seconds(15), yield);
                    wait_deal_2.async_wait(true, yield);
                    ++m_success_trade;
                    spawner.reset();
                }
                catch(...)
                {
                }
            });
        };

        async_wait_deal();

        int move_count = 0;

        spawner.spawn([&](TradingContext& ctx, yield_context yield) {
            for (;;++move_count)
            {
                VolumeIndicator vindicator(ios(), m_isin_id, order.dir, order.price, max_volume);
                async2::Watcher<VolumeIndicator> watcher(ios(), ctx, &vindicator);
                bool res = watcher.async_wait(yield);
                if (res)
                {
                    order.price = find_price(ios(), m_isin_id, order.dir, max_volume);
//                        std::cout << "price=" << order.price << "\n";
                    order_id = async_move_order(ctx, order_id, order.price, yield);
                    async_wait_deal();
                }
            }
        });

        DealType _type = ((order.dir == OrderDir::Bid) ?
                              DealType::Sell : DealType::Buy);
        double _price = ((order.dir == OrderDir::Bid) ?
//                             _order.deal_price+10*step :
//                             _order.deal_price-10*step);
                             _order.deal_price+25*step :
                             _order.deal_price-25*step);
        PriceIndicator price_indicator(ios(), m_isin_id, _type, _price);

//        spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//            Timer timer(ios(), ctx, pt::seconds(5));
//            timer.async_wait(yield);
//            spawner.reset();
//        });

        spawner.spawn([&](TradingContext& ctx, yield_context yield) {
            for(;;)
            {
                async2::Watcher<PriceIndicator> price_watcher(ios(), ctx, &price_indicator);
                price_watcher.async_wait(true, yield);
                Timer timer(ios(), ctx, pt::seconds(2));
                timer.async_wait(yield);
                if (price_indicator.value())
                {
                    ++m_fail_trade;
                    spawner.reset();
                    if (m_on_loss)
                    {
                        m_on_loss(order);
                    }
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

    void on_loss(const OnLossHandler& handler)
    {
        m_on_loss = handler;
    }

private:
    int64_t m_isin_id;
    OnLossHandler m_on_loss;
    int m_success_start = 0;
    int m_fail_start = 0;
    int m_success_trade = 0;
    int m_fail_trade = 0;
};


