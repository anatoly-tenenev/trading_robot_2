#ifndef ATRADE_BACKTESTER_UTILS_HPP
#define ATRADE_BACKTESTER_UTILS_HPP

#include "types.h"

namespace atrade {
namespace backtester {

Order to_order(const BOrder& order, double step);
BOrder from_order(const Order& order, double step);

Deal to_deal(const BDeal& deal, double step);
BDeal from_deal(const Deal& deal, double step);
BDeal from_qsh_deal(const qsh::Deal& deal);

std::vector<Quote> to_stocks(const std::vector<BQuote>& stocks, double step);

OrdLogEntry to_ordlog_entry(const BOrdLogEntry& entry, double step);
BOrdLogEntry from_ordlog_entry(const OrdLogEntry& entry, double step);
BOrdLogEntry from_qsh_ordlog_entry(const qsh::OrdLogEntry& entry);

}
}

#endif // ATRADE_BACKTESTER_UTILS_HPP
