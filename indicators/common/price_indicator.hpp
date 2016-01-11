#pragma once

#include <atrade/atrade.hpp>
#include <iostream>

class PriceIndicator : public atrade::async2::BooleanIndicator
{
public:
    PriceIndicator(boost::asio::io_service& ios, int64_t isin_id, atrade::DealType deal_type, double price) :
        BooleanIndicator(),
        m_ios(ios),
        m_isin_id(isin_id),
        m_deal_type(deal_type),
        m_price(atrade::price_round(price))
    {
        using namespace atrade;

        m_tick_subscription = async2::subscribe_tick(ios, [this](const Deal& tick) {
            if (m_isin_id != tick.isin_id)
            {
                return;
            }
            if (m_deal_type == DealType::Buy)
            {
                if (m_price >= atrade::price_round(tick.price))
                {
                    value(true);
                }
                else
                {
                    value(false);
                }
            }
            else if (m_deal_type == DealType::Sell)
            {
                if (m_price <= atrade::price_round(tick.price))
                {
                    value(true);
                }
                else
                {
                    value(false);
                }
            }
        });
    }

private:
    boost::asio::io_service& m_ios;
    int64_t m_isin_id;
    atrade::async2::Subscription<atrade::TickHandler> m_tick_subscription;
    atrade::DealType m_deal_type;
    double m_price;
};


