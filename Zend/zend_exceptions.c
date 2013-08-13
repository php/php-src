/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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

void zend_exception_set_previous(zval *exception, zval *add_previous TSRMLS_DC)
{
	zval *previous;

	if (exception == add_previous || !add_previous || !exception) {
		return;
	}
	if (Z_TYPE_P(add_previous) != IS_OBJECT && !instanceof_function(Z_OBJCE_P(add_previous), default_exception_ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot set non exception as previous exception");
		return;
	}
	while (exception && exception != add_previous && Z_OBJ_HANDLE_P(exception) != Z_OBJ_HANDLE_P(add_previous)) {
		previous = zend_read_property(default_exception_ce, exception, "previous", sizeof("previous")-1, 1 TSRMLS_CC);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property(default_exception_ce, exception, "previous", sizeof("previous")-1, add_previous TSRMLS_CC);
			Z_DELREF_P(add_previous);
			return;
		}
		exception = previous;
	}
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
		char *classname;
		int name_len;

		if (exception != NULL) {
			zend_get_object_classname(exception, &classname, &name_len TSRMLS_CC);
			DTRACE_EXCEPTION_THROWN(classname);
		} else {
			DTRACE_EXCEPTION_THROWN(NULL);
		}
	}
#endif /* HAVE_DTRACE */

	if (exception != NULL) {
		zval *previous = EG(exception);
		zend_exception_set_previous(exception, EG(exception) TSRMLS_CC);
		EG(exception) = exception;
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

	if (EG(current_execute_data)->opline == NULL ||
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
		zval_ptr_dtor(&EG(prev_exception));
		EG(prev_exception) = NULL;
	}
	if (!EG(exception)) {
		return;
	}
	zval_ptr_dtor(&EG(exception));
	EG(exception) = NULL;
	EG(current_execute_data)->opline = EG(opline_before_exception);
#if ZEND_DEBUG
	EG(opline_before_exception) = NULL;
#endif
}
/* }}} */

static zend_object_value zend_default_exception_new_ex(zend_class_entry *class_type, int skip_top_traces TSRMLS_DC) /* {{{ */
{
	zval obj;
	zend_object *object;
	zval *trace;

	Z_OBJVAL(obj) = zend_objects_new(&object, class_type TSRMLS_CC);
	Z_OBJ_HT(obj) = &default_exception_handlers;

	object_properties_init(object, class_type);

	ALLOC_ZVAL(trace);
	Z_UNSET_ISREF_P(trace);
	Z_SET_REFCOUNT_P(trace, 0);
	zend_fetch_debug_backtrace(trace, skip_top_traces, 0, 0 TSRMLS_CC);

	zend_update_property_string(default_exception_ce, &obj, "file", sizeof("file")-1, zend_get_executed_filename(TSRMLS_C) TSRMLS_CC);
	zend_update_property_long(default_exception_ce, &obj, "line", sizeof("line")-1, zend_get_executed_lineno(TSRMLS_C) TSRMLS_CC);
	zend_update_property(default_exception_ce, &obj, "trace", sizeof("trace")-1, trace TSRMLS_CC);

	return Z_OBJVAL(obj);
}
/* }}} */

static zend_object_value zend_default_exception_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	return zend_default_exception_new_ex(class_type, 0 TSRMLS_CC);
}
/* }}} */

static zend_object_value zend_error_exception_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
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
	char  *message = NULL;
	long   code = 0;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS(), message_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|slO!", &message, &message_len, &code, &previous, default_exception_ce) == FAILURE) {
		zend_error(E_ERROR, "Wrong parameters for Exception([string $exception [, long $code [, Exception $previous = NULL]]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_stringl(default_exception_ce, object, "message", sizeof("message")-1, message, message_len TSRMLS_CC);
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
	long   code = 0, severity = E_ERROR, lineno;
	zval  *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS(), message_len, filename_len;

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

	*return_value = *value;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
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
#define TRACE_APPEND_CHR(chr)                                            \
	*str = (char*)erealloc(*str, *len + 1 + 1);                          \
	(*str)[(*len)++] = chr

#define TRACE_APPEND_STRL(val, vallen)                                   \
	{                                                                    \
		int l = vallen;                                                  \
		*str = (char*)erealloc(*str, *len + l + 1);                      \
		memcpy((*str) + *len, val, l);                                   \
		*len += l;                                                       \
	}

#define TRACE_APPEND_STR(val)                                            \
	TRACE_APPEND_STRL(val, sizeof(val)-1)

#define TRACE_APPEND_KEY(key)                                                   \
	if (zend_hash_find(ht, key, sizeof(key), (void**)&tmp) == SUCCESS) {    \
		if (Z_TYPE_PP(tmp) != IS_STRING) {                              \
			zend_error(E_WARNING, "Value for %s is no string", key); \
			TRACE_APPEND_STR("[unknown]");                          \
		} else {                                                        \
			TRACE_APPEND_STRL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));  \
		}                                                               \
	}

/* }}} */

static int _build_trace_args(zval **arg TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char **str;
	int *len;

	str = va_arg(args, char**);
	len = va_arg(args, int*);

	/* the trivial way would be to do:
	 * conver_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	switch (Z_TYPE_PP(arg)) {
		case IS_NULL:
			TRACE_APPEND_STR("NULL, ");
			break;
		case IS_STRING: {
			int l_added;
			TRACE_APPEND_CHR('\'');
			if (Z_STRLEN_PP(arg) > 15) {
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_STRLEN_PP(arg);
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			while (--l_added) {
				if ((*str)[*len - l_added] < 32) {
					(*str)[*len - l_added] = '?';
				}
			}
			break;
		}
		case IS_BOOL:
			if (Z_LVAL_PP(arg)) {
				TRACE_APPEND_STR("true, ");
			} else {
				TRACE_APPEND_STR("false, ");
			}
			break;
		case IS_RESOURCE:
			TRACE_APPEND_STR("Resource id #");
			/* break; */
		case IS_LONG: {
			long lval = Z_LVAL_PP(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, "%ld", lval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_DOUBLE: {
			double dval = Z_DVAL_PP(arg);
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
			const char *class_name;
			zend_uint class_name_len;
			int dup;

			TRACE_APPEND_STR("Object(");

			dup = zend_get_object_classname(*arg, &class_name, &class_name_len TSRMLS_CC);

			TRACE_APPEND_STRL(class_name, class_name_len);
			if(!dup) {
				efree((char*)class_name);
			}

			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int _build_trace_string(zval **frame TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;

	if (Z_TYPE_PP(frame) != IS_ARRAY) {
		zend_error(E_WARNING, "Expected array for frame %lu", hash_key->h);
		return ZEND_HASH_APPLY_KEEP;
	}

	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);
	if (zend_hash_find(ht, "file", sizeof("file"), (void**)&file) == SUCCESS) {
		if (Z_TYPE_PP(file) != IS_STRING) {
			zend_error(E_WARNING, "Function name is no string");
			TRACE_APPEND_STR("[unknown function]");
		} else{
			if (zend_hash_find(ht, "line", sizeof("line"), (void**)&tmp) == SUCCESS) {
				if (Z_TYPE_PP(tmp) == IS_LONG) {
					line = Z_LVAL_PP(tmp);
				} else {
					zend_error(E_WARNING, "Line is no long");
					line = 0;
				}
			} else {
				line = 0;
			}
			s_tmp = emalloc(Z_STRLEN_PP(file) + MAX_LENGTH_OF_LONG + 4 + 1);
			sprintf(s_tmp, "%s(%ld): ", Z_STRVAL_PP(file), line);
			TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
			efree(s_tmp);
		}
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	if (zend_hash_find(ht, "args", sizeof("args"), (void**)&tmp) == SUCCESS) {
		if (Z_TYPE_PP(tmp) == IS_ARRAY) {
			int last_len = *len;
			zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp) TSRMLS_CC, (apply_func_args_t)_build_trace_args, 2, str, len);
			if (last_len != *len) {
				*len -= 2; /* remove last ', ' */
			}
		} else {
			zend_error(E_WARNING, "args element is no array");
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto string Exception::getTraceAsString()
   Obtain the backtrace for the exception as a string (instead of an array) */
ZEND_METHOD(exception, getTraceAsString)
{
	zval *trace;
	char *res, **str, *s_tmp;
	int res_len = 0, *len = &res_len, num = 0;

	DEFAULT_0_PARAMS;
	
	res = estrdup("");
	str = &res;

	trace = zend_read_property(default_exception_ce, getThis(), "trace", sizeof("trace")-1, 1 TSRMLS_CC);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(trace) TSRMLS_CC, (apply_func_args_t)_build_trace_string, 3, str, len, &num);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 7 + 1);
	sprintf(s_tmp, "#%d {main}", num);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);

	res[res_len] = '\0';	
	RETURN_STRINGL(res, res_len, 0); 
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
}

int zend_spprintf(char **message, int max_len, char *format, ...) /* {{{ */
{
	va_list arg;
	int len;

	va_start(arg, format);
	len = zend_vspprintf(message, max_len, format, arg);
	va_end(arg);
	return len;
}
/* }}} */

/* {{{ proto string Exception::__toString()
   Obtain the string representation of the Exception object */
ZEND_METHOD(exception, __toString)
{
	zval message, file, line, *trace, *exception;
	char *str, *prev_str;
	int len = 0;
	zend_fcall_info fci;
	zval fname;
	
	DEFAULT_0_PARAMS;
	
	str = estrndup("", 0);

	exception = getThis();
	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1, 1);

	while (exception && Z_TYPE_P(exception) == IS_OBJECT) {
		prev_str = str;
		_default_exception_get_entry(exception, "message", sizeof("message")-1, &message TSRMLS_CC);
		_default_exception_get_entry(exception, "file", sizeof("file")-1, &file TSRMLS_CC);
		_default_exception_get_entry(exception, "line", sizeof("line")-1, &line TSRMLS_CC);

		convert_to_string(&message);
		convert_to_string(&file);
		convert_to_long(&line);

		fci.size = sizeof(fci);
		fci.function_table = &Z_OBJCE_P(exception)->function_table;
		fci.function_name = &fname;
		fci.symbol_table = NULL;
		fci.object_ptr = exception;
		fci.retval_ptr_ptr = &trace;
		fci.param_count = 0;
		fci.params = NULL;
		fci.no_separation = 1;

		zend_call_function(&fci, NULL TSRMLS_CC);

		if (Z_TYPE_P(trace) != IS_STRING) {
			zval_ptr_dtor(&trace);
			trace = NULL;
		}

		if (Z_STRLEN(message) > 0) {
			len = zend_spprintf(&str, 0, "exception '%s' with message '%s' in %s:%ld\nStack trace:\n%s%s%s",
								Z_OBJCE_P(exception)->name, Z_STRVAL(message), Z_STRVAL(file), Z_LVAL(line),
								(trace && Z_STRLEN_P(trace)) ? Z_STRVAL_P(trace) : "#0 {main}\n",
								len ? "\n\nNext " : "", prev_str);
		} else {
			len = zend_spprintf(&str, 0, "exception '%s' in %s:%ld\nStack trace:\n%s%s%s",
								Z_OBJCE_P(exception)->name, Z_STRVAL(file), Z_LVAL(line),
								(trace && Z_STRLEN_P(trace)) ? Z_STRVAL_P(trace) : "#0 {main}\n",
								len ? "\n\nNext " : "", prev_str);
		}
		efree(prev_str);
		zval_dtor(&message);
		zval_dtor(&file);
		zval_dtor(&line);

		exception = zend_read_property(default_exception_ce, exception, "previous", sizeof("previous")-1, 0 TSRMLS_CC);

		if (trace) {
			zval_ptr_dtor(&trace);
		}

	}
	zval_dtor(&fname);

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	zend_update_property_string(default_exception_ce, getThis(), "string", sizeof("string")-1, str TSRMLS_CC);

	RETURN_STRINGL(str, len, 0);
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
	error_exception_ce = zend_register_internal_class_ex(&ce, default_exception_ce, NULL TSRMLS_CC);
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

ZEND_API zval * zend_throw_exception(zend_class_entry *exception_ce, char *message, long code TSRMLS_DC) /* {{{ */
{
	zval *ex;

	MAKE_STD_ZVAL(ex);
	if (exception_ce) {
		if (!instanceof_function(exception_ce, default_exception_ce TSRMLS_CC)) {
			zend_error(E_NOTICE, "Exceptions must be derived from the Exception base class");
			exception_ce = default_exception_ce;
		}
	} else {
		exception_ce = default_exception_ce;
	}
	object_init_ex(ex, exception_ce);


	if (message) {
		zend_update_property_string(default_exception_ce, ex, "message", sizeof("message")-1, message TSRMLS_CC);
	}
	if (code) {
		zend_update_property_long(default_exception_ce, ex, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	zend_throw_exception_internal(ex TSRMLS_CC);
	return ex;
}
/* }}} */

ZEND_API zval * zend_throw_exception_ex(zend_class_entry *exception_ce, long code TSRMLS_DC, char *format, ...) /* {{{ */
{
	va_list arg;
	char *message;
	zval *zexception;

	va_start(arg, format);
	zend_vspprintf(&message, 0, format, arg);
	va_end(arg);
	zexception = zend_throw_exception(exception_ce, message, code TSRMLS_CC);
	efree(message);
	return zexception;
}
/* }}} */

ZEND_API zval * zend_throw_error_exception(zend_class_entry *exception_ce, char *message, long code, int severity TSRMLS_DC) /* {{{ */
{
	zval *ex = zend_throw_exception(exception_ce, message, code TSRMLS_CC);
	zend_update_property_long(default_exception_ce, ex, "severity", sizeof("severity")-1, severity TSRMLS_CC);
	return ex;
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
ZEND_API void zend_exception_error(zval *exception, int severity TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce_exception = Z_OBJCE_P(exception);
	if (instanceof_function(ce_exception, default_exception_ce TSRMLS_CC)) {
		zval *str, *file, *line;

		EG(exception) = NULL;

		zend_call_method_with_0_params(&exception, ce_exception, NULL, "__tostring", &str);
		if (!EG(exception)) {
			if (Z_TYPE_P(str) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ce_exception->name);
			} else {
				zend_update_property_string(default_exception_ce, exception, "string", sizeof("string")-1, EG(exception) ? ce_exception->name : Z_STRVAL_P(str) TSRMLS_CC);
			}
		}
		zval_ptr_dtor(&str);

		if (EG(exception)) {
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, default_exception_ce TSRMLS_CC)) {
				file = zend_read_property(default_exception_ce, EG(exception), "file", sizeof("file")-1, 1 TSRMLS_CC);
				line = zend_read_property(default_exception_ce, EG(exception), "line", sizeof("line")-1, 1 TSRMLS_CC);

				convert_to_string(file);
				file = (Z_STRLEN_P(file) > 0) ? file : NULL;
				line = (Z_TYPE_P(line) == IS_LONG) ? line : NULL;
			} else {
				file = NULL;
				line = NULL;
			}
			zend_error_va(E_WARNING, file ? Z_STRVAL_P(file) : NULL, line ? Z_LVAL_P(line) : 0, "Uncaught %s in exception handling during call to %s::__tostring()", Z_OBJCE_P(EG(exception))->name, ce_exception->name);
		}

		str = zend_read_property(default_exception_ce, exception, "string", sizeof("string")-1, 1 TSRMLS_CC);
		file = zend_read_property(default_exception_ce, exception, "file", sizeof("file")-1, 1 TSRMLS_CC);
		line = zend_read_property(default_exception_ce, exception, "line", sizeof("line")-1, 1 TSRMLS_CC);

		convert_to_string(str);
		convert_to_string(file);
		convert_to_long(line);

		zend_error_va(severity, (Z_STRLEN_P(file) > 0) ? Z_STRVAL_P(file) : NULL, Z_LVAL_P(line), "Uncaught %s\n  thrown", Z_STRVAL_P(str));
	} else {
		zend_error(severity, "Uncaught exception '%s'", ce_exception->name);
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
