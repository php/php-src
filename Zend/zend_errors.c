/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Patrick Allaert <patrick@catchy.io>                         |
   +----------------------------------------------------------------------+
*/

/* $Id: $ */

#include "zend.h"
#include "zend_errors.h"
#include "php.h"
#include "php_globals.h"
#include "php_syslog.h"
#include "SAPI.h"
#include "ext/standard/html.h"

/* {{{ zend_error_display_cb */
ZEND_ERROR_CB_API zend_error_display_cb(ZEND_ERROR_CB_HOOK_ARGS)
{
	char *buffer = PG(last_error_message);

	if (PG(display_errors) && ((php_get_module_initialized() && !PG(during_request_startup)) || (PG(display_startup_errors)))) {
		if (PG(xmlrpc_errors)) {
			php_printf("<?xml version=\"1.0\"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>%pd</int></value></member><member><name>faultString</name><value><string>%s:%s in %s on line %d</string></value></member></struct></value></fault></methodResponse>", PG(xmlrpc_error_number), error_type_str, buffer, error_filename, error_lineno);
		} else {
			char *prepend_string = INI_STR("error_prepend_string");
			char *append_string = INI_STR("error_append_string");

			if (PG(html_errors)) {
				if (type == E_ERROR || type == E_PARSE) {
					zend_string *buf = php_escape_html_entities((unsigned char*)buffer, strlen(buffer), 0, ENT_COMPAT, NULL);
					php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buf->val, error_filename, error_lineno, STR_PRINT(append_string));
					zend_string_free(buf);
				} else {
					php_printf("%s<br />\n<b>%s</b>:  %s in <b>%s</b> on line <b>%d</b><br />\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
				}
			} else {
				/* Write CLI/CGI errors to stderr if display_errors = "stderr" */
				if ((!strcmp(sapi_module.name, "cli") || !strcmp(sapi_module.name, "cgi")) &&
					PG(display_errors) == PHP_DISPLAY_ERRORS_STDERR
				) {
#ifdef PHP_WIN32
					fprintf(stderr, "%s: %s in %s on line %u\n", error_type_str, buffer, error_filename, error_lineno);
					fflush(stderr);
#else
					fprintf(stderr, "%s: %s in %s on line %u\n", error_type_str, buffer, error_filename, error_lineno);
#endif
				} else {
					php_printf("%s\n%s: %s in %s on line %d\n%s", STR_PRINT(prepend_string), error_type_str, buffer, error_filename, error_lineno, STR_PRINT(append_string));
				}
			}
		}
	}
#if ZEND_DEBUG
	if (PG(report_zend_debug)) {
		zend_bool trigger_break;

		switch (type) {
			case E_ERROR:
			case E_CORE_ERROR:
			case E_COMPILE_ERROR:
			case E_USER_ERROR:
				trigger_break=1;
				break;
			default:
				trigger_break=0;
				break;
		}
		zend_output_debug_string(trigger_break, "%s(%d) : %s - %s", error_filename, error_lineno, error_type_str, buffer);
	}
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ zend_error_log_cb */
ZEND_ERROR_CB_API zend_error_log_cb(ZEND_ERROR_CB_HOOK_ARGS)
{
	char *buffer = PG(last_error_message);

	if (!php_get_module_initialized() || PG(log_errors)) {
		char *log_buffer;
#ifdef PHP_WIN32
		if (type == E_CORE_ERROR || type == E_CORE_WARNING) {
			syslog(LOG_ALERT, "PHP %s: %s (%s)", error_type_str, buffer, GetCommandLine());
		}
#endif
		spprintf(&log_buffer, 0, "PHP %s:  %s in %s on line %d", error_type_str, buffer, error_filename, error_lineno);
		php_log_err(log_buffer);
		efree(log_buffer);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ zend_error_bailout_cb */
ZEND_ERROR_CB_API zend_error_bailout_cb(ZEND_ERROR_CB_HOOK_ARGS)
{
	/* Bail out if we can't recover */
	switch (type) {
		case E_CORE_ERROR:
			if(!php_get_module_initialized()) {
				/* bad error in module startup - no way we can live with this */
				exit(-2);
			}
		/* no break - intentionally */
		case E_ERROR:
		case E_RECOVERABLE_ERROR:
		case E_PARSE:
		case E_COMPILE_ERROR:
		case E_USER_ERROR:
			EG(exit_status) = 255;
			if (php_get_module_initialized()) {
				if (!PG(display_errors) &&
				    !SG(headers_sent) &&
					SG(sapi_headers).http_response_code == 200
				) {
					sapi_header_line ctr = {0};

					ctr.line = "HTTP/1.0 500 Internal Server Error";
					ctr.line_len = sizeof("HTTP/1.0 500 Internal Server Error") - 1;
					sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
				}
				/* the parser would return 1 (failure), we can bail out nicely */
				if (type == E_PARSE) {
					CG(parse_error) = 0;
				} else {
					/* restore memory limit */
					zend_set_memory_limit(PG(memory_limit));
					zend_objects_store_mark_destructed(&EG(objects_store));
					zend_bailout();
					return FAILURE;
				}
			}
	}
	return SUCCESS;
}
/* }}} */

/* {{{ zend_append_error_hook */
ZEND_API void zend_append_error_hook(zend_error_cb_hook_t hook_part, int (*hook)(ZEND_ERROR_CB_HOOK_ARGS))
{
	zend_llist_add_element(&PG(error_hooks)[hook_part], (void *)&hook);
}
/* }}} */

/* {{{ zend_prepend_error_hook */
ZEND_API void zend_prepend_error_hook(zend_error_cb_hook_t hook_part, int (*hook)(ZEND_ERROR_CB_HOOK_ARGS))
{
	zend_llist_prepend_element(&PG(error_hooks)[hook_part], (void *)&hook);
}
/* }}} */

/* {{{ zend_clear_error_hook */
ZEND_API void zend_clear_error_hook(zend_error_cb_hook_t hook_part)
{
	zend_llist_clean(&PG(error_hooks)[hook_part]);
}
/* }}} */

/* {{{ zend_init_error_hooks */
ZEND_API void zend_init_error_hooks(void)
{
	int i;

	for (i = 0; i < E_HOOK_LAST; ++i) {
		zend_llist_init(&PG(error_hooks)[i], sizeof(int (*)(ZEND_ERROR_CB_HOOK_ARGS)), NULL, 1);
	}
}
/* }}} */

/* {{{ zend_register_error_hooks */
ZEND_API void zend_register_error_hooks(void)
{
	zend_append_error_hook(E_HOOK_DISPLAY, &zend_error_display_cb);
	zend_append_error_hook(E_HOOK_LOG, &zend_error_log_cb);
	zend_append_error_hook(E_HOOK_BAILOUT, &zend_error_bailout_cb);
}
/* }}} */

/* {{{ zend_unregister_error_hooks */
ZEND_API void zend_unregister_error_hooks(void)
{
	int i;

	for (i = 0; i < E_HOOK_LAST; ++i) {
		zend_llist_destroy(&PG(error_hooks)[i]);
	}
}
/* }}} */
