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
   | Author: Anatol Belski <ab@php.net>                                   |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "SAPI.h"

#include "win32/console.h"

/* true globals; only used from main thread and from kernel callback */
static zval ctrl_handler;
static DWORD ctrl_evt = (DWORD)-1;
static zend_atomic_bool *vm_interrupt_flag = NULL;

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

	zend_atomic_bool_store_ex(vm_interrupt_flag, true);

	ctrl_evt = evt;

	return TRUE;
}/*}}}*/

/* {{{ Assigns a CTRL signal handler to a PHP function */
PHP_FUNCTION(sapi_windows_set_ctrl_handler)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	bool add = 1;


	/* callable argument corresponds to the CTRL handler */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f!|b", &fci, &fcc, &add) == FAILURE) {
		RETURN_THROWS();
	}

#if ZTS
	if (!tsrm_is_main_thread()) {
		zend_throw_error(NULL, "CTRL events can only be received on the main thread");
		RETURN_THROWS();
	}
#endif

	if (!php_win32_console_is_cli_sapi()) {
		zend_throw_error(NULL, "CTRL events trapping is only supported on console");
		RETURN_THROWS();
	}

	if (!ZEND_FCI_INITIALIZED(fci)) {
		zval_ptr_dtor(&ctrl_handler);
		ZVAL_UNDEF(&ctrl_handler);
		if (!SetConsoleCtrlHandler(NULL, add)) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	}

	if (!SetConsoleCtrlHandler(NULL, FALSE) || !SetConsoleCtrlHandler(php_win32_signal_system_ctrl_handler, add)) {
		zend_string *func_name = zend_get_callable_name(&fci.function_name);
		php_error_docref(NULL, E_WARNING, "Unable to attach %s as a CTRL handler", ZSTR_VAL(func_name));
		zend_string_release_ex(func_name, 0);
		RETURN_FALSE;
	}

	zval_ptr_dtor_nogc(&ctrl_handler);
	ZVAL_COPY(&ctrl_handler, &fci.function_name);

	RETURN_TRUE;
}/*}}}*/

PHP_FUNCTION(sapi_windows_generate_ctrl_event)
{/*{{{*/
	zend_long evt, pid = 0;
	bool ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &evt, &pid) == FAILURE) {
		RETURN_THROWS();
	}

	if (!php_win32_console_is_cli_sapi()) {
		zend_throw_error(NULL, "CTRL events trapping is only supported on console");
		return;
	}

	SetConsoleCtrlHandler(NULL, TRUE);

	ret = (GenerateConsoleCtrlEvent(evt, pid) != 0);

	if (IS_UNDEF != Z_TYPE(ctrl_handler)) {
		ret = ret && SetConsoleCtrlHandler(php_win32_signal_system_ctrl_handler, TRUE);
	}

	RETURN_BOOL(ret);
}/*}}}*/
