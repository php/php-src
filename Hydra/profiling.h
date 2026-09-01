#ifndef HYDRA_PROFILING_H
#define HYDRA_PROFILING_H
#include "zend.h"
void hydra_profiling_init(void);
bool hydra_profiling_check_hot(const zend_op_array *op_array);
#endif
