#ifndef HYDRA_H
#define HYDRA_H

#define HYDRAPHP_VERSION "1.0.0"
#define HYDRAPHP_MAJOR_VERSION 1
#define HYDRAPHP_MINOR_VERSION 0
#define HYDRAPHP_PATCH_VERSION 0

#include "zend.h"
#include "zend_compile.h"
#include "Optimizer/zend_optimizer.h"

#ifndef ZEND_ACC_JIT
#define ZEND_ACC_JIT (1 << 30) /* Hopefully not used */
#endif

BEGIN_EXTERN_C()

int hydra_startup(void);
int hydra_shutdown(void);
int hydra_optimize_script(zend_script *script, zend_long optimization_level, zend_long debug_level);

/* Real optimization hooks */
void hydra_profile_hot_path(const zend_op_array *op_array);
bool hydra_escape_analysis_eliminate_allocation(zend_op_array *op_array, zend_op *opline);
void hydra_type_specialize(zend_op_array *op_array);
void hydra_inline_candidates(zend_script *script);

END_EXTERN_C()

#endif /* HYDRA_H */
