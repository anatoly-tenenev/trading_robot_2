#ifndef ATRADE_ASYNC_2_PORTFOLIO_H
#define ATRADE_ASYNC_2_PORTFOLIO_H

#include "../headers.h"
#include "asyncmarket.hpp"

namespace atrade {
namespace async2 {

class Portfolio
{
public:
    Portfolio(IAsyncMarket& market);
    ~Portfolio();

    size_t deals_amount(int64_t isin_id);

    double profit(int64_t isin_id);

    std::vector<Order> position(int64_t isin_id);

    Order closing_order(int64_t isin_id);

    // существующие на рынке заявки пользователя
    std::vector<Order> orders(int64_t isin_id);

private:
    IAsyncMarket* m_market;
    Subscription<OrderHandler> m_order_subscription;
    std::unordered_map<int64_t, std::vector<Order>> m_orders;
};

}
}

#endif // ATRADE_ASYNC_2_PORTFOLIO_H
