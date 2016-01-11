#ifndef ATRADE_IMARKET_H
#define ATRADE_IMARKET_H

#include "headers.h"
#include "types/types.h"
#include "security.h"

namespace atrade {

typedef std::function<void(int64_t, const OrdLogEntry&)> OrdLogHandler;
typedef std::function<void(const Order&)> OrderHandler;
typedef std::function<void(const Deal&)> TickHandler;
typedef std::function<void(int64_t, const std::vector<Quote>&)> StockHandler;
typedef std::function<void(const std::vector<Security>&)> SecurityHandler;
typedef std::function<void(const OrderError&)> OrderErrorHandler;

class IMarket
{
public:
    virtual ~IMarket() {}
    virtual void add_order(const Order& order) = 0;
    virtual void del_order(int64_t order_id) = 0;
    virtual void move_order(int64_t order_id, int64_t new_order_id, double price, int volume) = 0;
    virtual void req_securities() = 0;
    virtual void ordlog_handler(const OrdLogHandler& handler) = 0;
    virtual void order_handler(const OrderHandler& handler) = 0;
    virtual void order_error_handler(const OrderErrorHandler& handler) = 0;
    virtual void tick_handler(const TickHandler& handler) = 0;
    virtual void stock_handler(const StockHandler& handler) = 0;
    virtual void security_handler(const SecurityHandler& handler) = 0;
    virtual std::vector<Quote> stocks(int64_t isin_id) = 0;
    virtual pt::ptime datetime() = 0;
    virtual void process() = 0;
};

}

#endif // ATRADE_IMARKET_H
