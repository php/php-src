/*
   +----------------------------------------------------------------------+
   | Zend Engine, Func Info                                               |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_FUNC_INFO_H
#define ZEND_FUNC_INFO_H

#include "zend_ssa.h"

/* func/cfg flags */
#define ZEND_FUNC_INDIRECT_VAR_ACCESS      (1<<0)  /* accesses variables by name  */
#define ZEND_FUNC_HAS_CALLS                (1<<1)
#define ZEND_FUNC_VARARG                   (1<<2)  /* uses func_get_args()        */
#define ZEND_FUNC_NO_LOOPS                 (1<<3)
#define ZEND_FUNC_IRREDUCIBLE              (1<<4)
#define ZEND_FUNC_FREE_LOOP_VAR            (1<<5)
#define ZEND_FUNC_RECURSIVE                (1<<7)
#define ZEND_FUNC_RECURSIVE_DIRECTLY       (1<<8)
#define ZEND_FUNC_RECURSIVE_INDIRECTLY     (1<<9)
#define ZEND_FUNC_HAS_EXTENDED_FCALL       (1<<10)
#define ZEND_FUNC_HAS_EXTENDED_STMT        (1<<11)
#define ZEND_SSA_TSSA                      (1<<12) /* used by tracing JIT */

#define ZEND_FUNC_JIT_ON_FIRST_EXEC        (1<<13) /* used by JIT */
#define ZEND_FUNC_JIT_ON_PROF_REQUEST      (1<<14) /* used by JIT */
#define ZEND_FUNC_JIT_ON_HOT_COUNTERS      (1<<15) /* used by JIT */
#define ZEND_FUNC_JIT_ON_HOT_TRACE         (1<<16) /* used by JIT */


typedef struct _zend_func_info zend_func_info;
typedef struct _zend_call_info zend_call_info;

#define ZEND_FUNC_INFO(op_array) \
	((zend_func_info*)((op_array)->reserved[zend_func_info_rid]))

#define ZEND_SET_FUNC_INFO(op_array, info) do { \
		zend_func_info** pinfo = (zend_func_info**)&(op_array)->reserved[zend_func_info_rid]; \
		*pinfo = info; \
	} while (0)

BEGIN_EXTERN_C()

extern ZEND_API int zend_func_info_rid;

uint32_t zend_get_internal_func_info(
	const zend_function *callee_func, const zend_call_info *call_info, const zend_ssa *ssa);
ZEND_API uint32_t zend_get_func_info(
	const zend_call_info *call_info, const zend_ssa *ssa,
	zend_class_entry **ce, bool *ce_is_instanceof);

zend_result zend_func_info_startup(void);
zend_result zend_func_info_shutdown(void);

END_EXTERN_C()

#endif /* ZEND_FUNC_INFO_H */
