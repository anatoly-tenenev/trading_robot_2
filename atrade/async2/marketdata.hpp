#ifndef ATRADE_ASYNC_2_MARKETDATA_H
#define ATRADE_ASYNC_2_MARKETDATA_H

#include "../headers.h"
#include "subscription.hpp"
#include "../imarket.h"
#include "../security.h"

namespace atrade {
namespace async2 {

class MarketData
{
public:
    MarketData(IAsyncMarket* market);

    std::vector<Deal> ticks(int64_t isin_id) const;
    std::vector<Quote> stocks(int64_t isin_id) const;
    Security security(int64_t isin_id) const;
    std::vector<Security> securities() const;
    int64_t get_isin_id(const std::string& code) const;

private:
    IAsyncMarket* m_market;
    std::unordered_map<int64_t, std::vector<Deal>> m_ticks;
    std::unordered_map<int64_t, Security> m_securities;
    Subscription<TickHandler> m_tick_subscription;
    Subscription<StockHandler> m_stock_subscription;
    Subscription<SecurityHandler> m_security_subscription;
};

}
}

#endif // ATRADE_ASYNC_2_MARKETDATA_H
