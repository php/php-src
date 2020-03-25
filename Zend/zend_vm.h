/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_VM_H
#define ZEND_VM_H

BEGIN_EXTERN_C()

ZEND_API void ZEND_FASTCALL zend_vm_set_opcode_handler(zend_op* opcode);
ZEND_API void ZEND_FASTCALL zend_vm_set_opcode_handler_ex(zend_op* opcode, uint32_t op1_info, uint32_t op2_info, uint32_t res_info);
ZEND_API void ZEND_FASTCALL zend_serialize_opcode_handler(zend_op *op);
ZEND_API void ZEND_FASTCALL zend_deserialize_opcode_handler(zend_op *op);
ZEND_API const void* ZEND_FASTCALL zend_get_opcode_handler_func(const zend_op *op);
ZEND_API const zend_op *zend_get_halt_op(void);
ZEND_API int ZEND_FASTCALL zend_vm_call_opcode_handler(zend_execute_data *ex);
ZEND_API int zend_vm_kind(void);

void zend_vm_init(void);
void zend_vm_dtor(void);

END_EXTERN_C()

#define ZEND_VM_SET_OPCODE_HANDLER(opline) zend_vm_set_opcode_handler(opline)

#endif
