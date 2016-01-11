#ifndef QSH_HEADERS_H
#define QSH_HEADERS_H

#include <memory>
#include <string>
#include <istream>
#include <vector>
#include <map>
#include <functional>
#include <sys/types.h>

#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace qsh {
    namespace io = boost::iostreams;
    namespace pt = boost::posix_time;
    namespace gr = boost::gregorian;
    namespace reader {
        class IQshReader;
        class IQshStream;
        class ISecurityStream;
        class QshReader;
        namespace v4 {
            class DataReader;
            class QshReaderImpl;
            class QshStream;
            class OrdLogStream;
        }
    }
    enum class StreamType;
    class Security;
    class OrdLogEntry;
    enum class DealFlags;
    enum class OrderFlags;
    enum class AuxInfoFlags;
    enum class OrdLogEntryFlags;
    enum class OrdLogFlags;
    class RawQuotes;
    struct Quote;
    enum class QuoteType;
    struct Deal;
    enum class DealType;
}

#endif // QSH_HEADERS_H
