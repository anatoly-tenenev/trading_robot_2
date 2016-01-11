#ifndef IQSHSTREAM_HPP
#define IQSHSTREAM_HPP

#include "../headers.h"
#include "../security.hpp"
#include "../types/types.h"

namespace qsh {
namespace reader {

class IQshStream
{
public:
    virtual ~IQshStream() {}
    virtual StreamType type() const = 0;
    virtual void read(bool push) = 0;
};

class ISecurityStream : public IQshStream
{
public:
    virtual ~ISecurityStream() {}
    virtual Security security() const = 0;
};

typedef std::function<void(const Deal&)> DealHandler;
typedef std::function<void(int, const OrdLogEntry&)> OrdLogHandler;
typedef std::function<void(int, const std::vector<Quote>&)> StockHandler;

class IOrdLogStream
{
public:
    ~IOrdLogStream() {}
    virtual void handler(const OrdLogHandler& handler) = 0;
    virtual void deal_handler(const DealHandler& handler) = 0;
    virtual void stock_handler(const StockHandler& handler) = 0;
    virtual std::vector<Quote> quotes() const = 0;
};

}
}

#endif // IQSHSTREAM_HPP
