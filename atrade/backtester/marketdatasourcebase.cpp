#include "marketdatasourcebase.h"

namespace atrade {
namespace backtester {

MarketDataSourceBase::MarketDataSourceBase()
{
}

fumap<int64_t, std::vector<BQuote>()> MarketDataSourceBase::stocks()
{
    return m_stocks;
}

void MarketDataSourceBase::ordlog_handler(const BOrdLogHandler& handler)
{
    m_ordlog_handler = handler;
}

void MarketDataSourceBase::tick_handler(const BTickHandler& handler)
{
    m_tick_handler = handler;
}

void MarketDataSourceBase::stock_handler(const BStockHandler& handler)
{
    m_stock_handler = handler;
}

}
}
