#pragma once

#include <atrade/atrade.hpp>
#include <functions/functions.hpp>
#include <strategies/common/all.hpp>
#include <iostream>

class CalmOppStrategy : public atrade::async2::Strategy
{
public:
    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;

    CalmOppStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket* market, int64_t isin_id) :
        Strategy(ios, market),
        m_isin_id(isin_id)
    {
    }

    CalmOppStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket& market, int64_t isin_id) :
        CalmOppStrategy(ios, &market, isin_id)
    {
    }

    CalmOppStrategy(boost::asio::io_service& ios, int64_t isin_id) :
        CalmOppStrategy(ios, nullptr, isin_id)
    {
    }

    ~CalmOppStrategy()
    {
    }

    void operator() (atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        using namespace atrade;

        async2::Spawner spawner(ios(), ctx);

        ctx.on_error([&](const system::system_error& e) {
            if (e.code() == async2::Errc::AddOrderReject)
            {
                spawner.reset();
                spawner.spawn(SimpleClosing3{ ios(), market(), m_isin_id });
            }
            else
            {
                throw e;
            }
        });

        async2::IAsyncMarket* pmarket = (market() ? market() : &async2::get_market(ios()));
        async2::AsyncMarket cs_market{*pmarket};
        async2::AsyncMarket st_market{*pmarket};

        CalmStrategy calm_strata{ ios(), cs_market, m_isin_id };
//        MoWatcherStrategy calm_strata{ ios(), cs_market, m_isin_id };
        calm_strata.on_loss([&](const Order& order) {
            SimpleTradeStrategy simple_trade{ ios(), st_market, m_isin_id, order.dir };
            spawner.spawn(simple_trade);
        });
        calm_strata(ctx, yield);

//        spawner.async_join(yield);
//        spawner.spawn(SimpleClosing3(market.market(), m_isin_id));
        spawner.async_join(yield);
    }

    void run(atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        (*this)(ctx, yield);
    }

private:
    int64_t m_isin_id;
};


