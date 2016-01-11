#include "history.hpp"
#include "asyncmarket.hpp"

namespace atrade {
namespace async2 {

History::History(IAsyncMarket& market) :
    m_market(&market)
{
    m_order_subscription = market.subscribe_order([this] (const Order& order) {
        m_orders[order.order_id].push_back(order);
        m_isin_orders[order.isin_id].push_back(order);
    });
    m_order_error_subscription = market.subscribe_order_error([this] (const OrderError& error) mutable {
        m_order_errors.push_back(error);
    });
}

bool History::has_order(int64_t order_id) const
{
    return m_orders.count(order_id) > 0;
}

std::vector<Order> History::get_order_by_id(int64_t order_id) const
{
    return m_orders.at(order_id);
}

std::vector<Order> History::get_orders_by_isin_id(int64_t isin_id) const
{
    return m_isin_orders.at(isin_id);
}

std::unordered_map<int64_t, std::vector<Order>> History::orders() const
{
    return m_isin_orders;
}

bool History::has_order_error(int64_t order_id) const
{
    auto it = std::find_if(m_order_errors.begin(), m_order_errors.end(),
                           [&order_id](const auto& error) { return error.order_id == order_id; });
    return it != m_order_errors.end();
}

std::vector<OrderError> History::get_order_error_by_id(int64_t order_id) const
{
    std::vector<OrderError> errors;
    std::copy_if(m_order_errors.begin(), m_order_errors.end(),
                 std::back_inserter(errors),
                 [&order_id](const auto& error) { return error.order_id == order_id; });
    return errors;
}

std::vector<OrderError> History::order_errors() const
{
    return m_order_errors;
}

}
}
