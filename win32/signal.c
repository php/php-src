/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "SAPI.h"

#include "win32/console.h"

/* true globals; only used from main thread and from kernel callback */
static zval ctrl_handler;
static DWORD ctrl_evt = (DWORD)-1;
static zend_bool *vm_interrupt_flag = NULL;

static void (*orig_interrupt_function)(zend_execute_data *execute_data);

static void php_win32_signal_ctrl_interrupt_function(zend_execute_data *execute_data)
{/*{{{*/
	if (IS_UNDEF != Z_TYPE(ctrl_handler)) {
		zval retval, params[1];

		ZVAL_LONG(&params[0], ctrl_evt);

		/* If the function returns, */
		call_user_function(NULL, NULL, &ctrl_handler, &retval, 1, params);
		zval_ptr_dtor(&retval);
	}

	if (orig_interrupt_function) {
		orig_interrupt_function(execute_data);
	}
}/*}}}*/

PHP_WINUTIL_API void php_win32_signal_ctrl_handler_init(void)
{/*{{{*/
	/* We are in the main thread! */
	if (!php_win32_console_is_cli_sapi()) {
		return;
	}

	orig_interrupt_function = zend_interrupt_function;
	zend_interrupt_function = php_win32_signal_ctrl_interrupt_function;
	vm_interrupt_flag = &EG(vm_interrupt);
	ZVAL_UNDEF(&ctrl_handler);

	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_EVENT_CTRL_C", CTRL_C_EVENT, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("PHP_WINDOWS_EVENT_CTRL_BREAK", CTRL_BREAK_EVENT, CONST_PERSISTENT | CONST_CS);
}/*}}}*/

PHP_WINUTIL_API void php_win32_signal_ctrl_handler_shutdown(void)
{/*{{{*/
	if (!php_win32_console_is_cli_sapi()) {
		return;
	}

	zend_interrupt_function = orig_interrupt_function;
	orig_interrupt_function = NULL;
	vm_interrupt_flag = NULL;
	ZVAL_UNDEF(&ctrl_handler);
}/*}}}*/

static BOOL WINAPI php_win32_signal_system_ctrl_handler(DWORD evt)
{/*{{{*/
	if (CTRL_C_EVENT != evt && CTRL_BREAK_EVENT != evt) {
		return FALSE;
	}

	(void)InterlockedExchange8(vm_interrupt_flag, 1);

	ctrl_evt = evt;

	return TRUE;
}/*}}}*/

/* {{{ proto bool sapi_windows_set_ctrl_handler(callable handler, [, bool add = true])
   Assigns a CTRL signal handler to a PHP function */
PHP_FUNCTION(sapi_windows_set_ctrl_handler)
{
	zval *handler = NULL;
	zend_bool add = 1;

#if ZTS
	if (!tsrm_is_main_thread()) {
		php_error_docref(NULL, E_WARNING, "CTRL events can only be received on the main thread");
		return;
	}
#endif

	if (!php_win32_console_is_cli_sapi()) {
		php_error_docref(NULL, E_WARNING, "CTRL events trapping is only supported on console");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &handler, &add) == FAILURE) {
		return;
	}

	if (IS_NULL == Z_TYPE_P(handler)) {
		zval_dtor(&ctrl_handler);
		ZVAL_UNDEF(&ctrl_handler);
		if (!SetConsoleCtrlHandler(NULL, add)) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}

	if (!zend_is_callable(handler, 0, NULL)) {
		zend_string *func_name = zend_get_callable_name(handler);
		php_error_docref(NULL, E_WARNING, "%s is not a callable function name error", ZSTR_VAL(func_name));
		zend_string_release_ex(func_name, 0);
		RETURN_FALSE;
	}

	if (!SetConsoleCtrlHandler(NULL, FALSE) || !SetConsoleCtrlHandler(php_win32_signal_system_ctrl_handler, add)) {
		zend_string *func_name = zend_get_callable_name(handler);
		php_error_docref(NULL, E_WARNING, "Unable to attach %s as a CTRL handler", ZSTR_VAL(func_name));
		zend_string_release_ex(func_name, 0);
		RETURN_FALSE;
	}

	zval_dtor(&ctrl_handler);
	ZVAL_COPY(&ctrl_handler, handler);

	RETURN_TRUE;
}/*}}}*/

PHP_FUNCTION(sapi_windows_generate_ctrl_event)
{/*{{{*/
	zend_long evt, pid = 0;
	zend_bool ret = 0;

	if (!php_win32_console_is_cli_sapi()) {
		php_error_docref(NULL, E_WARNING, "CTRL events trapping is only supported on console");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &evt, &pid) == FAILURE) {
		return;
	}

	SetConsoleCtrlHandler(NULL, TRUE);

	ret = (GenerateConsoleCtrlEvent(evt, pid) != 0);

	if (IS_UNDEF != Z_TYPE(ctrl_handler)) {
		ret = ret && SetConsoleCtrlHandler(php_win32_signal_system_ctrl_handler, TRUE);
	}

	RETURN_BOOL(ret);
}/*}}}*/
