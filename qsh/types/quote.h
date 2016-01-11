#ifndef QSH_QUOTE_H
#define QSH_QUOTE_H

namespace qsh {

enum class QuoteType
{
    Ask,
    Bid
};

struct Quote
{
    int price;
    int volume;
    QuoteType type;

    Quote()
    {}
    Quote(int _price, int _volume, QuoteType _type) :
        price(_price),
        volume(_volume),
        type(_type)
    {}
};

}

#endif // QSH_QUOTE_H
