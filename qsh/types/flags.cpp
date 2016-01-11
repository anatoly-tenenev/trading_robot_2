#include "flags.h"

namespace qsh {

int operator& (OrdLogEntryFlags flag1, OrdLogEntryFlags flag2)
{
    return static_cast<int>(flag1) & static_cast<int>(flag2);
}

int operator& (OrdLogFlags flag1, OrdLogFlags flag2)
{
    return static_cast<int>(flag1) & static_cast<int>(flag2);
}

}
