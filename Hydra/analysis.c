#include "analysis.h"
#include "zend_compile.h"
#include "zend_vm.h"

bool hydra_escape_analysis_check(const zend_op_array *op_array, uint32_t var_num)
{
    if (!op_array) return false;
    
    zend_op *opline = op_array->opcodes;
    zend_op *end = opline + op_array->last;
    uint32_t ref_count = 0;

    while (opline < end) {
        if (opline->op1_type == IS_VAR && opline->op1.var == var_num) {
            ref_count++;
        }
        if (opline->op2_type == IS_VAR && opline->op2.var == var_num) {
            ref_count++;
        }
        if (opline->opcode == ZEND_RETURN || opline->opcode == ZEND_SEND_VAL || opline->opcode == ZEND_SEND_VAR) {
            if ((opline->op1_type == IS_VAR && opline->op1.var == var_num) ||
                (opline->op2_type == IS_VAR && opline->op2.var == var_num)) {
                return false; /* Escapes scope via return or argument passing */
            }
        }
        opline++;
    }

    return ref_count <= 2; /* Does not escape local scope, candidate for elimination */
}

void hydra_analysis_run(zend_op_array *op_array)
{
    if (!op_array) return;
    
    zend_op *opline = op_array->opcodes;
    zend_op *end = opline + op_array->last;
    
    while (opline < end) {
        if (opline->opcode == ZEND_NEW) {
            if (hydra_escape_analysis_check(op_array, opline->result.var)) {
                opline->extended_value |= ZEND_RETURNS_VALUE;
            }
        }
        opline++;
    }
}
