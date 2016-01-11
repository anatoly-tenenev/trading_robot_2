#ifndef TRADING_ROBOT_FUNCTIONS_HPP
#define TRADING_ROBOT_FUNCTIONS_HPP

#include <atrade/atrade.hpp>
#include <numeric>

class OrderBook
{
public:
    OrderBook(const std::vector<atrade::Quote>& quotes, double step) :
        m_step(atrade::price_round(step))
    {
        for (const atrade::Quote& q : quotes)
        {
            switch (q.type)
            {
            case atrade::QuoteType::Bid:
                m_bids[atrade::price_round(q.price)] = q.volume;
                break;
            case atrade::QuoteType::Ask:
                m_asks[atrade::price_round(q.price)] = q.volume;
                break;
            }
        }
    }

    OrderBook(atrade::asio::io_service& ios, int64_t isin_id) :
        OrderBook(atrade::async2::get_market_data(ios).stocks(isin_id),
                  atrade::async2::get_market_data(ios).security(isin_id).step())
    {
    }

    int get(atrade::QuoteType type, double _price) const
    {
        return (type == atrade::QuoteType::Bid) ?
                    bid(_price) :
                    ask(_price);
    }

    int get(atrade::OrderDir dir, double _price) const
    {
        return (dir == atrade::OrderDir::Bid) ?
                    bid(_price) :
                    ask(_price);
    }

    int bid(double _price) const
    {
        double price = atrade::price_round(_price);
        if (m_bids.count(price))
        {
            return m_bids.at(price);
        }
        else
        {
            return 0;
        }
    }

    int ask(double _price) const
    {
        double price = atrade::price_round(_price);
        if (m_asks.count(price))
        {
            return m_asks.at(price);
        }
        else
        {
            return 0;
        }
    }

    int get(atrade::QuoteType type, double price1, double price2) const
    {
        return (type == atrade::QuoteType::Bid) ?
                    bids(price1, price2) :
                    asks(price1, price2);
    }

    int get(atrade::OrderDir dir, double price1, double price2) const
    {
        return (dir == atrade::OrderDir::Bid) ?
                    bids(price1, price2) :
                    asks(price1, price2);
    }

    int bids(double price1, double price2) const
    {
        double max = atrade::price_round(price1);
        double min = atrade::price_round(price2);
        int result = 0;
        if (min > max)
        {
            return 0;
        }
        auto bids = m_bids;
        for (double p = min; p <= max; p = atrade::price_round(p + m_step))
        {
            result += bids[p];
        }
        return result;
    }

    int asks(double price1, double price2) const
    {
        double max = atrade::price_round(price2);
        double min = atrade::price_round(price1);
        if (min > max)
        {
            return 0;
        }
        int result = 0;
        auto asks = m_asks;
        for (double p = min; p <= max; p = atrade::price_round(p + m_step))
        {
            result += asks[p];
        }
        return result;
    }

    int bids(int length) const
    {
        double bb = best_bid();
        return bids(bb, bb - length*m_step);
    }

    int asks(int length) const
    {
        double ba = best_ask();
        return asks(ba, ba + length*m_step);
    }

    double best_bid() const
    {
        if (m_bids.empty())
        {
            return 0;
        }
        else
        {
            return m_bids.begin()->first;
        }
    }

    double best_ask() const
    {
        if (m_asks.empty())
        {
            return 0;
        }
        else
        {
            return m_asks.begin()->first;
        }
    }

    double best_price(atrade::QuoteType type) const
    {
        return (type == atrade::QuoteType::Bid) ?
                    best_bid() :
                    best_ask();
    }

    double best_price(atrade::OrderDir dir) const
    {
        return (dir == atrade::OrderDir::Bid) ?
                    best_bid() :
                    best_ask();
    }

    double center() const
    {
        double bid = best_bid();
        double ask = best_ask();
        if (!bid)
        {
            return ask;
        }
        else if (!ask)
        {
            return bid;
        }
        return atrade::price_round(bid + std::round((ask-bid)/2/m_step)*m_step);
    }

    double spread() const
    {
        return atrade::price_round(best_ask() - best_bid());
    }

private:
    std::map<double, int, std::greater<double>> m_bids;
    std::map<double, int> m_asks;
    double m_step;
};

namespace functions
{

double best_price(atrade::QuoteType type, const std::vector<atrade::Quote>& quotes);
double best_price(atrade::OrderDir type, const std::vector<atrade::Quote>& quotes);
double best_price(atrade::asio::io_service& ios, int64_t isin_id, atrade::QuoteType type);
double best_price(atrade::asio::io_service& ios, int64_t isin_id, atrade::OrderDir type);

double delta_price(atrade::QuoteType type, double step, size_t n=1);
double delta_price(atrade::OrderDir type, double step, size_t n=1);

double center_price(const std::vector<atrade::Quote>& quotes, double step);
double center_price(atrade::asio::io_service& ios, int64_t isin_id);

void print_stocks(atrade::asio::io_service& ios, int64_t isin_id);

double near_price(atrade::QuoteType type, double price1, double price2);
double near_price(atrade::OrderDir type, double price1, double price2);

bool is_before(atrade::QuoteType type, double price1, double price2);
bool is_before(atrade::OrderDir type, double price1, double price2);

double find_price(atrade::QuoteType type, int volume, const std::vector<atrade::Quote>& quotes, double step);
double find_price(atrade::OrderDir dir, int volume, const std::vector<atrade::Quote>& quotes, double step);
double find_price(atrade::asio::io_service& ios, int64_t isin_id, atrade::QuoteType type, int volume);
double find_price(atrade::asio::io_service& ios, int64_t isin_id, atrade::OrderDir dir, int volume);

template <typename T>
double mean(T&& array, size_t size)
{
    if (!size)
    {
        return 0;
    }
    std::vector<double> v;
    v.reserve(size);
    for (auto it = std::begin(array); it != std::end(array); ++it)
    {
        v.push_back(double(*it) / size);
    }
    return std::accumulate(v.begin(), v.end(), double(0));
}

//template<typename CompletionToken>
//auto get_isin_id(atrade::async::TradingContextBase& ctx, const std::string& code, CompletionToken&& token)
//{
//    namespace async = atrade::async;
//    namespace asio = atrade::asio;
//    namespace system = atrade::system;
//    namespace pt = atrade::pt;

//    using handler_type = typename asio::handler_type
//                                <CompletionToken, void(system::error_code, int64_t)>::type;

//    handler_type _handler(std::forward<CompletionToken>(token));

//    asio::async_result<handler_type> result(_handler);

//    auto handler = [&ctx, _handler, code]() mutable {
//        int64_t isin_id = ctx.market().market_data().get_isin_id(code);
//        ctx.ios().post([isin_id, _handler]() mutable {
//            _handler(system::error_code(), isin_id);
//        });
//    };

//    try
//    {
//        handler();
//    }
//    catch (async::NotFoundException&)
//    {
//        auto req_sequrities = std::make_shared<async::ReqSecurities>(ctx);
//        req_sequrities->async_do(pt::seconds(10), [handler, req_sequrities]
//                                                  (system::error_code, const std::vector<atrade::Security>&) mutable {
//            handler();
//        });
//    }

//    return result.get();
//}

//template<typename CompletionToken>
//auto get_security(atrade::async::TradingContextBase& ctx, int64_t isin_id, CompletionToken&& token)
//{
//    namespace async = atrade::async;
//    namespace asio = atrade::asio;
//    namespace system = atrade::system;
//    namespace pt = atrade::pt;
//    using atrade::Security;

//    using handler_type = typename asio::handler_type
//                                <CompletionToken, void(system::error_code, Security)>::type;

//    handler_type _handler(std::forward<CompletionToken>(token));

//    asio::async_result<handler_type> result(_handler);

//    auto handler = [&ctx, _handler, isin_id]() mutable {
//        Security security = ctx.market().market_data().security(isin_id);
//        ctx.ios().post([security, _handler]() mutable {
//            _handler(system::error_code(), security);
//        });
//    };

//    try
//    {
//        handler();
//    }
//    catch (std::out_of_range&)
//    {
//        auto req_sequrities = std::make_shared<async::ReqSecurities>(ctx);
//        req_sequrities->async_do(pt::seconds(10), [handler, req_sequrities]
//                                                  (system::error_code, const std::vector<atrade::Security>&) mutable {
//            handler();
//        });
//    }

//    return result.get();
//}

}

#endif // TRADING_ROBOT_FUNCTIONS_HPP
