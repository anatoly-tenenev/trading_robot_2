#include "rawquotes.h"
#include "../security.hpp"

namespace qsh {

RawQuotes::RawQuotes()
{
}

void RawQuotes::clear()
{
    m_quotes.clear();
}

int& RawQuotes::operator[](int key)
{
    return m_quotes[key];
}

void RawQuotes::remove(int key)
{
    m_quotes.erase(key);
}

std::vector<Quote> RawQuotes::quotes(size_t limit)
{
    std::vector<Quote> quotes;
    quotes.reserve(m_quotes.size());
    bool valid = false;
    for (auto it = m_quotes.begin(); it != m_quotes.end(); ++it)
    {
        const auto& price = it->first;
        const auto& volume = it->second;
        if (volume > 0)
        {
            quotes.emplace_back(price, volume, QuoteType::Ask);
        }
        else if (volume < 0)
        {
            if (!quotes.empty())
            {
                valid = true;
            }
            else
            {
                valid = false;
                break;
            }
            if ((limit > 0) && (quotes.size() > limit))
            {
                quotes.erase(quotes.begin(),
                             quotes.begin() + (quotes.size() - limit));
            }
            for (size_t i = 0; it != m_quotes.end(); ++it)
            {
                const auto& price = it->first;
                const auto& volume = it->second;
                if (volume < 0)
                {
                    ++i;
                    quotes.emplace_back(price, -volume, QuoteType::Bid);
                    if ((limit > 0) && (i >= limit))
                    {
                        break;
                    }
                }
            }
            break;
        }
    }
    if (!valid)
    {
        quotes.clear();
    }
    return quotes;
}

}
