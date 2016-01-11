#include "qshdatasource.h"
#include "utils.hpp"
#include "../utils.hpp"

namespace atrade {
namespace backtester {

QshDataSource::QshDataSource(const std::string& path) :
    MarketDataSourceBase(),
    m_reader(path),
    m_datetime(pt::not_a_date_time)
{
    for (const auto& s : m_reader.streams())
    {
        auto ss = static_cast<qsh::reader::ISecurityStream*>(s);

        qsh::Security sec = ss->security();
        m_securities[sec.key()] = Security(sec.key(), "", sec.ticker(), sec.step());

        if (ss->type() == qsh::StreamType::OrdLog)
        {
            auto ols = dynamic_cast<qsh::reader::IOrdLogStream*>(s);
            ols->deal_handler([this](const qsh::Deal& _deal) mutable {
                BDeal deal = from_qsh_deal(_deal);
                deal.datetime = m_datetime;
                if (m_tick_handler)
                {
                    m_tick_handler(deal);
                }
            });
            ols->stock_handler([this](int skey, const std::vector<qsh::Quote>& stocks) mutable {
                if (m_stock_handler)
                {
                    m_stock_handler(skey, stocks);
                }
            });
            ols->handler([this](int skey, const qsh::OrdLogEntry& _entry) mutable {
                BOrdLogEntry entry = from_qsh_ordlog_entry(_entry);
                entry.datetime = m_datetime;
                if (m_ordlog_handler)
                {
                    m_ordlog_handler(skey, entry);
                }
            });
            m_stocks[sec.key()] = [ols]() {
                return ols->quotes();
            };
        }
    }
}

std::vector<Security> QshDataSource::securities() const
{
    std::vector<Security> securities;
    std::transform(m_securities.begin(), m_securities.end(),
                   std::back_inserter(securities), second(m_securities));
    return securities;
}

pt::ptime QshDataSource::datetime() const
{
    return m_reader.datetime();
}

void QshDataSource::process()
{
    if (m_reader.eof())
    {
        throw EofQshException();
    }
    m_datetime = m_reader.datetime();
    m_reader.read(true);
}

}
}
