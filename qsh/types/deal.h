#ifndef QSH_DEAL_H
#define QSH_DEAL_H

#include "../headers.h"

namespace qsh {

enum class DealType
{
    Buy,
    Sell
};

struct Deal
{
    int sec_key;
    long id;
    int price;
    int volume;
    DealType type;
    int oi;
    pt::ptime datetime;

    Deal()
    {}

    Deal(int _sec_key, long _id, int _price, int _volume,
         DealType _type, int _oi, const pt::ptime& _datetime) :
        sec_key(_sec_key),
        id(_id),
        price(_price),
        volume(_volume),
        type(_type),
        oi(_oi),
        datetime(_datetime)
    {}
};

}

#endif // QSH_DEAL_H
