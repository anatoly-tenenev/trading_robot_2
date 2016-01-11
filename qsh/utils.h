#ifndef UTILS_H
#define UTILS_H

#include "headers.h"

namespace qsh {
namespace utils {

template<typename result, typename source>
result pointer_cast(source* v)
{
    return static_cast<result>(static_cast<void*>(v));
}

template<typename result, typename source>
result pointer_cast(const source* v)
{
    return static_cast<result>( const_cast<void*>( static_cast<const void*>(v) ) );
}

bool is_gzipped(std::istream& input);

pt::ptime datetime_from_millisecs(int64_t millisecs);
int64_t datetime_to_millisecs(const pt::ptime& datetime);

}
}

#endif // UTILS_H
