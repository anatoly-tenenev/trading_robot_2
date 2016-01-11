#include "qshreaderimpl.hpp"
#include "ordlogstream.hpp"
#include "../../exception.h"
#include "../../types/streamtype.h"
#include "../../exceptions/endofstreamexception.h"

namespace qsh {
namespace reader {
namespace v4 {

QshReaderImpl::QshReaderImpl(std::istream& input) :
    m_data_reader{input},
    m_app_name(m_data_reader.read_string()),
    m_comment(m_data_reader.read_string()),
    m_datetime(m_data_reader.read_datetime()),
    m_eof(false),
    m_last_millisecs(utils::datetime_to_millisecs(m_datetime))
{
    size_t streams_count = m_data_reader.read_byte();
    for (size_t i = 0; i < streams_count; ++i)
    {
        StreamType st = static_cast<StreamType>(m_data_reader.read_byte());
        switch (st)
        {
            case StreamType::OrdLog:
                m_streams.push_back(std::make_unique<OrdLogStream>(m_data_reader));
                break;
            default:
                throw Exception("Unsopported stream");
        }
    }
    if (m_streams.size() == 1)
    {
        m_current_stream = m_streams[0].get();
    }
    read_next_record_header();
}

std::string QshReaderImpl::app_name() const
{
    return m_app_name;
}

std::string QshReaderImpl::comment() const
{
    return m_comment;
}

boost::posix_time::ptime QshReaderImpl::header_datetime() const
{
    return m_datetime;
}

boost::posix_time::ptime QshReaderImpl::datetime() const
{
    return m_current_datetime;
}

std::vector<IQshStream*> QshReaderImpl::streams() const
{
    std::vector<IQshStream*> streams;
    for (const auto& stream : m_streams)
    {
        streams.push_back(stream.get());
    }
    return streams;
}

size_t QshReaderImpl::streams_count() const
{
    return m_streams.size();
}

bool QshReaderImpl::eof() const
{
    return m_eof;
}

void QshReaderImpl::read(bool push)
{
    m_current_stream->read(push);
    read_next_record_header();
}

void QshReaderImpl::read_next_record_header()
{
    try
    {
        m_last_millisecs = m_data_reader.read_growing(m_last_millisecs);
        m_current_datetime = utils::datetime_from_millisecs(m_last_millisecs);
        if (m_streams.size() > 1)
        {
            m_current_stream = m_streams[m_data_reader.read_byte()].get();
        }
    }
    catch (EndOfStreamException&)
    {
        m_eof = true;
    }
}

}
}
}
