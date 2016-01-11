#include "leb128.h"
#include "../exceptions/endofstreamexception.h"

int64_t leb128::read(std::istream& input)
{
    int64_t value = 0;
    u_int32_t shift = 0;
    while (true)
    {
        char byte = 0;
        input.read(&byte, 1);
        if (input.eof())
        {
            throw qsh::EndOfStreamException();
        }
        value |= (static_cast<int64_t>(byte & 0x7f) << shift);
        shift += 7;
        if((byte & 0x80) == 0)
        {
            if((shift < sizeof(int64_t) * 8) && ((byte & 0x40) != 0))
            {
                value |= -(1L << shift);
            }
            return value;
        }
    }
}

