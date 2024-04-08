/*
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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_test.h"
#include "observer.h"
#include "zend_observer.h"
#include "zend_smart_str.h"
#include "ext/standard/php_var.h"

static zend_observer_fcall_handlers observer_fcall_init(zend_execute_data *execute_data);

static int observer_show_opcode_in_user_handler(zend_execute_data *execute_data)
{
	if (ZT_G(observer_show_output)) {
		php_printf("%*s<!-- opcode: '%s' in user handler -->\n", 2 * ZT_G(observer_nesting_depth), "", zend_get_opcode_name(EX(opline)->opcode));
	}

	return ZEND_USER_OPCODE_DISPATCH;
}

static void observer_set_user_opcode_handler(const char *opcode_names, user_opcode_handler_t handler)
{
	const char *s = NULL, *e = opcode_names;

	while (1) {
		if (*e == ' ' || *e == ',' || *e == '\0') {
			if (s) {
				uint8_t opcode = zend_get_opcode_id(s, e - s);
				if (opcode <= ZEND_VM_LAST_OPCODE) {
					zend_set_user_opcode_handler(opcode, handler);
				} else {
					zend_error(E_WARNING, "Invalid opcode name %.*s", (int) (e - s), e);
				}
				s = NULL;
			}
		} else {
			if (!s) {
				s = e;
			}
		}
		if (*e == '\0') {
			break;
		}
		e++;
	}
}

static void observer_show_opcode(zend_execute_data *execute_data)
{
	if (!ZT_G(observer_show_opcode) || !ZEND_USER_CODE(EX(func)->type)) {
		return;
	}
	php_printf("%*s<!-- opcode: '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", zend_get_opcode_name(EX(opline)->opcode));
}

static inline void assert_observer_opline(zend_execute_data *execute_data) {
	ZEND_ASSERT(!ZEND_USER_CODE(EX(func)->type) ||
		(EX(opline) >= EX(func)->op_array.opcodes && EX(opline) < EX(func)->op_array.opcodes + EX(func)->op_array.last) ||
		(EX(opline) >= EG(exception_op) && EX(opline) < EG(exception_op) + 3));
}

static void observer_begin(zend_execute_data *execute_data)
{
	assert_observer_opline(execute_data);

	if (!ZT_G(observer_show_output)) {
		return;
	}

	if (execute_data->func && execute_data->func->common.function_name) {
		if (execute_data->func->common.scope) {
			php_printf("%*s<%s::%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.scope->name), ZSTR_VAL(execute_data->func->common.function_name));
		} else {
			php_printf("%*s<%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.function_name));
		}
	} else {
		php_printf("%*s<file '%s'>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->op_array.filename));
	}
	ZT_G(observer_nesting_depth)++;
	observer_show_opcode(execute_data);
}

static void get_retval_info(zval *retval, smart_str *buf)
{
	if (!ZT_G(observer_show_return_type) && !ZT_G(observer_show_return_value)) {
		return;
	}

	smart_str_appendc(buf, ':');
	if (retval == NULL) {
		smart_str_appendl(buf, "NULL", 4);
	} else if (ZT_G(observer_show_return_value)) {
		if (Z_TYPE_P(retval) == IS_OBJECT) {
			smart_str_appendl(buf, "object(", 7);
			smart_str_append(buf, Z_OBJCE_P(retval)->name);
			smart_str_appendl(buf, ")#", 2);
			smart_str_append_long(buf, Z_OBJ_HANDLE_P(retval));
		} else {
			php_var_export_ex(retval, 2 * ZT_G(observer_nesting_depth) + 3, buf);
		}
	} else if (ZT_G(observer_show_return_type)) {
		smart_str_appends(buf, zend_zval_type_name(retval));
	}
	smart_str_0(buf);
}

static void observer_end(zend_execute_data *execute_data, zval *retval)
{
	assert_observer_opline(execute_data);

	if (!ZT_G(observer_show_output)) {
		return;
	}

	if (EG(exception)) {
		php_printf("%*s<!-- Exception: %s -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(EG(exception)->ce->name));
	}
	observer_show_opcode(execute_data);
	ZT_G(observer_nesting_depth)--;
	if (execute_data->func && execute_data->func->common.function_name) {
		smart_str retval_info = {0};
		get_retval_info(retval, &retval_info);
		if (execute_data->func->common.scope) {
			php_printf("%*s</%s::%s%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.scope->name), ZSTR_VAL(execute_data->func->common.function_name), retval_info.s ? ZSTR_VAL(retval_info.s) : "");
		} else {
			php_printf("%*s</%s%s>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->common.function_name), retval_info.s ? ZSTR_VAL(retval_info.s) : "");
		}
		smart_str_free(&retval_info);
	} else {
		php_printf("%*s</file '%s'>\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(execute_data->func->op_array.filename));
	}
}

static void observer_show_init(zend_function *fbc)
{
	if (fbc->common.function_name) {
		if (fbc->common.scope) {
			php_printf("%*s<!-- init %s::%s() -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
		} else {
			php_printf("%*s<!-- init %s() -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->common.function_name));
		}
	} else {
		php_printf("%*s<!-- init '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->op_array.filename));
	}
}

static void observer_show_init_backtrace(zend_execute_data *execute_data)
{
	zend_execute_data *ex = execute_data;
	php_printf("%*s<!--\n", 2 * ZT_G(observer_nesting_depth), "");
	do {
		zend_function *fbc = ex->func;
		int indent = 2 * ZT_G(observer_nesting_depth) + 4;
		if (fbc->common.function_name) {
			if (fbc->common.scope) {
				php_printf("%*s%s::%s()\n", indent, "", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
			} else {
				php_printf("%*s%s()\n", indent, "", ZSTR_VAL(fbc->common.function_name));
			}
		} else {
			php_printf("%*s{main} %s\n", indent, "", ZSTR_VAL(fbc->op_array.filename));
		}
	} while ((ex = ex->prev_execute_data) != NULL);
	php_printf("%*s-->\n", 2 * ZT_G(observer_nesting_depth), "");
}

static zend_observer_fcall_handlers observer_fcall_init(zend_execute_data *execute_data)
{
	zend_function *fbc = execute_data->func;
	if (ZT_G(observer_show_output)) {
		observer_show_init(fbc);
		if (ZT_G(observer_show_init_backtrace)) {
			observer_show_init_backtrace(execute_data);
		}
		observer_show_opcode(execute_data);
	}

	if (ZT_G(observer_observe_all)) {
		return (zend_observer_fcall_handlers){observer_begin, observer_end};
	} else if (fbc->common.function_name) {
		if (ZT_G(observer_observe_functions)) {
			return (zend_observer_fcall_handlers){observer_begin, observer_end};
		} else if (zend_hash_exists(ZT_G(observer_observe_function_names), fbc->common.function_name)) {
			return (zend_observer_fcall_handlers){observer_begin, observer_end};
		}
	} else {
		if (ZT_G(observer_observe_includes)) {
			return (zend_observer_fcall_handlers){observer_begin, observer_end};
		}
	}
	return (zend_observer_fcall_handlers){NULL, NULL};
}

static void fiber_init_observer(zend_fiber_context *initializing) {
	if (ZT_G(observer_fiber_init)) {
		php_printf("<!-- alloc: %p -->\n", initializing);
	}
}

static void fiber_destroy_observer(zend_fiber_context *destroying) {
	if (ZT_G(observer_fiber_destroy)) {
		php_printf("<!-- destroy: %p -->\n", destroying);
	}
}

static void fiber_address_observer(zend_fiber_context *from, zend_fiber_context *to)
{
	if (ZT_G(observer_fiber_switch)) {
		php_printf("<!-- switching from fiber %p to %p -->\n", from, to);
	}
}

static void fiber_enter_observer(zend_fiber_context *from, zend_fiber_context *to)
{
	if (ZT_G(observer_fiber_switch)) {
		if (to->status == ZEND_FIBER_STATUS_INIT) {
			php_printf("<init '%p'>\n", to);
		} else if (to->kind == zend_ce_fiber) {
			zend_fiber *fiber = zend_fiber_from_context(to);
			if (fiber->caller != from) {
				return;
			}

			if (fiber->flags & ZEND_FIBER_FLAG_DESTROYED) {
				php_printf("<destroying '%p'>\n", to);
			} else if (to->status != ZEND_FIBER_STATUS_DEAD) {
				php_printf("<resume '%p'>\n", to);
			}
		}
	}
}

static void fiber_suspend_observer(zend_fiber_context *from, zend_fiber_context *to)
{
	if (ZT_G(observer_fiber_switch)) {
		if (from->status == ZEND_FIBER_STATUS_DEAD) {
			zend_fiber *fiber = (from->kind == zend_ce_fiber) ? zend_fiber_from_context(from) : NULL;

			if (fiber && fiber->flags & ZEND_FIBER_FLAG_THREW) {
				php_printf("<threw '%p'>\n", from);
			} else if (fiber && fiber->flags & ZEND_FIBER_FLAG_DESTROYED) {
				php_printf("<destroyed '%p'>\n", from);
			} else {
				php_printf("<returned '%p'>\n", from);
			}
		} else if (from->kind == zend_ce_fiber) {
			zend_fiber *fiber = zend_fiber_from_context(from);
			if (fiber->caller == NULL) {
				php_printf("<suspend '%p'>\n", from);
			}
		}
	}
}

void declared_function_observer(zend_op_array *op_array, zend_string *name) {
	if (ZT_G(observer_observe_declaring)) {
		php_printf("%*s<!-- declared function '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(name));
	}
}

void declared_class_observer(zend_class_entry *ce, zend_string *name) {
	if (ZT_G(observer_observe_declaring)) {
		php_printf("%*s<!-- declared class '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(name));
	}
}

static void (*zend_test_prev_execute_internal)(zend_execute_data *execute_data, zval *return_value);
static void zend_test_execute_internal(zend_execute_data *execute_data, zval *return_value) {
	zend_function *fbc = execute_data->func;

	if (fbc->common.function_name) {
		if (fbc->common.scope) {
			php_printf("%*s<!-- internal enter %s::%s() -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->common.scope->name), ZSTR_VAL(fbc->common.function_name));
		} else {
			php_printf("%*s<!-- internal enter %s() -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->common.function_name));
		}
	} else {
		php_printf("%*s<!-- internal enter '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", ZSTR_VAL(fbc->op_array.filename));
	}

	if (zend_test_prev_execute_internal) {
		zend_test_prev_execute_internal(execute_data, return_value);
	} else {
		fbc->internal_function.handler(execute_data, return_value);
	}
}

static ZEND_INI_MH(zend_test_observer_OnUpdateCommaList)
{
	zend_array **p = (zend_array **) ZEND_INI_GET_ADDR();
	zend_string *funcname;
	zend_function *func;
	if (stage != PHP_INI_STAGE_STARTUP && stage != PHP_INI_STAGE_ACTIVATE && stage != PHP_INI_STAGE_DEACTIVATE && stage != PHP_INI_STAGE_SHUTDOWN) {
		ZEND_HASH_FOREACH_STR_KEY(*p, funcname) {
			if ((func = zend_hash_find_ptr(EG(function_table), funcname))) {
				zend_observer_remove_begin_handler(func, observer_begin);
				zend_observer_remove_end_handler(func, observer_end);
			}
		} ZEND_HASH_FOREACH_END();
	}
	zend_hash_clean(*p);
	if (new_value && ZSTR_LEN(new_value)) {
		const char *start = ZSTR_VAL(new_value), *ptr;
		while ((ptr = strchr(start, ','))) {
			zend_string *str = zend_string_init(start, ptr - start, 1);
			GC_MAKE_PERSISTENT_LOCAL(str);
			zend_hash_add_empty_element(*p, str);
			zend_string_release(str);
			start = ptr + 1;
		}
		zend_string *str = zend_string_init(start, ZSTR_VAL(new_value) + ZSTR_LEN(new_value) - start, 1);
		GC_MAKE_PERSISTENT_LOCAL(str);
		zend_hash_add_empty_element(*p, str);
		zend_string_release(str);
		if (stage != PHP_INI_STAGE_STARTUP && stage != PHP_INI_STAGE_ACTIVATE && stage != PHP_INI_STAGE_DEACTIVATE && stage != PHP_INI_STAGE_SHUTDOWN) {
			ZEND_HASH_FOREACH_STR_KEY(*p, funcname) {
				if ((func = zend_hash_find_ptr(EG(function_table), funcname))) {
					zend_observer_add_begin_handler(func, observer_begin);
					zend_observer_add_end_handler(func, observer_end);
				}
			} ZEND_HASH_FOREACH_END();
		}
	}
	return SUCCESS;
}

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.observer.enabled", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_enabled, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_output", "1", PHP_INI_SYSTEM, OnUpdateBool, observer_show_output, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_all", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_all, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_includes", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_includes, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_functions", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_functions, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_declaring", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_declaring, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_ENTRY("zend_test.observer.observe_function_names", "", PHP_INI_ALL, zend_test_observer_OnUpdateCommaList, observer_observe_function_names, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_type", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_type, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_value", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_value, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_init_backtrace", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_init_backtrace, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_opcode", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_opcode, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_ENTRY("zend_test.observer.show_opcode_in_user_handler", "", PHP_INI_SYSTEM, OnUpdateString, observer_show_opcode_in_user_handler, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.fiber_init", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_fiber_init, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.fiber_switch", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_fiber_switch, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.fiber_destroy", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_fiber_destroy, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.execute_internal", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_execute_internal, zend_zend_test_globals, zend_test_globals)
PHP_INI_END()

void zend_test_observer_init(INIT_FUNC_ARGS)
{
	// Loading via dl() not supported with the observer API
	if (type != MODULE_TEMPORARY) {
		REGISTER_INI_ENTRIES();
		if (ZT_G(observer_enabled)) {
			zend_observer_fcall_register(observer_fcall_init);
		}
	} else {
		(void)ini_entries;
	}

	if (ZT_G(observer_enabled) && ZT_G(observer_show_opcode_in_user_handler)) {
		observer_set_user_opcode_handler(ZT_G(observer_show_opcode_in_user_handler), observer_show_opcode_in_user_handler);
	}

	if (ZT_G(observer_enabled)) {
		zend_observer_fiber_init_register(fiber_init_observer);
		zend_observer_fiber_switch_register(fiber_address_observer);
		zend_observer_fiber_switch_register(fiber_enter_observer);
		zend_observer_fiber_switch_register(fiber_suspend_observer);
		zend_observer_fiber_destroy_register(fiber_destroy_observer);

		zend_observer_function_declared_register(declared_function_observer);
		zend_observer_class_linked_register(declared_class_observer);
	}

	if (ZT_G(observer_execute_internal)) {
		zend_test_prev_execute_internal = zend_execute_internal;
		zend_execute_internal = zend_test_execute_internal;
	}
}

void zend_test_observer_shutdown(SHUTDOWN_FUNC_ARGS)
{
	if (type != MODULE_TEMPORARY) {
		UNREGISTER_INI_ENTRIES();
	}
}

void zend_test_observer_ginit(zend_zend_test_globals *zend_test_globals) {
	zend_test_globals->observer_observe_function_names = malloc(sizeof(HashTable));
	_zend_hash_init(zend_test_globals->observer_observe_function_names, 8, ZVAL_PTR_DTOR, 1);
	GC_MAKE_PERSISTENT_LOCAL(zend_test_globals->observer_observe_function_names);
}

void zend_test_observer_gshutdown(zend_zend_test_globals *zend_test_globals) {
	zend_hash_release(zend_test_globals->observer_observe_function_names);
}
