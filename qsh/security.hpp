#ifndef QSH_SECURITY_HPP
#define QSH_SECURITY_HPP

#include "headers.h"

namespace qsh {

class Security
{
public:
    Security();
    Security(const std::string& entry);
    void entry(const std::string& entry);
    int key() const;
    std::string entry() const;
    double price(int ticks) const;
    std::string ticker() const;
    double step() const;

public:
    static int get_key(const std::string& cname);
    static int get_key(int ckey, int id);
    static int get_key(int ckey, const std::string& ticker);
    static int get_key(int ckey, const std::string& ticker, const std::string& aux_code);

private:
    void init_key();
    void reset(const std::string& state);

private:
    std::string m_entry;
    std::string m_cname;
    std::string m_ticker;
    std::string m_aux_code;
    int m_id;
    double m_step;
    int m_key;
};

}

#endif // QSH_SECURITY_HPP
