#include "marketsimulator.h"
#include "../utils.hpp"
#include "exceptions.h"

namespace atrade {
namespace backtester {

MarketSimulator::MarketSimulator() :
    MarketBase()
{
}

IMarketDataSource& MarketSimulator::add_data_source(std::unique_ptr<IMarketDataSource>&& data_source)
{
    m_data_sources.push_back(std::move(data_source));
    IMarketDataSource& ds = *(m_data_sources.back());
    for (const Security& s : ds.securities())
    {
        m_securities[s.isin_id()] = s;
    }
    auto stocks = ds.stocks();
    m_stocks.insert(stocks.begin(), stocks.end());
    return ds;
}

void MarketSimulator::add_order(const Order& order)
{
    double step = m_securities.at(order.isin_id).step();
    BOrder _order = from_order(order, step);
    if (_order.dir == OrderDir::Bid)
    {
        _add_order<BidOrder>(_order);
    }
    else if (_order.dir == OrderDir::Ask)
    {
        _add_order<AskOrder>(_order);
    }
    else
    {
        throw UndefinedOrderDirException();
    }
}

bool MarketSimulator::_del_order(int64_t order_id)
{
    bool result = false;
    std::vector<OrderEntry> entries;
    for (auto& e : m_orders)
    {
        auto& orders = e.second;
        auto it = splice_if(orders.begin(), orders.end(),
                            std::back_inserter(entries),
                            [&](const OrderEntry& e) { return e.order.order_id == order_id; });
        orders.erase(it, orders.end());
    }
    for (OrderEntry& e : entries)
    {
        result = true;
        BOrder& order = e.order;
        order.status = OrderStatus::Cancel;
        order.amount_rest = 0;
        if (m_order_handler)
        {
            double step = m_securities.at(order.isin_id).step();
            m_order_handler(to_order(order, step));
        }
    }
    if (entries.empty() && m_order_error_handler)
    {
        result = false;
        m_order_error_handler({order_id, OrderOperationType::Delete, 0});
    }
    return result;
}

void MarketSimulator::del_order(int64_t order_id)
{
    _del_order(order_id);
}

void MarketSimulator::move_order(int64_t order_id, int64_t new_order_id, double price, int volume)
{
    for (auto& e : m_orders)
    {
        auto& orders = e.second;
        auto it = std::find_if(orders.begin(), orders.end(),
                               [&](const OrderEntry& e) { return e.order.order_id == order_id; });
        if (it != orders.end())
        {
            int64_t isin_id = it->order.isin_id;
            OrderDir dir = it->order.dir;
            int _volume = (volume ? volume : it->order.volume);
            if( _del_order(order_id) )
            {
                add_order(Order(isin_id, new_order_id, dir, price, _volume));
            }
            return;
        }
    }
}

void MarketSimulator::req_securities()
{
    std::vector<Security> securities;
    std::transform(m_securities.begin(), m_securities.end(),
                   std::back_inserter(securities), second(m_securities));
    m_security_handler(securities);
}

void MarketSimulator::order_handler(const OrderHandler& handler)
{
    MarketBase::order_handler(handler);
    OrderHandler _handler = m_order_handler;
    m_order_handler = [this, _handler](const Order& order) mutable {
        m_orders_history[order.isin_id].push_back(order);
        _handler(order);
    };
}

std::vector<Quote> MarketSimulator::stocks(int64_t isin_id)
{
    return to_stocks(bstocks(isin_id), m_securities.at(isin_id).step());
}

template <typename T>
auto first_data_source(T& data_sources)
{
    using ptr_t = std::unique_ptr<IMarketDataSource>;
    return std::min_element(data_sources.begin(),
                            data_sources.end(),
                            [](const ptr_t& p1, const ptr_t& p2) {
                                return p1->datetime() < p2->datetime();
                            });
}

pt::ptime MarketSimulator::datetime()
{
    auto it = first_data_source(m_data_sources);
    if (it != m_data_sources.end())
    {
        return (*it)->datetime();
    }
    else
    {
        throw HasNoDataSourceException();
    }
}

void MarketSimulator::process()
{
    auto it = first_data_source(m_data_sources);
    if (it != m_data_sources.end())
    {
        (*it)->process();
    }
    else
    {
        throw HasNoDataSourceException();
    }
}

std::unordered_map<int64_t, std::vector<Order>> MarketSimulator::orders_history() const
{
    return m_orders_history;
}

std::vector<BQuote> MarketSimulator::bstocks(int64_t isin_id)
{
    return (m_stocks.at(isin_id))();
}

}
}
