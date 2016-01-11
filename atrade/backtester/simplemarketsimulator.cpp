#include "simplemarketsimulator.h"
#include "exceptions.h"

namespace atrade {
namespace backtester {

SimpleMarketSimulator::SimpleMarketSimulator() :
    MarketSimulator()
{
}

IMarketDataSource& SimpleMarketSimulator::add_data_source(std::unique_ptr<IMarketDataSource>&& data_source)
{
    IMarketDataSource& ds = MarketSimulator::add_data_source(std::move(data_source));
    ds.tick_handler([this](const BDeal& tick) mutable {
        handle_tick(tick);
        if (m_tick_handler)
        {
            double step = m_securities.at(tick.isin_id).step();
            m_tick_handler(to_deal(tick, step));
        }
    });
    ds.stock_handler([this](int64_t isin_id, const std::vector<BQuote>& stocks) mutable {
        //handle_stocks(isin_id, stocks);
        if (m_stock_handler)
        {
            double step = m_securities.at(isin_id).step();
            m_stock_handler(isin_id, to_stocks(stocks, step));
        }
    });
    ds.ordlog_handler([this](int64_t isin_id, const BOrdLogEntry& entry) mutable {
        if (m_ordlog_handler)
        {
            double step = m_securities.at(isin_id).step();
            m_ordlog_handler(isin_id, to_ordlog_entry(entry, step));
        }
    });
    return ds;
}

void SimpleMarketSimulator::handle_tick(const BDeal& tick)
{
    if (tick.type == DealType::Buy)
    {
        _handle_tick<BuyDeal>(tick);
    }
    else if (tick.type == DealType::Sell)
    {
        _handle_tick<SellDeal>(tick);
    }
    else
    {
        throw UndefinedDealTypeException();
    }
}

void SimpleMarketSimulator::handle_stocks(int64_t isin_id, const std::vector<BQuote>& stocks)
{
    _handle_stocks<BidQuote>(isin_id, stocks);
    _handle_stocks<AskQuote>(isin_id, stocks);
}

}
}
