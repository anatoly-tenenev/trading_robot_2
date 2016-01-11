#ifndef ATRADE_ASYNC_2_EXCEPTIONS_HPP
#define ATRADE_ASYNC_2_EXCEPTIONS_HPP

#include <exception>

namespace atrade {
namespace async2 {

class NotFoundException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Not found.";
    }
};

class CtxClosedException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Trading context is closed.";
    }
};

class BadTaskException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "You try to push a bad task.";
    }
};

class TaskScheduledException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Task has already scheduled.";
    }
};

class TaskCanceledException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Task has already canceled.";
    }
};

class TaskExecutedException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Task has already canceled.";
    }
};

class BadSubscriptionException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "You try to push a bad subscription.";
    }
};

class SubscriptionIsActiveException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Subscription is active. You should cancel it.";
    }
};

class SubscriptionRemovedException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Subscription has already removed.";
    }
};

class UndefinedDatetimeException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Datetime is undefined.";
    }
};

class BadDatetimeException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "Datetime is bad.";
    }
};

}
}

#endif // ATRADE_ASYNC_2_EXCEPTIONS_HPP

