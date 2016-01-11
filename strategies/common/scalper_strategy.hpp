//#pragma once

//#include <atrade/atrade.hpp>
//#include <functions/functions.hpp>
//#include <strategies/common/simple_closing_2.hpp>
//#include <indicators/common/price_indicator.hpp>
//#include <indicators/common/market_order_indicator.hpp>
//#include <iostream>

//class ScalperStrategy
//{
//public:
//    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;

//    ScalperStrategy(boost::asio::io_service* ios, atrade::async2::TradingContext* ctx, int64_t isin_id) :
//        m_ios(ios),
//        m_ctx(ctx),
//        m_isin_id(isin_id),
//        m_profit_trades(0),
//        m_loss_trades(0)
//    {
//    }

//    ScalperStrategy(boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, int64_t isin_id) :
//        ScalperStrategy(&ios, &ctx, isin_id)
//    {
//    }

//    ScalperStrategy(int64_t isin_id) :
//        ScalperStrategy(nullptr, nullptr, isin_id)
//    {
//    }

//    ~ScalperStrategy()
//    {
//        if (m_profit_trades > 0 || m_loss_trades > 0)
//        {
//            std::cout << "profit trades=" << m_profit_trades
//                      << " loss trades=" << m_loss_trades
//                      << " rel=" << static_cast<double>(m_loss_trades)*100/(m_loss_trades+m_profit_trades) << "%" << "\n";
//            std::cout << "check profit trades=" << m_profits.size()
//                      << " loss trades=" << m_losses.size()
//                      << " rel=" << static_cast<double>(m_losses.size())*100/(m_losses.size()+m_profits.size()) << "%" << "\n";
//            std::cout << "avg_profit=" << functions::mean(m_profits, m_profits.size())
//                      << " avg_loss=" << functions::mean(m_losses, m_losses.size()) << "\n";
//            std::cout << "profit=" << atrade::calc_profit(m_orders) << "\n";
//        }
//    }

//    void operator() (boost::asio::yield_context yield)
//    {
//        (*this)(*m_ios, *m_ctx, yield);
//    }

//    void operator() (boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
//    {
//        using namespace atrade;

//        using asio::io_service;
//        using async2::TradingContext;
//        using asio::yield_context;
//        using async2::Timer;
//        using Watcher = async2::Watcher<async2::BooleanIndicator>;

//        using TimerPtr = std::unique_ptr<Timer>;
//        using WatcherPtr = std::unique_ptr<Watcher>;

//        atrade::Security security = async2::get_security(ios, ctx, m_isin_id, yield);

//        auto subscr = async2::subscribe_order(ios, [&](const Order& order) {
//            if (order.isin_id == m_isin_id)
//            {
//                m_orders.push_back(order);
//            }
//        });

//        MarketOrderIndicator market_order_indicator(ios, m_isin_id, 1000, pt::seconds(5));
//        auto mo_subscr = market_order_indicator.subscribe([&](DealType type) {
//            if (type != DealType::Undefined)
//            {
//                std::cout << "Yo!!!: " << async2::current_datetime(ios) << "\n";
//            }
//        });

//        int i = 0;
//        int loss_trades = 0;
//        int mul = 1;

//        async2::Spawner spawner(ios, ctx);
//        while (true)
//        {
//            std::vector<Order> _orders;
//            auto subscr = async2::subscribe_order(ios, [&](const Order& order) {
//                if (order.isin_id == m_isin_id)
//                {
//                    _orders.push_back(order);
//                }
//            });
////            std::cout << "\n#" << i << " " << async2::current_datetime(ios) << "\n";
//            auto stocks = async2::get_market_data(ios).stocks(m_isin_id);
//            double step = security.step();
//            double price = functions::center_price(stocks, step);
//            price = price_round(price + mul*1*step);
//            double bid_price = price;
//            double ask_price = price_round(price + 1*step);
//            Order orders[] = {Order(m_isin_id, OrderDir::Bid, bid_price, 1),
//                              Order(m_isin_id, OrderDir::Ask, ask_price, 1)};
//            size_t distance = 3;
//            PriceIndicator sell_indicator(ios, m_isin_id, atrade::DealType::Sell, ask_price - distance*step);
//            PriceIndicator buy_indicator(ios, m_isin_id, atrade::DealType::Buy, bid_price + distance*step);
//            async2::AnyIndicator price_indicator({&sell_indicator, &buy_indicator});
//            async2::Watcher<async2::AnyIndicator> watcher(ios, ctx, &price_indicator);
////            std::vector<WatcherPtr> watchers;
////            watchers.emplace_back(std::make_unique<Watcher>(ios, ctx, &sell_indicator));
////            watchers.emplace_back(std::make_unique<Watcher>(ios, ctx, &buy_indicator));
//            std::vector<TimerPtr> timers;
//            timers.emplace_back(std::make_unique<Timer>(ios, ctx, pt::minutes(3)));
//            timers.emplace_back(std::make_unique<Timer>(ios, ctx, pt::minutes(3)));
//            for (int i = 0; i < 2; ++i)
//            {
//                Order& order = orders[i];
////                Watcher& watcher = *watchers[i];
//                Timer& timer = *timers[i];
//                spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                    int64_t order_id = async2::async_add_order(ios, ctx, order, yield);
//                    async2::WaitDeal wait_deal(ios, ctx, order_id);
//                    wait_deal.async_wait(yield);
//                    timer.async_wait([&](boost::system::error_code errc) {
//                        if (!errc)
//                        {
//                            spawner.cancel_joins();
//                        }
//                    });
//                    watcher.async_wait([&](boost::system::error_code errc, bool value) {
//                        if (!errc && value)
//                        {
//                            spawner.cancel_joins();
//                        }
//                    });
//                });
//            }
//            try
//            {
//                spawner.async_join(yield);
//                timers.clear();
////                watchers.clear();
//                watcher.cancel();
//                loss_trades = 0;
//                ++m_profit_trades;
//                async2::Portfolio& portfolio = async2::get_portfolio(ios);
////                auto stocks = market.market_data().stocks(m_isin_id);
////                std::cout << "success\n";
////                std::cout << "bid=" << functions::best_price(QuoteType::Bid, stocks)
////                          << " ask=" << functions::best_price(QuoteType::Ask, stocks) << "\n";
////                std::cout << "profit=" << portfolio.profit(m_isin_id) << "\n";
//            }
//            catch (boost::system::system_error&)
//            {
//                timers.clear();
//                watcher.cancel();
//                ++loss_trades;
//                if (loss_trades > 2)
//                {
//                    mul *= -1;
//                    loss_trades = 0;
//                }
//                ++m_loss_trades;
//                spawner.reset();
//                spawner.spawn(SimpleClosing2(m_isin_id));
//                spawner.async_join(yield);
////                async2::Portfolio portfolio(market);
////                auto stocks = market.market_data().stocks(m_isin_id);
////                std::cout << "fail\n";
////                std::cout << "bid=" << functions::best_price(QuoteType::Bid, stocks)
////                          << " ask=" << functions::best_price(QuoteType::Ask, stocks) << "\n";
////                std::cout << "profit=" << portfolio.profit(m_isin_id) << "\n";
//            }
//            ++i;
//            double cur_profit = atrade::calc_profit(_orders);
//            if (cur_profit > 0)
//            {
//                m_profits.push_back(cur_profit);
////                if (cur_profit > 10)
////                {
////                    std::cout << "BIG PROFIT: ";
////                    for (auto& order : _orders)
////                    {
////                        std::cout << order << " ";
////                    }
////                    std::cout << "\n";
////                }
//            }
//            else
//            {
//                m_losses.push_back(cur_profit);
//            }
//        }
//    }

//    template <typename TIndicator, typename ...Args>
//    void add_stop_indicator(Args&&... args)
//    {
//        m_stop_indicators.emplace_back(std::make_shared<TIndicator>(std::forward<Args>(args)...));
//    }

//private:
//    boost::asio::io_service* m_ios;
//    atrade::async2::TradingContext* m_ctx;
//    int64_t m_isin_id;
//    int m_profit_trades;
//    int m_loss_trades;
//    std::vector<double> m_profits;
//    std::vector<double> m_losses;
//    std::vector<atrade::Order> m_orders;
//    std::vector<IndicatorPtr> m_stop_indicators;
//};


