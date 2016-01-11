#include "error_code.hpp"

namespace atrade {
namespace async2 {

const boost::system::error_category& get_atrade_error_category()
{
    static atrade_error_category_impl cat;
    return cat;
}

extern const boost::system::error_category& atrade_error_category;

boost::system::error_code make_error_code(Errc e)
{
    return boost::system::error_code(static_cast<int>(e), atrade_error_category);
}

}
}
