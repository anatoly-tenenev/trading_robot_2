#ifndef ATRADE_MARKETSIMULATOR_H
#define ATRADE_MARKETSIMULATOR_H

#include "../headers.h"
#include "../marketbase.h"
#include "../security.h"
#include "imarketdatasource.h"
#include "types.h"
#include "utils.hpp"
#include "exceptions.h"

namespace atrade {
namespace backtester {

struct OrderEntry
{
    BOrder order;
    int orders_before;
};

class MarketSimulator : public MarketBase
{
public:
    MarketSimulator();

    virtual IMarketDataSource& add_data_source(std::unique_ptr<IMarketDataSource>&& data_source);

    void add_order(const Order& order) override;
    void del_order(int64_t order_id) override;
    void move_order(int64_t order_id, int64_t new_order_id, double price, int volume) override;
    void req_securities() override;

    void order_handler(const OrderHandler& handler) override;

    std::vector<Quote> stocks(int64_t isin_id) override;
    pt::ptime datetime() override;
    void process() override;

    std::unordered_map<int64_t, std::vector<Order>> orders_history() const;

protected:
    std::unordered_map<int64_t, std::vector<OrderEntry>> m_orders;
    std::unordered_map<int64_t, Security> m_securities;
    std::vector<std::unique_ptr<IMarketDataSource>> m_data_sources;
    std::unordered_map<int64_t, std::vector<Order>> m_orders_history;
    fumap<int64_t, std::vector<BQuote>()> m_stocks;

    bool _del_order(int64_t order_id);
    std::vector<BQuote> bstocks(int64_t isin_id);

protected:

    class BidOrder
    {
    public:
        static OrderDir order_dir() { return OrderDir::Bid; }

        static QuoteType quote_type() { return QuoteType::Ask; }

        template <typename T>
        static bool before(T price1, T price2)
        {
            return price1 < price2;
        }
    };

    class AskOrder
    {
    public:
        static OrderDir order_dir() { return OrderDir::Ask; }

        static QuoteType quote_type() { return QuoteType::Bid; }

        template <typename T>
        static bool before(T price1, T price2)
        {
            return price1 > price2;
        }
    };


    template <class OrderTraits, typename TOrder>
    void _add_order(const TOrder& _order)
    {
        using TQuote = typename decltype(bstocks(0))::value_type;
        if (_order.volume <= 0)
        {
            return;
        }
        const int64_t isin_id = _order.isin_id;
        double step = m_securities.at(isin_id).step();
        std::vector<OrderEntry>& orders = m_orders[isin_id];
        std::vector<OrderEntry> other;
        std::copy_if(orders.begin(), orders.end(),
                     std::back_inserter(other),
                     [](const OrderEntry& e) { return e.order.dir == !OrderTraits::order_dir(); });
        auto first_order =
                std::min_element(other.begin(), other.end(),
                                 [](const OrderEntry& e1, const OrderEntry& e2) {
                                        return OrderTraits::before(e1.order.price, e2.order.price); });
        std::vector<TQuote> _stocks = bstocks(isin_id);
        std::vector<TQuote> stocks;
        std::copy_if(_stocks.begin(), _stocks.end(),
                     std::back_inserter(stocks),
                     [](const TQuote& q) { return (q.type == OrderTraits::quote_type()) &&
                                                  (q.volume > 0); });
        std::sort(stocks.begin(), stocks.end(),
                  [](const TQuote& q1, const TQuote& q2) { return OrderTraits::before(q1.price, q2.price); });
        if ((first_order != other.end()) && !stocks.empty() &&
            !OrderTraits::before(stocks[0].price, first_order->order.price) &&
            !OrderTraits::before(_order.price, first_order->order.price))
        {
            if (m_order_error_handler)
            {
                m_order_error_handler({_order.order_id, OrderOperationType::Add, 0});
            }
            return;
        }
        TOrder order = _order;
        order.status = OrderStatus::Add;
        order.amount_rest = order.volume;
        if (m_order_handler)
        {
            m_order_handler(to_order(order, step));
        }
        std::vector<TOrder> executed_orders;
        for (auto best_quote = stocks.begin(); order.volume > 0; ++best_quote)
        {
            if (best_quote == stocks.end())
            {
                throw AddOrderException();
            }
            if (OrderTraits::before(order.price, best_quote->price) && order.type != OrderType::Market)
            {
                if (order.type == OrderType::Limit)
                {
                    OrderEntry entry = { order , 0 };
                    auto it = std::find_if(_stocks.begin(), _stocks.end(),
                                           [&](const TQuote& q) { return (q.price == order.price) &&
                                (q.type != OrderTraits::quote_type()); });
                    if (it != _stocks.end())
                    {
                        entry.orders_before = it->volume;
                    }
                    m_orders[isin_id].push_back(entry);
                    break;
                }
                else if (order.type == OrderType::Counter)
                {
                    order.status = OrderStatus::Cancel;
                    order.deal_price = 0;
                    order.amount_rest = 0;
                    executed_orders.push_back(order);
                    break;
                }
                else if (order.type == OrderType::FillOrKill)
                {
                    executed_orders.clear();
                    TOrder order = _order;
                    order.status = OrderStatus::Cancel;
                    order.deal_price = 0;
                    order.amount_rest = 0;
                    executed_orders.push_back(order);
                    break;
                }
                else
                {
                    throw UndefinedOrderTypeException();
                }
            }
            else
            {
                int volume = best_quote->volume;
                if (volume > order.volume)
                {
                    order.status = OrderStatus::Fill;
                    order.deal_price = best_quote->price;
                    order.amount_rest = 0;
                    executed_orders.push_back(order);
                    break;
                }
                else if (volume > 0)
                {
                    order.volume -= volume;
                    TOrder new_order = order;
                    new_order.volume = volume;
                    new_order.status = OrderStatus::Fill;
                    new_order.deal_price = best_quote->price;
                    new_order.amount_rest = order.volume;
                    executed_orders.push_back(new_order);
                }
            }
        }
        if (m_order_handler)
        {
            for (const auto& order : executed_orders)
            {
                m_order_handler(to_order(order, step));
            }
        }
    }

};

}
}

#endif // ATRADE_MARKETSIMULATOR_H
