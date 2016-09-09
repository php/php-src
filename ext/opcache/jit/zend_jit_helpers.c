/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

static zend_function* ZEND_FASTCALL zend_jit_find_func_helper(zend_string *name)
{
	zval *func = zend_hash_find(EG(function_table), name);
	zend_function *fbc;

	if (UNEXPECTED(func == NULL)) {
		zend_throw_error(NULL, "Call to undefined function %s()", ZSTR_VAL(name));
		return NULL;
	}
	fbc = Z_FUNC_P(func);
	if (EXPECTED(fbc->type == ZEND_USER_FUNCTION) && UNEXPECTED(!fbc->op_array.run_time_cache)) {
		fbc->op_array.run_time_cache = zend_arena_alloc(&CG(arena), fbc->op_array.cache_size);
		memset(fbc->op_array.run_time_cache, 0, fbc->op_array.cache_size);
	}
	return fbc;
}

static zend_execute_data* ZEND_FASTCALL zend_jit_extend_stack_helper(uint32_t used_stack, zend_function *fbc)
{
	zend_execute_data *call = (zend_execute_data*)zend_vm_stack_extend(used_stack);
	call->func = fbc;
	ZEND_SET_CALL_INFO(call, 0, ZEND_CALL_NESTED_FUNCTION|ZEND_CALL_ALLOCATED);
	return call;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
