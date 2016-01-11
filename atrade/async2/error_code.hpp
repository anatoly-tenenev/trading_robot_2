#ifndef ATRADE_ASYNC_2_ERROR_CODE_HPP
#define ATRADE_ASYNC_2_ERROR_CODE_HPP

#include "../headers.h"

namespace atrade {
namespace async2 {

enum class Errc
{
    Timeout = 100000,
    OperationCanceled,
    OperationAborted,
    AddOrderTimeout,
    AddOrderReject,
    OrderCanceled,
    OrderFilled,
    CancelOrderTimeout,
    CancelOrderError,
    MoveOrderTimeout,
    MoveOrderError,
    MakeDealTimeout,
    CtxClosed,
    ReqSecuritiesTimeout,
    WatcherTimeout
};

class atrade_error_category_impl : public boost::system::error_category
{
public:
    const char* name() const noexcept
    {
        return "atrade";
    }

    std::string message(int value) const
    {
        if (value == static_cast<int>(Errc::Timeout))
        {
            return "Timeout";
        }
        if (value == static_cast<int>(Errc::AddOrderReject))
        {
            return "Order was rejected";
        }
        return "atrade error";
    }
};

const boost::system::error_category& get_atrade_error_category();
static const boost::system::error_category& atrade_error_category = get_atrade_error_category();
boost::system::error_code make_error_code(Errc e);

}
}

namespace boost
{
    namespace system
    {
        template<> struct is_error_code_enum<atrade::async2::Errc>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };
    }
}

#endif // ATRADE_ASYNC_2_ERROR_CODE_HPP
