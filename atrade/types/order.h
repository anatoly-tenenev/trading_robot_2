#ifndef ATRADE_ORDER_H
#define ATRADE_ORDER_H

#include "../headers.h"
#include <sys/types.h>

namespace atrade {

enum class OrderType
{
    Limit,
    Market,
    Counter,
    FillOrKill
};

enum class OrderDir
{
    Undefined,
    Bid,
    Ask
};

OrderDir operator! (OrderDir dir);

enum class OrderStatus
{
    Cancel,
    Add,
    Fill
};

template <typename T>
struct _Order
{
    int64_t isin_id;
    int64_t order_id;
    OrderDir dir;
    T price;
    int volume;
    OrderType type;
    OrderStatus status;
    int64_t deal_id;
    T deal_price;
    int amount_rest;

    _Order(int64_t _isin_id, int64_t _order_id, OrderDir _dir, T _price, int _volume,
           OrderType _type, OrderStatus _status, int64_t _deal_id, T _deal_price,
           int _amount_rest) :
        isin_id(_isin_id),
        order_id(_order_id),
        dir(_dir),
        price(_price),
        volume(_volume),
        type(_type),
        status(_status),
        deal_id(_deal_id),
        deal_price(_deal_price),
        amount_rest(_amount_rest)
    {}

    _Order(int64_t _isin_id, int64_t _order_id, OrderDir _dir, T _price, int _volume) :
        _Order(_isin_id, _order_id, _dir, _price, _volume, OrderType::Limit, OrderStatus::Add, 0, 0, _volume)
    {}

    _Order(int64_t _isin_id, OrderDir _dir, T _price, int _volume, OrderType _type) :
        _Order(_isin_id, 0, _dir, _price, _volume, _type, OrderStatus::Add, 0, 0, _volume)
    {}

    _Order(int64_t _isin_id, OrderDir _dir, T _price, int _volume) :
        _Order(_isin_id, 0, _dir, _price, _volume, OrderType::Limit, OrderStatus::Add, 0, 0, _volume)
    {}

    _Order(int64_t _isin_id, OrderDir _dir, int _volume) :
        _Order(_isin_id, 0, _dir, 0, _volume, OrderType::Market, OrderStatus::Add, 0, 0, _volume)
    {}

    _Order() :
        _Order(0, 0, OrderDir::Undefined, 0, 0, OrderType::Limit, OrderStatus::Add, 0, 0, 0)
    {}

    bool operator== (const _Order<T>& order) const
    {
        return (isin_id == order.isin_id) &&
               (order_id == order.order_id) &&
               (dir == order.dir) &&
               (price == order.price) &&
               (volume == order.volume) &&
               (type == order.type) &&
               (status == order.status) &&
               (deal_id == order.deal_id) &&
               (deal_price == order.deal_price) &&
               (amount_rest == order.amount_rest);
    }
};

using Order = _Order<double>;

std::ostream& operator<<(std::ostream& out, const Order& order);

enum class OrderOperationType
{
    Add,
    Delete,
    Move
};

struct OrderError
{
    int64_t order_id;
    OrderOperationType op_type;
    int code;

    OrderError(int64_t _order_id, OrderOperationType _op_type, int _code) :
        order_id(_order_id),
        op_type(_op_type),
        code(_code)
    {
    }

    OrderError() :
        OrderError(0, OrderOperationType::Add, 0)
    {
    }

    bool operator== (const OrderError& error) const
    {
        return (order_id == error.order_id) &&
               (op_type == error.op_type) &&
               (code == error.code);
    }
};

}

#endif // ATRADE_ORDER_H
