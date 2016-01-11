#ifndef SIMPLEMARKETSIMULATOR_H
#define SIMPLEMARKETSIMULATOR_H

#include "../headers.h"
#include "marketsimulator.h"
#include "types.h"
#include "utils.hpp"
#include "exceptions.h"

namespace atrade {
namespace backtester {

class SimpleMarketSimulator : public MarketSimulator
{
public:
    SimpleMarketSimulator();
    IMarketDataSource& add_data_source(std::unique_ptr<IMarketDataSource>&& data_source) override;

private:
    SimpleMarketSimulator(const SimpleMarketSimulator&) = delete;
    SimpleMarketSimulator& operator= (const SimpleMarketSimulator&) = delete;

protected:
    void handle_tick(const BDeal& tick);
    void handle_stocks(int64_t isin_id, const std::vector<BQuote>& stocks);

    class BuyDeal
    {
    public:
        static OrderDir order_dir() { return OrderDir::Ask; }

        template <typename T>
        static bool before(T price1, T price2)
        {
            return price1 < price2;
        }
    };

    class SellDeal
    {
    public:
        static OrderDir order_dir() { return OrderDir::Bid; }

        template <typename T>
        static bool before(T price1, T price2)
        {
            return price1 > price2;
        }
    };

    template <class DealTraits,
              typename TDeal>
    void _handle_tick(const TDeal& _tick)
    {
        using TOrder = decltype(OrderEntry::order);
        double step = m_securities.at(_tick.isin_id).step();
        if (_tick.volume <= 0)
        {
            return;
        }
        int64_t isin_id = _tick.isin_id;
        std::vector<OrderEntry>& orders = m_orders[isin_id];
        if (orders.empty())
        {
            return;
        }
        TDeal tick = _tick;
        std::sort(orders.begin(), orders.end(),
                  [](const OrderEntry& e1, const OrderEntry& e2) {
            if (DealTraits::before(e1.order.price, e2.order.price))
            {
                return true;
            }
            else if (DealTraits::before(e2.order.price, e1.order.price))
            {
                return false;
            }
            else
            {
                return e1.orders_before < e2.orders_before;
            }
        });
        auto it = std::find_if(orders.begin(), orders.end(),
                               [](const OrderEntry& e) { return e.order.dir == DealTraits::order_dir(); });
        for (; it != orders.end(); ++it)
        {
            TOrder& order = it->order;
            if (DealTraits::before(tick.price, order.price))
            {
                break;
            }
            else if (DealTraits::before(order.price, tick.price))
            {
                it->orders_before = 0;
                if (tick.volume <= 0)
                {
                    continue;
                }
                if (tick.volume > order.volume)
                {
                    order.amount_rest = 0;
                    order.deal_id = tick.deal_id;
                    order.deal_price = order.price;
                    order.status = OrderStatus::Fill;
                    if (m_order_handler)
                    {
                        m_order_handler(to_order(order, step));
                    }
                    tick.volume -= order.volume;
                    order.volume = 0;
                }
                else
                {
                    order.volume -= tick.volume;
                    TOrder new_order = order;
                    new_order.volume = tick.volume;
                    new_order.amount_rest = order.volume;
                    new_order.status = OrderStatus::Fill;
                    new_order.deal_id = tick.deal_id;
                    new_order.deal_price = order.price;
                    if (m_order_handler)
                    {
                        m_order_handler(to_order(new_order, step));
                    }
                    tick.volume = 0;
                }
                continue;
            }
            else
            {
                if (tick.volume <= 0)
                {
                    break;
                }
                if (tick.volume <= it->orders_before)
                {
                    it->orders_before -= tick.volume;
                    continue;
                }
                else
                {
                    int volume = tick.volume - it->orders_before;
                    it->orders_before = 0;
                    if (volume > order.volume)
                    {
                        order.amount_rest = 0;
                        order.deal_id = tick.deal_id;
                        order.deal_price = order.price;
                        order.status = OrderStatus::Fill;
                        if (m_order_handler)
                        {
                            m_order_handler(to_order(order, step));
                        }
                        tick.volume -= order.volume;
                        order.volume = 0;
                    }
                    else
                    {
                        order.volume -= volume;
                        TOrder new_order = order;
                        new_order.volume = volume;
                        new_order.amount_rest = order.volume;
                        new_order.status = OrderStatus::Fill;
                        new_order.deal_id = tick.deal_id;
                        new_order.deal_price = order.price;
                        if (m_order_handler)
                        {
                            m_order_handler(to_order(new_order, step));
                        }
                        tick.volume -= volume;
                    }
                    continue;
                }
            }
        }
        orders.erase(std::remove_if(orders.begin(), orders.end(),
                                    [](const OrderEntry& e) { return e.order.volume <= 0; }),
                     orders.end());
    }

    class AskQuote
    {
    public:
        static QuoteType quote_type() { return QuoteType::Ask; }

        static OrderDir order_dir() { return OrderDir::Bid; }

        template <typename T>
        static bool before(T price1, T price2)
        {
            return price1 < price2;
        }
    };

    class BidQuote
    {
    public:
        static QuoteType quote_type() { return QuoteType::Bid; }

        static OrderDir order_dir() { return OrderDir::Ask; }

        template <typename T>
        static bool before(T price1, T price2)
        {
            return price1 > price2;
        }
    };

    template <class QuoteTraits,
              typename TQuote>
    void _handle_stocks(int64_t isin_id, const std::vector<TQuote>& _stocks)
    {
        double step = m_securities.at(isin_id).step();
        if (m_orders[isin_id].empty())
        {
            return;
        }
        std::vector<TQuote> stocks;
        std::copy_if(_stocks.begin(), _stocks.end(),
                     std::back_inserter(stocks),
                     [](const TQuote& q) { return (q.type == QuoteTraits::quote_type()) &&
                                                  (q.volume > 0); });
        if (stocks.empty())
        {
            return;
        }
        auto best_quote = std::min_element(stocks.begin(), stocks.end(),
                                           [](const TQuote& q1, const TQuote& q2) {
                                                    return QuoteTraits::before(q1.price, q2.price); });
        std::vector<OrderEntry>& orders = m_orders[isin_id];
        for (OrderEntry& e : orders)
        {
            if (e.order.dir == QuoteTraits::order_dir())
            {
                if (!QuoteTraits::before(e.order.price, best_quote->price))
                {
                    e.order.amount_rest = 0;
                    e.order.deal_price = e.order.price;
                    e.order.status = OrderStatus::Fill;
                    if (m_order_handler)
                    {
                        m_order_handler(to_order(e.order, step));
                    }
                    e.order.volume = 0;
                }
            }
            else if (e.order.dir == !QuoteTraits::order_dir())
            {
                int orders_before = 0;
                auto it = std::find_if(stocks.begin(), stocks.end(),
                                       [&](const TQuote& q) { return q.price == e.order.price; });
                if (it != stocks.end())
                {
                    orders_before = it->volume;
                }
                e.orders_before = std::min(e.orders_before, orders_before);
            }
            else
            {
                throw UndefinedOrderDirException();
            }
        }
        orders.erase(std::remove_if(orders.begin(), orders.end(),
                                    [](const OrderEntry& e) { return e.order.volume <= 0; }),
                     orders.end());
    }
};

}
}

#endif // SIMPLEMARKETSIMULATOR_H
