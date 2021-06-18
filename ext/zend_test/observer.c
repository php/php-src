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
				zend_uchar opcode = zend_get_opcode_id(s, e - s);
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
	if (!ZT_G(observer_show_opcode)) {
		return;
	}
	php_printf("%*s<!-- opcode: '%s' -->\n", 2 * ZT_G(observer_nesting_depth), "", zend_get_opcode_name(EX(opline)->opcode));
}

static void observer_begin(zend_execute_data *execute_data)
{
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
	} else if (ZT_G(observer_observe_includes) && !fbc->common.function_name) {
		return (zend_observer_fcall_handlers){observer_begin, observer_end};
	} else if (ZT_G(observer_observe_functions) && fbc->common.function_name) {
		return (zend_observer_fcall_handlers){observer_begin, observer_end};
	}
	return (zend_observer_fcall_handlers){NULL, NULL};
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

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("zend_test.observer.enabled", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_enabled, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_output", "1", PHP_INI_SYSTEM, OnUpdateBool, observer_show_output, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_all", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_all, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_includes", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_includes, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.observe_functions", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_observe_functions, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_type", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_type, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_return_value", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_return_value, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_init_backtrace", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_init_backtrace, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.show_opcode", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_show_opcode, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_ENTRY("zend_test.observer.show_opcode_in_user_handler", "", PHP_INI_SYSTEM, OnUpdateString, observer_show_opcode_in_user_handler, zend_zend_test_globals, zend_test_globals)
	STD_PHP_INI_BOOLEAN("zend_test.observer.fiber_switch", "0", PHP_INI_SYSTEM, OnUpdateBool, observer_fiber_switch, zend_zend_test_globals, zend_test_globals)
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
		zend_observer_fiber_switch_register(fiber_address_observer);
		zend_observer_fiber_switch_register(fiber_enter_observer);
		zend_observer_fiber_switch_register(fiber_suspend_observer);
	}
}

void zend_test_observer_shutdown(SHUTDOWN_FUNC_ARGS)
{
	if (type != MODULE_TEMPORARY) {
		UNREGISTER_INI_ENTRIES();
	}
}
