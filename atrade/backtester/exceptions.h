#ifndef ATRADE_BACKTESTER_EXCEPTIONS_H
#define ATRADE_BACKTESTER_EXCEPTIONS_H

#include "exception"

namespace atrade {
namespace backtester {

class CantFindSecurityException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Can't find security with specific isin_id";
    }
};

class UndefinedOrderDirException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Order direction is undefined.";
    }
};

class UndefinedDealTypeException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Deal type is undefined.";
    }
};

class HaveNotQuotesException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "There have not quotes.";
    }
};

class HasNoDataSourceException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "There has not data source.";
    }
};

class AddOrderException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Add order exception.";
    }
};

class UndefinedOrderTypeException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Order type is undefined.";
    }
};

}
}

#endif // ATRADE_BACKTESTER_EXCEPTIONS_H
