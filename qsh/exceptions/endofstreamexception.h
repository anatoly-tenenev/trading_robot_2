#ifndef QSH_ENDOFSTREAMEXCEPTION_H
#define QSH_ENDOFSTREAMEXCEPTION_H

#include <exception>

namespace qsh {

class EndOfStreamException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Failed to read past end of stream.";
    }
};

}

#endif // QSH_ENDOFSTREAMEXCEPTION_H
