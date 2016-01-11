#pragma once

#include <atrade/atrade.hpp>
#include <iostream>
#include <functions/functions.hpp>

class CancelIndicator : public atrade::async2::Indicator<atrade::DealType>
{
public:
    CancelIndicator(boost::asio::io_service& ios, int64_t isin_id, int volume, const atrade::pt::time_duration& duration, double mul) :
        Indicator(atrade::DealType::Undefined),
        m_ios(&ios),
        m_isin_id(isin_id),
        m_buy_volume(0),
        m_sell_volume(0),
        m_volume(volume),
        m_duration(duration),
        m_last_deal_id(0),
        m_mul(mul)
    {
        using namespace atrade;

        double step = async2::get_market_data(ios).security(isin_id).step();

        m_ordlog_subscription = async2::subscribe_ordlog(ios, m_isin_id, [&, step](int64_t, const atrade::OrdLogEntry& entry) {
            if (entry.deal_id <= m_last_deal_id)
            {
                update_value();
                return;
            }
            m_last_deal_id = entry.deal_id;
            bool is_buy = ((entry.flags & atrade::OrdLogFlags::Buy) != 0);
            bool is_sell = ((entry.flags & atrade::OrdLogFlags::Sell) != 0);
            double center_price = functions::center_price(ios, m_isin_id);
            int n = 40;
            if (is_buy && (entry.price > center_price + n*step))
            {
                m_entries.push(entry);
                m_buy_volume += entry.amount;
            }
            else if (is_sell && (entry.price < center_price - n*step))
            {
                m_entries.push(entry);
                m_sell_volume += entry.amount;
            }
            update_value();
        });
    }

    CancelIndicator(asio::io_service& ios, int64_t isin_id, int volume, const pt::time_duration& duration) :
        CancelIndicator(ios, isin_id, volume, duration, 2)
    {
    }

    CancelIndicator(asio::io_service& ios, int64_t isin_id, const pt::time_duration& duration, double mul) :
        CancelIndicator(ios, isin_id, 0, duration, mul)
    {
    }

    double rel() const
    {
        double max = double(std::max(m_buy_volume, m_sell_volume));
        double min = double(std::min(m_buy_volume, m_sell_volume));
        return min/(min+max);
    }

    int buys() const
    {
        return m_buy_volume;
    }

    int sells() const
    {
        return m_sell_volume;
    }

private:
    boost::asio::io_service* m_ios;
    int64_t m_isin_id;
    atrade::async2::Subscription<atrade::OrdLogHandler> m_ordlog_subscription;
    std::queue<atrade::OrdLogEntry> m_entries;
    int m_buy_volume;
    int m_sell_volume;
    int m_volume;
    atrade::pt::time_duration m_duration;
    int64_t m_last_deal_id;
    double m_mul;

    void update_value()
    {
        using namespace atrade;

        pt::ptime now = async2::current_datetime(*m_ios);
        while (!m_entries.empty() && (now - m_entries.front().datetime > m_duration))
        {
            bool is_buy = ((m_entries.front().flags & atrade::OrdLogFlags::Buy) != 0);
            if (is_buy)
            {
                m_buy_volume -= m_entries.front().amount;
            }
            else
            {
                m_sell_volume -= m_entries.front().amount;
            }
            m_entries.pop();
        }
        if (m_buy_volume >= m_mul*m_sell_volume && m_buy_volume >= m_volume)
        {
            value(atrade::DealType::Buy);
        }
        else if (m_sell_volume >= m_mul*m_buy_volume && m_sell_volume >= m_volume)
        {
            value(atrade::DealType::Sell);
        }
        else
        {
            value(atrade::DealType::Undefined);
        }
    }
};


