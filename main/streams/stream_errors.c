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
   | Authors: Jakub Zelenka <bukka@php.net>                               |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_globals.h"
#include "php_streams.h"
#include "php_stream_errors.h"
#include "stream_errors_arginfo.h"
#include "zend_exceptions.h"
#include "ext/standard/file.h"

/* StreamException class entry */
static zend_class_entry *php_ce_stream_exception;

static void php_stream_error_entry_dtor(void *error)
{
	php_stream_error_entry *entry = *(php_stream_error_entry **) error;
	zend_string_release(entry->message);
	pefree(entry->wrapper_name, entry->persistent);
	pefree(entry->docref, entry->persistent);
	 // param is not currently supported for streams so cannot be persistent
	ZEND_ASSERT(!entry->persistent || entry->param == NULL);
	efree(entry->param);
	pefree(entry, entry->persistent);
}

static void php_stream_error_list_dtor(zval *item)
{
	zend_llist *list = (zend_llist *) Z_PTR_P(item);
	zend_llist_destroy(list);
	efree(list);
}

/* Context option helpers */

static int php_stream_auto_decide_error_store_mode(int error_mode)
{
	switch (error_mode) {
		case PHP_STREAM_ERROR_MODE_ERROR:
			return PHP_STREAM_ERROR_STORE_NONE;
		case PHP_STREAM_ERROR_MODE_EXCEPTION:
			return PHP_STREAM_ERROR_STORE_NON_TERM;
		case PHP_STREAM_ERROR_MODE_SILENT:
			return PHP_STREAM_ERROR_STORE_ALL;
		default:
			return PHP_STREAM_ERROR_STORE_NONE;
	}
}

static int php_stream_get_error_mode(php_stream_context *context)
{
	if (!context) {
		return PHP_STREAM_ERROR_MODE_ERROR;
	}

	zval *option = php_stream_context_get_option(context, "stream", "error_mode");
	if (option && Z_TYPE_P(option) == IS_LONG) {
		return Z_LVAL_P(option);
	}

	return PHP_STREAM_ERROR_MODE_ERROR;
}

static int php_stream_get_error_store_mode(php_stream_context *context, int error_mode)
{
	if (!context) {
		return php_stream_auto_decide_error_store_mode(error_mode);
	}

	zval *option = php_stream_context_get_option(context, "stream", "error_store");
	if (option && Z_TYPE_P(option) == IS_LONG) {
		int store_mode = Z_LVAL_P(option);

		if (store_mode == PHP_STREAM_ERROR_STORE_AUTO) {
			return php_stream_auto_decide_error_store_mode(error_mode);
		}

		return store_mode;
	}

	return php_stream_auto_decide_error_store_mode(error_mode);
}

static bool php_stream_should_store_error(int store_mode, bool terminal)
{
	switch (store_mode) {
		case PHP_STREAM_ERROR_STORE_NONE:
			return false;
		case PHP_STREAM_ERROR_STORE_NON_TERM:
			return !terminal;
		case PHP_STREAM_ERROR_STORE_TERMINAL:
			return terminal;
		case PHP_STREAM_ERROR_STORE_ALL:
			return true;
		default:
			return false;
	}
}

/* StreamException methods */

static void php_stream_throw_exception(
		const char *message, int code, const char *wrapper_name, const char *param)
{
	zval ex;

	object_init_ex(&ex, php_ce_stream_exception);

	zend_update_property_string(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("message"), message);
	zend_update_property_long(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("code"), code);
	zend_update_property_string(
			php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("wrapperName"), wrapper_name);

	if (param) {
		zend_update_property_string(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("param"), param);
	}

	zend_throw_exception_object(&ex);
}

/* Core error processing */

static void php_stream_process_error(php_stream_context *context, const char *wrapper_name,
		php_stream *stream, const char *docref, int code, const char *message, const char *param,
		int severity, bool terminal)
{
	int error_mode = php_stream_get_error_mode(context);

	/* Handle error based on error_mode */
	switch (error_mode) {
		case PHP_STREAM_ERROR_MODE_ERROR:
			if (param) {
				php_error_docref1(docref, param, severity, "%s", message);
			} else {
				php_error_docref(docref, severity, "%s", message);
			}
			break;

		case PHP_STREAM_ERROR_MODE_EXCEPTION:
			if (terminal) {
				php_stream_throw_exception(message, code, wrapper_name, param);
			}
			break;

		case PHP_STREAM_ERROR_MODE_SILENT:
			break;
	}

	/* Call user error handler if set */
	if (context) {
		zval *handler = php_stream_context_get_option(context, "stream", "error_handler");
		if (handler) {
			zend_fcall_info_cache fcc;
			char *is_callable_error = NULL;
			zval retval;
			zval args[5];

			if (!zend_is_callable_ex(handler, NULL, 0, NULL, &fcc, &is_callable_error)) {
				if (is_callable_error) {
					zend_type_error(
							"stream error handler must be a valid callback, %s", is_callable_error);
					efree(is_callable_error);
				} else {
					zend_type_error("stream error must be a valid callback");
				}
				return;
			}

			/* Arg 0: wrapper name */
			ZVAL_STRING(&args[0], wrapper_name);

			/* Arg 1: stream resource or null */
			if (stream && stream->res) {
				ZVAL_RES(&args[1], stream->res);
				GC_ADDREF(stream->res);
			} else {
				ZVAL_NULL(&args[1]);
			}

			/* Arg 2: error code */
			ZVAL_LONG(&args[2], code);

			/* Arg 3: message */
			ZVAL_STRING(&args[3], message);

			/* Arg 4: param (or null) */
			if (param) {
				ZVAL_STRING(&args[4], param);
			} else {
				ZVAL_NULL(&args[4]);
			}

			call_user_function(NULL, NULL, handler, &retval, 5, args);

			zval_ptr_dtor(&retval);
			zval_ptr_dtor(&args[0]);
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[3]);
			zval_ptr_dtor(&args[4]);
		}
	}
}

/* Helper to create error entry */
static php_stream_error_entry *php_stream_create_error_entry(zend_string *message, int code,
		const char *wrapper_name, const char *docref, char *param, int severity,
		bool terminal, bool persistent)
{
	if (persistent) {
		message = zend_string_dup(message, true);
	} else {
		zend_string_addref(message);
	}

	php_stream_error_entry *entry = pemalloc(sizeof(php_stream_error_entry), persistent);
	entry->message = message;
	entry->code = code;
	entry->wrapper_name = wrapper_name ? pestrdup(wrapper_name, persistent) : NULL;
	entry->docref = docref ? pestrdup(docref, persistent) : NULL;
	entry->param = param;
	entry->severity = severity;
	entry->terminal = terminal;
	entry->persistent = persistent;

	return entry;
}

/* Common storage function*/
static void php_stream_store_error_common(php_stream_context *context, php_stream *stream,
		zend_string *message, const char *docref, int code,
		const char *wrapper_name, char *param, int severity, bool terminal)
{
	int error_mode = php_stream_get_error_mode(context);
	int store_mode = php_stream_get_error_store_mode(context, error_mode);

	if (!php_stream_should_store_error(store_mode, terminal)) {
		efree(param);
		return;
	}

	zend_llist *list;
	bool persistent = false;
	if (stream) {
		persistent = stream->is_persistent;
		/* Store in stream's error list */
		if (!stream->error_list) {
			stream->error_list = pemalloc(sizeof(zend_llist), persistent);
			zend_llist_init(stream->error_list, sizeof(php_stream_error_entry *),
					php_stream_error_entry_dtor, persistent);
		}
		list = stream->error_list;
	} else {
		/* Store in FG(stream_errors) for wrapper errors */
		if (!FG(wrapper_stored_errors)) {
			ALLOC_HASHTABLE(FG(wrapper_stored_errors));
			zend_hash_init(FG(wrapper_stored_errors), 8, NULL, php_stream_error_list_dtor, 0);
			list = NULL;
		} else {
			list = zend_hash_str_find_ptr(
					FG(wrapper_stored_errors), wrapper_name, strlen(wrapper_name));
		}

		if (!list) {
			zend_llist new_list;
			zend_llist_init(
					&new_list, sizeof(php_stream_error_entry *), php_stream_error_entry_dtor, 0);
			list = zend_hash_str_update_mem(FG(wrapper_stored_errors), wrapper_name,
					strlen(wrapper_name), &new_list, sizeof(new_list));
		}
	}

	php_stream_error_entry *entry = php_stream_create_error_entry(
			message, code, wrapper_name, docref, param, severity, terminal, persistent);

	zend_llist_add_element(list, &entry);
}

/* Wrapper error reporting - stores in FG(wrapper_stored_errors) */
static void php_stream_wrapper_error_internal_with_name(const char *wrapper_name,
		php_stream_context *context, const char *docref, int options, int severity, bool terminal,
		int code, char *param, const char *fmt, va_list args)
{
	zend_string *message = vstrpprintf(0, fmt, args);

	php_stream_process_error(context, wrapper_name, NULL, docref, code, ZSTR_VAL(message),
			param, severity, terminal);

	php_stream_store_error_common(
			context, NULL, message, docref, code, wrapper_name, param, severity, terminal);

	zend_string_release(message);
}

static void php_stream_wrapper_error_internal(php_stream_wrapper *wrapper,
		php_stream_context *context, const char *docref, int options, int severity, bool terminal,
		int code, char *param, const char *fmt, va_list args)
{
	const char *wrapper_name = wrapper ? wrapper->wops->label : "unknown";

	php_stream_wrapper_error_internal_with_name(wrapper_name, context, docref, options, severity,
			terminal, code, param, fmt, args);
}

PHPAPI void php_stream_wrapper_error_with_name(const char *wrapper_name,
		php_stream_context *context, const char *docref, int options, int severity, bool terminal,
		int code, const char *fmt, ...)
{
	if (options & REPORT_ERRORS) {
		va_list args;
		va_start(args, fmt);
		php_stream_wrapper_error_internal_with_name(
				wrapper_name, context, docref, options, severity, terminal, code, NULL, fmt, args);
		va_end(args);
	}
}

PHPAPI void php_stream_wrapper_error(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminal, int code, const char *fmt,
		...)
{
	if (options & REPORT_ERRORS) {
		va_list args;
		va_start(args, fmt);
		php_stream_wrapper_error_internal(
				wrapper, context, docref, options, severity, terminal, code, NULL, fmt, args);
		va_end(args);
	}
}

PHPAPI void php_stream_wrapper_error_param(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminal, int code, const char *param,
		const char *fmt, ...)
{
	if (options & REPORT_ERRORS) {
		va_list args;
		va_start(args, fmt);
		char *param_copy = param ? estrdup(param): NULL;
		php_stream_wrapper_error_internal(
				wrapper, context, docref, options, severity, terminal, code, param_copy, fmt, args);
		va_end(args);
	}
}

PHPAPI void php_stream_wrapper_error_param2(php_stream_wrapper *wrapper,
		php_stream_context *context, const char *docref, int options, int severity, bool terminal,
		int code, const char *param1, const char *param2, const char *fmt, ...)
{
	if (options & REPORT_ERRORS) {
		char *combined_param;
		spprintf(&combined_param, 0, "%s,%s", param1, param2);

		va_list args;
		va_start(args, fmt);
		php_stream_wrapper_error_internal(
				wrapper, context, docref, options, severity, terminal, code, combined_param, fmt, args);
		va_end(args);
	}
}

/* Wrapper error logging - stores in FG(wrapper_logged_errors) */

static void php_stream_wrapper_log_store_error(zend_string *message, int code,
		const char *wrapper_name, const char *param, int severity, bool terminal)
{
	char *param_copy = param ? estrdup(param): NULL;
	php_stream_error_entry *entry = php_stream_create_error_entry(
			message, code, wrapper_name, NULL, param_copy, severity, terminal, false);

	if (!FG(wrapper_logged_errors)) {
		ALLOC_HASHTABLE(FG(wrapper_logged_errors));
		zend_hash_init(FG(wrapper_logged_errors), 8, NULL, php_stream_error_list_dtor, 0);
	}

	zend_llist *list = zend_hash_str_find_ptr(
			FG(wrapper_logged_errors), wrapper_name, strlen(wrapper_name));

	if (!list) {
		zend_llist new_list;
		zend_llist_init(
				&new_list, sizeof(php_stream_error_entry *), php_stream_error_entry_dtor, 0);
		list = zend_hash_str_update_mem(FG(wrapper_logged_errors), wrapper_name,
				strlen(wrapper_name), &new_list, sizeof(new_list));
	}

	zend_llist_add_element(list, &entry);
}

static void php_stream_wrapper_log_error_internal(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminal, int code,
		char *param, const char *fmt, va_list args)
{
	zend_string *message = vstrpprintf(0, fmt, args);
	const char *wrapper_name = wrapper ? wrapper->wops->label : "unknown";

	if (options & REPORT_ERRORS) {
		/* Report immediately using standard error functions */
		php_stream_wrapper_error_internal_with_name(
				wrapper_name, context, NULL, options, severity, terminal, code, param, fmt, args);
	} else {
		/* Store for later display in FG(wrapper_logged_errors) */
		php_stream_wrapper_log_store_error(
				message, code, wrapper_name, param, severity, terminal);
	}
	zend_string_release(message);
}

PHPAPI void php_stream_wrapper_log_error(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminal, int code,
		const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	php_stream_wrapper_log_error_internal(
			wrapper, context, options, severity, terminal, code, NULL, fmt, args);
	va_end(args);
}

PHPAPI void php_stream_wrapper_log_error_param(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminal, int code,
		const char *param, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char *param_copy = param ? estrdup(param): NULL;
	php_stream_wrapper_log_error_internal(
			wrapper, context, options, severity, terminal, code, param_copy, fmt, args);
	va_end(args);
}

static zend_llist *php_stream_get_wrapper_errors_list(const char *wrapper_name)
{
	if (!FG(wrapper_logged_errors)) {
		return NULL;
	} else {
		return (zend_llist *) zend_hash_str_find_ptr(
				FG(wrapper_logged_errors), wrapper_name, strlen(wrapper_name));
	}
}

void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper,
		php_stream_context *context, int code, const char *path, const char *caption)
{
	char *msg;
	char errstr[256];
	int free_msg = 0;

	if (EG(exception)) {
		/* Don't emit additional warnings if an exception has already been thrown. */
		return;
	}

	const char *wrapper_name = wrapper ? wrapper->wops->label : "unknown";
	char *tmp = estrdup(path);
	if (wrapper) {
		zend_llist *err_list = php_stream_get_wrapper_errors_list(wrapper_name);
		if (err_list) {
			size_t l = 0;
			int brlen;
			int i;
			int count = (int) zend_llist_count(err_list);
			const char *br;
			php_stream_error_entry **err_entry_p;
			zend_llist_position pos;

			if (PG(html_errors)) {
				brlen = 7;
				br = "<br />\n";
			} else {
				brlen = 1;
				br = "\n";
			}

			for (err_entry_p = zend_llist_get_first_ex(err_list, &pos), i = 0; err_entry_p;
					err_entry_p = zend_llist_get_next_ex(err_list, &pos), i++) {
				l += ZSTR_LEN((*err_entry_p)->message);
				if (i < count - 1) {
					l += brlen;
				}
			}
			msg = emalloc(l + 1);
			msg[0] = '\0';
			for (err_entry_p = zend_llist_get_first_ex(err_list, &pos), i = 0; err_entry_p;
					err_entry_p = zend_llist_get_next_ex(err_list, &pos), i++) {
				strcat(msg, ZSTR_VAL((*err_entry_p)->message));
				if (i < count - 1) {
					strcat(msg, br);
				}
			}

			free_msg = 1;
		} else {
			if (wrapper == &php_plain_files_wrapper) {
				msg = php_socket_strerror_s(errno, errstr, sizeof(errstr));
			} else {
				msg = "operation failed";
			}
		}
	} else {
		msg = "no suitable wrapper could be found";
	}

	php_strip_url_passwd(tmp);
	php_stream_wrapper_warn_param(wrapper, context, REPORT_ERRORS, code, tmp,
			"%s: %s", caption, msg);
	efree(tmp);
	if (free_msg) {
		efree(msg);
	}
}

void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper)
{
	if (wrapper && FG(wrapper_logged_errors)) {
		const char *wrapper_name = wrapper ? wrapper->wops->label : "unknown";
		zend_hash_str_del(FG(wrapper_logged_errors), wrapper_name, strlen(wrapper_name));
	}
}

/* Stream error reporting */

PHPAPI void php_stream_error(php_stream *stream, const char *docref, int severity, bool terminal,
		int code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	php_stream_wrapper *wrapper = stream->wrapper;
	const char *wrapper_name = wrapper ? wrapper->wops->label : "stream";

	php_stream_context *context = PHP_STREAM_CONTEXT(stream);

	/* Process the error - always report for stream errors */
	php_stream_process_error(context, wrapper_name, stream, docref, code, ZSTR_VAL(message), NULL,
			severity, terminal);

	/* Store error */
	php_stream_store_error_common(context, stream, message, docref, code, wrapper_name, NULL,
			severity, terminal);

	zend_string_release(message);
}

/* StreamException class and error constants registration */

PHP_MINIT_FUNCTION(stream_errors)
{
	register_stream_errors_symbols(module_number);

	php_ce_stream_exception = register_class_StreamException(zend_ce_exception);

	return SUCCESS;
}

PHP_METHOD(StreamException, getParam)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *param = zend_read_property(
			php_ce_stream_exception, Z_OBJ_P(getThis()), ZEND_STRL("param"), 1, NULL);
	RETURN_COPY(param);
}

PHP_METHOD(StreamException, getWrapperName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *wrapper_name = zend_read_property(
			php_ce_stream_exception, Z_OBJ_P(getThis()), ZEND_STRL("wrapperName"), 1, NULL);
	RETURN_COPY(wrapper_name);
}
