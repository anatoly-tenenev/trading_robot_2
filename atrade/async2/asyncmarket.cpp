#include "asyncmarket.hpp"

namespace atrade {
namespace async2 {

AsyncMarketBase::AsyncMarketBase(AsyncMarketBase* root, AsyncMarketBase* parent) :
    m_root(root),
    m_parent(parent),
    m_order_subscriptions(),
    m_history(*this)
{
    if (m_parent)
    {
        m_parent->m_children.insert(this);
    }
    m_order_subscription = subscribe_order([this](const Order& order) {
        if (order.status == OrderStatus::Add)
        {
            m_add_order_pendings.erase(order.order_id);
        }
    });
    m_order_error_subscription = subscribe_order_error([this](const OrderError& error) {
        if (error.op_type == OrderOperationType::Add)
        {
            m_add_order_pendings.erase(error.order_id);
        }
    });
}

AsyncMarketBase::AsyncMarketBase(AsyncMarketBase& parent) :
    AsyncMarketBase(parent.m_root, &parent)
{
}

void AsyncMarketBase::add_order(const Order& order)
{
    m_add_order_pendings.insert(order.order_id);
}

void AsyncMarketBase::move_order(int64_t, int64_t new_order_id, double, int)
{
    m_add_order_pendings.insert(new_order_id);
}

AsyncMarketBase::~AsyncMarketBase()
{
    if (m_parent)
    {
        m_parent->m_children.erase(this);
    }
    for (auto& market : m_children)
    {
        market->m_parent = nullptr;
    }
    m_ordlog_subscriptions.clear();
    m_order_subscriptions.clear();
    m_order_error_subscriptions.clear();
    m_tick_subscriptions.clear();
    m_stock_subscriptions.clear();
    m_security_subscriptions.clear();
}

void AsyncMarketBase::subscribe(Subscription<OrdLogHandler>& subscription)
{
    m_ordlog_subscriptions.push(&subscription);
}

void AsyncMarketBase::subscribe(Subscription<OrderHandler>& subscription)
{
    m_order_subscriptions.push(&subscription);
}

void AsyncMarketBase::subscribe(Subscription<OrderErrorHandler>& subscription)
{
    m_order_error_subscriptions.push(&subscription);
}

void AsyncMarketBase::subscribe(Subscription<TickHandler>& subscription)
{
    m_tick_subscriptions.push(&subscription);
}

void AsyncMarketBase::subscribe(Subscription<StockHandler>& subscription)
{
    m_stock_subscriptions.push(&subscription);
}

void AsyncMarketBase::subscribe(Subscription<SecurityHandler>& subscription)
{
    m_security_subscriptions.push(&subscription);
}

Subscription<OrdLogHandler>
AsyncMarketBase::subscribe_ordlog(const OrdLogHandler& handler)
{
    return Subscription<OrdLogHandler>(m_ordlog_subscriptions, handler);
}

Subscription<OrderHandler>
AsyncMarketBase::subscribe_order(const OrderHandler& handler)
{
    return Subscription<OrderHandler>(m_order_subscriptions, handler);
}

Subscription<OrderErrorHandler>
AsyncMarketBase::subscribe_order_error(const OrderErrorHandler& handler)
{
    return Subscription<OrderErrorHandler>(m_order_error_subscriptions, handler);
}

Subscription<TickHandler>
AsyncMarketBase::subscribe_tick(const TickHandler &handler)
{
    return Subscription<TickHandler>(m_tick_subscriptions, handler);
}

Subscription<StockHandler>
AsyncMarketBase::subscribe_stock(const StockHandler& handler)
{
    return Subscription<StockHandler>(m_stock_subscriptions, handler);
}

Subscription<SecurityHandler>
AsyncMarketBase::subscribe_security(const SecurityHandler& handler)
{
    return Subscription<SecurityHandler>(m_security_subscriptions, handler);
}

History& AsyncMarketBase::history()
{
    return m_history;
}

std::unordered_set<int64_t> AsyncMarketBase::add_order_pendings() const
{
    return m_add_order_pendings;
}

}
}
