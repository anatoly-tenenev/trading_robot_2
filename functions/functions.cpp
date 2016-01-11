#include "functions.hpp"
#include <boost/iterator/filter_iterator.hpp>

using namespace atrade;

namespace functions
{

double best_price(QuoteType type, const std::vector<Quote>& quotes)
{
    std::function<bool(const Quote&)> pred = [type](const Quote& q) { return q.type == type; };
    std::function<bool(const Quote&, const Quote&)> comp;
    if (type == QuoteType::Bid)
    {
        comp = [](const Quote& q1, const Quote& q2) { return q1.price < q2.price; };
    }
    else
    {
        comp = [](const Quote& q1, const Quote& q2) { return q1.price > q2.price; };
    }
    auto start = boost::make_filter_iterator(pred, quotes.begin(), quotes.end());
    auto end = boost::make_filter_iterator(pred, quotes.end(), quotes.end());
    auto it = std::max_element(start, end, comp);
    if (it != end)
    {
        return it->price;
    }
    return 0;
}

double best_price(OrderDir type, const std::vector<Quote>& quotes)
{
    return best_price(type == OrderDir::Bid ? QuoteType::Bid : QuoteType::Ask, quotes);
}

double delta_price(QuoteType type, double step, size_t n)
{
    return (type == QuoteType::Bid ? 1 : -1) * step * n;
}

double delta_price(OrderDir type, double step, size_t n)
{
    return (type == OrderDir::Bid ? 1 : -1) * step * n;
}

double center_price(const std::vector<Quote>& quotes, double step)
{
    double best_bid = best_price(OrderDir::Bid, quotes);
    double best_ask = best_price(OrderDir::Ask, quotes);
    if (!best_bid)
    {
        return best_ask;
    }
    else if (!best_ask)
    {
        return best_bid;
    }
    return price_round(best_bid + std::round((best_ask-best_bid)/2/step)*step);
}

double best_price(asio::io_service& ios, int64_t isin_id, QuoteType type)
{
    std::vector<Quote> quotes = async2::get_market_data(ios).stocks(isin_id);
    return best_price(type, quotes);
}

double best_price(asio::io_service& ios, int64_t isin_id, OrderDir type)
{
    std::vector<Quote> quotes = async2::get_market_data(ios).stocks(isin_id);
    return best_price(type, quotes);
}

double center_price(asio::io_service& ios, int64_t isin_id)
{
    async2::MarketData& market_data = async2::get_market_data(ios);
    std::vector<Quote> quotes = market_data.stocks(isin_id);
    double step = market_data.security(isin_id).step();
    return center_price(quotes, step);
}

void print_stocks(atrade::asio::io_service& ios, int64_t isin_id)
{
    std::vector<Quote> quotes = async2::get_market_data(ios).stocks(isin_id);
    std::sort(quotes.begin(), quotes.end(),
              [](auto& q1, auto& q2) { return q1.price > q2.price; });
    std::vector<Quote> asks;
    std::copy_if(quotes.begin(), quotes.end(),
                 std::back_inserter(asks),
                 [](auto& q) { return q.type == QuoteType::Ask; });
    std::vector<Quote> bids;
    std::copy_if(quotes.begin(), quotes.end(),
                 std::back_inserter(bids),
                 [](auto& q) { return q.type == QuoteType::Bid; });
    if (asks.size() <= 5)
    {
        for (auto& q : asks)
        {
            std::cout << "a:" << q.price << "(" << q.volume << ") ";
        }
    }
    else
    {
        for (size_t i = asks.size() - 5; i < asks.size(); ++i)
        {
            std::cout << "a:" << asks[i].price << "(" << asks[i].volume << ") ";
        }
    }
    for (size_t i = 0; i < 5 && i < bids.size(); ++i)
    {
        std::cout << "b:" << bids[i].price
                  << "(" << bids[i].volume << ") ";
    }
    std::cout << "\n";
}

double near_price(QuoteType type, double price1, double price2)
{
    if (type == QuoteType::Bid)
    {
        return std::max(price1, price2);
    }
    else
    {
        return std::min(price1, price2);
    }
}

double near_price(OrderDir type, double price1, double price2)
{
    return near_price(type == OrderDir::Bid ? QuoteType::Bid : QuoteType::Ask,
                      price1, price2);
}

bool is_before(atrade::QuoteType type, double price1, double price2)
{
    if (type == QuoteType::Bid)
    {
        return price1 > price2;
    }
    else
    {
        return price1 < price2;
    }
}

bool is_before(atrade::OrderDir type, double price1, double price2)
{
    return is_before(type == OrderDir::Bid ? QuoteType::Bid : QuoteType::Ask,
                     price1, price2);
}

class QuotesAreEmptyException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Quotes are empty.";
    }
};

double find_price(QuoteType type, int volume, const std::vector<Quote>& quotes, double step)
{
    std::vector<Quote> _quotes;
    std::copy_if(quotes.begin(), quotes.end(),
                 std::back_inserter(_quotes),
                 [&](const auto& q) { return q.type == type; });
    if (_quotes.empty())
    {
        throw QuotesAreEmptyException();
    }
    if (type == QuoteType::Bid)
    {
        std::sort(_quotes.begin(), _quotes.end(),
                  [](const auto& q1, const auto& q2) { return q1.price > q2.price; });
    }
    else
    {
        std::sort(_quotes.begin(), _quotes.end(),
                  [](const auto& q1, const auto& q2) { return q1.price < q2.price; });
    }
    int _volume = 0;
    for (auto it = _quotes.begin(); it != _quotes.end(); ++it)
    {
        const Quote& q = *it;
        _volume += q.volume;
        if (_volume > volume)
        {
            return atrade::price_round(q.price + delta_price(type, step, 1));
        }
    }
    return _quotes.back().price;
}

double find_price(OrderDir dir, int volume, const std::vector<Quote>& quotes, double step)
{
    return find_price(dir == OrderDir::Bid ? QuoteType::Bid : QuoteType::Ask,
                      volume, quotes, step);
}

double find_price(asio::io_service& ios, int64_t isin_id, QuoteType type, int volume)
{
    async2::MarketData& market_data = async2::get_market_data(ios);
    std::vector<Quote> quotes = market_data.stocks(isin_id);
    double step = market_data.security(isin_id).step();
    return find_price(type, volume, quotes, step);
}

double find_price(asio::io_service& ios, int64_t isin_id, OrderDir dir, int volume)
{
    async2::MarketData& market_data = async2::get_market_data(ios);
    std::vector<Quote> quotes = market_data.stocks(isin_id);
    double step = market_data.security(isin_id).step();
    return find_price(dir, volume, quotes, step);
}

}
