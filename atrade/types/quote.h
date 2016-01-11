#ifndef ATRADE_QUOTE_H
#define ATRADE_QUOTE_H

#include <qsh/types/types.h>
#include <sys/types.h>

namespace atrade {

using QuoteType = qsh::QuoteType;

struct Quote
{
    double price;
    int volume;
    QuoteType type;

    Quote(double _price, int _volume, QuoteType _type) :
        price(_price),
        volume(_volume),
        type(_type)
    {
    }

    Quote() :
        Quote(0, 0, QuoteType::Bid)
    {
    }
};

}

#endif // ATRADE_QUOTE_H
