#ifndef QSH_ORDLOGENTRY_H
#define QSH_ORDLOGENTRY_H

#include "../headers.h"
#include "types.h"

namespace qsh {

class OrdLogEntry
{
public:
    OrdLogEntry(OrdLogFlags flags, const pt::ptime& datetime, long order_id,
                int price, int amount, int amount_rest, long deal_id,
                int deal_price, int oi) :
        m_flags(flags),
        m_datetime(datetime),
        m_order_id(order_id),
        m_price(price),
        m_amount(amount),
        m_amount_rest(amount_rest),
        m_deal_id(deal_id),
        m_deal_price(deal_price),
        m_oi(oi)
    {
    }

    OrdLogFlags flags() const { return m_flags; }
    pt::ptime datetime() const { return m_datetime; }
    long order_id() const { return m_order_id; }
    int price() const { return m_price; }
    int amount() const { return m_amount; }
    int amount_rest() const { return m_amount_rest; }
    long deal_id() const { return m_deal_id; }
    int deal_price() const { return m_deal_price; }
    int oi() const { return m_oi; }

private:
    OrdLogFlags m_flags;
    pt::ptime m_datetime;
    long m_order_id;
    int m_price;
    int m_amount;
    int m_amount_rest;
    long m_deal_id;
    int m_deal_price;
    int m_oi;

};

}

#endif // QSH_ORDLOGENTRY_H
