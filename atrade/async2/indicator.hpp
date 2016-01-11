#ifndef ATRADE_ASYNC_2_INDICATOR_HPP
#define ATRADE_ASYNC_2_INDICATOR_HPP

#include "../headers.h"
#include "subscription.hpp"
#include "error_code.hpp"

namespace atrade {
namespace async2 {

template <typename T>
class IIndicator: public Subscribable<T>
{
public:
    typedef T value_type;
    typedef typename Subscribable<T>::Handler handler_type;

    virtual ~IIndicator() {}
    virtual T value() const = 0;
    virtual void value(const T _value) = 0;
};

template <typename T>
class Indicator: public IIndicator<T>
{
public:
    Indicator(T value) :
        m_value(value)
    {
    }

    T value() const override
    {
        return m_value;
    }

protected:
    T m_value;

    void value(const T _value) override
    {
        if (_value != m_value)
        {
            m_value = _value;
            this->publish(m_value);
        }
    }
};

class BooleanIndicator : public Indicator<bool>
{
public:
    BooleanIndicator(bool value) :
        Indicator(value)
    {
    }

    BooleanIndicator() :
        BooleanIndicator(false)
    {
    }
};

class AnyIndicator : public BooleanIndicator
{
public:
    typedef Subscription<BooleanIndicator::handler_type> subscr_type;

    AnyIndicator(const std::vector<BooleanIndicator*>& indicators) :
        BooleanIndicator(),
        m_indicators(),
        m_suscriptions()
    {
        for (auto& indicator : indicators)
        {
            add_indicator(*indicator);
        }
    }

    AnyIndicator() :
        AnyIndicator(std::vector<BooleanIndicator*>{})
    {
    }

    void add_indicator(BooleanIndicator& indicator)
    {
        m_indicators.push_back(&indicator);
        update_value();
        auto subscr = std::make_unique<subscr_type>();
        *subscr = indicator.subscribe([&](bool) {
            update_value();
        });
        m_suscriptions.emplace_back(std::move(subscr));
    }

private:
    std::vector<BooleanIndicator*> m_indicators;
    vector_uptr<subscr_type> m_suscriptions;

    void update_value()
    {
        for (auto& indicator : m_indicators)
        {
            if (indicator->value())
            {
                value(true);
                return;
            }
        }
        value(false);
    }
};

class AllIndicator : public BooleanIndicator
{
public:
    typedef Subscription<BooleanIndicator::handler_type> subscr_type;

    AllIndicator(const std::vector<BooleanIndicator*>& indicators) :
        BooleanIndicator(),
        m_indicators(),
        m_suscriptions()
    {
        for (auto& indicator : indicators)
        {
            add_indicator(*indicator);
        }
    }

    AllIndicator() :
        AllIndicator(std::vector<BooleanIndicator*>{})
    {
    }

    void add_indicator(BooleanIndicator& indicator)
    {
        m_indicators.push_back(&indicator);
        update_value();
        auto subscr = std::make_unique<subscr_type>();
        *subscr = indicator.subscribe([&](bool) {
            update_value();
        });
        m_suscriptions.emplace_back(std::move(subscr));
    }

private:
    std::vector<BooleanIndicator*> m_indicators;
    vector_uptr<subscr_type> m_suscriptions;

    void update_value()
    {
        for (auto& indicator : m_indicators)
        {
            if (!indicator->value())
            {
                value(false);
                return;
            }
        }
        value(true);
    }
};

}
}

#endif // ATRADE_ASYNC_2_INDICATOR_HPP
