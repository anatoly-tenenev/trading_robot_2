#ifndef ULEB128_H
#define ULEB128_H

#include <sys/types.h>
#include <istream>

class uleb128
{
public:
    static const u_int32_t Max1BValue = 127;
    static const u_int32_t Max2BValue = 16383;
    static const u_int32_t Max3BValue = 2097151;
    static const u_int32_t Max4BValue = 268435455;

    static u_int32_t read(std::istream& input);
};

#endif // ULEB128_H
