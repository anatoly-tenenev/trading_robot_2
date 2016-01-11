#pragma once

#include <atrade/atrade.hpp>
#include <iostream>

class StepIndicator : public atrade::async2::Indicator<atrade::DealType>
{
public:
    StepIndicator(boost::asio::io_service& ios, int64_t isin_id, int steps) :
        Indicator(atrade::DealType::Undefined),
        m_ios(&ios),
        m_isin_id(isin_id),
        m_steps(steps)
    {
        using namespace atrade;

        double step = async2::get_market_data(ios).security(isin_id).step();
        double center_price = functions::center_price(ios, m_isin_id);

        m_stock_subscription = async2::subscribe_stock(ios, [&, step, center_price]
                                                            (int64_t isin_id, const std::vector<Quote>&) {
            if (m_isin_id != isin_id)
            {
                return;
            }
            double cur_center_price = functions::center_price(ios, m_isin_id);
            if (std::round(std::abs((cur_center_price-center_price)/step)) >= m_steps)
            {
                if (cur_center_price - center_price > 0)
                    value(DealType::Buy);
                else
                    value(DealType::Sell);
            }
            else
            {
                value(DealType::Undefined);
            }
        });
    }

private:
    boost::asio::io_service* m_ios;
    int64_t m_isin_id;
    atrade::async2::Subscription<atrade::StockHandler> m_stock_subscription;
    int m_steps;
};


