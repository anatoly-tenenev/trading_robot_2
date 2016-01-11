#ifndef ATRADE_ASYNC_2_MOVE_ORDER_HPP
#define ATRADE_ASYNC_2_MOVE_ORDER_HPP

#include "add_order.hpp"
#include "cancel_order.hpp"

namespace atrade {
namespace async2 {

class MoveOrderService : public asio::io_service::service
{
public:

    MoveOrderService(asio::io_service& ios) :
        service(ios),
        m_ios(ios)
    {
        if (!asio::has_service<AddOrderService>(ios))
        {
            asio::add_service<AddOrderService>(ios, new AddOrderService(ios));
        }
        if (!asio::has_service<CancelOrderService>(ios))
        {
            asio::add_service<CancelOrderService>(ios, new CancelOrderService(ios));
        }
    }

    ~MoveOrderService()
    {
    }

    template<typename Handler>
    void async_move_order(TradingContext* ctx, IAsyncMarket* market,
                          int64_t order_id, int64_t new_order_id, double price, int volume,
                          const pt::time_duration& duration, Handler&& handler)
    {
        auto __handler = std::move(handler);
        auto _handler = [&, __handler] (system::error_code errc, int64_t order_id) mutable {
            get_io_service().post([errc, order_id, __handler]() mutable {
                __handler(errc, order_id);
            });
        };

        asio::spawn(m_ios, [=] (asio::yield_context yield) mutable {
            int64_t _new_order_id = new_order_id;
            if (!_new_order_id)
            {
                _new_order_id = generate_order_id();
            }
            if (market)
            {
                market->move_order(order_id, _new_order_id, price, volume);
            }
            else
            {
                async2::move_order(m_ios, order_id, _new_order_id, price, volume);
            }
            try
            {
                async2::async_wait_cancel(m_ios, *ctx, order_id, duration, yield);
                async2::async_wait_order(m_ios, *ctx, _new_order_id, duration, yield);
            }
            catch (system::system_error& e)
            {
                _handler(system::error_code(e.code()), _new_order_id);
                return;
            }
            _handler(system::error_code(), _new_order_id);
        });
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
    asio::io_service& m_ios;

    void shutdown_service()
    {
    }
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext* ctx, IAsyncMarket* market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    if (!asio::has_service<MoveOrderService>(ios))
    {
        asio::add_service<MoveOrderService>(ios, new MoveOrderService(ios));
    }

    using handler_type = typename asio::handler_type
                                <CompletionToken, void(system::error_code, int64_t)>::type;

    handler_type handler(std::forward<CompletionToken>(token));

    asio::async_result<handler_type> result(handler);

    MoveOrderService& service = asio::use_service<MoveOrderService>(ios);

    service.async_move_order(ctx, market, order_id, new_order_id, price, volume, duration, std::move(handler));

    return result.get();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, nullptr, order_id, new_order_id, price, volume,
                            duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    return async_move_order(ios, nullptr, nullptr, order_id, new_order_id, price, volume,
                            duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      CompletionToken&& token)
{
    return async_move_order(ios, &ctx, nullptr, order_id, new_order_id, price, volume,
                            pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, int64_t order_id, int64_t new_order_id, double price, int volume,
                      CompletionToken&& token)
{
    return async_move_order(ios, nullptr, nullptr, order_id, new_order_id, price, volume,
                            pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket* market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, market, order_id, new_order_id, price, volume,
                            duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket& market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, &market, order_id, new_order_id, price, volume,
                            duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, IAsyncMarket* market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    return async_move_order(ios, nullptr, market, order_id, new_order_id, price, volume,
                            duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, IAsyncMarket& market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      const pt::time_duration& duration, CompletionToken&& token)
{
    return async_move_order(ios, nullptr, &market, order_id, new_order_id, price, volume,
                            duration, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket* market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      CompletionToken&& token)
{
    return async_move_order(ios, &ctx, market, order_id, new_order_id, price, volume,
                            pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket& market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      CompletionToken&& token)
{
    return async_move_order(ios, &ctx, &market, order_id, new_order_id, price, volume,
                            pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, IAsyncMarket* market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      CompletionToken&& token)
{
    return async_move_order(ios, nullptr, market, order_id, new_order_id, price, volume,
                            pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, IAsyncMarket& market,
                      int64_t order_id, int64_t new_order_id, double price, int volume,
                      CompletionToken&& token)
{
    return async_move_order(ios, nullptr, &market, order_id, new_order_id, price, volume,
                            pt::not_a_date_time, std::forward<CompletionToken>(token));
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket* market,
                      int64_t order_id, double price, int volume, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, market, order_id, 0, price, volume, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket& market,
                      int64_t order_id, double price, int volume, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, &market, order_id, 0, price, volume, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket* market,
                      int64_t order_id, double price, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, market, order_id, 0, price, 0, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

template<typename CompletionToken>
auto async_move_order(asio::io_service& ios, TradingContext& ctx, IAsyncMarket& market,
                      int64_t order_id, double price, CompletionToken&& token)
{
    return async_move_order(ios, &ctx, &market, order_id, 0, price, 0, pt::not_a_date_time, std::forward<CompletionToken>(token));
}

}
}

#endif // ATRADE_ASYNC_2_MOVE_ORDER_HPP
