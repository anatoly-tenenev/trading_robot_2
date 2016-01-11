#include "marketbase.h"

namespace atrade {

MarketBase::MarketBase()
{
}

void MarketBase::ordlog_handler(const OrdLogHandler &handler)
{
    m_ordlog_handler = handler;
}

void MarketBase::order_handler(const OrderHandler &handler)
{
    m_order_handler = handler;
}

void MarketBase::order_error_handler(const OrderErrorHandler &handler)
{
    m_order_error_handler = handler;
}

void MarketBase::tick_handler(const TickHandler &handler)
{
    m_tick_handler = handler;
}

void MarketBase::stock_handler(const StockHandler &handler)
{
    m_stock_handler = handler;
}

void MarketBase::security_handler(const SecurityHandler &handler)
{
    m_security_handler = handler;
}

}
