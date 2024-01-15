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
#include "zend_exceptions_arginfo.h"
#include "zend_observer.h"

ZEND_API zend_class_entry *zend_ce_throwable;
ZEND_API zend_class_entry *zend_ce_exception;
ZEND_API zend_class_entry *zend_ce_error_exception;
ZEND_API zend_class_entry *zend_ce_error;
ZEND_API zend_class_entry *zend_ce_compile_error;
ZEND_API zend_class_entry *zend_ce_parse_error;
ZEND_API zend_class_entry *zend_ce_type_error;
ZEND_API zend_class_entry *zend_ce_argument_count_error;
ZEND_API zend_class_entry *zend_ce_value_error;
ZEND_API zend_class_entry *zend_ce_arithmetic_error;
ZEND_API zend_class_entry *zend_ce_division_by_zero_error;
ZEND_API zend_class_entry *zend_ce_unhandled_match_error;

/* Internal pseudo-exception that is not exposed to userland. Throwing this exception *does not* execute finally blocks. */
static zend_class_entry zend_ce_unwind_exit;

/* Internal pseudo-exception that is not exposed to userland. Throwing this exception *does* execute finally blocks. */
static zend_class_entry zend_ce_graceful_exit;

ZEND_API void (*zend_throw_exception_hook)(zend_object *ex);

static zend_object_handlers default_exception_handlers;

/* {{{ zend_implement_throwable */
static int zend_implement_throwable(zend_class_entry *interface, zend_class_entry *class_type)
{
	/* zend_ce_exception and zend_ce_error may not be initialized yet when this is called (e.g when
	 * implementing Throwable for Exception itself). Perform a manual inheritance check. */
	zend_class_entry *root = class_type;
	while (root->parent) {
		root = root->parent;
	}
	if (zend_string_equals_literal(root->name, "Exception")
			|| zend_string_equals_literal(root->name, "Error")) {
		return SUCCESS;
	}

	bool can_extend = (class_type->ce_flags & ZEND_ACC_ENUM) == 0;

	zend_error_noreturn(E_ERROR,
		can_extend
			? "%s %s cannot implement interface %s, extend Exception or Error instead"
			: "%s %s cannot implement interface %s",
		zend_get_object_type_uc(class_type),
		ZSTR_VAL(class_type->name),
		ZSTR_VAL(interface->name));
	return FAILURE;
}
/* }}} */

static inline zend_class_entry *i_get_exception_base(zend_object *object) /* {{{ */
{
	return instanceof_function(object->ce, zend_ce_exception) ? zend_ce_exception : zend_ce_error;
}
/* }}} */

ZEND_API zend_class_entry *zend_get_exception_base(zend_object *object) /* {{{ */
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

	if (exception == add_previous || zend_is_unwind_exit(add_previous) || zend_is_graceful_exit(add_previous)) {
		OBJ_RELEASE(add_previous);
		return;
	}

	ZEND_ASSERT(instanceof_function(add_previous->ce, zend_ce_throwable)
		&& "Previous exception must implement Throwable");

	ZVAL_OBJ(&pv, add_previous);
	ZVAL_OBJ(&zv, exception);
	ex = &zv;
	do {
		ancestor = zend_read_property_ex(i_get_exception_base(add_previous), add_previous, ZSTR_KNOWN(ZEND_STR_PREVIOUS), 1, &rv);
		while (Z_TYPE_P(ancestor) == IS_OBJECT) {
			if (Z_OBJ_P(ancestor) == Z_OBJ_P(ex)) {
				OBJ_RELEASE(add_previous);
				return;
			}
			ancestor = zend_read_property_ex(i_get_exception_base(Z_OBJ_P(ancestor)), Z_OBJ_P(ancestor), ZSTR_KNOWN(ZEND_STR_PREVIOUS), 1, &rv);
		}
		base_ce = i_get_exception_base(Z_OBJ_P(ex));
		previous = zend_read_property_ex(base_ce, Z_OBJ_P(ex), ZSTR_KNOWN(ZEND_STR_PREVIOUS), 1, &rv);
		if (Z_TYPE_P(previous) == IS_NULL) {
			zend_update_property_ex(base_ce, Z_OBJ_P(ex), ZSTR_KNOWN(ZEND_STR_PREVIOUS), &pv);
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

static zend_always_inline bool is_handle_exception_set(void) {
	zend_execute_data *execute_data = EG(current_execute_data);
	return !execute_data
		|| !execute_data->func
		|| !ZEND_USER_CODE(execute_data->func->common.type)
		|| execute_data->opline->opcode == ZEND_HANDLE_EXCEPTION;
}

ZEND_API ZEND_COLD void zend_throw_exception_internal(zend_object *exception) /* {{{ */
{
#ifdef HAVE_DTRACE
	if (DTRACE_EXCEPTION_THROWN_ENABLED()) {
		if (exception != NULL) {
			DTRACE_EXCEPTION_THROWN(ZSTR_VAL(exception->ce->name));
		} else {
			DTRACE_EXCEPTION_THROWN(NULL);
		}
	}
#endif /* HAVE_DTRACE */

	if (exception != NULL) {
		zend_object *previous = EG(exception);
		if (previous && zend_is_unwind_exit(previous)) {
			/* Don't replace unwinding exception with different exception. */
			OBJ_RELEASE(exception);
			return;
		}

		zend_exception_set_previous(exception, EG(exception));
		EG(exception) = exception;
		if (previous) {
			ZEND_ASSERT(is_handle_exception_set() && "HANDLE_EXCEPTION not set?");
			return;
		}
	}
	if (!EG(current_execute_data)) {
		if (exception && (exception->ce == zend_ce_parse_error || exception->ce == zend_ce_compile_error)) {
			return;
		}
		if (EG(exception)) {
			zend_exception_error(EG(exception), E_ERROR);
			zend_bailout();
		}
		zend_error_noreturn(E_CORE_ERROR, "Exception thrown without a stack frame");
	}

	if (zend_throw_exception_hook) {
		zend_throw_exception_hook(exception);
	}

	if (is_handle_exception_set()) {
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

static zend_object *zend_default_exception_new_ex(zend_class_entry *class_type, bool skip_top_traces) /* {{{ */
{
	zval tmp;
	zval trace;
	zend_class_entry *base_ce;
	zend_string *filename;

	zend_object *object = zend_objects_new(class_type);
	object->handlers = &default_exception_handlers;

	object_properties_init(object, class_type);

	if (EG(current_execute_data)) {
		zend_fetch_debug_backtrace(&trace,
			skip_top_traces,
			EG(exception_ignore_args) ? DEBUG_BACKTRACE_IGNORE_ARGS : 0, 0);
	} else {
		array_init(&trace);
	}
	Z_SET_REFCOUNT(trace, 0);

	base_ce = i_get_exception_base(object);

	if (EXPECTED((class_type != zend_ce_parse_error && class_type != zend_ce_compile_error)
			|| !(filename = zend_get_compiled_filename()))) {
		ZVAL_STRING(&tmp, zend_get_executed_filename());
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
		zval_ptr_dtor(&tmp);
		ZVAL_LONG(&tmp, zend_get_executed_lineno());
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	} else {
		ZVAL_STR(&tmp, filename);
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
		ZVAL_LONG(&tmp, zend_get_compiled_lineno());
		zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	}
	zend_update_property_ex(base_ce, object, ZSTR_KNOWN(ZEND_STR_TRACE), &trace);

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
	return zend_default_exception_new_ex(class_type, 0);
}
/* }}} */

/* {{{ Clone the exception object */
ZEND_COLD ZEND_METHOD(Exception, __clone)
{
	/* Should never be executable */
	zend_throw_exception(NULL, "Cannot clone object using __clone()", 0);
}
/* }}} */

/* {{{ Exception constructor */
ZEND_METHOD(Exception, __construct)
{
	zend_string *message = NULL;
	zend_long   code = 0;
	zval  tmp, *object, *previous = NULL;
	zend_class_entry *base_ce;

	object = ZEND_THIS;
	base_ce = i_get_exception_base(Z_OBJ_P(object));

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|SlO!", &message, &code, &previous, zend_ce_throwable) == FAILURE) {
		RETURN_THROWS();
	}

	if (message) {
		ZVAL_STR(&tmp, message);
		zend_update_property_ex(base_ce, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
	}

	if (code) {
		ZVAL_LONG(&tmp, code);
		zend_update_property_ex(base_ce, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_CODE), &tmp);
	}

	if (previous) {
		zend_update_property_ex(base_ce, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_PREVIOUS), previous);
	}
}
/* }}} */

/* {{{ Exception unserialize checks */
#define CHECK_EXC_TYPE(id, type) \
	pvalue = zend_read_property_ex(i_get_exception_base(Z_OBJ_P(object)), Z_OBJ_P(object), ZSTR_KNOWN(id), 1, &value); \
	if (Z_TYPE_P(pvalue) != IS_NULL && Z_TYPE_P(pvalue) != type) { \
		zend_unset_property(i_get_exception_base(Z_OBJ_P(object)), Z_OBJ_P(object), ZSTR_VAL(ZSTR_KNOWN(id)), ZSTR_LEN(ZSTR_KNOWN(id))); \
	}

ZEND_METHOD(Exception, __wakeup)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval value, *pvalue;
	zval *object = ZEND_THIS;
	CHECK_EXC_TYPE(ZEND_STR_MESSAGE, IS_STRING);
	CHECK_EXC_TYPE(ZEND_STR_CODE,    IS_LONG);
	/* The type of all other properties is enforced through typed properties. */
}
/* }}} */

/* {{{ ErrorException constructor */
ZEND_METHOD(ErrorException, __construct)
{
	zend_string *message = NULL, *filename = NULL;
	zend_long   code = 0, severity = E_ERROR, lineno;
	bool lineno_is_null = 1;
	zval   tmp, *object, *previous = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|SllS!l!O!", &message, &code, &severity, &filename, &lineno, &lineno_is_null, &previous, zend_ce_throwable) == FAILURE) {
		RETURN_THROWS();
	}

	object = ZEND_THIS;

	if (message) {
		ZVAL_STR_COPY(&tmp, message);
		zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
		zval_ptr_dtor(&tmp);
	}

	if (code) {
		ZVAL_LONG(&tmp, code);
		zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_CODE), &tmp);
	}

	if (previous) {
		zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_PREVIOUS), previous);
	}

	ZVAL_LONG(&tmp, severity);
	zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_SEVERITY), &tmp);

	if (filename) {
		ZVAL_STR_COPY(&tmp, filename);
		zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_FILE), &tmp);
		zval_ptr_dtor(&tmp);
	}

	if (!lineno_is_null) {
		ZVAL_LONG(&tmp, lineno);
		zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	} else if (filename) {
		ZVAL_LONG(&tmp, 0);
		zend_update_property_ex(zend_ce_exception, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_LINE), &tmp);
	}
}
/* }}} */

#define GET_PROPERTY(object, id) \
	zend_read_property_ex(i_get_exception_base(Z_OBJ_P(object)), Z_OBJ_P(object), ZSTR_KNOWN(id), 0, &rv)
#define GET_PROPERTY_SILENT(object, id) \
	zend_read_property_ex(i_get_exception_base(Z_OBJ_P(object)), Z_OBJ_P(object), ZSTR_KNOWN(id), 1, &rv)

/* {{{ Get the file in which the exception occurred */
ZEND_METHOD(Exception, getFile)
{
	zval *prop, rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_FILE);
	RETURN_STR(zval_get_string(prop));
}
/* }}} */

/* {{{ Get the line in which the exception occurred */
ZEND_METHOD(Exception, getLine)
{
	zval *prop, rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_LINE);
	RETURN_LONG(zval_get_long(prop));
}
/* }}} */

/* {{{ Get the exception message */
ZEND_METHOD(Exception, getMessage)
{
	zval *prop, rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_MESSAGE);
	RETURN_STR(zval_get_string(prop));
}
/* }}} */

/* {{{ Get the exception code */
ZEND_METHOD(Exception, getCode)
{
	zval *prop, rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_CODE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ Get the stack trace for the location in which the exception occurred */
ZEND_METHOD(Exception, getTrace)
{
	zval *prop, rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_TRACE);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

/* {{{ Get the exception severity */
ZEND_METHOD(ErrorException, getSeverity)
{
	zval *prop, rv;

	ZEND_PARSE_PARAMETERS_NONE();

	prop = GET_PROPERTY(ZEND_THIS, ZEND_STR_SEVERITY);
	ZVAL_DEREF(prop);
	ZVAL_COPY(return_value, prop);
}
/* }}} */

#define TRACE_APPEND_KEY(key) do {                                          \
		tmp = zend_hash_find(ht, key);                                      \
		if (tmp) {                                                          \
			if (Z_TYPE_P(tmp) != IS_STRING) {                               \
				zend_error(E_WARNING, "Value for %s is not a string",       \
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
	 * convert_to_string(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	ZVAL_DEREF(arg);

	if (Z_TYPE_P(arg) <= IS_STRING) {
		smart_str_append_scalar(str, arg, EG(exception_string_param_max_len));
		smart_str_appends(str, ", ");
	} else {
		switch (Z_TYPE_P(arg)) {
			case IS_RESOURCE:
				smart_str_appends(str, "Resource id #");
				smart_str_append_long(str, Z_RES_HANDLE_P(arg));
				smart_str_appends(str, ", ");
				break;
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
}
/* }}} */

static void _build_trace_string(smart_str *str, HashTable *ht, uint32_t num) /* {{{ */
{
	zval *file, *tmp;

	smart_str_appendc(str, '#');
	smart_str_append_long(str, num);
	smart_str_appendc(str, ' ');

	file = zend_hash_find_known_hash(ht, ZSTR_KNOWN(ZEND_STR_FILE));
	if (file) {
		if (Z_TYPE_P(file) != IS_STRING) {
			zend_error(E_WARNING, "File name is not a string");
			smart_str_appends(str, "[unknown file]: ");
		} else{
			zend_long line = 0;
			tmp = zend_hash_find_known_hash(ht, ZSTR_KNOWN(ZEND_STR_LINE));
			if (tmp) {
				if (Z_TYPE_P(tmp) == IS_LONG) {
					line = Z_LVAL_P(tmp);
				} else {
					zend_error(E_WARNING, "Line is not an int");
				}
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
	tmp = zend_hash_find_known_hash(ht, ZSTR_KNOWN(ZEND_STR_ARGS));
	if (tmp) {
		if (Z_TYPE_P(tmp) == IS_ARRAY) {
			size_t last_len = ZSTR_LEN(str->s);
			zend_string *name;
			zval *arg;

			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(tmp), name, arg) {
				if (name) {
					smart_str_append(str, name);
					smart_str_appends(str, ": ");
				}
				_build_trace_args(arg, str);
			} ZEND_HASH_FOREACH_END();

			if (last_len != ZSTR_LEN(str->s)) {
				ZSTR_LEN(str->s) -= 2; /* remove last ', ' */
			}
		} else {
			zend_error(E_WARNING, "args element is not an array");
		}
	}
	smart_str_appends(str, ")\n");
}
/* }}} */

ZEND_API zend_string *zend_trace_to_string(HashTable *trace, bool include_main) {
	zend_ulong index;
	zval *frame;
	uint32_t num = 0;
	smart_str str = {0};

	ZEND_HASH_FOREACH_NUM_KEY_VAL(trace, index, frame) {
		if (Z_TYPE_P(frame) != IS_ARRAY) {
			zend_error(E_WARNING, "Expected array for frame " ZEND_ULONG_FMT, index);
			continue;
		}

		_build_trace_string(&str, Z_ARRVAL_P(frame), num++);
	} ZEND_HASH_FOREACH_END();

	if (include_main) {
		smart_str_appendc(&str, '#');
		smart_str_append_long(&str, num);
		smart_str_appends(&str, " {main}");
	}

	smart_str_0(&str);
	return str.s ? str.s : ZSTR_EMPTY_ALLOC();
}

/* {{{ Obtain the backtrace for the exception as a string (instead of an array) */
ZEND_METHOD(Exception, getTraceAsString)
{

	ZEND_PARSE_PARAMETERS_NONE();

	zval *object = ZEND_THIS;
	zend_class_entry *base_ce = i_get_exception_base(Z_OBJ_P(object));
	zval rv;
	zval *trace = zend_read_property_ex(base_ce, Z_OBJ_P(object), ZSTR_KNOWN(ZEND_STR_TRACE), 1, &rv);
	if (EG(exception)) {
		RETURN_THROWS();
	}

	/* Type should be guaranteed by property type. */
	ZEND_ASSERT(Z_TYPE_P(trace) == IS_ARRAY);
	RETURN_NEW_STR(zend_trace_to_string(Z_ARRVAL_P(trace), /* include_main */ true));
}
/* }}} */

/* {{{ Return previous Throwable or NULL. */
ZEND_METHOD(Exception, getPrevious)
{
	zval rv;

	ZEND_PARSE_PARAMETERS_NONE();

	ZVAL_COPY(return_value, GET_PROPERTY_SILENT(ZEND_THIS, ZEND_STR_PREVIOUS));
} /* }}} */

/* {{{ Obtain the string representation of the Exception object */
ZEND_METHOD(Exception, __toString)
{
	zval trace, *exception;
	zend_class_entry *base_ce;
	zend_string *str;
	zend_fcall_info fci;
	zval rv, tmp;
	zend_string *fname;

	ZEND_PARSE_PARAMETERS_NONE();

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
		fci.named_params = NULL;

		zend_call_function(&fci, NULL);

		if (Z_TYPE(trace) != IS_STRING) {
			zval_ptr_dtor(&trace);
			ZVAL_UNDEF(&trace);
		}

		if ((Z_OBJCE_P(exception) == zend_ce_type_error || Z_OBJCE_P(exception) == zend_ce_argument_count_error) && strstr(ZSTR_VAL(message), ", called in ")) {
			zval message_zv;
			ZVAL_STR(&message_zv, message);
			zend_string *real_message = zend_strpprintf_unchecked(0, "%Z and defined", &message_zv);
			zend_string_release_ex(message, 0);
			message = real_message;
		}

		zend_string *tmp_trace = (Z_TYPE(trace) == IS_STRING && Z_STRLEN(trace))
			? zend_string_copy(Z_STR(trace))
			: ZSTR_INIT_LITERAL("#0 {main}\n", false);

		zval name_zv, trace_zv, file_zv, prev_str_zv;
		ZVAL_STR(&name_zv, Z_OBJCE_P(exception)->name);
		ZVAL_STR(&trace_zv, tmp_trace);
		ZVAL_STR(&file_zv, file);
		ZVAL_STR(&prev_str_zv, prev_str);

		if (ZSTR_LEN(message) > 0) {
			zval message_zv;
			ZVAL_STR(&message_zv, message);

			str = zend_strpprintf_unchecked(0, "%Z: %Z in %Z:" ZEND_LONG_FMT "\nStack trace:\n%Z%s%Z",
				&name_zv, &message_zv, &file_zv, line,
				&trace_zv, ZSTR_LEN(prev_str) ? "\n\nNext " : "", &prev_str_zv);
		} else {
			str = zend_strpprintf_unchecked(0, "%Z in %Z:" ZEND_LONG_FMT "\nStack trace:\n%Z%s%Z",
				&name_zv, &file_zv, line,
				&trace_zv, ZSTR_LEN(prev_str) ? "\n\nNext " : "", &prev_str_zv);
		}
		zend_string_release_ex(tmp_trace, false);

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
	while (exception && Z_TYPE_P(exception) == IS_OBJECT && (base_ce = i_get_exception_base(Z_OBJ_P(exception))) && instanceof_function(Z_OBJCE_P(exception), base_ce)) {
		if (Z_IS_RECURSIVE_P(exception)) {
			Z_UNPROTECT_RECURSION_P(exception);
		} else {
			break;
		}
		exception = GET_PROPERTY(exception, ZEND_STR_PREVIOUS);
	}

	exception = ZEND_THIS;
	base_ce = i_get_exception_base(Z_OBJ_P(exception));

	/* We store the result in the private property string so we can access
	 * the result in uncaught exception handlers without memleaks. */
	ZVAL_STR(&tmp, str);
	zend_update_property_ex(base_ce, Z_OBJ_P(exception), ZSTR_KNOWN(ZEND_STR_STRING), &tmp);

	RETURN_STR(str);
}
/* }}} */

void zend_register_default_exception(void) /* {{{ */
{
	zend_ce_throwable = register_class_Throwable(zend_ce_stringable);
	zend_ce_throwable->interface_gets_implemented = zend_implement_throwable;

	memcpy(&default_exception_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	default_exception_handlers.clone_obj = NULL;

	zend_ce_exception = register_class_Exception(zend_ce_throwable);
	zend_ce_exception->create_object = zend_default_exception_new;

	zend_ce_error_exception = register_class_ErrorException(zend_ce_exception);
	zend_ce_error_exception->create_object = zend_error_exception_new;

	zend_ce_error = register_class_Error(zend_ce_throwable);
	zend_ce_error->create_object = zend_default_exception_new;

	zend_ce_compile_error = register_class_CompileError(zend_ce_error);
	zend_ce_compile_error->create_object = zend_default_exception_new;

	zend_ce_parse_error = register_class_ParseError(zend_ce_compile_error);
	zend_ce_parse_error->create_object = zend_default_exception_new;

	zend_ce_type_error = register_class_TypeError(zend_ce_error);
	zend_ce_type_error->create_object = zend_default_exception_new;

	zend_ce_argument_count_error = register_class_ArgumentCountError(zend_ce_type_error);
	zend_ce_argument_count_error->create_object = zend_default_exception_new;

	zend_ce_value_error = register_class_ValueError(zend_ce_error);
	zend_ce_value_error->create_object = zend_default_exception_new;

	zend_ce_arithmetic_error = register_class_ArithmeticError(zend_ce_error);
	zend_ce_arithmetic_error->create_object = zend_default_exception_new;

	zend_ce_division_by_zero_error = register_class_DivisionByZeroError(zend_ce_arithmetic_error);
	zend_ce_division_by_zero_error->create_object = zend_default_exception_new;

	zend_ce_unhandled_match_error = register_class_UnhandledMatchError(zend_ce_error);
	zend_ce_unhandled_match_error->create_object = zend_default_exception_new;

	INIT_CLASS_ENTRY(zend_ce_unwind_exit, "UnwindExit", NULL);

	INIT_CLASS_ENTRY(zend_ce_graceful_exit, "GracefulExit", NULL);
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

static zend_object *zend_throw_exception_zstr(zend_class_entry *exception_ce, zend_string *message, zend_long code) /* {{{ */
{
	zval ex, tmp;

	if (!exception_ce) {
		exception_ce = zend_ce_exception;
	}

	ZEND_ASSERT(instanceof_function(exception_ce, zend_ce_throwable)
		&& "Exceptions must implement Throwable");

	object_init_ex(&ex, exception_ce);

	if (message) {
		ZVAL_STR(&tmp, message);
		zend_update_property_ex(exception_ce, Z_OBJ(ex), ZSTR_KNOWN(ZEND_STR_MESSAGE), &tmp);
	}
	if (code) {
		ZVAL_LONG(&tmp, code);
		zend_update_property_ex(exception_ce, Z_OBJ(ex), ZSTR_KNOWN(ZEND_STR_CODE), &tmp);
	}

	zend_throw_exception_internal(Z_OBJ(ex));

	return Z_OBJ(ex);
}
/* }}} */

ZEND_API ZEND_COLD zend_object *zend_throw_exception(zend_class_entry *exception_ce, const char *message, zend_long code) /* {{{ */
{
	zend_string *msg_str = message ? zend_string_init(message, strlen(message), 0) : NULL;
	zend_object *ex = zend_throw_exception_zstr(exception_ce, msg_str, code);
	if (msg_str) {
		zend_string_release(msg_str);
	}
	return ex;
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

ZEND_API ZEND_COLD zend_object *zend_throw_error_exception(zend_class_entry *exception_ce, zend_string *message, zend_long code, int severity) /* {{{ */
{
	zend_object *obj = zend_throw_exception_zstr(exception_ce, message, code);
	if (exception_ce && instanceof_function(exception_ce, zend_ce_error_exception)) {
		zval tmp;
		ZVAL_LONG(&tmp, severity);
		zend_update_property_ex(zend_ce_error_exception, obj, ZSTR_KNOWN(ZEND_STR_SEVERITY), &tmp);
	}
	return obj;
}
/* }}} */

static void zend_error_va(int type, zend_string *file, uint32_t lineno, const char *format, ...) /* {{{ */
{
	va_list args;
	va_start(args, format);
	zend_string *message = zend_vstrpprintf(0, format, args);
	zend_observer_error_notify(type, file, lineno, message);
	zend_error_cb(type, file, lineno, message);
	zend_string_release(message);
	va_end(args);
}
/* }}} */

/* This function doesn't return if it uses E_ERROR */
ZEND_API ZEND_COLD zend_result zend_exception_error(zend_object *ex, int severity) /* {{{ */
{
	zval exception, rv;
	zend_class_entry *ce_exception;
	zend_result result = FAILURE;

	ZVAL_OBJ(&exception, ex);
	ce_exception = ex->ce;
	EG(exception) = NULL;
	if (ce_exception == zend_ce_parse_error || ce_exception == zend_ce_compile_error) {
		zend_string *message = zval_get_string(GET_PROPERTY(&exception, ZEND_STR_MESSAGE));
		zend_string *file = zval_get_string(GET_PROPERTY_SILENT(&exception, ZEND_STR_FILE));
		zend_long line = zval_get_long(GET_PROPERTY_SILENT(&exception, ZEND_STR_LINE));
		int type = (ce_exception == zend_ce_parse_error ? E_PARSE : E_COMPILE_ERROR) | E_DONT_BAIL;

		zend_observer_error_notify(type, file, line, message);
		zend_error_cb(type, file, line, message);

		zend_string_release_ex(file, 0);
		zend_string_release_ex(message, 0);
	} else if (instanceof_function(ce_exception, zend_ce_throwable)) {
		zval tmp;
		zend_string *str, *file = NULL;
		zend_long line = 0;

		zend_call_known_instance_method_with_0_params(ex->ce->__tostring, ex, &tmp);
		if (!EG(exception)) {
			if (Z_TYPE(tmp) != IS_STRING) {
				zend_error(E_WARNING, "%s::__toString() must return a string", ZSTR_VAL(ce_exception->name));
			} else {
				zend_update_property_ex(i_get_exception_base(ex), ex, ZSTR_KNOWN(ZEND_STR_STRING), &tmp);
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

			zend_error_va(E_WARNING, (file && ZSTR_LEN(file) > 0) ? file : NULL, line,
				"Uncaught %s in exception handling during call to %s::__toString()",
				ZSTR_VAL(Z_OBJCE(zv)->name), ZSTR_VAL(ce_exception->name));

			if (file) {
				zend_string_release_ex(file, 0);
			}
		}

		str = zval_get_string(GET_PROPERTY_SILENT(&exception, ZEND_STR_STRING));
		file = zval_get_string(GET_PROPERTY_SILENT(&exception, ZEND_STR_FILE));
		line = zval_get_long(GET_PROPERTY_SILENT(&exception, ZEND_STR_LINE));

		ZVAL_STR(&tmp, str);
		zend_error_va(severity | E_DONT_BAIL,
			(file && ZSTR_LEN(file) > 0) ? file : NULL, line,
			"Uncaught %Z\n  thrown", &tmp);

		zend_string_release_ex(str, 0);
		zend_string_release_ex(file, 0);
	} else if (ce_exception == &zend_ce_unwind_exit || ce_exception == &zend_ce_graceful_exit) {
		/* We successfully unwound, nothing more to do.
		 * We still return FAILURE in this case, as further execution should still be aborted. */
	} else {
		zend_error(severity, "Uncaught exception %s", ZSTR_VAL(ce_exception->name));
	}

	OBJ_RELEASE(ex);
	return result;
}
/* }}} */

ZEND_NORETURN void zend_exception_uncaught_error(const char *format, ...) {
	va_list va;
	va_start(va, format);
	zend_string *prefix = zend_vstrpprintf(0, format, va);
	va_end(va);

	ZEND_ASSERT(EG(exception));
	zval exception_zv;
	ZVAL_OBJ_COPY(&exception_zv, EG(exception));
	zend_clear_exception();

	zend_string *exception_str = zval_get_string(&exception_zv);
	zend_error_noreturn(E_ERROR,
		"%s: Uncaught %s", ZSTR_VAL(prefix), ZSTR_VAL(exception_str));
}

ZEND_API ZEND_COLD void zend_throw_exception_object(zval *exception) /* {{{ */
{
	if (exception == NULL || Z_TYPE_P(exception) != IS_OBJECT) {
		zend_error_noreturn(E_CORE_ERROR, "Need to supply an object when throwing an exception");
	}

	zend_class_entry *exception_ce = Z_OBJCE_P(exception);

	if (!exception_ce || !instanceof_function(exception_ce, zend_ce_throwable)) {
		zend_throw_error(NULL, "Cannot throw objects that do not implement Throwable");
		zval_ptr_dtor(exception);
		return;
	}

	zend_throw_exception_internal(Z_OBJ_P(exception));
}
/* }}} */

ZEND_API ZEND_COLD zend_object *zend_create_unwind_exit(void)
{
	return zend_objects_new(&zend_ce_unwind_exit);
}

ZEND_API ZEND_COLD zend_object *zend_create_graceful_exit(void)
{
	return zend_objects_new(&zend_ce_graceful_exit);
}

ZEND_API ZEND_COLD void zend_throw_unwind_exit(void)
{
	ZEND_ASSERT(!EG(exception));
	EG(exception) = zend_create_unwind_exit();
	EG(opline_before_exception) = EG(current_execute_data)->opline;
	EG(current_execute_data)->opline = EG(exception_op);
}

ZEND_API ZEND_COLD void zend_throw_graceful_exit(void)
{
	ZEND_ASSERT(!EG(exception));
	EG(exception) = zend_create_graceful_exit();
	EG(opline_before_exception) = EG(current_execute_data)->opline;
	EG(current_execute_data)->opline = EG(exception_op);
}

ZEND_API bool zend_is_unwind_exit(const zend_object *ex)
{
	return ex->ce == &zend_ce_unwind_exit;
}

ZEND_API bool zend_is_graceful_exit(const zend_object *ex)
{
	return ex->ce == &zend_ce_graceful_exit;
}
