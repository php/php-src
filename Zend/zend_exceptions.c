/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Marcus Boerger <helly@php.net>                              |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

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
ZEND_API zend_class_entry *zend_ce_compile_error;
ZEND_API zend_class_entry *zend_ce_parse_error;
ZEND_API zend_class_entry *zend_ce_type_error;
ZEND_API zend_class_entry *zend_ce_argument_count_error;
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

static inline zend_class_entry *i_get_exception_base(zval *object) /* {{{ */
{
	return instanceof_function(Z_OBJCE_P(object), zend_ce_exception) ? zend_ce_exception : zend_ce_error;
}
/* }}} */

ZEND_API zend_class_entry *zend_get_exception_base(zval *object) /* {{{ */
{
	return i_get_exception_base(object);
}
/* }}} */

void zend_exception_set_previous(zend_object *exception, zend_object *add_previous) /* {{{ */
{
    zval *previous, *ancestor, *ex;
	zval  pv, zv, rv;
	zend_class_entry *base_ce;

	if (!exception || !add_previous) {
		return;
	}

	if (exception == add_previous) {
		OBJ_RELEASE(add_previous);
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
		ancestor = zend_read_property_ex(i_get_exception_base(&pv), &pv, ZSTR_KNOWN(ZEND_STR_PREVIOUS), 1, &rv);
		while (Z_TYPE_P(ancestor) == IS_OBJECT) {
			if (Z_OBJ_P(ancestor) == Z_OBJ_P(ex)) {
				OBJ_RELEASE(add_previous);
				return;
			}
			ancestor = zend_read_property_ex(i_get_exception_base(ancestor), ancestor, ZSTR_KNOWN(ZEND_STR_PREVIOUS), 1, &rv);
		}
		base_ce = i_get_exception_base(ex);
		previous = zend_read_property_ex(base_ce, ex, ZSTR_KNOWN(ZEND_STR_PREVIOUS), 1, &rv);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property_ex(base_ce, ex, ZSTR_KNOWN(ZEND_STR_PREVIOUS), &pv);
			GC_DELREF(add_previous);
			return;
		}
		ex = previous;
	} while (Z_OBJ_P(ex) != add_previous);
}
/* }}} */

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
		if (exception && (Z_OBJCE_P(exception) == zend_ce_parse_error || Z_OBJCE_P(exception) == zend_ce_compile_error)) {
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
	zend_object *exception;
	if (EG(prev_exception)) {
		OBJ_RELEASE(EG(prev_exception));
		EG(prev_exception) = NULL;
	}
	if (!EG(exception)) {
		return;
	}
	/* exception may have destructor */
	exception = EG(exception);
	EG(exception) = NULL;
	OBJ_RELEASE(exception);
	if (EG(current_execute_data)) {
		EG(current_execute_data)->opline = EG(opline_before_exception);
	}
#if ZEND_DEBUG
	EG(opline_before_exception) = NULL;
#endif
}
/* }}} */

static zend_object *zend_default_exception_new_ex(zend_class_entry *class_type, int skip_top_traces) /* {{{ */
{
	zval obj, tmp;
	zend_object *object;
	zval trace;
	zend_class_entry *base_ce;
	zend_string *filename;

	Z_OBJ(obj) = object = zend_objects_new(class_type);
	Z_OBJ_HT(obj) = &default_exception_handlers;

	object_properties_init(object, class_type);

	if (EG(current_execute_data)) {
		zend_fetch_debug_backtrace(&trace,
			skip_top_traces,
			EG(exception_ignore_args) ? DEBUG_BACKTRACE_IGNORE_ARGS : 0, 0);
	} else {
		array_init(&trace);
	}
	Z_SET_REFCOUNT(trace, 0);

	base_ce = i_get_exception_base(&obj);

	if (EXPECTED((class_type != zend_ce_parse_error && class_type != zend_ce_compile_error)
			|| !(filename = zend_get_compiled_filename()))) {
		ZVAL_STRING(&tmp, zend_get_executed_filename());
		zend_update_property_ex(base_ce, &obj, ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
		zval_ptr_dtor(&tmp);
		ZVAL_LONG(&tmp, zend_get_executed_lineno());
		zend_update_property_ex(base_ce, &obj, ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	} else {
		ZVAL_STR(&tmp, filename);
		zend_update_property_ex(base_ce, &obj, ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
		ZVAL_LONG(&tmp, zend_get_compiled_lineno());
		zend_update_property_ex(base_ce, &obj, ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	}
	zend_update_property_ex(base_ce, &obj, ZSTR_KNOWN(ZEND_STR_TRACE), &trace);

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
	zval  tmp, *object, *previous = NULL;
	zend_class_entry *base_ce;
	int    argc = ZEND_NUM_ARGS();

	object = ZEND_THIS;
	base_ce = i_get_exception_base(object);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc, "|SlO!", &message, &code, &previous, zend_ce_throwable) == FAILURE) {
		zend_class_entry *ce;

		if (Z_TYPE(EX(This)) == IS_OBJECT) {
			ce = Z_OBJCE(EX(This));
		} else if (Z_CE(EX(This))) {
			ce = Z_CE(EX(This));
		} else {
			ce = base_ce;
		}
		zend_throw_error(NULL, "Wrong parameters for %s([string $message [, long $code [, Throwable $previous = NULL]]])", ZSTR_VAL(ce->name));
		return;
	}

	if (message) {
		ZVAL_STR(&tmp, message);
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
	}

	if (code) {
		ZVAL_LONG(&tmp, code);
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_CODE), &tmp);
	}

	if (previous) {
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_PREVIOUS), previous);
	}
}
/* }}} */

/* {{{ proto Exception::__wakeup()
   Exception unserialize checks */
#define CHECK_EXC_TYPE(id, type) \
	pvalue = zend_read_property_ex(i_get_exception_base(object), (object), ZSTR_KNOWN(id), 1, &value); \
	if (Z_TYPE_P(pvalue) != IS_NULL && Z_TYPE_P(pvalue) != type) { \
		zend_unset_property(i_get_exception_base(object), object, ZSTR_VAL(ZSTR_KNOWN(id)), ZSTR_LEN(ZSTR_KNOWN(id))); \
	}

ZEND_METHOD(exception, __wakeup)
{
	zval value, *pvalue;
	zval *object = ZEND_THIS;
	CHECK_EXC_TYPE(ZEND_STR_MESSAGE,  IS_STRING);
	CHECK_EXC_TYPE(ZEND_STR_STRING,   IS_STRING);
	CHECK_EXC_TYPE(ZEND_STR_CODE,     IS_LONG);
	CHECK_EXC_TYPE(ZEND_STR_FILE,     IS_STRING);
	CHECK_EXC_TYPE(ZEND_STR_LINE,     IS_LONG);
	CHECK_EXC_TYPE(ZEND_STR_TRACE,    IS_ARRAY);
	pvalue = zend_read_property(i_get_exception_base(object), object, "previous", sizeof("previous")-1, 1, &value);
	if (pvalue && Z_TYPE_P(pvalue) != IS_NULL && (Z_TYPE_P(pvalue) != IS_OBJECT ||
			!instanceof_function(Z_OBJCE_P(pvalue), zend_ce_throwable) ||
			pvalue == object)) {
		zend_unset_property(i_get_exception_base(object), object, "previous", sizeof("previous")-1);
	}
}
/* }}} */

/* {{{ proto ErrorException::__construct(string message, int code, int severity [, string filename [, int lineno [, Throwable previous]]])
   ErrorException constructor */
ZEND_METHOD(error_exception, __construct)
{
	zend_string *message = NULL, *filename = NULL;
	zend_long   code = 0, severity = E_ERROR, lineno;
	zval   tmp, *object, *previous = NULL;
	int    argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, argc, "|SllSlO!", &message, &code, &severity, &filename, &lineno, &previous, zend_ce_throwable) == FAILURE) {
		zend_class_entry *ce;

		if (Z_TYPE(EX(This)) == IS_OBJECT) {
			ce = Z_OBJCE(EX(This));
		} else if (Z_CE(EX(This))) {
			ce = Z_CE(EX(This));
		} else {
			ce = zend_ce_error_exception;
		}
		zend_throw_error(NULL, "Wrong parameters for %s([string $message [, long $code, [ long $severity, [ string $filename, [ long $lineno  [, Throwable $previous = NULL]]]]]])", ZSTR_VAL(ce->name));
		return;
	}

	object = ZEND_THIS;

	if (message) {
		ZVAL_STR_COPY(&tmp, message);
		zend_update_property_ex(zend_ce_exception, object, ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
		zval_ptr_dtor(&tmp);
	}

	if (code) {
		ZVAL_LONG(&tmp, code);
		zend_update_property_ex(zend_ce_exception, object, ZSTR_KNOWN(ZEND_STR_CODE), &tmp);
	}

	if (previous) {
		zend_update_property_ex(zend_ce_exception, object, ZSTR_KNOWN(ZEND_STR_PREVIOUS), previous);
	}

	ZVAL_LONG(&tmp, severity);
	zend_update_property_ex(zend_ce_exception, object, ZSTR_KNOWN(ZEND_STR_SEVERITY), &tmp);

	if (argc >= 4) {
		ZVAL_STR_COPY(&tmp, filename);
		zend_update_property_ex(zend_ce_exception, object, ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
		zval_ptr_dtor(&tmp);
    	if (argc < 5) {
    	    lineno = 0; /* invalidate lineno */
    	}
		ZVAL_LONG(&tmp, lineno);
		zend_update_property_ex(zend_ce_exception, object, ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	}
}
/* }}} */

#define DEFAULT_0_PARAMS \
	if (zend_parse_parameters_none() == FAILURE) { \
		return; \
	}

#define GET_PROPERTY(object, id) \
	zend_read_property_ex(i_get_exception_base(object), (object), ZSTR_KNOWN(id), 0, &rv)
#define GET_PROPERTY_SILENT(object, id) \
	zend_read_property_ex(i_get_exception_base(object), (object), ZSTR_KNOWN(id), 1, &rv)

/* {{{ proto string Exception|Error::getFile()
   Get the file in which the exception occurred */
ZEND_METHOD(exception, getFile)
{
	zval *prop, rv;

	DEFAULT_0_PARAMS;

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_FILE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ proto int Exception|Error::getLine()
   Get the line in which the exception occurred */
ZEND_METHOD(exception, getLine)
{
	zval *prop, rv;

	DEFAULT_0_PARAMS;

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_LINE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ proto string Exception|Error::getMessage()
   Get the exception message */
ZEND_METHOD(exception, getMessage)
{
	zval *prop, rv;

	DEFAULT_0_PARAMS;

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_MESSAGE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ proto int Exception|Error::getCode()
   Get the exception code */
ZEND_METHOD(exception, getCode)
{
	zval *prop, rv;

	DEFAULT_0_PARAMS;

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_CODE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ proto array Exception|Error::getTrace()
   Get the stack trace for the location in which the exception occurred */
ZEND_METHOD(exception, getTrace)
{
	zval *prop, rv;

	DEFAULT_0_PARAMS;

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_TRACE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ proto int ErrorException::getSeverity()
   Get the exception severity */
ZEND_METHOD(error_exception, getSeverity)
{
	zval *prop, rv;

	DEFAULT_0_PARAMS;

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_SEVERITY);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

#define TRACE_APPEND_KEY(key) do {                                          \
		tmp = zend_hash_find(ht, key);                                      \
		if (tmp) {                                                          \
			if (Z_TYPE_P(tmp) != IS_STRING) {                               \
				zend_error(E_WARNING, "Value for %s is no string",          \
					ZSTR_VAL(key));                                         \
				smart_str_appends(str, "[unknown]");                        \
			} else {                                                        \
				smart_str_appends(str, Z_STRVAL_P(tmp));                    \
			}                                                               \
		} \
	} while (0)

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
			smart_str_append_printf(str, "%.*G", (int) EG(precision), Z_DVAL_P(arg));
			smart_str_appends(str, ", ");
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
			zend_string_release_ex(class_name, 0);
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

	file = zend_hash_find_ex(ht, ZSTR_KNOWN(ZEND_STR_FILE), 1);
	if (file) {
		if (Z_TYPE_P(file) != IS_STRING) {
			zend_error(E_WARNING, "Function name is no string");
			smart_str_appends(str, "[unknown function]");
		} else{
			zend_long line;
			tmp = zend_hash_find_ex(ht, ZSTR_KNOWN(ZEND_STR_LINE), 1);
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
	TRACE_APPEND_KEY(ZSTR_KNOWN(ZEND_STR_CLASS));
	TRACE_APPEND_KEY(ZSTR_KNOWN(ZEND_STR_TYPE));
	TRACE_APPEND_KEY(ZSTR_KNOWN(ZEND_STR_FUNCTION));
	smart_str_appendc(str, '(');
	tmp = zend_hash_find_ex(ht, ZSTR_KNOWN(ZEND_STR_ARGS), 1);
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

	object = ZEND_THIS;
	base_ce = i_get_exception_base(object);

	trace = zend_read_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_TRACE), 1, &rv);
	if (Z_TYPE_P(trace) != IS_ARRAY) {
		RETURN_FALSE;
	}
	ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(trace), index, frame) {
		if (Z_TYPE_P(frame) != IS_ARRAY) {
			zend_error(E_WARNING, "Expected array for frame " ZEND_ULONG_FMT, index);
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

	ZVAL_COPY(return_value, GET_PROPERTY_SILENT(ZEND_THIS, ZEND_STR_PREVIOUS));
} /* }}} */

/* {{{ proto string Exception|Error::__toString()
   Obtain the string representation of the Exception object */
ZEND_METHOD(exception, __toString)
{
	zval trace, *exception;
	zend_class_entry *base_ce;
	zend_string *str;
	zend_fcall_info fci;
	zval rv, tmp;
	zend_string *fname;

	DEFAULT_0_PARAMS;

	str = ZSTR_EMPTY_ALLOC();

	exception = ZEND_THIS;
	fname = zend_string_init("gettraceasstring", sizeof("gettraceasstring")-1, 0);

	while (exception && Z_TYPE_P(exception) == IS_OBJECT && instanceof_function(Z_OBJCE_P(exception), zend_ce_throwable)) {
		zend_string *prev_str = str;
		zend_string *message = zval_get_string(GET_PROPERTY(exception, ZEND_STR_MESSAGE));
		zend_string *file = zval_get_string(GET_PROPERTY(exception, ZEND_STR_FILE));
		zend_long line = zval_get_long(GET_PROPERTY(exception, ZEND_STR_LINE));

		fci.size = sizeof(fci);
		ZVAL_STR(&fci.function_name, fname);
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

		if ((Z_OBJCE_P(exception) == zend_ce_type_error || Z_OBJCE_P(exception) == zend_ce_argument_count_error) && strstr(ZSTR_VAL(message), ", called in ")) {
			zend_string *real_message = zend_strpprintf(0, "%s and defined", ZSTR_VAL(message));
			zend_string_release_ex(message, 0);
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

		zend_string_release_ex(prev_str, 0);
		zend_string_release_ex(message, 0);
		zend_string_release_ex(file, 0);
		zval_ptr_dtor(&trace);

		Z_PROTECT_RECURSION_P(exception);
		exception = GET_PROPERTY(exception, ZEND_STR_PREVIOUS);
		if (exception && Z_TYPE_P(exception) == IS_OBJECT && Z_IS_RECURSIVE_P(exception)) {
			break;
		}
	}
	zend_string_release_ex(fname, 0);

	exception = ZEND_THIS;
	/* Reset apply counts */
	while (exception && Z_TYPE_P(exception) == IS_OBJECT && (base_ce = i_get_exception_base(exception)) && instanceof_function(Z_OBJCE_P(exception), base_ce)) {
		if (Z_IS_RECURSIVE_P(exception)) {
			Z_UNPROTECT_RECURSION_P(exception);
		} else {
			break;
		}
		exception = GET_PROPERTY(exception, ZEND_STR_PREVIOUS);
	}

	exception = ZEND_THIS;
	base_ce = i_get_exception_base(exception);

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	ZVAL_STR(&tmp, str);
	zend_update_property_ex(base_ce, exception, ZSTR_KNOWN(ZEND_STR_STRING), &tmp);

	RETURN_STR(str);
}
/* }}} */

/** {{{ Throwable method definition */
static const zend_function_entry zend_funcs_throwable[] = {
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
 * override something then it is method __toString().
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

	memcpy(&default_exception_handlers, &std_object_handlers, sizeof(zend_object_handlers));
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

	INIT_CLASS_ENTRY(ce, "CompileError", NULL);
	zend_ce_compile_error = zend_register_internal_class_ex(&ce, zend_ce_error);
	zend_ce_compile_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "ParseError", NULL);
	zend_ce_parse_error = zend_register_internal_class_ex(&ce, zend_ce_compile_error);
	zend_ce_parse_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "TypeError", NULL);
	zend_ce_type_error = zend_register_internal_class_ex(&ce, zend_ce_error);
	zend_ce_type_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(ce, "ArgumentCountError", NULL);
	zend_ce_argument_count_error = zend_register_internal_class_ex(&ce, zend_ce_type_error);
	zend_ce_argument_count_error->create_object = zend_default_exception_new;

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
	zval ex, tmp;

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
		ZVAL_STRING(&tmp, message);
		zend_update_property_ex(exception_ce, &ex, ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
		zval_ptr_dtor(&tmp);
	}
	if (code) {
		ZVAL_LONG(&tmp, code);
		zend_update_property_ex(exception_ce, &ex, ZSTR_KNOWN(ZEND_STR_CODE), &tmp);
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
	zval ex, tmp;
	zend_object *obj = zend_throw_exception(exception_ce, message, code);
	ZVAL_OBJ(&ex, obj);
	ZVAL_LONG(&tmp, severity);
	zend_update_property_ex(zend_ce_error_exception, &ex, ZSTR_KNOWN(ZEND_STR_SEVERITY), &tmp);
	return obj;
}
/* }}} */

static void zend_error_va(int type, const char *file, uint32_t lineno, const char *format, ...) /* {{{ */
{
	va_list args;

	va_start(args, format);
	zend_error_cb(type, file, lineno, format, args);
	va_end(args);
}
/* }}} */

static void zend_error_helper(int type, const char *filename, const uint32_t lineno, const char *format, ...) /* {{{ */
{
	va_list va;

	va_start(va, format);
	zend_error_cb(type, filename, lineno, format, va);
	va_end(va);
}
/* }}} */

/* This function doesn't return if it uses E_ERROR */
ZEND_API ZEND_COLD void zend_exception_error(zend_object *ex, int severity) /* {{{ */
{
	zval exception, rv;
	zend_class_entry *ce_exception;

	ZVAL_OBJ(&exception, ex);
	ce_exception = ex->ce;
	EG(exception) = NULL;
	if (ce_exception == zend_ce_parse_error || ce_exception == zend_ce_compile_error) {
		zend_string *message = zval_get_string(GET_PROPERTY(&exception, ZEND_STR_MESSAGE));
		zend_string *file = zval_get_string(GET_PROPERTY_SILENT(&exception, ZEND_STR_FILE));
		zend_long line = zval_get_long(GET_PROPERTY_SILENT(&exception, ZEND_STR_LINE));

		zend_error_helper(ce_exception == zend_ce_parse_error ? E_PARSE : E_COMPILE_ERROR,
			ZSTR_VAL(file), line, "%s", ZSTR_VAL(message));

		zend_string_release_ex(file, 0);
		zend_string_release_ex(message, 0);
	} else if (instanceof_function(ce_exception, zend_ce_throwable)) {
		zval tmp;
		zend_string *str, *file = NULL;
		zend_long line = 0;

		zend_call_method_with_0_params(&exception, ce_exception, &ex->ce->__tostring, "__tostring", &tmp);
		if (!EG(exception)) {
			if (Z_TYPE(tmp) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ZSTR_VAL(ce_exception->name));
			} else {
				zend_update_property_ex(i_get_exception_base(&exception), &exception, ZSTR_KNOWN(ZEND_STR_STRING), &tmp);
			}
		}
		zval_ptr_dtor(&tmp);

		if (EG(exception)) {
			zval zv;

			ZVAL_OBJ(&zv, EG(exception));
			/* do the best we can to inform about the inner exception */
			if (instanceof_function(ce_exception, zend_ce_exception) || instanceof_function(ce_exception, zend_ce_error)) {
				file = zval_get_string(GET_PROPERTY_SILENT(&zv, ZEND_STR_FILE));
				line = zval_get_long(GET_PROPERTY_SILENT(&zv, ZEND_STR_LINE));
			}

			zend_error_va(E_WARNING, (file && ZSTR_LEN(file) > 0) ? ZSTR_VAL(file) : NULL, line,
				"Uncaught %s in exception handling during call to %s::__tostring()",
				ZSTR_VAL(Z_OBJCE(zv)->name), ZSTR_VAL(ce_exception->name));

			if (file) {
				zend_string_release_ex(file, 0);
			}
		}

		str = zval_get_string(GET_PROPERTY_SILENT(&exception, ZEND_STR_STRING));
		file = zval_get_string(GET_PROPERTY_SILENT(&exception, ZEND_STR_FILE));
		line = zval_get_long(GET_PROPERTY_SILENT(&exception, ZEND_STR_LINE));

		zend_error_va(severity, (file && ZSTR_LEN(file) > 0) ? ZSTR_VAL(file) : NULL, line,
			"Uncaught %s\n  thrown", ZSTR_VAL(str));

		zend_string_release_ex(str, 0);
		zend_string_release_ex(file, 0);
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
		zval_ptr_dtor(exception);
		return;
	}
	zend_throw_exception_internal(exception);
}
/* }}} */
