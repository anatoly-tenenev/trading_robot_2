#ifndef ATRADE_QSHDATASOURCE_H
#define ATRADE_QSHDATASOURCE_H

#include "../headers.h"
#include "marketdatasourcebase.h"
#include <qsh/qsh.hpp>

namespace atrade {
namespace backtester {

class EofQshException : public std::exception
{
public:
    const char* what() const noexcept
    {
        return "End of QSH file";
    }
};

class QshDataSource : public MarketDataSourceBase
{
public:
    QshDataSource(const std::string& path);

    std::vector<Security> securities() const;
    pt::ptime datetime() const;
    void process();

private:
    qsh::reader::QshReader m_reader;
    pt::ptime m_datetime;
    std::unordered_map<int64_t, Security> m_securities;
};

}
}

#endif // ATRADE_QSHDATASOURCE_H
