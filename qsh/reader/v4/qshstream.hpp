#ifndef QSHSTREAM_HPP
#define QSHSTREAM_HPP

#include "../../headers.h"
#include "../iqshstream.hpp"
#include "datareader.hpp"

namespace qsh {
namespace reader {
namespace v4 {

class QshStream : public ISecurityStream
{
public:
    QshStream(DataReader& data_reader, StreamType stream_type);
    StreamType type() const;
    Security security() const;

protected:
    DataReader* m_data_reader;
    StreamType m_type;
    Security m_security;
};

}
}
}

#endif // QSHSTREAM_HPP
