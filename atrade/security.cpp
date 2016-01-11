#include "security.h"

namespace atrade {

Security::Security(int64_t isin_id, const std::string& code, const std::string& full_code,
                   double step) :
    m_isin_id(isin_id),
    m_code(code),
    m_full_code(full_code),
    m_step(step)
{
}

Security::Security() :
    Security(0, "", "", 0.0)
{
}

int64_t Security::isin_id() const
{
    return m_isin_id;
}

std::string Security::code() const
{
    return m_code;
}

std::string Security::full_code() const
{
    return m_full_code;
}

double Security::step() const
{
    return m_step;
}

}
