#include "qshstream.hpp"

namespace qsh {
namespace reader {
namespace v4 {

QshStream::QshStream(DataReader& data_reader, StreamType stream_type) :
    m_data_reader(&data_reader),
    m_type(stream_type)
{
    if (m_type != StreamType::Messages) {
        m_security.entry(m_data_reader->read_string());
    }
}

StreamType QshStream::type() const
{
    return m_type;
}

Security QshStream::security() const
{
    return m_security;
}

}
}
}
