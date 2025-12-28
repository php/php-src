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
#include "zend_enum.h"
#include "zend_exceptions.h"
#include "ext/standard/file.h"
#include "stream_errors_arginfo.h"

/* Class entries */
static zend_class_entry *php_ce_stream_error_code;
static zend_class_entry *php_ce_stream_error_mode;
static zend_class_entry *php_ce_stream_error_store;
static zend_class_entry *php_ce_stream_error;
static zend_class_entry *php_ce_stream_exception;

/* Lookup table for error code to case name */
static const char *const php_stream_error_code_names[200] = {
#define V(uc_name, name, val) [val] = #name,
	PHP_STREAM_ERROR_CODES(V)
#undef V
};

/* Forward declarations */
static void php_stream_error_entry_free(php_stream_error_entry *entry);
static bool php_stream_error_code_in_range(zval *this_zv, int start, int end);

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
	if (!option) {
		return PHP_STREAM_ERROR_MODE_ERROR;
	}

	if (Z_TYPE_P(option) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(option), php_ce_stream_error_mode)) {
		zend_type_error("stream context option 'error_mode' must be of type StreamErrorMode");
		return PHP_STREAM_ERROR_MODE_ERROR;
	}

	/* Get enum case name */
	zend_string *case_name = Z_STR_P(zend_enum_fetch_case_name(Z_OBJ_P(option)));

	if (zend_string_equals_literal(case_name, "Error")) {
		return PHP_STREAM_ERROR_MODE_ERROR;
	} else if (zend_string_equals_literal(case_name, "Exception")) {
		return PHP_STREAM_ERROR_MODE_EXCEPTION;
	} else if (zend_string_equals_literal(case_name, "Silent")) {
		return PHP_STREAM_ERROR_MODE_SILENT;
	}

	return PHP_STREAM_ERROR_MODE_ERROR;
}

static int php_stream_get_error_store_mode(php_stream_context *context, int error_mode)
{
	if (!context) {
		return php_stream_auto_decide_error_store_mode(error_mode);
	}

	zval *option = php_stream_context_get_option(context, "stream", "error_store");
	if (!option) {
		return php_stream_auto_decide_error_store_mode(error_mode);
	}

	if (Z_TYPE_P(option) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(option), php_ce_stream_error_store)) {
		zend_type_error("stream context option 'error_store' must be of type StreamErrorStore");
		return php_stream_auto_decide_error_store_mode(error_mode);
	}

	/* Get enum case name */
	zend_string *case_name = Z_STR_P(zend_enum_fetch_case_name(Z_OBJ_P(option)));

	if (zend_string_equals_literal(case_name, "Auto")) {
		return php_stream_auto_decide_error_store_mode(error_mode);
	} else if (zend_string_equals_literal(case_name, "None")) {
		return PHP_STREAM_ERROR_STORE_NONE;
	} else if (zend_string_equals_literal(case_name, "NonTerminal")) {
		return PHP_STREAM_ERROR_STORE_NON_TERM;
	} else if (zend_string_equals_literal(case_name, "Terminal")) {
		return PHP_STREAM_ERROR_STORE_TERMINAL;
	} else if (zend_string_equals_literal(case_name, "All")) {
		return PHP_STREAM_ERROR_STORE_ALL;
	}

	return php_stream_auto_decide_error_store_mode(error_mode);
}

/* Helper functions */

static bool php_stream_has_terminating_error(php_stream_error_operation *op)
{
	php_stream_error_entry *entry = op->first_error;
	while (entry) {
		if (entry->terminating) {
			return true;
		}
		entry = entry->next;
	}
	return false;
}

static void php_stream_error_entry_free(php_stream_error_entry *entry)
{
	while (entry) {
		php_stream_error_entry *next = entry->next;
		zend_string_release(entry->message);
		efree(entry->wrapper_name);
		efree(entry->param);
		efree(entry);
		entry = next;
	}
}

static void php_stream_error_operation_free(php_stream_error_operation *op)
{
	php_stream_error_entry_free(op->first_error);
	efree(op);
}

/* Cleanup function for request shutdown */
PHPAPI void php_stream_error_state_cleanup(void)
{
	/* Clear active operations (shouldn't normally have any, but clean up just in case) */
	while (FG(stream_error_state).current_operation) {
		php_stream_error_operation *op = FG(stream_error_state).current_operation;
		FG(stream_error_state).current_operation = op->parent;
		php_stream_error_operation_free(op);
	}

	/* Clear stored errors */
	php_stream_stored_error *stored = FG(stream_error_state).stored_errors;
	while (stored) {
		php_stream_stored_error *next = stored->next;
		php_stream_error_entry_free(stored->first_error);
		efree(stored);
		stored = next;
	}

	FG(stream_error_state).stored_errors = NULL;
	FG(stream_error_state).stored_count = 0;
	FG(stream_error_state).operation_depth = 0;
}

/* Error operation stack management */

PHPAPI php_stream_error_operation *php_stream_error_operation_begin(php_stream_context *context)
{
	/* Check depth limit */
	if (FG(stream_error_state).operation_depth >= PHP_STREAM_ERROR_MAX_DEPTH) {
		php_error_docref(NULL, E_WARNING,
				"Stream error operation depth exceeded (%u), possible infinite recursion",
				FG(stream_error_state).operation_depth);
		return NULL;
	}

	/* Create new operation with empty error list */
	php_stream_error_operation *op = emalloc(sizeof(php_stream_error_operation));
	op->first_error = NULL;
	op->last_error = NULL;
	op->error_count = 0;
	op->parent = FG(stream_error_state).current_operation;

	/* Push onto stack */
	FG(stream_error_state).current_operation = op;
	FG(stream_error_state).operation_depth++;

	return op;
}

static void php_stream_error_add(StreamErrorCode code, const char *wrapper_name,
		zend_string *message, const char *docref, char *param, int severity, bool terminating)
{
	php_stream_error_operation *op = FG(stream_error_state).current_operation;
	ZEND_ASSERT(op != NULL);

	php_stream_error_entry *entry = emalloc(sizeof(php_stream_error_entry));
	entry->message = message;
	entry->code = code;
	entry->wrapper_name = wrapper_name ? estrdup(wrapper_name) : NULL;
	entry->param = param;
	entry->param = docref ? estrdup(docref) : NULL;
	entry->severity = severity;
	entry->terminating = terminating;
	entry->next = NULL;

	/* Append to operation's error list */
	if (op->last_error) {
		op->last_error->next = entry;
	} else {
		op->first_error = entry;
	}
	op->last_error = entry;
	op->error_count++;
}

/* Error reporting */

static void php_stream_call_error_handler(zval *handler, zval *error_obj)
{
	zend_fcall_info_cache fcc;
	char *is_callable_error = NULL;

	if (!zend_is_callable_ex(handler, NULL, 0, NULL, &fcc, &is_callable_error)) {
		if (is_callable_error) {
			zend_type_error("stream error handler must be a valid callback, %s", is_callable_error);
			efree(is_callable_error);
		}
		return;
	}

	zval retval;

	call_user_function(NULL, NULL, handler, &retval, 1, error_obj);

	zval_ptr_dtor(&retval);
}

static void php_stream_throw_exception_with_errors(php_stream_error_operation *op)
{
	if (!op->first_error) {
		return;
	}

	/* Create StreamError object from error chain */
	zval error_obj;
	php_stream_error_create_object(&error_obj, op->first_error);

	/* Create exception */
	zval ex;
	object_init_ex(&ex, php_ce_stream_exception);

	/* Set message from first error */
	zend_update_property_string(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("message"),
			ZSTR_VAL(op->first_error->message));

	/* Set code from first error enum value */
	zend_update_property_long(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("code"),
			(zend_long) op->first_error->code);

	/* Set the complete error object */
	zend_update_property(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("error"), &error_obj);

	zval_ptr_dtor(&error_obj);
	zend_throw_exception_object(&ex);
}

static void php_stream_report_errors(php_stream_context *context, php_stream_error_operation *op,
		int error_mode, bool is_terminating)
{
	switch (error_mode) {
		case PHP_STREAM_ERROR_MODE_ERROR: {
			/* Report all errors individually */
			php_stream_error_entry *entry = op->first_error;
			while (entry) {
				if (entry->param) {
					php_error_docref1(entry->docref, entry->param, entry->severity, "%s",
							ZSTR_VAL(entry->message));
				} else {
					php_error_docref(
							entry->docref, entry->severity, "%s", ZSTR_VAL(entry->message));
				}
				entry = entry->next;
			}
			break;
		}

		case PHP_STREAM_ERROR_MODE_EXCEPTION: {
			if (is_terminating) {
				php_stream_throw_exception_with_errors(op);
			}
			break;
		}

		case PHP_STREAM_ERROR_MODE_SILENT:
			/* Don't report */
			break;
	}

	/* Call user error handler if set */
	zval *handler
			= context ? php_stream_context_get_option(context, "stream", "error_handler") : NULL;

	if (handler) {
		/* Create StreamError object from error chain */
		zval error_obj;
		php_stream_error_create_object(&error_obj, op->first_error);

		/* Call handler(error) */
		php_stream_call_error_handler(handler, &error_obj);

		zval_ptr_dtor(&error_obj);
	}
}

/* Error storage - move and filter */

PHPAPI void php_stream_error_operation_end(php_stream_context *context)
{
	php_stream_error_operation *op = FG(stream_error_state).current_operation;

	if (!op) {
		return;
	}

	/* Pop from stack */
	FG(stream_error_state).current_operation = op->parent;
	FG(stream_error_state).operation_depth--;

	/* Process errors if we have any */
	if (op->error_count > 0) {
		/* Get error handling settings */
		int error_mode = php_stream_get_error_mode(context);
		int store_mode = php_stream_get_error_store_mode(context, error_mode);

		bool is_terminating = php_stream_has_terminating_error(op);

		/* Always report errors */
		php_stream_report_errors(context, op, error_mode, is_terminating);

		/* Handle storage */
		if (store_mode == PHP_STREAM_ERROR_STORE_NONE) {
			/* Free all errors */
			php_stream_error_entry_free(op->first_error);
			op->first_error = NULL;
			op->last_error = NULL;
		} else {
			/* Filter and store */
			php_stream_error_entry *entry = op->first_error;
			php_stream_error_entry *prev = NULL;
			php_stream_error_entry *to_store_first = NULL;
			php_stream_error_entry *to_store_last = NULL;

			while (entry) {
				php_stream_error_entry *next = entry->next;
				bool should_store = false;

				if (store_mode == PHP_STREAM_ERROR_STORE_ALL) {
					should_store = true;
				} else if (store_mode == PHP_STREAM_ERROR_STORE_NON_TERM && !entry->terminating) {
					should_store = true;
				} else if (store_mode == PHP_STREAM_ERROR_STORE_TERMINAL && entry->terminating) {
					should_store = true;
				}

				if (should_store) {
					/* Move to storage chain */
					if (prev) {
						prev->next = next;
					} else {
						op->first_error = next;
					}

					entry->next = NULL;
					if (to_store_last) {
						to_store_last->next = entry;
					} else {
						to_store_first = entry;
					}
					to_store_last = entry;
				} else {
					/* Free this error */
					if (prev) {
						prev->next = next;
					} else {
						op->first_error = next;
					}

					zend_string_release(entry->message);
					efree(entry->wrapper_name);
					efree(entry->param);
					efree(entry);

					/* Don't update prev */
					entry = next;
					continue;
				}

				prev = entry;
				entry = next;
			}

			/* Store filtered errors if any */
			if (to_store_first) {
				php_stream_stored_error *stored = emalloc(sizeof(php_stream_stored_error));
				stored->first_error = to_store_first;
				stored->next = FG(stream_error_state).stored_errors;

				FG(stream_error_state).stored_errors = stored;
				FG(stream_error_state).stored_count++;
			}

			/* Free any remaining errors not moved to storage */
			php_stream_error_entry_free(op->first_error);
			op->first_error = NULL;
			op->last_error = NULL;
		}
	}

	/* Free operation structure */
	efree(op);
}

PHPAPI void php_stream_error_operation_abort(void)
{
	php_stream_error_operation *op = FG(stream_error_state).current_operation;

	if (!op) {
		return;
	}

	FG(stream_error_state).current_operation = op->parent;
	FG(stream_error_state).operation_depth--;

	php_stream_error_operation_free(op);
}

/* Wrapper error reporting */

static void php_stream_wrapper_error_internal(const char *wrapper_name, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating, int code, char *param,
		zend_string *message)
{
	/* If not in an operation, create one */
	bool implicit_operation = (FG(stream_error_state).current_operation == NULL);
	if (implicit_operation) {
		php_stream_error_operation_begin(context);
	}

	/* Add to current operation (or skip if no operation) */
	php_stream_error_add(code, wrapper_name, message, docref, param, severity, terminating);

	/* If we created implicit operation, end it immediately */
	if (implicit_operation) {
		php_stream_error_operation_end(context);
	}
}

PHPAPI void php_stream_wrapper_error_with_name(const char *wrapper_name,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, int code, const char *fmt, ...)
{
	if (!(options & REPORT_ERRORS)) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	php_stream_wrapper_error_internal(
			wrapper_name, context, docref, options, severity, terminating, code, NULL, message);
}

PHPAPI void php_stream_wrapper_error(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating, int code, const char *fmt,
		...)
{
	if (!(options & REPORT_ERRORS)) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);

	php_stream_wrapper_error_internal(
			wrapper_name, context, docref, options, severity, terminating, code, NULL, message);
}

PHPAPI void php_stream_wrapper_error_param(php_stream_wrapper *wrapper, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating, int code,
		const char *param, const char *fmt, ...)
{
	if (!(options & REPORT_ERRORS)) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);
	char *param_copy = param ? estrdup(param) : NULL;

	php_stream_wrapper_error_internal(wrapper_name, context, docref, options, severity, terminating,
			code, param_copy, message);
}

PHPAPI void php_stream_wrapper_error_param2(php_stream_wrapper *wrapper,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, int code, const char *param1, const char *param2, const char *fmt, ...)
{
	if (!(options & REPORT_ERRORS)) {
		return;
	}

	char *combined_param;
	spprintf(&combined_param, 0, "%s,%s", param1, param2);

	va_list args;
	va_start(args, fmt);
	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);

	php_stream_wrapper_error_internal(wrapper_name, context, docref, options, severity, terminating,
			code, combined_param, message);
}

/* Stream error reporting - delegates to wrapper errors */

PHPAPI void php_stream_error(php_stream *stream, const char *docref, int severity, bool terminating,
		int code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	const char *wrapper_name = stream->wrapper ? stream->wrapper->wops->label : "stream";

	php_stream_context *context = PHP_STREAM_CONTEXT(stream);

	/* Just call the wrapper error function */
	php_stream_wrapper_error_internal(wrapper_name, context, docref, REPORT_ERRORS, severity,
			terminating, code, NULL, message);
}

/* Legacy wrapper error logging */

static void php_stream_error_entry_dtor_legacy(void *error)
{
	php_stream_error_entry *entry = *(php_stream_error_entry **) error;
	zend_string_release(entry->message);
	efree(entry->wrapper_name);
	efree(entry->param);
	efree(entry->docref);
	efree(entry);
}

static void php_stream_error_list_dtor(zval *item)
{
	zend_llist *list = (zend_llist *) Z_PTR_P(item);
	zend_llist_destroy(list);
	efree(list);
}

static void php_stream_wrapper_log_store_error(zend_string *message, int code,
		const char *wrapper_name, const char *param, int severity, bool terminating)
{
	char *param_copy = param ? estrdup(param) : NULL;

	php_stream_error_entry *entry = emalloc(sizeof(php_stream_error_entry));
	entry->message = message;
	entry->code = code;
	entry->wrapper_name = wrapper_name ? estrdup(wrapper_name) : NULL;
	entry->param = param_copy;
	entry->severity = severity;
	entry->terminating = terminating;
	entry->next = NULL;

	if (!FG(wrapper_logged_errors)) {
		ALLOC_HASHTABLE(FG(wrapper_logged_errors));
		zend_hash_init(FG(wrapper_logged_errors), 8, NULL, php_stream_error_list_dtor, 0);
	}

	zend_llist *list
			= zend_hash_str_find_ptr(FG(wrapper_logged_errors), wrapper_name, strlen(wrapper_name));

	if (!list) {
		zend_llist new_list;
		zend_llist_init(
				&new_list, sizeof(php_stream_error_entry *), php_stream_error_entry_dtor_legacy, 0);
		list = zend_hash_str_update_mem(FG(wrapper_logged_errors), wrapper_name,
				strlen(wrapper_name), &new_list, sizeof(new_list));
	}

	zend_llist_add_element(list, &entry);
}

static void php_stream_wrapper_log_error_internal(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating, int code,
		char *param, const char *fmt, va_list args)
{
	zend_string *message = vstrpprintf(0, fmt, args);
	const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);

	if (options & REPORT_ERRORS) {
		/* Report immediately */
		php_stream_wrapper_error_internal(
				wrapper_name, context, NULL, options, severity, terminating, code, param, message);
	} else {
		/* Store for later display */
		php_stream_wrapper_log_store_error(
				message, code, wrapper_name, param, severity, terminating);
	}
}

PHPAPI void php_stream_wrapper_log_error(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating, int code,
		const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	php_stream_wrapper_log_error_internal(
			wrapper, context, options, severity, terminating, code, NULL, fmt, args);
	va_end(args);
}

PHPAPI void php_stream_wrapper_log_error_param(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating, int code,
		const char *param, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char *param_copy = param ? estrdup(param) : NULL;
	php_stream_wrapper_log_error_internal(
			wrapper, context, options, severity, terminating, code, param_copy, fmt, args);
	va_end(args);
}

static zend_llist *php_stream_get_wrapper_errors_list(const char *wrapper_name)
{
	if (!FG(wrapper_logged_errors)) {
		return NULL;
	}
	return (zend_llist *) zend_hash_str_find_ptr(
			FG(wrapper_logged_errors), wrapper_name, strlen(wrapper_name));
}

PHPAPI void php_stream_display_wrapper_name_errors(const char *wrapper_name,
		php_stream_context *context, int code, const char *path, const char *caption)
{
	char *msg;
	char errstr[256];
	int free_msg = 0;

	if (EG(exception)) {
		return;
	}

	char *tmp = estrdup(path);
	if (strcmp(wrapper_name, PHP_STREAM_ERROR_WRAPPER_DEFAULT_NAME)) {
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
			if (!strcmp(wrapper_name, php_plain_files_wrapper.wops->label)) {
				msg = php_socket_strerror_s(errno, errstr, sizeof(errstr));
			} else {
				msg = "operation failed";
			}
		}
	} else {
		msg = "no suitable wrapper could be found";
	}

	php_strip_url_passwd(tmp);

	zend_string *message = strpprintf(0, "%s: %s", caption, msg);

	php_stream_wrapper_error_internal(
			wrapper_name, context, NULL, REPORT_ERRORS, E_WARNING, true, code, tmp, message);

	if (free_msg) {
		efree(msg);
	}
}

PHPAPI void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper,
		php_stream_context *context, int code, const char *path, const char *caption)
{
	if (wrapper) {
		const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);
		php_stream_display_wrapper_name_errors(wrapper_name, context, code, path, caption);
	}
}

PHPAPI void php_stream_tidy_wrapper_name_error_log(const char *wrapper_name)
{
	if (FG(wrapper_logged_errors)) {
		zend_hash_str_del(FG(wrapper_logged_errors), wrapper_name, strlen(wrapper_name));
	}
}

PHPAPI void php_stream_tidy_wrapper_error_log(php_stream_wrapper *wrapper)
{
	if (wrapper) {
		const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);
		php_stream_tidy_wrapper_name_error_log(wrapper_name);
	}
}

/* StreamError object creation - no enum cache */

PHPAPI void php_stream_error_create_object(zval *zv, php_stream_error_entry *entry)
{
	if (!entry) {
		ZVAL_NULL(zv);
		return;
	}

	object_init_ex(zv, php_ce_stream_error);

	/* Get enum case by value using lookup array */
	const char *case_name = NULL;
	int code_value = (int) entry->code;

	if (code_value >= 0
			&& code_value < (int) (sizeof(php_stream_error_code_names)
					   / sizeof(php_stream_error_code_names[0]))) {
		case_name = php_stream_error_code_names[code_value];
	}

	if (!case_name) {
		case_name = "Generic";
	}

	zend_object *enum_obj = zend_enum_get_case_cstr(php_ce_stream_error_code, case_name);
	ZEND_ASSERT(enum_obj != NULL);

	zval code_enum;
	ZVAL_OBJ(&code_enum, enum_obj);
	GC_ADDREF(enum_obj);

	zend_update_property(php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("code"), &code_enum);
	zval_ptr_dtor(&code_enum);

	/* Set other properties */
	zend_update_property_str(
			php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("message"), entry->message);

	zend_update_property_string(php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("wrapperName"),
			entry->wrapper_name ? entry->wrapper_name : "");

	zend_update_property_long(
			php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("severity"), entry->severity);

	zend_update_property_bool(
			php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("terminating"), entry->terminating);

	if (entry->param) {
		zend_update_property_string(
				php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("param"), entry->param);
	} else {
		zend_update_property_null(php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("param"));
	}

	if (entry->next) {
		zval next_error;
		php_stream_error_create_object(&next_error, entry->next);
		zend_update_property(php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("next"), &next_error);
		zval_ptr_dtor(&next_error);
	} else {
		zend_update_property_null(php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("next"));
	}
}

/* StreamError methods */

PHP_METHOD(StreamError, hasCode)
{
	zval *search_code;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_OBJECT_OF_CLASS(search_code, php_ce_stream_error_code)
	ZEND_PARSE_PARAMETERS_END();

	zval *current_error_zv = ZEND_THIS;

	while (Z_TYPE_P(current_error_zv) == IS_OBJECT) {
		zval *code_zv = zend_read_property(
				php_ce_stream_error, Z_OBJ_P(current_error_zv), ZEND_STRL("code"), 1, NULL);

		/* Compare enum objects */
		if (Z_TYPE_P(code_zv) == IS_OBJECT && Z_OBJ_P(code_zv) == Z_OBJ_P(search_code)) {
			RETURN_TRUE;
		}

		/* Move to next error */
		current_error_zv = zend_read_property(
				php_ce_stream_error, Z_OBJ_P(current_error_zv), ZEND_STRL("next"), 1, NULL);

		if (Z_TYPE_P(current_error_zv) != IS_OBJECT) {
			break;
		}
	}

	RETURN_FALSE;
}

PHP_METHOD(StreamError, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_long count = 1;
	zval *current_error_zv = ZEND_THIS;

	while (1) {
		current_error_zv = zend_read_property(
				php_ce_stream_error, Z_OBJ_P(current_error_zv), ZEND_STRL("next"), 1, NULL);

		if (Z_TYPE_P(current_error_zv) != IS_OBJECT) {
			break;
		}

		count++;
	}

	RETURN_LONG(count);
}

/* StreamException methods */

PHP_METHOD(StreamException, getError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *error = zend_read_property(
			php_ce_stream_exception, Z_OBJ_P(ZEND_THIS), ZEND_STRL("error"), 1, NULL);

	RETURN_COPY(error);
}

/* StreamErrorCode helper */

static bool php_stream_error_code_in_range(zval *this_zv, int start, int end)
{
	zval *backing = zend_enum_fetch_case_value(Z_OBJ_P(this_zv));
	if (!backing || Z_TYPE_P(backing) != IS_LONG) {
		return false;
	}

	zend_long value = Z_LVAL_P(backing);
	return value >= start && value < end;
}

/* StreamErrorCode methods */

PHP_METHOD(StreamErrorCode, isIoError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_IO_START, STREAM_ERROR_CODE_IO_END));
}

PHP_METHOD(StreamErrorCode, isFileSystemError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_FILESYSTEM_START, STREAM_ERROR_CODE_FILESYSTEM_END));
}

PHP_METHOD(StreamErrorCode, isWrapperError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_WRAPPER_START, STREAM_ERROR_CODE_WRAPPER_END));
}

PHP_METHOD(StreamErrorCode, isFilterError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_FILTER_START, STREAM_ERROR_CODE_FILTER_END));
}

PHP_METHOD(StreamErrorCode, isCastError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_CAST_START, STREAM_ERROR_CODE_CAST_END));
}

PHP_METHOD(StreamErrorCode, isNetworkError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_NETWORK_START, STREAM_ERROR_CODE_NETWORK_END));
}

PHP_METHOD(StreamErrorCode, isEncodingError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_ENCODING_START, STREAM_ERROR_CODE_ENCODING_END));
}

PHP_METHOD(StreamErrorCode, isResourceError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_RESOURCE_START, STREAM_ERROR_CODE_RESOURCE_END));
}

PHP_METHOD(StreamErrorCode, isLockError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_LOCK_START, STREAM_ERROR_CODE_LOCK_END));
}

PHP_METHOD(StreamErrorCode, isUserspaceError)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(php_stream_error_code_in_range(
			ZEND_THIS, STREAM_ERROR_CODE_USERSPACE_START, STREAM_ERROR_CODE_USERSPACE_END));
}

/* Module init */

PHP_MINIT_FUNCTION(stream_errors)
{
	/* Register enums */
	php_ce_stream_error_code = register_class_StreamErrorCode();
	php_ce_stream_error_mode = register_class_StreamErrorMode();
	php_ce_stream_error_store = register_class_StreamErrorStore();

	/* Add cases to StreamErrorCode */
#define V(uc_name, name, val) \
	{ \
		zval enum_case_value; \
		ZVAL_LONG(&enum_case_value, val); \
		zend_enum_add_case_cstr(php_ce_stream_error_code, #name, &enum_case_value); \
	}
	PHP_STREAM_ERROR_CODES(V)
#undef V

	/* Register classes */
	php_ce_stream_error = register_class_StreamError();
	php_ce_stream_exception = register_class_StreamException(zend_ce_exception);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(stream_errors)
{
	return SUCCESS;
}
