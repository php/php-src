/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Marcus Boerger <helly@php.net>                              |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_vm.h"
#include "zend_dtrace.h"
#include "zend_smart_str.h"

ZEND_API zend_class_entry *zend_ce_throwable;
ZEND_API zend_class_entry *zend_ce_exception;
ZEND_API zend_class_entry *zend_ce_error_exception;
ZEND_API zend_class_entry *zend_ce_error;
ZEND_API zend_class_entry *zend_ce_parse_error;
ZEND_API zend_class_entry *zend_ce_type_error;
ZEND_API zend_class_entry *zend_ce_arithmetic_error;
ZEND_API zend_class_entry *zend_ce_division_by_zero_error;

ZEND_API void (*zend_throw_exception_hook)(zval *ex);

static zend_object_handlers default_exception_handlers;

/* {{{ zend_implement_throwable */
static int zend_implement_throwable(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (instanceof_function(class_type, zend_ce_exception) || instanceof_function(class_type, zend_ce_error)) {
		return SUCCESS;
	}
	zend_error_noreturn(E_ERROR, "Class %s cannot implement interface %s, extend %s or %s instead",
		ZSTR_VAL(class_type->name),
		ZSTR_VAL(interface->name),
		ZSTR_VAL(zend_ce_exception->name),
		ZSTR_VAL(zend_ce_error->name));
	return FAILURE;
}
/* }}} */

static inline zend_class_entry *i_get_exception_base(zval *object)
{
	return instanceof_function(Z_OBJCE_P(object), zend_ce_exception) ? zend_ce_exception : zend_ce_error;
}

ZEND_API zend_class_entry *zend_get_exception_base(zval *object)
{
	return i_get_exception_base(object);
}

void zend_exception_set_previous(zend_object *exception, zend_object *add_previous)
{
    zval *previous, *ancestor, *ex;
	zval  pv, zv, rv;
	zend_class_entry *base_ce;

	if (exception == add_previous || !add_previous || !exception) {
		return;
	}
	ZVAL_OBJ(&pv, add_previous);
	if (!instanceof_function(Z_OBJCE(pv), zend_ce_throwable)) {
		zend_error_noreturn(E_CORE_ERROR, "Previous exception must implement Throwable");
		return;
	}
	ZVAL_OBJ(&zv, exception);
	ex = &zv;
	do {
		ancestor = zend_read_property(i_get_exception_base(&pv), &pv, "previous", sizeof("previous")-1, 1, &rv);
		while (Z_TYPE_P(ancestor) == IS_OBJECT) {
			if (Z_OBJ_P(ancestor) == Z_OBJ_P(ex)) {
				OBJ_RELEASE(add_previous);
				return;
			}
			ancestor = zend_read_property(i_get_exception_base(ancestor), ancestor, "previous", sizeof("previous")-1, 1, &rv);
		}
		base_ce = i_get_exception_base(ex);
		previous = zend_read_property(base_ce, ex, "previous", sizeof("previous")-1, 1, &rv);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property(base_ce, ex, "previous", sizeof("previous")-1, &pv);
			GC_REFCOUNT(add_previous)--;
			return;
		}
		ex = previous;
	} while (Z_OBJ_P(ex) != add_previous);
}

void zend_exception_save(void) /* {{{ */
{
	if (EG(prev_exception)) {
		zend_exception_set_previous(EG(exception), EG(prev_exception));
	}
	if (EG(exception)) {
		EG(prev_exception) = EG(exception);
	}
	EG(exception) = NULL;
}
/* }}} */

void zend_exception_restore(void) /* {{{ */
{
	if (EG(prev_exception)) {
		if (EG(exception)) {
			zend_exception_set_previous(EG(exception), EG(prev_exception));
		} else {
			EG(exception) = EG(prev_exception);
		}
		EG(prev_exception) = NULL;
	}
}
/* }}} */

ZEND_API ZEND_COLD void zend_throw_exception_internal(zval *exception) /* {{{ */
{
#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_THROWN_ENABLED()) {
		if (exception != NULL) {
			DTRACE_EXCEPTION_THROWN(ZSTR_VAL(Z_OBJ_P(exception)->ce->name));
		} else {
			DTRACE_EXCEPTION_THROWN(NULL);
		}
	}
#endif /* HAVE_DTRACE */

	if (exception != NULL) {
		zend_object *previous = EG(exception);
		zend_exception_set_previous(Z_OBJ_P(exception), EG(exception));
		EG(exception) = Z_OBJ_P(exception);
		if (previous) {
			return;
		}
	}
	if (!EG(current_execute_data)) {
		if (exception && Z_OBJCE_P(exception) == zend_ce_parse_error) {
			return;
		}
		if(EG(exception)) {
			zend_exception_error(EG(exception), E_ERROR);
		}
		zend_error_noreturn(E_CORE_ERROR, "Exception thrown without a stack frame");
	}

	if (zend_throw_exception_hook) {
		zend_throw_exception_hook(exception);
	}

	if (!EG(current_execute_data)->func ||
	    !ZEND_USER_CODE(EG(current_execute_data)->func->common.type) ||
	    EG(current_execute_data)->opline->opcode == ZEND_HANDLE_EXCEPTION) {
		/* no need to rethrow the exception */
		return;
	}
	EG(opline_before_exception) = EG(current_execute_data)->opline;
	EG(current_execute_data)->opline = EG(exception_op);
}
/* }}} */

ZEND_API void zend_clear_exception(void) /* {{{ */
{
	if (EG(prev_exception)) {

		OBJ_RELEASE(EG(prev_exception));
		EG(prev_exception) = NULL;
	}
	if (!EG(exception)) {
		return;
	}
	OBJ_RELEASE(EG(exception));
	EG(exception) = NULL;
	EG(current_execute_data)->opline = EG(opline_before_exception);
#if ZEND_DEBUG
	EG(opline_before_exception) = NULL;
#endif
}
/* }}} */

static zend_object *zend_default_exception_new_ex(zend_class_entry *class_type, int skip_top_traces) /* {{{ */
{
	zval obj;
	zend_object *object;
	zval trace;
	zend_class_entry *base_ce;
	zend_string *filename;

	Z_OBJ(obj) = object = zend_objects_new(class_type);
	Z_OBJ_HT(obj) = &default_exception_handlers;

	object_properties_init(object, class_type);

	if (EG(current_execute_data)) {
		zend_fetch_debug_backtrace(&trace, skip_top_traces, 0, 0);
	} else {
		array_init(&trace);
	}
	Z_SET_REFCOUNT(trace, 0);

	base_ce = i_get_exception_base(&obj);

	if (EXPECTED(class_type != zend_ce_parse_error || !(filename = zend_get_compiled_filename()))) {
		zend_update_property_string(base_ce, &obj, "file", sizeof("file")-1, zend_get_executed_filename());
		zend_update_property_long(base_ce, &obj, "line", sizeof("line")-1, zend_get_executed_lineno());
	} else {
		zend_update_property_str(base_ce, &obj, "file", sizeof("file")-1, filename);
		zend_update_property_long(base_ce, &obj, "line", sizeof("line")-1, zend_get_compiled_lineno());
	}
	zend_update_property(base_ce, &obj, "trace", sizeof("trace")-1, &trace);

	return object;
}
/* }}} */

static zend_object *zend_default_exception_new(zend_class_entry *class_type) /* {{{ */
{
	return zend_default_exception_new_ex(class_type, 0);
}
/* }}} */

static zend_object *zend_error_exception_new(zend_class_entry *class_type) /* {{{ */
{
	return zend_default_exception_new_ex(class_type, 2);
}
/* }}} */

/* {{{ proto Exception|Error Exception|Error::__clone()
   Clone the exception object */
ZEND_COLD ZEND_METHOD(exception, __clone)
{
	/* Should never be executable */
	zend_throw_exception(NULL, "Cannot clone object using __clone()", 0);
}
/* }}} */

/* {{{ proto Exception|Error::__construct(string message, int code [, Throwable previous])
   Exception constructor */
ZEND_METHOD(exception, __construct)
{
	zend_string *message = NULL;
	zend_long   code = 0;
	zval  *object, *previous = NULL;
	zend_class_entry *base_ce;
	int    argc = ZEND_NUM_ARGS();

	object = getThis();
	base_ce = i_get_exception_base(object);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc, "|SlO!", &message, &code, &previous, zend_ce_throwable) == FAILURE) {
		zend_class_entry *ce;

		if (execute_data->called_scope) {
			ce = execute_data->called_scope;
		} else {
			ce = base_ce;
		}
		zend_throw_error(NULL, "Wrong parameters for %s([string $message [, long $code [, Throwable $previous = NULL]]])", ZSTR_VAL(ce->name));
		return;
	}

	if (message) {
		zend_update_property_str(base_ce, object, "message", sizeof("message")-1, message);
	}

	if (code) {
		zend_update_property_long(base_ce, object, "code", sizeof("code")-1, code);
	}

	if (previous) {
		zend_update_property(base_ce, object, "previous", sizeof("previous")-1, previous);
	}
}
/* }}} */

/* {{{ proto Exception::__wakeup()
   Exception unserialize checks */
#define CHECK_EXC_TYPE(name, type) \
	ZVAL_UNDEF(&value); \
	pvalue = zend_read_property(i_get_exception_base(object), (object), name, sizeof(name) - 1, 1, &value); \
	if(Z_TYPE_P(pvalue) != IS_UNDEF && Z_TYPE_P(pvalue) != type) { \
		zval tmp; \
		ZVAL_STRINGL(&tmp, name, sizeof(name) - 1); \
		Z_OBJ_HANDLER_P(object, unset_property)(object, &tmp, NULL); \
		zval_ptr_dtor(&tmp); \
	}

ZEND_METHOD(exception, __wakeup)
{
	zval value, *pvalue;
	zval *object = getThis();
	CHECK_EXC_TYPE("message", IS_STRING);
	CHECK_EXC_TYPE("string", IS_STRING);
	CHECK_EXC_TYPE("code", IS_LONG);
	CHECK_EXC_TYPE("file", IS_STRING);
	CHECK_EXC_TYPE("line", IS_LONG);
	CHECK_EXC_TYPE("trace", IS_ARRAY);
	CHECK_EXC_TYPE("previous", IS_OBJECT);
}
/* }}} */

/* {{{ proto ErrorException::__construct(string message, int code, int severity [, string filename [, int lineno [, Throwable previous]]])
   ErrorException constructor */
ZEND_METHOD(error_exception, __construct)
{
	char  *message = NULL, *filename = NULL;
	zend_long   code = 0, severity = E_ERROR, lineno;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS();
	size_t message_len, filename_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc, "|sllslO!", &message, &message_len, &code, &severity, &filename, &filename_len, &lineno, &previous, zend_ce_throwable) == FAILURE) {
		zend_class_entry *ce;

		if (execute_data->called_scope) {
			ce = execute_data->called_scope;
		} else {
			ce = zend_ce_error_exception;
		}
		zend_throw_error(NULL, "Wrong parameters for %s([string $message [, long $code, [ long $severity, [ string $filename, [ long $lineno  [, Throwable $previous = NULL]]]]]])", ZSTR_VAL(ce->name));
		return;
	}

	object = getThis();

	if (message) {
		zend_update_property_string(zend_ce_exception, object, "message", sizeof("message")-1, message);
	}

	if (code) {
		zend_update_property_long(zend_ce_exception, object, "code", sizeof("code")-1, code);
	}

	if (previous) {
		zend_update_property(zend_ce_exception, object, "previous", sizeof("previous")-1, previous);
	}

	zend_update_property_long(zend_ce_error_exception, object, "severity", sizeof("severity")-1, severity);

	if (argc >= 4) {
	    zend_update_property_string(zend_ce_exception, object, "file", sizeof("file")-1, filename);
    	if (argc < 5) {
    	    lineno = 0; /* invalidate lineno */
    	}
		zend_update_property_long(zend_ce_exception, object, "line", sizeof("line")-1, lineno);
	}
}
/* }}} */

#define DEFAULT_0_PARAMS \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	}

#define GET_PROPERTY(object, name) \
	zend_read_property(i_get_exception_base(object), (object), name, sizeof(name) - 1, 0, &rv)
#define GET_PROPERTY_SILENT(object, name) \
	zend_read_property(i_get_exception_base(object), (object), name, sizeof(name) - 1, 1, &rv)

/* {{{ proto string Exception|Error::getFile()
   Get the file in which the exception occurred */
ZEND_METHOD(exception, getFile)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "file"));
}
/* }}} */

/* {{{ proto int Exception|Error::getLine()
   Get the line in which the exception occurred */
ZEND_METHOD(exception, getLine)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "line"));
}
/* }}} */

/* {{{ proto string Exception|Error::getMessage()
   Get the exception message */
ZEND_METHOD(exception, getMessage)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "message"));
}
/* }}} */

/* {{{ proto int Exception|Error::getCode()
   Get the exception code */
ZEND_METHOD(exception, getCode)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "code"));
}
/* }}} */

/* {{{ proto array Exception|Error::getTrace()
   Get the stack trace for the location in which the exception occurred */
ZEND_METHOD(exception, getTrace)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "trace"));
}
/* }}} */

/* {{{ proto int ErrorException::getSeverity()
   Get the exception severity */
ZEND_METHOD(error_exception, getSeverity)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "severity"));
}
/* }}} */

#define TRACE_APPEND_KEY(key) do {                                          \
		tmp = zend_hash_str_find(ht, key, sizeof(key)-1);                   \
		if (tmp) {                                                          \
			if (Z_TYPE_P(tmp) != IS_STRING) {                               \
				zend_error(E_WARNING, "Value for %s is no string", key);    \
				smart_str_appends(str, "[unknown]");                        \
			} else {                                                        \
				smart_str_appends(str, Z_STRVAL_P(tmp));   \
			}                                                               \
		} \
	} while (0)

/* Windows uses VK_ESCAPE instead of \e */
#ifndef VK_ESCAPE
#define VK_ESCAPE '\e'
#endif

static size_t compute_escaped_string_len(const char *s, size_t l) {
	size_t i, len = l;
	for (i = 0; i < l; ++i) {
		char c = s[i];
		if (c == '\n' || c == '\r' || c == '\t' ||
			c == '\f' || c == '\v' || c == '\\' || c == VK_ESCAPE) {
			len += 1;
		} else if (c < 32 || c > 126) {
			len += 3;
		}
	}
	return len;
}

static void smart_str_append_escaped(smart_str *str, const char *s, size_t l) {
	char *res;
	size_t i, len = compute_escaped_string_len(s, l);

	smart_str_alloc(str, len, 0);
	res = &ZSTR_VAL(str->s)[ZSTR_LEN(str->s)];
	ZSTR_LEN(str->s) += len;

	for (i = 0; i < l; ++i) {
		unsigned char c = s[i];
		if (c < 32 || c == '\\' || c > 126) {
			*res++ = '\\';
			switch (c) {
				case '\n': *res++ = 'n'; break;
				case '\r': *res++ = 'r'; break;
				case '\t': *res++ = 't'; break;
				case '\f': *res++ = 'f'; break;
				case '\v': *res++ = 'v'; break;
				case '\\': *res++ = '\\'; break;
				case VK_ESCAPE: *res++ = 'e'; break;
				default:
					*res++ = 'x';
					if ((c >> 4) < 10) {
						*res++ = (c >> 4) + '0';
					} else {
						*res++ = (c >> 4) + 'A' - 10;
					}
					if ((c & 0xf) < 10) {
						*res++ = (c & 0xf) + '0';
					} else {
						*res++ = (c & 0xf) + 'A' - 10;
					}
			}
		} else {
			*res++ = c;
		}
	}
}

static void _build_trace_args(zval *arg, smart_str *str) /* {{{ */
{
	/* the trivial way would be to do
	 * convert_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	ZVAL_DEREF(arg);
	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			smart_str_appends(str, "NULL, ");
			break;
		case IS_STRING:
			smart_str_appendc(str, '\'');
			smart_str_append_escaped(str, Z_STRVAL_P(arg), MIN(Z_STRLEN_P(arg), 15));
			if (Z_STRLEN_P(arg) > 15) {
				smart_str_appends(str, "...', ");
			} else {
				smart_str_appends(str, "', ");
			}
			break;
		case IS_FALSE:
			smart_str_appends(str, "false, ");
			break;
		case IS_TRUE:
			smart_str_appends(str, "true, ");
			break;
		case IS_RESOURCE:
			smart_str_appends(str, "Resource id #");
			smart_str_append_long(str, Z_RES_HANDLE_P(arg));
			smart_str_appends(str, ", ");
			break;
		case IS_LONG:
			smart_str_append_long(str, Z_LVAL_P(arg));
			smart_str_appends(str, ", ");
			break;
		case IS_DOUBLE: {
			double dval = Z_DVAL_P(arg);
			char *s_tmp = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			int l_tmp = zend_sprintf(s_tmp, "%.*G", (int) EG(precision), dval);  /* SAFE */
			smart_str_appendl(str, s_tmp, l_tmp);
			smart_str_appends(str, ", ");
			efree(s_tmp);
			break;
		}
		case IS_ARRAY:
			smart_str_appends(str, "Array, ");
			break;
		case IS_OBJECT: {
			zend_string *class_name = Z_OBJ_HANDLER_P(arg, get_class_name)(Z_OBJ_P(arg));
			smart_str_appends(str, "Object(");
			smart_str_appends(str, ZSTR_VAL(class_name));
			smart_str_appends(str, "), ");
			break;
		}
	}
}
/* }}} */

static void _build_trace_string(smart_str *str, HashTable *ht, uint32_t num) /* {{{ */
{
	zval *file, *tmp;

	smart_str_appendc(str, '#');
	smart_str_append_long(str, num);
	smart_str_appendc(str, ' ');

	file = zend_hash_str_find(ht, "file", sizeof("file")-1);
	if (file) {
		if (Z_TYPE_P(file) != IS_STRING) {
			zend_error(E_WARNING, "Function name is no string");
			smart_str_appends(str, "[unknown function]");
		} else{
			zend_long line;
			tmp = zend_hash_str_find(ht, "line", sizeof("line")-1);
			if (tmp) {
				if (Z_TYPE_P(tmp) == IS_LONG) {
					line = Z_LVAL_P(tmp);
				} else {
					zend_error(E_WARNING, "Line is no long");
					line = 0;
				}
			} else {
				line = 0;
			}
			smart_str_append(str, Z_STR_P(file));
			smart_str_appendc(str, '(');
			smart_str_append_long(str, line);
			smart_str_appends(str, "): ");
		}
	} else {
		smart_str_appends(str, "[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	smart_str_appendc(str, '(');
	tmp = zend_hash_str_find(ht, "args", sizeof("args")-1);
	if (tmp) {
		if (Z_TYPE_P(tmp) == IS_ARRAY) {
			size_t last_len = ZSTR_LEN(str->s);
			zval *arg;

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tmp), arg) {
				_build_trace_args(arg, str);
			} ZEND_HASH_FOREACH_END();

			if (last_len != ZSTR_LEN(str->s)) {
				ZSTR_LEN(str->s) -= 2; /* remove last ', ' */
			}
		} else {
			zend_error(E_WARNING, "args element is no array");
		}
	}
	smart_str_appends(str, ")\n");
}
/* }}} */

/* {{{ proto string Exception|Error::getTraceAsString()
   Obtain the backtrace for the exception as a string (instead of an array) */
ZEND_METHOD(exception, getTraceAsString)
{
	zval *trace, *frame, rv;
	zend_ulong index;
	zval *object;
	zend_class_entry *base_ce;
	smart_str str = {0};
	uint32_t num = 0;

	DEFAULT_0_PARAMS;

	object = getThis();
	base_ce = i_get_exception_base(object);

	trace = zend_read_property(base_ce, object, "trace", sizeof("trace")-1, 1, &rv);
	if (Z_TYPE_P(trace) != IS_ARRAY) {
		RETURN_FALSE;
	}
	ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(trace), index, frame) {
		if (Z_TYPE_P(frame) != IS_ARRAY) {
			zend_error(E_WARNING, "Expected array for frame %pu", index);
			continue;
		}

		_build_trace_string(&str, Z_ARRVAL_P(frame), num++);
	} ZEND_HASH_FOREACH_END();

	smart_str_appendc(&str, '#');
	smart_str_append_long(&str, num);
	smart_str_appends(&str, " {main}");
	smart_str_0(&str);

	RETURN_NEW_STR(str.s);
}
/* }}} */

/* {{{ proto Throwable Exception|Error::getPrevious()
   Return previous Throwable or NULL. */
ZEND_METHOD(exception, getPrevious)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY_SILENT(getThis(), "previous"));
} /* }}} */

size_t zend_spprintf(char **message, size_t max_len, const char *format, ...) /* {{{ */
{
	va_list arg;
	size_t len;

	va_start(arg, format);
	len = zend_vspprintf(message, max_len, format, arg);
	va_end(arg);
	return len;
}
/* }}} */

zend_string *zend_strpprintf(size_t max_len, const char *format, ...) /* {{{ */
{
	va_list arg;
	zend_string *str;

	va_start(arg, format);
	str = zend_vstrpprintf(max_len, format, arg);
	va_end(arg);
	return str;
}
/* }}} */

/* {{{ proto string Exception|Error::__toString()
   Obtain the string representation of the Exception object */
ZEND_METHOD(exception, __toString)
{
	zval trace, *exception;
	zend_class_entry *base_ce;
	zend_string *str;
	zend_fcall_info fci;
	zval fname, rv;

	DEFAULT_0_PARAMS;

	str = ZSTR_EMPTY_ALLOC();

	exception = getThis();
	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1);

	while (exception && Z_TYPE_P(exception) == IS_OBJECT && instanceof_function(Z_OBJCE_P(exception), zend_ce_throwable)) {
		zend_string *prev_str = str;
		zend_string *message = zval_get_string(GET_PROPERTY(exception, "message"));
		zend_string *file = zval_get_string(GET_PROPERTY(exception, "file"));
		zend_long line = zval_get_long(GET_PROPERTY(exception, "line"));

		fci.size = sizeof(fci);
		fci.function_table = &Z_OBJCE_P(exception)->function_table;
		ZVAL_COPY_VALUE(&fci.function_name, &fname);
		fci.symbol_table = NULL;
		fci.object = Z_OBJ_P(exception);
		fci.retval = &trace;
		fci.param_count = 0;
		fci.params = NULL;
		fci.no_separation = 1;

		zend_call_function(&fci, NULL);

		if (Z_TYPE(trace) != IS_STRING) {
			zval_ptr_dtor(&trace);
			ZVAL_UNDEF(&trace);
		}

		if (Z_OBJCE_P(exception) == zend_ce_type_error && strstr(ZSTR_VAL(message), ", called in ")) {
			zend_string *real_message = zend_strpprintf(0, "%s and defined", ZSTR_VAL(message));
			zend_string_release(message);
			message = real_message;
		}

		if (ZSTR_LEN(message) > 0) {
			str = zend_strpprintf(0, "%s: %s in %s:" ZEND_LONG_FMT
					"\nStack trace:\n%s%s%s",
					ZSTR_VAL(Z_OBJCE_P(exception)->name), ZSTR_VAL(message), ZSTR_VAL(file), line,
					(Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace)) ? Z_STRVAL(trace) : "#0 {main}\n",
					ZSTR_LEN(prev_str) ? "\n\nNext " : "", ZSTR_VAL(prev_str));
		} else {
			str = zend_strpprintf(0, "%s in %s:" ZEND_LONG_FMT
					"\nStack trace:\n%s%s%s",
					ZSTR_VAL(Z_OBJCE_P(exception)->name), ZSTR_VAL(file), line,
					(Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace)) ? Z_STRVAL(trace) : "#0 {main}\n",
					ZSTR_LEN(prev_str) ? "\n\nNext " : "", ZSTR_VAL(prev_str));
		}

		zend_string_release(prev_str);
		zend_string_release(message);
		zend_string_release(file);
		zval_ptr_dtor(&trace);

		exception = GET_PROPERTY(exception, "previous");
	}
	zval_dtor(&fname);

	exception = getThis();
	base_ce = i_get_exception_base(exception);

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	zend_update_property_str(base_ce, exception, "string", sizeof("string")-1, str);

	RETURN_STR(str);
}
/* }}} */

/** {{{ Throwable method definition */
const zend_function_entry zend_funcs_throwable[] = {
	ZEND_ABSTRACT_ME(throwable, getMessage,       NULL)
	ZEND_ABSTRACT_ME(throwable, getCode,          NULL)
	ZEND_ABSTRACT_ME(throwable, getFile,          NULL)
	ZEND_ABSTRACT_ME(throwable, getLine,          NULL)
	ZEND_ABSTRACT_ME(throwable, getTrace,         NULL)
	ZEND_ABSTRACT_ME(throwable, getPrevious,      NULL)
	ZEND_ABSTRACT_ME(throwable, getTraceAsString, NULL)
	ZEND_ABSTRACT_ME(throwable, __toString,       NULL)
	ZEND_FE_END
};
/* }}} */

/* {{{ internal structs */
/* All functions that may be used in uncaught exception handlers must be final
 * and must not throw exceptions. Otherwise we would need a facility to handle
 * such exceptions in that handler.
 * Also all getXY() methods are final because thy serve as read only access to
 * their corresponding properties, no more, no less. If after all you need to
 * override somthing then it is method __toString().
 * And never try to change the state of exceptions and never implement anything
 * that gives the user anything to accomplish this.
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_exception___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, code)
	ZEND_ARG_INFO(0, previous)
ZEND_END_ARG_INFO()

static const zend_function_entry default_exception_functions[] = {
	ZEND_ME(exception, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(exception, __construct, arginfo_exception___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(exception, __wakeup, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(exception, getMessage, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getCode, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getFile, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getLine, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getTrace, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getPrevious, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getTraceAsString, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, __toString, NULL, 0)
	ZEND_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_error_exception___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, code)
	ZEND_ARG_INFO(0, severity)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, lineno)
	ZEND_ARG_INFO(0, previous)
ZEND_END_ARG_INFO()

static const zend_function_entry error_exception_functions[] = {
	ZEND_ME(error_exception, __construct, arginfo_error_exception___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(error_exception, getSeverity, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_FE_END
};
/* }}} */

void zend_register_default_exception(void) /* {{{ */
{
	zend_class_entry ce;

	REGISTER_MAGIC_INTERFACE(throwable, Throwable);

	memcpy(&default_exception_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	default_exception_handlers.clone_obj = NULL;

	INIT_CLASS_ENTRY(ce, "Exception", default_exception_functions);
	zend_ce_exception = zend_register_internal_class_ex(&ce, NULL);
	zend_ce_exception->create_object = zend_default_exception_new;
	zend_class_implements(zend_ce_exception, 1, zend_ce_throwable);

	zend_declare_property_string(zend_ce_exception, "message", sizeof("message")-1, "", ZEND_ACC_PROTECTED);
	zend_declare_property_string(zend_ce_exception, "string", sizeof("string")-1, "", ZEND_ACC_PRIVATE);
	zend_declare_property_long(zend_ce_exception, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(zend_ce_exception, "file", sizeof("file")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(zend_ce_exception, "line", sizeof("line")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(zend_ce_exception, "trace", sizeof("trace")-1, ZEND_ACC_PRIVATE);
	zend_declare_property_null(zend_ce_exception, "previous", sizeof("previous")-1, ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(ce, "ErrorException", error_exception_functions);
	zend_ce_error_exception = zend_register_internal_class_ex(&ce, zend_ce_exception);
	zend_ce_error_exception->create_object = zend_error_exception_new;
	zend_declare_property_long(zend_ce_error_exception, "severity", sizeof("severity")-1, E_ERROR, ZEND_ACC_PROTECTED);

	INIT_CLASS_ENTRY(ce, "Error", default_exception_functions);
	zend_ce_error = zend_register_internal_class_ex(&ce, NULL);
	zend_ce_error->create_object = zend_default_exception_new;
	zend_class_implements(zend_ce_error, 1, zend_ce_throwable);

	zend_declare_property_string(zend_ce_error, "message", sizeof("message")-1, "", ZEND_ACC_PROTECTED);
	zend_declare_property_string(zend_ce_error, "string", sizeof("string")-1, "", ZEND_ACC_PRIVATE);
	zend_declare_property_long(zend_ce_error, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(zend_ce_error, "file", sizeof("file")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(zend_ce_error, "line", sizeof("line")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(zend_ce_error, "trace", sizeof("trace")-1, ZEND_ACC_PRIVATE);
	zend_declare_property_null(zend_ce_error, "previous", sizeof("previous")-1, ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(ce, "ParseError", NULL);
	zend_ce_parse_error = zend_register_internal_class_ex(&ce, zend_ce_error);
	zend_ce_parse_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "TypeError", NULL);
	zend_ce_type_error = zend_register_internal_class_ex(&ce, zend_ce_error);
	zend_ce_type_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "ArithmeticError", NULL);
	zend_ce_arithmetic_error = zend_register_internal_class_ex(&ce, zend_ce_error);
	zend_ce_arithmetic_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "DivisionByZeroError", NULL);
	zend_ce_division_by_zero_error = zend_register_internal_class_ex(&ce, zend_ce_arithmetic_error);
	zend_ce_division_by_zero_error->create_object = zend_default_exception_new;
}
/* }}} */

/* {{{ Deprecated - Use zend_ce_exception directly instead */
ZEND_API zend_class_entry *zend_exception_get_default(void)
{
	return zend_ce_exception;
}
/* }}} */

/* {{{ Deprecated - Use zend_ce_error_exception directly instead */
ZEND_API zend_class_entry *zend_get_error_exception(void)
{
	return zend_ce_error_exception;
}
/* }}} */

ZEND_API ZEND_COLD zend_object *zend_throw_exception(zend_class_entry *exception_ce, const char *message, zend_long code) /* {{{ */
{
	zval ex;

	if (exception_ce) {
		if (!instanceof_function(exception_ce, zend_ce_throwable)) {
			zend_error(E_NOTICE, "Exceptions must implement Throwable");
			exception_ce = zend_ce_exception;
		}
	} else {
		exception_ce = zend_ce_exception;
	}
	object_init_ex(&ex, exception_ce);


	if (message) {
		zend_update_property_string(exception_ce, &ex, "message", sizeof("message")-1, message);
	}
	if (code) {
		zend_update_property_long(exception_ce, &ex, "code", sizeof("code")-1, code);
	}

	zend_throw_exception_internal(&ex);
	return Z_OBJ(ex);
}
/* }}} */

ZEND_API ZEND_COLD zend_object *zend_throw_exception_ex(zend_class_entry *exception_ce, zend_long code, const char *format, ...) /* {{{ */
{
	va_list arg;
	char *message;
	zend_object *obj;

	va_start(arg, format);
	zend_vspprintf(&message, 0, format, arg);
	va_end(arg);
	obj = zend_throw_exception(exception_ce, message, code);
	efree(message);
	return obj;
}
/* }}} */

ZEND_API ZEND_COLD zend_object *zend_throw_error_exception(zend_class_entry *exception_ce, const char *message, zend_long code, int severity) /* {{{ */
{
	zval ex;
	zend_object *obj = zend_throw_exception(exception_ce, message, code);
	ZVAL_OBJ(&ex, obj);
	zend_update_property_long(zend_ce_error_exception, &ex, "severity", sizeof("severity")-1, severity);
	return obj;
}
/* }}} */

static void zend_error_va(int type, const char *file, uint lineno, const char *format, ...) /* {{{ */
{
	va_list args;

	va_start(args, format);
	zend_error_cb(type, file, lineno, format, args);
	va_end(args);
}
/* }}} */

static void zend_error_helper(int type, const char *filename, const uint lineno, const char *format, ...)
{
	va_list va;

	va_start(va, format);
	zend_error_cb(type, filename, lineno, format, va);
	va_end(va);
}

/* This function doesn't return if it uses E_ERROR */
ZEND_API ZEND_COLD void zend_exception_error(zend_object *ex, int severity) /* {{{ */
{
	zval exception, rv;
	zend_class_entry *ce_exception;

	ZVAL_OBJ(&exception, ex);
	ce_exception = Z_OBJCE(exception);
	EG(exception) = NULL;
	if (ce_exception == zend_ce_parse_error) {
		zend_string *message = zval_get_string(GET_PROPERTY(&exception, "message"));
		zend_string *file = zval_get_string(GET_PROPERTY_SILENT(&exception, "file"));
		zend_long line = zval_get_long(GET_PROPERTY_SILENT(&exception, "line"));

		zend_error_helper(E_PARSE, ZSTR_VAL(file), line, "%s", ZSTR_VAL(message));

		zend_string_release(file);
		zend_string_release(message);
	} else if (instanceof_function(ce_exception, zend_ce_throwable)) {
		zval tmp, rv;
		zend_string *str, *file = NULL;
		zend_long line = 0;

		zend_call_method_with_0_params(&exception, ce_exception, NULL, "__tostring", &tmp);
		if (!EG(exception)) {
			if (Z_TYPE(tmp) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ZSTR_VAL(ce_exception->name));
			} else {
				zend_update_property(i_get_exception_base(&exception), &exception, "string", sizeof("string")-1, &tmp);
			}
		}
		zval_ptr_dtor(&tmp);

		if (EG(exception)) {
			zval zv;

			ZVAL_OBJ(&zv, EG(exception));
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, zend_ce_exception) || instanceof_function(ce_exception, zend_ce_error)) {
				file = zval_get_string(GET_PROPERTY_SILENT(&zv, "file"));
				line = zval_get_long(GET_PROPERTY_SILENT(&zv, "line"));
			}

			zend_error_va(E_WARNING, (file && ZSTR_LEN(file) > 0) ? ZSTR_VAL(file) : NULL, line,
				"Uncaught %s in exception handling during call to %s::__tostring()",
				ZSTR_VAL(Z_OBJCE(zv)->name), ZSTR_VAL(ce_exception->name));

			if (file) {
				zend_string_release(file);
			}
		}

		str = zval_get_string(GET_PROPERTY_SILENT(&exception, "string"));
		file = zval_get_string(GET_PROPERTY_SILENT(&exception, "file"));
		line = zval_get_long(GET_PROPERTY_SILENT(&exception, "line"));

		zend_error_va(severity, (file && ZSTR_LEN(file) > 0) ? ZSTR_VAL(file) : NULL, line,
			"Uncaught %s\n  thrown", ZSTR_VAL(str));

		zend_string_release(str);
		zend_string_release(file);
	} else {
		zend_error(severity, "Uncaught exception '%s'", ZSTR_VAL(ce_exception->name));
	}

	OBJ_RELEASE(ex);
}
/* }}} */

ZEND_API ZEND_COLD void zend_throw_exception_object(zval *exception) /* {{{ */
{
	zend_class_entry *exception_ce;

	if (exception == NULL || Z_TYPE_P(exception) != IS_OBJECT) {
		zend_error_noreturn(E_CORE_ERROR, "Need to supply an object when throwing an exception");
	}

	exception_ce = Z_OBJCE_P(exception);

	if (!exception_ce || !instanceof_function(exception_ce, zend_ce_throwable)) {
		zend_throw_error(NULL, "Cannot throw objects that do not implement Throwable");
		return;
	}
	zend_throw_exception_internal(exception);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
