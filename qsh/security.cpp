#include "security.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <functional>

namespace qsh {

Security::Security()
{
}

Security::Security(const std::string& entry) :
    Security::Security()
{
    this->entry(entry);
}

void Security::entry(const std::string& entry)
{
    if (entry.empty())
    {
        m_entry.clear();
        reset("");
        return;
    }
    else
    {
        m_entry = entry;
    }
    std::vector<std::string> s;
    boost::split(s, entry, boost::is_any_of(":"));
    if (s.size() == 5)
    {
        m_cname = s[0];
        m_ticker = s[1];
        m_aux_code = s[2];
        bool error = false;
        try
        {
            m_id = boost::lexical_cast<int>(s[3]);
        }
        catch (const boost::bad_lexical_cast&)
        {
            error = true;
            m_id = 0;
        }
        try
        {
            m_step = boost::lexical_cast<double>(s[4]);
        }
        catch (const boost::bad_lexical_cast&)
        {
            error = true;
            m_step = 1.0;
        }
        if (error)
        {
            std::stringstream ticker;
            ticker << "{" << m_ticker << "}";
            m_ticker = ticker.str();
            m_key = 0;
        }
        else
        {
            init_key();
        }
    }
    else
    {
        reset("{err}");
    }
}

int Security::key() const
{
    return m_key;
}

std::string Security::entry() const
{
    return m_entry;
}

double Security::price(int ticks) const
{
    return static_cast<double>(ticks) * m_step;
}

std::string Security::ticker() const
{
    return m_ticker;
}

double Security::step() const
{
    return m_step;
}

int Security::get_key(const std::string& cname)
{
    std::hash<std::string> hash_fn;
    return hash_fn(cname);
}

int Security::get_key(int ckey, int id)
{
    return ckey ^ id;
}

int Security::get_key(int ckey, const std::string& ticker)
{
    std::hash<std::string> hash_fn;
    return ckey ^ hash_fn(ticker);
}

int Security::get_key(int ckey, const std::string& ticker, const std::string& aux_code)
{
    std::hash<std::string> hash_fn;
    return ckey ^ hash_fn(ticker) ^ (~hash_fn(aux_code));
}

void Security::init_key()
{
    if (m_id != 0)
    {
        m_key = get_key(get_key(m_cname), m_id);
    }
    else if (m_aux_code.empty())
    {
        m_key = get_key(get_key(m_cname), m_ticker);
    }
    else
    {
        m_key = get_key(get_key(m_cname), m_ticker, m_aux_code);
    }
}

void Security::reset(const std::string& state)
{
    m_cname = m_ticker = state;
    m_aux_code.clear();
    m_id = 0;
    m_step = 1.0;
    m_key = 0;
}

}
