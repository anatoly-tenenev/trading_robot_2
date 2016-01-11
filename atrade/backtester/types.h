#ifndef ATRADE_BACKTESTER_TYPES_H
#define ATRADE_BACKTESTER_TYPES_H

#include "../types/types.h"

namespace atrade {
namespace backtester {

using BOrdLogEntry = _OrdLogEntry<int>;
using BDeal = _Deal<int>;
using BOrder = _Order<int>;
using BQuote = qsh::Quote;


}
}

#endif // ATRADE_BACKTESTER_TYPES_H
