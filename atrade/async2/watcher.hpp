#ifndef ATRADE_ASYNC_2_WATCHER_HPP
#define ATRADE_ASYNC_2_WATCHER_HPP

#include "../headers.h"
#include "../globals.h"
#include "market_service.hpp"
#include "trading_context.hpp"
#include "error_code.hpp"
#include "../utils.hpp"
#include "exceptions.hpp"
#include "indicator.hpp"

namespace atrade {
namespace async2 {

template <typename IndicatorT, typename IndicatorPtrT>
class WatcherService : public asio::io_service::service
{
public:
    typedef typename IndicatorT::value_type value_type;
    typedef typename IndicatorT::handler_type handler_type;

    struct implementation_type
    {
        TradingContext* ctx;
        IndicatorPtrT indicator;
    };

    typedef implementation_type impl_type;

    struct _Watcher
    {
        Task task;
        Subscription<CtxClosingHandler> on_ctx_closing;
        Subscription<handler_type> subscription;
        std::function<void()> cancel;
    };

    WatcherService(asio::io_service& ios) :
        service(ios),
        m_shutdowned(false)
    {
    }

    ~WatcherService()
    {
    }

    void cancel(impl_type& impl)
    {
        if (m_shutdowned || !m_watchers.count(&impl))
        {
            return;
        }
        for (auto& e : m_watchers[&impl])
        {
            _Watcher& watcher = *e.second;
            watcher.cancel();
        }
        m_watchers.erase(&impl);
    }

    template<typename Pred, typename Handler>
    void async_wait(impl_type& impl, Pred&& pred, const pt::time_duration& duration, Handler&& handler)
    {
        auto watcher_ptr = std::make_unique<_Watcher>();
        _Watcher& watcher = *watcher_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc, value_type value) mutable {
            get_io_service().post([errc, value, __handler]() mutable {
                __handler(errc, value);
            });
        };

        value_type current_value = impl.indicator->value();
        if (pred(current_value))
        {
            _handler(system::error_code(), current_value);
            return;
        }

        if (impl.ctx)
        {
            if (impl.ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed), value_type{});
                return;
            }

            watcher.on_ctx_closing = impl.ctx->subscribe([&, _handler]() mutable {
                _handler(system::error_code(Errc::CtxClosed), value_type{});
                remove_watcher(impl, watcher);
            });
        }

        watcher.cancel = [_handler]() mutable {
            _handler(system::error_code(Errc::OperationAborted), value_type{});
        };

        if (!duration.is_not_a_date_time())
        {
            watcher.task = add_task(get_io_service(), duration, [&, _handler]() mutable {
                _handler(system::error_code(Errc::WatcherTimeout), value_type{});
                remove_watcher(impl, watcher);
            });
        }

        watcher.subscription = impl.indicator->subscribe([&, pred, _handler] (value_type value) mutable {
            if (pred(value))
            {
                _handler(system::error_code(), value);
                remove_watcher(impl, watcher);
            }
        });

        m_watchers[&impl].emplace(&watcher, std::move(watcher_ptr));
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
    umap<void*, umap<void*, std::unique_ptr<_Watcher>>> m_watchers;
    bool m_shutdowned;

    void shutdown_service()
    {
        m_shutdowned = true;
        m_watchers.clear();
    }

    void remove_watcher(impl_type& impl, _Watcher& watcher)
    {
        if (!m_watchers.count(&impl))
        {
            return;
        }
        m_watchers[&impl].erase(&watcher);
        if (m_watchers[&impl].empty())
        {
            m_watchers.erase(&impl);
        }
    }
};

template <typename IndicatorT, typename IndicatorPtrT>
asio::io_service::id WatcherService<IndicatorT, IndicatorPtrT>::id;

template <typename IndicatorT, typename IndicatorPtrT = IndicatorT*>
class Watcher : public asio::basic_io_object<WatcherService<IndicatorT, IndicatorPtrT>>
{
    typedef asio::basic_io_object<WatcherService<IndicatorT, IndicatorPtrT>> super;
    typedef typename IndicatorT::value_type value_type;

    template <typename _IndicatorPtr>
    Watcher(asio::io_service& ios, TradingContext* ctx, _IndicatorPtr&& indicator) :
        super(ios)
    {
        if (ctx && ctx->closed())
        {
            throw CtxClosedException();
        }
        auto& impl = super::get_implementation();
        impl.ctx = ctx;
        impl.indicator = std::forward<_IndicatorPtr>(indicator);
    }

public:
    template <typename _IndicatorPtr>
    Watcher(asio::io_service& ios, TradingContext& ctx, _IndicatorPtr&& indicator) :
        Watcher(ios, &ctx, std::forward<_IndicatorPtr>(indicator))
    {
    }

    template <typename _IndicatorPtr>
    Watcher(asio::io_service& ios, _IndicatorPtr&& indicator) :
        Watcher(ios, nullptr, std::forward<_IndicatorPtr>(indicator))
    {
    }

    Watcher(const Watcher&) = delete;
    Watcher& operator= (const Watcher&) = delete;

    void cancel()
    {
        super::get_service().cancel(super::get_implementation());
    }

    template<typename CompletionToken>
    auto async_wait(CompletionToken&& token)
    {
        return async_wait(pt::time_duration(pt::not_a_date_time), std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(pt::time_duration duration, CompletionToken&& token)
    {
        value_type current_value = super::get_implementation().indicator->value();
        auto pred = [current_value](const value_type& value) {
            return value != current_value;
        };
        return async_wait(std::move(pred), duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(value_type target, CompletionToken&& token)
    {
        return async_wait(target, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(value_type target, const pt::time_duration& duration, CompletionToken&& token)
    {
        auto pred = [target](const value_type& value) {
            return value == target;
        };
        return async_wait(std::move(pred), duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait_not(value_type target, CompletionToken&& token)
    {
        return async_wait_not(target, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait_not(value_type target, const pt::time_duration& duration, CompletionToken&& token)
    {
        auto pred = [target](const value_type& value) {
            return value != target;
        };
        return async_wait(std::move(pred), duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_wait(bool target_set, value_type target, const pt::time_duration& duration, CompletionToken&& token)
    {
        if (target_set)
        {
            return async_wait(target, duration, std::forward<CompletionToken>(token));
        }
        else
        {
            return async_wait(pt::time_duration(duration), std::forward<CompletionToken>(token));
        }
    }

    template<typename Pred, typename CompletionToken>
    auto async_wait(Pred&& pred, CompletionToken&& token)
    {
        return async_wait(std::forward<Pred>(pred), pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename Pred, typename CompletionToken>
    auto async_wait(Pred&& pred, const pt::time_duration& duration, CompletionToken&& token)
    {
        using handler_type = typename asio::handler_type
                                    <CompletionToken, void(system::error_code, value_type)>::type;

        handler_type handler(std::forward<CompletionToken>(token));

        asio::async_result<handler_type> result(handler);

        super::get_service().async_wait(super::get_implementation(), std::forward<Pred>(pred), duration, std::move(handler));

        return result.get();
    }

    IndicatorT& indicator()
    {
        return *super::get_implementation().indicator;
    }
};

}
}

#endif // ATRADE_ASYNC_2_WATCHER_HPP
