#ifndef ATRADE_MARKETBASE_H
#define ATRADE_MARKETBASE_H

#include "headers.h"
#include "imarket.h"

namespace atrade {

class MarketBase : public IMarket
{
public:
    MarketBase();
    void ordlog_handler(const OrdLogHandler& handler) override;
    void order_handler(const OrderHandler& handler) override;
    void order_error_handler(const OrderErrorHandler& handler) override;
    void tick_handler(const TickHandler& handler) override;
    void stock_handler(const StockHandler& handler) override;
    void security_handler(const SecurityHandler& handler) override;

protected:
    OrdLogHandler m_ordlog_handler;
    OrderHandler m_order_handler;
    OrderErrorHandler m_order_error_handler;
    TickHandler m_tick_handler;
    StockHandler m_stock_handler;
    SecurityHandler m_security_handler;
};

}

#endif // ATRADE_MARKETBASE_H
