/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_API.h"
#include "zend_reflection_api.h"
#include "zend_builtin_functions.h"
#include "zend_interfaces.h"

static zend_class_entry *default_exception_ptr;
static zend_object_handlers default_exception_handlers;
ZEND_API void zend_throw_exception(zend_class_entry *exception_ce, char *message, long code TSRMLS_DC);

static zend_object_value zend_default_exception_new(zend_class_entry *class_type TSRMLS_DC)
{
	zval tmp, obj;
	zend_object *object;
	zval *trace;

	obj.value.obj = zend_objects_new(&object, class_type TSRMLS_CC);
	obj.value.obj.handlers = &default_exception_handlers;

	ALLOC_HASHTABLE(object->properties);
	zend_hash_init(object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(object->properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	ALLOC_ZVAL(trace);
	trace->is_ref = 0;
	trace->refcount = 0;
	zend_fetch_debug_backtrace(trace, 0 TSRMLS_CC);

	zend_update_property_string(class_type, &obj, "file", sizeof("file")-1, zend_get_executed_filename(TSRMLS_C) TSRMLS_CC);
	zend_update_property_long(class_type, &obj, "line", sizeof("line")-1, zend_get_executed_lineno(TSRMLS_C) TSRMLS_CC);
	zend_update_property(class_type, &obj, "trace", sizeof("trace")-1, trace TSRMLS_CC);

	return obj.value.obj;
}

ZEND_METHOD(exception, __clone)
{
	/* Should never be executable */
	zend_throw_exception(NULL, "Cannot clone object using __clone()", 0 TSRMLS_CC);
}

ZEND_METHOD(exception, __construct)
{
	char  *message = NULL;
	long   code = 0;
	zval  *object;
	int    argc = ZEND_NUM_ARGS(), message_len;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc TSRMLS_CC, "|sl", &message, &message_len, &code) == FAILURE) {
		zend_error(E_CORE_ERROR, "Wrong parameter count for exception([string $exception [, long $code ]])");
	}

	object = getThis();

	if (message) {
		zend_update_property_string(Z_OBJCE_P(object), object, "message", sizeof("message")-1, message TSRMLS_CC);
	}

	if (code) {
		zend_update_property_long(Z_OBJCE_P(object), object, "code", sizeof("code")-1, code TSRMLS_CC);
	}
}

#define DEFAULT_0_PARAMS \
	if (ZEND_NUM_ARGS() > 0) { \
		ZEND_WRONG_PARAM_COUNT(); \
	}

static void _default_exception_get_entry(zval *object, char *name, int name_len, zval *return_value TSRMLS_DC)
{
	zval *value;

	value = zend_read_property(Z_OBJCE_P(object), object, name, name_len, 0 TSRMLS_CC);

	*return_value = *value;
	zval_copy_ctor(return_value);
}

ZEND_METHOD(exception, getFile)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "file", sizeof("file")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getLine)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "line", sizeof("line")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getMessage)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "message", sizeof("message")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getCode)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "code", sizeof("code")-1, return_value TSRMLS_CC);
}

ZEND_METHOD(exception, getTrace)
{
	DEFAULT_0_PARAMS;

	_default_exception_get_entry(getThis(), "trace", sizeof("trace")-1, return_value TSRMLS_CC);
}

/* {{{ ZEND_METHOD(exception, gettraceasstring) */
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

#define TRACE_APPEND_KEY(key)                                            \
	if (zend_hash_find(ht, key, sizeof(key), (void**)&tmp) == SUCCESS) { \
	    TRACE_APPEND_STRL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));           \
	}

static int _build_trace_args(zval **arg, int num_args, va_list args, zend_hash_key *hash_key)
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
			TSRMLS_FETCH();

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
			TSRMLS_FETCH();

			TRACE_APPEND_STR("Object(");
			TRACE_APPEND_STRL(Z_OBJCE_PP(arg)->name, strlen(Z_OBJCE_PP(arg)->name));
			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;
}

static int _build_trace_string(zval **frame, int num_args, va_list args, zend_hash_key *hash_key)
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;

	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);
	if (zend_hash_find(ht, "file", sizeof("file"), (void**)&file) == SUCCESS) {
		if (zend_hash_find(ht, "line", sizeof("line"), (void**)&tmp) == SUCCESS) {
			line = Z_LVAL_PP(tmp);
		} else {
			line = 0;
		}
		s_tmp = emalloc(Z_STRLEN_PP(file) + MAX_LENGTH_OF_LONG + 2 + 1);
		sprintf(s_tmp, "%s(%ld): ", Z_STRVAL_PP(file), line);
		TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
		efree(s_tmp);
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	if (zend_hash_find(ht, "args", sizeof("args"), (void**)&tmp) == SUCCESS) {
		int last_len = *len;
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp), (apply_func_args_t)_build_trace_args, 2, str, len);
		if (last_len != *len) {
			*len -= 2; /* remove last ', ' */
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}

ZEND_METHOD(exception, getTraceAsString)
{
	zval *trace;
	char *res = estrdup(""), **str = &res, *s_tmp;
	int res_len = 0, *len = &res_len, num = 0;
	
	trace = zend_read_property(Z_OBJCE_P(getThis()), getThis(), "trace", sizeof("trace")-1, 1 TSRMLS_CC);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(trace), (apply_func_args_t)_build_trace_string, 3, str, len, &num);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 7 + 1);
	sprintf(s_tmp, "#%d {main}", num);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);

	res[res_len] = '\0';	
	RETURN_STRINGL(res, res_len, 0); 
}
/* }}} */

int zend_spprintf(char **message, int max_len, char *format, ...)
{
	va_list arg;
	int len;

	va_start(arg, format); 
	len = zend_vspprintf(message, max_len, format, arg);
	va_end(arg);
	return len;
}

ZEND_METHOD(exception, __toString)
{
	zval *message, *file, *line, *trace;
	char *str;
	int len;
	zend_fcall_info fci;
	zval fname;
	
	message = zend_read_property(Z_OBJCE_P(getThis()), getThis(), "message", sizeof("message")-1, 1 TSRMLS_CC);
	file = zend_read_property(Z_OBJCE_P(getThis()), getThis(), "file", sizeof("file")-1, 1 TSRMLS_CC);
	line = zend_read_property(Z_OBJCE_P(getThis()), getThis(), "line", sizeof("line")-1, 1 TSRMLS_CC);

	ZVAL_STRINGL(&fname, "gettraceasstring", sizeof("gettraceasstring")-1, 0);

	fci.size = sizeof(fci);
	fci.function_table = &Z_OBJCE_P(getThis())->function_table;
	fci.function_name = &fname;
	fci.symbol_table = NULL;
	fci.object_pp = &getThis();
	fci.retval_ptr_ptr = &trace;
	fci.param_count = 0;
	fci.params = NULL;
	fci.no_separation = 1;

	zend_call_function(&fci, NULL TSRMLS_CC);

	if (Z_STRLEN_P(message) > 0) {
		len = zend_spprintf(&str, 0, "exception '%s' with message '%s' in %s:%ld\nStack trace:\n%s", 
							Z_OBJCE_P(getThis())->name, Z_STRVAL_P(message), Z_STRVAL_P(file), Z_LVAL_P(line), 
							Z_STRLEN_P(trace) ? Z_STRVAL_P(trace) : "#0 {main}\n");
	} else {
		len = zend_spprintf(&str, 0, "exception '%s' in %s:%ld\nStack trace:\n%s", 
							Z_OBJCE_P(getThis())->name, Z_STRVAL_P(file), Z_LVAL_P(line), 
							Z_STRLEN_P(trace) ? Z_STRVAL_P(trace) : "#0 {main}\n");
	}

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	zend_update_property_string(default_exception_ptr, getThis(), "string", sizeof("string")-1, str TSRMLS_CC);

	zval_ptr_dtor(&trace);

	RETURN_STRINGL(str, len, 0);
}

/* All functions that may be used in uncaught exception handlers must be final
 * and must not throw exceptions. Otherwise we would need a facility to handle
 * such exceptions in that handler. 
 * Also all getXY() methods are final because thy serve as read only access to
 * their corresponding properties, no more, no less. If after all you need to 
 * override somthing then it is method __toString().
 * And never try to change the state of exceptions and never implement anything
 * that gives the user anything to accomplish this.
 */
static zend_function_entry default_exception_functions[] = {
	ZEND_ME(exception, __clone, NULL, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(exception, __construct, NULL, 0)
	ZEND_ME(exception, getMessage, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getCode, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getFile, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getLine, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getTrace, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, getTraceAsString, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_ME(exception, __toString, NULL, 0)
	{NULL, NULL, NULL}
};

static void zend_register_default_exception(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "exception", default_exception_functions);
	default_exception_ptr = zend_register_internal_class(&ce TSRMLS_CC);
	default_exception_ptr->create_object = zend_default_exception_new; 
	memcpy(&default_exception_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	default_exception_handlers.clone_obj = NULL;

	zend_declare_property_string(default_exception_ptr, "message", sizeof("message")-1, "", ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_string(default_exception_ptr, "string", sizeof("string")-1, "", ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_long(default_exception_ptr, "code", sizeof("code")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ptr, "file", sizeof("file")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ptr, "line", sizeof("line")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(default_exception_ptr, "trace", sizeof("trace")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}

ZEND_API zend_class_entry *zend_exception_get_default(void)
{
	return default_exception_ptr;
}

ZEND_API void zend_throw_exception_ex(zend_class_entry *exception_ce, long code TSRMLS_DC, char *format, ...)
{
	zval *ex;
	va_list arg;
	char *message;

	va_start(arg, format); 
	zend_vspprintf(&message, 0, format, arg);
	va_end(arg);

	MAKE_STD_ZVAL(ex);
	if (exception_ce) {
		if (!instanceof_function(exception_ce, default_exception_ptr TSRMLS_CC)) {
			zend_error(E_NOTICE, "Exceptions must be derived from exception");
			exception_ce = default_exception_ptr;
		}
	} else {
		exception_ce = default_exception_ptr;
	}
	object_init_ex(ex, exception_ce);
	

	if (message) {
		zend_update_property_string(exception_ce, ex, "message", sizeof("message")-1, message TSRMLS_CC);
	}
	if (code) {
		zend_update_property_long(exception_ce, ex, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	efree(message);

	EG(exception) = ex;
}

/* at the moment we can't use zend_throw_exception_ex because we don't have a protable
 * vsnprintf that tells us the number of characters needed nor do we have spprintf from
 * php or asprintf from glibc always.
 */
ZEND_API void zend_throw_exception(zend_class_entry *exception_ce, char *message, long code TSRMLS_DC)
{
	zval *ex;

	MAKE_STD_ZVAL(ex);
	if (exception_ce) {
		if (!instanceof_function(exception_ce, default_exception_ptr TSRMLS_CC)) {
			zend_error(E_NOTICE, "Exceptions must be derived from exception");
			exception_ce = default_exception_ptr;
		}
	} else {
		exception_ce = default_exception_ptr;
	}
	object_init_ex(ex, exception_ce);
	

	if (message) {
		zend_update_property_string(exception_ce, ex, "message", sizeof("message")-1, message TSRMLS_CC);
	}
	if (code) {
		zend_update_property_long(exception_ce, ex, "code", sizeof("code")-1, code TSRMLS_CC);
	}

	EG(exception) = ex;
}

static void zend_error_va(int type, const char *file, uint lineno, const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	zend_error_cb(type, file, lineno, format, args);
	va_end(args);
}

ZEND_API void zend_exception_error(zval *exception TSRMLS_DC)
{
	zend_class_entry *ce_exception = Z_OBJCE_P(exception);
	if (instanceof_function(ce_exception, default_exception_ptr TSRMLS_CC)) {
		zval *str, *file, *line;
		zval *old_exception = EG(exception);

		EG(exception) = NULL;
		
		zend_call_method_with_0_params(&exception, ce_exception, NULL, "__tostring", &str);
		if (!EG(exception)) {
			if (Z_TYPE_P(str) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ce_exception->name);
			} else {
				zend_update_property_string(default_exception_ptr, exception, "string", sizeof("string")-1, EG(exception) ? ce_exception->name : Z_STRVAL_P(str) TSRMLS_CC);
			}
		}
		zval_ptr_dtor(&str);
	
		if (EG(exception)) {
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, default_exception_ptr TSRMLS_CC)) {
				file = zend_read_property(default_exception_ptr, EG(exception), "file", sizeof("file")-1, 1 TSRMLS_CC);
				line = zend_read_property(default_exception_ptr, EG(exception), "line", sizeof("line")-1, 1 TSRMLS_CC);
			} else {
				file = NULL;
				line = NULL;
			}
			zend_error_va(E_WARNING, file ? Z_STRVAL_P(file) : NULL, line ? Z_LVAL_P(line) : 0, "Uncaught %s in exception handling during call to %s::__tostring()", Z_OBJCE_P(EG(exception))->name, ce_exception->name);
		}

		str = zend_read_property(default_exception_ptr, exception, "string", sizeof("string")-1, 1 TSRMLS_CC);
		file = zend_read_property(default_exception_ptr, exception, "file", sizeof("file")-1, 1 TSRMLS_CC);
		line = zend_read_property(default_exception_ptr, exception, "line", sizeof("line")-1, 1 TSRMLS_CC);

		EG(exception) = old_exception;

		zend_error_va(E_ERROR, Z_STRVAL_P(file), Z_LVAL_P(line), "Uncaught %s\n  thrown", Z_STRVAL_P(str));
	} else {
		zend_error(E_ERROR, "Uncaught exception '%s'", ce_exception->name);
	}
}

ZEND_API void zend_register_default_classes(TSRMLS_D)
{
	zend_register_interfaces(TSRMLS_C);
	zend_register_default_exception(TSRMLS_C);
	zend_register_reflection_api(TSRMLS_C);
	zend_register_iterator_wrapper(TSRMLS_C);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
