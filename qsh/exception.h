#ifndef QSH_EXCEPTION_H
#define QSH_EXCEPTION_H

#include <exception>

class Exception : public std::exception
{
public:
    Exception();
    Exception(const char* msg);
    const char* what() const noexcept;

private:
    char m_msg[256];
};

#endif // EXCEPTION_H
