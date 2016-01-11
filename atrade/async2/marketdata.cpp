#include "marketdata.hpp"
#include "asyncmarket.hpp"
#include "exceptions.hpp"

namespace atrade {
namespace async2 {

MarketData::MarketData(IAsyncMarket* market) :
    m_market(market)
{
    if (m_market)
    {
        m_tick_subscription = m_market->subscribe_tick([this](const Deal& tick) {
            m_ticks[tick.isin_id].push_back(tick);
        });
//        m_stock_subscription = m_market->subscribe_stock([this](int64_t isin_id, const std::vector<Quote>& stocks) {
//        });
        m_security_subscription = m_market->subscribe_security([this](const std::vector<Security>& securities) {
            m_securities.clear();
            for (auto& security : securities)
            {
                m_securities[security.isin_id()] = security;
            }
        });
    }
}

std::vector<Deal> MarketData::ticks(int64_t isin_id) const
{
    return m_ticks.at(isin_id);
}

std::vector<Quote> MarketData::stocks(int64_t isin_id) const
{
    return m_market->stocks(isin_id);
}

Security MarketData::security(int64_t isin_id) const
{
    return m_securities.at(isin_id);
}

std::vector<Security> MarketData::securities() const
{
    std::vector<Security> securities;
    for (auto& e : m_securities)
    {
        securities.push_back(e.second);
    }
    return securities;
}

int64_t MarketData::get_isin_id(const std::string& code) const
{
    for (auto& e : m_securities)
    {
        const Security& s = e.second;
        if (s.code() == code || s.full_code() == code)
        {
            return s.isin_id();
        }
    }
    throw NotFoundException();
}

}
}

