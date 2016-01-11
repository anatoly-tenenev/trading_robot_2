#ifndef ATRADE_ASYNC_2_SUBSCRIPTION_HPP
#define ATRADE_ASYNC_2_SUBSCRIPTION_HPP

#include "../headers.h"
#include "../utils.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

template <typename ...Args>
class Subscribable
{
public:
    typedef std::function<void(Args...)> Handler;

    virtual ~Subscribable() {}

    Subscription<Handler> subscribe(Handler&& handler)
    {
        return Subscription<Handler>(m_service, std::forward<Handler>(handler));
    }

protected:
    void publish(const Args&... args)
    {
        m_service.publish(args...);
    }

private:
    SubscriptionService<Handler> m_service;
};

template <typename THandler>
class Subscription
{
    friend class SubscriptionService<THandler>;

    template <typename _THandler>
    Subscription(SubscriptionService<THandler>* service, _THandler&& handler) :
        m_service(nullptr),
        m_id(0),
        m_removed(false),
        m_handler(std::forward<_THandler>(handler))
    {
        if (service)
        {
            service->push(this);
        }
    }

public:
    template <typename _THandler>
    Subscription(SubscriptionService<THandler>& service, _THandler&& handler) :
        Subscription(&service, std::forward<_THandler>(handler))
    {
    }

    Subscription(SubscriptionService<THandler>& service) :
        Subscription(&service, nullptr)
    {
    }

    Subscription() :
        Subscription(nullptr, nullptr)
    {
    }

    Subscription(const Subscription&) = delete;
    Subscription& operator= (const Subscription&) = delete;

    Subscription(Subscription&& subscription)
    {
        move(std::move(subscription));
    }

    Subscription& operator= (Subscription&& subscription)
    {
        if (this == &subscription)
        {
            return *this;
        }
        move(std::move(subscription));
        return *this;
    }

    Subscription& operator= (std::nullptr_t)
    {
        unsubscribe();
        m_handler = nullptr;
        return *this;
    }

    ~Subscription()
    {
        unsubscribe();
    }

    explicit operator bool() const
    {
        return m_service != nullptr;
    }

    void set_handler(THandler&& handler)
    {
        m_handler = std::forward<THandler>(handler);
    }

    void unsubscribe()
    {
        if (m_service)
        {
            m_service->remove(this);
        }
    }

    class Order
    {
    public:
        bool operator() (const Subscription<THandler>* s1, const Subscription<THandler>* s2)
        {
            return s1->m_id < s2->m_id;
        }
    };

private:
    SubscriptionService<THandler>* m_service;
    u_int64_t m_id;
    bool m_removed;
    THandler m_handler;

    template <typename ...Args>
    void handler(const Args&... args)
    {
        if (m_handler)
        {
            m_handler(args...);
        }
    }

    void move(Subscription&& subscription)
    {
        unsubscribe();
        m_service = nullptr;
        auto service = subscription.m_service;
        m_id = subscription.m_id;
        m_removed = subscription.m_removed;
        m_handler = std::move(subscription.m_handler);
        subscription.m_handler = nullptr;
        subscription.unsubscribe();
        if (service)
        {
            service->push(this);
        }
    }
};

template <typename THandler>
class SubscriptionService
{
public:
    typedef typename Subscription<THandler>::Order Order;

    SubscriptionService() :
        m_subscription_id(0)
    {
    }

    ~SubscriptionService()
    {
        clear();
    }

    SubscriptionService(const SubscriptionService&) = delete;
    SubscriptionService& operator= (const SubscriptionService&) = delete;

    SubscriptionService(SubscriptionService&& service)
    {
        move(std::move(service));
    }

    SubscriptionService& operator= (SubscriptionService&& service)
    {
        if (this == &service)
        {
            return *this;
        }
        move(std::move(service));
        return *this;
    }

    void push(Subscription<THandler>* subscription)
    {
        if (!subscription)
        {
            throw BadSubscriptionException();
        }
        if (subscription->m_removed)
        {
            throw SubscriptionRemovedException();
        }
        if (subscription->m_service)
        {
            throw SubscriptionIsActiveException();
        }
        if(!subscription->m_id)
        {
            ++m_subscription_id;
            subscription->m_id = m_subscription_id;
        }
        subscription->m_service = this;
        m_subscriptions.emplace(subscription);
    }

    void remove(Subscription<THandler>* subscription)
    {
        if (!subscription || subscription->m_service != this)
        {
            return;
        }
        subscription->m_service = nullptr;
        subscription->m_removed = true;
        m_subscriptions.erase(subscription);
    }

    template <typename ...Args>
    void publish(const Args&... args)
    {
        std::vector<THandler> handlers;
        handlers.reserve(m_subscriptions.size());
        for (auto& s : m_subscriptions)
        {
            if (s->m_handler)
            {
                handlers.push_back(s->m_handler);
            }
        }
        for (auto& h : handlers)
        {
            h(args...);
        }
    }

    void clear()
    {
        for (auto& s : m_subscriptions)
        {
            s->m_service = nullptr;
            s->m_removed = true;
        }
        m_subscriptions.clear();
    }

private:
    u_int64_t m_subscription_id;
    std::multiset<Subscription<THandler>*, Order> m_subscriptions;

    void move(SubscriptionService&& service)
    {
        m_subscription_id = service.m_subscription_id;
        m_subscriptions = std::move(service.m_subscriptions);
        service.m_subscriptions.clear();
        for (auto& s : m_subscriptions)
        {
            s->m_service = this;
        }
    }
};

}
}

#endif // ATRADE_ASYNC_2_SUBSCRIPTION_HPP
