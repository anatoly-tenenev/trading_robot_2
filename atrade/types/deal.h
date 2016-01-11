#ifndef ATRADE_DEAL_H
#define ATRADE_DEAL_H

#include "../headers.h"
#include <sys/types.h>

namespace atrade {

enum class DealType
{
    Undefined,
    Buy,
    Sell
};

DealType operator! (DealType type);

template <typename T>
struct _Deal
{
    int64_t isin_id;
    int64_t deal_id;
    T price;
    int volume;
    DealType type;
    pt::ptime datetime;

    _Deal(int64_t _isin_id, int64_t _deal_id, T _price, int _volume,
          DealType _type, const pt::ptime& _datetime) :
        isin_id(_isin_id),
        deal_id(_deal_id),
        price(_price),
        volume(_volume),
        type(_type),
        datetime(_datetime)
    {
    }

    _Deal(int64_t isin_id, int64_t deal_id, T price, int volume, DealType type) :
        _Deal(isin_id, deal_id, price, volume, type, pt::min_date_time)
    {
    }

    _Deal(int64_t isin_id, T price, int volume, DealType type) :
        _Deal(isin_id, 0, price, volume, type, pt::min_date_time)
    {
    }

    _Deal() :
        _Deal(0, 0, 0, 0, DealType::Undefined, pt::min_date_time)
    {
    }
};

using Deal = _Deal<double>;

}

#endif // ATRADE_DEAL_H
