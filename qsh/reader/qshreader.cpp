#include "qshreader.hpp"
#include "v4/qshreaderimpl.hpp"
#include "../utils.h"
#include "../exception.h"

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>

namespace qsh {
namespace reader {

QshReader::QshReader(const std::string& path) :
    m_input(nullptr)
{
    init(path);
    const char* const signature = "QScalp History Data";
    char cur_signature[50] = {};
    m_input.read(cur_signature, strlen(signature));
    if (strcmp(signature, cur_signature) != 0) {
        throw Exception("Not qsh format");
    }
    u_int8_t version;
    m_input.read(utils::pointer_cast<char*>(&version), 1);
    switch(version)
    {
        case 4:
            m_qsh_reader = std::make_unique<v4::QshReaderImpl>(m_input);
            break;
        default:
            throw Exception("Unsopported qsh version");
    }
}

std::string QshReader::app_name() const
{
    return m_qsh_reader->app_name();
}

std::string QshReader::comment() const
{
    return m_qsh_reader->comment();
}

boost::posix_time::ptime QshReader::header_datetime() const
{
    return m_qsh_reader->header_datetime();
}

boost::posix_time::ptime QshReader::datetime() const
{
    return m_qsh_reader->datetime();
}

std::vector<IQshStream*> QshReader::streams() const
{
    return m_qsh_reader->streams();
}

size_t QshReader::streams_count() const
{
    return m_qsh_reader->streams_count();
}

bool QshReader::eof() const
{
    return m_qsh_reader->eof();
}

void QshReader::read(bool push)
{
    m_qsh_reader->read(push);
}

void QshReader::init(const std::string& path)
{
    io::stream<io::mapped_file_source> file;
    file.open(io::mapped_file_source(path));
    io::filtering_streambuf<io::input> in;
    if (utils::is_gzipped(file)) {
        in.push(io::gzip_decompressor());
    }
    in.push(file);
    io::back_insert_device<std::vector<char>> sink{m_buffer};
    io::stream<io::back_insert_device<std::vector<char>>> out{sink};
    io::copy(in, out);
    m_streambuf.open(io::array_source(m_buffer.data(), m_buffer.size()));
    m_input.rdbuf(&m_streambuf);
}

}
}
