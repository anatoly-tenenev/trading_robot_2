#include "utils.hpp"
#include <cmath>

namespace atrade {
namespace backtester {

Order to_order(const BOrder& order, double step)
{
    return Order(order.isin_id, order.order_id, order.dir, order.price * step,
                 order.volume, order.type, order.status, order.deal_id,
                 order.deal_price * step, order.amount_rest);
}

BOrder from_order(const Order& order, double step)
{
    return BOrder(order.isin_id, order.order_id, order.dir, std::lround(order.price/step),
                  order.volume, order.type, order.status, order.deal_id,
                  std::lround(order.deal_price/step), order.amount_rest);
}

Deal to_deal(const BDeal& deal, double step)
{
    return Deal(deal.isin_id, deal.deal_id, deal.price * step,
                deal.volume, deal.type, deal.datetime);
}

BDeal from_deal(const Deal &deal, double step)
{
    return BDeal(deal.isin_id, deal.deal_id, std::lround(deal.price/step),
                 deal.volume, deal.type, deal.datetime);
}

BDeal from_qsh_deal(const qsh::Deal& deal)
{
    return BDeal(deal.sec_key, deal.id, deal.price, deal.volume,
                 (deal.type == qsh::DealType::Buy) ? DealType::Buy : DealType::Sell,
                 deal.datetime);
}

std::vector<Quote> to_stocks(const std::vector<BQuote>& stocks, double step)
{
    std::vector<Quote> _stocks(stocks.size());
    for (size_t i = 0; i < stocks.size(); ++i)
    {
        _stocks[i] = Quote(stocks[i].price * step, stocks[i].volume, stocks[i].type);
    }
    return _stocks;
}

OrdLogEntry to_ordlog_entry(const BOrdLogEntry& entry, double step)
{
    return OrdLogEntry(entry.flags, entry.datetime, entry.order_id,
                       entry.price * step, entry.amount, entry.amount_rest,
                       entry.deal_id, entry.deal_price * step);
}

BOrdLogEntry from_ordlog_entry(const OrdLogEntry& entry, double step)
{
    return BOrdLogEntry(entry.flags, entry.datetime, entry.order_id,
                        std::lround(entry.price/step), entry.amount, entry.amount_rest,
                        entry.deal_id, std::lround(entry.deal_price/step));
}

BOrdLogEntry from_qsh_ordlog_entry(const qsh::OrdLogEntry& entry)
{
    return BOrdLogEntry(entry.flags(), entry.datetime(), entry.order_id(),
                        entry.price(), entry.amount(), entry.amount_rest(),
                        entry.deal_id(), entry.deal_price());
}

}
}
