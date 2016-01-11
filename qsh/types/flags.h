#ifndef QSH_FLAGS_H
#define QSH_FLAGS_H

namespace qsh {

// ************************************************************************

enum class DealFlags
{
    Type = 0x03,

    DateTime = 0x04,
    Id = 0x08,
    OrderId = 0x10,
    Price = 0x20,
    Volume = 0x40,
    OI = 0x80,

    None = 0
};

// **********************************************************************

enum class OrderFlags
{
    DropAll = 0x01,
    Active = 0x02,
    External = 0x04,
    Stop = 0x08,

    None = 0
};

// **********************************************************************

enum class AuxInfoFlags
{
    DateTime = 0x01,

    AskTotal = 0x02,
    BidTotal = 0x04,
    OI = 0x08,
    Price = 0x10,

    SessionInfo = 0x20,
    Rate = 0x40,
    Message = 0x80,

    None = 0
};

// ************************************************************************

enum class OrdLogEntryFlags
{
    DateTime = 0x01,
    OrderId = 0x02,
    Price = 0x04,

    Amount = 0x08,
    AmountRest = 0x10,

    DealId = 0x20,
    DealPrice = 0x40,
    OI = 0x80,

    None = 0
};

int operator& (OrdLogEntryFlags flag1, OrdLogEntryFlags flag2);

// ************************************************************************

enum class OrdLogFlags
{
    NonZeroReplAct = 1 << 0,
    SessIdChanged = 1 << 1,

    Add = 1 << 2,
    Fill = 1 << 3,

    Buy = 1 << 4,
    Sell = 1 << 5,

    Quote = 1 << 7, // Котировочная
    Counter = 1 << 8, // Встречная
    NonSystem = 1 << 9, // Внесистемная
    EndOfTransaction = 1 << 10, // Запись является последней в транзакции
    FillOrKill = 1 << 11, // Заявка Fill-or-kill
    Moved = 1 << 12, // Запись является результатом операции перемещения заявки
    Canceled = 1 << 13, // Запись является результатом операции удаления заявки
    CanceledGroup = 1 << 14, // Запись является результатом группового удаления
    CrossTrade = 1 << 15, // Признак удаления остатка заявки по причине кросс-сделки

    None = 0
};

int operator& (OrdLogFlags flag1, OrdLogFlags flag2);

// ************************************************************************

}

#endif // QSH_FLAGS_H
