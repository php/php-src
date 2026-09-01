#include "optimizer.h"
#include "hydra.h"
#include "analysis.h"

void hydra_optimizer_run(zend_script *script)
{
    if (!script) return;
    
    hydra_analysis_run(&script->main_op_array);
    hydra_inline_candidates(script);
}
