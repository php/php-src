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
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_attributes.h"
#include "zend_gc.h"
#include "zend_builtin_functions.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_extensions.h"
#include "zend_closures.h"
#include "zend_generators.h"
#include "zend_builtin_functions_arginfo.h"
#include "zend_smart_str.h"

/* }}} */

ZEND_MINIT_FUNCTION(core) { /* {{{ */
	zend_register_default_classes();

	zend_standard_class_def = register_class_stdClass();

	return SUCCESS;
}
/* }}} */

zend_module_entry zend_builtin_module = { /* {{{ */
	STANDARD_MODULE_HEADER,
	"Core",
	ext_functions,
	ZEND_MINIT(core),
	NULL,
	NULL,
	NULL,
	NULL,
	ZEND_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

zend_result zend_startup_builtin_functions(void) /* {{{ */
{
	zend_module_entry *module;
	EG(current_module) = module = zend_register_module_ex(&zend_builtin_module, MODULE_PERSISTENT);
	if (UNEXPECTED(module == NULL)) {
		return FAILURE;
	}
	ZEND_ASSERT(module->module_number == 0);
	return SUCCESS;
}
/* }}} */

/* {{{ Get the version of the Zend Engine */
ZEND_FUNCTION(zend_version)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1);
}
/* }}} */

/* {{{ Reclaims memory used by MM caches.
   Returns number of freed bytes */
ZEND_FUNCTION(gc_mem_caches)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(zend_mm_gc(zend_mm_get_heap()));
}
/* }}} */

/* {{{ Forces collection of any existing garbage cycles.
   Returns number of freed zvals */
ZEND_FUNCTION(gc_collect_cycles)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG(gc_collect_cycles());
}
/* }}} */

/* {{{ Returns status of the circular reference collector */
ZEND_FUNCTION(gc_enabled)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_BOOL(gc_enabled());
}
/* }}} */

/* {{{ Activates the circular reference collector */
ZEND_FUNCTION(gc_enable)
{
	zend_string *key;

	ZEND_PARSE_PARAMETERS_NONE();

	key = ZSTR_INIT_LITERAL("zend.enable_gc", 0);
	zend_alter_ini_entry_chars(key, "1", sizeof("1")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	zend_string_release_ex(key, 0);
}
/* }}} */

/* {{{ Deactivates the circular reference collector */
ZEND_FUNCTION(gc_disable)
{
	zend_string *key;

	ZEND_PARSE_PARAMETERS_NONE();

	key = ZSTR_INIT_LITERAL("zend.enable_gc", 0);
	zend_alter_ini_entry_chars(key, "0", sizeof("0")-1, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	zend_string_release_ex(key, 0);
}
/* }}} */

/* {{{ Returns current GC statistics */
ZEND_FUNCTION(gc_status)
{
	zend_gc_status status;

	ZEND_PARSE_PARAMETERS_NONE();

	zend_gc_get_status(&status);

	array_init_size(return_value, 16);

	add_assoc_bool_ex(return_value, "running", sizeof("running")-1, status.active);
	add_assoc_bool_ex(return_value, "protected", sizeof("protected")-1, status.gc_protected);
	add_assoc_bool_ex(return_value, "full", sizeof("full")-1, status.full);
	add_assoc_long_ex(return_value, "runs", sizeof("runs")-1, (long)status.runs);
	add_assoc_long_ex(return_value, "collected", sizeof("collected")-1, (long)status.collected);
	add_assoc_long_ex(return_value, "threshold", sizeof("threshold")-1, (long)status.threshold);
	add_assoc_long_ex(return_value, "buffer_size", sizeof("buffer_size")-1, (long)status.buf_size);
	add_assoc_long_ex(return_value, "roots", sizeof("roots")-1, (long)status.num_roots);

	/* Using double because zend_long may be too small on some platforms */
	add_assoc_double_ex(return_value, "application_time", sizeof("application_time")-1, (double) status.application_time / ZEND_NANO_IN_SEC);
	add_assoc_double_ex(return_value, "collector_time", sizeof("collector_time")-1, (double) status.collector_time / ZEND_NANO_IN_SEC);
	add_assoc_double_ex(return_value, "destructor_time", sizeof("destructor_time")-1, (double) status.dtor_time / ZEND_NANO_IN_SEC);
	add_assoc_double_ex(return_value, "free_time", sizeof("free_time")-1, (double) status.free_time / ZEND_NANO_IN_SEC);
}
/* }}} */

/* {{{ Get the number of arguments that were passed to the function */
ZEND_FUNCTION(func_num_args)
{
	zend_execute_data *ex = EX(prev_execute_data);

	ZEND_PARSE_PARAMETERS_NONE();

	if (ex && (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE)) {
		zend_throw_error(NULL, "func_num_args() must be called from a function context");
		RETURN_THROWS();
	}

	if (zend_forbid_dynamic_call() == FAILURE) {
		RETURN_LONG(-1);
	}

	RETURN_LONG(ZEND_CALL_NUM_ARGS(ex));
}
/* }}} */

/* {{{ Get the $arg_num'th argument that was passed to the function */
ZEND_FUNCTION(func_get_arg)
{
	uint32_t arg_count, first_extra_arg;
	zval *arg;
	zend_long requested_offset;
	zend_execute_data *ex;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &requested_offset) == FAILURE) {
		RETURN_THROWS();
	}

	if (requested_offset < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	ex = EX(prev_execute_data);
	if (ex && (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE)) {
		zend_throw_error(NULL, "func_get_arg() cannot be called from the global scope");
		RETURN_THROWS();
	}

	if (zend_forbid_dynamic_call() == FAILURE) {
		RETURN_THROWS();
	}

	arg_count = ZEND_CALL_NUM_ARGS(ex);

	if ((zend_ulong)requested_offset >= arg_count) {
		zend_argument_value_error(1, "must be less than the number of the arguments passed to the currently executed function");
		RETURN_THROWS();
	}

	first_extra_arg = ex->func->op_array.num_args;
	if ((zend_ulong)requested_offset >= first_extra_arg && (ZEND_CALL_NUM_ARGS(ex) > first_extra_arg)) {
		arg = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var + ex->func->op_array.T) + (requested_offset - first_extra_arg);
	} else {
		arg = ZEND_CALL_ARG(ex, requested_offset + 1);
	}
	if (EXPECTED(!Z_ISUNDEF_P(arg))) {
		RETURN_COPY_DEREF(arg);
	}
}
/* }}} */

/* {{{ Get an array of the arguments that were passed to the function */
ZEND_FUNCTION(func_get_args)
{
	zval *p, *q;
	uint32_t arg_count, first_extra_arg;
	uint32_t i;
	zend_execute_data *ex = EX(prev_execute_data);

	ZEND_PARSE_PARAMETERS_NONE();

	if (ex && (ZEND_CALL_INFO(ex) & ZEND_CALL_CODE)) {
		zend_throw_error(NULL, "func_get_args() cannot be called from the global scope");
		RETURN_THROWS();
	}

	if (zend_forbid_dynamic_call() == FAILURE) {
		RETURN_THROWS();
	}

	arg_count = ZEND_CALL_NUM_ARGS(ex);

	if (arg_count) {
		array_init_size(return_value, arg_count);
		first_extra_arg = ex->func->op_array.num_args;
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			i = 0;
			p = ZEND_CALL_ARG(ex, 1);
			if (arg_count > first_extra_arg) {
				while (i < first_extra_arg) {
					q = p;
					if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
						ZVAL_DEREF(q);
						if (Z_OPT_REFCOUNTED_P(q)) {
							Z_ADDREF_P(q);
						}
						ZEND_HASH_FILL_SET(q);
					} else {
						ZEND_HASH_FILL_SET_NULL();
					}
					ZEND_HASH_FILL_NEXT();
					p++;
					i++;
				}
				p = ZEND_CALL_VAR_NUM(ex, ex->func->op_array.last_var + ex->func->op_array.T);
			}
			while (i < arg_count) {
				q = p;
				if (EXPECTED(Z_TYPE_INFO_P(q) != IS_UNDEF)) {
					ZVAL_DEREF(q);
					if (Z_OPT_REFCOUNTED_P(q)) {
						Z_ADDREF_P(q);
					}
					ZEND_HASH_FILL_SET(q);
				} else {
					ZEND_HASH_FILL_SET_NULL();
				}
				ZEND_HASH_FILL_NEXT();
				p++;
				i++;
			}
		} ZEND_HASH_FILL_END();
		Z_ARRVAL_P(return_value)->nNumOfElements = arg_count;
	} else {
		RETURN_EMPTY_ARRAY();
	}
}
/* }}} */

/* {{{ Get string length
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
ZEND_FUNCTION(strlen)
{
	zend_string *s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG(ZSTR_LEN(s));
}
/* }}} */

/* {{{ Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	zend_string *s1, *s2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_binary_strcmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2)));
}
/* }}} */

/* {{{ Binary safe string comparison */
ZEND_FUNCTION(strncmp)
{
	zend_string *s1, *s2;
	zend_long len;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
		Z_PARAM_LONG(len)
	ZEND_PARSE_PARAMETERS_END();

	if (len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	RETURN_LONG(zend_binary_strncmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2), len));
}
/* }}} */

/* {{{ Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	zend_string *s1, *s2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(zend_binary_strcasecmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2)));
}
/* }}} */

/* {{{ Binary safe string comparison */
ZEND_FUNCTION(strncasecmp)
{
	zend_string *s1, *s2;
	zend_long len;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(s1)
		Z_PARAM_STR(s2)
		Z_PARAM_LONG(len)
	ZEND_PARSE_PARAMETERS_END();

	if (len < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	RETURN_LONG(zend_binary_strncasecmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2), len));
}
/* }}} */

/* {{{ Return the current error_reporting level, and if an argument was passed - change to the new level */
ZEND_FUNCTION(error_reporting)
{
	zend_long err;
	bool err_is_null = 1;
	int old_error_reporting;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(err, err_is_null)
	ZEND_PARSE_PARAMETERS_END();

	old_error_reporting = EG(error_reporting);

	if (!err_is_null && err != old_error_reporting) {
		zend_ini_entry *p = EG(error_reporting_ini_entry);

		if (!p) {
			zval *zv = zend_hash_find_known_hash(EG(ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING));
			if (!zv) {
				/* Ini setting does not exist -- can this happen? */
				RETURN_LONG(old_error_reporting);
			}

			p = EG(error_reporting_ini_entry) = (zend_ini_entry*)Z_PTR_P(zv);
		}
		if (!p->modified) {
			if (!EG(modified_ini_directives)) {
				ALLOC_HASHTABLE(EG(modified_ini_directives));
				zend_hash_init(EG(modified_ini_directives), 8, NULL, NULL, 0);
			}
			if (EXPECTED(zend_hash_add_ptr(EG(modified_ini_directives), ZSTR_KNOWN(ZEND_STR_ERROR_REPORTING), p) != NULL)) {
				p->orig_value = p->value;
				p->orig_modifiable = p->modifiable;
				p->modified = 1;
			}
		} else if (p->orig_value != p->value) {
			zend_string_release_ex(p->value, 0);
		}

		p->value = zend_long_to_str(err);
		EG(error_reporting) = err;
	}

	RETURN_LONG(old_error_reporting);
}
/* }}} */

static bool validate_constant_array_argument(HashTable *ht, int argument_number) /* {{{ */
{
	bool ret = 1;
	zval *val;

	GC_PROTECT_RECURSION(ht);
	ZEND_HASH_FOREACH_VAL(ht, val) {
		ZVAL_DEREF(val);
		if (Z_TYPE_P(val) == IS_ARRAY && Z_REFCOUNTED_P(val)) {
			if (Z_IS_RECURSIVE_P(val)) {
				zend_argument_value_error(argument_number, "cannot be a recursive array");
				ret = 0;
				break;
			} else if (!validate_constant_array_argument(Z_ARRVAL_P(val), argument_number)) {
				ret = 0;
				break;
			}
		}
	} ZEND_HASH_FOREACH_END();
	GC_UNPROTECT_RECURSION(ht);
	return ret;
}
/* }}} */

static void copy_constant_array(zval *dst, zval *src) /* {{{ */
{
	zend_string *key;
	zend_ulong idx;
	zval *new_val, *val;

	array_init_size(dst, zend_hash_num_elements(Z_ARRVAL_P(src)));
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(src), idx, key, val) {
		/* constant arrays can't contain references */
		ZVAL_DEREF(val);
		if (key) {
			new_val = zend_hash_add_new(Z_ARRVAL_P(dst), key, val);
		} else {
			new_val = zend_hash_index_add_new(Z_ARRVAL_P(dst), idx, val);
		}
		if (Z_TYPE_P(val) == IS_ARRAY) {
			if (Z_REFCOUNTED_P(val)) {
				copy_constant_array(new_val, val);
			}
		} else {
			Z_TRY_ADDREF_P(val);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Define a new constant */
ZEND_FUNCTION(define)
{
	zend_string *name;
	zval *val, val_free;
	bool non_cs = 0;
	zend_constant c;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(name)
		Z_PARAM_ZVAL(val)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(non_cs)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_memnstr(ZSTR_VAL(name), "::", sizeof("::") - 1, ZSTR_VAL(name) + ZSTR_LEN(name))) {
		zend_argument_value_error(1, "cannot be a class constant");
		RETURN_THROWS();
	}

	if (non_cs) {
		zend_error(E_WARNING, "define(): Argument #3 ($case_insensitive) is ignored since declaration of case-insensitive constants is no longer supported");
	}

	ZVAL_UNDEF(&val_free);

	if (Z_TYPE_P(val) == IS_ARRAY) {
		if (Z_REFCOUNTED_P(val)) {
			if (!validate_constant_array_argument(Z_ARRVAL_P(val), 2)) {
				RETURN_THROWS();
			} else {
				copy_constant_array(&c.value, val);
				goto register_constant;
			}
		}
	}

	ZVAL_COPY(&c.value, val);
	zval_ptr_dtor(&val_free);

register_constant:
	/* non persistent */
	ZEND_CONSTANT_SET_FLAGS(&c, 0, PHP_USER_CONSTANT);
	c.name = zend_string_copy(name);
	if (zend_register_constant(&c) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Check whether a constant exists
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
ZEND_FUNCTION(defined)
{
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_get_constant_ex(name, zend_get_executed_scope(), ZEND_FETCH_CLASS_SILENT)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Retrieves the class name */
ZEND_FUNCTION(get_class)
{
	zval *obj = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|o", &obj) == FAILURE) {
		RETURN_THROWS();
	}

	if (!obj) {
		zend_class_entry *scope = zend_get_executed_scope();

		if (scope) {
			zend_error(E_DEPRECATED, "Calling get_class() without arguments is deprecated");
			if (UNEXPECTED(EG(exception))) {
				RETURN_THROWS();
			}
			RETURN_STR_COPY(scope->name);
		} else {
			zend_throw_error(NULL, "get_class() without arguments must be called from within a class");
			RETURN_THROWS();
		}
	}

	RETURN_STR_COPY(Z_OBJCE_P(obj)->name);
}
/* }}} */

/* {{{ Retrieves the "Late Static Binding" class name */
ZEND_FUNCTION(get_called_class)
{
	zend_class_entry *called_scope;

	ZEND_PARSE_PARAMETERS_NONE();

	called_scope = zend_get_called_scope(execute_data);
	if (!called_scope) {
		zend_throw_error(NULL, "get_called_class() must be called from within a class");
		RETURN_THROWS();
	}

	RETURN_STR_COPY(called_scope->name);
}
/* }}} */

/* {{{ Retrieves the parent class name for object or class or current scope or false if not in a scope. */
ZEND_FUNCTION(get_parent_class)
{
	zend_class_entry *ce = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OR_CLASS_NAME(ce)
	ZEND_PARSE_PARAMETERS_END();

	if (!ce) {
		zend_error(E_DEPRECATED, "Calling get_parent_class() without arguments is deprecated");
		if (UNEXPECTED(EG(exception))) {
			RETURN_THROWS();
		}
		ce = zend_get_executed_scope();
	}

	if (ce && ce->parent) {
		RETURN_STR_COPY(ce->parent->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

static void is_a_impl(INTERNAL_FUNCTION_PARAMETERS, bool only_subclass) /* {{{ */
{
	zval *obj;
	zend_string *class_name;
	zend_class_entry *instance_ce;
	zend_class_entry *ce;
	bool allow_string = only_subclass;
	bool retval;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(obj)
		Z_PARAM_STR(class_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(allow_string)
	ZEND_PARSE_PARAMETERS_END();
	/*
	 * allow_string - is_a default is no, is_subclass_of is yes.
	 *   if it's allowed, then the autoloader will be called if the class does not exist.
	 *   default behaviour is different, as 'is_a' used to be used to test mixed return values
	 *   and there is no easy way to deprecate this.
	 */

	if (allow_string && Z_TYPE_P(obj) == IS_STRING) {
		instance_ce = zend_lookup_class(Z_STR_P(obj));
		if (!instance_ce) {
			RETURN_FALSE;
		}
	} else if (Z_TYPE_P(obj) == IS_OBJECT) {
		instance_ce = Z_OBJCE_P(obj);
	} else {
		RETURN_FALSE;
	}

	if (!only_subclass && EXPECTED(zend_string_equals(instance_ce->name, class_name))) {
		retval = 1;
	} else {
		ce = zend_lookup_class_ex(class_name, NULL, ZEND_FETCH_CLASS_NO_AUTOLOAD);
		if (!ce) {
			retval = 0;
		} else {
			if (only_subclass && instance_ce == ce) {
				retval = 0;
			} else {
				retval = instanceof_function(instance_ce, ce);
			}
		}
	}

	RETURN_BOOL(retval);
}
/* }}} */

/* {{{ Returns true if the object has this class as one of its parents */
ZEND_FUNCTION(is_subclass_of)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Returns true if the first argument is an object and is this class or has this class as one of its parents, */
ZEND_FUNCTION(is_a)
{
	is_a_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *scope, zend_class_entry *ce, bool statics, zval *return_value)
{
	zend_property_info *prop_info;
	zval *prop, prop_copy;
	zend_string *key;
	zval *default_properties_table = CE_DEFAULT_PROPERTIES_TABLE(ce);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, key, prop_info) {
		if (((prop_info->flags & ZEND_ACC_PROTECTED) &&
			 !zend_check_protected(prop_info->ce, scope)) ||
			((prop_info->flags & ZEND_ACC_PRIVATE) &&
			  prop_info->ce != scope)) {
			continue;
		}
		prop = NULL;
		if (statics && (prop_info->flags & ZEND_ACC_STATIC) != 0) {
			prop = &ce->default_static_members_table[prop_info->offset];
			ZVAL_DEINDIRECT(prop);
		} else if (!statics && (prop_info->flags & ZEND_ACC_STATIC) == 0) {
			prop = &default_properties_table[OBJ_PROP_TO_NUM(prop_info->offset)];
		}
		if (!prop) {
			continue;
		}

		if (Z_ISUNDEF_P(prop)) {
			/* Return uninitialized typed properties as a null value */
			ZVAL_NULL(&prop_copy);
		} else {
			/* copy: enforce read only access */
			ZVAL_COPY_OR_DUP(&prop_copy, prop);
		}
		prop = &prop_copy;

		/* this is necessary to make it able to work with default array
		 * properties, returned to user */
		if (Z_OPT_TYPE_P(prop) == IS_CONSTANT_AST) {
			if (UNEXPECTED(zval_update_constant_ex(prop, ce) != SUCCESS)) {
				return;
			}
		}

		zend_hash_add_new(Z_ARRVAL_P(return_value), key, prop);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns an array of default properties of the class. */
ZEND_FUNCTION(get_class_vars)
{
	zend_class_entry *ce = NULL, *scope;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "C", &ce) == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	if (UNEXPECTED(!(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED))) {
		if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
			return;
		}
	}

	scope = zend_get_executed_scope();
	add_class_vars(scope, ce, 0, return_value);
	add_class_vars(scope, ce, 1, return_value);
}
/* }}} */

/* {{{ Returns an array of object properties */
ZEND_FUNCTION(get_object_vars)
{
	zval *value;
	HashTable *properties;
	zend_string *key;
	zend_object *zobj;
	zend_ulong num_key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(zobj)
	ZEND_PARSE_PARAMETERS_END();

	properties = zobj->handlers->get_properties(zobj);
	if (properties == NULL) {
		RETURN_EMPTY_ARRAY();
	}

	if (!zobj->ce->default_properties_count && properties == zobj->properties && !GC_IS_RECURSIVE(properties)) {
		/* fast copy */
		if (EXPECTED(zobj->handlers == &std_object_handlers)) {
			RETURN_ARR(zend_proptable_to_symtable(properties, 0));
		}
		RETURN_ARR(zend_proptable_to_symtable(properties, 1));
	} else {
		array_init_size(return_value, zend_hash_num_elements(properties));

		ZEND_HASH_FOREACH_KEY_VAL(properties, num_key, key, value) {
			bool is_dynamic = 1;
			if (Z_TYPE_P(value) == IS_INDIRECT) {
				value = Z_INDIRECT_P(value);
				if (UNEXPECTED(Z_ISUNDEF_P(value))) {
					continue;
				}

				is_dynamic = 0;
			}

			if (key && zend_check_property_access(zobj, key, is_dynamic) == FAILURE) {
				continue;
			}

			if (Z_ISREF_P(value) && Z_REFCOUNT_P(value) == 1) {
				value = Z_REFVAL_P(value);
			}
			Z_TRY_ADDREF_P(value);

			if (UNEXPECTED(!key)) {
				/* This case is only possible due to loopholes, e.g. ArrayObject */
				zend_hash_index_add(Z_ARRVAL_P(return_value), num_key, value);
			} else if (!is_dynamic && ZSTR_VAL(key)[0] == 0) {
				const char *prop_name, *class_name;
				size_t prop_len;
				zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_len);
				/* We assume here that a mangled property name is never
				 * numeric. This is probably a safe assumption, but
				 * theoretically someone might write an extension with
				 * private, numeric properties. Well, too bad.
				 */
				zend_hash_str_add_new(Z_ARRVAL_P(return_value), prop_name, prop_len, value);
			} else {
				zend_symtable_add_new(Z_ARRVAL_P(return_value), key, value);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ Returns an array of mangled object properties. Does not respect property visibility. */
ZEND_FUNCTION(get_mangled_object_vars)
{
	zend_object *obj;
	HashTable *properties;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ(obj)
	ZEND_PARSE_PARAMETERS_END();

	properties = obj->handlers->get_properties(obj);
	if (!properties) {
		ZVAL_EMPTY_ARRAY(return_value);
		return;
	}

	properties = zend_proptable_to_symtable(properties,
		(obj->ce->default_properties_count ||
		 obj->handlers != &std_object_handlers ||
		 GC_IS_RECURSIVE(properties)));
	RETURN_ARR(properties);
}
/* }}} */

/* {{{ Returns an array of method names for class or class instance. */
ZEND_FUNCTION(get_class_methods)
{
	zval method_name;
	zend_class_entry *ce = NULL;
	zend_class_entry *scope;
	zend_function *mptr;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OR_CLASS_NAME(ce)
	ZEND_PARSE_PARAMETERS_END();

	array_init(return_value);
	scope = zend_get_executed_scope();

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, mptr) {
		if ((mptr->common.fn_flags & ZEND_ACC_PUBLIC)
		 || (scope &&
			 (((mptr->common.fn_flags & ZEND_ACC_PROTECTED) &&
			   zend_check_protected(mptr->common.scope, scope))
		   || ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) &&
			   scope == mptr->common.scope)))
		) {
			ZVAL_STR_COPY(&method_name, mptr->common.function_name);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &method_name);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Checks if the class method exists */
ZEND_FUNCTION(method_exists)
{
	zval *klass;
	zend_string *method_name;
	zend_string *lcname;
	zend_class_entry *ce;
	zend_function *func;

	/* We do not use Z_PARAM_OBJ_OR_STR here to be able to exclude int, float, and bool which are bogus class names */
	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(klass)
		Z_PARAM_STR(method_name)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE_P(klass) == IS_OBJECT) {
		ce = Z_OBJCE_P(klass);
	} else if (Z_TYPE_P(klass) == IS_STRING) {
		if ((ce = zend_lookup_class(Z_STR_P(klass))) == NULL) {
			RETURN_FALSE;
		}
	} else {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_value_name(klass));
		RETURN_THROWS();
	}

	lcname = zend_string_tolower(method_name);
	func = zend_hash_find_ptr(&ce->function_table, lcname);
	zend_string_release_ex(lcname, 0);

	if (func) {
		/* Exclude shadow properties when checking a method on a specific class. Include
		 * them when checking an object, as method_exists() generally ignores visibility.
		 * TODO: Should we use EG(scope) for the object case instead? */
		RETURN_BOOL(Z_TYPE_P(klass) == IS_OBJECT
			|| !(func->common.fn_flags & ZEND_ACC_PRIVATE) || func->common.scope == ce);
	}

	if (Z_TYPE_P(klass) == IS_OBJECT) {
		zend_object *obj = Z_OBJ_P(klass);
		func = Z_OBJ_HT_P(klass)->get_method(&obj, method_name, NULL);
		if (func != NULL) {
			if (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
				/* Returns true for the fake Closure's __invoke */
				RETVAL_BOOL(func->common.scope == zend_ce_closure
					&& zend_string_equals_literal_ci(method_name, ZEND_INVOKE_FUNC_NAME));

				zend_string_release_ex(func->common.function_name, 0);
				zend_free_trampoline(func);
				return;
			}
			RETURN_TRUE;
		}
	} else {
	    /* Returns true for fake Closure::__invoke */
	    if (ce == zend_ce_closure
	        && zend_string_equals_literal_ci(method_name, ZEND_INVOKE_FUNC_NAME)) {
	        RETURN_TRUE;
	    }
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Checks if the object or class has a property */
ZEND_FUNCTION(property_exists)
{
	zval *object;
	zend_string *property;
	zend_class_entry *ce;
	zend_property_info *property_info;

	/* We do not use Z_PARAM_OBJ_OR_STR here to be able to exclude int, float, and bool which are bogus class names */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zS", &object, &property) == FAILURE) {
		RETURN_THROWS();
	}

	if (Z_TYPE_P(object) == IS_STRING) {
		ce = zend_lookup_class(Z_STR_P(object));
		if (!ce) {
			RETURN_FALSE;
		}
	} else if (Z_TYPE_P(object) == IS_OBJECT) {
		ce = Z_OBJCE_P(object);
	} else {
		zend_argument_type_error(1, "must be of type object|string, %s given", zend_zval_value_name(object));
		RETURN_THROWS();
	}

	property_info = zend_hash_find_ptr(&ce->properties_info, property);
	if (property_info != NULL
	 && (!(property_info->flags & ZEND_ACC_PRIVATE)
	  || property_info->ce == ce)) {
		RETURN_TRUE;
	}

	if (Z_TYPE_P(object) ==  IS_OBJECT &&
		Z_OBJ_HANDLER_P(object, has_property)(Z_OBJ_P(object), property, 2, NULL)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

static inline void class_exists_impl(INTERNAL_FUNCTION_PARAMETERS, int flags, int skip_flags) /* {{{ */
{
	zend_string *name;
	zend_string *lcname;
	zend_class_entry *ce;
	bool autoload = 1;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(autoload)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_HAS_CE_CACHE(name)) {
		ce = ZSTR_GET_CE_CACHE(name);
		if (ce) {
			RETURN_BOOL(((ce->ce_flags & flags) == flags) && !(ce->ce_flags & skip_flags));
		}
	}

	if (!autoload) {
		if (ZSTR_VAL(name)[0] == '\\') {
			/* Ignore leading "\" */
			lcname = zend_string_alloc(ZSTR_LEN(name) - 1, 0);
			zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
		} else {
			lcname = zend_string_tolower(name);
		}

		ce = zend_hash_find_ptr(EG(class_table), lcname);
		zend_string_release_ex(lcname, 0);
	} else {
		ce = zend_lookup_class(name);
	}

	if (ce) {
		RETURN_BOOL(((ce->ce_flags & flags) == flags) && !(ce->ce_flags & skip_flags));
	} else {
		RETURN_FALSE;
	}
}
/* {{{ */

/* {{{ Checks if the class exists */
ZEND_FUNCTION(class_exists)
{
	class_exists_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_LINKED, ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT);
}
/* }}} */

/* {{{ Checks if the class exists */
ZEND_FUNCTION(interface_exists)
{
	class_exists_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_LINKED|ZEND_ACC_INTERFACE, 0);
}
/* }}} */

/* {{{ Checks if the trait exists */
ZEND_FUNCTION(trait_exists)
{
	class_exists_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_TRAIT, 0);
}
/* }}} */

ZEND_FUNCTION(enum_exists)
{
	class_exists_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ENUM, 0);
}

/* {{{ Checks if the function exists */
ZEND_FUNCTION(function_exists)
{
	zend_string *name;
	bool exists;
	zend_string *lcname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_VAL(name)[0] == '\\') {
		/* Ignore leading "\" */
		lcname = zend_string_alloc(ZSTR_LEN(name) - 1, 0);
		zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(name) + 1, ZSTR_LEN(name) - 1);
	} else {
		lcname = zend_string_tolower(name);
	}

	exists = zend_hash_exists(EG(function_table), lcname);
	zend_string_release_ex(lcname, 0);

	RETURN_BOOL(exists);
}
/* }}} */

/* {{{ Creates an alias for user defined class */
ZEND_FUNCTION(class_alias)
{
	zend_string *class_name;
	zend_string *alias_name;
	zend_class_entry *ce;
	bool autoload = 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(class_name)
		Z_PARAM_STR(alias_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(autoload)
	ZEND_PARSE_PARAMETERS_END();

	ce = zend_lookup_class_ex(class_name, NULL, !autoload ? ZEND_FETCH_CLASS_NO_AUTOLOAD : 0);

	if (ce) {
		if (zend_register_class_alias_ex(ZSTR_VAL(alias_name), ZSTR_LEN(alias_name), ce, false) == SUCCESS) {
			RETURN_TRUE;
		} else {
			zend_error(E_WARNING, "Cannot declare %s %s, because the name is already in use", zend_get_object_type(ce), ZSTR_VAL(alias_name));
			RETURN_FALSE;
		}
	} else {
		zend_error(E_WARNING, "Class \"%s\" not found", ZSTR_VAL(class_name));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns an array with the file names that were include_once()'d */
ZEND_FUNCTION(get_included_files)
{
	zend_string *entry;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	ZEND_HASH_MAP_FOREACH_STR_KEY(&EG(included_files), entry) {
		if (entry) {
			add_next_index_str(return_value, zend_string_copy(entry));
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Generates a user-level error/warning/notice message */
ZEND_FUNCTION(trigger_error)
{
	zend_long error_type = E_USER_NOTICE;
	char *message;
	size_t message_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &message, &message_len, &error_type) == FAILURE) {
		RETURN_THROWS();
	}

	switch (error_type) {
		case E_USER_ERROR:
		case E_USER_WARNING:
		case E_USER_NOTICE:
		case E_USER_DEPRECATED:
			break;
		default:
			zend_argument_value_error(2, "must be one of E_USER_ERROR, E_USER_WARNING, E_USER_NOTICE,"
				" or E_USER_DEPRECATED");
			RETURN_THROWS();
			break;
	}

	zend_error((int)error_type, "%s", message);
	// TODO Change to void
	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets a user-defined error handler function.  Returns the previously defined error handler, or false on error */
ZEND_FUNCTION(set_error_handler)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_long error_type = E_ALL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_FUNC_OR_NULL(fci, fcc)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(error_type)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
		ZVAL_COPY(return_value, &EG(user_error_handler));
	}

	zend_stack_push(&EG(user_error_handlers_error_reporting), &EG(user_error_handler_error_reporting));
	zend_stack_push(&EG(user_error_handlers), &EG(user_error_handler));

	if (!ZEND_FCI_INITIALIZED(fci)) { /* unset user-defined handler */
		ZVAL_UNDEF(&EG(user_error_handler));
		return;
	}

	ZVAL_COPY(&EG(user_error_handler), &(fci.function_name));
	EG(user_error_handler_error_reporting) = (int)error_type;
}
/* }}} */

/* {{{ Restores the previously defined error handler function */
ZEND_FUNCTION(restore_error_handler)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (Z_TYPE(EG(user_error_handler)) != IS_UNDEF) {
		zval zeh;

		ZVAL_COPY_VALUE(&zeh, &EG(user_error_handler));
		ZVAL_UNDEF(&EG(user_error_handler));
		zval_ptr_dtor(&zeh);
	}

	if (zend_stack_is_empty(&EG(user_error_handlers))) {
		ZVAL_UNDEF(&EG(user_error_handler));
	} else {
		zval *tmp;
		EG(user_error_handler_error_reporting) = zend_stack_int_top(&EG(user_error_handlers_error_reporting));
		zend_stack_del_top(&EG(user_error_handlers_error_reporting));
		tmp = zend_stack_top(&EG(user_error_handlers));
		ZVAL_COPY_VALUE(&EG(user_error_handler), tmp);
		zend_stack_del_top(&EG(user_error_handlers));
	}

	// TODO Change to void
	RETURN_TRUE;
}
/* }}} */

/* {{{ Sets a user-defined exception handler function. Returns the previously defined exception handler, or false on error */
ZEND_FUNCTION(set_exception_handler)
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_FUNC_OR_NULL(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
		ZVAL_COPY(return_value, &EG(user_exception_handler));
	}

	zend_stack_push(&EG(user_exception_handlers), &EG(user_exception_handler));

	if (!ZEND_FCI_INITIALIZED(fci)) { /* unset user-defined handler */
		ZVAL_UNDEF(&EG(user_exception_handler));
		return;
	}

	ZVAL_COPY(&EG(user_exception_handler), &(fci.function_name));
}
/* }}} */

/* {{{ Restores the previously defined exception handler function */
ZEND_FUNCTION(restore_exception_handler)
{
	ZEND_PARSE_PARAMETERS_NONE();

	if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
		zval_ptr_dtor(&EG(user_exception_handler));
	}
	if (zend_stack_is_empty(&EG(user_exception_handlers))) {
		ZVAL_UNDEF(&EG(user_exception_handler));
	} else {
		zval *tmp = zend_stack_top(&EG(user_exception_handlers));
		ZVAL_COPY_VALUE(&EG(user_exception_handler), tmp);
		zend_stack_del_top(&EG(user_exception_handlers));
	}

	// TODO Change to void
	RETURN_TRUE;
}
/* }}} */

static inline void get_declared_class_impl(INTERNAL_FUNCTION_PARAMETERS, int flags) /* {{{ */
{
	zend_string *key;
	zval *zv;
	zend_class_entry *ce;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
	ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(EG(class_table), key, zv) {
			ce = Z_PTR_P(zv);
			if ((ce->ce_flags & (ZEND_ACC_LINKED|ZEND_ACC_INTERFACE|ZEND_ACC_TRAIT)) == flags
			 && key
			 && ZSTR_VAL(key)[0] != 0) {
				ZEND_HASH_FILL_GROW();
				if (EXPECTED(Z_TYPE_P(zv) == IS_PTR)) {
					ZEND_HASH_FILL_SET_STR_COPY(ce->name);
				} else {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_ALIAS_PTR);
					ZEND_HASH_FILL_SET_STR_COPY(key);
				}
				ZEND_HASH_FILL_NEXT();
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FILL_END();
}
/* {{{ */

/* {{{ Returns an array of all declared traits. */
ZEND_FUNCTION(get_declared_traits)
{
	get_declared_class_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_LINKED | ZEND_ACC_TRAIT);
}
/* }}} */

/* {{{ Returns an array of all declared classes. */
ZEND_FUNCTION(get_declared_classes)
{
	get_declared_class_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_LINKED);
}
/* }}} */

/* {{{ Returns an array of all declared interfaces. */
ZEND_FUNCTION(get_declared_interfaces)
{
	get_declared_class_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_LINKED | ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ Returns an array of all defined functions */
ZEND_FUNCTION(get_defined_functions)
{
	zval internal, user;
	zend_string *key;
	zend_function *func;
	bool exclude_disabled = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &exclude_disabled) == FAILURE) {
		RETURN_THROWS();
	}

	if (exclude_disabled == 0) {
		zend_error(E_DEPRECATED,
			"get_defined_functions(): Setting $exclude_disabled to false has no effect");
	}

	array_init(&internal);
	array_init(&user);
	array_init(return_value);

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(EG(function_table), key, func) {
		if (key && ZSTR_VAL(key)[0] != 0) {
			if (func->type == ZEND_INTERNAL_FUNCTION) {
				add_next_index_str(&internal, zend_string_copy(key));
			} else if (func->type == ZEND_USER_FUNCTION) {
				add_next_index_str(&user, zend_string_copy(key));
			}
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_str_add_new(Z_ARRVAL_P(return_value), "internal", sizeof("internal")-1, &internal);
	zend_hash_add_new(Z_ARRVAL_P(return_value), ZSTR_KNOWN(ZEND_STR_USER), &user);
}
/* }}} */

/* {{{ Returns an associative array of names and values of all currently defined variable names (variables in the current scope) */
ZEND_FUNCTION(get_defined_vars)
{
	zend_array *symbol_table;

	ZEND_PARSE_PARAMETERS_NONE();

	if (zend_forbid_dynamic_call() == FAILURE) {
		return;
	}

	symbol_table = zend_rebuild_symbol_table();
	if (UNEXPECTED(symbol_table == NULL)) {
		RETURN_EMPTY_ARRAY();
	}

	RETURN_ARR(zend_array_dup(symbol_table));
}
/* }}} */

#if ZEND_DEBUG && defined(ZTS)
ZEND_FUNCTION(zend_thread_id)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG((zend_long)tsrm_thread_id());
}
#endif

/* {{{ Get the resource type name for a given resource */
ZEND_FUNCTION(get_resource_type)
{
	const char *resource_type;
	zval *z_resource_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &z_resource_type) == FAILURE) {
		RETURN_THROWS();
	}

	resource_type = zend_rsrc_list_get_rsrc_type(Z_RES_P(z_resource_type));
	if (resource_type) {
		RETURN_STRING(resource_type);
	} else {
		RETURN_STRING("Unknown");
	}
}
/* }}} */

/* {{{ Get the resource ID for a given resource */
ZEND_FUNCTION(get_resource_id)
{
	zval *resource;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(resource)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(Z_RES_HANDLE_P(resource));
}
/* }}} */

/* {{{ Get an array with all active resources */
ZEND_FUNCTION(get_resources)
{
	zend_string *type = NULL;
	zend_string *key;
	zend_ulong index;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!", &type) == FAILURE) {
		RETURN_THROWS();
	}

	if (!type) {
		array_init(return_value);
		ZEND_HASH_FOREACH_KEY_VAL(&EG(regular_list), index, key, val) {
			if (!key) {
				Z_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), index, val);
			}
		} ZEND_HASH_FOREACH_END();
	} else if (zend_string_equals_literal(type, "Unknown")) {
		array_init(return_value);
		ZEND_HASH_FOREACH_KEY_VAL(&EG(regular_list), index, key, val) {
			if (!key && Z_RES_TYPE_P(val) <= 0) {
				Z_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), index, val);
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		int id = zend_fetch_list_dtor_id(ZSTR_VAL(type));

		if (id <= 0) {
			zend_argument_value_error(1, "must be a valid resource type");
			RETURN_THROWS();
		}

		array_init(return_value);
		ZEND_HASH_FOREACH_KEY_VAL(&EG(regular_list), index, key, val) {
			if (!key && Z_RES_TYPE_P(val) == id) {
				Z_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), index, val);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static void add_zendext_info(zend_extension *ext, void *arg) /* {{{ */
{
	zval *name_array = (zval *)arg;
	add_next_index_string(name_array, ext->name);
}
/* }}} */

/* {{{ Return an array containing names of loaded extensions */
ZEND_FUNCTION(get_loaded_extensions)
{
	bool zendext = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &zendext) == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	if (zendext) {
		zend_llist_apply_with_argument(&zend_extensions, (llist_apply_with_arg_func_t) add_zendext_info, return_value);
	} else {
		zend_module_entry *module;

		ZEND_HASH_MAP_FOREACH_PTR(&module_registry, module) {
			add_next_index_string(return_value, module->name);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ Return an array containing the names and values of all defined constants */
ZEND_FUNCTION(get_defined_constants)
{
	bool categorize = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &categorize) == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	if (categorize) {
		zend_constant *val;
		int module_number;
		zval *modules, const_val;
		char **module_names;
		zend_module_entry *module;
		int i = 1;

		modules = ecalloc(zend_hash_num_elements(&module_registry) + 2, sizeof(zval));
		module_names = emalloc((zend_hash_num_elements(&module_registry) + 2) * sizeof(char *));

		module_names[0] = "internal";
		ZEND_HASH_MAP_FOREACH_PTR(&module_registry, module) {
			module_names[module->module_number] = (char *)module->name;
			i++;
		} ZEND_HASH_FOREACH_END();
		module_names[i] = "user";

		ZEND_HASH_MAP_FOREACH_PTR(EG(zend_constants), val) {
			if (!val->name) {
				/* skip special constants */
				continue;
			}

			if (ZEND_CONSTANT_MODULE_NUMBER(val) == PHP_USER_CONSTANT) {
				module_number = i;
			} else if (ZEND_CONSTANT_MODULE_NUMBER(val) > i) {
				/* should not happen */
				continue;
			} else {
				module_number = ZEND_CONSTANT_MODULE_NUMBER(val);
			}

			if (Z_TYPE(modules[module_number]) == IS_UNDEF) {
				array_init(&modules[module_number]);
				add_assoc_zval(return_value, module_names[module_number], &modules[module_number]);
			}

			ZVAL_COPY_OR_DUP(&const_val, &val->value);
			zend_hash_add_new(Z_ARRVAL(modules[module_number]), val->name, &const_val);
		} ZEND_HASH_FOREACH_END();

		efree(module_names);
		efree(modules);
	} else {
		zend_constant *constant;
		zval const_val;

		ZEND_HASH_MAP_FOREACH_PTR(EG(zend_constants), constant) {
			if (!constant->name) {
				/* skip special constants */
				continue;
			}
			ZVAL_COPY_OR_DUP(&const_val, &constant->value);
			zend_hash_add_new(Z_ARRVAL_P(return_value), constant->name, &const_val);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

static void debug_backtrace_get_args(zend_execute_data *call, zval *arg_array) /* {{{ */
{
	uint32_t num_args = ZEND_CALL_NUM_ARGS(call);

	if (num_args) {
		uint32_t i = 0;
		zval *p = ZEND_CALL_ARG(call, 1);

		array_init_size(arg_array, num_args);
		zend_hash_real_init_packed(Z_ARRVAL_P(arg_array));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(arg_array)) {
			if (call->func->type == ZEND_USER_FUNCTION) {
				uint32_t first_extra_arg = MIN(num_args, call->func->op_array.num_args);

				if (UNEXPECTED(ZEND_CALL_INFO(call) & ZEND_CALL_HAS_SYMBOL_TABLE)) {
					/* In case of attached symbol_table, values on stack may be invalid
					 * and we have to access them through symbol_table
					 * See: https://bugs.php.net/bug.php?id=73156
					 */
					while (i < first_extra_arg) {
						zend_string *arg_name = call->func->op_array.vars[i];
						zval original_arg;
						zval *arg = zend_hash_find_ex_ind(call->symbol_table, arg_name, 1);
						zend_attribute *attribute = zend_get_parameter_attribute_str(
							call->func->common.attributes,
							"sensitiveparameter",
							sizeof("sensitiveparameter") - 1,
							i
						);

						bool is_sensitive = attribute != NULL;

						if (arg) {
							ZVAL_DEREF(arg);
							ZVAL_COPY_VALUE(&original_arg, arg);
						} else {
							ZVAL_NULL(&original_arg);
						}

						if (is_sensitive) {
							zval redacted_arg;
							object_init_ex(&redacted_arg, zend_ce_sensitive_parameter_value);
							zend_call_method_with_1_params(Z_OBJ_P(&redacted_arg), zend_ce_sensitive_parameter_value, &zend_ce_sensitive_parameter_value->constructor, "__construct", NULL, &original_arg);
							ZEND_HASH_FILL_SET(&redacted_arg);
						} else {
							Z_TRY_ADDREF_P(&original_arg);
							ZEND_HASH_FILL_SET(&original_arg);
						}

						ZEND_HASH_FILL_NEXT();
						i++;
					}
				} else {
					while (i < first_extra_arg) {
						zval original_arg;
						zend_attribute *attribute = zend_get_parameter_attribute_str(
							call->func->common.attributes,
							"sensitiveparameter",
							sizeof("sensitiveparameter") - 1,
							i
						);
						bool is_sensitive = attribute != NULL;

						if (EXPECTED(Z_TYPE_INFO_P(p) != IS_UNDEF)) {
							zval *arg = p;
							ZVAL_DEREF(arg);
							ZVAL_COPY_VALUE(&original_arg, arg);
						} else {
							ZVAL_NULL(&original_arg);
						}

						if (is_sensitive) {
							zval redacted_arg;
							object_init_ex(&redacted_arg, zend_ce_sensitive_parameter_value);
							zend_call_method_with_1_params(Z_OBJ_P(&redacted_arg), zend_ce_sensitive_parameter_value, &zend_ce_sensitive_parameter_value->constructor, "__construct", NULL, &original_arg);
							ZEND_HASH_FILL_SET(&redacted_arg);
						} else {
							Z_TRY_ADDREF_P(&original_arg);
							ZEND_HASH_FILL_SET(&original_arg);
						}

						ZEND_HASH_FILL_NEXT();
						p++;
						i++;
					}
				}
				p = ZEND_CALL_VAR_NUM(call, call->func->op_array.last_var + call->func->op_array.T);
			}

			while (i < num_args) {
				zval original_arg;
				bool is_sensitive = 0;

				if (i < call->func->common.num_args || call->func->common.fn_flags & ZEND_ACC_VARIADIC) {
					zend_attribute *attribute = zend_get_parameter_attribute_str(
						call->func->common.attributes,
						"sensitiveparameter",
						sizeof("sensitiveparameter") - 1,
						MIN(i, call->func->common.num_args)
					);
					is_sensitive = attribute != NULL;
				}

				if (EXPECTED(Z_TYPE_INFO_P(p) != IS_UNDEF)) {
					zval *arg = p;
					ZVAL_DEREF(arg);
					ZVAL_COPY_VALUE(&original_arg, arg);
				} else {
					ZVAL_NULL(&original_arg);
				}

				if (is_sensitive) {
					zval redacted_arg;
					object_init_ex(&redacted_arg, zend_ce_sensitive_parameter_value);
					zend_call_method_with_1_params(Z_OBJ_P(&redacted_arg), zend_ce_sensitive_parameter_value, &zend_ce_sensitive_parameter_value->constructor, "__construct", NULL, &original_arg);
					ZEND_HASH_FILL_SET(&redacted_arg);
				} else {
					Z_TRY_ADDREF_P(&original_arg);
					ZEND_HASH_FILL_SET(&original_arg);
				}

				ZEND_HASH_FILL_NEXT();
				p++;
				i++;
			}
		} ZEND_HASH_FILL_END();
		Z_ARRVAL_P(arg_array)->nNumOfElements = num_args;
	} else {
		ZVAL_EMPTY_ARRAY(arg_array);
	}

	if (ZEND_CALL_INFO(call) & ZEND_CALL_HAS_EXTRA_NAMED_PARAMS) {
		zend_string *name;
		zval *arg;
		SEPARATE_ARRAY(arg_array);
		ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(call->extra_named_params, name, arg) {
			ZVAL_DEREF(arg);
			Z_TRY_ADDREF_P(arg);
			zend_hash_add_new(Z_ARRVAL_P(arg_array), name, arg);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ */
ZEND_FUNCTION(debug_print_backtrace)
{
	zend_long options = 0;
	zend_long limit = 0;
	zval backtrace;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ll", &options, &limit) == FAILURE) {
		RETURN_THROWS();
	}

	zend_fetch_debug_backtrace(&backtrace, 1, options, limit);
	ZEND_ASSERT(Z_TYPE(backtrace) == IS_ARRAY);

	zend_string *str = zend_trace_to_string(Z_ARRVAL(backtrace), /* include_main */ false);
	ZEND_WRITE(ZSTR_VAL(str), ZSTR_LEN(str));
	zend_string_release(str);
	zval_ptr_dtor(&backtrace);
}

/* }}} */

ZEND_API void zend_fetch_debug_backtrace(zval *return_value, int skip_last, int options, int limit) /* {{{ */
{
	zend_execute_data *call;
	zend_object *object;
	bool fake_frame = 0;
	int lineno, frameno = 0;
	zend_function *func;
	zend_string *filename;
	zend_string *include_filename = NULL;
	zval tmp;
	HashTable *stack_frame;

	array_init(return_value);

	call = EG(current_execute_data);
	if (!call) {
		return;
	}

	if (EG(filename_override)) {
		// Add the current execution point to the frame so we don't lose it
		zend_string *filename_override = EG(filename_override);
		zend_long lineno_override = EG(lineno_override);
		EG(filename_override) = NULL;
		EG(lineno_override) = -1;

		zend_string *filename = zend_get_executed_filename_ex();
		zend_long lineno = zend_get_executed_lineno();
		if (filename && (!zend_string_equals(filename, filename_override) || lineno != lineno_override)) {
			stack_frame = zend_new_array(8);
			zend_hash_real_init_mixed(stack_frame);
			ZVAL_STR_COPY(&tmp, filename);
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_FILE), &tmp, 1);
			ZVAL_LONG(&tmp, lineno);
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_LINE), &tmp, 1);
			ZVAL_STR_COPY(&tmp, ZSTR_KNOWN(ZEND_STR_CONST_EXPR_PLACEHOLDER));
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_FUNCTION), &tmp, 1);
			ZVAL_ARR(&tmp, stack_frame);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		}

		EG(filename_override) = filename_override;
		EG(lineno_override) = lineno_override;
	}

	if (skip_last) {
		/* skip debug_backtrace() */
		call = call->prev_execute_data;
	}

	while (call && (limit == 0 || frameno < limit)) {
		zend_execute_data *prev = call->prev_execute_data;

		if (!prev) {
			/* add frame for a handler call without {main} code */
			if (EXPECTED((ZEND_CALL_INFO(call) & ZEND_CALL_TOP_FUNCTION) == 0)) {
				break;
			}
		} else if (UNEXPECTED((ZEND_CALL_INFO(call) & ZEND_CALL_GENERATOR) != 0)) {
			prev = zend_generator_check_placeholder_frame(prev);
		}

		/* We use _zend_hash_append*() and the array must be preallocated */
		stack_frame = zend_new_array(8);
		zend_hash_real_init_mixed(stack_frame);

		if (prev && prev->func && ZEND_USER_CODE(prev->func->common.type)) {
			filename = prev->func->op_array.filename;
			if (prev->opline->opcode == ZEND_HANDLE_EXCEPTION) {
				if (EG(opline_before_exception)) {
					lineno = EG(opline_before_exception)->lineno;
				} else {
					lineno = prev->func->op_array.line_end;
				}
			} else {
				lineno = prev->opline->lineno;
			}
			ZVAL_STR_COPY(&tmp, filename);
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_FILE), &tmp, 1);
			ZVAL_LONG(&tmp, lineno);
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_LINE), &tmp, 1);

			/* try to fetch args only if an FCALL was just made - elsewise we're in the middle of a function
			 * and debug_backtrace() might have been called by the error_handler. in this case we don't
			 * want to pop anything of the argument-stack */
		} else {
			zend_execute_data *prev_call = prev;

			while (prev_call) {
				zend_execute_data *prev;

				if (prev_call &&
					prev_call->func &&
					!ZEND_USER_CODE(prev_call->func->common.type) &&
					!(prev_call->func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
					break;
				}

				prev = prev_call->prev_execute_data;
				if (prev && prev->func && ZEND_USER_CODE(prev->func->common.type)) {
					ZVAL_STR_COPY(&tmp, prev->func->op_array.filename);
					_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_FILE), &tmp, 1);
					ZVAL_LONG(&tmp, prev->opline->lineno);
					_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_LINE), &tmp, 1);
					break;
				}
				prev_call = prev;
			}
			filename = NULL;
		}

		func = call->func;
		if (!fake_frame && func->common.function_name) {
			ZVAL_STR_COPY(&tmp, func->common.function_name);
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_FUNCTION), &tmp, 1);

			if (Z_TYPE(call->This) == IS_OBJECT) {
				object = Z_OBJ(call->This);
				/* $this may be passed into regular internal functions */
				if (func->common.scope) {
					ZVAL_STR_COPY(&tmp, func->common.scope->name);
				} else if (object->handlers->get_class_name == zend_std_get_class_name) {
					ZVAL_STR_COPY(&tmp, object->ce->name);
				} else {
					ZVAL_STR(&tmp, object->handlers->get_class_name(object));
				}
				_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_CLASS), &tmp, 1);
				if ((options & DEBUG_BACKTRACE_PROVIDE_OBJECT) != 0) {
					ZVAL_OBJ_COPY(&tmp, object);
					_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_OBJECT), &tmp, 1);
				}

				ZVAL_INTERNED_STR(&tmp, ZSTR_KNOWN(ZEND_STR_OBJECT_OPERATOR));
				_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_TYPE), &tmp, 1);
			} else if (func->common.scope) {
				ZVAL_STR_COPY(&tmp, func->common.scope->name);
				_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_CLASS), &tmp, 1);
				ZVAL_INTERNED_STR(&tmp, ZSTR_KNOWN(ZEND_STR_PAAMAYIM_NEKUDOTAYIM));
				_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_TYPE), &tmp, 1);
			}

			if ((options & DEBUG_BACKTRACE_IGNORE_ARGS) == 0 &&
				func->type != ZEND_EVAL_CODE) {

				debug_backtrace_get_args(call, &tmp);
				_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_ARGS), &tmp, 1);
			}
		} else {
			/* i know this is kinda ugly, but i'm trying to avoid extra cycles in the main execution loop */
			bool build_filename_arg = 1;
			zend_string *pseudo_function_name;
			uint32_t include_kind = 0;
			if (prev && prev->func && ZEND_USER_CODE(prev->func->common.type) && prev->opline->opcode == ZEND_INCLUDE_OR_EVAL) {
				include_kind = prev->opline->extended_value;
			}

			switch (include_kind) {
				case ZEND_EVAL:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_EVAL);
					build_filename_arg = 0;
					break;
				case ZEND_INCLUDE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_INCLUDE);
					break;
				case ZEND_REQUIRE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_REQUIRE);
					break;
				case ZEND_INCLUDE_ONCE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_INCLUDE_ONCE);
					break;
				case ZEND_REQUIRE_ONCE:
					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_REQUIRE_ONCE);
					break;
				default:
					/* Skip dummy frame unless it is needed to preserve filename/lineno info. */
					if (!filename) {
						zend_array_destroy(stack_frame);
						goto skip_frame;
					}

					pseudo_function_name = ZSTR_KNOWN(ZEND_STR_UNKNOWN);
					build_filename_arg = 0;
					break;
			}

			if (build_filename_arg && include_filename) {
				zval arg_array;

				array_init(&arg_array);

				/* include_filename always points to the last filename of the last last called-function.
				   if we have called include in the frame above - this is the file we have included.
				 */

				ZVAL_STR_COPY(&tmp, include_filename);
				zend_hash_next_index_insert_new(Z_ARRVAL(arg_array), &tmp);
				_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_ARGS), &arg_array, 1);
			}

			ZVAL_INTERNED_STR(&tmp, pseudo_function_name);
			_zend_hash_append_ex(stack_frame, ZSTR_KNOWN(ZEND_STR_FUNCTION), &tmp, 1);
		}

		ZVAL_ARR(&tmp, stack_frame);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		frameno++;

skip_frame:
		if (UNEXPECTED(ZEND_CALL_KIND(call) == ZEND_CALL_TOP_FUNCTION)
		 && !fake_frame
		 && prev
		 && prev->func
		 && ZEND_USER_CODE(prev->func->common.type)
		 && prev->opline->opcode == ZEND_INCLUDE_OR_EVAL) {
			fake_frame = 1;
		} else {
			fake_frame = 0;
			include_filename = filename;
			call = prev;
		}
	}
}
/* }}} */

/* {{{ Return backtrace as array */
ZEND_FUNCTION(debug_backtrace)
{
	zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
	zend_long limit = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ll", &options, &limit) == FAILURE) {
		RETURN_THROWS();
	}

	zend_fetch_debug_backtrace(return_value, 1, options, limit);
}
/* }}} */

/* {{{ Returns true if the named extension is loaded */
ZEND_FUNCTION(extension_loaded)
{
	zend_string *extension_name;
	zend_string *lcname;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &extension_name) == FAILURE) {
		RETURN_THROWS();
	}

	lcname = zend_string_tolower(extension_name);
	if (zend_hash_exists(&module_registry, lcname)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	zend_string_release_ex(lcname, 0);
}
/* }}} */

/* {{{ Returns an array with the names of functions belonging to the named extension */
ZEND_FUNCTION(get_extension_funcs)
{
	zend_string *extension_name;
	zend_string *lcname;
	bool array;
	zend_module_entry *module;
	zend_function *zif;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &extension_name) == FAILURE) {
		RETURN_THROWS();
	}
	if (strncasecmp(ZSTR_VAL(extension_name), "zend", sizeof("zend"))) {
		lcname = zend_string_tolower(extension_name);
		module = zend_hash_find_ptr(&module_registry, lcname);
		zend_string_release_ex(lcname, 0);
	} else {
		module = zend_hash_str_find_ptr(&module_registry, "core", sizeof("core") - 1);
	}

	if (!module) {
		RETURN_FALSE;
	}

	if (module->functions) {
		/* avoid BC break, if functions list is empty, will return an empty array */
		array_init(return_value);
		array = 1;
	} else {
		array = 0;
	}

	ZEND_HASH_MAP_FOREACH_PTR(CG(function_table), zif) {
		if (zif->common.type == ZEND_INTERNAL_FUNCTION
			&& zif->internal_function.module == module) {
			if (!array) {
				array_init(return_value);
				array = 1;
			}
			add_next_index_str(return_value, zend_string_copy(zif->common.function_name));
		}
	} ZEND_HASH_FOREACH_END();

	if (!array) {
		RETURN_FALSE;
	}
}
/* }}} */
