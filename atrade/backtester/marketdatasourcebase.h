#ifndef MARKETDATASOURCEBASE_H
#define MARKETDATASOURCEBASE_H

#include "imarketdatasource.h"

namespace atrade {
namespace backtester {

class MarketDataSourceBase : public IMarketDataSource
{
public:
    MarketDataSourceBase();
    fumap<int64_t, std::vector<BQuote>()> stocks() override;
    void ordlog_handler(const BOrdLogHandler& handler) override;
    void tick_handler(const BTickHandler& handler) override;
    void stock_handler(const BStockHandler& handler) override;

protected:
    fumap<int64_t, std::vector<BQuote>()> m_stocks;
    BOrdLogHandler m_ordlog_handler;
    BTickHandler m_tick_handler;
    BStockHandler m_stock_handler;
};

}
}

#endif // MARKETDATASOURCEBASE_H
