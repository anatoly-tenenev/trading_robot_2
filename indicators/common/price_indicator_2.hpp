#pragma once

#include <atrade/atrade.hpp>
#include <iostream>

class PriceIndicator2 : public atrade::async2::BooleanIndicator
{
public:
    PriceIndicator2(boost::asio::io_service& ios, int64_t isin_id, int dist, const atrade::pt::time_duration& duration) :
        BooleanIndicator(),
        m_ios(ios),
        m_isin_id(isin_id),
        m_dist(dist),
        m_duration(duration),
        m_datetime(atrade::pt::not_a_date_time)
    {
        using namespace atrade;

        double step = async2::get_market_data(ios).security(isin_id).step();
        auto less = [](const Deal& t1, const Deal& t2) {
            return t1.price < t2.price;
        };

        m_tick_subscription = async2::subscribe_tick(ios, [&, step, less](const Deal& tick) {
            if (m_isin_id != tick.isin_id)
            {
                return;
            }
            auto now = async2::current_datetime(m_ios);
            while (!m_ticks.empty() && (now - m_ticks.front().datetime) > m_duration)
            {
                m_ticks.pop_front();
            }
            m_ticks.push_back(tick);

            auto max = std::max_element(m_ticks.begin(), m_ticks.end(), less);
            auto min = std::min_element(m_ticks.begin(), m_ticks.end(), less);

            if (max->price - min->price <= m_dist*step)
            {
                if (m_datetime.is_not_a_date_time())
                {
                    m_datetime = now;
                }
                else if (now - m_datetime >= m_duration)
                {
                    value(true);
                }
            }
            else
            {
                value(false);
                m_datetime = pt::not_a_date_time;
            }
        });
    }

private:
    boost::asio::io_service& m_ios;
    int64_t m_isin_id;
    atrade::async2::Subscription<atrade::TickHandler> m_tick_subscription;
    std::deque<atrade::Deal> m_ticks;
    int m_dist;
    atrade::pt::time_duration m_duration;
    atrade::pt::ptime m_datetime;
};


