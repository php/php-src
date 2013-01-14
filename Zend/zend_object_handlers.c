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
#include "zend_interfaces.h"
#include "zend_closures.h"
#include "zend_compile.h"

#define DEBUG_OBJECT_HANDLERS 0

#define Z_OBJ_P(zval_p) zend_objects_get_address(zval_p TSRMLS_CC)

/*
  __X accessors explanation:

  if we have __get and property that is not part of the properties array is
  requested, we call __get handler. If it fails, we return uninitialized.

  if we have __set and property that is not part of the properties array is
  set, we call __set handler. If it fails, we do not change the array.

  for both handlers above, when we are inside __get/__set, no further calls for
  __get/__set for this property of this object will be made, to prevent endless
  recursion and enable accessors to change properties array.

  if we have __call and method which is not part of the class function table is
  called, we cal __call handler.
*/

ZEND_API HashTable *zend_std_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zobj = Z_OBJ_P(object);
	return zobj->properties;
}
/* }}} */

ZEND_API HashTable *zend_std_get_debug_info(zval *object, int *is_temp TSRMLS_DC) /* {{{ */
{
	*is_temp = 0;
	return zend_std_get_properties(object TSRMLS_CC);
}
/* }}} */

static zval *zend_std_call_getter(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	zval *retval = NULL;
	zend_class_entry *ce = Z_OBJCE_P(object);

	/* __get handler is called with one argument:
	      property name

	   it should return whether the call was successfull or not
	*/

	SEPARATE_ARG_IF_REF(member);

	zend_call_method_with_1_params(&object, ce, &ce->__get, ZEND_GET_FUNC_NAME, &retval, member);

	zval_ptr_dtor(&member);

	if (retval) {
		Z_DELREF_P(retval);
	}

	return retval;
}
/* }}} */

static int zend_std_call_setter(zval *object, zval *member, zval *value TSRMLS_DC) /* {{{ */
{
	zval *retval = NULL;
	int result;
	zend_class_entry *ce = Z_OBJCE_P(object);

	SEPARATE_ARG_IF_REF(member);
	Z_ADDREF_P(value);

	/* __set handler is called with two arguments:
	     property name
	     value to be set

	   it should return whether the call was successfull or not
	*/
	zend_call_method_with_2_params(&object, ce, &ce->__set, ZEND_SET_FUNC_NAME, &retval, member, value);

	zval_ptr_dtor(&member);
	zval_ptr_dtor(&value);

	if (retval) {
		result = i_zend_is_true(retval) ? SUCCESS : FAILURE;
		zval_ptr_dtor(&retval);
		return result;
	} else {
		return FAILURE;
	}
}
/* }}} */

static void zend_std_call_unsetter(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(object);

	/* __unset handler is called with one argument:
	      property name
	*/

	SEPARATE_ARG_IF_REF(member);

	zend_call_method_with_1_params(&object, ce, &ce->__unset, ZEND_UNSET_FUNC_NAME, NULL, member);

	zval_ptr_dtor(&member);
}
/* }}} */

static zval *zend_std_call_issetter(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	zval *retval = NULL;
	zend_class_entry *ce = Z_OBJCE_P(object);

	/* __isset handler is called with one argument:
	      property name

	   it should return whether the property is set or not
	*/

	SEPARATE_ARG_IF_REF(member);

	zend_call_method_with_1_params(&object, ce, &ce->__isset, ZEND_ISSET_FUNC_NAME, &retval, member);

	zval_ptr_dtor(&member);

	return retval;
}
/* }}} */

static int zend_verify_property_access(zend_property_info *property_info, zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	switch (property_info->flags & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			return 1;
		case ZEND_ACC_PROTECTED:
			return zend_check_protected(property_info->ce, EG(scope));
		case ZEND_ACC_PRIVATE:
			if ((ce==EG(scope) || property_info->ce == EG(scope)) && EG(scope)) {
				return 1;
			} else {
				return 0;
			}
			break;
	}
	return 0;
}
/* }}} */

static inline zend_bool is_derived_class(zend_class_entry *child_class, zend_class_entry *parent_class) /* {{{ */
{
	child_class = child_class->parent;
	while (child_class) {
		if (child_class == parent_class) {
			return 1;
		}
		child_class = child_class->parent;
	}

	return 0;
}
/* }}} */

ZEND_API struct _zend_property_info *zend_get_property_info(zend_class_entry *ce, zval *member, int silent TSRMLS_DC) /* {{{ */
{
	zend_property_info *property_info = NULL;
	zend_property_info *scope_property_info;
	zend_bool denied_access = 0;
	ulong h;

	if (Z_STRVAL_P(member)[0] == '\0') {
		if (!silent) {
			if (Z_STRLEN_P(member) == 0) {
				zend_error(E_ERROR, "Cannot access empty property");
			} else {
				zend_error(E_ERROR, "Cannot access property started with '\\0'");
			}
		}
		return NULL;
	}
	h = zend_get_hash_value(Z_STRVAL_P(member), Z_STRLEN_P(member) + 1);
	if (zend_hash_quick_find(&ce->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, h, (void **) &property_info)==SUCCESS) {
		if(property_info->flags & ZEND_ACC_SHADOW) {
			/* if it's a shadow - go to access it's private */
			property_info = NULL;
		} else {
			if (zend_verify_property_access(property_info, ce TSRMLS_CC)) {
				if (property_info->flags & ZEND_ACC_CHANGED
					&& !(property_info->flags & ZEND_ACC_PRIVATE)) {
					/* We still need to make sure that we're not in a context
					 * where the right property is a different 'statically linked' private
					 * continue checking below...
					 */
				} else {
					if (!silent && (property_info->flags & ZEND_ACC_STATIC)) {
						zend_error(E_STRICT, "Accessing static property %s::$%s as non static", ce->name, Z_STRVAL_P(member));
					}
					return property_info;
				}
			} else {
				/* Try to look in the scope instead */
				denied_access = 1;
			}
		}
	}
	if (EG(scope) != ce
		&& is_derived_class(ce, EG(scope))
		&& EG(scope)
		&& zend_hash_quick_find(&EG(scope)->properties_info, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, h, (void **) &scope_property_info)==SUCCESS
		&& scope_property_info->flags & ZEND_ACC_PRIVATE) {
		return scope_property_info;
	} else if (property_info) {
		if (denied_access) {
			/* Information was available, but we were denied access.  Error out. */
			if (silent) {
				return NULL;
			}
			zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), ce->name, Z_STRVAL_P(member));
		} else {
			/* fall through, return property_info... */
		}
	} else {
		EG(std_property_info).flags = ZEND_ACC_PUBLIC;
		EG(std_property_info).name = Z_STRVAL_P(member);
		EG(std_property_info).name_length = Z_STRLEN_P(member);
		EG(std_property_info).h = h;
		EG(std_property_info).ce = ce;
		property_info = &EG(std_property_info);
	}
	return property_info;
}
/* }}} */

ZEND_API int zend_check_property_access(zend_object *zobj, char *prop_info_name, int prop_info_name_len TSRMLS_DC) /* {{{ */
{
	zend_property_info *property_info;
	char *class_name, *prop_name;
	zval member;

	zend_unmangle_property_name(prop_info_name, prop_info_name_len, &class_name, &prop_name);
	ZVAL_STRING(&member, prop_name, 0);
	property_info = zend_get_property_info(zobj->ce, &member, 1 TSRMLS_CC);
	if (!property_info) {
		return FAILURE;
	}
	if (class_name && class_name[0] != '*') {
		if (!(property_info->flags & ZEND_ACC_PRIVATE)) {
			/* we we're looking for a private prop but found a non private one of the same name */
			return FAILURE;
		} else if (strcmp(prop_info_name+1, property_info->name+1)) {
			/* we we're looking for a private prop but found a private one of the same name but another class */
			return FAILURE;
		}
	}
	return zend_verify_property_access(property_info, zobj->ce TSRMLS_CC) ? SUCCESS : FAILURE;
}
/* }}} */

static int zend_get_property_guard(zend_object *zobj, zend_property_info *property_info, zval *member, zend_guard **pguard) /* {{{ */
{
	zend_property_info info;
	zend_guard stub;

	if (!property_info) {
		property_info = &info;
		info.name = Z_STRVAL_P(member);
		info.name_length = Z_STRLEN_P(member);
		info.h = zend_get_hash_value(Z_STRVAL_P(member), Z_STRLEN_P(member) + 1);
	} else if(property_info->name[0] == '\0'){
		const char *class_name = NULL, *prop_name = NULL;
		zend_unmangle_property_name(property_info->name, property_info->name_length, &class_name, &prop_name);
		if(class_name) {
			/* use unmangled name for protected properties */
			info.name = prop_name;
			info.name_length = strlen(prop_name);
			info.h = zend_get_hash_value(info.name, info.name_length+1);
			property_info = &info;
		}
	}
	if (!zobj->guards) {
		ALLOC_HASHTABLE(zobj->guards);
		zend_hash_init(zobj->guards, 0, NULL, NULL, 0);
	} else if (zend_hash_quick_find(zobj->guards, property_info->name, property_info->name_length+1, property_info->h, (void **) pguard) == SUCCESS) {
		return SUCCESS;
	}
	stub.in_get = 0;
	stub.in_set = 0;
	stub.in_unset = 0;
	stub.in_isset = 0;
	return zend_hash_quick_add(zobj->guards, property_info->name, property_info->name_length+1, property_info->h, (void**)&stub, sizeof(stub), (void**) pguard);
}
/* }}} */

zval *zend_std_read_property(zval *object, zval *member, int type TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zval *tmp_member = NULL;
	zval **retval;
	zval *rv = NULL;
	zend_property_info *property_info;
	int silent;

	silent = (type == BP_VAR_IS);
	zobj = Z_OBJ_P(object);

 	if (Z_TYPE_P(member) != IS_STRING) {
 		ALLOC_ZVAL(tmp_member);
		*tmp_member = *member;
		INIT_PZVAL(tmp_member);
		zval_copy_ctor(tmp_member);
		convert_to_string(tmp_member);
		member = tmp_member;
	}

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Read object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif

	/* make zend_get_property_info silent if we have getter - we may want to use it */
	property_info = zend_get_property_info(zobj->ce, member, (zobj->ce->__get != NULL) TSRMLS_CC);

	if (!property_info || zend_hash_quick_find(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, (void **) &retval) == FAILURE) {
		zend_guard *guard = NULL;

		if (zobj->ce->__get &&
		    zend_get_property_guard(zobj, property_info, member, &guard) == SUCCESS &&
		    !guard->in_get) {
			/* have getter - try with it! */
			Z_ADDREF_P(object);
			if (PZVAL_IS_REF(object)) {
				SEPARATE_ZVAL(&object);
			}
			guard->in_get = 1; /* prevent circular getting */
			rv = zend_std_call_getter(object, member TSRMLS_CC);
			guard->in_get = 0;

			if (rv) {
				retval = &rv;
				if (!Z_ISREF_P(rv) &&
				    (type == BP_VAR_W || type == BP_VAR_RW  || type == BP_VAR_UNSET)) {
					if (Z_REFCOUNT_P(rv) > 0) {
						zval *tmp = rv;

						ALLOC_ZVAL(rv);
						*rv = *tmp;
						zval_copy_ctor(rv);
						Z_UNSET_ISREF_P(rv);
						Z_SET_REFCOUNT_P(rv, 0);
					}
					if (Z_TYPE_P(rv) != IS_OBJECT) {
						zend_error(E_NOTICE, "Indirect modification of overloaded property %s::$%s has no effect", zobj->ce->name, Z_STRVAL_P(member));
					}
				}
			} else {
				retval = &EG(uninitialized_zval_ptr);
			}
			if (EXPECTED(*retval != object)) {
				zval_ptr_dtor(&object);
			} else {
				Z_DELREF_P(object);
			}
		} else {
			if (zobj->ce->__get && guard && guard->in_get == 1) {
				if (Z_STRVAL_P(member)[0] == '\0') {
					if (Z_STRLEN_P(member) == 0) {
						zend_error(E_ERROR, "Cannot access empty property");
					} else {
						zend_error(E_ERROR, "Cannot access property started with '\\0'");
					}
				}
			}
			if (!silent) {
				zend_error(E_NOTICE,"Undefined property: %s::$%s", zobj->ce->name, Z_STRVAL_P(member));
			}
			retval = &EG(uninitialized_zval_ptr);
		}
	}
	if (tmp_member) {
		Z_ADDREF_PP(retval);
		zval_ptr_dtor(&tmp_member);
		Z_DELREF_PP(retval);
	}
	return *retval;
}
/* }}} */

static void zend_std_write_property(zval *object, zval *member, zval *value TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zval *tmp_member = NULL;
	zval **variable_ptr;
	zend_property_info *property_info;

	zobj = Z_OBJ_P(object);

 	if (Z_TYPE_P(member) != IS_STRING) {
 		ALLOC_ZVAL(tmp_member);
		*tmp_member = *member;
		INIT_PZVAL(tmp_member);
		zval_copy_ctor(tmp_member);
		convert_to_string(tmp_member);
		member = tmp_member;
	}

	property_info = zend_get_property_info(zobj->ce, member, (zobj->ce->__set != NULL) TSRMLS_CC);

	if (property_info && zend_hash_quick_find(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, (void **) &variable_ptr) == SUCCESS) {
		/* if we already have this value there, we don't actually need to do anything */
		if (*variable_ptr != value) {
			/* if we are assigning reference, we shouldn't move it, but instead assign variable
			   to the same pointer */
			if (PZVAL_IS_REF(*variable_ptr)) {
				zval garbage = **variable_ptr; /* old value should be destroyed */

				/* To check: can't *variable_ptr be some system variable like error_zval here? */
				Z_TYPE_PP(variable_ptr) = Z_TYPE_P(value);
				(*variable_ptr)->value = value->value;
				if (Z_REFCOUNT_P(value) > 0) {
					zval_copy_ctor(*variable_ptr);
				} else {
					efree(value);
				}
				zval_dtor(&garbage);
			} else {
				zval *garbage = *variable_ptr;

				/* if we assign referenced variable, we should separate it */
				Z_ADDREF_P(value);
				if (PZVAL_IS_REF(value)) {
					SEPARATE_ZVAL(&value);
				}
				*variable_ptr = value;
				zval_ptr_dtor(&garbage);
			}
		}
	} else {
		zend_guard *guard = NULL;

		if (zobj->ce->__set &&
		    zend_get_property_guard(zobj, property_info, member, &guard) == SUCCESS &&
		    !guard->in_set) {
			Z_ADDREF_P(object);
			if (PZVAL_IS_REF(object)) {
				SEPARATE_ZVAL(&object);
			}
			guard->in_set = 1; /* prevent circular setting */
			if (zend_std_call_setter(object, member, value TSRMLS_CC) != SUCCESS) {
				/* for now, just ignore it - __set should take care of warnings, etc. */
			}
			guard->in_set = 0;
			zval_ptr_dtor(&object);
		} else if (property_info) {
			zval **foo;

			/* if we assign referenced variable, we should separate it */
			Z_ADDREF_P(value);
			if (PZVAL_IS_REF(value)) {
				SEPARATE_ZVAL(&value);
			}
			zend_hash_quick_update(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, &value, sizeof(zval *), (void **) &foo);
		} else if (zobj->ce->__set && guard && guard->in_set == 1) {
			if (Z_STRVAL_P(member)[0] == '\0') {
				if (Z_STRLEN_P(member) == 0) {
					zend_error(E_ERROR, "Cannot access empty property");
				} else {
					zend_error(E_ERROR, "Cannot access property started with '\\0'");
				}
			}
		}
	}

	if (tmp_member) {
		zval_ptr_dtor(&tmp_member);
	}
}
/* }}} */

zval *zend_std_read_dimension(zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(object);
	zval *retval;

	if (instanceof_function_ex(ce, zend_ce_arrayaccess, 1 TSRMLS_CC)) {
		if(offset == NULL) {
			/* [] construct */
			ALLOC_INIT_ZVAL(offset);
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		zend_call_method_with_1_params(&object, ce, NULL, "offsetget", &retval, offset);

		zval_ptr_dtor(&offset);

		if (!retval) {
			if (!EG(exception)) {
				zend_error(E_ERROR, "Undefined offset for object of type %s used as array", ce->name);
			}
			return 0;
		}

		/* Undo PZVAL_LOCK() */
		Z_DELREF_P(retval);

		return retval;
	} else {
		zend_error(E_ERROR, "Cannot use object of type %s as array", ce->name);
		return 0;
	}
}
/* }}} */

static void zend_std_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(object);

	if (instanceof_function_ex(ce, zend_ce_arrayaccess, 1 TSRMLS_CC)) {
		if (!offset) {
			ALLOC_INIT_ZVAL(offset);
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		zend_call_method_with_2_params(&object, ce, NULL, "offsetset", NULL, offset, value);
		zval_ptr_dtor(&offset);
	} else {
		zend_error(E_ERROR, "Cannot use object of type %s as array", ce->name);
	}
}
/* }}} */

static int zend_std_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(object);
	zval *retval;
	int result;

	if (instanceof_function_ex(ce, zend_ce_arrayaccess, 1 TSRMLS_CC)) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, ce, NULL, "offsetexists", &retval, offset);
		if (retval) {
			result = i_zend_is_true(retval);
			zval_ptr_dtor(&retval);
			if (check_empty && result && !EG(exception)) {
				zend_call_method_with_1_params(&object, ce, NULL, "offsetget", &retval, offset);
				if (retval) {
					result = i_zend_is_true(retval);
					zval_ptr_dtor(&retval);
				}
			}
		} else {
			result = 0;
		}
		zval_ptr_dtor(&offset);
	} else {
		zend_error(E_ERROR, "Cannot use object of type %s as array", ce->name);
		return 0;
	}
	return result;
}
/* }}} */

static zval **zend_std_get_property_ptr_ptr(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zval tmp_member;
	zval **retval;
	zend_property_info *property_info;

	zobj = Z_OBJ_P(object);

 	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Ptr object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif

	property_info = zend_get_property_info(zobj->ce, member, (zobj->ce->__get != NULL) TSRMLS_CC);

	if (!property_info || zend_hash_quick_find(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, (void **) &retval) == FAILURE) {
		zval *new_zval;
		zend_guard *guard;

		if (!zobj->ce->__get ||
			zend_get_property_guard(zobj, property_info, member, &guard) != SUCCESS ||
			(property_info && guard->in_get)) {
			/* we don't have access controls - will just add it */
			new_zval = &EG(uninitialized_zval);

/* 			zend_error(E_NOTICE, "Undefined property: %s", Z_STRVAL_P(member)); */
			Z_ADDREF_P(new_zval);
			zend_hash_quick_update(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, &new_zval, sizeof(zval *), (void **) &retval);
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
/* }}} */

static void zend_std_unset_property(zval *object, zval *member TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zval *tmp_member = NULL;
	zend_property_info *property_info;

	zobj = Z_OBJ_P(object);

 	if (Z_TYPE_P(member) != IS_STRING) {
 		ALLOC_ZVAL(tmp_member);
		*tmp_member = *member;
		INIT_PZVAL(tmp_member);
		zval_copy_ctor(tmp_member);
		convert_to_string(tmp_member);
		member = tmp_member;
	}

	property_info = zend_get_property_info(zobj->ce, member, (zobj->ce->__unset != NULL) TSRMLS_CC);

	if (!property_info || zend_hash_quick_del(zobj->properties, property_info->name, property_info->name_length+1, property_info->h) == FAILURE) {
		zend_guard *guard = NULL;

		if (zobj->ce->__unset &&
		    zend_get_property_guard(zobj, property_info, member, &guard) == SUCCESS &&
		    !guard->in_unset) {
			/* have unseter - try with it! */
			Z_ADDREF_P(object);
			if (PZVAL_IS_REF(object)) {
				SEPARATE_ZVAL(&object);
			}
			guard->in_unset = 1; /* prevent circular unsetting */
			zend_std_call_unsetter(object, member TSRMLS_CC);
			guard->in_unset = 0;
			zval_ptr_dtor(&object);
		} else if (zobj->ce->__unset && guard && guard->in_unset == 1) {
			if (Z_STRVAL_P(member)[0] == '\0') {
				if (Z_STRLEN_P(member) == 0) {
					zend_error(E_ERROR, "Cannot access empty property");
				} else {
					zend_error(E_ERROR, "Cannot access property started with '\\0'");
				}
			}
		}
	}

	if (tmp_member) {
		zval_ptr_dtor(&tmp_member);
	}
}
/* }}} */

static void zend_std_unset_dimension(zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce = Z_OBJCE_P(object);

	if (instanceof_function_ex(ce, zend_ce_arrayaccess, 1 TSRMLS_CC)) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(&object, ce, NULL, "offsetunset", NULL, offset);
		zval_ptr_dtor(&offset);
	} else {
		zend_error(E_ERROR, "Cannot use object of type %s as array", ce->name);
	}
}
/* }}} */

ZEND_API void zend_std_call_user_call(INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	zend_internal_function *func = (zend_internal_function *)EG(current_execute_data)->function_state.function;
	zval *method_name_ptr, *method_args_ptr;
	zval *method_result_ptr = NULL;
	zend_class_entry *ce = Z_OBJCE_P(this_ptr);

	ALLOC_ZVAL(method_args_ptr);
	INIT_PZVAL(method_args_ptr);
	array_init_size(method_args_ptr, ZEND_NUM_ARGS());

	if (zend_copy_parameters_array(ZEND_NUM_ARGS(), method_args_ptr TSRMLS_CC) == FAILURE) {
		zval_dtor(method_args_ptr);
		zend_error(E_ERROR, "Cannot get arguments for __call");
		RETURN_FALSE;
	}

	ALLOC_ZVAL(method_name_ptr);
	INIT_PZVAL(method_name_ptr);
	ZVAL_STRING(method_name_ptr, func->function_name, 0); /* no dup - it's a copy */

	/* __call handler is called with two arguments:
	   method name
	   array of method parameters

	*/
	zend_call_method_with_2_params(&this_ptr, ce, &ce->__call, ZEND_CALL_FUNC_NAME, &method_result_ptr, method_name_ptr, method_args_ptr);

	if (method_result_ptr) {
		if (Z_ISREF_P(method_result_ptr) || Z_REFCOUNT_P(method_result_ptr) > 1) {
			RETVAL_ZVAL(method_result_ptr, 1, 1);
		} else {
			RETVAL_ZVAL(method_result_ptr, 0, 1);
		}
	}

	/* now destruct all auxiliaries */
	zval_ptr_dtor(&method_args_ptr);
	zval_ptr_dtor(&method_name_ptr);

	/* destruct the function also, then - we have allocated it in get_method */
	efree(func);
}
/* }}} */

/* Ensures that we're allowed to call a private method.
 * Returns the function address that should be called, or NULL
 * if no such function exists.
 */
static inline zend_function *zend_check_private_int(zend_function *fbc, zend_class_entry *ce, char *function_name_strval, int function_name_strlen TSRMLS_DC) /* {{{ */
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
/* }}} */

ZEND_API int zend_check_private(zend_function *fbc, zend_class_entry *ce, char *function_name_strval, int function_name_strlen TSRMLS_DC) /* {{{ */
{
	return zend_check_private_int(fbc, ce, function_name_strval, function_name_strlen TSRMLS_CC) != NULL;
}
/* }}} */

/* Ensures that we're allowed to call a protected method.
 */
ZEND_API int zend_check_protected(zend_class_entry *ce, zend_class_entry *scope) /* {{{ */
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
/* }}} */

static inline zend_class_entry * zend_get_function_root_class(zend_function *fbc) /* {{{ */
{
	return fbc->common.prototype ? fbc->common.prototype->common.scope : fbc->common.scope;
}
/* }}} */

static inline union _zend_function *zend_get_user_call_function(zend_class_entry *ce, const char *method_name, int method_len) /* {{{ */
{
	zend_internal_function *call_user_call = emalloc(sizeof(zend_internal_function));
	call_user_call->type = ZEND_INTERNAL_FUNCTION;
	call_user_call->module = ce->module;
	call_user_call->handler = zend_std_call_user_call;
	call_user_call->arg_info = NULL;
	call_user_call->num_args = 0;
	call_user_call->scope = ce;
	call_user_call->fn_flags = ZEND_ACC_CALL_VIA_HANDLER;
	call_user_call->function_name = estrndup(method_name, method_len);
	call_user_call->pass_rest_by_reference = 0;
	call_user_call->return_reference = ZEND_RETURN_VALUE;

	return (union _zend_function *)call_user_call;
}
/* }}} */

static union _zend_function *zend_std_get_method(zval **object_ptr, char *method_name, int method_len TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zend_function *fbc;
	char *lc_method_name;
	zval *object = *object_ptr;
	ALLOCA_FLAG(use_heap)

	lc_method_name = do_alloca(method_len+1, use_heap);
	/* Create a zend_copy_str_tolower(dest, src, src_length); */
	zend_str_tolower_copy(lc_method_name, method_name, method_len);

	zobj = Z_OBJ_P(object);
	if (zend_hash_find(&zobj->ce->function_table, lc_method_name, method_len+1, (void **)&fbc) == FAILURE) {
		free_alloca(lc_method_name, use_heap);
		if (zobj->ce->__call) {
			return zend_get_user_call_function(zobj->ce, method_name, method_len);
		} else {
			return NULL;
		}
	}

	/* Check access level */
	if (fbc->op_array.fn_flags & ZEND_ACC_PRIVATE) {
		zend_function *updated_fbc;

		/* Ensure that if we're calling a private function, we're allowed to do so.
		 * If we're not and __call() handler exists, invoke it, otherwise error out.
		 */
		updated_fbc = zend_check_private_int(fbc, Z_OBJ_HANDLER_P(object, get_class_entry)(object TSRMLS_CC), lc_method_name, method_len TSRMLS_CC);
		if (updated_fbc) {
			fbc = updated_fbc;
		} else {
			if (zobj->ce->__call) {
				fbc = zend_get_user_call_function(zobj->ce, method_name, method_len);
			} else {
				zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), method_name, EG(scope) ? EG(scope)->name : "");
			}
		}
	} else {
		/* Ensure that we haven't overridden a private function and end up calling
		 * the overriding public function...
		 */
		if (EG(scope) &&
		    is_derived_class(fbc->common.scope, EG(scope)) &&
		    fbc->op_array.fn_flags & ZEND_ACC_CHANGED) {
			zend_function *priv_fbc;

			if (zend_hash_find(&EG(scope)->function_table, lc_method_name, method_len+1, (void **) &priv_fbc)==SUCCESS
				&& priv_fbc->common.fn_flags & ZEND_ACC_PRIVATE
				&& priv_fbc->common.scope == EG(scope)) {
				fbc = priv_fbc;
			}
		}
		if ((fbc->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 * If we're not and __call() handler exists, invoke it, otherwise error out.
			 */
			if (!zend_check_protected(zend_get_function_root_class(fbc), EG(scope))) {
				if (zobj->ce->__call) {
					fbc = zend_get_user_call_function(zobj->ce, method_name, method_len);
				} else {
					zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), method_name, EG(scope) ? EG(scope)->name : "");
				}
			}
		}
	}

	free_alloca(lc_method_name, use_heap);
	return fbc;
}
/* }}} */

ZEND_API void zend_std_callstatic_user_call(INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	zend_internal_function *func = (zend_internal_function *)EG(current_execute_data)->function_state.function;
	zval *method_name_ptr, *method_args_ptr;
	zval *method_result_ptr = NULL;
	zend_class_entry *ce = EG(scope);

	ALLOC_ZVAL(method_args_ptr);
	INIT_PZVAL(method_args_ptr);
	array_init_size(method_args_ptr, ZEND_NUM_ARGS());

	if (zend_copy_parameters_array(ZEND_NUM_ARGS(), method_args_ptr TSRMLS_CC) == FAILURE) {
		zval_dtor(method_args_ptr);
		zend_error(E_ERROR, "Cannot get arguments for " ZEND_CALLSTATIC_FUNC_NAME);
		RETURN_FALSE;
	}

	ALLOC_ZVAL(method_name_ptr);
	INIT_PZVAL(method_name_ptr);
	ZVAL_STRING(method_name_ptr, func->function_name, 0); /* no dup - it's a copy */

	/* __callStatic handler is called with two arguments:
	   method name
	   array of method parameters
	*/
	zend_call_method_with_2_params(NULL, ce, &ce->__callstatic, ZEND_CALLSTATIC_FUNC_NAME, &method_result_ptr, method_name_ptr, method_args_ptr);

	if (method_result_ptr) {
		if (Z_ISREF_P(method_result_ptr) || Z_REFCOUNT_P(method_result_ptr) > 1) {
			RETVAL_ZVAL(method_result_ptr, 1, 1);
		} else {
			RETVAL_ZVAL(method_result_ptr, 0, 1);
		}
	}

	/* now destruct all auxiliaries */
	zval_ptr_dtor(&method_args_ptr);
	zval_ptr_dtor(&method_name_ptr);

	/* destruct the function also, then - we have allocated it in get_method */
	efree(func);
}
/* }}} */

static inline union _zend_function *zend_get_user_callstatic_function(zend_class_entry *ce, const char *method_name, int method_len) /* {{{ */
{
	zend_internal_function *callstatic_user_call = emalloc(sizeof(zend_internal_function));
	callstatic_user_call->type     = ZEND_INTERNAL_FUNCTION;
	callstatic_user_call->module   = ce->module;
	callstatic_user_call->handler  = zend_std_callstatic_user_call;
	callstatic_user_call->arg_info = NULL;
	callstatic_user_call->num_args = 0;
	callstatic_user_call->scope    = ce;
	callstatic_user_call->fn_flags = ZEND_ACC_STATIC | ZEND_ACC_PUBLIC | ZEND_ACC_CALL_VIA_HANDLER;
	callstatic_user_call->function_name = estrndup(method_name, method_len);
	callstatic_user_call->pass_rest_by_reference = 0;
	callstatic_user_call->return_reference       = ZEND_RETURN_VALUE;

	return (zend_function *)callstatic_user_call;
}
/* }}} */

/* This is not (yet?) in the API, but it belongs in the built-in objects callbacks */

ZEND_API zend_function *zend_std_get_static_method(zend_class_entry *ce, char *function_name_strval, int function_name_strlen TSRMLS_DC) /* {{{ */
{
	zend_function *fbc = NULL;
	char *lc_class_name, *lc_function_name = NULL;
	
	lc_function_name = zend_str_tolower_dup(function_name_strval, function_name_strlen);

	if (function_name_strlen == ce->name_length && ce->constructor) {
		lc_class_name = zend_str_tolower_dup(ce->name, ce->name_length);
		/* Only change the method to the constructor if the constructor isn't called __construct
		 * we check for __ so we can be binary safe for lowering, we should use ZEND_CONSTRUCTOR_FUNC_NAME
		 */
		if (!memcmp(lc_class_name, lc_function_name, function_name_strlen) && memcmp(ce->constructor->common.function_name, "__", sizeof("__") - 1)) {
			fbc = ce->constructor;
		}
		efree(lc_class_name);
	}
	if (!fbc && zend_hash_find(&ce->function_table, lc_function_name, function_name_strlen+1, (void **) &fbc)==FAILURE) {
		efree(lc_function_name);

		if (ce->__call &&
		    EG(This) &&
		    Z_OBJ_HT_P(EG(This))->get_class_entry &&
		    instanceof_function(Z_OBJCE_P(EG(This)), ce TSRMLS_CC)) {
			return zend_get_user_call_function(ce, function_name_strval, function_name_strlen);
		} else if (ce->__callstatic) {
			return zend_get_user_callstatic_function(ce, function_name_strval, function_name_strlen);
		} else {
	   		return NULL;
		}
	}
	efree(lc_function_name);

#if MBO_0
	/* right now this function is used for non static method lookup too */
	/* Is the function static */
	if (!(fbc->common.fn_flags & ZEND_ACC_STATIC)) {
		zend_error(E_ERROR, "Cannot call non static method %s::%s() without object", ZEND_FN_SCOPE_NAME(fbc), fbc->common.function_name);
	}
#endif 
	if (fbc->op_array.fn_flags & ZEND_ACC_PUBLIC) {
		/* No further checks necessary, most common case */
	} else if (fbc->op_array.fn_flags & ZEND_ACC_PRIVATE) {
		zend_function *updated_fbc;

		/* Ensure that if we're calling a private function, we're allowed to do so.
		 */
		updated_fbc = zend_check_private_int(fbc, EG(scope), function_name_strval, function_name_strlen TSRMLS_CC);
		if (updated_fbc) {
			fbc = updated_fbc;
		} else {
			if (ce->__callstatic) {
				return zend_get_user_callstatic_function(ce, function_name_strval, function_name_strlen);
			}
			zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), function_name_strval, EG(scope) ? EG(scope)->name : "");
		}
	} else if ((fbc->common.fn_flags & ZEND_ACC_PROTECTED)) {
		/* Ensure that if we're calling a protected function, we're allowed to do so.
		 */
		if (!zend_check_protected(zend_get_function_root_class(fbc), EG(scope))) {
			if (ce->__callstatic) {
				return zend_get_user_callstatic_function(ce, function_name_strval, function_name_strlen);
			}
			zend_error(E_ERROR, "Call to %s method %s::%s() from context '%s'", zend_visibility_string(fbc->common.fn_flags), ZEND_FN_SCOPE_NAME(fbc), function_name_strval, EG(scope) ? EG(scope)->name : "");
		}
	}

	return fbc;
}
/* }}} */

ZEND_API zval **zend_std_get_static_property(zend_class_entry *ce, char *property_name, int property_name_len, zend_bool silent TSRMLS_DC) /* {{{ */
{
	zval **retval = NULL;
	zend_class_entry *tmp_ce = ce;
	zend_property_info *property_info;
	zend_property_info std_property_info;

	if (zend_hash_find(&ce->properties_info, property_name, property_name_len+1, (void **) &property_info)==FAILURE) {
		std_property_info.flags = ZEND_ACC_PUBLIC;
		std_property_info.name = property_name;
		std_property_info.name_length = property_name_len;
		std_property_info.h = zend_get_hash_value(std_property_info.name, std_property_info.name_length+1);
		std_property_info.ce = ce;
		property_info = &std_property_info;
	}

#if DEBUG_OBJECT_HANDLERS
	zend_printf("Access type for %s::%s is %s\n", ce->name, property_name, zend_visibility_string(property_info->flags));
#endif

	if (!zend_verify_property_access(property_info, ce TSRMLS_CC)) {
		if (!silent) {
			zend_error(E_ERROR, "Cannot access %s property %s::$%s", zend_visibility_string(property_info->flags), ce->name, property_name);
		}
		return NULL;
	}

	zend_update_class_constants(tmp_ce TSRMLS_CC);

	zend_hash_quick_find(CE_STATIC_MEMBERS(tmp_ce), property_info->name, property_info->name_length+1, property_info->h, (void **) &retval);

	if (!retval) {
		if (silent) {
			return NULL;
		} else {
			zend_error(E_ERROR, "Access to undeclared static property: %s::$%s", ce->name, property_name);
		}
	}

	return retval;
}
/* }}} */

ZEND_API zend_bool zend_std_unset_static_property(zend_class_entry *ce, char *property_name, int property_name_len TSRMLS_DC) /* {{{ */
{
	zend_error(E_ERROR, "Attempt to unset static property %s::$%s", ce->name, property_name);
	return 0;
}
/* }}} */

ZEND_API union _zend_function *zend_std_get_constructor(zval *object TSRMLS_DC) /* {{{ */
{
	zend_object *zobj = Z_OBJ_P(object);
	zend_function *constructor = zobj->ce->constructor;

	if (constructor) {
		if (constructor->op_array.fn_flags & ZEND_ACC_PUBLIC) {
			/* No further checks necessary */
		} else if (constructor->op_array.fn_flags & ZEND_ACC_PRIVATE) {
			/* Ensure that if we're calling a private function, we're allowed to do so.
			 */
			if (constructor->common.scope != EG(scope)) {
				if (EG(scope)) {
					zend_error(E_ERROR, "Call to private %s::%s() from context '%s'", constructor->common.scope->name, constructor->common.function_name, EG(scope)->name);
				} else {
					zend_error(E_ERROR, "Call to private %s::%s() from invalid context", constructor->common.scope->name, constructor->common.function_name);
				}
			}
		} else if ((constructor->common.fn_flags & ZEND_ACC_PROTECTED)) {
			/* Ensure that if we're calling a protected function, we're allowed to do so.
			 * Constructors only have prototype if they are defined by an interface but
			 * it is the compilers responsibility to take care of the prototype.
			 */
			if (!zend_check_protected(zend_get_function_root_class(constructor), EG(scope))) {
				if (EG(scope)) {
					zend_error(E_ERROR, "Call to protected %s::%s() from context '%s'", constructor->common.scope->name, constructor->common.function_name, EG(scope)->name);
				} else {
					zend_error(E_ERROR, "Call to protected %s::%s() from invalid context", constructor->common.scope->name, constructor->common.function_name);
				}
			}
		}
	}

	return constructor;
}
/* }}} */

int zend_compare_symbol_tables_i(HashTable *ht1, HashTable *ht2 TSRMLS_DC);

static int zend_std_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	zend_object *zobj1, *zobj2;

	zobj1 = Z_OBJ_P(o1);
	zobj2 = Z_OBJ_P(o2);

	if (zobj1->ce != zobj2->ce) {
		return 1; /* different classes */
	}
	return zend_compare_symbol_tables_i(zobj1->properties, zobj2->properties TSRMLS_CC);
}
/* }}} */

static int zend_std_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	int result;
	zval **value;
	zval *tmp_member = NULL;
	zend_property_info *property_info;

	zobj = Z_OBJ_P(object);

	if (Z_TYPE_P(member) != IS_STRING) {
		ALLOC_ZVAL(tmp_member);
		*tmp_member = *member;
		INIT_PZVAL(tmp_member);
		zval_copy_ctor(tmp_member);
		convert_to_string(tmp_member);
		member = tmp_member;
	}

#if DEBUG_OBJECT_HANDLERS
	fprintf(stderr, "Read object #%d property: %s\n", Z_OBJ_HANDLE_P(object), Z_STRVAL_P(member));
#endif

	property_info = zend_get_property_info(zobj->ce, member, 1 TSRMLS_CC);

	if (!property_info || zend_hash_quick_find(zobj->properties, property_info->name, property_info->name_length+1, property_info->h, (void **) &value) == FAILURE) {
		zend_guard *guard;

		result = 0;
		if ((has_set_exists != 2) &&
		    zobj->ce->__isset &&
		    zend_get_property_guard(zobj, property_info, member, &guard) == SUCCESS &&
		    !guard->in_isset) {
			zval *rv;

			/* have issetter - try with it! */
			Z_ADDREF_P(object);
			if (PZVAL_IS_REF(object)) {
				SEPARATE_ZVAL(&object);
			}
			guard->in_isset = 1; /* prevent circular getting */
			rv = zend_std_call_issetter(object, member TSRMLS_CC);
			if (rv) {
				result = zend_is_true(rv);
				zval_ptr_dtor(&rv);
				if (has_set_exists && result) {
					if (!EG(exception) && zobj->ce->__get && !guard->in_get) {
						guard->in_get = 1;
						rv = zend_std_call_getter(object, member TSRMLS_CC);
						guard->in_get = 0;
						if (rv) {
							Z_ADDREF_P(rv);
							result = i_zend_is_true(rv);
							zval_ptr_dtor(&rv);
						} else {
							result = 0;
						}
					} else {
						result = 0;
					}
				}
			}
			guard->in_isset = 0;
			zval_ptr_dtor(&object);
		}
	} else {
		switch (has_set_exists) {
		case 0:
			result = (Z_TYPE_PP(value) != IS_NULL);
			break;
		default:
			result = zend_is_true(*value);
			break;
		case 2:
			result = 1;
			break;
		}
	}

	if (tmp_member) {
		zval_ptr_dtor(&tmp_member);
	}
	return result;
}
/* }}} */

zend_class_entry *zend_std_object_get_class(const zval *object TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zobj = Z_OBJ_P(object);

	return zobj->ce;
}
/* }}} */

int zend_std_object_get_class_name(const zval *object, char **class_name, zend_uint *class_name_len, int parent TSRMLS_DC) /* {{{ */
{
	zend_object *zobj;
	zend_class_entry *ce;
	zobj = Z_OBJ_P(object);

	if (parent) {
		if (!zobj->ce->parent) {
			return FAILURE;
		}
		ce = zobj->ce->parent;
	} else {
		ce = zobj->ce;
	}

	*class_name_len = ce->name_length;
	*class_name = estrndup(ce->name, ce->name_length);
	return SUCCESS;
}
/* }}} */

ZEND_API int zend_std_cast_object_tostring(zval *readobj, zval *writeobj, int type TSRMLS_DC) /* {{{ */
{
	zval *retval;
	zend_class_entry *ce;

	switch (type) {
		case IS_STRING:
			ce = Z_OBJCE_P(readobj);
			if (ce->__tostring &&
				(zend_call_method_with_0_params(&readobj, ce, &ce->__tostring, "__tostring", &retval) || EG(exception))) {
				if (EG(exception)) {
					if (retval) {
						zval_ptr_dtor(&retval);
					}
					EG(exception) = NULL;
					zend_error(E_ERROR, "Method %s::__toString() must not throw an exception", ce->name);
					return FAILURE;
				}
				if (Z_TYPE_P(retval) == IS_STRING) {
					INIT_PZVAL(writeobj);
					if (readobj == writeobj) {
						zval_dtor(readobj);
					}
					ZVAL_ZVAL(writeobj, retval, 1, 1);
					if (Z_TYPE_P(writeobj) != type) {
						convert_to_explicit_type(writeobj, type);
					}
					return SUCCESS;
				} else {
					zval_ptr_dtor(&retval);
					INIT_PZVAL(writeobj);
					if (readobj == writeobj) {
						zval_dtor(readobj);
					}
					ZVAL_EMPTY_STRING(writeobj);
					zend_error(E_RECOVERABLE_ERROR, "Method %s::__toString() must return a string value", ce->name);
					return SUCCESS;
				}
			}
			return FAILURE;
		case IS_BOOL:
			INIT_PZVAL(writeobj);
			ZVAL_BOOL(writeobj, 1);
			return SUCCESS;
		case IS_LONG:
			ce = Z_OBJCE_P(readobj);
			zend_error(E_NOTICE, "Object of class %s could not be converted to int", ce->name);
			INIT_PZVAL(writeobj);
			if (readobj == writeobj) {
				zval_dtor(readobj);
			}
			ZVAL_LONG(writeobj, 1);
			return SUCCESS;
		case IS_DOUBLE:
			ce = Z_OBJCE_P(readobj);
			zend_error(E_NOTICE, "Object of class %s could not be converted to double", ce->name);
			INIT_PZVAL(writeobj);
			if (readobj == writeobj) {
				zval_dtor(readobj);
			}
			ZVAL_DOUBLE(writeobj, 1);
			return SUCCESS;
		default:
			INIT_PZVAL(writeobj);
			Z_TYPE_P(writeobj) = IS_NULL;
			break;
	}
	return FAILURE;
}
/* }}} */

int zend_std_get_closure(zval *obj, zend_class_entry **ce_ptr, zend_function **fptr_ptr, zval **zobj_ptr TSRMLS_DC) /* {{{ */
{
	zend_class_entry *ce;
	if (Z_TYPE_P(obj) != IS_OBJECT) {
		return FAILURE;
	}

	ce = Z_OBJCE_P(obj);

	if (zend_hash_find(&ce->function_table, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME), (void**)fptr_ptr) == FAILURE) {
		return FAILURE;
	}

	*ce_ptr = ce;
	if ((*fptr_ptr)->common.fn_flags & ZEND_ACC_STATIC) {
		if (zobj_ptr) {
			*zobj_ptr = NULL;
		}
	} else {
		if (zobj_ptr) {
			*zobj_ptr = obj;
		}
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_object_handlers std_object_handlers = {
	zend_objects_store_add_ref,				/* add_ref */
	zend_objects_store_del_ref,				/* del_ref */
	zend_objects_clone_obj,					/* clone_obj */

	zend_std_read_property,					/* read_property */
	zend_std_write_property,				/* write_property */
	zend_std_read_dimension,				/* read_dimension */
	zend_std_write_dimension,				/* write_dimension */
	zend_std_get_property_ptr_ptr,			/* get_property_ptr_ptr */
	NULL,									/* get */
	NULL,									/* set */
	zend_std_has_property,					/* has_property */
	zend_std_unset_property,				/* unset_property */
	zend_std_has_dimension,					/* has_dimension */
	zend_std_unset_dimension,				/* unset_dimension */
	zend_std_get_properties,				/* get_properties */
	zend_std_get_method,					/* get_method */
	NULL,									/* call_method */
	zend_std_get_constructor,				/* get_constructor */
	zend_std_object_get_class,				/* get_class_entry */
	zend_std_object_get_class_name,			/* get_class_name */
	zend_std_compare_objects,				/* compare_objects */
	zend_std_cast_object_tostring,			/* cast_object */
	NULL,									/* count_elements */
	NULL,									/* get_debug_info */
	zend_std_get_closure,					/* get_closure */
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
