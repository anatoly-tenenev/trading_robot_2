//#pragma once

//#include <atrade/atrade.hpp>
//#include <functions/functions.hpp>
//#include <strategies/common/simple_closing_2.hpp>
//#include <strategies/common/simple_closing_3.hpp>
//#include <indicators/common/price_indicator.hpp>
//#include <indicators/common/market_order_indicator.hpp>
//#include <indicators/common/step_indicator.hpp>
//#include <indicators/common/volume_indicator.hpp>
//#include <iostream>

//class ScalperStrategy4 : public atrade::async2::IStrategy
//{
//public:
//    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;

//    ScalperStrategy4(boost::asio::io_service* ios, atrade::async2::TradingContext* ctx, int64_t isin_id) :
//        m_ios(ios),
//        m_ctx(ctx),
//        m_isin_id(isin_id),
//        m_profit_trades(0),
//        m_loss_trades(0),
//        break_1(0),
//        break_2(0),
//        break_3(0),
//        m_executed(false),
//        m_max_price(0)
//    {
//    }

//    ScalperStrategy4(boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, int64_t isin_id) :
//        ScalperStrategy4(&ios, &ctx, isin_id)
//    {
//    }

//    ScalperStrategy4(int64_t isin_id) :
//        ScalperStrategy4(nullptr, nullptr, isin_id)
//    {
//    }

//    ~ScalperStrategy4()
//    {
//        if (!m_executed)
//        {
//            return;
//        }
//        if (m_profits.size() > 0 || m_losses.size() > 0)
//        {
//            std::cout << "profit trades=" << m_profits.size()
//                      << " loss trades=" << m_losses.size()
//                      << " rel=" << static_cast<double>(m_losses.size())*100/(m_losses.size()+m_profits.size()) << "%" << "\n";
//            std::cout << "avg_profit=" << functions::mean(m_profits, m_profits.size())
//                      << " avg_loss=" << functions::mean(m_losses, m_losses.size()) << "\n";
//            std::cout << "profit=" << atrade::calc_profit(m_orders) << "\n";
//        }
//        std::cout << "break_1=" << break_1 << " break_2=" << break_2
//                  << " break_3=" << break_3 << "\n";
//        std::cout << "max_price=" << m_max_price << "\n";
//    }

//    void operator() (boost::asio::yield_context yield)
//    {
//        (*this)(*m_ios, *m_ctx, yield);
//    }

//    void operator() (boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
//    {
//        using namespace atrade;
//        using namespace functions;

//        using asio::io_service;
//        using async2::TradingContext;
//        using asio::yield_context;
//        using async2::Timer;
//        using Watcher = async2::Watcher<async2::BooleanIndicator>;

//        using asio::io_service;
//        using async2::TradingContext;
//        using asio::yield_context;
//        using async2::Timer;
//        using Watcher = async2::Watcher<async2::BooleanIndicator>;

//        using TimerPtr = std::unique_ptr<Timer>;
//        using WatcherPtr = std::unique_ptr<Watcher>;

//        m_executed = true;

//        atrade::Security security = async2::get_security(ios, ctx, m_isin_id, yield);
//        double step = security.step();

//        auto subscr = async2::subscribe_order(ios, [&](const Order& order) {
//            if (order.isin_id == m_isin_id)
//            {
//                m_orders.push_back(order);
//            }
//        });

//        int i = 0;
//        int loss_trades = 0;
//        int mul = 1;

//        pt::time_duration td = pt::seconds(100);
//        DealType deal_type = DealType::Undefined;
//        DealType g_deal_type = DealType::Undefined;
//        bool is_pos = false;

//        auto _ts = async2::subscribe_tick(ios, [&](const atrade::Deal tick) {
//            m_max_price = std::max(m_max_price, tick.price);
//        });

////        std::unordered_map<int64_t, std::vector<Quote>> quotes;
////        auto ss1 = async2::subscribe_stock(ios, [&](int64_t isin_id, const std::vector<Quote>& stocks) {
////                quotes[isin_id] = stocks;
////        });
////        auto ss2 = async2::subscribe_stock(ios, [&](int64_t isin_id, const std::vector<Quote>& stocks) {
////                quotes[isin_id] = stocks;
////        });

//        DealType glob_dir = DealType::Undefined;

//        MarketOrderIndicator gmoi(ios, m_isin_id, pt::seconds(90), 2);

//        auto glob_dir_subscr = gmoi.subscribe([&](DealType type) {
//                glob_dir = type;
//        });

//        auto print_dir = [&](DealType type) {
//            switch (type)
//            {
//            case DealType::Undefined:
//                return "undefined";
//            case DealType::Buy:
//                return "buy";
//            case DealType::Sell:
//                return "sell";
//            default:
//                return "undefined";
//            }
//        };

//        async2::Spawner spawner(ios, ctx);
//        while (true)
//        {
//            async2::Market market(ios, spawner.ctx());

////            MarketOrderIndicator market_order_indicator(ios, m_isin_id, 300, pt::seconds(5));
////            auto mo_subscr = market_order_indicator.subscribe([&](DealType type) {
////                if (type != DealType::Undefined)
////                {
////                    std::cout << "Yo!!!: " << async2::current_datetime(ios) << "\n";
////                }
////            });

////            if (g_deal_type == DealType::Undefined)
////            {
////                if (td > pt::seconds(2))
////                {
////                    async2::Watcher<MarketOrderIndicator> mo_watcher(ios, ctx, &market_order_indicator);
////                    deal_type = mo_watcher.async_wait_not(DealType::Undefined, yield);
////                }
////            }
////            else
////            {
////                deal_type = g_deal_type;
////                g_deal_type = DealType::Undefined;
////            }

//            std::vector<pt::ptime> ttimes;
//            std::vector<int> buys, sells;

//            deal_type = DealType::Undefined;

//            bool should_continue = true;
//            for (int i=0; i<3; ++i)
//            {
//                MarketOrderIndicator moi(ios, m_isin_id, 200, pt::seconds(5));
//                async2::Watcher<MarketOrderIndicator> mo_watcher(ios, ctx, &moi);
//                DealType dt;
//                if (deal_type == DealType::Undefined)
//                {
//                    dt = mo_watcher.async_wait_not(DealType::Undefined, yield);
//                }
//                else
//                {
//                    boost::system::error_code errc;
//                    dt = mo_watcher.async_wait_not(DealType::Undefined, pt::seconds(5), yield[errc]);
//                    if (errc)
//                    {
//                        should_continue = false;
//                        break;
//                    }
//                }
//                ttimes.push_back(async2::current_datetime(ios));
//                buys.push_back(moi.buys());
//                sells.push_back(moi.sells());
//                if (deal_type == DealType::Undefined)
//                {
//                    deal_type = dt;
//                }
//                else if (deal_type != dt)
//                {
//                    should_continue = false;
//                    break;
//                }
//            }
//            if (!should_continue)
//            {
//                continue;
//            }

//            boost::system::error_code errc;

////            double cp = functions::center_price(ios, m_isin_id);

////            double buys = 0;
////            double sells = 0;
////            auto suscr2 = async2::subscribe_ordlog(ios, [&] (int64_t isin_id, const OrdLogEntry& entry) {
////                if (m_isin_id != isin_id)
////                {
////                    return;
////                }
////                bool is_add = ((entry.flags & atrade::OrdLogFlags::Add) != 0);
////                if (!is_add)
////                {
////                    return;
////                }
////                if (std::abs(cp - entry.price) > 20*step)
////                {
////                    return;
////                }
////                bool is_buy = ((entry.flags & atrade::OrdLogFlags::Buy) != 0);
////                if (is_buy)
////                {
////                    buys += entry.amount;
////                }
////                else
////                {
////                    sells += entry.amount;
////                }
////            });

////            Timer timer1(ios, ctx, pt::seconds(4));
////            timer1.async_wait(yield);

////            DealType deal_type1;

////            if (0.2*buys > sells)
////            {
////                deal_type1 = DealType::Buy;
////            }
////            else if (0.2*sells > buys)
////            {
////                deal_type1 = DealType::Sell;
////            }
////            else
////            {
////                continue;
////            }

////            if (deal_type != deal_type1)
////            {
////                continue;
////            }

//            OrderDir order_dir = (deal_type == DealType::Buy) ? OrderDir::Bid : OrderDir::Ask;

////            OrderBook orderbook(ios, m_isin_id);

////            int qs = 4;

////            double b1 = orderbook.best_bid();
////            double b2 = atrade::price_round(b1 - qs*step);

////            double a1 = orderbook.best_ask();
////            double a2 = atrade::price_round(a1 + qs*step);

////            if (order_dir == OrderDir::Bid)
////            {
////                if ((orderbook.bids(b1, b2)*0.7 < orderbook.asks(a1, a2)) ||
////                    (orderbook.bids(b1, b2) < 200))
////                {
////                    break_1++;
////                    continue;
////                }
////            }
////            else
////            {
////                if ((orderbook.bids(b1, b2) > 0.7*orderbook.asks(a1, a2)) ||
////                    (orderbook.asks(a1, a2) < 200))
////                {
////                    break_1++;
////                    continue;
////                }
////            }

////            Timer o_timer(ios, ctx, pt::seconds(3));
////            o_timer.async_wait(yield);

//            double orel;

//            OrderBook orderbook2(ios, m_isin_id);

//            int dist = 10;

////            if (order_dir == OrderDir::Bid)
////            {
////                orel = double(orderbook2.bids(dist))/orderbook2.asks(dist);
////            }
////            else
////            {
////                orel = double(orderbook2.asks(dist))/orderbook2.bids(dist);
////            }


////            if (order_dir == OrderDir::Bid)
////            {
////                if (orderbook2.bid_d(15) < orderbook2.ask_d(15))
////                {
////                    break_2++;
////                    continue;
////                }
////            }
////            else
////            {
////                if (orderbook2.bid_d(15) > orderbook2.ask_d(15))
////                {
////                    break_2++;
////                    continue;
////                }
////            }

////            if (order_dir == OrderDir::Bid)
////            {
////                if ((orderbook2.bid_d(4)*0.7 < orderbook2.ask_d(5)) &&
////                    (orderbook2.bid_d(4) < 100))
////                {
////                    break_2++;
////                    continue;
////                }
////            }
////            else
////            {
////                if ((orderbook2.bid_d(5) > 0.7*orderbook2.ask_d(4)) &&
////                    (orderbook2.ask_d(4) < 100))
////                {
////                    break_2++;
////                    continue;
////                }
////            }

////            if (order_dir == OrderDir::Bid)
////            {
////                if (orderbook.bids(b1, b2) > 0.6*orderbook2.bids(orderbook2.best_bid(), b2))
////                {
////                    break_3++;
////                    continue;
////                }
////            }
////            else
////            {
////                if (orderbook.asks(a1, a2) > 0.6*orderbook2.asks(orderbook2.best_ask(), a2))
////                {
////                    break_3++;
////                    continue;
////                }
////            }

////            functions::print_stocks(ios, m_isin_id);

////            std::cout << orderbook.asks(a1, a2) << " " << orderbook.bids(b1, b2) << " ";

////            std::cout << ((deal_type == DealType::Buy) ? "Buy" : "Sell");


////            StepIndicator step_indictor(ios, m_isin_id, 5);
////            async2::Watcher<StepIndicator> s_watcher(ios, ctx, &step_indictor);
////            double p1 = functions::center_price(ios, m_isin_id);
////            deal_type = s_watcher.async_wait_not(DealType::Undefined, pt::seconds(3), yield[errc]);
////            if (errc)
////            {
////                continue;
////            }
////            double p2 = functions::center_price(ios, m_isin_id);
////            //std::cout << "delta price = " << p2 - p1 << "\n";

//            OrderBook orderbook3(ios, m_isin_id);

//            Order order;
//            int order_id;

//            std::vector<Order> __orders;
//            Order _order;

//            auto t1 = async2::current_datetime(ios);

//            order = Order(m_isin_id,
//                          order_dir,
//                          (order_dir == OrderDir::Bid) ?
//                              orderbook3.best_bid() - 1*step :
//                              orderbook3.best_ask() + 1*step,
//                          1);
//            order_id = market.async_add_order(order, yield);
//            async2::WaitDeal wait_deal_1(ios, ctx, order_id);
//            __orders = wait_deal_1.async_wait(true, pt::seconds(10), yield[errc]);
//            if (errc)
//            {
//                spawner.spawn(SimpleClosing3(market.market(), m_isin_id));
//                spawner.async_join(yield);
//                continue;
//            }
//            _order = __orders.front();

//            if (order_dir == OrderDir::Bid)
//            {
//                orel = double(orderbook2.bids(dist))/orderbook2.asks(dist);
//            }
//            else
//            {
//                orel = double(orderbook2.asks(dist))/orderbook2.bids(dist);
//            }

//            int64_t did1 = _order.deal_id, did2;
//            double dp1 = _order.deal_price, dp2;

////            std::cout << " deal_price = " << _order.deal_price;

////            MarketOrderIndicator market_order_indicator_2(ios, m_isin_id, 10, pt::seconds(5));
////            async2::Watcher<MarketOrderIndicator> mo_watcher_2(ios, ctx, &market_order_indicator_2);
////            mo_watcher_2.async_wait_not(!deal_type, pt::seconds(10), yield[errc]);

////            Timer timer(ios, ctx, pt::seconds(20));
////            timer.async_wait(yield);

//            const int max_volume = 50;

//            order.dir = !order.dir;
//            order.type = atrade::OrderType::Limit;
//            order.price = find_price(ios, m_isin_id, order.dir, max_volume);
//            order_id = market.async_add_order(order, yield);

////            PriceIndicator price_indicator(ios,
////                                           m_isin_id,
////                                           !deal_type,
////                                           (deal_type == DealType::Buy) ?
////                                               _order.deal_price-5*step :
////                                               _order.deal_price+5*step);

////            spawner.spawn([&](io_service& ios, TradingContext& ctx, yield_context yield) {
////                async2::Watcher<PriceIndicator> watcher(ios, ctx, &price_indicator);
////                watcher.async_wait(yield[errc]);
////                spawner.reset();
////            });

//            bool is_init = false;
//            double bad_price = 0;

//            auto _ts = async2::subscribe_tick(ios, [&](const atrade::Deal tick) {
//                if (!bad_price)
//                {
//                    bad_price = tick.price;
//                    is_init = true;
//                }
//                if (order.dir == OrderDir::Bid && tick.type == DealType::Sell)
//                {
//                    bad_price = std::max(bad_price, tick.price);
//                }
//                else if (order.dir == OrderDir::Ask && tick.type == DealType::Buy)
//                {
//                    bad_price = std::min(bad_price, tick.price);
//                }
//            });

//            bool _opp = false;

//            MarketOrderIndicator market_order_indicator2(ios, m_isin_id, 400, pt::seconds(5));

//            auto _mo_subscr = market_order_indicator2.subscribe([&](DealType type) {
//                    if (type == !deal_type)
//                    {
//                        _opp = true;
//                    }
//            });

//            pt::ptime t2;
//            std::vector<Order> res;

////            spawner.spawn([&](io_service& ios, TradingContext& ctx, yield_context yield) {
////                async2::Watcher<MarketOrderIndicator> mo_watcher(ios, ctx, &market_order_indicator2);
////                g_deal_type = mo_watcher.async_wait(!deal_type, yield);
////                spawner.reset();
////                t2 = async2::current_datetime(ios);
////            });

//            auto async_wait_deal = [&]() {
//                spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                    try
//                    {
//                        async2::WaitDeal wait_deal_2(ios, ctx, order_id);
//                        res = wait_deal_2.async_wait(true, pt::seconds(15), yield);
//                        spawner.reset();
//                    }
//                    catch(...)
//                    {
//                    }
//                }, [&](boost::system::error_code errc) {
//                    if (!res.empty())
//                    {
//                        did2 = res.front().deal_id;
//                        dp2 = res.front().deal_price;
//                    }
//                });
//            };

//            async_wait_deal();

//            int move_count = 0;

//            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                for (;;move_count++)
//                {
//                    VolumeIndicator vindicator(ios, m_isin_id, order.dir, order.price, max_volume);
//                    async2::Watcher<VolumeIndicator> watcher(ios, ctx, &vindicator);
//                    bool res = watcher.async_wait(yield);
//                    if (res)
//                    {
//                        order.price = find_price(ios, m_isin_id, order.dir, max_volume);
////                        std::cout << "price=" << order.price << "\n";
//                        order_id = market.async_move_order(order_id, order.price, yield);
//                        async_wait_deal();
//                    }
//                }
//            });

//            DealType _type = ((order.dir == OrderDir::Bid) ?
//                                  DealType::Sell : DealType::Buy);
//            double _price = ((order.dir == OrderDir::Bid) ?
//                                 _order.deal_price+10*step :
//                                 _order.deal_price-10*step);
//            PriceIndicator price_indicator(ios, m_isin_id, _type, _price);

//            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                Timer timer(ios, ctx, pt::seconds(5));
//                timer.async_wait(yield);
//                spawner.reset();
//            });

//            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                for(;;)
//                {
//                    async2::Watcher<PriceIndicator> price_watcher(ios, ctx, &price_indicator);
//                    price_watcher.async_wait(true, yield);
//                    Timer timer(ios, ctx, pt::seconds(2));
//                    timer.async_wait(yield);
//                    if (price_indicator.value())
//                    {
//                        break;
//                    }
//                }
//                spawner.reset();
//            });

//            spawner.async_join(yield);

//            t2 = async2::current_datetime(ios);

////            std::cout << "move_count=" << move_count << "\n";

////            spawner.spawn(SimpleClosing2(m_isin_id));
////            spawner.async_join(yield);

//            spawner.reset();
//            spawner.spawn(SimpleClosing3(market.market(), m_isin_id));
//            spawner.async_join(yield);

////            if (errc)
////            {
////                spawner.reset();
////                spawner.spawn(SimpleClosing2(m_isin_id));
////                spawner.async_join(yield);
////            }

//            auto _orders = market.market().history().get_orders_by_isin_id(m_isin_id);

//            ++i;
//            double cur_profit = atrade::calc_profit(_orders);
//            if (cur_profit > 0)
//            {
//                is_pos = true;
//                td = t2 - t1;
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
//                is_pos = false;
//                td = pt::seconds(100);
//                m_losses.push_back(cur_profit);
//            }

//            for (int i=0; i<3; i++)
//            {
//                std::cout << ttimes[i]
//                          << " buys=" << buys[i]
//                          << " sells=" << sells[i] << "\n";
//            }

//            std::cout << t1 << " "
//                      << ((deal_type == DealType::Buy) ? "buy" : "sell")
//                      << " cur_profit=" << cur_profit
//                      << " orders=" << _orders.size()
//                      << " glob=" << print_dir(glob_dir)
////                      << " orel=" << orel
////                      << " did1=" << did1
////                      << " bp1=" << dp1
////                      << " did2=" << did2
////                      << " bp2=" << dp2
////                      << " init=" << is_init
////                      << " bad_delta=" << std::abs(bad_price - order.price)
////                      << " opp=" << _opp
//                      << " " << t2 - t1 <<"\n";
//        }
//    }

//    void run(boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
//    {
//        (*this)(ios, ctx, yield);
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
//    int break_1;
//    int break_2;
//    int break_3;
//    bool m_executed;
//    double m_max_price;
//};


