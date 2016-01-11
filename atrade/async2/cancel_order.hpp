#ifndef ATRADE_ASYNC_2_CANCEL_ORDER_HPP
#define ATRADE_ASYNC_2_CANCEL_ORDER_HPP

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
    struct CancelOrder
    {
        virtual ~CancelOrder() {}

        int64_t order_id;
        std::function<void()> success;
        std::function<void()> error;
    };

    class CancelOrderService
    {
    public:
        CancelOrderService(asio::io_service& ios) :
            m_ios(ios)
        {
            m_order_subscription = subscribe_order(m_ios, [this](const Order& order) mutable {
                if (m_orders.count(order.order_id) && order.status == OrderStatus::Cancel)
                {
                    for (auto& op : m_orders[order.order_id])
                    {
                        op->success();
                    }
                    m_orders.erase(order.order_id);
                }
            });
            m_order_error_subscription = subscribe_order_error(m_ios, [this] (const OrderError& error) mutable {
                if (m_orders.count(error.order_id) && error.op_type == OrderOperationType::Delete)
                {
                    for (auto& op : m_orders[error.order_id])
                    {
                        op->error();
                    }
                    m_orders.erase(error.order_id);
                }
            });
        }

        void async_cancel_order(int64_t order_id, std::unique_ptr<CancelOrder>&& cancel_order)
        {
            async2::del_order(m_ios, order_id);
            cancel_order->order_id = order_id;
            m_orders[order_id].emplace_back(std::move(cancel_order));
        }

        void async_wait_cancel(int64_t order_id, std::unique_ptr<CancelOrder>&& wait_cancel)
        {
            History& history = get_history(m_ios);
            if (history.has_order(order_id) &&
                history.get_order_by_id(order_id).back().status == OrderStatus::Cancel)
            {
                wait_cancel->success();
                wait_cancel.reset();
                return;
            }
            std::vector<OrderError> errors = history.get_order_error_by_id(order_id);
            if (!errors.empty())
            {
                auto it = std::find_if(errors.begin(), errors.end(),
                                       [](const auto& e) { return e.op_type == OrderOperationType::Delete; });
                if (it != errors.end())
                {
                    wait_cancel->error();
                    wait_cancel.reset();
                    return;
                }
            }
            wait_cancel->order_id = order_id;
            m_orders[order_id].emplace_back(std::move(wait_cancel));
        }

        void cancel(int64_t order_id, CancelOrder* cancel_order)
        {
            if (m_orders.count(order_id))
            {
                std::vector<std::unique_ptr<CancelOrder>>& orders =  m_orders[order_id];
                orders.erase(std::remove_if(orders.begin(), orders.end(),
                                            [&cancel_order](const auto& ptr) { return ptr.get() == cancel_order; }),
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
        std::unordered_map<int64_t, std::vector<std::unique_ptr<CancelOrder>>> m_orders;
    };
}

class CancelOrderService : public asio::io_service::service
{
public:

    struct _CancelOrder : public detail::CancelOrder
    {
        Task task;
        Subscription<CtxClosingHandler> on_ctx_closing;
    };

    CancelOrderService(asio::io_service& ios) :
        service(ios),
        m_service(ios)
    {
    }

    ~CancelOrderService()
    {
    }

    template<typename Handler>
    void async_cancel_order(TradingContext* ctx, int64_t order_id, const pt::time_duration& duration, Handler&& handler)
    {
        auto cancel_order_ptr = std::make_unique<_CancelOrder>();
        _CancelOrder& cancel_order = *cancel_order_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc) mutable {
            get_io_service().post([errc, __handler]() mutable {
                __handler(errc);
            });
        };

        if (ctx)
        {
            if (ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed));
                return;
            }

            cancel_order.on_ctx_closing = ctx->subscribe([this, _handler, &cancel_order]() mutable {
                _handler(system::error_code(Errc::CtxClosed));
                m_service.cancel(cancel_order.order_id, &cancel_order);
            });
        }

        if (!duration.is_not_a_date_time())
        {
            cancel_order.task = add_task(get_io_service(), duration, [this, _handler, &cancel_order]() mutable {
                _handler(system::error_code(Errc::CancelOrderTimeout));
                m_service.cancel(cancel_order.order_id, &cancel_order);
            });
        }

        cancel_order.success = [_handler] () mutable {
            _handler(system::error_code());
        };

        cancel_order.error = [_handler] () mutable {
            _handler(system::error_code(Errc::CancelOrderError));
        };

        m_service.async_cancel_order(order_id, std::move(cancel_order_ptr));
    }

    template<typename Handler>
    void async_wait_cancel(TradingContext* ctx, int64_t order_id, const pt::time_duration& duration, Handler&& handler)
    {
        auto wait_cancel_ptr = std::make_unique<_CancelOrder>();
        _CancelOrder& wait_cancel = *wait_cancel_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc) mutable {
            get_io_service().post([errc, __handler]() mutable {
                __handler(errc);
            });
        };

        if (ctx)
        {
            if (ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed));
                return;
            }

            wait_cancel.on_ctx_closing = ctx->subscribe([this, _handler, &wait_cancel]() mutable {
                _handler(system::error_code(Errc::CtxClosed));
                m_service.cancel(wait_cancel.order_id, &wait_cancel);
            });
        }

        if (!duration.is_not_a_date_time())
        {
            wait_cancel.task = add_task(get_io_service(), duration, [this, _handler, &wait_cancel]() mutable {
                _handler(system::error_code(Errc::CancelOrderTimeout));
                m_service.cancel(wait_cancel.order_id, &wait_cancel);
            });
        }

        wait_cancel.success = [_handler] () mutable {
            _handler(system::error_code());
        };

        wait_cancel.error = [_handler] () mutable {
            _handler(system::error_code(Errc::CancelOrderError));
        };

        m_service.async_wait_cancel(order_id, std::move(wait_cancel_ptr));
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
    detail::CancelOrderService m_service;

    void shutdown_service()
    {
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_cancel_order(asio::io_service& ios, TradingContext* ctx, int64_t order_id, const pt::time_duration& duration,
                        CompletionToken&& token)
{
    if (!asio::has_service<CancelOrderService>(ios))
    {
        asio::add_service<CancelOrderService>(ios, new CancelOrderService(ios));
    }

    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code)>::type;

    handler_type handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(handler);

    CancelOrderService& service = asio::use_service<CancelOrderService>(ios);

    service.async_cancel_order(ctx, order_id, duration, std::move(handler));

    return result.get();
}

template<typename CompletionToken>
auto async_cancel_order(asio::io_service& ios, TradingContext& ctx, int64_t order_id, const pt::time_duration& duration,
                        CompletionToken&& token)
{
    return async_cancel_order(ios, &ctx, order_id, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_cancel_order(asio::io_service& ios, int64_t order_id, const pt::time_duration& duration, CompletionToken&& token)
{
    return async_cancel_order(ios, nullptr, order_id, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_cancel_order(asio::io_service& ios, TradingContext& ctx, int64_t order_id, CompletionToken&& token)
{
    return async_cancel_order(ios, &ctx, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_cancel_order(asio::io_service& ios, int64_t order_id, CompletionToken&& token)
{
    return async_cancel_order(ios, nullptr, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_wait_cancel(asio::io_service& ios, TradingContext* ctx, int64_t order_id, const pt::time_duration& duration,
                       CompletionToken&& token)
{
    if (!asio::has_service<CancelOrderService>(ios))
    {
        asio::add_service<CancelOrderService>(ios, new CancelOrderService(ios));
    }

    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code)>::type;

    handler_type handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(handler);

    CancelOrderService& service = asio::use_service<CancelOrderService>(ios);

    service.async_wait_cancel(ctx, order_id, duration, std::move(handler));

    return result.get();
}

template<typename CompletionToken>
auto async_wait_cancel(asio::io_service& ios, TradingContext& ctx, int64_t order_id, const pt::time_duration& duration,
                       CompletionToken&& token)
{
    return async_wait_cancel(ios, &ctx, order_id, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_wait_cancel(asio::io_service& ios, int64_t order_id, const pt::time_duration& duration, CompletionToken&& token)
{
    return async_wait_cancel(ios, nullptr, order_id, duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_wait_cancel(asio::io_service& ios, TradingContext& ctx, int64_t order_id, CompletionToken&& token)
{
    return async_wait_cancel(ios, &ctx, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_wait_cancel(asio::io_service& ios, int64_t order_id, CompletionToken&& token)
{
    return async_wait_cancel(ios, nullptr, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

}
}

#endif // ATRADE_ASYNC_2_CANCEL_ORDER_HPP
