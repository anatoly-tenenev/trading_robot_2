#pragma once

#include <atrade/atrade.hpp>
#include <functions/functions.hpp>
#include <strategies/common/all.hpp>
#include <iostream>

class LoopStrategy : public atrade::async2::Strategy
{
public:
    typedef std::shared_ptr<atrade::async2::BooleanIndicator> IndicatorPtr;

    LoopStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket* market, int64_t isin_id) :
        Strategy(ios, market),
        m_isin_id(isin_id)
    {
    }

    LoopStrategy(boost::asio::io_service& ios, atrade::async2::IAsyncMarket& market, int64_t isin_id) :
        LoopStrategy(ios, &market, isin_id)
    {
    }

    LoopStrategy(boost::asio::io_service& ios, int64_t isin_id) :
        LoopStrategy(ios, nullptr, isin_id)
    {
    }

    ~LoopStrategy()
    {
    }

    void operator() (atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        using namespace atrade;

        async2::Spawner spawner(ios(), ctx);

        CalmStrategy calm_strata{ ios(), m_isin_id};
//        CalmOppStrategy calm_strata{ ios(), m_isin_id};
//        MoWatcherStrategy calm_strata{ ios(), m_isin_id };

        while (true)
        {
            calm_strata(ctx, yield);
        }
    }

    void run(atrade::async2::TradingContext& ctx, boost::asio::yield_context yield)
    {
        (*this)(ctx, yield);
    }

private:
    int64_t m_isin_id;
};


