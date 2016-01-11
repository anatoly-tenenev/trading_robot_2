#ifndef QSHREADERIMPL_HPP
#define QSHREADERIMPL_HPP

#include "../../headers.h"
#include "../iqshreader.hpp"
#include "../iqshstream.hpp"
#include "datareader.hpp"

namespace qsh {
namespace reader {
namespace v4 {

class QshReaderImpl : public IQshReader
{
public:
    QshReaderImpl(std::istream& input);
    std::string app_name() const;
    std::string comment() const;
    pt::ptime header_datetime() const;
    pt::ptime datetime() const;
    std::vector<IQshStream*> streams() const;
    size_t streams_count() const;
    bool eof() const;
    void read(bool push);
    void read_next_record_header();

private:
    DataReader m_data_reader;
    std::vector<std::unique_ptr<IQshStream>> m_streams;
    std::string m_app_name;
    std::string m_comment;
    pt::ptime m_datetime;
    bool m_eof;
    IQshStream* m_current_stream;
    int64_t m_last_millisecs;
    pt::ptime m_current_datetime;
};

}
}
}

#endif // QSHREADERIMPL_HPP
