#ifndef ATRADE_ORDLOGENTRY_H
#define ATRADE_ORDLOGENTRY_H

#include "../headers.h"
#include <qsh/types/types.h>

namespace atrade {

using OrdLogFlags = qsh::OrdLogFlags;

template <typename T>
struct _OrdLogEntry
{
    OrdLogFlags flags;
    pt::ptime datetime;
    int64_t order_id;
    T price;
    int amount;
    int amount_rest;
    int64_t deal_id;
    T deal_price;

    _OrdLogEntry(OrdLogFlags flags, const pt::ptime& datetime, long order_id,
                 T price, int amount, int amount_rest, int64_t deal_id, T deal_price) :
        flags(flags),
        datetime(datetime),
        order_id(order_id),
        price(price),
        amount(amount),
        amount_rest(amount_rest),
        deal_id(deal_id),
        deal_price(deal_price)
    {
    }

    _OrdLogEntry() :
        _OrdLogEntry(OrdLogFlags::None, pt::min_date_time, 0, 0, 0, 0, 0, 0)
    {
    }
};

using OrdLogEntry = _OrdLogEntry<double>;

}

#endif // ATRADE_ORDLOGENTRY_H
