#ifndef ATRADE_ASYNC_2_HISTORY_H
#define ATRADE_ASYNC_2_HISTORY_H

#include "../headers.h"
#include "subscription.hpp"
#include "../imarket.h"

namespace atrade {
namespace async2 {

class History
{
public:
    History(IAsyncMarket& market);

    History(const History&) = delete;
    History& operator= (const History&) = delete;

    History(History&&) = default;
    History& operator= (History&&) = default;

    bool has_order(int64_t order_id) const;
    std::vector<Order> get_order_by_id(int64_t order_id) const;
    std::vector<Order> get_orders_by_isin_id(int64_t isin_id) const;
    std::unordered_map<int64_t, std::vector<Order>> orders() const;

    bool has_order_error(int64_t order_id) const;
    std::vector<OrderError> get_order_error_by_id(int64_t order_id) const;
    std::vector<OrderError> order_errors() const;

private:
    IAsyncMarket* m_market;
    Subscription<OrderHandler> m_order_subscription;
    Subscription<OrderErrorHandler> m_order_error_subscription;
    std::unordered_map<int64_t, std::vector<Order>> m_orders;
    std::unordered_map<int64_t, std::vector<Order>> m_isin_orders;
    std::vector<OrderError> m_order_errors;
};

}
}

#endif // ATRADE_ASYNC_2_HISTORY_H
