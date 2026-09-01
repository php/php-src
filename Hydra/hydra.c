#include "hydra.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "analysis.h"
#include "profiling.h"
#include "jit.h"

int hydra_startup(void)
{
    hydra_profiling_init();
    hydra_jit_init();
    return SUCCESS;
}

int hydra_shutdown(void)
{
    return SUCCESS;
}

void hydra_inline_candidates(zend_script *script)
{
    if (!script) return;
    
    zend_op_array *op_array = &script->main_op_array;
    if (op_array->last < 1000) {
        hydra_analysis_run(op_array);
        if (hydra_jit_compile_hot_path(op_array)) {
            op_array->fn_flags |= ZEND_ACC_JIT;
        }
    }
}

int hydra_optimize_script(zend_script *script, zend_long optimization_level, zend_long debug_level)
{
    if (optimization_level <= 0) {
        return SUCCESS;
    }

    hydra_inline_candidates(script);

    /* Delegate execution optimization to Zend Optimizer while leveraging Hydra Escape Analysis */
    return zend_optimizer_optimize_ex(script, optimization_level, debug_level, NULL);
}
