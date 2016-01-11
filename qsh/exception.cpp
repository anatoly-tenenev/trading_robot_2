#include "exception.h"
#include <cstring>

Exception::Exception()
{
}

Exception::Exception(const char* msg)
{
    strcpy(m_msg, msg);
}

const char* Exception::what() const noexcept
{
    return m_msg;
}

