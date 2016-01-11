#ifndef ATRADE_ASYNC_2_TRADINGCONTEXT_H
#define ATRADE_ASYNC_2_TRADINGCONTEXT_H

#include "../headers.h"
#include "subscription.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

typedef Subscribable<>::Handler CtxClosingHandler;

class TradingContext
{
public:
    typedef std::function<void(const system::system_error&)> ErrorHandler;

    TradingContext(TradingContext* parent) :
        m_parent(parent),
        m_closed(m_parent ? m_parent->m_closed : false)
    {
        if (m_parent)
        {
            if (m_parent->closed())
            {
                throw CtxClosedException();
            }
            m_parent->m_children.insert(this);
        }
    }

    TradingContext(TradingContext& parent) :
        TradingContext(&parent)
    {
    }

    TradingContext() :
        TradingContext(nullptr)
    {
    }

    ~TradingContext()
    {
        close();
    }

    bool closed() const
    {
        return m_closed;
    }

    void close()
    {
        if (m_closed)
        {
            return;
        }
        m_closed = true;
        if (m_parent)
        {
            m_parent->m_children.erase(this);
        }
        for (auto& ctx : m_children)
        {
            ctx->m_parent = nullptr;
            ctx->close();
        }
        m_children.clear();
        m_subscriptions.publish();
    }

    template <typename Handler>
    Subscription<CtxClosingHandler> subscribe(Handler&& handler)
    {
        return Subscription<CtxClosingHandler>(m_subscriptions, std::forward<Handler>(handler));
    }

    void on_error(const ErrorHandler& handler)
    {
        m_error_handler = handler;
    }

    bool error(const system::system_error& e)
    {
        bool handled = false;
        bool handled_by_parent = false;
        if (m_error_handler)
        {
            handled = true;
            m_error_handler(e);
        }
        if (m_parent)
        {
            handled_by_parent = m_parent->error(e);
        }
        return handled || handled_by_parent;
    }

private:
    TradingContext* m_parent;
    bool m_closed;
    std::unordered_set<TradingContext*> m_children;
    SubscriptionService<CtxClosingHandler> m_subscriptions;
    ErrorHandler m_error_handler;
};

}
}

#endif // ATRADE_ASYNC_2_TRADINGCONTEXT_H
