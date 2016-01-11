#ifndef ATRADE_ASYNC_2_WAIT_DEAL_HPP
#define ATRADE_ASYNC_2_WAIT_DEAL_HPP

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
    class WaitDealException : public std::exception
    {
    public:
        const char* what() const noexcept
        {
            return "Wait deal exception.";
        }
    };

    struct WaitDeal
    {
        virtual ~WaitDeal() {}

        std::function<void(const std::vector<Order>&)> success;
        std::function<void(Errc, const std::vector<Order>&)> error;
        bool entire;
        size_t* deals;
        std::vector<Order> buffer;
    };

    struct impl_type
    {
        TradingContext* ctx;
        int64_t order_id;
        size_t deals;
    };

    class WaitDealService
    {
    public:
        WaitDealService(asio::io_service& ios) :
            m_ios(ios)
        {
            m_order_subscription = subscribe_order(m_ios, [this](const Order& order) mutable {
                if (m_deals.count(order.order_id))
                {
                    auto& deals = m_deals[order.order_id];
                    for (auto it = deals.begin(); it != deals.end(); )
                    {
                        WaitDeal& wait_deal = *it->second;
                        if (order.status == OrderStatus::Cancel)
                        {
                            *wait_deal.deals += wait_deal.buffer.size();
                            wait_deal.error(Errc::OrderCanceled, wait_deal.buffer);
                            it = deals.erase(it);
                            continue;
                        }
                        else if (!wait_deal.entire || !order.amount_rest)
                        {
                            wait_deal.buffer.push_back(order);
                            *wait_deal.deals += wait_deal.buffer.size();
                            wait_deal.success(wait_deal.buffer);
                            it = deals.erase(it);
                            continue;
                        }
                        else
                        {
                            wait_deal.buffer.push_back(order);
                            ++it;
                        }
                    }
                    if (deals.empty())
                    {
                        m_deals.erase(order.order_id);
                    }
                }
            });
        }

        void async_wait_deal(impl_type& impl, bool entire, std::unique_ptr<WaitDeal>&& wait_deal_ptr)
        {
            if (m_deals.count(impl.order_id) && m_deals[impl.order_id].count(&impl))
            {
                throw WaitDealException();
            }
            WaitDeal& wait_deal = *wait_deal_ptr;
            std::vector<Order> order = get_history(m_ios).get_order_by_id(impl.order_id);
            std::vector<Order> filled_order;
            std::copy_if(order.begin(), order.end(),
                         std::back_inserter(filled_order),
                         [](const Order& o) { return o.status == OrderStatus::Fill; });
            if (impl.deals < filled_order.size())
            {
                std::vector<Order> result(filled_order.begin() + impl.deals,
                                          filled_order.end());
                if (!entire || !filled_order.back().amount_rest)
                {
                    impl.deals += result.size();
                    wait_deal.success(result);
                    return;
                }
                // entire == true
                if (order.back().status == OrderStatus::Cancel)
                {
                    impl.deals += result.size();
                    wait_deal.error(Errc::OrderCanceled, result);
                    return;
                }
                wait_deal.buffer = result;
            }
            else if (!filled_order.empty())
            {
                if (order.back().status == OrderStatus::Cancel)
                {
                    wait_deal.error(Errc::OrderCanceled, {});
                    return;
                }
                if (!filled_order.back().amount_rest)
                {
                    wait_deal.error(Errc::OrderFilled, {});
                    return;
                }
            }
            wait_deal.entire = entire;
            wait_deal.deals = &impl.deals;
            m_deals[impl.order_id].emplace(&impl, std::move(wait_deal_ptr));
        }

        void remove(impl_type& impl)
        {
            if (m_deals.count(impl.order_id))
            {
                m_deals[impl.order_id].erase(&impl);
                if (m_deals[impl.order_id].empty())
                {
                    m_deals.erase(impl.order_id);
                }
            }
        }

        void cancel(impl_type& impl)
        {
            if (m_deals.count(impl.order_id) && m_deals[impl.order_id].count(&impl))
            {
                m_deals[impl.order_id][&impl]->error(Errc::OperationAborted,
                                                     std::vector<Order>{});
            }
            remove(impl);
        }

        void shutdown()
        {
            m_deals.clear();
            m_order_subscription = nullptr;
        }

    private:
        asio::io_service& m_ios;
        Subscription<OrderHandler> m_order_subscription;
        umap<int64_t, umap<impl_type*, std::unique_ptr<WaitDeal>>> m_deals;
    };
}

class WaitDealService : public asio::io_service::service
{
public:

    typedef detail::impl_type implementation_type;

    typedef implementation_type impl_type;

    struct _WaitDeal : public detail::WaitDeal
    {
        Task task;
        Subscription<CtxClosingHandler> on_ctx_closing;
    };

    WaitDealService(asio::io_service& ios) :
        service(ios),
        m_service(ios),
        m_shutdowned(false)
    {
    }

    ~WaitDealService()
    {
    }

    void cancel(impl_type& impl)
    {
        if (m_shutdowned)
        {
            return;
        }
        m_service.cancel(impl);
    }

    template<typename Handler>
    void async_wait(impl_type& impl, bool entire, const pt::time_duration& duration, Handler&& handler)
    {
        auto wait_deal_ptr = std::make_unique<_WaitDeal>();
        _WaitDeal& wait_deal = *wait_deal_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc, const std::vector<Order> order) mutable {
            get_io_service().post([errc, order, __handler]() mutable {
                __handler(errc, order);
            });
        };

        if (impl.ctx)
        {
            if (impl.ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed), {});
                return;
            }

            wait_deal.on_ctx_closing = impl.ctx->subscribe([&, _handler]() mutable {
                _handler(system::error_code(Errc::CtxClosed), {});
                m_service.remove(impl);
            });
        }

        if (!duration.is_not_a_date_time())
        {
            wait_deal.task = add_task(get_io_service(), duration, [&, _handler]() mutable {
                _handler(system::error_code(Errc::MakeDealTimeout), {});
                m_service.remove(impl);
            });
        }

        wait_deal.success = [this, _handler] (const std::vector<Order> order) mutable {
            _handler(system::error_code(), order);
        };

        wait_deal.error = [this, _handler] (Errc errc, const std::vector<Order> order) mutable {
            _handler(system::error_code(errc), order);
        };

        m_service.async_wait_deal(impl, entire, std::move(wait_deal_ptr));
    }

    static asio::io_service::id id;

    void construct(impl_type&)
    {
    }

    void destroy(impl_type& impl)
    {
        cancel(impl);
    }

private:
    detail::WaitDealService m_service;
    bool m_shutdowned;

    void shutdown_service()
    {
        m_shutdowned = true;
        m_service.shutdown();
    }
};

class WaitDeal : public asio::basic_io_object<WaitDealService>
{
    WaitDeal(asio::io_service& ios, TradingContext* ctx, int64_t order_id) :
        basic_io_object(ios)
    {
        if (ctx && ctx->closed())
        {
            throw CtxClosedException();
        }
        auto& impl = get_implementation();
        impl.ctx = ctx;
        impl.order_id = order_id;
        impl.deals = 0;
    }

public:
    WaitDeal(asio::io_service& ios, TradingContext& ctx, int64_t order_id) :
        WaitDeal(ios, &ctx, order_id)
    {
    }

    WaitDeal(asio::io_service& ios, int64_t order_id) :
        WaitDeal(ios, nullptr, order_id)
    {
    }

    WaitDeal(const WaitDeal&) = delete;
    WaitDeal& operator= (const WaitDeal&) = delete;

    void cancel()
    {
        get_service().cancel(get_implementation());
    }

    template<typename CompletionToken>
    auto async_wait(CompletionToken&& token)
    {
        return async_wait(false, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(const pt::time_duration& duration, CompletionToken&& token)
    {
        return async_wait(false, duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(bool entire, CompletionToken&& token)
    {
        return async_wait(entire, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(bool entire, const pt::time_duration& duration, CompletionToken&& token)
    {
        using handler_type = typename asio::handler_type
                                    <CompletionToken, void(system::error_code, std::vector<Order>)>::type;

        handler_type handler(std::forward<CompletionToken>(token));

        asio::async_result<handler_type> result(handler);

        get_service().async_wait(get_implementation(), entire, duration, std::move(handler));

        return result.get();
    }
};

}
}

#endif // ATRADE_ASYNC_2_WAIT_DEAL_HPP
