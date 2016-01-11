#pragma once

#include <atrade/atrade.hpp>
#include <iostream>
#include <functions/functions.hpp>

class VolumeIndicator : public atrade::async2::BooleanIndicator
{
public:
    VolumeIndicator(boost::asio::io_service& ios, int64_t isin_id, atrade::QuoteType type, double price, int volume) :
        BooleanIndicator(),
        m_ios(&ios),
        m_isin_id(isin_id),
        m_type(type),
        m_price(atrade::price_round(price)),
        m_volume(volume)
    {
        using namespace atrade;

        update_value();

        m_ordlog_subscription = async2::subscribe_ordlog(ios, m_isin_id, [&](int64_t, const OrdLogEntry&) {
            update_value();
        });
    }

    VolumeIndicator(boost::asio::io_service& ios, int64_t isin_id, atrade::OrderDir dir, double price, int volume) :
        VolumeIndicator(ios,
                        isin_id,
                        (dir == atrade::OrderDir::Bid ?
                             atrade::QuoteType::Bid :
                             atrade::QuoteType::Ask),
                        price,
                        volume)
    {
    }

private:
    boost::asio::io_service* m_ios;
    int64_t m_isin_id;
    atrade::async2::Subscription<atrade::OrdLogHandler> m_ordlog_subscription;
    atrade::QuoteType m_type;
    double m_price;
    int m_volume;

    void update_value()
    {
        using namespace atrade;
        using namespace functions;

        std::vector<Quote> all_quotes = async2::get_market_data(*m_ios).stocks(m_isin_id);
        std::vector<Quote> quotes;
        std::copy_if(all_quotes.begin(), all_quotes.end(),
                     std::back_inserter(quotes),
                     [&](const auto& q) { return q.type == m_type; });
        if (m_type == QuoteType::Bid)
        {
            std::sort(quotes.begin(), quotes.end(),
                      [](const auto& q1, const auto& q2) { return q1.price > q2.price; });
        }
        else
        {
            std::sort(quotes.begin(), quotes.end(),
                      [](const auto& q1, const auto& q2) { return q1.price < q2.price; });
        }
        int volume = 0;
        for (auto it = quotes.begin(); it != quotes.end(); ++it)
        {
            const Quote& quote = *it;
            if (is_before(m_type, m_price, quote.price))
            {
                break;
            }
            volume += quote.volume;
        }
        if (volume >= m_volume)
        {
            value(true);
        }
        else
        {
            value(false);
        }
    }
};


