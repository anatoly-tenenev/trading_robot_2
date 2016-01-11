#include "ordlogstream.hpp"

namespace qsh {
namespace reader {
namespace v4 {

OrdLogStream::OrdLogStream(DataReader& data_reader) :
    QshStream(data_reader, StreamType::OrdLog),
    m_last_millisecs(0),
    m_last_order_id(0),
    m_last_price(0),
    m_last_amount(0),
    m_last_amount_rest(0),
    m_last_deal_id(0),
    m_last_deal_price(0),
    m_last_oi(0),
    m_last_stock_push(pt::min_date_time),
    m_last_pushed_deal_id(0),
    m_stock_push_interval(pt::milliseconds(5))
{
}

void OrdLogStream::read(bool push)
{
    pt::ptime datetime;
    int64_t order_id = 0;
    int amount_rest = 0;
    int64_t deal_id = 0;
    int deal_price = 0;
    int oi = 0;

    OrdLogEntryFlags flags = static_cast<OrdLogEntryFlags>(m_data_reader->read<u_int8_t>());
    OrdLogFlags ordLogFlags = static_cast<OrdLogFlags>(m_data_reader->read<u_int16_t>());

    bool is_add = ((ordLogFlags & OrdLogFlags::Add) != 0);
    bool is_fill = ((ordLogFlags & OrdLogFlags::Fill) != 0);

    bool is_buy = ((ordLogFlags & OrdLogFlags::Buy) != 0);
    bool is_sell = ((ordLogFlags & OrdLogFlags::Sell) != 0);

    if((flags & OrdLogEntryFlags::DateTime) != 0)
        m_last_millisecs = m_data_reader->read_growing(m_last_millisecs);

    datetime = utils::datetime_from_millisecs(m_last_millisecs);

    if((flags & OrdLogEntryFlags::OrderId) == 0)
        order_id = m_last_order_id;
    else if(is_add)
        order_id = m_last_order_id = m_data_reader->read_growing(m_last_order_id);
    else
        order_id = m_last_order_id + m_data_reader->read_leb128();

    if((flags & OrdLogEntryFlags::Price) != 0)
        m_last_price += m_data_reader->read_leb128();

    if((flags & OrdLogEntryFlags::Amount) != 0)
        m_last_amount = m_data_reader->read_leb128();

    if((flags & OrdLogEntryFlags::AmountRest) != 0)
        amount_rest = m_last_amount_rest = m_data_reader->read_leb128();
    else if(is_add)
        amount_rest = m_last_amount;
    else
        amount_rest = 0;

    if((flags & OrdLogEntryFlags::DealId) != 0)
        deal_id = m_last_deal_id = m_data_reader->read_growing(m_last_deal_id);
    else if(is_fill)
        deal_id = m_last_deal_id;
    else
        deal_id = 0;

    if((flags & OrdLogEntryFlags::DealPrice) != 0)
    {
        m_last_deal_price += m_data_reader->read_leb128();
        deal_price = m_last_deal_price;
    }
    else if(is_fill)
        deal_price = m_last_deal_price;
    else
        deal_price = 0;

    if((flags & OrdLogEntryFlags::OI) != 0)
    {
        m_last_oi += m_data_reader->read_leb128();
        oi = m_last_oi;
    }
    else if(is_fill)
        oi = m_last_oi;
    else
        oi = 0;

    if (m_handler && push)
    {
        m_handler(m_security.key(),
                  OrdLogEntry(ordLogFlags, datetime, order_id, m_last_price,
                              m_last_amount, amount_rest, deal_id, deal_price, oi));
    }

    if((ordLogFlags & OrdLogFlags::SessIdChanged) != 0)
    {
        m_raw_quotes.clear();
    }

    if(!(is_buy ^ is_sell)
       || ((ordLogFlags & OrdLogFlags::NonSystem) != 0)
       || ((ordLogFlags & OrdLogFlags::NonZeroReplAct) != 0))
    {
        return;
    }

    int quantity = m_raw_quotes[m_last_price];

    if(is_add ? is_sell : is_buy)
        quantity += m_last_amount;
    else
        quantity -= m_last_amount;

    if(quantity == 0)
        m_raw_quotes.remove(m_last_price);
    else
        m_raw_quotes[m_last_price] = quantity;

    if ((ordLogFlags & OrdLogFlags::EndOfTransaction) != 0)
    {
        m_quotes = m_raw_quotes.quotes(50);
    }

    if (push)
    {
        if((ordLogFlags & OrdLogFlags::EndOfTransaction) != 0)
        {
            pt::ptime now = datetime;
            if(now - m_last_stock_push > m_stock_push_interval)
            {
                m_last_stock_push = now;
                if(m_stock_handler && !m_quotes.empty())
                {
                    m_stock_handler(m_security.key(), m_quotes);
                }
            }
        }

        if(m_last_pushed_deal_id < deal_id)
        {
            if(m_deal_handler)
            {
                m_deal_handler(Deal(m_security.key(),
                                    deal_id,
                                    deal_price,
                                    m_last_amount,
                                    is_sell ? DealType::Sell : DealType::Buy,
                                    oi,
                                    datetime));
            }
            m_last_pushed_deal_id = deal_id;
        }
    }
}

void OrdLogStream::handler(const OrdLogHandler& handler)
{
    m_handler = handler;
}

void OrdLogStream::deal_handler(const DealHandler& handler)
{
    m_deal_handler = handler;
}

void OrdLogStream::stock_handler(const StockHandler& handler)
{
    m_stock_handler = handler;
}

std::vector<Quote> OrdLogStream::quotes() const
{
    return m_quotes;
}

}
}
}
