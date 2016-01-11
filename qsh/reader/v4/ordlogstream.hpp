#ifndef ORDLOGSTREAM_HPP
#define ORDLOGSTREAM_HPP

#include "../../headers.h"
#include "../iqshstream.hpp"
#include "qshstream.hpp"

namespace qsh {
namespace reader {
namespace v4 {

class OrdLogStream : public QshStream, public IOrdLogStream
{
public:
    OrdLogStream(DataReader& data_reader);
    void read(bool push) override;
    void handler(const OrdLogHandler& handler) override;
    void deal_handler(const DealHandler& handler) override;
    void stock_handler(const StockHandler& handler) override;
    std::vector<Quote> quotes() const override;

private:
    OrdLogHandler m_handler;
    DealHandler m_deal_handler;
    StockHandler m_stock_handler;
    int64_t m_last_millisecs;
    int64_t m_last_order_id;
    int m_last_price;
    int m_last_amount;
    int m_last_amount_rest;
    int64_t m_last_deal_id;
    int m_last_deal_price;
    int m_last_oi;
    RawQuotes m_raw_quotes;
    pt::ptime m_last_stock_push;
    int64_t m_last_pushed_deal_id;
    pt::time_duration m_stock_push_interval;
    std::vector<Quote> m_quotes;
};

}
}
}

#endif // ORDLOGSTREAM_HPP
