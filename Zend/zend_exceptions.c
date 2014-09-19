/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

static zend_class_entry *default_exception_ce;
static zend_class_entry *error_exception_ce;
static zend_object_handlers default_exception_handlers;
ZEND_API void (*zend_throw_exception_hook)(zval *ex TSRMLS_DC);

void zend_exception_set_previous(zend_object *exception, zend_object *add_previous TSRMLS_DC)
{
    zval tmp, *previous, zv, *pzv;

	if (exception == add_previous || !add_previous || !exception) {
		return;
	}
	ZVAL_OBJ(&tmp, add_previous);
	if (!instanceof_function(Z_OBJCE(tmp), default_exception_ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot set non exception as previous exception");
		return;
	}
	ZVAL_OBJ(&zv, exception);
	pzv = &zv;
	do {
		previous = zend_read_property(default_exception_ce, pzv, "previous", sizeof("previous")-1, 1 TSRMLS_CC);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property(default_exception_ce, pzv, "previous", sizeof("previous")-1, &tmp TSRMLS_CC);
			GC_REFCOUNT(add_previous)--;
			return;
		}
		pzv = previous;
	} while (pzv && Z_OBJ_P(pzv) != add_previous);
}

void zend_exception_save(TSRMLS_D) /* {{{ */
{
	if (EG(prev_exception)) {
		zend_exception_set_previous(EG(exception), EG(prev_exception) TSRMLS_CC);
	}
	if (EG(exception)) {
		EG(prev_exception) = EG(exception);
	}
	EG(exception) = NULL;
}
/* }}} */

void zend_exception_restore(TSRMLS_D) /* {{{ */
{
	if (EG(prev_exception)) {
		if (EG(exception)) {
			zend_exception_set_previous(EG(exception), EG(prev_exception) TSRMLS_CC);
		} else {
			EG(exception) = EG(prev_exception);
		}
		EG(prev_exception) = NULL;
	}
}
/* }}} */

void zend_throw_exception_internal(zval *exception TSRMLS_DC) /* {{{ */
{
#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_THROWN_ENABLED()) {
		zend_string *classname;

		if (exception != NULL) {
			classname = zend_get_object_classname(Z_OBJ_P(exception) TSRMLS_CC);
			DTRACE_EXCEPTION_THROWN(classname->val);
		} else {
			DTRACE_EXCEPTION_THROWN(NULL);
		}
	}
#endif /* HAVE_DTRACE */

	if (exception != NULL) {
		zend_object *previous = EG(exception);
		zend_exception_set_previous(Z_OBJ_P(exception), EG(exception) TSRMLS_CC);
		EG(exception) = Z_OBJ_P(exception);
		if (previous) {
			return;
		}
	}
	if (!EG(current_execute_data)) {
		if(EG(exception)) {
			zend_exception_error(EG(exception), E_ERROR TSRMLS_CC);
		}
		zend_error(E_ERROR, "Exception thrown without a stack frame");
	}

	if (zend_throw_exception_hook) {
		zend_throw_exception_hook(exception TSRMLS_CC);
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

ZEND_API void zend_clear_exception(TSRMLS_D) /* {{{ */
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

static zend_object *zend_default_exception_new_ex(zend_class_entry *class_type, int skip_top_traces TSRMLS_DC) /* {{{ */
{
	zval obj;
	zend_object *object;
	zval trace;

	Z_OBJ(obj) = object = zend_objects_new(class_type TSRMLS_CC);
	Z_OBJ_HT(obj) = &default_exception_handlers;

	object_properties_init(object, class_type);

	zend_fetch_debug_backtrace(&trace, skip_top_traces, 0, 0 TSRMLS_CC);
	Z_SET_REFCOUNT(trace, 0);

	zend_update_property_string(default_exception_ce, &obj, "file", sizeof("file")-1, zend_get_executed_filename(TSRMLS_C) TSRMLS_CC);
	zend_update_property_long(default_exception_ce, &obj, "line", sizeof("line")-1, zend_get_executed_lineno(TSRMLS_C) TSRMLS_CC);
	zend_update_property(default_exception_ce, &obj, "trace", sizeof("trace")-1, &trace TSRMLS_CC);

	return object;
}
/* }}} */

static zend_object *zend_default_exception_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	return zend_default_exception_new_ex(class_type, 0 TSRMLS_CC);
}
/* }}} */

static zend_object *zend_error_exception_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	return zend_default_exception_new_ex(class_type, 2 TSRMLS_CC);
}
/* }}} */

/* {{{ proto Exception Exception::__clone()
   Clone the exception object */
ZEND_METHOD(exception, __clone)
{
	/* Should never be executable */
	zend_throw_exception(NULL, "Cannot clone object using __clone()", 0 TSRMLS_CC);
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

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|SlO!", &message, &code, &previous, default_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Exception([string $exception [, long $code [, Exception $previous = NULL]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_str(default_exception_ce, object, "message", sizeof("message")-1, message TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(default_exception_ce, object, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	if (previous) {
		zend_update_property(default_exception_ce, object, "previous", sizeof("previous")-1, previous TSRMLS_CC);
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

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|sllslO!", &message, &message_len, &code, &severity, &filename, &filename_len, &lineno, &previous, default_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for ErrorException([string $exception [, long $code, [ long $severity, [ string $filename, [ long $lineno  [, Exception $previous = NULL]]]]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_string(default_exception_ce, object, "message", sizeof("message")-1, message TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(default_exception_ce, object, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	if (previous) {
		zend_update_property(default_exception_ce, object, "previous", sizeof("previous")-1, previous TSRMLS_CC);
	}

	zend_update_property_long(default_exception_ce, object, "severity", sizeof("severity")-1, severity TSRMLS_CC);

	if (argc >= 4) {
	    zend_update_property_string(default_exception_ce, object, "file", sizeof("file")-1, filename TSRMLS_CC);
    	if (argc < 5) {
    	    lineno = 0; /* invalidate lineno */
    	}
    	zend_update_property_long(default_exception_ce, object, "line", sizeof("line")-1, lineno TSRMLS_CC);
	}
}
/* }}} */

#define DEFAULT_0_PARAMS \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	}

static void _default_exception_get_entry(zval *object, char *name, int name_len, zval *return_value TSRMLS_DC) /* {{{ */
{
	zval *value;

	value = zend_read_property(default_exception_ce, object, name, name_len, 0 TSRMLS_CC);
	ZVAL_COPY(return_value, value);
}
/* }}} */

/* {{{ proto string Exception::getFile()
   Get the file in which the exception occurred */
ZEND_METHOD(exception, getFile)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "file", sizeof("file")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto int Exception::getLine()
   Get the line in which the exception occurred */
ZEND_METHOD(exception, getLine)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "line", sizeof("line")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto string Exception::getMessage()
   Get the exception message */
ZEND_METHOD(exception, getMessage)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "message", sizeof("message")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto int Exception::getCode()
   Get the exception code */
ZEND_METHOD(exception, getCode)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "code", sizeof("code")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array Exception::getTrace()
   Get the stack trace for the location in which the exception occurred */
ZEND_METHOD(exception, getTrace)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "trace", sizeof("trace")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto int ErrorException::getSeverity()
   Get the exception severity */
ZEND_METHOD(error_exception, getSeverity)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "severity", sizeof("severity")-1, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ gettraceasstring() macros */
#define TRACE_APPEND_CHR(chr)                                        \
	str = zend_string_realloc(str, str->len + 1, 0);                         \
    str->val[str->len - 1] = chr

#define TRACE_APPEND_STRL(v, l)                                      \
	{                                                                \
		str = zend_string_realloc(str, str->len + (l), 0);                   \
		memcpy(str->val + str->len - (l), (v), (l));                 \
	}

#define TRACE_APPEND_STR(v)                                          \
	TRACE_APPEND_STRL((v), sizeof((v))-1)

#define TRACE_APPEND_KEY(key) do {                                          \
		tmp = zend_hash_str_find(ht, key, sizeof(key)-1);                   \
		if (tmp) {                                                          \
			if (Z_TYPE_P(tmp) != IS_STRING) {                               \
				zend_error(E_WARNING, "Value for %s is no string", key);    \
				TRACE_APPEND_STR("[unknown]");                              \
			} else {                                                        \
				TRACE_APPEND_STRL(Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));        \
			}                                                               \
		} \
	} while (0)


#define TRACE_ARG_APPEND(vallen) do { \
		size_t len = str->len; \
		str = zend_string_realloc(str, len + vallen, 0); \
		memmove(str->val + len - l_added + 1 + vallen, str->val + len - l_added + 1, l_added); \
	} while (0)

/* }}} */

static void _build_trace_args(zval *arg, zend_string **str_ptr TSRMLS_DC) /* {{{ */
{
	zend_string *str = *str_ptr;

	/* the trivial way would be to do:
	 * convert_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	ZVAL_DEREF(arg);
	switch (Z_TYPE_P(arg)) {
		case IS_NULL:
			TRACE_APPEND_STR("NULL, ");
			break;
		case IS_STRING: {
			int l_added;
			TRACE_APPEND_CHR('\'');
			if (Z_STRLEN_P(arg) > 15) {
				TRACE_APPEND_STRL(Z_STRVAL_P(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_STRLEN_P(arg);
				TRACE_APPEND_STRL(Z_STRVAL_P(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			while (--l_added) {
				unsigned char chr = str->val[str->len - l_added];
				if (chr < 32 || chr == '\\' || chr > 126) {
					str->val[str->len - l_added] = '\\';

					switch (chr) {
						case '\n':
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = 'n';
							break;
						case '\r':
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = 'r';
							break;
						case '\t':
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = 't';
							break;
						case '\f':
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = 'f';
							break;
						case '\v':
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = 'v';
							break;
#ifndef PHP_WIN32
						case '\e':
#else
						case VK_ESCAPE:
#endif
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = 'e';
							break;
						case '\\':
							TRACE_ARG_APPEND(1);
							str->val[str->len - l_added] = '\\';
							break;
						default:
							TRACE_ARG_APPEND(3);
							str->val[str->len - l_added - 2] = 'x';
							if ((chr >> 4) < 10) {
								str->val[str->len - l_added - 1] = (chr >> 4) + '0';
							} else {
								str->val[str->len - l_added - 1] = (chr >> 4) + 'A' - 10;
							}
							if (chr % 16 < 10) {
								str->val[str->len - l_added] = chr % 16 + '0';
							} else {
								str->val[str->len - l_added] = chr % 16 + 'A' - 10;
							}
					}
				}
			}
			break;
		}
		case IS_FALSE:
			TRACE_APPEND_STR("false, ");
			break;
		case IS_TRUE:
			TRACE_APPEND_STR("true, ");
			break;
		case IS_RESOURCE: {
			zend_long lval = Z_RES_HANDLE_P(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, ZEND_LONG_FMT, lval);  /* SAFE */
			TRACE_APPEND_STR("Resource id #");
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_LONG: {
			zend_long lval = Z_LVAL_P(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, ZEND_LONG_FMT, lval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_DOUBLE: {
			double dval = Z_DVAL_P(arg);
			char *s_tmp;
			int l_tmp;

			s_tmp = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			l_tmp = zend_sprintf(s_tmp, "%.*G", (int) EG(precision), dval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			efree(s_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_ARRAY:
			TRACE_APPEND_STR("Array, ");
			break;
		case IS_OBJECT: {
			zend_string *class_name;

			TRACE_APPEND_STR("Object(");

			class_name = zend_get_object_classname(Z_OBJ_P(arg) TSRMLS_CC);

			TRACE_APPEND_STRL(class_name->val, class_name->len);
			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	*str_ptr = str;
}
/* }}} */

static void _build_trace_string(zval *frame, zend_ulong index, zend_string **str_ptr, int *num TSRMLS_DC) /* {{{ */
{
	char *s_tmp;
	int len;
	zend_long line;
	HashTable *ht;
	zval *file, *tmp;
	zend_string *str = *str_ptr;

	if (Z_TYPE_P(frame) != IS_ARRAY) {
		zend_error(E_WARNING, "Expected array for frame %pu", index);
		return;
	}

	ht = Z_ARRVAL_P(frame);
	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	len = sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, len);
	efree(s_tmp);
	file = zend_hash_str_find(ht, "file", sizeof("file")-1);
	if (file) {
		if (Z_TYPE_P(file) != IS_STRING) {
			zend_error(E_WARNING, "Function name is no string");
			TRACE_APPEND_STR("[unknown function]");
		} else{
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
			s_tmp = emalloc(Z_STRLEN_P(file) + MAX_LENGTH_OF_LONG + 4 + 1);
			len = sprintf(s_tmp, "%s(" ZEND_LONG_FMT "): ", Z_STRVAL_P(file), line);
			TRACE_APPEND_STRL(s_tmp, len);
			efree(s_tmp);
		}
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	tmp = zend_hash_str_find(ht, "args", sizeof("args")-1);
	if (tmp) {
		if (Z_TYPE_P(tmp) == IS_ARRAY) {
			int last_len = str->len;
			zval *arg;
			
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tmp), arg) {
				_build_trace_args(arg, &str TSRMLS_CC);
			} ZEND_HASH_FOREACH_END();

			if (last_len != str->len) {
				str->len -= 2; /* remove last ', ' */
			}
		} else {
			zend_error(E_WARNING, "args element is no array");
		}
	}
	TRACE_APPEND_STR(")\n");
	*str_ptr = str;
}
/* }}} */

/* {{{ proto string Exception::getTraceAsString()
   Obtain the backtrace for the exception as a string (instead of an array) */
ZEND_METHOD(exception, getTraceAsString)
{
	zval *trace, *frame;
	zend_ulong index;
	zend_string *str;
	int num = 0, len;
	char s_tmp[MAX_LENGTH_OF_LONG + 7 + 1 + 1];

	DEFAULT_0_PARAMS;
	
	str = zend_string_alloc(0, 0);

	trace = zend_read_property(default_exception_ce, getThis(), "trace", sizeof("trace")-1, 1 TSRMLS_CC);
	ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(trace), index, frame) {
		_build_trace_string(frame, index, &str, &num TSRMLS_CC);
	} ZEND_HASH_FOREACH_END();

	len = sprintf(s_tmp, "#%d {main}", num);
	TRACE_APPEND_STRL(s_tmp, len);

	str->val[str->len] = '\0';	

	RETURN_NEW_STR(str); 
}
/* }}} */

/* {{{ proto string Exception::getPrevious()
   Return previous Exception or NULL. */
ZEND_METHOD(exception, getPrevious)
{
	zval *previous;

	DEFAULT_0_PARAMS;

	previous = zend_read_property(default_exception_ce, getThis(), "previous", sizeof("previous")-1, 1 TSRMLS_CC);
	RETURN_ZVAL(previous, 1, 0);
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
	zval message, file, line, trace, *exception;
	zend_string *str, *prev_str;
	zend_fcall_info fci;
	zval fname;
	
	DEFAULT_0_PARAMS;
	
	str = STR_EMPTY_ALLOC();

	exception = getThis();
	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1);

	while (exception && Z_TYPE_P(exception) == IS_OBJECT) {
		prev_str = str;
		_default_exception_get_entry(exception, "message", sizeof("message")-1, &message TSRMLS_CC);
		_default_exception_get_entry(exception, "file", sizeof("file")-1, &file TSRMLS_CC);
		_default_exception_get_entry(exception, "line", sizeof("line")-1, &line TSRMLS_CC);

		convert_to_string_ex(&message);
		convert_to_string_ex(&file);
		convert_to_long_ex(&line);

		fci.size = sizeof(fci);
		fci.function_table = &Z_OBJCE_P(exception)->function_table;
		ZVAL_COPY_VALUE(&fci.function_name, &fname);
		fci.symbol_table = NULL;
		fci.object = Z_OBJ_P(exception);
		fci.retval = &trace;
		fci.param_count = 0;
		fci.params = NULL;
		fci.no_separation = 1;

		zend_call_function(&fci, NULL TSRMLS_CC);

		if (Z_TYPE(trace) != IS_STRING) {
			zval_ptr_dtor(&trace);
			ZVAL_UNDEF(&trace);
		}

		if (Z_STRLEN(message) > 0) {
			str = zend_strpprintf(0, "exception '%s' with message '%s' in %s:%ld\nStack trace:\n%s%s%s",
					Z_OBJCE_P(exception)->name->val, Z_STRVAL(message), Z_STRVAL(file), Z_LVAL(line),
					(Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace)) ? Z_STRVAL(trace) : "#0 {main}\n",
					prev_str->len ? "\n\nNext " : "", prev_str->val);
		} else {
			str = zend_strpprintf(0, "exception '%s' in %s:%ld\nStack trace:\n%s%s%s",
					Z_OBJCE_P(exception)->name->val, Z_STRVAL(file), Z_LVAL(line),
					(Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace)) ? Z_STRVAL(trace) : "#0 {main}\n",
					prev_str->len ? "\n\nNext " : "", prev_str->val);
		}
		zend_string_release(prev_str);
		zval_dtor(&message);
		zval_dtor(&file);
		zval_dtor(&line);

		exception = zend_read_property(default_exception_ce, exception, "previous", sizeof("previous")-1, 0 TSRMLS_CC);

		zval_ptr_dtor(&trace);

	}
	zval_dtor(&fname);

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	zend_update_property_str(default_exception_ce, getThis(), "string", sizeof("string")-1, str TSRMLS_CC);

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

const static zend_function_entry default_exception_functions[] = {
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
	{NULL, NULL, NULL}
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
	{NULL, NULL, NULL}
};
/* }}} */

void zend_register_default_exception(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Exception", default_exception_functions);
	default_exception_ce = zend_register_internal_class(&ce TSRMLS_CC);
	default_exception_ce->create_object = zend_default_exception_new;
	memcpy(&default_exception_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	default_exception_handlers.clone_obj = NULL;

	zend_declare_property_string(default_exception_ce, "message", sizeof("message")-1, "", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(default_exception_ce, "string", sizeof("string")-1, "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(default_exception_ce, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "file", sizeof("file")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "line", sizeof("line")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "trace", sizeof("trace")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(default_exception_ce, "previous", sizeof("previous")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "ErrorException", error_exception_functions);
	error_exception_ce = zend_register_internal_class_ex(&ce, default_exception_ce TSRMLS_CC);
	error_exception_ce->create_object = zend_error_exception_new;
	zend_declare_property_long(error_exception_ce, "severity", sizeof("severity")-1, E_ERROR, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

ZEND_API zend_class_entry *zend_exception_get_default(TSRMLS_D) /* {{{ */
{
	return default_exception_ce;
}
/* }}} */

ZEND_API zend_class_entry *zend_get_error_exception(TSRMLS_D) /* {{{ */
{
	return error_exception_ce;
}
/* }}} */

ZEND_API zend_object *zend_throw_exception(zend_class_entry *exception_ce, const char *message, zend_long code TSRMLS_DC) /* {{{ */
{
	zval ex;

	if (exception_ce) {
		if (!instanceof_function(exception_ce, default_exception_ce TSRMLS_CC)) {
			zend_error(E_NOTICE, "Exceptions must be derived from the Exception base class");
			exception_ce = default_exception_ce;
		}
	} else {
		exception_ce = default_exception_ce;
	}
	object_init_ex(&ex, exception_ce);


	if (message) {
		zend_update_property_string(default_exception_ce, &ex, "message", sizeof("message")-1, message TSRMLS_CC);
	}
	if (code) {
		zend_update_property_long(default_exception_ce, &ex, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	zend_throw_exception_internal(&ex TSRMLS_CC);
	return Z_OBJ(ex);
}
/* }}} */

ZEND_API zend_object *zend_throw_exception_ex(zend_class_entry *exception_ce, zend_long code TSRMLS_DC, const char *format, ...) /* {{{ */
{
	va_list arg;
	char *message;
	zend_object *obj;

	va_start(arg, format);
	zend_vspprintf(&message, 0, format, arg);
	va_end(arg);
	obj = zend_throw_exception(exception_ce, message, code TSRMLS_CC);
	efree(message);
	return obj;
}
/* }}} */

ZEND_API zend_object *zend_throw_error_exception(zend_class_entry *exception_ce, const char *message, zend_long code, int severity TSRMLS_DC) /* {{{ */
{
	zval ex;
	zend_object *obj = zend_throw_exception(exception_ce, message, code TSRMLS_CC);
	ZVAL_OBJ(&ex, obj);
	zend_update_property_long(default_exception_ce, &ex, "severity", sizeof("severity")-1, severity TSRMLS_CC);
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

/* This function doesn't return if it uses E_ERROR */
ZEND_API void zend_exception_error(zend_object *ex, int severity TSRMLS_DC) /* {{{ */
{
	zval exception;
	zend_class_entry *ce_exception;
	
	ZVAL_OBJ(&exception, ex);
	ce_exception = Z_OBJCE(exception);
	if (instanceof_function(ce_exception, default_exception_ce TSRMLS_CC)) {
		zval tmp, *str, *file, *line;

		EG(exception) = NULL;

		zend_call_method_with_0_params(&exception, ce_exception, NULL, "__tostring", &tmp);
		if (!EG(exception)) {
			if (Z_TYPE(tmp) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ce_exception->name->val);
			} else {
				zend_update_property_string(default_exception_ce, &exception, "string", sizeof("string")-1, EG(exception) ? ce_exception->name->val : Z_STRVAL(tmp) TSRMLS_CC);
			}
		}
		zval_ptr_dtor(&tmp);

		if (EG(exception)) {
			zval zv;

			ZVAL_OBJ(&zv, EG(exception));
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, default_exception_ce TSRMLS_CC)) {
				file = zend_read_property(default_exception_ce, &zv, "file", sizeof("file")-1, 1 TSRMLS_CC);
				line = zend_read_property(default_exception_ce, &zv, "line", sizeof("line")-1, 1 TSRMLS_CC);

				convert_to_string_ex(file);
				file = (Z_STRLEN_P(file) > 0) ? file : NULL;
				line = (Z_TYPE_P(line) == IS_LONG) ? line : NULL;
			} else {
				file = NULL;
				line = NULL;
			}
			zend_error_va(E_WARNING, file ? Z_STRVAL_P(file) : NULL, line ? Z_LVAL_P(line) : 0, "Uncaught %s in exception handling during call to %s::__tostring()", Z_OBJCE(zv)->name->val, ce_exception->name->val);
		}

		str = zend_read_property(default_exception_ce, &exception, "string", sizeof("string")-1, 1 TSRMLS_CC);
		file = zend_read_property(default_exception_ce, &exception, "file", sizeof("file")-1, 1 TSRMLS_CC);
		line = zend_read_property(default_exception_ce, &exception, "line", sizeof("line")-1, 1 TSRMLS_CC);

		convert_to_string_ex(str);
		convert_to_string_ex(file);
		convert_to_long_ex(line);

		zend_error_va(severity, (Z_STRLEN_P(file) > 0) ? Z_STRVAL_P(file) : NULL, Z_LVAL_P(line), "Uncaught %s\n  thrown", Z_STRVAL_P(str));
	} else {
		zend_error(severity, "Uncaught exception '%s'", ce_exception->name->val);
	}
}
/* }}} */

ZEND_API void zend_throw_exception_object(zval *exception TSRMLS_DC) /* {{{ */
{
	zend_class_entry *exception_ce;

	if (exception == NULL || Z_TYPE_P(exception) != IS_OBJECT) {
		zend_error(E_ERROR, "Need to supply an object when throwing an exception");
	}

	exception_ce = Z_OBJCE_P(exception);

	if (!exception_ce || !instanceof_function(exception_ce, default_exception_ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Exceptions must be valid objects derived from the Exception base class");
	}
	zend_throw_exception_internal(exception TSRMLS_CC);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
