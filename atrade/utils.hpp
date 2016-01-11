#ifndef ATRADE_UTILS_HPP
#define ATRADE_UTILS_HPP

#include "headers.h"
#include "types/types.h"

namespace atrade {

template <typename FIterator, typename OIterator, typename Pred>
FIterator splice_if( FIterator first, FIterator last, OIterator out, Pred p )
{
   FIterator result = first;
   for ( ; first != last; ++first ) {
      if ( !p( *first ) ) {
         *result++ = *first;
      } else {
         *out++ = *first;
      }
   }
   return result;
}

template< typename tPair >
struct second_t
{
    typename tPair::second_type operator()( const tPair& p ) { return p.second; }
};

template< typename tMap >
second_t< typename tMap::value_type > second( const tMap& ) { return second_t< typename tMap::value_type >(); }

class Lock
{
public:
    Lock (bool& lock) :
        m_lock(&lock)
    {
        *m_lock = true;
    }

    ~Lock()
    {
        *m_lock = false;
    }

    void release()
    {
        *m_lock = false;
    }

private:
    bool* m_lock;
};

template <typename T>
class Counter
{
public:
    Counter(T& counter) :
        m_counter(&counter)
    {
        (*m_counter)++;
    }

    ~Counter()
    {
        --(*m_counter);
    }

private:
    T* m_counter;
};

class ScopeWatcher
{
public:
    ScopeWatcher(const std::function<void()>& handler) :
        m_handler(handler)
    {
    }

    ScopeWatcher() :
        ScopeWatcher(nullptr)
    {
    }

    ~ScopeWatcher()
    {
        if (m_handler)
        {
            try
            {
                m_handler();
            }
            catch (...)
            {
            }
        }
    }

private:
    std::function<void()> m_handler;
};

template <typename TSignature>
class CalledOnce
{
public:
    typedef std::function<TSignature> func_type;
    typedef typename func_type::result_type result_type;

    template <typename T>
    CalledOnce(const T& func) :
        m_func(func),
        m_result{},
        m_called(false)
    {
    }

    CalledOnce() :
        CalledOnce(nullptr)
    {
    }

    CalledOnce(const CalledOnce&) = delete;

    CalledOnce(CalledOnce&& obj)
    {
        move(std::move(obj));
    }

    CalledOnce& operator= (const CalledOnce&) = delete;

    CalledOnce& operator= (CalledOnce&& obj)
    {
        if (this != &obj)
        {
            move(std::move(obj));
        }
        return *this;
    }

    CalledOnce& operator= (std::nullptr_t)
    {
        m_func = nullptr;
        m_called = false;
        return *this;
    }

    template <typename ...Args>
    result_type operator() (Args&&... args)
    {
        return call(std::forward<Args>(args)...);
    }

    template <typename ...Args>
    result_type call(Args&&... args)
    {
        if (!m_called)
        {
            m_called = true;
            m_result = m_func(std::forward<Args>(args)...);
        }
        return m_result;
    }

private:
    func_type m_func;
    result_type m_result;
    bool m_called;

    void move(CalledOnce&& obj)
    {
        m_func = std::move(obj.m_func);
        m_result = std::move(obj.m_result);
        m_called = obj.m_called;
    }
};

template <typename ...Args>
class CalledOnce<void(Args...)>
{
public:
    typedef std::function<void(Args...)> func_type;

    template <typename T>
    CalledOnce(const T& func) :
        m_func(func),
        m_called(false)
    {
    }

    CalledOnce() :
        CalledOnce(nullptr)
    {
    }

    CalledOnce(const CalledOnce&) = delete;

    CalledOnce(CalledOnce&& obj)
    {
        move(std::move(obj));
    }

    CalledOnce& operator= (const CalledOnce&) = delete;

    CalledOnce& operator= (CalledOnce&& obj)
    {
        if (this != &obj)
        {
            move(std::move(obj));
        }
        return *this;
    }

    CalledOnce& operator= (std::nullptr_t)
    {
        m_func = nullptr;
        m_called = false;
        return *this;
    }

    template <typename ..._Args>
    void operator() (_Args&&... args)
    {
        call(std::forward<_Args>(args)...);
    }

    template <typename ..._Args>
    void call(_Args&&... args)
    {
        if (!m_called)
        {
            m_called = true;
            m_func(std::forward<_Args>(args)...);
        }
    }

private:
    func_type m_func;
    bool m_called;

    void move(CalledOnce&& obj)
    {
        m_func = std::move(obj.m_func);
        m_called = obj.m_called;
    }
};

template <typename T>
T* ptr_to_ptr(std::unique_ptr<T>& ptr)
{
    return ptr.get();
}

template <typename T, typename T_ptr = T*>
T* ptr_to_ptr(const T_ptr& ptr)
{
    return ptr;
}

double calc_profit(const std::vector<Order>& orders);
size_t calc_deals_amount(const std::vector<Order>& orders);

double round(double value, int frac);
double price_round(double price);

}

#endif // ATRADE_UTILS_HPP
