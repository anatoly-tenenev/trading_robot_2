#ifndef ATRADE_ASYNC_2_TIMER_HPP
#define ATRADE_ASYNC_2_TIMER_HPP

#include "../headers.h"
#include "market_service.hpp"
#include "trading_context.hpp"
#include "error_code.hpp"
#include "../utils.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

class TimerService : public asio::io_service::service
{
public:

    struct implementation_type
    {
        TradingContext* ctx;
        pt::ptime datetime;
    };

    typedef implementation_type impl_type;

    struct _Timer
    {
        Task task;
        Subscription<CtxClosingHandler> on_ctx_closing;
        std::function<void(Errc errc)> cancel;
    };

    TimerService(asio::io_service& ios) :
        service(ios),
        m_shutdowned(false)
    {
    }

    ~TimerService()
    {
    }

    size_t cancel(impl_type& impl, Errc errc)
    {
        if (m_shutdowned || !m_timers.count(&impl))
        {
            return 0;
        }
        size_t result = m_timers[&impl].size();
        for (auto& e : m_timers[&impl])
        {
            _Timer& timer = *e.second;
            timer.cancel(errc);
        }
        m_timers.erase(&impl);
        return result;
    }

    template<typename Handler>
    void async_wait(impl_type& impl, Handler&& handler)
    {
        auto timer_ptr = std::make_unique<_Timer>();
        _Timer& timer = *timer_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc) mutable {
            get_io_service().post([errc, __handler]() mutable {
                __handler(errc);
            });
        };

        if (impl.ctx)
        {
            if (impl.ctx->closed())
            {
                _handler(system::error_code(Errc::CtxClosed));
                return;
            }

            timer.on_ctx_closing = impl.ctx->subscribe([this, _handler, &impl, &timer]() mutable {
                _handler(system::error_code(Errc::CtxClosed));
                remove_timer(impl, timer);
            });
        }

        timer.cancel = [this, _handler,  &impl, &timer](Errc errc) mutable {
            _handler(system::error_code(errc));
        };

        timer.task = add_task(get_io_service(), impl.datetime, [this, _handler,  &impl, &timer]() mutable {
            _handler(system::error_code());
            remove_timer(impl, timer);
        });

        m_timers[&impl].emplace(&timer, std::move(timer_ptr));
    }

    static asio::io_service::id id;

    void construct(impl_type&)
    {
    }

    void destroy(impl_type& impl)
    {
        cancel(impl, Errc::OperationAborted);
    }

private:
    umap<void*, umap<void*, std::unique_ptr<_Timer>>> m_timers;
    bool m_shutdowned;

    void shutdown_service()
    {
        m_shutdowned = true;
        m_timers.clear();
    }

    void remove_timer(impl_type& impl, _Timer& timer)
    {
        if (!m_timers.count(&impl))
        {
            return;
        }
        m_timers[&impl].erase(&timer);
        if (m_timers[&impl].empty())
        {
            m_timers.erase(&impl);
        }
    }
};

class Timer : public asio::basic_io_object<TimerService>
{
    Timer(asio::io_service& ios, TradingContext* ctx, const pt::ptime& datetime, const pt::time_duration& duration) :
        basic_io_object(ios)
    {
        if (ctx && ctx->closed())
        {
            throw CtxClosedException();
        }
        auto& impl = get_implementation();
        impl.ctx = ctx;
        if (datetime != pt::not_a_date_time)
        {
            impl.datetime = datetime;
        }
        else
        {
            impl.datetime = current_datetime(ios) + duration;
        }
    }

public:
    Timer(asio::io_service& ios, TradingContext& ctx, const pt::time_duration& duration) :
        Timer(ios, &ctx, pt::not_a_date_time, duration)
    {
    }

    Timer(asio::io_service& ios, const pt::time_duration& duration) :
        Timer(ios, nullptr, pt::not_a_date_time, duration)
    {
    }

    Timer(asio::io_service& ios, TradingContext& ctx, const pt::ptime& datetime) :
        Timer(ios, &ctx, datetime, pt::not_a_date_time)
    {
    }

    Timer(asio::io_service& ios, const pt::ptime& datetime) :
        Timer(ios, nullptr, datetime, pt::not_a_date_time)
    {
    }

    Timer(const Timer&) = delete;
    Timer& operator= (const Timer&) = delete;

    pt::ptime expires_at() const
    {
        return get_implementation().datetime;
    }

    size_t expires_at(const pt::ptime& datetime)
    {
        size_t result = cancel();
        get_implementation().datetime = datetime;
        return result;
    }

    size_t cancel()
    {
        return get_service().cancel(get_implementation(), Errc::OperationAborted);
    }

    template<typename CompletionToken>
    auto async_wait(CompletionToken&& token)
    {
        using handler_type = typename asio::handler_type
                                    <CompletionToken, void(system::error_code)>::type;

        handler_type handler(std::forward<CompletionToken>(token));

        asio::async_result<handler_type> result(handler);

        get_service().async_wait(get_implementation(), std::move(handler));

        return result.get();
    }
};

class Interval
{
    template <typename FunctionT>
    Interval(asio::io_service& ios, TradingContext* ctx, const pt::time_duration& duration, FunctionT&& func) :
        m_timer(ios, *ctx, duration),
        m_duration(duration),
        m_func(std::forward<FunctionT>(func))
    {
        if (m_func)
        {
            m_func();
        }
        m_wrapper = [&] (boost::system::error_code errc) {
            if (!errc)
            {
                if (m_func)
                {
                    m_func();
                }
                m_timer.expires_at(m_timer.expires_at() + m_duration);
                m_timer.async_wait(m_wrapper);
            }
        };
        m_timer.async_wait(m_wrapper);
    }

public:
    template <typename FunctionT>
    Interval(asio::io_service& ios, TradingContext& ctx, const pt::time_duration& duration, FunctionT&& func) :
        Interval(ios, &ctx, duration, std::forward<FunctionT>(func))
    {
    }

    Interval(asio::io_service& ios, TradingContext& ctx, const pt::time_duration& duration) :
        Interval(ios, &ctx, duration, nullptr)
    {
    }

    template <typename FunctionT>
    Interval(asio::io_service& ios, const pt::time_duration& duration, FunctionT&& func) :
        Interval(ios, nullptr, duration, std::forward<FunctionT>(func))
    {
    }

    Interval(asio::io_service& ios, const pt::time_duration& duration) :
        Interval(ios, nullptr, duration, nullptr)
    {
    }

private:
    Timer m_timer;
    pt::time_duration m_duration;
    std::function<void()> m_func;
    std::function<void(boost::system::error_code)> m_wrapper;
};

}
}

#endif // ATRADE_ASYNC_2_TIMER_HPP
