#ifndef ATRADE_ASYCN_2_ASYNCMARKET_H
#define ATRADE_ASYCN_2_ASYNCMARKET_H

#include "../headers.h"
#include "../imarket.h"
#include "subscription.hpp"
#include "history.hpp"
#include "marketdata.hpp"

namespace atrade {
namespace async2 {

class AsyncMarketRootIsNullException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "AsyncMarket root is null.";
    }
};

class AsyncMarketParentIsNullException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "AsyncMarket parent is null.";
    }
};

class IAsyncMarket
{
public:
    virtual ~IAsyncMarket() {}

    virtual void add_order(const Order& order) = 0;
    virtual void del_order(int64_t order_id) = 0;
    virtual void move_order(int64_t order_id, int64_t new_order_id, double price, int volume) = 0;
    virtual void req_securities() = 0;

    virtual Subscription<OrdLogHandler> subscribe_ordlog(const OrdLogHandler& handler) = 0;
    virtual Subscription<OrderHandler> subscribe_order(const OrderHandler& handler) = 0;
    virtual Subscription<OrderErrorHandler> subscribe_order_error(const OrderErrorHandler& handler) = 0;
    virtual Subscription<TickHandler> subscribe_tick(const TickHandler& handler) = 0;
    virtual Subscription<StockHandler> subscribe_stock(const StockHandler& handler) = 0;
    virtual Subscription<SecurityHandler> subscribe_security(const SecurityHandler& handler) = 0;

    virtual History& history() = 0;
    virtual MarketData& market_data() = 0;
    virtual std::vector<Quote> stocks(int64_t isin_id) = 0;
    virtual std::unordered_set<int64_t> add_order_pendings() const = 0;
};

class AsyncMarketBase : public IAsyncMarket
{
protected:
    AsyncMarketBase(AsyncMarketBase* root, AsyncMarketBase* parent);
    AsyncMarketBase(AsyncMarketBase& parent);

public:
    virtual ~AsyncMarketBase();

    AsyncMarketBase(const AsyncMarketBase&) = delete;
    AsyncMarketBase& operator= (const AsyncMarketBase&) = delete;

    AsyncMarketBase(AsyncMarketBase&&) = default;
    AsyncMarketBase& operator= (AsyncMarketBase&&) = default;

    void add_order(const Order& order);
    void move_order(int64_t order_id, int64_t new_order_id, double price, int volume);

    void subscribe(Subscription<OrdLogHandler>& subscription);
    void subscribe(Subscription<OrderHandler>& subscription);
    void subscribe(Subscription<OrderErrorHandler>& subscription);
    void subscribe(Subscription<TickHandler>& subscription);
    void subscribe(Subscription<StockHandler>& subscription);
    void subscribe(Subscription<SecurityHandler>& subscription);

    Subscription<OrdLogHandler> subscribe_ordlog(const OrdLogHandler& handler);
    Subscription<OrderHandler> subscribe_order(const OrderHandler& handler);
    Subscription<OrderErrorHandler> subscribe_order_error(const OrderErrorHandler& handler);
    Subscription<TickHandler> subscribe_tick(const TickHandler& handler);
    Subscription<StockHandler> subscribe_stock(const StockHandler& handler);
    Subscription<SecurityHandler> subscribe_security(const SecurityHandler& handler);

    History& history();

    std::unordered_set<int64_t> add_order_pendings() const;

protected:
    AsyncMarketBase* m_root;
    AsyncMarketBase* m_parent;
    uset<AsyncMarketBase*> m_children;
    SubscriptionService<OrdLogHandler> m_ordlog_subscriptions;
    SubscriptionService<OrderHandler> m_order_subscriptions;
    SubscriptionService<OrderErrorHandler> m_order_error_subscriptions;
    SubscriptionService<TickHandler> m_tick_subscriptions;
    SubscriptionService<StockHandler> m_stock_subscriptions;
    SubscriptionService<SecurityHandler> m_security_subscriptions;
    History m_history;
    std::unordered_set<int64_t> m_add_order_pendings;
    Subscription<OrderHandler> m_order_subscription;
    Subscription<OrderErrorHandler> m_order_error_subscription;
};

class AsyncMarket : public AsyncMarketBase
{
public:
    AsyncMarket(IAsyncMarket& parent) :
        AsyncMarketBase(static_cast<AsyncMarketBase&>(parent))
    {
        m_ordlog_subscription = m_parent->subscribe_ordlog([this](int64_t isin_id, const OrdLogEntry& entry) {
            if (m_order_ids.count(entry.order_id))
            {
                m_ordlog_subscriptions.publish(isin_id, entry);
            }
        });
        m_order_subscription = m_parent->subscribe_order([this](const Order& order) {
            if (m_order_ids.count(order.order_id))
            {
                m_order_subscriptions.publish(order);
            }
        });
        m_order_error_subscription = m_parent->subscribe_order_error([this](const OrderError& error) {
            if (m_order_ids.count(error.order_id))
            {
                m_order_error_subscriptions.publish(error);
            }
        });
        m_tick_subscription = m_parent->subscribe_tick([this](const Deal& tick) {
            m_tick_subscriptions.publish(tick);
        });
        m_stock_subscription = m_parent->subscribe_stock([this](int64_t isin_id, const std::vector<Quote>& stocks) {
            m_stock_subscriptions.publish(isin_id, stocks);
        });
        m_security_subscription = m_parent->subscribe_security([this](const std::vector<Security>& securities) {
            m_security_subscriptions.publish(securities);
        });
    }

    virtual ~AsyncMarket()
    {
    }

    AsyncMarket(const AsyncMarket&) = delete;
    AsyncMarket& operator= (const AsyncMarket&) = delete;

    AsyncMarket(AsyncMarket&&) = default;
    AsyncMarket& operator= (AsyncMarket&&) = default;

    void add_order(const Order& order)
    {
        if (m_parent)
        {
            AsyncMarketBase::add_order(order);
            m_order_ids.insert(order.order_id);
            m_parent->add_order(order);
        }
        else
        {
            throw AsyncMarketParentIsNullException();
        }
    }

    void del_order(int64_t order_id)
    {
        if (m_root)
        {
            m_root->del_order(order_id);
        }
        else
        {
            throw AsyncMarketRootIsNullException();
        }
    }

    void move_order(int64_t order_id, int64_t new_order_id, double price, int volume)
    {
        if (m_root)
        {
            AsyncMarketBase::move_order(order_id, new_order_id, price, volume);
            m_order_ids.insert(new_order_id);
            m_root->move_order(order_id, new_order_id, price, volume);
        }
        else
        {
            throw AsyncMarketRootIsNullException();
        }
    }

    void req_securities()
    {
        if (m_root)
        {
            m_root->req_securities();
        }
        else
        {
            throw AsyncMarketRootIsNullException();
        }
    }

    MarketData& market_data()
    {
        if (m_root)
        {
            return m_root->market_data();
        }
        else
        {
            throw AsyncMarketRootIsNullException();
        }
    }

    std::vector<Quote> stocks(int64_t isin_id)
    {
        if (m_root)
        {
            return m_root->stocks(isin_id);
        }
        else
        {
            throw AsyncMarketRootIsNullException();
        }
    }

private:
    std::unordered_set<int64_t> m_order_ids;
    Subscription<OrdLogHandler> m_ordlog_subscription;
    Subscription<OrderHandler> m_order_subscription;
    Subscription<OrderErrorHandler> m_order_error_subscription;
    Subscription<TickHandler> m_tick_subscription;
    Subscription<StockHandler> m_stock_subscription;
    Subscription<SecurityHandler> m_security_subscription;
};

class RootAsyncMarket : public AsyncMarketBase
{
public:
    RootAsyncMarket(IMarket& market) :
        AsyncMarketBase(this, nullptr),
        m_market(&market),
        m_market_data(this)
    {
        m_market->ordlog_handler([this](int64_t isin_id, const OrdLogEntry& entry) {
            bool is_buy = ((entry.flags & OrdLogFlags::Buy) != 0);
            bool is_sell = ((entry.flags & OrdLogFlags::Sell) != 0);
            bool is_non_system = ((entry.flags & OrdLogFlags::NonSystem) != 0);
            bool is_repl_act = ((entry.flags & OrdLogFlags::NonZeroReplAct) != 0);
            if(!(is_buy ^ is_sell) || is_non_system || is_repl_act)
            {
                return;
            }
            m_ordlog_subscriptions.publish(isin_id, entry);
        });
        m_market->order_handler([this](const Order& order) {
            m_order_subscriptions.publish(order);
        });
        m_market->order_error_handler([this](const OrderError& error) {
            m_order_error_subscriptions.publish(error);
        });
        m_market->tick_handler([this](const Deal& tick) {
            m_tick_subscriptions.publish(tick);
        });
        m_market->stock_handler([this](int64_t isin_id, const std::vector<Quote>& stocks) {
            m_stock_subscriptions.publish(isin_id, stocks);
        });
        m_market->security_handler([this](const std::vector<Security>& securities) {
            m_security_subscriptions.publish(securities);
        });
    }

    virtual ~RootAsyncMarket()
    {
    }

    RootAsyncMarket(const RootAsyncMarket&) = delete;
    RootAsyncMarket& operator= (const RootAsyncMarket&) = delete;

    void add_order(const Order& order)
    {
        AsyncMarketBase::add_order(order);
        m_market->add_order(order);
    }

    void del_order(int64_t order_id)
    {
        m_market->del_order(order_id);
    }

    void move_order(int64_t order_id, int64_t new_order_id, double price, int volume)
    {
        AsyncMarketBase::move_order(order_id, new_order_id, price, volume);
        m_market->move_order(order_id, new_order_id, price, volume);
    }

    void req_securities()
    {
        m_market->req_securities();
    }

    pt::ptime datetime() const
    {
        return m_market->datetime();
    }

    MarketData& market_data()
    {
        return m_market_data;
    }

    std::vector<Quote> stocks(int64_t isin_id)
    {
        return m_market->stocks(isin_id);
    }

    void process()
    {
        m_market->process();
    }

private:
    IMarket* m_market;
    MarketData m_market_data;
};

}
}

#endif // ATRADE_ASYCN_2_ASYNCMARKET_H
