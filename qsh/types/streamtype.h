#ifndef STREAMTYPE_H
#define STREAMTYPE_H

namespace qsh {

enum class StreamType
{
    Stock = 0x10,
    Deals = 0x20,
    Orders = 0x30,
    Trades = 0x40,
    Messages = 0x50,
    AuxInfo = 0x60,
    OrdLog = 0x70,
    None = 0
};

}

#endif // STREAMTYPE_H
