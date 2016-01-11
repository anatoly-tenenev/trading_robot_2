#include "portfolio.hpp"
#include "../utils.hpp"

namespace atrade {
namespace async2 {

Portfolio::Portfolio(IAsyncMarket& market) :
    m_market(&market),
    m_orders(m_market->history().orders())
{
    m_order_subscription = m_market->subscribe_order([this] (const Order& order) {
        m_orders[order.isin_id].push_back(order);
    });
}

Portfolio::~Portfolio()
{
}

size_t Portfolio::deals_amount(int64_t isin_id)
{
    if (m_orders.count(isin_id))
    {
        return calc_deals_amount(m_orders[isin_id]);
    }
    else
    {
        return 0;
    }
}

double Portfolio::profit(int64_t isin_id)
{
    if (m_orders.count(isin_id))
    {
        return calc_profit(m_orders[isin_id]);
    }
    else
    {
        return 0;
    }
}

std::vector<Order> _position(const std::vector<Order>& orders)
{
    std::deque<Order> position;
    for (const Order& order : orders)
    {
        if (order.status != OrderStatus::Fill)
        {
            continue;
        }
        if (!position.size())
        {
            position.push_back(order);
            continue;
        }
        if (position.back().dir == order.dir)
        {
            position.push_back(order);
        }
        else
        {
            Order _order = order;
            while (position.size() && _order.volume)
            {
                Order& first = position.front();
                if (first.volume > _order.volume)
                {
                    first.volume -= _order.volume;
                    _order.volume = 0;
                }
                else
                {
                    _order.volume -= first.volume;
                    position.pop_front();
                }
            }
            if (_order.volume)
            {
                position.push_back(_order);
            }
        }
    }
    return std::vector<Order>(position.begin(), position.end());
}

std::vector<Order> Portfolio::position(int64_t isin_id)
{
    if (m_orders.count(isin_id))
    {
        return _position(m_orders[isin_id]);
    }
    else
    {
        return {};
    }
}

Order Portfolio::closing_order(int64_t isin_id)
{
    std::vector<Order> _position = position(isin_id);
    if (!_position.size())
    {
        return {};
    }
    else
    {
        int volume = 0;
        for (const Order& order : _position)
        {
            volume += order.volume;
        }
        return Order(isin_id,
                     _position.front().dir == OrderDir::Bid ? OrderDir::Ask : OrderDir::Bid,
                     volume);
    }
}

std::vector<Order> _orders(const std::vector<Order>& orders)
{
    std::unordered_map<int64_t, Order> _orders;
    for (const Order& order : orders)
    {
        if (order.status == OrderStatus::Cancel || !order.amount_rest)
        {
            _orders.erase(order.order_id);
        }
        else
        {
            _orders[order.order_id] = order;
        }
    }
    std::vector<Order> result;
    for (auto& e : _orders)
    {
        result.push_back(e.second);
    }
    return result;
}

std::vector<Order> Portfolio::orders(int64_t isin_id)
{
    if (m_orders.count(isin_id))
    {
        return _orders(m_orders[isin_id]);
    }
    else
    {
        return {};
    }
}

}
}

