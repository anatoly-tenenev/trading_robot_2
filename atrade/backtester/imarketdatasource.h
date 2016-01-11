#ifndef BACKTESTER_IMARKETDATASOURCE_H
#define BACKTESTER_IMARKETDATASOURCE_H

#include "../headers.h"
#include "../imarket.h"
#include "../security.h"
#include "types.h"

namespace atrade {
namespace backtester {

typedef std::function<void(int64_t, const BOrdLogEntry&)> BOrdLogHandler;
typedef std::function<void(const BDeal&)> BTickHandler;
typedef std::function<void(int64_t, const std::vector<BQuote>&)> BStockHandler;

class IMarketDataSource
{
public:
    virtual ~IMarketDataSource() {}
    virtual std::vector<Security> securities() const = 0;
    virtual pt::ptime datetime() const = 0;
    virtual void process() = 0;
    virtual fumap<int64_t, std::vector<BQuote>()> stocks() = 0;
    virtual void ordlog_handler(const BOrdLogHandler& handler) = 0;
    virtual void tick_handler(const BTickHandler& handler) = 0;
    virtual void stock_handler(const BStockHandler& handler) = 0;
};

}
}

#endif // BACKTESTER_IMARKETDATASOURCE_H
