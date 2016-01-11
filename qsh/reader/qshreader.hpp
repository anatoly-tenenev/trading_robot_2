#ifndef QSHREADER_HPP
#define QSHREADER_HPP

#include "../headers.h"
#include "iqshreader.hpp"

namespace qsh {
namespace reader {

class QshReader
{
public:
    QshReader(const std::string& path);
    std::string app_name() const;
    std::string comment() const;
    pt::ptime header_datetime() const;
    pt::ptime datetime() const;
    std::vector<IQshStream*> streams() const;
    size_t streams_count() const;
    bool eof() const;
    void read(bool push);

private:
    void init(const std::string& path);

private:
    std::vector<char> m_buffer;
    std::istream m_input;
    io::stream_buffer<io::array_source> m_streambuf;
    std::unique_ptr<IQshReader> m_qsh_reader;
};

}
}

#endif // QSHREADER_HPP
