#include "uleb128.h"
#include "../exceptions/endofstreamexception.h"

u_int32_t uleb128::read(std::istream& input)
{
    u_int32_t value = 0;
    u_int32_t shift = 0;
    while (true)
    {
        char byte = 0;
        input.read(&byte, 1);
        if (input.eof())
        {
            throw qsh::EndOfStreamException();
        }
        value |= ((byte & 0x7f) << shift);
        if((byte & 0x80) == 0)
        {
            return value;
        }
        shift += 7;
    }
}

