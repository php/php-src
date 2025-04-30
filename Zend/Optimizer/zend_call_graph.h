/*
   +----------------------------------------------------------------------+
   | Zend Engine, Call Graph                                              |
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

#ifndef ZEND_CALL_GRAPH_H
#define ZEND_CALL_GRAPH_H

#include "zend_ssa.h"
#include "zend_func_info.h"
#include "zend_optimizer.h"

typedef struct _zend_send_arg_info {
	zend_op                *opline;
} zend_send_arg_info;

struct _zend_call_info {
	zend_op_array          *caller_op_array;
	zend_op                *caller_init_opline;
	zend_op                *caller_call_opline;
	zend_function          *callee_func;
	zend_call_info         *next_caller;
	zend_call_info         *next_callee;
	bool               recursive;
	bool               send_unpack;  /* Parameters passed by SEND_UNPACK or SEND_ARRAY */
	bool               named_args;   /* Function has named arguments */
	bool               is_prototype; /* An overridden child method may be called */
	bool               is_frameless; /* A frameless function sends arguments through operands */
	int                     num_args;	/* Number of arguments, excluding named and variadic arguments */
	zend_send_arg_info      arg_info[1];
};

struct _zend_func_info {
	int                     num;
	uint32_t                flags;
	zend_ssa                ssa;          /* Static Single Assignment Form  */
	zend_call_info         *caller_info;  /* where this function is called from */
	zend_call_info         *callee_info;  /* which functions are called from this one */
	zend_call_info        **call_map;     /* Call info associated with init/call/send opnum */
	zend_ssa_var_info       return_info;
};

typedef struct _zend_call_graph {
	int                     op_arrays_count;
	zend_op_array         **op_arrays;
	zend_func_info         *func_infos;
} zend_call_graph;

BEGIN_EXTERN_C()

ZEND_API void zend_build_call_graph(zend_arena **arena, zend_script *script, zend_call_graph *call_graph);
ZEND_API void zend_analyze_call_graph(zend_arena **arena, zend_script *script, zend_call_graph *call_graph);
ZEND_API zend_call_info **zend_build_call_map(zend_arena **arena, zend_func_info *info, const zend_op_array *op_array);
ZEND_API void zend_analyze_calls(zend_arena **arena, zend_script *script, uint32_t build_flags, zend_op_array *op_array, zend_func_info *func_info);

END_EXTERN_C()

#endif /* ZEND_CALL_GRAPH_H */
