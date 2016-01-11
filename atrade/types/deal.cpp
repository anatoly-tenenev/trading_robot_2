#include "deal.h"

namespace atrade {

DealType operator! (DealType type)
{
    switch (type)
    {
    case DealType::Buy:
        return DealType::Sell;
    case DealType::Sell:
        return DealType::Buy;
    default:
        return type;
    }
}

}
