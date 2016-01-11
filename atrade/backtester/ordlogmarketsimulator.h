#ifndef ORDLOGMARKETSIMULATOR_H
#define ORDLOGMARKETSIMULATOR_H

#include "../headers.h"
#include "simplemarketsimulator.h"
#include "types.h"
#include "utils.hpp"

namespace atrade {
namespace backtester {

class OrdlogMarketSimulator : public SimpleMarketSimulator
{
public:
    OrdlogMarketSimulator();
    IMarketDataSource& add_data_source(std::unique_ptr<IMarketDataSource>&& data_source) override;

protected:
    void handle_ordlog(const BOrdLogEntry& entry);
};

}
}

#endif // ORDLOGMARKETSIMULATOR_H
