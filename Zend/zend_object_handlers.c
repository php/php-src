/* 
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_objects.h"
#include "zend_objects_API.h"
#include "zend_object_handlers.h"

#define DEBUG_OBJECT_HANDLERS 0

#define Z_OBJ_P(zval_p) zend_objects_get_address(zval_p)

/*
  __X accessors explanation:

  if we have __get and property that is not part of the properties array is
  requested, we call __get handler. If it fails, we return uninitialized.

  if we have __set and property that is not part of the properties array is
  set, we call __set handler. If it fails, we do not change the array.

  for both handlers above, when we are inside __get/__set, no further calls for
  __get/__set for these objects will be made, to prevent endless recursion and
  enable accessors to change properties array.

  if we have __call and method which is not part of the class function table is
  called, we cal __call handler. 
*/

static HashTable *zend_std_get_properties(zval *object TSRMLS_DC)
{
	zend_object *zobj;
	zobj = Z_OBJ_P(object);
	return zobj->properties;
}

static zval *zend_std_call_getter(zval *object, zval *member TSRMLS_DC)
{
	zval **call_args[1];
	zval *retval = NULL;
	zval __get_name;
	int call_result;
	
	/* __get handler is called with two arguments:
	   property name
	   return value for the object (by reference)

	   it should return whether the call was successfull or not
	*/
	INIT_PZVAL(&__get_name);
	ZVAL_STRINGL(&__get_name, ZEND_GET_FUNC_NAME, sizeof(ZEND_GET_FUNC_NAME)-1, 0);

	call_args[0] = &member;

	/* go call the __get handler */
	call_result = call_user_function_ex(NULL,
										&object,
										&__get_name,
										&retval,
										1, call_args,
										0, NULL TSRMLS_CC);

	/* 
	   call_result is if call_user_function gone OK.
	   retval returns the value that is received
	*/
	

	if (call_result == FAILURE) {
		zend_error(E_ERROR, "Could not call __get handler for class %s", Z_OBJCE_P(object)->name);
		return NULL;
	}

	retval->refcount--;
	
	return retval;
}

static int zend_std_call_setter(zval *object, zval *member, zval *value TSRMLS_DC)
{
	zval **call_args[2];
	zval *retval = NULL;
	zval __set_name;
	int call_result;
	int ret;
	
	/* __set handler is called with two arguments:
	   property name
	   value to be set

	   it should return whether the call was successfull or not
	*/
	INIT_PZVAL(&__set_name);
	ZVAL_STRINGL(&__set_name, ZEND_SET_FUNC_NAME, sizeof(ZEND_SET_FUNC_NAME)-1, 0);

	call_args[0] = &member;
	value->refcount++;
	call_args[1] = &value;

	/* go call the __set handler */
	call_result = call_user_function_ex(NULL,
										&object,
										&__set_name,
										&retval,
										2, call_args,
										0, NULL TSRMLS_CC);

	/* 
	   call_result is if call_user_function gone OK.
	   retval shows if __get method went OK.
	*/
	

	if (call_result == FAILURE) {
		zend_error(E_ERROR, "Could not call __set handler for class %s", Z_OBJCE_P(object)->name);
		return FAILURE;
	}

	zval_ptr_dtor(&value);

	if (retval && zval_is_true(retval)) {
		ret = SUCCESS;
	} else {
		ret = FAILURE;
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}
	
	return ret;
}


inline int zend_verify_property_access(zend_property_info *property_info, zend_class_entry *ce TSRMLS_DC)
{
	switch (property_info->flags & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			return 1;
		case ZEND_ACC_PRIVATE:
			if (ce == EG(scope)) {
				return 1;
			} else {
				return 0;
			}
			break;
		case ZEND_ACC_PROTECTED:
			while (ce) {
				if (ce==EG(scope)) {
					return 1;
				}
				ce = ce->parent;
			}
			return 0;
	}
	return 0;
}


zval *zend_std_read_property(zval *object, zval *member TSRMLS_DC)
{
	zend_object *zobj;
	zval tmp_member;
	zval **retval;
	zval *rv = NULL;
	zend_property_info *property_info;
	zend_property_info std_property_info;
	
	zobj = Z_OBJ_P(object);

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Read object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif			

	if (zend_hash_find(&zobj->ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = Z_STRVAL_P(member);
		std_property_info.name_length = Z_STRLEN_P(member);
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		property_info = &std_property_info;
	}

#if DEBUG_OBJECT_HANDLERS
	zend_printf("Access type for %s::%s is %s\n", zobj->ce->name, Z_STRVAL_P(member), zend_visibility_string(property_info->flags));
#endif
	if (!zend_verify_property_access(property_info, zobj->ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), zobj->ce->name, Z_STRVAL_P(member));
	}

	if (zend_hash_find(zobj->properties, property_info->name, property_info->name_length+1, (void **) &retval) == FAILURE) {
		if (zobj->ce->__get && !zobj->in_get) {
			/* have getter - try with it! */
			zobj->in_get = 1; /* prevent circular getting */
			rv = zend_std_call_getter(object, member TSRMLS_CC);
			zobj->in_get = 0;

			if (rv) {
				retval = &rv;
			} else {
				retval = &EG(uninitialized_zval_ptr);	
			}
		} else {
			zend_error(E_NOTICE,"Undefined property:  %s::$%s", zobj->ce->name, Z_STRVAL_P(member));
		}
	}
	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return *retval;
}


static void zend_std_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	zend_object *zobj;
	zval tmp_member;
	zval **variable_ptr;
	int setter_done = 0;
	zend_property_info *property_info;
	zend_property_info std_property_info;
	
	zobj = Z_OBJ_P(object);

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (zend_hash_find(&zobj->ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = Z_STRVAL_P(member);
		std_property_info.name_length = Z_STRLEN_P(member);
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		property_info = &std_property_info;
	}

#if DEBUG_OBJECT_HANDLERS
	zend_printf("Access type for %s::%s is %s\n", zobj->ce->name, Z_STRVAL_P(member), zend_visibility_string(property_info->flags));
#endif

	if (!zend_verify_property_access(property_info, zobj->ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), zobj->ce->name, Z_STRVAL_P(member));
	}

	if (zend_hash_quick_find(zobj->properties, property_info->name, property_info->name_length+1, std_property_info.h, (void **) &variable_ptr) == SUCCESS) {
		if (*variable_ptr == value) {
			/* if we already have this value there, we don't actually need to do anything */
			setter_done = 1;
		} else {
			/* if we are assigning reference, we shouldn't move it, but instead assign variable
			   to the same pointer */
			if (PZVAL_IS_REF(*variable_ptr)) {
				zval_dtor(*variable_ptr); /* old value should be destroyed */
				/* To check: can't *variable_ptr be some system variable like error_zval here? */
				(*variable_ptr)->type = value->type;
				(*variable_ptr)->value = value->value;
				if (value->refcount>0) {
					zval_copy_ctor(*variable_ptr);
				}
				setter_done = 1;
			}
		}
	} else {
		if (zobj->ce->__set && !zobj->in_set) {
			zobj->in_set = 1; /* prevent circular setting */
			if (zend_std_call_setter(object, member, value TSRMLS_CC) != SUCCESS) {
				/* for now, just ignore it - __set should take care of warnings, etc. */
			}
			setter_done = 1;
			zobj->in_set = 0;
		}
	}

	if (!setter_done) {
		/* if we assign referenced variable, we should separate it */
		value->refcount++;
		if (PZVAL_IS_REF(value)) {
			SEPARATE_ZVAL(&value);
		}
		zend_hash_update(zobj->properties, property_info->name, property_info->name_length+1, &value, sizeof(zval *), NULL);
	}
	if (member == &tmp_member) {
		zval_dtor(member);
	}
}

static zval **zend_std_get_property_ptr(zval *object, zval *member TSRMLS_DC)
{
	zend_object *zobj;
	zval tmp_member;
	zval **retval;
	zend_property_info *property_info;
	zend_property_info std_property_info;
	
	zobj = Z_OBJ_P(object);

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (zend_hash_find(&zobj->ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = Z_STRVAL_P(member);
		std_property_info.name_length = Z_STRLEN_P(member);
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		property_info = &std_property_info;
	}

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Ptr object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif			

	if (!zend_verify_property_access(property_info, zobj->ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), zobj->ce->name, Z_STRVAL_P(member));
	}

	if (zend_hash_quick_find(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, (void **) &retval) == FAILURE) {
		zval *new_zval;

		if (!zobj->ce->__get) {
			/* we don't have getter - will just add it */
			new_zval = &EG(uninitialized_zval);

/* 			zend_error(E_NOTICE, "Undefined property: %s", Z_STRVAL_P(member)); */
			new_zval->refcount++;
			zend_hash_update(zobj->properties, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, &new_zval, sizeof(zval *), (void **) &retval);
		} else {
			/* we do have getter - fail and let it try again with usual get/set */
			retval = NULL;
		}
	}
	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}

static void zend_std_unset_property(zval *object, zval *member TSRMLS_DC)
{
	zend_object *zobj;
	zval tmp_member;
	zend_property_info *property_info;
	zend_property_info std_property_info;
	
	zobj = Z_OBJ_P(object);

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	if (zend_hash_find(&zobj->ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = Z_STRVAL_P(member);
		std_property_info.name_length = Z_STRLEN_P(member);
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		property_info = &std_property_info;
	}

	if (!zend_verify_property_access(property_info, zobj->ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), zobj->ce->name, Z_STRVAL_P(member));
	}
	
	zend_hash_del(zobj->properties, property_info->name, property_info->name_length+1);
	if (member == &tmp_member) {
		zval_dtor(member);
	}
}

static void zend_std_call_user_call(INTERNAL_FUNCTION_PARAMETERS)
{
	zval ***args;
	zend_internal_function *func = (zend_internal_function *)EG(function_state_ptr)->function;
	zval method_name, method_args, __call_name;
	zval *method_name_ptr, *method_args_ptr;
	zval **call_args[2];
	zval *method_result_ptr = NULL;
	int i, call_result;
	
	args = (zval ***)emalloc(ZEND_NUM_ARGS() * sizeof(zval **));

	if (zend_get_parameters_array_ex(ZEND_NUM_ARGS(), args) == FAILURE) {
		efree(args);
		zend_error(E_ERROR, "Cannot get arguments for __call");
		RETURN_FALSE;
	}

	method_name_ptr = &method_name;
	INIT_PZVAL(method_name_ptr);
	ZVAL_STRING(method_name_ptr, func->function_name, 0); /* no dup - it's a copy */

	method_args_ptr = &method_args;
	INIT_PZVAL(method_args_ptr);
	array_init(method_args_ptr);

	for(i=0; i<ZEND_NUM_ARGS(); i++) {
		zval_add_ref(args[i]);
		add_next_index_zval(method_args_ptr, *args[i]);
	}

	efree(args);
	
	INIT_PZVAL(&__call_name);
	ZVAL_STRINGL(&__call_name, ZEND_CALL_FUNC_NAME, sizeof(ZEND_CALL_FUNC_NAME)-1, 0);
	
	/* __call handler is called with two arguments:
	   method name
	   array of method parameters

	*/
	call_args[0] = &method_name_ptr;
	call_args[1] = &method_args_ptr;

	/* go call the __call handler */
	call_result = call_user_function_ex(NULL,
										&this_ptr,
										&__call_name,
										&method_result_ptr,
										2, call_args,
										0, NULL TSRMLS_CC);

	/* call_result is if call_user_function gone OK.
	   method_result_ptr is the true result of the called method
	*/
	
	if (method_result_ptr) {
		*return_value = *method_result_ptr;
		zval_copy_ctor(return_value);
		zval_ptr_dtor(&method_result_ptr);
	}
	
	if (call_result == FAILURE) {
		zend_error(E_ERROR, "Could not call __call handler for class %s", Z_OBJCE_P(this_ptr)->name);
	}

	/* now destruct all auxiliaries */
	zval_dtor(method_args_ptr);
	zval_dtor(method_name_ptr);

	/* destruct the function also, then - we have allocated it in get_method */
	efree(func);
}

/* Ensures that we're allowed to call a private method.
 * Returns the function address that should be called, or NULL
 * if no such function exists.
 */
inline zend_function *zend_check_private(zend_function *fbc, zend_class_entry *ce, int fn_flags, char *function_name_strval, int function_name_strlen TSRMLS_DC)
{
	if (!ce) {
		return 0;
	}

	/* We may call a private function if:
	 * 1.  The class of our object is the same as the scope, and the private
	 *     function (EX(fbc)) has the same scope.
	 * 2.  One of our parent classes are the same as the scope, and it contains
	 *     a private function with the same name that has the same scope.
	 */
	if (fbc->common.scope == ce && EG(scope) == ce) {
		/* rule #1 checks out ok, allow the function call */
		return fbc;
	}


	/* Check rule #2 */
	ce = ce->parent;
	while (ce) {
		if (ce == EG(scope)) {
			if (zend_hash_find(&ce->function_table, function_name_strval, function_name_strlen+1, (void **) &fbc)==SUCCESS
				&& fbc->op_array.fn_flags & ZEND_ACC_PRIVATE
				&& fbc->common.scope == EG(scope)) {
				return fbc;
			}
			break;
		}
		ce = ce->parent;
	}
	return NULL;
}


/* Ensures that we're allowed to call a protected method.
 */
inline int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope)
{
	zend_class_entry *fbc_scope = ce;

	/* Is the context that's calling the function, the same as one of
	 * the function's parents?
	 */
	while (fbc_scope) {
		if (fbc_scope==scope) {
			return 1;
		}
		fbc_scope = fbc_scope->parent;
	}

	/* Is the function's scope the same as our current object context,
	 * or any of the parents of our context?
	 */
	while (scope) {
		if (scope==ce) {
			return 1;
		}
		scope = scope->parent;
	}
	return 0;
}


static union _zend_function *zend_std_get_method(zval *object, char *method_name, int method_len TSRMLS_DC)
{
	zend_object *zobj;
	zend_function *fbc;
	char *lc_method_name;
	
	lc_method_name = do_alloca(method_len+1);
	/* Create a zend_copy_str_tolower(dest, src, src_length); */
	memcpy(lc_method_name, method_name, method_len+1);
	zend_str_tolower(lc_method_name, method_len);
		
	zobj = Z_OBJ_P(object);
	if (zend_hash_find(&zobj->ce->function_table, lc_method_name, method_len+1, (void **)&fbc) == FAILURE) {
		if (zobj->ce->__call) {
			zend_internal_function *call_user_call = emalloc(sizeof(zend_internal_function));
			call_user_call->type = ZEND_INTERNAL_FUNCTION;
			call_user_call->handler = zend_std_call_user_call;
			call_user_call->arg_types = NULL;
			call_user_call->scope = zobj->ce;
			call_user_call->fn_flags = 0;
			call_user_call->function_name = estrndup(method_name, method_len);

			free_alloca(lc_method_name);
			return (union _zend_function *)call_user_call;
		} else {
			return NULL;
		}
	}

	/* Check access level */
	if (fbc->op_array.fn_flags & ZEND_ACC_PUBLIC) {
		/* Ensure that we haven't overridden a private function and end up calling
		 * the overriding public function...
		 */
		if (EG(scope) && fbc->op_array.fn_flags & ZEND_ACC_CHANGED) {
			zend_function *priv_fbc;

			if (zend_hash_find(&EG(scope)->function_table, method_name, method_len+1, (void **) &priv_fbc)==SUCCESS
				&& priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE) {
				fbc = priv_fbc;
			}
		}
	} else if (fbc->op_array.fn_flags & ZEND_ACC_PRIVATE) {
		zend_function *updated_fbc;

		/* Ensure that if we're calling a private function, we're allowed to do so.
		 */
		updated_fbc = zend_check_private(fbc, object->value.obj.handlers->get_class_entry(object TSRMLS_CC), fbc->common.fn_flags, method_name, method_len TSRMLS_CC);
		if (!updated_fbc) {
			zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), method_name, EG(scope) ? EG(scope)->name : "");
		}
		fbc = updated_fbc;
	} else if ((fbc->common.fn_flags & ZEND_ACC_PROTECTED)) {
		/* Ensure that if we're calling a protected function, we're allowed to do so.
		 */
		if (!zend_check_protected(fbc->common.scope, EG(scope))) {
			zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), method_name, EG(scope) ? EG(scope)->name : "");
		}
	}

	free_alloca(lc_method_name);
	return fbc;
}


/* This is not (yet?) in the API, but it belongs in the built-in objects callbacks */
zend_function *zend_get_static_method(zend_class_entry *ce, char *function_name_strval, int function_name_strlen TSRMLS_DC)
{
	zend_function *fbc;

	if (zend_hash_find(&ce->function_table, function_name_strval, function_name_strlen+1, (void **) &fbc)==FAILURE) {
		zend_error(E_ERROR, "Call to undefined method %s::%s()", ce->name, function_name_strval);
	}
	if (fbc->op_array.fn_flags & ZEND_ACC_PUBLIC) {
		/* No further checks necessary, most common case */
	} else if (fbc->op_array.fn_flags & ZEND_ACC_PRIVATE) {
		zend_function *updated_fbc;

		/* Ensure that if we're calling a private function, we're allowed to do so.
		 */
		updated_fbc = zend_check_private(fbc, EG(scope), fbc->common.fn_flags, function_name_strval, function_name_strlen TSRMLS_CC); 
		if (!updated_fbc) {
			zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), function_name_strval, EG(scope) ? EG(scope)->name : "");
		}
		fbc = updated_fbc;
	} else if ((fbc->common.fn_flags & ZEND_ACC_PROTECTED)) {
		/* Ensure that if we're calling a protected function, we're allowed to do so.
		 */
		if (!zend_check_protected(EG(scope), fbc->common.scope)) {
			zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), function_name_strval, EG(scope) ? EG(scope)->name : "");
		}
	}

	return fbc;
}


zval **zend_get_static_property(zend_class_entry *ce, char *property_name, int property_name_len, int type TSRMLS_DC)
{
	HashTable *statics_table;
	zval **retval = NULL;
	zend_class_entry *tmp_ce = ce;
	zend_property_info *property_info;
	zend_property_info std_property_info;

	if (zend_hash_find(&ce->properties_info, property_name, property_name_len+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = property_name;
		std_property_info.name_length = property_name_len;
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		property_info = &std_property_info;
	}

#if 1&&DEBUG_OBJECT_HANDLERS
	zend_printf("Access type for %s::%s is %s\n", ce->name, property_name, zend_visibility_string(property_info->flags));
#endif

	if (!zend_verify_property_access(property_info, ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), ce->name, property_name);
	}

	while (tmp_ce) {
		if (zend_hash_quick_find(tmp_ce->static_members, property_info->name, property_info->name_length+1, property_info->h, (void **) &retval)==SUCCESS) {
			statics_table = tmp_ce->static_members;
			break;
		}
		tmp_ce = tmp_ce->parent;
	}

	if (!retval) {
		switch (type) {
			case BP_VAR_R: 
				zend_error(E_NOTICE,"Undefined variable:  %s::$%s", ce->name, property_name);
				/* break missing intentionally */
			case BP_VAR_IS:
				retval = &EG(uninitialized_zval_ptr);
				break;
			case BP_VAR_RW:
				zend_error(E_NOTICE,"Undefined variable:  %s::$%s", ce->name, property_name);
				/* break missing intentionally */
			case BP_VAR_W: {					
					zval *new_zval = &EG(uninitialized_zval);

					new_zval->refcount++;
					zend_hash_quick_update(ce->static_members, property_info->name, property_info->name_length+1, property_info->h, &new_zval, sizeof(zval *), (void **) &retval);
				}
				break;
			EMPTY_SWITCH_DEFAULT_CASE()
		}
	}

	return retval;
}


static union _zend_function *zend_std_get_constructor(zval *object TSRMLS_DC)
{
	zend_object *zobj = Z_OBJ_P(object);
	zend_function *constructor = zobj->ce->constructor;

	if (constructor) {
		if (constructor->op_array.fn_flags & ZEND_ACC_PUBLIC) {
			/* No further checks necessary */
		} else if (constructor->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (object->value.obj.handlers->get_class_entry(object TSRMLS_CC) != EG(scope)) {
				zend_error(E_ERROR, "Call to private constructor from context '%s'", EG(scope) ? EG(scope)->name : "");
			}
		} else if ((constructor->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 */
			if (!zend_check_protected(constructor->common.scope, EG(scope))) {
				zend_error(E_ERROR, "Call to protected constructor from context '%s'", EG(scope) ? EG(scope)->name : "");
			}
		}
	}

	return constructor;
}


int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);


static int zend_std_compare_objects(zval *o1, zval *o2 TSRMLS_DC)
{
	zend_object *zobj1, *zobj2;
	
	zobj1 = Z_OBJ_P(o1);
	zobj2 = Z_OBJ_P(o2);

	if (zobj1->ce != zobj2->ce) {
		return 1; /* different classes */
	}
	return zend_compare_symbol_tables_i(zobj1->properties, zobj2->properties TSRMLS_CC);
}


static int zend_std_has_property(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	zend_object *zobj;
	int result;
	zval **value;
	zval tmp_member;
	
	zend_property_info *property_info;
	zend_property_info std_property_info;
	
	zobj = Z_OBJ_P(object);

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Read object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif			

	if (zend_hash_find(&zobj->ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = Z_STRVAL_P(member);
		std_property_info.name_length = Z_STRLEN_P(member);
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		property_info = &std_property_info;
	}

#if DEBUG_OBJECT_HANDLERS
	zend_printf("Access type for %s::%s is %s\n", zobj->ce->name, Z_STRVAL_P(member), zend_visibility_string(property_info->flags));
#endif
	if (!zend_verify_property_access(property_info, zobj->ce TSRMLS_CC)) {
		zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), zobj->ce->name, Z_STRVAL_P(member));
	}

		
	if (zend_hash_find(zobj->properties, property_info->name, property_info->name_length+1, (void **) &value) == SUCCESS) {
		if (check_empty) {
			result = zend_is_true(*value);
		} else {
			result = (Z_TYPE_PP(value) != IS_NULL);
		}
	} else {
		result = 0;
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return result;
}


zend_class_entry *zend_std_object_get_class(zval *object TSRMLS_DC)
{
	zend_object *zobj;
	zobj = Z_OBJ_P(object);

	return zobj->ce;
}

int zend_std_object_get_class_name(zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC)
{
	zend_object *zobj;
	zobj = Z_OBJ_P(object);

	if (parent) {
		if (!zobj->ce->parent) {
			return FAILURE;
		}
		*class_name = zobj->ce->parent->name;
		*class_name_len = zobj->ce->parent->name_length;
	} else {
		*class_name = zobj->ce->name;
		*class_name_len = zobj->ce->name_length;
	}
	return SUCCESS;
}

zend_object_handlers std_object_handlers = {
	zend_objects_store_add_ref,              /* add_ref */
	zend_objects_store_del_ref,              /* del_ref */
	zend_objects_store_delete_obj,           /* delete_obj */
	zend_objects_clone_obj,                  /* clone_obj */
	
	zend_std_read_property,                  /* read_property */
	zend_std_write_property,                 /* write_property */
	zend_std_get_property_ptr,               /* get_property_ptr */
	zend_std_get_property_ptr,               /* get_property_zval_ptr */
	NULL,                                    /* get */
	NULL,                                    /* set */
	zend_std_has_property,                   /* has_property */
	zend_std_unset_property,                 /* unset_property */
	zend_std_get_properties,                 /* get_properties */
	zend_std_get_method,                     /* get_method */
	NULL,                                    /* call_method */
	zend_std_get_constructor,                /* get_constructor */
	zend_std_object_get_class,               /* get_class_entry */
	zend_std_object_get_class_name,          /* get_class_name */
	zend_std_compare_objects                 /* compare_objects */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
