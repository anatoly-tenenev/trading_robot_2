#ifndef ATRADE_ASYNC_2_ADD_ORDER_HPP
#define ATRADE_ASYNC_2_ADD_ORDER_HPP

#include "../headers.h"
#include "../globals.h"
#include "market_service.hpp"
#include "trading_context.hpp"
#include "error_code.hpp"
#include "../utils.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

namespace detail
{
    struct AddOrder
    {
        virtual ~AddOrder() {}

        int64_t order_id;
        std::function<void(int64_t)> success;
        std::function<void()> error;
    };

    class AddOrderService
    {
    public:
        AddOrderService(asio::io_service& ios) :
            m_ios(ios)
        {
            m_order_subscription = subscribe_order(m_ios, [this](const Order& order) mutable {
                if (m_orders.count(order.order_id))
                {
                    for (auto& op : m_orders[order.order_id])
                    {
                        op->success(order.order_id);
                    }
                    m_orders.erase(order.order_id);
                }
            });
            m_order_error_subscription = subscribe_order_error(m_ios, [this] (const OrderError& error) mutable {
                if (m_orders.count(error.order_id))
                {
                    for (auto& op : m_orders[error.order_id])
                    {
                        op->error();
                    }
                    m_orders.erase(error.order_id);
                }
            });
        }

        void async_add_order(IAsyncMarket* market, const Order& order, std::unique_ptr<AddOrder>&& add_order)
        {
            Order _order = order;
            if (!_order.order_id)
            {
                _order.order_id = generate_order_id();
            }
            if (market)
            {
                market->add_order(_order);
            }
            else
            {
                async2::add_order(m_ios, _order);
            }
            add_order->order_id = _order.order_id;
            m_orders[_order.order_id].emplace_back(std::move(add_order));
        }

        void async_wait_order(int64_t order_id, std::unique_ptr<AddOrder>&& wait_order)
        {
            if (get_history(m_ios).has_order(order_id))
            {
                wait_order->success(order_id);
                wait_order.reset();
                return;
            }
            else if (get_history(m_ios).has_order_error(order_id))
            {
                wait_order->error();
                wait_order.reset();
                return;
            }
            wait_order->order_id = order_id;
            m_orders[order_id].emplace_back(std::move(wait_order));
        }

        void cancel(int64_t order_id, AddOrder* add_order)
        {
            if (m_orders.count(order_id))
            {
                std::vector<std::unique_ptr<AddOrder>>& orders =  m_orders[order_id];
                orders.erase(std::remove_if(orders.begin(), orders.end(),
                                            [&add_order](const auto& ptr) { return ptr.get() == add_order; }),
                             orders.end());
                if (orders.empty())
                {
                    m_orders.erase(order_id);
                }
            }
        }

    private:
        asio::io_service& m_ios;
        Subscription<OrderHandler> m_order_subscription;
        Subscription<OrderErrorHandler> m_order_error_subscription;
        std::unordered_map<int64_t, std::vector<std::unique_ptr<AddOrder>>> m_orders;
    };
}

class AddOrderService : public asio::io_service::service
{
public:

    struct _AddOrder : public detail::AddOrder
    {
        Task task;
        Subscription<CtxClosingHandler> on_ctx_closing;
    };

    AddOrderService(asio::io_service& ios) :
        service(ios),
        m_service(ios)
    {
    }

    ~AddOrderService()
    {
    }

    template<typename Handler>
    void async_add_order(TradingContext* ctx, IAsyncMarket* market, const Order& order,
                         const pt::time_duration& duration, Handler&& handler)
    {
        auto add_order_ptr = std::make_unique<_AddOrder>();
        _AddOrder& add_order = *add_order_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc, int64_t order_id) mutable {
            get_io_service().post([errc, order_id, __handler]() mutable {
                __handler(errc, order_id);
            });
        };

        if (ctx)
        {
            if (ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed), 0);
                return;
            }

            add_order.on_ctx_closing = ctx->subscribe([this, _handler, &add_order]() mutable {
                _handler(system::error_code(Errc::CtxClosed), 0);
                m_service.cancel(add_order.order_id, &add_order);
            });
        }

        if (!duration.is_not_a_date_time())
        {
            add_order.task = add_task(get_io_service(), duration, [this, _handler, &add_order]() mutable {
                _handler(system::error_code(Errc::AddOrderTimeout), 0);
                m_service.cancel(add_order.order_id, &add_order);
            });
        }

        add_order.success = [_handler] (int64_t order_id) mutable {
            _handler(system::error_code(), order_id);
        };

        add_order.error = [_handler] () mutable {
            _handler(system::error_code(Errc::AddOrderReject), 0);
        };

        m_service.async_add_order(market, order, std::move(add_order_ptr));
    }

    template<typename Handler>
    void async_wait_order(TradingContext* ctx, int64_t order_id, const pt::time_duration& duration, Handler&& handler)
    {
        auto wait_order_ptr = std::make_unique<_AddOrder>();
        _AddOrder& wait_order = *wait_order_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc, int64_t order_id) mutable {
            get_io_service().post([errc, order_id, __handler]() mutable {
                __handler(errc, order_id);
            });
        };

        if (ctx)
        {
            if (ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed), 0);
                return;
            }

            wait_order.on_ctx_closing = ctx->subscribe([this, _handler, &wait_order]() mutable {
                _handler(system::error_code(Errc::CtxClosed), 0);
                m_service.cancel(wait_order.order_id, &wait_order);
            });
        }

        if (!duration.is_not_a_date_time())
        {
            wait_order.task = add_task(get_io_service(), duration, [this, _handler, &wait_order]() mutable {
                _handler(system::error_code(Errc::AddOrderTimeout), 0);
                m_service.cancel(wait_order.order_id, &wait_order);
            });
        }

        wait_order.success = [_handler] (int64_t order_id) mutable {
            _handler(system::error_code(), order_id);
        };

        wait_order.error = [_handler] () mutable {
            _handler(system::error_code(Errc::AddOrderReject), 0);
        };

        m_service.async_wait_order(order_id, std::move(wait_order_ptr));
    }

    static asio::io_service::id id;

    struct implementation_type
    {
    };

    void construct(implementation_type&)
    {
    }

    void destroy(implementation_type&)
    {
    }

private:
    detail::AddOrderService m_service;

    void shutdown_service()
    {
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext* ctx, IAsyncMarket* market, const Order& order,
                     const pt::time_duration& duration, CompletionToken&& token)
{
    if (!asio::has_service<AddOrderService>(ios))
    {
        asio::add_service<AddOrderService>(ios, new AddOrderService(ios));
    }

    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code, int64_t)>::type;

    handler_type handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(handler);

    AddOrderService& service = asio::use_service<AddOrderService>(ios);

    service.async_add_order(ctx, market, order, duration, std::move(handler));

    return result.get();
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext& ctx, const Order& order, const pt::time_duration& duration,
                     CompletionToken&& token)
{
    return async_add_order(ios, &ctx, nullptr, order, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, const Order& order, const pt::time_duration& duration, CompletionToken&& token)
{
    return async_add_order(ios, nullptr, nullptr, order, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext& ctx, const Order& order, CompletionToken&& token)
{
    return async_add_order(ios, &ctx, nullptr, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, const Order& order, CompletionToken&& token)
{
    return async_add_order(ios, nullptr, nullptr, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket* market, const Order& order,
                     const pt::time_duration& duration, CompletionToken&& token)
{
    return async_add_order(ios, &ctx, market, order, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket& market, const Order& order,
                     const pt::time_duration& duration, CompletionToken&& token)
{
    return async_add_order(ios, &ctx, &market, order, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, IAsyncMarket* market, const Order& order,
                     const pt::time_duration& duration, CompletionToken&& token)
{
    return async_add_order(ios, nullptr, market, order, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, IAsyncMarket& market, const Order& order,
                     const pt::time_duration& duration, CompletionToken&& token)
{
    return async_add_order(ios, nullptr, &market, order, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket* market,
                     const Order& order, CompletionToken&& token)
{
    return async_add_order(ios, &ctx, market, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket& market,
                     const Order& order, CompletionToken&& token)
{
    return async_add_order(ios, &ctx, &market, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, IAsyncMarket* market, const Order& order, CompletionToken&& token)
{
    return async_add_order(ios, nullptr, market, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_add_order(asio::io_service& ios, IAsyncMarket& market, const Order& order, CompletionToken&& token)
{
    return async_add_order(ios, nullptr, &market, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_wait_order(asio::io_service& ios, TradingContext* ctx, int64_t order_id, const pt::time_duration& duration,
                      CompletionToken&& token)
{
    if (!asio::has_service<AddOrderService>(ios))
    {
        asio::add_service<AddOrderService>(ios, new AddOrderService(ios));
    }

    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code, int64_t)>::type;

    handler_type handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(handler);

    AddOrderService& service = asio::use_service<AddOrderService>(ios);

    service.async_wait_order(ctx, order_id, duration, std::move(handler));

    return result.get();
}

template<typename CompletionToken>
auto async_wait_order(asio::io_service& ios, TradingContext& ctx, int64_t order_id, const pt::time_duration& duration,
                      CompletionToken&& token)
{
    return async_wait_order(ios, &ctx, order_id, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_wait_order(asio::io_service& ios, int64_t order_id, const pt::time_duration& duration, CompletionToken&& token)
{
    return async_wait_order(ios, nullptr, order_id, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_wait_order(asio::io_service& ios, TradingContext& ctx, int64_t order_id, CompletionToken&& token)
{
    return async_wait_order(ios, &ctx, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_wait_order(asio::io_service& ios, int64_t order_id, CompletionToken&& token)
{
    return async_wait_order(ios, nullptr, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

}
}

#endif // ATRADE_ASYNC_2_ADD_ORDER_HPP
