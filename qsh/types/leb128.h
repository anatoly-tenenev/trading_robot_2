#ifndef LEB128_H
#define LEB128_H

#include <sys/types.h>
#include <istream>

class leb128
{
public:
    static const int32_t Min1BValue = -64;
    static const int32_t Max1BValue = -Min1BValue - 1;

    static const int32_t Min2BValue = -8192;
    static const int32_t Max2BValue = -Min2BValue - 1;

    static const int32_t Min3BValue = -1048576;
    static const int32_t Max3BValue = -Min3BValue - 1;

    static const int32_t Min4BValue = -134217728;
    static const int32_t Max4BValue = -Min4BValue - 1;

    static const int64_t Min5BValue = -17179869184;
    static const int64_t Max5BValue = -Min5BValue - 1;

    static const int64_t Min6BValue = -2199023255552;
    static const int64_t Max6BValue = -Min6BValue - 1;

    static const int64_t Min7BValue = -281474976710656;
    static const int64_t Max7BValue = -Min7BValue - 1;

    static const int64_t Min8BValue = -36028797018963968;
    static const int64_t Max8BValue = -Min8BValue - 1;

    static const int64_t Min9BValue = -4611686018427387904;
    static const int64_t Max9BValue = -Min9BValue - 1;

    static int64_t read(std::istream& input);
};

#endif // LEB128_H
