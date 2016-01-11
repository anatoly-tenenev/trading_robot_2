#ifndef ATRADE_SECURITY_H
#define ATRADE_SECURITY_H

#include "headers.h"

namespace atrade {

class Security
{
public:
    Security(int64_t isin_id, const std::string& code, const std::string& full_code,
             double step);
    Security();

    int64_t isin_id() const;
    std::string code() const;
    std::string full_code() const;
    double step() const;

private:
    int64_t m_isin_id;
    std::string m_code;
    std::string m_full_code;
    double m_step;
};

}

#endif // ATRADE_SECURITY_H
