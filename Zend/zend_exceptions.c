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

static zend_class_entry *base_exception_ce;
static zend_class_entry *default_exception_ce;
static zend_class_entry *error_exception_ce;
static zend_class_entry *engine_exception_ce;
static zend_class_entry *parse_exception_ce;
static zend_object_handlers default_exception_handlers;
ZEND_API void (*zend_throw_exception_hook)(zval *ex);

void zend_exception_set_previous(zend_object *exception, zend_object *add_previous)
{
    zval tmp, *previous, zv, *pzv, rv;

	if (exception == add_previous || !add_previous || !exception) {
		return;
	}
	ZVAL_OBJ(&tmp, add_previous);
	if (!instanceof_function(Z_OBJCE(tmp), base_exception_ce)) {
		zend_error(E_ERROR, "Cannot set non exception as previous exception");
		return;
	}
	ZVAL_OBJ(&zv, exception);
	pzv = &zv;
	do {
		previous = zend_read_property(base_exception_ce, pzv, "previous", sizeof("previous")-1, 1, &rv);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property(base_exception_ce, pzv, "previous", sizeof("previous")-1, &tmp);
			GC_REFCOUNT(add_previous)--;
			return;
		}
		pzv = previous;
	} while (pzv && Z_OBJ_P(pzv) != add_previous);
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

ZEND_API void zend_throw_exception_internal(zval *exception) /* {{{ */
{
#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_THROWN_ENABLED()) {
		if (exception != NULL) {
			DTRACE_EXCEPTION_THROWN(Z_OBJ_P(exception)->ce->name->val);
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
		if (exception && Z_OBJCE_P(exception) == parse_exception_ce) {
			return;
		}
		if(EG(exception)) {
			zend_exception_error(EG(exception), E_ERROR);
		}
		zend_error(E_ERROR, "Exception thrown without a stack frame");
	}

	if (zend_throw_exception_hook) {
		zend_throw_exception_hook(exception);
	}

	if (!EG(current_execute_data)->func ||
	    !ZEND_USER_CODE(EG(current_execute_data)->func->common.type) ||
	    (EG(current_execute_data)->opline+1)->opcode == ZEND_HANDLE_EXCEPTION) {
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

	Z_OBJ(obj) = object = zend_objects_new(class_type);
	Z_OBJ_HT(obj) = &default_exception_handlers;

	object_properties_init(object, class_type);

	if (EG(current_execute_data)) {
		zend_fetch_debug_backtrace(&trace, skip_top_traces, 0, 0);
	} else {
		array_init(&trace);
	}
	Z_SET_REFCOUNT(trace, 0);

	if (EXPECTED(class_type != parse_exception_ce)) {
		zend_update_property_string(base_exception_ce, &obj, "file", sizeof("file")-1, zend_get_executed_filename());
		zend_update_property_long(base_exception_ce, &obj, "line", sizeof("line")-1, zend_get_executed_lineno());
	} else {
		zend_update_property_string(base_exception_ce, &obj, "file", sizeof("file")-1, zend_get_compiled_filename()->val);
		zend_update_property_long(base_exception_ce, &obj, "line", sizeof("line")-1, zend_get_compiled_lineno());
	}
	zend_update_property(base_exception_ce, &obj, "trace", sizeof("trace")-1, &trace);

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

/* {{{ proto Exception Exception::__clone()
   Clone the exception object */
ZEND_METHOD(exception, __clone)
{
	/* Should never be executable */
	zend_throw_exception(NULL, "Cannot clone object using __clone()", 0);
}
/* }}} */

/* {{{ proto Exception::__construct(string message, int code [, Exception previous])
   Exception constructor */
ZEND_METHOD(exception, __construct)
{
	zend_string *message = NULL;
	zend_long   code = 0;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc, "|SlO!", &message, &code, &previous, base_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Exception([string $exception [, long $code [, Exception $previous = NULL]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_str(base_exception_ce, object, "message", sizeof("message")-1, message);
	}

	if (code) {
		zend_update_property_long(base_exception_ce, object, "code", sizeof("code")-1, code);
	}

	if (previous) {
		zend_update_property(base_exception_ce, object, "previous", sizeof("previous")-1, previous);
	}
}
/* }}} */

/* {{{ proto ErrorException::__construct(string message, int code, int severity [, string filename [, int lineno [, Exception previous]]])
   ErrorException constructor */
ZEND_METHOD(error_exception, __construct)
{
	char  *message = NULL, *filename = NULL;
	zend_long   code = 0, severity = E_ERROR, lineno;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS();
	size_t message_len, filename_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc, "|sllslO!", &message, &message_len, &code, &severity, &filename, &filename_len, &lineno, &previous, base_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for ErrorException([string $exception [, long $code, [ long $severity, [ string $filename, [ long $lineno  [, Exception $previous = NULL]]]]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_string(base_exception_ce, object, "message", sizeof("message")-1, message);
	}

	if (code) {
		zend_update_property_long(base_exception_ce, object, "code", sizeof("code")-1, code);
	}

	if (previous) {
		zend_update_property(base_exception_ce, object, "previous", sizeof("previous")-1, previous);
	}

	zend_update_property_long(base_exception_ce, object, "severity", sizeof("severity")-1, severity);

	if (argc >= 4) {
	    zend_update_property_string(base_exception_ce, object, "file", sizeof("file")-1, filename);
    	if (argc < 5) {
    	    lineno = 0; /* invalidate lineno */
    	}
		zend_update_property_long(base_exception_ce, object, "line", sizeof("line")-1, lineno);
	}
}
/* }}} */

#define DEFAULT_0_PARAMS \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	}

#define GET_PROPERTY(object, name) \
	zend_read_property(base_exception_ce, (object), name, sizeof(name) - 1, 0, &rv)
#define GET_PROPERTY_SILENT(object, name) \
	zend_read_property(base_exception_ce, (object), name, sizeof(name) - 1, 1, &rv)

/* {{{ proto string Exception::getFile()
   Get the file in which the exception occurred */
ZEND_METHOD(exception, getFile)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "file"));
}
/* }}} */

/* {{{ proto int Exception::getLine()
   Get the line in which the exception occurred */
ZEND_METHOD(exception, getLine)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "line"));
}
/* }}} */

/* {{{ proto string Exception::getMessage()
   Get the exception message */
ZEND_METHOD(exception, getMessage)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "message"));
}
/* }}} */

/* {{{ proto int Exception::getCode()
   Get the exception code */
ZEND_METHOD(exception, getCode)
{
	zval rv;

	DEFAULT_0_PARAMS;

	ZVAL_COPY(return_value, GET_PROPERTY(getThis(), "code"));
}
/* }}} */

/* {{{ proto array Exception::getTrace()
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
				smart_str_append(str, Z_STR_P(tmp));   \
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
	res = &str->s->val[str->s->len];
	str->s->len += len;

	for (i = 0; i < l; ++i) {
		char c = s[i];
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
		case IS_OBJECT:
			smart_str_appends(str, "Object(");
			smart_str_append(str, Z_OBJCE_P(arg)->name);
			smart_str_appends(str, "), ");
			break;
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
			size_t last_len = str->s->len;
			zval *arg;

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tmp), arg) {
				_build_trace_args(arg, str);
			} ZEND_HASH_FOREACH_END();

			if (last_len != str->s->len) {
				str->s->len -= 2; /* remove last ', ' */
			}
		} else {
			zend_error(E_WARNING, "args element is no array");
		}
	}
	smart_str_appends(str, ")\n");
}
/* }}} */

/* {{{ proto string Exception::getTraceAsString()
   Obtain the backtrace for the exception as a string (instead of an array) */
ZEND_METHOD(exception, getTraceAsString)
{
	zval *trace, *frame, rv;
	zend_ulong index;
	smart_str str = {0};
	uint32_t num = 0;

	DEFAULT_0_PARAMS;

	trace = zend_read_property(base_exception_ce, getThis(), "trace", sizeof("trace")-1, 1, &rv);
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

/* {{{ proto string Exception::getPrevious()
   Return previous Exception or NULL. */
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

/* {{{ proto string Exception::__toString()
   Obtain the string representation of the Exception object */
ZEND_METHOD(exception, __toString)
{
	zval trace, *exception;
	zend_string *str;
	zend_fcall_info fci;
	zval fname, rv;

	DEFAULT_0_PARAMS;

	str = STR_EMPTY_ALLOC();

	exception = getThis();
	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1);

	while (exception && Z_TYPE_P(exception) == IS_OBJECT) {
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

		if (message->len > 0) {
			str = zend_strpprintf(0, "exception '%s' with message '%s' in %s:" ZEND_LONG_FMT
					"\nStack trace:\n%s%s%s",
					Z_OBJCE_P(exception)->name->val, message->val, file->val, line,
					(Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace)) ? Z_STRVAL(trace) : "#0 {main}\n",
					prev_str->len ? "\n\nNext " : "", prev_str->val);
		} else {
			str = zend_strpprintf(0, "exception '%s' in %s:" ZEND_LONG_FMT
					"\nStack trace:\n%s%s%s",
					Z_OBJCE_P(exception)->name->val, file->val, line,
					(Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace)) ? Z_STRVAL(trace) : "#0 {main}\n",
					prev_str->len ? "\n\nNext " : "", prev_str->val);
		}

		zend_string_release(prev_str);
		zend_string_release(message);
		zend_string_release(file);
		zval_ptr_dtor(&trace);

		exception = GET_PROPERTY(exception, "previous");
	}
	zval_dtor(&fname);

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	zend_update_property_str(base_exception_ce, getThis(), "string", sizeof("string")-1, str);

	RETURN_STR(str);
}
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
	zend_property_info *prop;

	INIT_CLASS_ENTRY(ce, "BaseException", default_exception_functions);
	base_exception_ce = zend_register_internal_class(&ce);
	base_exception_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	base_exception_ce->create_object = NULL;
	memcpy(&default_exception_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	default_exception_handlers.clone_obj = NULL;

	zend_declare_property_string(base_exception_ce, "message", sizeof("message")-1, "", ZEND_ACC_PROTECTED);
	zend_declare_property_string(base_exception_ce, "string", sizeof("string")-1, "", ZEND_ACC_PRIVATE);
	zend_declare_property_long(base_exception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(base_exception_ce, "file", sizeof("file")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(base_exception_ce, "line", sizeof("line")-1, ZEND_ACC_PROTECTED);
	zend_declare_property_null(base_exception_ce, "trace", sizeof("trace")-1, ZEND_ACC_PRIVATE);
	zend_declare_property_null(base_exception_ce, "previous", sizeof("previous")-1, ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(ce, "Exception", NULL);
	default_exception_ce = zend_register_internal_class_ex(&ce, base_exception_ce);
	default_exception_ce->create_object = zend_default_exception_new;

	/* A trick, to make visible prvate properties of BaseException */
	ZEND_HASH_FOREACH_PTR(&default_exception_ce->properties_info, prop) {
		if (prop->flags & ZEND_ACC_SHADOW) {
			if (prop->name->len == sizeof("\0BaseException\0string")-1) {
				prop->flags &= ~ZEND_ACC_SHADOW;
				prop->flags |= ZEND_ACC_PRIVATE;
				prop->ce = default_exception_ce;
			} else if (prop->name->len == sizeof("\0BaseException\0trace")-1) {
				prop->flags &= ~ZEND_ACC_SHADOW;
				prop->flags |= ZEND_ACC_PRIVATE;
				prop->ce = default_exception_ce;
			} else if (prop->name->len == sizeof("\0BaseException\0previous")-1) {
				prop->flags &= ~ZEND_ACC_SHADOW;
				prop->flags |= ZEND_ACC_PRIVATE;
				prop->ce = default_exception_ce;
			}
		}
	} ZEND_HASH_FOREACH_END();

	INIT_CLASS_ENTRY(ce, "ErrorException", error_exception_functions);
	error_exception_ce = zend_register_internal_class_ex(&ce, default_exception_ce);
	error_exception_ce->create_object = zend_error_exception_new;
	zend_declare_property_long(error_exception_ce, "severity", sizeof("severity")-1, E_ERROR, ZEND_ACC_PROTECTED);

	INIT_CLASS_ENTRY(ce, "EngineException", NULL);
	engine_exception_ce = zend_register_internal_class_ex(&ce, base_exception_ce);
	engine_exception_ce->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "ParseException", NULL);
	parse_exception_ce = zend_register_internal_class_ex(&ce, base_exception_ce);
	parse_exception_ce->create_object = zend_default_exception_new;
}
/* }}} */

ZEND_API zend_class_entry *zend_exception_get_base(void) /* {{{ */
{
	return base_exception_ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_exception_get_default(void) /* {{{ */
{
	return default_exception_ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_get_error_exception(void) /* {{{ */
{
	return error_exception_ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_get_engine_exception(void) /* {{{ */
{
	return engine_exception_ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_get_parse_exception(void) /* {{{ */
{
	return parse_exception_ce;
}
/* }}} */


ZEND_API zend_object *zend_throw_exception(zend_class_entry *exception_ce, const char *message, zend_long code) /* {{{ */
{
	zval ex;

	if (exception_ce) {
		if (!instanceof_function(exception_ce, base_exception_ce)) {
			zend_error(E_NOTICE, "Exceptions must be derived from the Exception base class");
			exception_ce = default_exception_ce;
		}
	} else {
		exception_ce = default_exception_ce;
	}
	object_init_ex(&ex, exception_ce);


	if (message) {
		zend_update_property_string(base_exception_ce, &ex, "message", sizeof("message")-1, message);
	}
	if (code) {
		zend_update_property_long(base_exception_ce, &ex, "code", sizeof("code")-1, code);
	}

	zend_throw_exception_internal(&ex);
	return Z_OBJ(ex);
}
/* }}} */

ZEND_API zend_object *zend_throw_exception_ex(zend_class_entry *exception_ce, zend_long code, const char *format, ...) /* {{{ */
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

ZEND_API zend_object *zend_throw_error_exception(zend_class_entry *exception_ce, const char *message, zend_long code, int severity) /* {{{ */
{
	zval ex;
	zend_object *obj = zend_throw_exception(exception_ce, message, code);
	ZVAL_OBJ(&ex, obj);
	zend_update_property_long(base_exception_ce, &ex, "severity", sizeof("severity")-1, severity);
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
ZEND_API void zend_exception_error(zend_object *ex, int severity) /* {{{ */
{
	zval exception, rv;
	zend_class_entry *ce_exception;

	ZVAL_OBJ(&exception, ex);
	ce_exception = Z_OBJCE(exception);
	EG(exception) = NULL;
	if (ce_exception == parse_exception_ce || ce_exception == engine_exception_ce) {
		zend_string *message = zval_get_string(GET_PROPERTY(&exception, "message"));
		zend_string *file = zval_get_string(GET_PROPERTY_SILENT(&exception, "file"));
		zend_long line = zval_get_long(GET_PROPERTY_SILENT(&exception, "line"));
		zend_long code = zval_get_long(GET_PROPERTY_SILENT(&exception, "code"));

		zend_error_helper(code, file->val, line, "%s", message->val);
		zend_string_release(file);
		zend_string_release(message);
		OBJ_RELEASE(ex);
	} else if (instanceof_function(ce_exception, base_exception_ce)) {
		zval tmp, rv;
		zend_string *str, *file = NULL;
		zend_long line = 0;

		zend_call_method_with_0_params(&exception, ce_exception, NULL, "__tostring", &tmp);
		if (!EG(exception)) {
			if (Z_TYPE(tmp) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ce_exception->name->val);
			} else {
				zend_update_property_string(base_exception_ce, &exception, "string", sizeof("string")-1, EG(exception) ? ce_exception->name->val : Z_STRVAL(tmp));
			}
		}
		zval_ptr_dtor(&tmp);

		if (EG(exception)) {
			zval zv;

			ZVAL_OBJ(&zv, EG(exception));
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, base_exception_ce)) {
				file = zval_get_string(GET_PROPERTY_SILENT(&zv, "file"));
				line = zval_get_long(GET_PROPERTY_SILENT(&zv, "line"));
			}

			zend_error_va(E_WARNING, (file && file->len > 0) ? file->val : NULL, line,
				"Uncaught %s in exception handling during call to %s::__tostring()",
				Z_OBJCE(zv)->name->val, ce_exception->name->val);

			if (file) {
				zend_string_release(file);
			}
		}

		str = zval_get_string(GET_PROPERTY_SILENT(&exception, "string"));
		file = zval_get_string(GET_PROPERTY_SILENT(&exception, "file"));
		line = zval_get_long(GET_PROPERTY_SILENT(&exception, "line"));

		zend_error_va(severity, (file && file->len > 0) ? file->val : NULL, line,
			"Uncaught %s\n  thrown", str->val);

		zend_string_release(str);
		zend_string_release(file);
	} else {
		zend_error(severity, "Uncaught exception '%s'", ce_exception->name->val);
	}
}
/* }}} */

ZEND_API void zend_throw_exception_object(zval *exception) /* {{{ */
{
	zend_class_entry *exception_ce;

	if (exception == NULL || Z_TYPE_P(exception) != IS_OBJECT) {
		zend_error(E_ERROR, "Need to supply an object when throwing an exception");
	}

	exception_ce = Z_OBJCE_P(exception);

	if (!exception_ce || !instanceof_function(exception_ce, base_exception_ce)) {
		zend_error(E_ERROR, "Exceptions must be valid objects derived from the Exception base class");
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
