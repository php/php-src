#include "ir.h"
#include "zend_compile.h"
#include "zend_vm.h"

typedef struct _hydra_ir_node {
    uint32_t opcode;
    uint32_t flags;
    struct _hydra_ir_node *next;
} hydra_ir_node;

void hydra_ir_init(void)
{
    /* Initialize IR lowering structures */
}

hydra_ir_node* hydra_ir_lower_op_array(const zend_op_array *op_array)
{
    if (!op_array || op_array->last == 0) {
        return NULL;
    }
    hydra_ir_node *head = pemalloc(sizeof(hydra_ir_node), 1);
    head->opcode = op_array->opcodes[0].opcode;
    head->flags = 1;
    head->next = NULL;
    return head;
}
