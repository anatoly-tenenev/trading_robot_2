#include "utils.hpp"

namespace atrade {

double calc_profit(const std::vector<Order>& orders)
{
    std::vector<double> bids;
    std::vector<double> asks;
    for (const Order& order : orders)
    {
        if (order.status != OrderStatus::Fill)
        {
            continue;
        }
        if (order.dir == OrderDir::Bid)
        {
            bids.insert(bids.end(), order.volume, order.deal_price);
        }
        else if (order.dir == OrderDir::Ask)
        {
            asks.insert(asks.end(), order.volume, order.deal_price);
        }
    }
    size_t size = std::min(bids.size(), asks.size());
    double profit = 0.0;
    for (size_t i = 0; i < size; ++i)
    {
        profit += (asks[i] - bids[i]);
    }
    return profit;
}

size_t calc_deals_amount(const std::vector<Order>& orders)
{
    return std::count_if(orders.begin(), orders.end(),
                         [](const Order& order) { return order.status == OrderStatus::Fill; });
}

double round(double value, int frac)
{
    double d = static_cast<double>(std::pow(10, frac));
    return std::round(value*d)/d;
}

double price_round(double price)
{
    return round(price, 6);
}

}
