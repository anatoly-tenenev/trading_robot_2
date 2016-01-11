#ifndef ATRADE_ASYNC_2_ROUTINES_HPP
#define ATRADE_ASYNC_2_ROUTINES_HPP

#include "../headers.h"
#include "req_securities.hpp"

namespace atrade {
namespace async2 {

template<typename CompletionToken>
auto get_isin_id(asio::io_service& ios, TradingContext* ctx, const std::string& code, CompletionToken&& token)
{
    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code, int64_t)>::type;

    handler_type _handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(_handler);

    auto handler = [&ios, h=std::move(_handler), code]() mutable {
        int64_t isin_id = get_market_data(ios).get_isin_id(code);
        ios.post([isin_id, h=std::move(h)]() mutable {
            h(system::error_code(), isin_id);
        });
    };

    try
    {
        handler();
    }
    catch (NotFoundException&)
    {
        async_req_securities(ios,
                             *ctx,
                             pt::seconds(10),
                             [h=std::move(handler)]
                             (system::error_code, const std::vector<atrade::Security>&) mutable {
                                h();
                             });
    }

    return result.get();
}

template<typename CompletionToken>
auto get_isin_id(asio::io_service& ios, TradingContext& ctx, const std::string& code, CompletionToken&& token)
{
    return get_isin_id(ios, &ctx, code, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto get_isin_id(asio::io_service& ios, const std::string& code, CompletionToken&& token)
{
    return get_isin_id(ios, nullptr, code, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto get_security(asio::io_service& ios, TradingContext* ctx, int64_t isin_id, CompletionToken&& token)
{
    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code, Security)>::type;

    handler_type _handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(_handler);

    auto handler = [&ios, h=std::move(_handler), isin_id]() mutable {
        Security security = get_market_data(ios).security(isin_id);
        ios.post([security, h=std::move(h)]() mutable {
            h(system::error_code(), security);
        });
    };

    try
    {
        handler();
    }
    catch (std::out_of_range&)
    {
        async_req_securities(ios,
                             *ctx,
                             pt::seconds(10),
                             [h=std::move(handler)]
                             (system::error_code, const std::vector<atrade::Security>&) mutable {
                                h();
                             });
    }

    return result.get();
}

template<typename CompletionToken>
auto get_security(asio::io_service& ios, TradingContext& ctx, int64_t isin_id, CompletionToken&& token)
{
    return get_security(ios, &ctx, isin_id, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto get_security(asio::io_service& ios, int64_t isin_id, CompletionToken&& token)
{
    return get_security(ios, nullptr, isin_id, std::forward<CompletionToken>(token));
}

}
}

#endif // ATRADE_ASYNC_2_ROUTINES_HPP
