#ifndef ATRADE_ASYNC_2_SPAWNER_HPP
#define ATRADE_ASYNC_2_SPAWNER_HPP

#include "../headers.h"
#include "indicator.hpp"
#include "watcher.hpp"
#include "../utils.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

class CounterIndicator : public BooleanIndicator
{
public:
    CounterIndicator() :
        BooleanIndicator(true),
        m_counter(0)
    {
    }

    void inc()
    {
        ++m_counter;
        value(false);
    }

    void dec()
    {
        --m_counter;
        if (!m_counter)
        {
            value(true);
        }
    }

private:
    size_t m_counter;
};

class Spawner
{
public:
    typedef std::function<void(system::error_code)> Callback;

    Spawner(asio::io_service& ios, TradingContext* ctx) :
        m_ios(ios),
        m_general_ctx(ctx),
        m_ctx(std::make_unique<TradingContext>(m_general_ctx)),
        m_watcher(std::make_shared<Watcher<CounterIndicator, std::unique_ptr<CounterIndicator>>>(
                                   m_ios, m_general_ctx, std::make_unique<CounterIndicator>()))
    {

    }

    Spawner(asio::io_service& ios, TradingContext& ctx) :
        Spawner(ios, &ctx)
    {
    }

    Spawner(asio::io_service& ios) :
        Spawner(ios, nullptr)
    {
    }

    template <typename Function>
    void spawn(Function&& function)
    {
        this->spawn(std::forward<Function>(function), nullptr);
    }

    template <typename Function>
    void spawn(Function&& function, const Callback& callback)
    {
        auto ctx = std::make_shared<TradingContext>(*m_ctx);
        auto watcher = m_watcher;
        watcher->indicator().inc();
        asio::spawn(m_ios, [this, ctx, watcher, function, callback] (asio::yield_context yield) mutable {
            system::error_code errc{};
            ScopeWatcher scope_watcher([&errc, watcher, callback]() {
                if (callback)
                {
                    callback(errc);
                }
                watcher->indicator().dec();
            });
            try
            {
                function(*ctx, yield);
            }
            catch (system::system_error& e)
            {
                errc = system::error_code(e.code());
                if (e.code() != Errc::CtxClosed)
                {
                    std::cout << e.code().message() << std::endl;
                    if ( ! ctx->error(e) )
                    {
                        throw;
                    }
                }
            }
            catch (CtxClosedException&)
            {
                errc = system::error_code(Errc::CtxClosed);
            }
        });
    }

    TradingContext& ctx()
    {
        return *m_ctx;
    }

    void reset()
    {
        m_ctx->close();
        m_ctx = std::make_unique<TradingContext>(m_general_ctx);
    }

    void cancel_joins()
    {
        m_watcher->cancel();
    }

    template<typename CompletionToken>
    auto async_join(CompletionToken&& token)
    {
        return async_join(pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_join(const pt::time_duration& duration, CompletionToken&& token)
    {
        return m_watcher->async_wait(true, duration, std::forward<CompletionToken>(token));
    }

private:
    asio::io_service& m_ios;
    TradingContext m_general_ctx;
    std::unique_ptr<TradingContext> m_ctx;
    std::shared_ptr<Watcher<CounterIndicator,
                            std::unique_ptr<CounterIndicator>>> m_watcher;
};

}
}

#endif // ATRADE_ASYNC_2_SPAWNER_HPP

