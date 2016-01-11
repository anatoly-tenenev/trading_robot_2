#pragma once

#include <atrade/atrade.hpp>
#include <iostream>

class MoexIndicator2 : public atrade::async2::BooleanIndicator
{
public:
    MoexIndicator2(boost::asio::io_service& ios, atrade::async2::TradingContext& ctx) :
        BooleanIndicator(),
        m_ios(ios),
        m_ctx(ctx)
    {
        using namespace atrade;

        std::cout << "current datetime: " << async2::current_datetime(ios) << "\n";
        /*
        10.00 - 14.00 	Начало основной торговой сессии
        14.00 - 14.03 	Промежуточный клиринговый сеанс (дневной клиринг)
        14.03 - 18.45 	Окончание основной торговой сессии
        18.45 - 19.00 	Вечерний клиринговый сеанс
        19.00 - 23.50 	Вечерняя торговая сессия
        */
        std::vector<pt::time_duration> time = {
            pt::time_duration{7,01,00}, pt::time_duration{10,59,00},
            pt::time_duration{11,04,00}, pt::time_duration{15,44,00},
            pt::time_duration{16,00,01}, pt::time_duration{20,49,00}
        };
        auto current_time = async2::current_datetime(ios).time_of_day();
        if (current_time < time[0])
        {
            value(false);
        }
        else if (current_time < time[1])
        {
            value(true);
        }
        else if (current_time < time[2])
        {
            value(false);
        }
        else if (current_time < time[3])
        {
            value(true);
        }
        else if (current_time < time[4])
        {
            value(false);
        }
        else if (current_time < time[5])
        {
            value(true);
        }
        else
        {
            value(false);
        }
        auto it = std::find_if(time.begin(), time.end(),
                               [&current_time](const pt::time_duration& time) { return current_time < time; });
        size_t index = it - time.begin();
        asio::spawn(ios, [this, &ios, &ctx, index, time] (asio::yield_context yield) mutable {
            gr::date current_date = async2::current_datetime(ios).date();
            for (;; ++index)
            {
                if (index >= time.size())
                {
                    index = 0;
                    current_date += gr::days(1);
                }
                async2::Timer timer(ios, ctx, pt::ptime(current_date, time[index]));
                timer.async_wait(yield);
                if (index%2)
                {
                    std::cout << "stop: " << async2::current_datetime(ios) << "\n";
                    value(false);
                }
                else
                {
                    std::cout << "start: " << async2::current_datetime(ios) << "\n";
                    value(true);
                }
            }
        });
    }

private:
    boost::asio::io_service& m_ios;
    atrade::async2::TradingContext& m_ctx;
};


