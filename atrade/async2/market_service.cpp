#include "market_service.hpp"

namespace atrade {
namespace async2 {

void init_market(asio::io_service& ios, IMarket& market)
{
    if (!asio::has_service<MarketService>(ios))
    {
        asio::add_service(ios, new MarketService(ios));
    }
    MarketService& service = asio::use_service<MarketService>(ios);
    service.init_market(market);
}

void process_market(asio::io_service &ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    service.process_market();
}

pt::ptime current_datetime(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.datetime();
}

pt::ptime _current_datetime(asio::io_service* ios)
{
    return current_datetime(*ios);
}

void add_order(asio::io_service& ios, const Order& order)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    service.add_order(order);
}

void del_order(asio::io_service& ios, int64_t order_id)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    service.del_order(order_id);
}

void move_order(asio::io_service& ios, int64_t order_id, int64_t new_order_id, double price, int volume)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    service.move_order(order_id, new_order_id, price, volume);
}

void req_securities(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    service.req_securities();
}

Subscription<OrdLogHandler> subscribe_ordlog(asio::io_service& ios, const OrdLogHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_ordlog(handler);
}

Subscription<OrdLogHandler> subscribe_ordlog(asio::io_service& ios, int64_t isin_id, const OrdLogHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_ordlog([=](int64_t _isin_id, const OrdLogEntry& entry) {
        if (isin_id != _isin_id)
        {
            return;
        }
        handler(_isin_id, entry);
    });
}

Subscription<OrderHandler> subscribe_order(asio::io_service& ios, const OrderHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_order(handler);
}

Subscription<OrderErrorHandler> subscribe_order_error(asio::io_service& ios, const OrderErrorHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_order_error(handler);
}

Subscription<TickHandler> subscribe_tick(asio::io_service& ios, const TickHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_tick(handler);
}

Subscription<StockHandler> subscribe_stock(asio::io_service& ios, const StockHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_stock(handler);
}

Subscription<SecurityHandler> subscribe_security(asio::io_service& ios, const SecurityHandler& handler)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.subscribe_security(handler);
}

Portfolio& get_portfolio(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.get_portfolio();
}

History& get_history(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.get_history();
}

MarketData& get_market_data(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.get_market_data();
}

std::unordered_set<int64_t> get_add_order_pendings(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.get_add_order_pendings();
}

IAsyncMarket& get_market(asio::io_service& ios)
{
    MarketService& service = asio::use_service<MarketService>(ios);
    return service.get_market();
}


}
}
