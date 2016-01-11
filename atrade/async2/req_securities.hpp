#ifndef ATRADE_ASYNC_2_REQ_SECURITIES_HPP
#define ATRADE_ASYNC_2_REQ_SECURITIES_HPP

#include "../headers.h"
#include "../globals.h"
#include "market_service.hpp"
#include "trading_context.hpp"
#include "error_code.hpp"
#include "../utils.hpp"
#include "exceptions.hpp"
#include "../security.h"

namespace atrade {
namespace async2 {

class ReqSecuritiesService : public asio::io_service::service
{
public:

    struct _ReqSecurities
    {
        Task task;
        Subscription<CtxClosingHandler> on_ctx_closing;
        std::function<void(const std::vector<Security>&)> success;
    };

    ReqSecuritiesService(asio::io_service& ios) :
        service(ios)
    {
        m_security_subscription = subscribe_security(ios, [this] (const std::vector<Security>& securities) {
            if (!m_req_securities.empty())
            {
                for (auto& e : m_req_securities)
                {
                    e.second->success(securities);
                }
                m_req_securities.clear();
            }
        });
    }

    ~ReqSecuritiesService()
    {
    }

    template<typename Handler>
    void async_req_securities(TradingContext* ctx, const pt::time_duration& duration, bool force, Handler&& handler)
    {
        auto req_securities_ptr = std::make_unique<_ReqSecurities>();
        _ReqSecurities& req_securities = *req_securities_ptr;

        auto __handler = std::move(handler);
        auto _handler = [this, __handler] (system::error_code errc, const std::vector<Security>& securities) mutable {
            get_io_service().post([errc, securities, __handler]() mutable {
                __handler(errc, securities);
            });
        };

        if (ctx && ctx->closed())
        {
            _handler(system::error_code(Errc::CtxClosed), {});
            return;
        }

        if (!force)
        {
            std::vector<Security> securities = get_market_data(get_io_service()).securities();
            if (!securities.empty())
            {
                 _handler(system::error_code(), securities);
                 return;
            }
        }

        async2::req_securities(get_io_service());

        if (ctx)
        {
            req_securities.on_ctx_closing = ctx->subscribe([this, _handler, &req_securities]() mutable {
                _handler(system::error_code(Errc::CtxClosed), {});
                m_req_securities.erase(&req_securities);
            });
        }

        if (!duration.is_not_a_date_time())
        {
            req_securities.task = add_task(get_io_service(), duration, [this, _handler, &req_securities]() mutable {
                _handler(system::error_code(Errc::ReqSecuritiesTimeout), {});
                m_req_securities.erase(&req_securities);
            });
        }

        req_securities.success = [_handler](const std::vector<Security>& securities) mutable {
            _handler(system::error_code(), securities);
        };

        m_req_securities.emplace(&req_securities, std::move(req_securities_ptr));
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
    std::unordered_map<void*, std::unique_ptr<_ReqSecurities>> m_req_securities;
    Subscription<SecurityHandler> m_security_subscription;

    void shutdown_service()
    {
    }
};

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, TradingContext* ctx, const pt::time_duration& duration,
                          bool force, CompletionToken&& token)
{
    if (!asio::has_service<ReqSecuritiesService>(ios))
    {
        asio::add_service<ReqSecuritiesService>(ios, new ReqSecuritiesService(ios));
    }

    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code, std::vector<Security>)>::type;

    handler_type handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(handler);

    ReqSecuritiesService& service = asio::use_service<ReqSecuritiesService>(ios);

    service.async_req_securities(ctx, duration, force, std::move(handler));

    return result.get();
}

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, TradingContext& ctx, const pt::time_duration& duration,
                          bool force, CompletionToken&& token)
{
    return async_req_securities(ios, &ctx, duration, force, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, const pt::time_duration& duration, bool force, CompletionToken&& token)
{
    return async_req_securities(ios, nullptr, duration, force, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, TradingContext& ctx, const pt::time_duration& duration, CompletionToken&& token)
{
    return async_req_securities(ios, &ctx, duration, false, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, const pt::time_duration& duration, CompletionToken&& token)
{
    return async_req_securities(ios, nullptr, duration, false, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, TradingContext& ctx, CompletionToken&& token)
{
    return async_req_securities(ios, &ctx, pt::not_a_date_time, false, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_req_securities(asio::io_service& ios, CompletionToken&& token)
{
    return async_req_securities(ios, nullptr, pt::not_a_date_time, false, std::forward<CompletionToken>(token));
}

}
}

#endif // ATRADE_ASYNC_2_REQ_SECURITIES_HPP
