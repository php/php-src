/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* 
 * TODO:
 * + provide a way for user to enable/disabling overloading of get/set/call
 *   individually 
 * - call original overloaded handlers if necessary
 * + use local copy of CE with NULL'ed out handler when calling object's
 *   overloaded function
 * - handle both OE_IS_OBJECT and OE_IS_ARRAY in the whole chain
 * + see how to fix the issue of object trying to set its own property inside
 *   the handler
 * + check if function exists in function table, then call it, otherwise
 *   call handler (aka AUTOLOAD in Perl)
 * + should it check for existing properties first before calling __get/__set:
 *   yes
 * + turn off all overloading handlers on a call to a handler
 * - pass array overloading info on to handlers?
 * - add unoverload() function
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_overload.h"

#ifndef ZEND_ENGINE_2
#if HAVE_OVERLOAD

#define GET_HANDLER  "__get"
#define SET_HANDLER  "__set"
#define CALL_HANDLER "__call"

#define DISABLE_HANDLERS(ce)          \
	(ce).handle_property_get  = NULL; \
	(ce).handle_property_set  = NULL; \
	(ce).handle_function_call = NULL;

typedef struct _oo_class_data {
	void (*handle_function_call)(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
	zval (*handle_property_get)(zend_property_reference *property_reference);
	int (*handle_property_set)(zend_property_reference *property_reference, zval *value);
	HashTable getters;
	HashTable setters;
} oo_class_data;

ZEND_DECLARE_MODULE_GLOBALS(overload)

function_entry overload_functions[] = {
	PHP_FE(overload,	NULL)
	{NULL, NULL, NULL}
};

zend_module_entry overload_module_entry = {
	STANDARD_MODULE_HEADER,
	"overload",
	overload_functions,
	PHP_MINIT(overload),
	PHP_MSHUTDOWN(overload),
	NULL,
	PHP_RSHUTDOWN(overload),
	PHP_MINFO(overload),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_OVERLOAD
ZEND_GET_MODULE(overload)
#endif

static void overloaded_class_dtor(oo_class_data *oo_data)
{
	zend_hash_destroy(&oo_data->getters);
	zend_hash_destroy(&oo_data->setters);
}

/* {{{ php_overload_init_globals */
static void php_overload_init_globals(zend_overload_globals *overload_globals TSRMLS_DC)
{
	zend_hash_init(&overload_globals->overloaded_classes, 10, NULL,
				   (dtor_func_t)overloaded_class_dtor, 1);
}
/* }}} */

/* {{{ php_overload_destroy_globals */
static void php_overload_destroy_globals(zend_overload_globals *overload_globals TSRMLS_DC)
{
	zend_hash_destroy(&overload_globals->overloaded_classes);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(overload) */
PHP_MINIT_FUNCTION(overload)
{
	ZEND_INIT_MODULE_GLOBALS(overload, php_overload_init_globals, php_overload_destroy_globals);

	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(overload) */
PHP_MSHUTDOWN_FUNCTION(overload)
{
#ifdef ZTS
	ts_free_id(overload_globals_id);
#else
	php_overload_destroy_globals(&overload_globals TSRMLS_CC);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(overload) */
PHP_RSHUTDOWN_FUNCTION(overload)
{
	zend_hash_clean(&OOG(overloaded_classes));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(overload)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "User-Space Object Overloading Support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
 * In all three handlers, we save the original CE of the object, and replace it
 * with a temporary one that has all handlers turned off. This is to avoid
 * recursive calls to our handlers. We can't simply set a handler to NULL on the
 * original CE, as that would disable overloading on other objects of the same
 * class. After invoking the callback we restore the object's CE.
 */

/* {{{ static int call_get_handler() */
static int call_get_handler(zval *object, zval *prop_name, zval **prop_value TSRMLS_DC)
{
	int call_result;
	zend_class_entry temp_ce, *orig_ce;
	zval result, *result_ptr = &result;
	zval get_handler;
	zval **args[2];
	zval *retval = NULL;
	char *lcase_prop_name;
	zval **accessor_name;
	oo_class_data *oo_data;

	if (zend_hash_index_find(&OOG(overloaded_classes), (long)Z_OBJCE_P(object), (void**)&oo_data) == FAILURE) {
		php_error(E_WARNING, "internal problem trying to get property");
		return 0;
	}

	temp_ce = *Z_OBJCE_P(object);
	DISABLE_HANDLERS(temp_ce);
	orig_ce = Z_OBJCE_P(object);
	Z_OBJ_P(object)->ce = &temp_ce;

	result_ptr->is_ref = 1;
	result_ptr->refcount = 1;
	ZVAL_NULL(result_ptr);

	lcase_prop_name = estrndup(Z_STRVAL_P(prop_name), Z_STRLEN_P(prop_name));
	zend_str_tolower(lcase_prop_name, Z_STRLEN_P(prop_name));
	if (zend_hash_find(&oo_data->getters, lcase_prop_name,
					   Z_STRLEN_P(prop_name)+1, (void **)&accessor_name) == SUCCESS) {
		efree(lcase_prop_name);
		args[0] = &result_ptr;

		call_result = call_user_function_ex(NULL,
											&object,
											*accessor_name,
											&retval,
											1, args,
											0, NULL TSRMLS_CC);
		Z_OBJ_P(object)->ce = orig_ce;

		if (call_result == FAILURE || !retval) {
			php_error(E_WARNING, "unable to call %s::" GET_HANDLER "_%s() handler", Z_OBJCE_P(object)->name, Z_STRVAL_P(prop_name));
			return 0;
		}
	} else {
		efree(lcase_prop_name);
		ZVAL_STRINGL(&get_handler, GET_HANDLER, sizeof(GET_HANDLER)-1, 0);
		args[0] = &prop_name;
		args[1] = &result_ptr;

		call_result = call_user_function_ex(NULL,
											&object,
											&get_handler,
											&retval,
											2, args,
											0, NULL TSRMLS_CC);
		Z_OBJ_P(object)->ce = orig_ce;

		if (call_result == FAILURE || !retval) {
			php_error(E_WARNING, "unable to call %s::" GET_HANDLER "() handler", Z_OBJCE_P(object)->name);
			return 0;
		}
	}

	if (zval_is_true(retval)) {
		REPLACE_ZVAL_VALUE(prop_value, result_ptr, 0);
		zval_ptr_dtor(&retval);
		return 1;
	}

	zval_ptr_dtor(&retval);
	zval_dtor(result_ptr);

	if (!oo_data->handle_property_get) {
		return 0;
	}

	/* TODO: call original OO handler */

	return 0;
}
/* }}} */

/* {{{ static int call_set_handler() */
int call_set_handler(zval *object, zval *prop_name, zval *value TSRMLS_DC)
{
	int call_result;
	zend_class_entry temp_ce, *orig_ce;
	zval set_handler;
	zval *value_copy;
	zval **args[2];
	zval *retval = NULL;
	char *lcase_prop_name;
	zval **accessor_name;
	oo_class_data *oo_data;

	if (zend_hash_index_find(&OOG(overloaded_classes), (long)Z_OBJCE_P(object), (void**)&oo_data) == FAILURE) {
		php_error(E_WARNING, "internal problem trying to set property");
		return 0;
	}

	temp_ce = *Z_OBJCE_P(object);
	DISABLE_HANDLERS(temp_ce);
	orig_ce = Z_OBJCE_P(object);
	Z_OBJ_P(object)->ce = &temp_ce;

	if (value->refcount == 0) {
		MAKE_STD_ZVAL(value_copy);
		*value_copy = *value;
		zval_copy_ctor(value_copy);
		value = value_copy;
	}

	lcase_prop_name = estrndup(Z_STRVAL_P(prop_name), Z_STRLEN_P(prop_name));
	zend_str_tolower(lcase_prop_name, Z_STRLEN_P(prop_name));
	if (zend_hash_find(&oo_data->setters, lcase_prop_name,
					   Z_STRLEN_P(prop_name)+1, (void **)&accessor_name) == SUCCESS) {
		efree(lcase_prop_name);
		args[0] = &value;

		call_result = call_user_function_ex(NULL,
											&object,
											*accessor_name,
											&retval,
											1, args,
											0, NULL TSRMLS_CC);
		Z_OBJ_P(object)->ce = orig_ce;

		if (call_result == FAILURE || !retval) {
			php_error(E_WARNING, "unable to call %s::" SET_HANDLER "_%s() handler", Z_OBJCE_P(object)->name, Z_STRVAL_P(prop_name));
			return 0;
		}
	} else {
		efree(lcase_prop_name);
		ZVAL_STRINGL(&set_handler, SET_HANDLER, sizeof(SET_HANDLER)-1, 0);
		args[0] = &prop_name;
		args[1] = &value;

		call_result = call_user_function_ex(NULL,
											&object,
											&set_handler,
											&retval,
											2, args,
											0, NULL TSRMLS_CC);
		Z_OBJ_P(object)->ce = orig_ce;

		if (call_result == FAILURE || !retval) {
			php_error(E_WARNING, "unable to call %s::" SET_HANDLER "() handler", orig_ce->name);
			return 0;
		}
	}

	if (zval_is_true(retval)) {
		zval_ptr_dtor(&retval);
		return 1;
	}

	zval_ptr_dtor(&retval);

	if (!oo_data->handle_property_set) {
		return 0;
	}

	/* TODO: call original OO handler */

	return 0;

}
/* }}} */

#define CLEANUP_OO_CHAIN() { \
	for (; element; element=element->next) { \
		zval_dtor(&((zend_overloaded_element *)element->data)->element); \
	} \
} \

/* {{{ zval overload_get_property() */
static zval overload_get_property(zend_property_reference *property_reference)
{
	zval result;
	zval *result_ptr = &result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zval object = *property_reference->object;
	zval **real_prop;
	int got_prop = 0;
	TSRMLS_FETCH();

	INIT_PZVAL(result_ptr);

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		ZVAL_NULL(result_ptr);

		if (Z_TYPE_P(overloaded_property) == OE_IS_OBJECT) {
			/* Trying to access a property on a non-object. */
			if (Z_TYPE(object) != IS_OBJECT) {
				CLEANUP_OO_CHAIN();
				if (got_prop)
					zval_dtor(&object);
				return result;
			}

			if (zend_hash_find(Z_OBJPROP(object),
							   Z_STRVAL(overloaded_property->element),
							   Z_STRLEN(overloaded_property->element)+1,
							   (void **)&real_prop) == SUCCESS) {
				result = **real_prop;
				/* printf("is_ref: %d, refcount: %d\n", (*real_prop)->is_ref, (*real_prop)->refcount); */
				/* REPLACE_ZVAL_VALUE(&result_ptr, *real_prop, 1); */
			} else if (Z_OBJCE(object)->handle_property_get == overload_get_property &&
					   call_get_handler(&object,
										&overloaded_property->element,
										&result_ptr TSRMLS_CC)) {
				got_prop = 1;
			} else {
				php_error(E_NOTICE, "Undefined property: %s", Z_STRVAL(overloaded_property->element));	
				CLEANUP_OO_CHAIN();
				if (got_prop)
					zval_dtor(&object);
				return result;
			}
		} else if (Z_TYPE_P(overloaded_property) == OE_IS_ARRAY) {
			/* Trying to access index on a non-array. */
			if (Z_TYPE(object) != IS_ARRAY) {
				CLEANUP_OO_CHAIN();
				if (got_prop)
					zval_dtor(&object);
				return result;
			}

			if (Z_TYPE(overloaded_property->element) == IS_STRING) {
				if (zend_hash_find(Z_ARRVAL(object),
								   Z_STRVAL(overloaded_property->element),
								   Z_STRLEN(overloaded_property->element)+1,
								   (void **)&real_prop) == FAILURE) {
					CLEANUP_OO_CHAIN();
					if (got_prop)
						zval_dtor(&object);
					return result;
				}
			} else if (Z_TYPE(overloaded_property->element) == IS_LONG) {
				if (zend_hash_index_find(Z_ARRVAL(object),
										  Z_LVAL(overloaded_property->element),
										  (void **)&real_prop) == FAILURE) {
					CLEANUP_OO_CHAIN();
					if (got_prop)
						zval_dtor(&object);
					return result;
				}
			}

			result = **real_prop;
		}

		zval_dtor(&overloaded_property->element);
		/* printf("got_prop: %d\n", got_prop); */
		if (element != property_reference->elements_list->head && got_prop) {
			zval_dtor(&object);
			got_prop = 0;
		}

		object = result;
	}

	if (!got_prop)
		zval_copy_ctor(&result);

	return result;
}
/* }}} */

/* {{{ int overload_set_property() */
static int overload_set_property(zend_property_reference *property_reference, zval *value)
{
	zval result;
	zval *result_ptr = &result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zval **object = &property_reference->object;
	TSRMLS_FETCH();

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		ZVAL_NULL(result_ptr);

		if (Z_TYPE_P(overloaded_property) == OE_IS_OBJECT) {
			/* Trying to access a property on a non-object. */
			if (Z_TYPE_PP(object) != IS_OBJECT) {
				CLEANUP_OO_CHAIN();
				return FAILURE;
			} 
			
			if (zend_hash_find(Z_OBJPROP_PP(object),
							   Z_STRVAL(overloaded_property->element),
							   Z_STRLEN(overloaded_property->element)+1,
							   (void **)&object) == FAILURE) {

				if (element == property_reference->elements_list->tail) {
					if (Z_OBJCE_PP(object)->handle_property_set == overload_set_property &&
						call_set_handler(*object,
										  &overloaded_property->element,
										  value TSRMLS_CC)) {
						CLEANUP_OO_CHAIN();
						return SUCCESS;
					} else {
						php_error(E_WARNING, "Unable to set property: %s", Z_STRVAL(overloaded_property->element));
						CLEANUP_OO_CHAIN();
						return FAILURE;
					}
				}

				if (Z_OBJCE_PP(object)->handle_property_get == overload_get_property &&
					call_get_handler(*object,
									 &overloaded_property->element,
									 &result_ptr TSRMLS_CC)) {
					object = &result_ptr;
				} else {
					php_error(E_NOTICE, "Undefined property: %s", Z_STRVAL(overloaded_property->element));	
					CLEANUP_OO_CHAIN();
					return FAILURE;
				}
			}
		} else if (Z_TYPE_P(overloaded_property) == OE_IS_ARRAY) {
		}

		zval_dtor(&overloaded_property->element);
	}

	/* printf("value is_ref: %d, refcount: %d\n", value->is_ref, value->refcount); */
	REPLACE_ZVAL_VALUE(object, value, 1);
	/* printf("object is_ref: %d, refcount: %d\n", (*object)->is_ref, (*object)->refcount); */

	return SUCCESS;
}
/* }}} */

/* {{{ void overload_call_method() */
static void overload_call_method(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zval ***args;
	zval *retval = NULL;
	int call_result;
	zend_bool use_call_handler = 1;
	zval *object = property_reference->object;
	zval call_handler, method_name, *method_name_ptr = &method_name;
	zend_overloaded_element *method = (zend_overloaded_element *)property_reference->elements_list->tail->data;

	/*
	 * We don't use the call handler if the invoked method exists in object's
	 * method table.
	 */
	if (zend_hash_exists(&Z_OBJCE_P(object)->function_table,
						 Z_STRVAL(method->element),
						 Z_STRLEN(method->element) + 1)) {
		use_call_handler = 0;
	}

	args = (zval ***)emalloc(ZEND_NUM_ARGS() * sizeof(zval **));

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		php_error(E_WARNING, "unable to obtain arguments");
		return;
	}

	if (use_call_handler) {
		zval **handler_args[3];
		zval *arg_array;
		zval result, *result_ptr = &result;
		zend_class_entry temp_ce, *orig_ce;
		int i;

		temp_ce = *Z_OBJCE_P(object);
		DISABLE_HANDLERS(temp_ce);
		orig_ce = Z_OBJCE_P(object);
		Z_OBJ_P(object)->ce = &temp_ce;

		ZVAL_STRINGL(&call_handler, CALL_HANDLER, sizeof(CALL_HANDLER)-1, 0);
		ZVAL_STRINGL(&method_name, Z_STRVAL(method->element), Z_STRLEN(method->element), 0);
		INIT_PZVAL(&call_handler);
		INIT_PZVAL(method_name_ptr);

		MAKE_STD_ZVAL(arg_array);
		array_init(arg_array);
		for (i = 0; i < ZEND_NUM_ARGS(); i++) {
			zval_add_ref(args[i]);
			add_next_index_zval(arg_array, *args[i]);
		}

		result_ptr->is_ref = 1;
		result_ptr->refcount = 1;
		ZVAL_NULL(result_ptr);
		
		handler_args[0] = &method_name_ptr;
		handler_args[1] = &arg_array;
		handler_args[2] = &result_ptr;
		call_result = call_user_function_ex(NULL,
											&object,
											&call_handler,
											&retval,
											3, handler_args,
											0, NULL TSRMLS_CC);
		Z_OBJ_P(object)->ce = orig_ce;
		zval_ptr_dtor(&arg_array);

		if (call_result == FAILURE || !retval) {
			efree(args);
			zval_dtor(result_ptr);
			php_error(E_WARNING, "unable to call %s::" CALL_HANDLER "() handler", Z_OBJCE_P(object)->name);
			return;
		}

		if (zval_is_true(retval)) {
			*return_value = *result_ptr;
			INIT_PZVAL(return_value);
		} else {
			zval_dtor(result_ptr);
			php_error(E_WARNING, "Call to undefined method %s::%s()", Z_OBJCE_P(object)->name, Z_STRVAL(method_name));
		}
		zval_ptr_dtor(&retval);
	} else {
		ZVAL_STRINGL(&call_handler, Z_STRVAL(method->element), Z_STRLEN(method->element), 0);
		call_result = call_user_function_ex(NULL,
											&object,
											&call_handler,
											&retval,
											ZEND_NUM_ARGS(), args,
											0, NULL TSRMLS_CC);

		if (call_result == FAILURE || !retval) {
			efree(args);
			php_error(E_WARNING, "unable to call %s::%s() method", Z_OBJCE_P(object)->name, Z_STRVAL(method->element));
			return;
		}

		return_value->value = retval->value;
		zval_copy_ctor(return_value);
		zval_ptr_dtor(&retval);
	}
	
	efree(args);
	zval_dtor(&method->element);
}
/* }}} */

/* {{{ static int locate_accessors() */
static int locate_accessors(zend_function *method, oo_class_data *oo_data TSRMLS_DC)
{
	zval *accessor_name;
	char *function_name = method->common.function_name;
	int function_name_len = strlen(method->common.function_name);

	if (!strncmp(function_name, GET_HANDLER "_", sizeof(GET_HANDLER "_")-1)) {
		MAKE_STD_ZVAL(accessor_name);
		ZVAL_STRINGL(accessor_name, function_name, function_name_len, 1);
		zend_hash_update(&oo_data->getters,
						 function_name + sizeof(GET_HANDLER "_") - 1, 
						 function_name_len - sizeof(GET_HANDLER "_") + 2,
						 (void *)&accessor_name, sizeof(zval *), NULL);

	} else if (!strncmp(function_name, SET_HANDLER "_", sizeof(SET_HANDLER "_")-1)) {
		MAKE_STD_ZVAL(accessor_name);
		ZVAL_STRINGL(accessor_name, function_name, function_name_len, 1);
		zend_hash_update(&oo_data->setters,
						 function_name + sizeof(SET_HANDLER "_") - 1, 
						 function_name_len - sizeof(SET_HANDLER "_") + 2,
						 (void *)&accessor_name, sizeof(zval *), NULL);
	}

	return 0;
}
/* }}} */

/* {{{ proto void overload(string class_entry)
    Enables property and method call overloading for a class. */
PHP_FUNCTION(overload)
{
	char *class_entry = NULL;
	int argc = ZEND_NUM_ARGS();
	int class_entry_len;
	zend_class_entry *ce = NULL;
	oo_class_data oo_data;

	if (zend_parse_parameters(argc TSRMLS_CC, "s/", &class_entry, &class_entry_len) == FAILURE) 
		return;

	zend_str_tolower(class_entry, class_entry_len);
	if (zend_hash_find(EG(class_table), class_entry, class_entry_len+1, (void**)&ce) == FAILURE) {
		php_error(E_WARNING, "%s() was unable to locate class '%s'", get_active_function_name(TSRMLS_C), class_entry);
		RETURN_FALSE;
	}

	/* Check if the handlers have already been installed for this class. */
	if (zend_hash_index_exists(&OOG(overloaded_classes), (long)ce)) {
		RETURN_TRUE;
	}

	zend_hash_init(&oo_data.getters, 10, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_init(&oo_data.setters, 10, NULL, ZVAL_PTR_DTOR, 0);

	zend_hash_apply_with_argument(&ce->function_table, (apply_func_arg_t)locate_accessors, (void *)&oo_data TSRMLS_CC);
	
	if (zend_hash_exists(&ce->function_table, GET_HANDLER, sizeof(GET_HANDLER)) ||
		zend_hash_num_elements(&oo_data.getters)) {
		oo_data.handle_property_get = ce->handle_property_get;
		ce->handle_property_get = overload_get_property;
	} else
		oo_data.handle_property_get = NULL;

	if (zend_hash_exists(&ce->function_table, SET_HANDLER, sizeof(SET_HANDLER)) ||
		zend_hash_num_elements(&oo_data.setters)) {
		oo_data.handle_property_set = ce->handle_property_set;
		ce->handle_property_set = overload_set_property;
	} else
		oo_data.handle_property_set = NULL;

	if (zend_hash_exists(&ce->function_table, CALL_HANDLER, sizeof(CALL_HANDLER))) {
		oo_data.handle_function_call = ce->handle_function_call;
		ce->handle_function_call = overload_call_method;
	} else
		oo_data.handle_function_call = NULL;

	zend_hash_index_update(&OOG(overloaded_classes), (long)ce, &oo_data, sizeof(oo_data), NULL);

	RETURN_TRUE;
}
/* }}} */

#endif /* HAVE_OVERLOAD */
#endif /* ZEND_ENGINE_2 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
