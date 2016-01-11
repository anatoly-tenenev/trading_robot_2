//#pragma once

//#include <atrade/atrade.hpp>
//#include <functions/functions.hpp>
//#include <strategies/common/simple_closing_2.hpp>
//#include <indicators/common/price_indicator.hpp>
//#include <indicators/common/market_order_indicator.hpp>
//#include <indicators/common/step_indicator.hpp>
//#include <iostream>

//class ResearchStrategy
//{
//public:
//    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;

//    ResearchStrategy(boost::asio::io_service* ios, atrade::async2::TradingContext* ctx, int64_t isin_id) :
//        m_ios(ios),
//        m_ctx(ctx),
//        m_isin_id(isin_id)
//    {
//    }

//    ResearchStrategy(boost::asio::io_service& ios, atrade::async2::TradingContext& ctx, int64_t isin_id) :
//        ResearchStrategy(&ios, &ctx, isin_id)
//    {
//    }

//    ResearchStrategy(int64_t isin_id) :
//        ResearchStrategy(nullptr, nullptr, isin_id)
//    {
//    }

//    ~ResearchStrategy()
//    {
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

//        while (true)
//        {
//            MarketOrderIndicator market_order_indicator1(ios, m_isin_id, 1000, pt::seconds(5));
//            MarketOrderIndicator market_order_indicator2(ios, m_isin_id, 100, pt::seconds(5));
//            async2::Watcher<MarketOrderIndicator> mo_watcher1(ios, ctx, &market_order_indicator1);
//            async2::Watcher<MarketOrderIndicator> mo_watcher2(ios, ctx, &market_order_indicator2);
//            DealType dt = mo_watcher1.async_wait_not(DealType::Undefined, yield);
//            boost::system::error_code errc;
//            mo_watcher2.async_wait(dt, pt::seconds(10), yield[errc]);
//            if (!errc)
//            {
//                std::cout << "Yo!!!: " << async2::current_datetime(ios) << "\n";
//            }
//        }
//    }

//private:
//    boost::asio::io_service* m_ios;
//    atrade::async2::TradingContext* m_ctx;
//    int64_t m_isin_id;
//};


