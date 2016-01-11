#include "order.h"

namespace atrade {

OrderDir operator! (OrderDir dir)
{
    switch (dir)
    {
    case OrderDir::Ask:
        return OrderDir::Bid;
    case OrderDir::Bid:
        return OrderDir::Ask;
    default:
        return dir;
    }
}

std::ostream& operator<<(std::ostream& out, const Order& order)
{
    out << "Order(" << "isin_id=" << order.isin_id << ",order_id=" << order.order_id
        << ",dir=" << ((order.dir == OrderDir::Bid) ? "Bid" :
                       (order.dir == OrderDir::Ask) ? "Ask" : "Undefined")
        << ",price=" << order.price << ",volume=" << order.volume
        << ",status=" << ((order.status == OrderStatus::Add) ? "Add" :
                          (order.status == OrderStatus::Fill) ? "Fill" :
                          (order.status == OrderStatus::Cancel) ? "Cancel" : "")
        << ",deal_price=" << order.deal_price
        << ",amount_rest=" << order.amount_rest << ")";
    return out;
}

}

