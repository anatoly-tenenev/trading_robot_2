#include "globals.h"

namespace atrade
{

int64_t generate_order_id()
{
    static int64_t order_id = 0;
    return ++order_id;
}

}
