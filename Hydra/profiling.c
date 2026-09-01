#include "profiling.h"
#include "zend_compile.h"

static uint64_t hydra_hot_path_counter = 0;

void hydra_profiling_init(void)
{
    hydra_hot_path_counter = 0;
}

bool hydra_profiling_check_hot(const zend_op_array *op_array)
{
    if (!op_array) return false;
    hydra_hot_path_counter++;
    return (op_array->last > 15 || hydra_hot_path_counter > 1000);
}
