#include "utils.h"

namespace qsh {
namespace utils {

bool is_gzipped(std::istream& input)
{
    u_int8_t magic[2];
    input.read(pointer_cast<char*>(magic), 2);
    input.seekg(std::ios_base::beg);
    return (magic[0] == 0x1f) && (magic[1] == 0x8b);
}

pt::ptime datetime_from_millisecs(int64_t millisecs)
{
    const int64_t csharp_1900_millisecs = 59926608000000L;
    pt::ptime epoch(gr::date(1900,gr::Jan,1));
    return epoch + pt::milliseconds(millisecs - csharp_1900_millisecs);
}

int64_t datetime_to_millisecs(const pt::ptime& datetime)
{
    const int64_t csharp_1900_millisecs = 59926608000000L;
    pt::ptime epoch(gr::date(1900,gr::Jan,1));
    return csharp_1900_millisecs + (datetime - epoch).total_milliseconds();
}

}
}
