#ifndef FAKE_MARKET_DATA_SOURCE_H
#define FAKE_MARKET_DATA_SOURCE_H

#include <atrade/backtester/marketdatasourcebase.h>

using namespace atrade;
using namespace atrade::backtester;

class FakeMarketDataSource : public backtester::MarketDataSourceBase
{
public:
    FakeMarketDataSource() :
        MarketDataSourceBase()
    {
        m_stocks[0] = [&]() {
            return m_quotes[0];
        };
    }

    std::vector<Security> securities() const
    {
        return std::vector<Security>({Security(0, "UnitTestCode", "UnitTestCode", 10.0)});
    }

    pt::ptime datetime() const
    {
        return pt::min_date_time;
    }

    void process()
    {
    }

    void send_tick(const BDeal& tick)
    {
        m_tick_handler(tick);
    }

    void send_stocks(int64_t isin_id, const std::vector<BQuote>& stocks)
    {
        m_stock_handler(isin_id, stocks);
        m_quotes[isin_id] = stocks;
    }

private:
    atrade::umap<int64_t, std::vector<BQuote>> m_quotes;
};

#endif // FAKE_MARKET_DATA_SOURCE_H
