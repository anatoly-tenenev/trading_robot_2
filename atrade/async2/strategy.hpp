#ifndef ATRADE_ASYNC2_STRATEGY_HPP
#define ATRADE_ASYNC2_STRATEGY_HPP

#include "../headers.h"
#include "market_service.hpp"
#include "add_order.hpp"
#include "move_order.hpp"

namespace atrade {
namespace async2 {

class IStrategy
{
public:
    virtual ~IStrategy() {}
    virtual void run(TradingContext& ctx, asio::yield_context yield) = 0;
    virtual asio::io_service& ios() = 0;
    virtual IAsyncMarket* market() const = 0;
    virtual pt::ptime current_datetime() = 0;
};

class Strategy : public IStrategy
{
public:
    Strategy(asio::io_service& ios, atrade::async2::IAsyncMarket* market) :
        m_ios(&ios),
        m_market(market)
    {
    }

    template<typename CompletionToken>
    auto async_add_order(TradingContext& ctx, const Order& order, const pt::time_duration& duration, CompletionToken&& token)
    {
        return async2::async_add_order(*m_ios, ctx, m_market, order, duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_add_order(TradingContext& ctx, const Order& order, CompletionToken&& token)
    {
        return async_add_order(ctx, order, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_move_order(TradingContext& ctx, int64_t order_id, int64_t new_order_id, double price, int volume,
                          const pt::time_duration& duration, CompletionToken&& token)
    {
        return async2::async_move_order(*m_ios, ctx, m_market, order_id, new_order_id, price, volume,
                                        duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_move_order(TradingContext& ctx, int64_t order_id, int64_t new_order_id, double price, int volume, CompletionToken&& token)
    {
        return async_move_order(ctx, order_id, new_order_id, price, volume, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_move_order(TradingContext& ctx, int64_t order_id, double price, int volume, const pt::time_duration& duration, CompletionToken&& token)
    {
        return async_move_order(ctx, order_id, 0, price, volume, duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_move_order(TradingContext& ctx, int64_t order_id, double price, int volume, CompletionToken&& token)
    {
        return async_move_order(ctx, order_id, 0, price, volume, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_move_order(TradingContext& ctx, int64_t order_id, double price, const pt::time_duration& duration, CompletionToken&& token)
    {
        return async_move_order(ctx, order_id, 0, price, 0, duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_move_order(TradingContext& ctx, int64_t order_id, double price, CompletionToken&& token)
    {
        return async_move_order(ctx, order_id, 0, price, 0, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_cancel_order(TradingContext& ctx, int64_t order_id, const pt::time_duration& duration, CompletionToken&& token)
    {
        return async2::async_cancel_order(*m_ios, &ctx, order_id, duration, std::forward<CompletionToken>(token));
    }

    template<typename CompletionToken>
    auto async_cancel_order(TradingContext& ctx, int64_t order_id, CompletionToken&& token)
    {
        return async_cancel_order(ctx, order_id, pt::not_a_date_time, std::forward<CompletionToken>(token));
    }

    asio::io_service& ios()
    {
        return *m_ios;
    }

    IAsyncMarket* market() const
    {
        return m_market;
    }

    pt::ptime current_datetime()
    {
        return async2::current_datetime(ios());
    }

private:
    asio::io_service* m_ios;
    IAsyncMarket* m_market;
};

}
}

#endif // ATRADE_ASYNC2_STRATEGY_HPP

