//#ifndef SIMPLE_RANDOM_CHOISE_2_HPP
//#define SIMPLE_RANDOM_CHOISE_2_HPP

//#include <atrade/atrade.hpp>
//#include <functions/functions.hpp>
//#include <strategies/common/simple_closing_2.hpp>
//#include <iostream>

//class SimpleRandomChoise2
//{
//public:
//    SimpleRandomChoise2(boost::asio::io_service* ios, atrade::async2::TradingContext* ctx, int64_t isin_id) :
//        m_ios(ios),
//        m_ctx(ctx),
//        m_isin_id(isin_id),
//        m_profit_trades(0),
//        m_loss_trades(0)
//    {
//    }

//    SimpleRandomChoise2(boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, int64_t isin_id) :
//        SimpleRandomChoise2(&ios, &ctx, isin_id)
//    {
//    }

//    SimpleRandomChoise2(int64_t isin_id) :
//        SimpleRandomChoise2(nullptr, nullptr, isin_id)
//    {
//    }

//    ~SimpleRandomChoise2()
//    {
//        std::cout << "profit trades: " << m_profit_trades << "\n";
//        std::cout << "loss trades: " << m_loss_trades << "\n";
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

//        atrade::Security security = get_security(ios, ctx, m_isin_id, yield);

//        int i = 0;

//        async2::Spawner spawner(ios, ctx);
//        while (true)
//        {
//            //std::cout << "\n#" << i << " " << async2::current_datetime(ios) << "\n";
//            auto stocks = async2::get_market_data(ios).stocks(m_isin_id);
//            double price = functions::center_price(stocks, security.step());
//            double step = security.step();
//            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                int64_t order_id = async2::async_add_order(ios, ctx, Order(m_isin_id, OrderDir::Bid, price, 1), yield);
//                async2::WaitDeal wait_deal(ios, ctx, order_id);
//                auto order = wait_deal.async_wait(yield);
////                std::cout << "deal bid=" << price
////                          << " deal_id=" << order[0].deal_id << "\n";
//            });
//            spawner.spawn([&](TradingContext& ctx, yield_context yield) {
//                int64_t order_id = async2::async_add_order(ios, ctx, Order(m_isin_id, OrderDir::Ask, price+step, 1), yield);
//                async2::WaitDeal wait_deal(ios, ctx, order_id);
//                auto order = wait_deal.async_wait(yield);
////                std::cout << "deal ask=" << price+step
////                          << " deal_id=" << order[0].deal_id << "\n";
//            });
//            try
//            {
//                spawner.async_join(pt::minutes(3), yield);
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
//        }
//    }

//private:
//    boost::asio::io_service* m_ios;
//    atrade::async2::TradingContext* m_ctx;
//    int64_t m_isin_id;
//    int m_profit_trades;
//    int m_loss_trades;
//};

//#endif // SIMPLE_RANDOM_CHOISE_2_HPP

