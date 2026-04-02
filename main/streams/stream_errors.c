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

#define ZEND_ENUM_StreamErrorCode_USE_NAME_TABLE
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

/* Forward declarations */
static void php_stream_error_entry_free(php_stream_error_entry *entry);

/* Helper to create a single StreamError object from an entry */
static void php_stream_error_create_object(zval *zv, php_stream_error_entry *entry)
{
	object_init_ex(zv, php_ce_stream_error);

	const char *case_name = NULL;
	if (entry->code > 0 && entry->code <= ZEND_ENUM_StreamErrorCode_CASE_COUNT) {
		case_name = zend_enum_StreamErrorCode_case_names[entry->code];
	}
	if (!case_name) {
		case_name = "Generic";
	}

	zend_object *enum_obj = zend_enum_get_case_cstr(php_ce_stream_error_code, case_name);
	ZEND_ASSERT(enum_obj != NULL);

	zval code_enum;
	ZVAL_OBJ_COPY(&code_enum, enum_obj);

	zend_update_property(php_ce_stream_error, Z_OBJ_P(zv), ZEND_STRL("code"), &code_enum);
	zval_ptr_dtor(&code_enum);

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
}

/* Create array of StreamError objects from error chain */
PHPAPI void php_stream_error_create_array(zval *zv, php_stream_error_entry *first)
{
	array_init(zv);

	php_stream_error_entry *entry = first;
	while (entry) {
		zval error_obj;
		php_stream_error_create_object(&error_obj, entry);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(zv), &error_obj);
		entry = entry->next;
	}
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
	if (!option) {
		return PHP_STREAM_ERROR_MODE_ERROR;
	}

	if (Z_TYPE_P(option) != IS_OBJECT
			|| !instanceof_function(Z_OBJCE_P(option), php_ce_stream_error_mode)) {
		zend_type_error("stream context option 'error_mode' must be of type StreamErrorMode");
		return PHP_STREAM_ERROR_MODE_ERROR;
	}

	switch ((zend_enum_StreamErrorMode) zend_enum_fetch_case_id(Z_OBJ_P(option))) {
		case ZEND_ENUM_StreamErrorMode_Error:
			return PHP_STREAM_ERROR_MODE_ERROR;
		case ZEND_ENUM_StreamErrorMode_Exception:
			return PHP_STREAM_ERROR_MODE_EXCEPTION;
		case ZEND_ENUM_StreamErrorMode_Silent:
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

	switch ((zend_enum_StreamErrorStore) zend_enum_fetch_case_id(Z_OBJ_P(option))) {
		case ZEND_ENUM_StreamErrorStore_Auto:
			return php_stream_auto_decide_error_store_mode(error_mode);
		case ZEND_ENUM_StreamErrorStore_None:
			return PHP_STREAM_ERROR_STORE_NONE;
		case ZEND_ENUM_StreamErrorStore_NonTerminating:
			return PHP_STREAM_ERROR_STORE_NON_TERM;
		case ZEND_ENUM_StreamErrorStore_Terminating:
			return PHP_STREAM_ERROR_STORE_TERMINAL;
		case ZEND_ENUM_StreamErrorStore_All:
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

static inline php_stream_error_operation *php_stream_get_operation_at_depth(uint32_t depth)
{
	php_stream_error_state *state = &FG(stream_error_state);

	if (depth < PHP_STREAM_ERROR_OPERATION_POOL_SIZE) {
		return &state->operation_pool[depth];
	} else {
		uint32_t overflow_index = depth - PHP_STREAM_ERROR_OPERATION_POOL_SIZE;
		ZEND_ASSERT(overflow_index < state->overflow_capacity);
		return &state->overflow_operations[overflow_index];
	}
}

static inline php_stream_error_operation *php_stream_get_parent_operation(void)
{
	php_stream_error_state *state = &FG(stream_error_state);

	if (state->operation_depth <= 1) {
		return NULL;
	}

	return php_stream_get_operation_at_depth(state->operation_depth - 2);
}

/* Clean up functions */

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

PHPAPI void php_stream_error_state_cleanup(void)
{
	php_stream_error_state *state = &FG(stream_error_state);

	while (state->current_operation) {
		php_stream_error_operation *op = state->current_operation;
		state->operation_depth--;
		state->current_operation = php_stream_get_parent_operation();

		php_stream_error_entry_free(op->first_error);

		op->first_error = NULL;
		op->last_error = NULL;
		op->error_count = 0;
	}

	php_stream_stored_error *stored = state->stored_errors;
	while (stored) {
		php_stream_stored_error *next = stored->next;
		php_stream_error_entry_free(stored->first_error);
		efree(stored);
		stored = next;
	}

	state->stored_errors = NULL;
	state->stored_count = 0;
	state->operation_depth = 0;

	if (state->overflow_operations) {
		efree(state->overflow_operations);
		state->overflow_operations = NULL;
		state->overflow_capacity = 0;
	}
}

PHPAPI void php_stream_error_get_last(zval *return_value)
{
	php_stream_error_state *state = &FG(stream_error_state);

	if (!state->stored_errors) {
		ZVAL_EMPTY_ARRAY(return_value);
		return;
	}

	php_stream_error_create_array(return_value, state->stored_errors->first_error);
}

PHPAPI void php_stream_error_clear_stored(void)
{
	php_stream_error_state *state = &FG(stream_error_state);

	php_stream_stored_error *stored = state->stored_errors;
	while (stored) {
		php_stream_stored_error *next = stored->next;
		php_stream_error_entry_free(stored->first_error);
		efree(stored);
		stored = next;
	}

	state->stored_errors = NULL;
	state->stored_count = 0;
}

/* Error operation stack management */

PHPAPI php_stream_error_operation *php_stream_error_operation_begin(void)
{
	php_stream_error_state *state = &FG(stream_error_state);

	if (state->operation_depth >= PHP_STREAM_ERROR_MAX_DEPTH) {
		php_error_docref(NULL, E_WARNING,
				"Stream error operation depth exceeded (%u), possible infinite recursion",
				state->operation_depth);
		return NULL;
	}

	php_stream_error_operation *op;

	if (state->operation_depth < PHP_STREAM_ERROR_OPERATION_POOL_SIZE) {
		op = &state->operation_pool[state->operation_depth];
	} else {
		uint32_t overflow_index = state->operation_depth - PHP_STREAM_ERROR_OPERATION_POOL_SIZE;

		if (overflow_index >= state->overflow_capacity) {
			uint32_t new_capacity
					= state->overflow_capacity == 0 ? 8 : state->overflow_capacity * 2;
			php_stream_error_operation *new_overflow = erealloc(
					state->overflow_operations, sizeof(php_stream_error_operation) * new_capacity);
			state->overflow_operations = new_overflow;
			state->overflow_capacity = new_capacity;
		}

		op = &state->overflow_operations[overflow_index];
	}

	op->first_error = NULL;
	op->last_error = NULL;
	op->error_count = 0;

	state->current_operation = op;
	state->operation_depth++;

	return op;
}

static void php_stream_error_add(zend_enum_StreamErrorCode code, const char *wrapper_name,
		zend_string *message, const char *docref, char *param, int severity, bool terminating)
{
	php_stream_error_operation *op = FG(stream_error_state).current_operation;
	ZEND_ASSERT(op != NULL);

	php_stream_error_entry *entry = emalloc(sizeof(php_stream_error_entry));
	entry->message = message;
	entry->code = code;
	entry->wrapper_name = wrapper_name ? estrdup(wrapper_name) : NULL;
	entry->param = param;
	entry->docref = docref ? estrdup(docref) : NULL;
	entry->severity = severity;
	entry->terminating = terminating;
	entry->next = NULL;

	if (op->last_error) {
		op->last_error->next = entry;
	} else {
		op->first_error = entry;
	}
	op->last_error = entry;
	op->error_count++;
}

/* Error reporting */

static void php_stream_call_error_handler(zval *handler, zval *errors_array)
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

	call_user_function(NULL, NULL, handler, &retval, 1, errors_array);

	zval_ptr_dtor(&retval);
}

static void php_stream_throw_exception_with_errors(php_stream_error_operation *op)
{
	if (!op->first_error) {
		return;
	}

	zval ex;
	object_init_ex(&ex, php_ce_stream_exception);

	/* Set message from first error */
	zend_update_property_string(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("message"),
			ZSTR_VAL(op->first_error->message));

	/* Set code from first error */
	zend_update_property_long(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("code"),
			(zend_long) op->first_error->code);

	/* Build errors array and set it */
	zval errors_array;
	php_stream_error_create_array(&errors_array, op->first_error);
	zend_update_property(php_ce_stream_exception, Z_OBJ(ex), ZEND_STRL("errors"), &errors_array);
	zval_ptr_dtor(&errors_array);

	zend_throw_exception_object(&ex);
}

static void php_stream_report_errors(php_stream_context *context, php_stream_error_operation *op,
		int error_mode, bool is_terminating)
{
	switch (error_mode) {
		case PHP_STREAM_ERROR_MODE_ERROR: {
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
			break;
	}

	/* Call user error handler if set */
	zval *handler
			= context ? php_stream_context_get_option(context, "stream", "error_handler") : NULL;

	if (handler) {
		zval errors_array;
		php_stream_error_create_array(&errors_array, op->first_error);

		php_stream_call_error_handler(handler, &errors_array);

		zval_ptr_dtor(&errors_array);
	}
}

/* Error storage */

PHPAPI void php_stream_error_operation_end(php_stream_context *context)
{
	php_stream_error_state *state = &FG(stream_error_state);
	php_stream_error_operation *op = state->current_operation;

	if (!op) {
		return;
	}

	state->operation_depth--;
	state->current_operation = php_stream_get_parent_operation();

	if (op->error_count > 0) {
		if (context == NULL) {
			context = FG(default_context);
		}

		int error_mode = php_stream_get_error_mode(context);
		int store_mode = php_stream_get_error_store_mode(context, error_mode);

		bool is_terminating = php_stream_has_terminating_error(op);

		php_stream_report_errors(context, op, error_mode, is_terminating);

		if (store_mode == PHP_STREAM_ERROR_STORE_NONE) {
			php_stream_error_entry_free(op->first_error);
			op->first_error = NULL;
		} else {
			php_stream_error_entry *entry = op->first_error;
			php_stream_error_entry *prev = NULL;
			php_stream_error_entry *to_store_first = NULL;
			php_stream_error_entry *to_store_last = NULL;
			uint32_t to_store_count = 0;
			php_stream_error_entry *remaining_first = NULL;

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
					entry->next = NULL;
					if (to_store_last) {
						to_store_last->next = entry;
					} else {
						to_store_first = entry;
					}
					to_store_last = entry;
					to_store_count++;
				} else {
					entry->next = NULL;
					if (prev) {
						prev->next = entry;
					} else {
						remaining_first = entry;
					}
					prev = entry;
				}

				entry = next;
			}

			if (to_store_first) {
				php_stream_stored_error *stored = emalloc(sizeof(php_stream_stored_error));
				stored->first_error = to_store_first;
				stored->error_count = to_store_count;
				stored->next = state->stored_errors;

				state->stored_errors = stored;
				state->stored_count++;
			}

			if (remaining_first) {
				php_stream_error_entry_free(remaining_first);
			}

			op->first_error = NULL;
		}
	}

	op->first_error = NULL;
	op->last_error = NULL;
	op->error_count = 0;
}

PHPAPI void php_stream_error_operation_end_for_stream(php_stream *stream)
{
	php_stream_error_state *state = &FG(stream_error_state);
	php_stream_error_operation *op = state->current_operation;

	if (!op) {
		return;
	}

	if (op->error_count == 0) {
		state->operation_depth--;
		state->current_operation = php_stream_get_parent_operation();

		op->first_error = NULL;
		op->last_error = NULL;
		return;
	}

	php_stream_context *context = PHP_STREAM_CONTEXT(stream);
	php_stream_error_operation_end(context);
}

PHPAPI void php_stream_error_operation_abort(void)
{
	php_stream_error_state *state = &FG(stream_error_state);
	php_stream_error_operation *op = state->current_operation;

	if (!op) {
		return;
	}

	state->operation_depth--;
	state->current_operation = php_stream_get_parent_operation();

	php_stream_error_entry_free(op->first_error);
	op->first_error = NULL;
	op->last_error = NULL;
	op->error_count = 0;
}

/* Wrapper error reporting */

static void php_stream_wrapper_error_internal(const char *wrapper_name, php_stream_context *context,
		const char *docref, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, char *param, zend_string *message)
{
	bool implicit_operation = (FG(stream_error_state).current_operation == NULL);
	if (implicit_operation) {
		php_stream_error_operation_begin();
	}

	php_stream_error_add(code, wrapper_name, message, docref, param, severity, terminating);

	if (implicit_operation) {
		php_stream_error_operation_end(context);
	}
}

PHPAPI void php_stream_wrapper_error_with_name(const char *wrapper_name,
		php_stream_context *context, const char *docref, int options, int severity,
		bool terminating, zend_enum_StreamErrorCode code, const char *fmt, ...)
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
		const char *docref, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *fmt, ...)
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
		const char *docref, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *param, const char *fmt, ...)
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
		bool terminating, zend_enum_StreamErrorCode code, const char *param1, const char *param2,
		const char *fmt, ...)
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

/* Stream error reporting */

PHPAPI void php_stream_error(php_stream *stream, const char *docref, int severity,
		bool terminating, zend_enum_StreamErrorCode code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	zend_string *message = vstrpprintf(0, fmt, args);
	va_end(args);

	const char *wrapper_name = stream->wrapper ? stream->wrapper->wops->label : "stream";

	php_stream_context *context = PHP_STREAM_CONTEXT(stream);

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

static void php_stream_wrapper_log_store_error(zend_string *message, zend_enum_StreamErrorCode code,
		const char *wrapper_name, const char *param, int severity, bool terminating)
{
	char *param_copy = param ? estrdup(param) : NULL;

	php_stream_error_entry *entry = ecalloc(1, sizeof(php_stream_error_entry));
	entry->message = message;
	entry->code = code;
	entry->wrapper_name = wrapper_name ? estrdup(wrapper_name) : NULL;
	entry->param = param_copy;
	entry->severity = severity;
	entry->terminating = terminating;

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
		php_stream_context *context, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, char *param, const char *fmt, va_list args)
{
	zend_string *message = vstrpprintf(0, fmt, args);
	const char *wrapper_name = PHP_STREAM_ERROR_WRAPPER_NAME(wrapper);

	if (options & REPORT_ERRORS) {
		php_stream_wrapper_error_internal(
				wrapper_name, context, NULL, options, severity, terminating, code, param, message);
	} else {
		php_stream_wrapper_log_store_error(
				message, code, wrapper_name, param, severity, terminating);
	}
}

PHPAPI void php_stream_wrapper_log_error(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	php_stream_wrapper_log_error_internal(
			wrapper, context, options, severity, terminating, code, NULL, fmt, args);
	va_end(args);
}

PHPAPI void php_stream_wrapper_log_error_param(const php_stream_wrapper *wrapper,
		php_stream_context *context, int options, int severity, bool terminating,
		zend_enum_StreamErrorCode code, const char *param, const char *fmt, ...)
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
		php_stream_context *context, zend_enum_StreamErrorCode code, const char *path,
		const char *caption)
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

	php_stream_wrapper_error_internal(wrapper_name, context, NULL, REPORT_ERRORS, E_WARNING, true,
			code, tmp, message);

	if (free_msg) {
		efree(msg);
	}
}

PHPAPI void php_stream_display_wrapper_errors(php_stream_wrapper *wrapper,
		php_stream_context *context, zend_enum_StreamErrorCode code, const char *path,
		const char *caption)
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

/* StreamException methods */

PHP_METHOD(StreamException, getErrors)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval *errors = zend_read_property(
			php_ce_stream_exception, Z_OBJ_P(ZEND_THIS), ZEND_STRL("errors"), 1, NULL);

	RETURN_COPY(errors);
}

/* Module init */

PHP_MINIT_FUNCTION(stream_errors)
{
	php_ce_stream_error_code = register_class_StreamErrorCode();
	php_ce_stream_error_mode = register_class_StreamErrorMode();
	php_ce_stream_error_store = register_class_StreamErrorStore();

	php_ce_stream_error = register_class_StreamError();
	php_ce_stream_exception = register_class_StreamException(zend_ce_exception);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(stream_errors)
{
	return SUCCESS;
}
