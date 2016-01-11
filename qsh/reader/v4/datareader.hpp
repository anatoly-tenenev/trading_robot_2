#ifndef DATAREADER_HPP
#define DATAREADER_HPP

#include "../../headers.h"
#include "../../utils.h"

namespace qsh {
namespace reader {
namespace v4 {

class DataReader
{
public:
    DataReader(std::istream& input);
    u_int8_t read_byte();
    u_int16_t read_uint16();
    std::string read_string();
    pt::ptime read_datetime();
    int64_t read_growing(int64_t last_value);
    int64_t read_leb128();

    template <typename T>
    T read()
    {
        T value;
        m_input->read(utils::pointer_cast<char*>(&value), sizeof(value));
        return value;
    }

private:
    std::istream* m_input;
};

}
}
}

#endif // DATAREADER_HPP
