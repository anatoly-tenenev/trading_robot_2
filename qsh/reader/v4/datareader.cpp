#include "datareader.hpp"
#include "../../types/uleb128.h"
#include "../../types/leb128.h"

namespace qsh {
namespace reader {
namespace v4 {

DataReader::DataReader(std::istream& input) :
    m_input(&input)
{
}

u_int8_t DataReader::read_byte()
{
    char byte;
    m_input->read(&byte, 1);
    return static_cast<u_int8_t>(byte);
}

u_int16_t DataReader::read_uint16()
{
    u_int16_t value;
    m_input->read(utils::pointer_cast<char*>(&value), sizeof(value));
    return value;
}

std::string DataReader::read_string()
{
    size_t length = uleb128::read(*m_input);
    std::vector<char> buffer(length);
    m_input->read(buffer.data(), length);
    return std::string(buffer.begin(), buffer.end());
}

boost::posix_time::ptime DataReader::read_datetime()
{
    const int64_t csharp_1900_ticks = 599266080000000000L;
    int64_t ticks;
    m_input->read(utils::pointer_cast<char*>(&ticks), sizeof(ticks));
    ticks -= csharp_1900_ticks;
    pt::ptime epoch(gr::date(1900,gr::Jan,1));
    return epoch + pt::microseconds(ticks/10);
}

int64_t DataReader::read_growing(int64_t last_value)
{
    u_int32_t offset = uleb128::read(*m_input);
    if(offset == uleb128::Max4BValue)
        return last_value + leb128::read(*m_input);
    else
        return last_value + static_cast<int64_t>(offset);
}

int64_t DataReader::read_leb128()
{
    return leb128::read(*m_input);
}

}
}
}
