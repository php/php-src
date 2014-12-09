/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_iterators.h"
#include "spl_array.h"
#include "spl_exceptions.h"

zend_object_handlers spl_handler_ArrayObject;
PHPAPI zend_class_entry  *spl_ce_ArrayObject;

zend_object_handlers spl_handler_ArrayIterator;
PHPAPI zend_class_entry  *spl_ce_ArrayIterator;
PHPAPI zend_class_entry  *spl_ce_RecursiveArrayIterator;

#define SPL_ARRAY_STD_PROP_LIST      0x00000001
#define SPL_ARRAY_ARRAY_AS_PROPS     0x00000002
#define SPL_ARRAY_CHILD_ARRAYS_ONLY  0x00000004
#define SPL_ARRAY_OVERLOADED_REWIND  0x00010000
#define SPL_ARRAY_OVERLOADED_VALID   0x00020000
#define SPL_ARRAY_OVERLOADED_KEY     0x00040000
#define SPL_ARRAY_OVERLOADED_CURRENT 0x00080000
#define SPL_ARRAY_OVERLOADED_NEXT    0x00100000
#define SPL_ARRAY_IS_REF             0x01000000
#define SPL_ARRAY_IS_SELF            0x02000000
#define SPL_ARRAY_USE_OTHER          0x04000000
#define SPL_ARRAY_INT_MASK           0xFFFF0000
#define SPL_ARRAY_CLONE_MASK         0x0300FFFF

#define SPL_ARRAY_METHOD_NO_ARG				0
#define SPL_ARRAY_METHOD_USE_ARG    		1
#define SPL_ARRAY_METHOD_MAY_USER_ARG 		2

typedef struct _spl_array_object {
	zval              array;
	zval              retval;
	HashPosition      pos;
	zend_ulong             pos_h;
	int               ar_flags;
	int               is_self;
	zend_function     *fptr_offset_get;
	zend_function     *fptr_offset_set;
	zend_function     *fptr_offset_has;
	zend_function     *fptr_offset_del;
	zend_function     *fptr_count;
	zend_class_entry* ce_get_iterator;
	HashTable         *debug_info;
	unsigned char	  nApplyCount;
	zend_object       std;
} spl_array_object;

static inline spl_array_object *spl_array_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_array_object*)((char*)(obj) - XtOffsetOf(spl_array_object, std));
}
/* }}} */

#define Z_SPLARRAY_P(zv)  spl_array_from_obj(Z_OBJ_P((zv)))

static inline HashTable *spl_array_get_hash_table(spl_array_object* intern, int check_std_props TSRMLS_DC) { /* {{{ */
	if ((intern->ar_flags & SPL_ARRAY_IS_SELF) != 0) {
		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		return intern->std.properties;
	} else if ((intern->ar_flags & SPL_ARRAY_USE_OTHER) && (check_std_props == 0 || (intern->ar_flags & SPL_ARRAY_STD_PROP_LIST) == 0) && Z_TYPE(intern->array) == IS_OBJECT) {
		spl_array_object *other = Z_SPLARRAY_P(&intern->array);
		return spl_array_get_hash_table(other, check_std_props TSRMLS_CC);
	} else if ((intern->ar_flags & ((check_std_props ? SPL_ARRAY_STD_PROP_LIST : 0) | SPL_ARRAY_IS_SELF)) != 0) {
		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		return intern->std.properties;
	} else {
		return HASH_OF(&intern->array);
	}
} /* }}} */

static void spl_array_rewind(spl_array_object *intern TSRMLS_DC);

static void spl_array_update_pos(HashTable *ht, spl_array_object* intern) /* {{{ */
{
	uint pos = intern->pos;
	if (pos != INVALID_IDX) {
		intern->pos_h = BUCKET_HASH_VAL(&ht->arData[pos]);
	}
} /* }}} */

static void spl_array_set_pos(spl_array_object* intern, HashTable *ht, HashPosition pos) /* {{{ */
{
	intern->pos = pos;
	spl_array_update_pos(ht, intern);
} /* }}} */

SPL_API int spl_hash_verify_pos_ex(spl_array_object * intern, HashTable * ht TSRMLS_DC) /* {{{ */
{
	uint idx;

/*	IS_CONSISTENT(ht);*/

/*	HASH_PROTECT_RECURSION(ht);*/
	if (ht->u.flags & HASH_FLAG_PACKED) {
		if (intern->pos_h == intern->pos && Z_TYPE(ht->arData[intern->pos_h].val) != IS_UNDEF) {
			return SUCCESS;
		}
	} else {
		idx = ht->arHash[intern->pos_h & ht->nTableMask];
		while (idx != INVALID_IDX) {		
			if (idx == intern->pos) {
				return SUCCESS;
			}
			idx = Z_NEXT(ht->arData[idx].val);
		}
	}
/*	HASH_UNPROTECT_RECURSION(ht); */
	spl_array_rewind(intern TSRMLS_CC);
	return FAILURE;

} /* }}} */

SPL_API int spl_hash_verify_pos(spl_array_object * intern TSRMLS_DC) /* {{{ */
{
	HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	return spl_hash_verify_pos_ex(intern, ht TSRMLS_CC);
}
/* }}} */

/* {{{ spl_array_object_free_storage */
static void spl_array_object_free_storage(zend_object *object TSRMLS_DC)
{
	spl_array_object *intern = spl_array_from_obj(object);

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	zval_ptr_dtor(&intern->array);
	zval_ptr_dtor(&intern->retval);

	if (intern->debug_info != NULL) {
		zend_hash_destroy(intern->debug_info);
		efree(intern->debug_info);
	}
}
/* }}} */

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC);

/* {{{ spl_array_object_new_ex */
static zend_object *spl_array_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig TSRMLS_DC)
{
	spl_array_object *intern;
	zend_class_entry *parent = class_type;
	int inherited = 0;

	intern = ecalloc(1, sizeof(spl_array_object) + sizeof(zval) * (parent->default_properties_count - 1));

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	intern->ar_flags = 0;
	intern->debug_info       = NULL;
	intern->ce_get_iterator = spl_ce_ArrayIterator;
	if (orig) {
		spl_array_object *other = Z_SPLARRAY_P(orig);

		intern->ar_flags &= ~ SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= (other->ar_flags & SPL_ARRAY_CLONE_MASK);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (clone_orig) {
			intern->array = other->array;
			if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayObject) {
				ZVAL_NEW_ARR(&intern->array);
				zend_array_dup(Z_ARRVAL(intern->array), HASH_OF(&other->array));
			}
			if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayIterator) {
				Z_ADDREF_P(&other->array);
			}
		} else {
			intern->array = *orig;
			Z_ADDREF_P(&intern->array);
			intern->ar_flags |= SPL_ARRAY_IS_REF | SPL_ARRAY_USE_OTHER;
		}
	} else {
		array_init(&intern->array);
		intern->ar_flags &= ~SPL_ARRAY_IS_REF;
	}

	while (parent) {
		if (parent == spl_ce_ArrayIterator || parent == spl_ce_RecursiveArrayIterator) {
			intern->std.handlers = &spl_handler_ArrayIterator;
			class_type->get_iterator = spl_array_get_iterator;
			break;
		} else if (parent == spl_ce_ArrayObject) {
			intern->std.handlers = &spl_handler_ArrayObject;
			break;
		}
		parent = parent->parent;
		inherited = 1;
	}
	if (!parent) { /* this must never happen */
		php_error_docref(NULL TSRMLS_CC, E_COMPILE_ERROR, "Internal compiler error, Class is not child of ArrayObject or ArrayIterator");
	}
	if (inherited) {
		intern->fptr_offset_get = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", sizeof("offsetget") - 1);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		intern->fptr_offset_set = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", sizeof("offsetset") - 1);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		intern->fptr_offset_has = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", sizeof("offsetexists") - 1);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		intern->fptr_offset_del = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset",  sizeof("offsetunset") - 1);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		intern->fptr_count = zend_hash_str_find_ptr(&class_type->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}
	}
	/* Cache iterator functions if ArrayIterator or derived. Check current's */
	/* cache since only current is always required */
	if (intern->std.handlers == &spl_handler_ArrayIterator) {
		if (!class_type->iterator_funcs.zf_current) {
			class_type->iterator_funcs.zf_rewind = zend_hash_str_find_ptr(&class_type->function_table, "rewind", sizeof("rewind") - 1);
			class_type->iterator_funcs.zf_valid = zend_hash_str_find_ptr(&class_type->function_table, "valid", sizeof("valid") - 1);
			class_type->iterator_funcs.zf_key = zend_hash_str_find_ptr(&class_type->function_table, "key", sizeof("key") - 1);
			class_type->iterator_funcs.zf_current = zend_hash_str_find_ptr(&class_type->function_table, "current", sizeof("current") - 1);
			class_type->iterator_funcs.zf_next = zend_hash_str_find_ptr(&class_type->function_table, "next", sizeof("next") - 1);
		}
		if (inherited) {
			if (class_type->iterator_funcs.zf_rewind->common.scope  != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_REWIND;
			if (class_type->iterator_funcs.zf_valid->common.scope   != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_VALID;
			if (class_type->iterator_funcs.zf_key->common.scope     != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_KEY;
			if (class_type->iterator_funcs.zf_current->common.scope != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_CURRENT;
			if (class_type->iterator_funcs.zf_next->common.scope    != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_NEXT;
		}
	}

	spl_array_rewind(intern TSRMLS_CC);
	return &intern->std;
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object *spl_array_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	return spl_array_object_new_ex(class_type, NULL, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ spl_array_object_clone */
static zend_object *spl_array_object_clone(zval *zobject TSRMLS_DC)
{
	zend_object *old_object;
	zend_object *new_object;

	old_object = Z_OBJ_P(zobject);
	new_object = spl_array_object_new_ex(old_object->ce, zobject, 1 TSRMLS_CC);

	zend_objects_clone_members(new_object, old_object TSRMLS_CC);

	return new_object;
}
/* }}} */

static zval *spl_array_get_dimension_ptr(int check_inherited, zval *object, zval *offset, int type TSRMLS_DC) /* {{{ */
{
	zval *retval;
	zend_long index;
	zend_string *offset_key;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!offset || Z_ISUNDEF_P(offset)) {
		return &EG(uninitialized_zval);
	}

	if ((type == BP_VAR_W || type == BP_VAR_RW) && (ht->u.v.nApplyCount > 0)) {
		zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
		return &EG(error_zval);;
	}

	switch (Z_TYPE_P(offset)) {
	case IS_NULL:
	   offset_key = STR_EMPTY_ALLOC();
	   goto fetch_dim_string;
	case IS_STRING:
	   offset_key = Z_STR_P(offset);
fetch_dim_string:
		retval = zend_symtable_find(ht, offset_key);
		if (retval) {
			if (Z_TYPE_P(retval) == IS_INDIRECT) {
				retval = Z_INDIRECT_P(retval);
				if (Z_TYPE_P(retval) == IS_UNDEF) {
					switch (type) {
						case BP_VAR_R:
							zend_error(E_NOTICE, "Undefined index: %s", offset_key->val);
						case BP_VAR_UNSET:
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval);
							break;
						case BP_VAR_RW:
							zend_error(E_NOTICE,"Undefined index: %s", offset_key->val);
						case BP_VAR_W: {
							ZVAL_NULL(retval);
						}
					}
				}
			}
		} else {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_NOTICE, "Undefined index: %s", offset_key->val);
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined index: %s", offset_key->val);
				case BP_VAR_W: {
				    zval value;
					ZVAL_NULL(&value);
				    retval = zend_symtable_update(ht, offset_key, &value);
				}
			}
		}
		return retval;
	case IS_RESOURCE:
		zend_error(E_STRICT, "Resource ID#%pd used as offset, casting to integer (%pd)", Z_RES_P(offset)->handle, Z_RES_P(offset)->handle);
		index = Z_RES_P(offset)->handle;
		goto num_index;
	case IS_DOUBLE:
		index = (zend_long)Z_DVAL_P(offset);
		goto num_index;
	case IS_FALSE:
		index = 0;
		goto num_index;
	case IS_TRUE:
		index = 1;
		goto num_index;
	case IS_LONG:
		index = Z_LVAL_P(offset);
num_index:
		if ((retval = zend_hash_index_find(ht, index)) == NULL) {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_NOTICE, "Undefined offset: %pd", index);
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE, "Undefined offset: %pd", index);
				case BP_VAR_W: {
				    zval value;
					ZVAL_UNDEF(&value);
					retval = zend_hash_index_update(ht, index, &value);
			   }
			}
		}
		return retval;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return (type == BP_VAR_W || type == BP_VAR_RW) ?
			&EG(error_zval) : &EG(uninitialized_zval);
	}
} /* }}} */

static zval *spl_array_read_dimension_ex(int check_inherited, zval *object, zval *offset, int type, zval *zv TSRMLS_DC) /* {{{ */
{
	zval *ret;

	if (check_inherited) {
		spl_array_object *intern = Z_SPLARRAY_P(object);
		if (intern->fptr_offset_get) {
			zval rv, tmp;
			if (!offset) {
				ZVAL_UNDEF(&tmp);
				offset = &tmp;
			} else {
				SEPARATE_ARG_IF_REF(offset);
			}
			zend_call_method_with_1_params(object, Z_OBJCE_P(object), &intern->fptr_offset_get, "offsetGet", &rv, offset);
			zval_ptr_dtor(offset);
			if (!Z_ISUNDEF(rv)) {
				zval_ptr_dtor(&intern->retval);
				ZVAL_ZVAL(&intern->retval, &rv, 0, 0);
				return &intern->retval;
			}
			return &EG(uninitialized_zval);
		}
	}
	ret = spl_array_get_dimension_ptr(check_inherited, object, offset, type TSRMLS_CC);
	//!!! FIXME?
	//	ZVAL_COPY(result, ret);

	/* When in a write context,
	 * ZE has to be fooled into thinking this is in a reference set
	 * by separating (if necessary) and returning as an is_ref=1 zval (even if refcount == 1) 
	 */
	
	if ((type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) &&
	    !Z_ISREF_P(ret) &&
	    EXPECTED(ret != &EG(uninitialized_zval))) {
		ZVAL_NEW_REF(ret, ret);
	}

	return ret;
} /* }}} */

static zval *spl_array_read_dimension(zval *object, zval *offset, int type, zval *rv TSRMLS_DC) /* {{{ */
{
	return spl_array_read_dimension_ex(1, object, offset, type, rv TSRMLS_CC);
} /* }}} */

static void spl_array_write_dimension_ex(int check_inherited, zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_long index;
	HashTable *ht;

	if (check_inherited && intern->fptr_offset_set) {
		zval tmp;

		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		zend_call_method_with_2_params(object, Z_OBJCE_P(object), &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		zval_ptr_dtor(offset);
		return;
	}

	if (!offset) {
		ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
		if (ht->u.v.nApplyCount > 0) {
			zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
			return;
		}
		if (Z_REFCOUNTED_P(value)) {
			Z_ADDREF_P(value);
		}
		zend_hash_next_index_insert(ht, value);
		return;
	}

	if (Z_REFCOUNTED_P(value)) {
		Z_ADDREF_P(value);
	}
	switch (Z_TYPE_P(offset)) {
		case IS_STRING:
			ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
			if (ht->u.v.nApplyCount > 0) {
				zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
				return;
			}
			zend_symtable_update_ind(ht, Z_STR_P(offset), value);
			return;
		case IS_DOUBLE:
			index = (zend_long)Z_DVAL_P(offset);
			goto num_index;
		case IS_RESOURCE:
			index = Z_RES_HANDLE_P(offset);
			goto num_index;
		case IS_FALSE:
			index = 0;
			goto num_index;
		case IS_TRUE:
			index = 1;
			goto num_index;
		case IS_LONG:
			index = Z_LVAL_P(offset);
num_index:
			ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
			if (ht->u.v.nApplyCount > 0) {
				zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
				return;
			}
			zend_hash_index_update(ht, index, value);
			return;
		case IS_NULL:
			ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
			if (ht->u.v.nApplyCount > 0) {
				zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
				return;
			}
			zend_hash_next_index_insert(ht, value);
			return;
		default:
			zend_error(E_WARNING, "Illegal offset type");
			return;
	}
} /* }}} */

static void spl_array_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC) /* {{{ */
{
	spl_array_write_dimension_ex(1, object, offset, value TSRMLS_CC);
} /* }}} */

static void spl_array_unset_dimension_ex(int check_inherited, zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	zend_long index;
	HashTable *ht;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (check_inherited && intern->fptr_offset_del) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, Z_OBJCE_P(object), &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		zval_ptr_dtor(offset);
		return;
	}

	switch(Z_TYPE_P(offset)) {
	case IS_STRING:
		ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
		if (ht->u.v.nApplyCount > 0) {
			zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
			return;
		}
		if (ht == &EG(symbol_table).ht) {
			if (zend_delete_global_variable(Z_STR_P(offset) TSRMLS_CC)) {
				zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
			}
		} else {
//??? see below
#if 0
			if (zend_symtable_del_ind(ht, Z_STR_P(offset)) == FAILURE) {
#else 
			zval *data = zend_symtable_find(ht, Z_STR_P(offset));

			if (data) {
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) == IS_UNDEF) {
						zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
					} else {
						zval_ptr_dtor(data);
						ZVAL_UNDEF(data);
					}
//??? fix for ext/spl/tests/bug45614.phpt (may be fix is wrong)
					spl_array_rewind(intern TSRMLS_CC);
				} else if (zend_symtable_del(ht, Z_STR_P(offset)) == FAILURE) {
					zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
				}
			} else {
#endif
				zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
			}
		}
		break;
	case IS_DOUBLE:
		index = (zend_long)Z_DVAL_P(offset);
		goto num_index;
	case IS_RESOURCE:
		index = Z_RES_HANDLE_P(offset);
		goto num_index;
	case IS_FALSE:
		index = 0;
		goto num_index;
	case IS_TRUE:
		index = 1;
		goto num_index;
	case IS_LONG:
		index = Z_LVAL_P(offset);
num_index:
		ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
		if (ht->u.v.nApplyCount > 0) {
			zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
			return;
		}
		if (zend_hash_index_del(ht, index) == FAILURE) {
			zend_error(E_NOTICE,"Undefined offset: %pd", index);
		}
		break;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
	spl_hash_verify_pos(intern TSRMLS_CC); /* call rewind on FAILURE */
} /* }}} */

static void spl_array_unset_dimension(zval *object, zval *offset TSRMLS_DC) /* {{{ */
{
	spl_array_unset_dimension_ex(1, object, offset TSRMLS_CC);
} /* }}} */

static int spl_array_has_dimension_ex(int check_inherited, zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_long index;
	zval rv, *value = NULL, *tmp;

	if (check_inherited && intern->fptr_offset_has) {
//???		zval offset_tmp;
//???		ZVAL_COPY_VALUE(&offset_tmp, offset);
//???		SEPARATE_ARG_IF_REF(&offset_tmp);
//???		zend_call_method_with_1_params(object, Z_OBJCE_P(object), &intern->fptr_offset_has, "offsetExists", &rv, &offset_tmp);
//???		zval_ptr_dtor(&offset_tmp);
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, Z_OBJCE_P(object), &intern->fptr_offset_has, "offsetExists", &rv, offset);
		zval_ptr_dtor(offset);

		if (!Z_ISUNDEF(rv) && zend_is_true(&rv TSRMLS_CC)) {
			zval_ptr_dtor(&rv);
			if (check_empty != 1) {
				return 1;
			} else if (intern->fptr_offset_get) {
				value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R, &rv TSRMLS_CC);
			}
		} else {
			zval_ptr_dtor(&rv);
			return 0;
		}
	}

	if (!value) {
		HashTable *ht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

		switch(Z_TYPE_P(offset)) {
			case IS_STRING: 
				if ((tmp = zend_symtable_find(ht, Z_STR_P(offset))) != NULL) {
					if (check_empty == 2) {
						return 1;
					}
				} else {
					return 0;
				}
				break;

			case IS_DOUBLE:
				index = (zend_long)Z_DVAL_P(offset);
				goto num_index;
			case IS_RESOURCE:
				index = Z_RES_HANDLE_P(offset);
				goto num_index;
			case IS_FALSE: 
				index = 0;
				goto num_index;
			case IS_TRUE: 
				index = 1;
				goto num_index;
			case IS_LONG:
				index = Z_LVAL_P(offset);
num_index:
				if ((tmp = zend_hash_index_find(ht, index)) != NULL) {
					if (check_empty == 2) {
						return 1;
					}
				} else {
					return 0;
				}
				break;

			default:
				zend_error(E_WARNING, "Illegal offset type");
				return 0;
		}

		if (check_empty && check_inherited && intern->fptr_offset_get) {
			value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R, &rv TSRMLS_CC);
		} else {
			value = tmp;
		}
	}

	return check_empty ? zend_is_true(value TSRMLS_CC) : Z_TYPE_P(value) != IS_NULL;
} /* }}} */

static int spl_array_has_dimension(zval *object, zval *offset, int check_empty TSRMLS_DC) /* {{{ */
{
	return spl_array_has_dimension_ex(1, object, offset, check_empty TSRMLS_CC);
} /* }}} */

/* {{{ spl_array_object_verify_pos_ex */
static inline int spl_array_object_verify_pos_ex(spl_array_object *object, HashTable *ht, const char *msg_prefix TSRMLS_DC)
{
	if (!ht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%sArray was modified outside object and is no longer an array", msg_prefix);
		return FAILURE;
	}

	if (object->pos != INVALID_IDX && (object->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos_ex(object, ht TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "%sArray was modified outside object and internal position is no longer valid", msg_prefix);
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */

/* {{{ spl_array_object_verify_pos */
static inline int spl_array_object_verify_pos(spl_array_object *object, HashTable *ht TSRMLS_DC)
{
	return spl_array_object_verify_pos_ex(object, ht, "" TSRMLS_CC);
} /* }}} */

/* {{{ proto bool ArrayObject::offsetExists(mixed $index)
       proto bool ArrayIterator::offsetExists(mixed $index)
   Returns whether the requested $index exists. */
SPL_METHOD(Array, offsetExists)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	RETURN_BOOL(spl_array_has_dimension_ex(0, getThis(), index, 2 TSRMLS_CC));
} /* }}} */

/* {{{ proto mixed ArrayObject::offsetGet(mixed $index)
       proto mixed ArrayIterator::offsetGet(mixed $index)
   Returns the value at the specified $index. */
SPL_METHOD(Array, offsetGet)
{
	zval *value, *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	value = spl_array_read_dimension_ex(0, getThis(), index, BP_VAR_R, return_value TSRMLS_CC);
	if (value != return_value) {
		RETURN_ZVAL(value, 1, 0);
	}
} /* }}} */

/* {{{ proto void ArrayObject::offsetSet(mixed $index, mixed $newval)
       proto void ArrayIterator::offsetSet(mixed $index, mixed $newval)
   Sets the value at the specified $index to $newval. */
SPL_METHOD(Array, offsetSet)
{
	zval *index, *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &index, &value) == FAILURE) {
		return;
	}
	spl_array_write_dimension_ex(0, getThis(), index, value TSRMLS_CC);
} /* }}} */

void spl_array_iterator_append(zval *object, zval *append_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (Z_TYPE(intern->array) == IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Cannot append properties to objects, use %s::offsetSet() instead", Z_OBJCE_P(object)->name->val);
		return;
	}

	spl_array_write_dimension(object, NULL, append_value TSRMLS_CC);
	if (intern->pos == INVALID_IDX) {
		if (aht->nNumUsed && !Z_ISUNDEF(aht->arData[aht->nNumUsed-1].val)) {
			spl_array_set_pos(intern, aht, aht->nNumUsed - 1);
		}
	}
} /* }}} */

/* {{{ proto void ArrayObject::append(mixed $newval)
       proto void ArrayIterator::append(mixed $newval)
   Appends the value (cannot be called for objects). */
SPL_METHOD(Array, append)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &value) == FAILURE) {
		return;
	}
	spl_array_iterator_append(getThis(), value TSRMLS_CC);
} /* }}} */

/* {{{ proto void ArrayObject::offsetUnset(mixed $index)
       proto void ArrayIterator::offsetUnset(mixed $index)
   Unsets the value at the specified $index. */
SPL_METHOD(Array, offsetUnset)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		return;
	}
	spl_array_unset_dimension_ex(0, getThis(), index TSRMLS_CC);
} /* }}} */

/* {{{ proto array ArrayObject::getArrayCopy()
      proto array ArrayIterator::getArrayCopy()
   Return a copy of the contained array */
SPL_METHOD(Array, getArrayCopy)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);

	ZVAL_NEW_ARR(return_value);
	zend_array_dup(Z_ARRVAL_P(return_value), spl_array_get_hash_table(intern, 0 TSRMLS_CC));
} /* }}} */

static HashTable *spl_array_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *result;

	if (intern->nApplyCount > 1) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Nesting level too deep - recursive dependency?");
	}

	intern->nApplyCount++;
	result = spl_array_get_hash_table(intern, 1 TSRMLS_CC);
	intern->nApplyCount--;
	return result;
} /* }}} */

static HashTable* spl_array_get_debug_info(zval *obj, int *is_temp TSRMLS_DC) /* {{{ */
{
	zval *storage;
	zend_string *zname;
	zend_class_entry *base;
	spl_array_object *intern = Z_SPLARRAY_P(obj);

	*is_temp = 0;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (HASH_OF(&intern->array) == intern->std.properties) {
		return intern->std.properties;
	} else {
		if (intern->debug_info == NULL) {
			ALLOC_HASHTABLE(intern->debug_info);
			ZEND_INIT_SYMTABLE_EX(intern->debug_info, zend_hash_num_elements(intern->std.properties) + 1, 0);
		}

		if (intern->debug_info->u.v.nApplyCount == 0) {
			zend_hash_clean(intern->debug_info);
			zend_hash_copy(intern->debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref);

			storage = &intern->array;
			zval_add_ref(storage);

			base = (Z_OBJ_HT_P(obj) == &spl_handler_ArrayIterator) ? spl_ce_ArrayIterator : spl_ce_ArrayObject;
			zname = spl_gen_private_prop_name(base, "storage", sizeof("storage")-1 TSRMLS_CC);
			zend_symtable_update(intern->debug_info, zname, storage);
			zend_string_release(zname);
		}

		return intern->debug_info;
	}
}
/* }}} */

static zval *spl_array_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !std_object_handlers.has_property(object, member, 2, cache_slot TSRMLS_CC)) {
		return spl_array_read_dimension(object, member, type, rv TSRMLS_CC);
	}
	return std_object_handlers.read_property(object, member, type, cache_slot, rv TSRMLS_CC);
} /* }}} */

static void spl_array_write_property(zval *object, zval *member, zval *value, void **cache_slot TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !std_object_handlers.has_property(object, member, 2, cache_slot TSRMLS_CC)) {
		spl_array_write_dimension(object, member, value TSRMLS_CC);
		return;
	}
	std_object_handlers.write_property(object, member, value, cache_slot TSRMLS_CC);
} /* }}} */

static zval *spl_array_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !std_object_handlers.has_property(object, member, 2, cache_slot TSRMLS_CC)) {
		return spl_array_get_dimension_ptr(1, object, member, type TSRMLS_CC);
	}
	//!!! FIXME
	//return std_object_handlers.get_property_ptr_ptr(object, member, type, key TSRMLS_CC);
	return NULL;
} /* }}} */

static int spl_array_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !std_object_handlers.has_property(object, member, 2, cache_slot TSRMLS_CC)) {
		return spl_array_has_dimension(object, member, has_set_exists TSRMLS_CC);
	}
	return std_object_handlers.has_property(object, member, has_set_exists, cache_slot TSRMLS_CC);
} /* }}} */

static void spl_array_unset_property(zval *object, zval *member, void **cache_slot TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !std_object_handlers.has_property(object, member, 2, cache_slot TSRMLS_CC)) {
		spl_array_unset_dimension(object, member TSRMLS_CC);
		spl_array_rewind(intern TSRMLS_CC); /* because deletion might invalidate position */
		return;
	}
	std_object_handlers.unset_property(object, member, cache_slot TSRMLS_CC);
} /* }}} */

static int spl_array_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	HashTable			*ht1,
						*ht2;
	spl_array_object	*intern1,
						*intern2;
	int					result	= 0;
	zval				temp_zv;

	intern1	= Z_SPLARRAY_P(o1);
	intern2	= Z_SPLARRAY_P(o2);
	ht1		= spl_array_get_hash_table(intern1, 0 TSRMLS_CC);
	ht2		= spl_array_get_hash_table(intern2, 0 TSRMLS_CC);

	zend_compare_symbol_tables(&temp_zv, ht1, ht2 TSRMLS_CC);
	result = (int)Z_LVAL(temp_zv);
	/* if we just compared std.properties, don't do it again */
	if (result == 0 &&
			!(ht1 == intern1->std.properties && ht2 == intern2->std.properties)) {
		result = std_object_handlers.compare_objects(o1, o2 TSRMLS_CC);
	}
	return result;
} /* }}} */

static int spl_array_skip_protected(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{
	zend_string *string_key;
	zend_ulong num_key;
	zval *data;

	if (Z_TYPE(intern->array) == IS_OBJECT) {
		do {
			if (zend_hash_get_current_key_ex(aht, &string_key, &num_key, 0, &intern->pos) == HASH_KEY_IS_STRING) {
				data = zend_hash_get_current_data_ex(aht, &intern->pos);
				if (data && Z_TYPE_P(data) == IS_INDIRECT &&
				    Z_TYPE_P(data = Z_INDIRECT_P(data)) == IS_UNDEF) {
					/* skip */
				} else if (!string_key->len || string_key->val[0]) {
					return SUCCESS;
				}
			} else {
				return SUCCESS;
			}
			if (zend_hash_has_more_elements_ex(aht, &intern->pos) != SUCCESS) {
				return FAILURE;
			}
			zend_hash_move_forward_ex(aht, &intern->pos);
			spl_array_update_pos(aht, intern);
		} while (1);
	}
	return FAILURE;
} /* }}} */

static int spl_array_next_no_verify(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{
	zend_hash_move_forward_ex(aht, &intern->pos);
	spl_array_update_pos(aht, intern);
	if (Z_TYPE(intern->array) == IS_OBJECT) {
		return spl_array_skip_protected(intern, aht TSRMLS_CC);
	} else {
		return zend_hash_has_more_elements_ex(aht, &intern->pos);
	}
} /* }}} */

static int spl_array_next_ex(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{
	if ((intern->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos_ex(intern, aht TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and internal position is no longer valid");
		return FAILURE;
	}

	return spl_array_next_no_verify(intern, aht TSRMLS_CC);
} /* }}} */

static int spl_array_next(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	return spl_array_next_ex(intern, aht TSRMLS_CC);

} /* }}} */

static void spl_array_it_dtor(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	zend_user_it_invalidate_current(iter TSRMLS_CC);
	zval_ptr_dtor(&iter->data);
}
/* }}} */

static int spl_array_it_valid(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_VALID) {
		return zend_user_it_valid(iter TSRMLS_CC);
	} else {
		if (spl_array_object_verify_pos_ex(object, aht, "ArrayIterator::valid(): " TSRMLS_CC) == FAILURE) {
			return FAILURE;
		}

		return zend_hash_has_more_elements_ex(aht, &object->pos);
	}
}
/* }}} */

static zval *spl_array_it_get_current_data(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT) {
		return zend_user_it_get_current_data(iter TSRMLS_CC);
	} else {
		zval *data = zend_hash_get_current_data_ex(aht, &object->pos);
		if (Z_TYPE_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
		}
		return data;
	}
}
/* }}} */

static void spl_array_it_get_current_key(zend_object_iterator *iter, zval *key TSRMLS_DC) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_KEY) {
		zend_user_it_get_current_key(iter, key TSRMLS_CC);
	} else {
		if (spl_array_object_verify_pos_ex(object, aht, "ArrayIterator::current(): " TSRMLS_CC) == FAILURE) {
			ZVAL_NULL(key);
		} else {
			zend_hash_get_current_key_zval_ex(aht, key, &object->pos);
		}
	}
}
/* }}} */

static void spl_array_it_move_forward(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object, 0 TSRMLS_CC);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_NEXT) {
		zend_user_it_move_forward(iter TSRMLS_CC);
	} else {
		zend_user_it_invalidate_current(iter TSRMLS_CC);
		if (!aht) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and is no longer an array");
			return;
		}

		if ((object->ar_flags & SPL_ARRAY_IS_REF) && spl_hash_verify_pos_ex(object, aht TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::next(): Array was modified outside object and internal position is no longer valid");
		} else {
			spl_array_next_no_verify(object, aht TSRMLS_CC);
		}
	}
}
/* }}} */

static void spl_array_rewind_ex(spl_array_object *intern, HashTable *aht TSRMLS_DC) /* {{{ */
{

	zend_hash_internal_pointer_reset_ex(aht, &intern->pos);
	spl_array_update_pos(aht, intern);
	spl_array_skip_protected(intern, aht TSRMLS_CC);

} /* }}} */

static void spl_array_rewind(spl_array_object *intern TSRMLS_DC) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "ArrayIterator::rewind(): Array was modified outside object and is no longer an array");
		return;
	}

	spl_array_rewind_ex(intern, aht TSRMLS_CC);
}
/* }}} */

static void spl_array_it_rewind(zend_object_iterator *iter TSRMLS_DC) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_REWIND) {
		zend_user_it_rewind(iter TSRMLS_CC);
	} else {
		zend_user_it_invalidate_current(iter TSRMLS_CC);
		spl_array_rewind(object TSRMLS_CC);
	}
}
/* }}} */

/* {{{ spl_array_set_array */
static void spl_array_set_array(zval *object, spl_array_object *intern, zval *array, zend_long ar_flags, int just_array TSRMLS_DC) {

	if (Z_TYPE_P(array) == IS_ARRAY) {
		SEPARATE_ARRAY(array);
	}

	if (Z_TYPE_P(array) == IS_OBJECT && (Z_OBJ_HT_P(array) == &spl_handler_ArrayObject || Z_OBJ_HT_P(array) == &spl_handler_ArrayIterator)) {
		zval_ptr_dtor(&intern->array);
		if (just_array)	{
			spl_array_object *other = Z_SPLARRAY_P(array);
			ar_flags = other->ar_flags & ~SPL_ARRAY_INT_MASK;
		}
		ar_flags |= SPL_ARRAY_USE_OTHER;
		ZVAL_COPY_VALUE(&intern->array, array);
	} else {
		if (Z_TYPE_P(array) != IS_OBJECT && Z_TYPE_P(array) != IS_ARRAY) {
			zend_throw_exception(spl_ce_InvalidArgumentException, "Passed variable is not an array or object, using empty array instead", 0 TSRMLS_CC);
			return;
		}
		zval_ptr_dtor(&intern->array);
		ZVAL_COPY_VALUE(&intern->array, array);
	}
	if (Z_TYPE_P(array) == IS_OBJECT && Z_OBJ_P(object) == Z_OBJ_P(array)) {
		intern->ar_flags |= SPL_ARRAY_IS_SELF;
		intern->ar_flags &= ~SPL_ARRAY_USE_OTHER;
	} else {
		intern->ar_flags &= ~SPL_ARRAY_IS_SELF;
	}
	intern->ar_flags |= ar_flags;
	Z_ADDREF_P(&intern->array);
	if (Z_TYPE_P(array) == IS_OBJECT) {
		zend_object_get_properties_t handler = Z_OBJ_HANDLER_P(array, get_properties);
		if ((handler != std_object_handlers.get_properties && handler != spl_array_get_properties)
		|| !spl_array_get_hash_table(intern, 0 TSRMLS_CC)) {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Overloaded object of type %s is not compatible with %s", Z_OBJCE_P(array)->name, intern->std.ce->name);
		}
	}

	spl_array_rewind(intern TSRMLS_CC);
}
/* }}} */

/* iterator handler table */
zend_object_iterator_funcs spl_array_it_funcs = {
	spl_array_it_dtor,
	spl_array_it_valid,
	spl_array_it_get_current_data,
	spl_array_it_get_current_key,
	spl_array_it_move_forward,
	spl_array_it_rewind
};

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref TSRMLS_DC) /* {{{ */
{
	zend_user_iterator *iterator;
	spl_array_object *array_object = Z_SPLARRAY_P(object);

	if (by_ref && (array_object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT)) {
		zend_error(E_ERROR, "An iterator cannot be used with foreach by reference");
	}

	iterator = emalloc(sizeof(zend_user_iterator));

	zend_iterator_init(&iterator->it TSRMLS_CC);

	ZVAL_COPY(&iterator->it.data, object);
	iterator->it.funcs = &spl_array_it_funcs;
	iterator->ce = ce;
	ZVAL_UNDEF(&iterator->value);

	return &iterator->it;
}
/* }}} */

/* {{{ proto void ArrayObject::__construct(array|object ar = array() [, int flags = 0 [, string iterator_class = "ArrayIterator"]])
       proto void ArrayIterator::__construct(array|object ar = array() [, int flags = 0])
   Constructs a new array iterator from a path. */
SPL_METHOD(Array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval *array;
	zend_long ar_flags = 0;
	zend_class_entry *ce_get_iterator = spl_ce_Iterator;
	zend_error_handling error_handling;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

	intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|lC", &array, &ar_flags, &ce_get_iterator) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	if (ZEND_NUM_ARGS() > 2) {
		intern->ce_get_iterator = ce_get_iterator;
	}

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	spl_array_set_array(object, intern, array, ar_flags, ZEND_NUM_ARGS() == 1 TSRMLS_CC);

	zend_restore_error_handling(&error_handling TSRMLS_CC);

}
 /* }}} */

/* {{{ proto void ArrayObject::setIteratorClass(string iterator_class)
   Set the class used in getIterator. */
SPL_METHOD(Array, setIteratorClass)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_class_entry * ce_get_iterator = spl_ce_Iterator;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "C", &ce_get_iterator) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_CLASS(ce_get_iterator)
	ZEND_PARSE_PARAMETERS_END();
#endif

	intern->ce_get_iterator = ce_get_iterator;
}
/* }}} */

/* {{{ proto string ArrayObject::getIteratorClass()
   Get the class used in getIterator. */
SPL_METHOD(Array, getIteratorClass)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_string_addref(intern->ce_get_iterator->name);
	RETURN_STR(intern->ce_get_iterator->name);
}
/* }}} */

/* {{{ proto int ArrayObject::getFlags()
   Get flags */
SPL_METHOD(Array, getFlags)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->ar_flags & ~SPL_ARRAY_INT_MASK);
}
/* }}} */

/* {{{ proto void ArrayObject::setFlags(int flags)
   Set flags */
SPL_METHOD(Array, setFlags)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_long ar_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ar_flags) == FAILURE) {
		return;
	}

	intern->ar_flags = (intern->ar_flags & SPL_ARRAY_INT_MASK) | (ar_flags & ~SPL_ARRAY_INT_MASK);
}
/* }}} */

/* {{{ proto Array|Object ArrayObject::exchangeArray(Array|Object ar = array())
   Replace the referenced array or object with a new one and return the old one (right now copy - to be changed) */
SPL_METHOD(Array, exchangeArray)
{
	zval *object = getThis(), *array;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	ZVAL_NEW_ARR(return_value);
	zend_array_dup(Z_ARRVAL_P(return_value), spl_array_get_hash_table(intern, 0 TSRMLS_CC));
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &array) == FAILURE) {
		return;
	}

	spl_array_set_array(object, intern, array, 0L, 1 TSRMLS_CC);
}
/* }}} */

/* {{{ proto ArrayIterator ArrayObject::getIterator()
   Create a new iterator from a ArrayObject instance */
SPL_METHOD(Array, getIterator)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	ZVAL_OBJ(return_value, spl_array_object_new_ex(intern->ce_get_iterator, object, 0 TSRMLS_CC));
	Z_SET_REFCOUNT_P(return_value, 1);
	//!!!PZ_SET_ISREF_P(return_value);
}
/* }}} */

/* {{{ proto void ArrayIterator::rewind()
   Rewind array back to the start */
SPL_METHOD(Array, rewind)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_array_rewind(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto void ArrayIterator::seek(int $position)
   Seek to position. */
SPL_METHOD(Array, seek)
{
	zend_long opos, position;
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &position) == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	opos = position;

	if (position >= 0) { /* negative values are not supported */
		spl_array_rewind(intern TSRMLS_CC);
		result = SUCCESS;

		while (position-- > 0 && (result = spl_array_next(intern TSRMLS_CC)) == SUCCESS);

		if (result == SUCCESS && zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS) {
			return; /* ok */
		}
	}
	zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0 TSRMLS_CC, "Seek position %pd is out of range", opos);
} /* }}} */

int static spl_array_object_count_elements_helper(spl_array_object *intern, zend_long *count TSRMLS_DC) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	HashPosition pos;

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		*count = 0;
		return FAILURE;
	}

	if (Z_TYPE(intern->array) == IS_OBJECT) {
		/* We need to store the 'pos' since we'll modify it in the functions
		 * we're going to call and which do not support 'pos' as parameter. */
		pos = intern->pos;
		*count = 0;
		spl_array_rewind(intern TSRMLS_CC);
		while(intern->pos != INVALID_IDX && spl_array_next(intern TSRMLS_CC) == SUCCESS) {
			(*count)++;
		}
		spl_array_set_pos(intern, aht, pos);
		return SUCCESS;
	} else {
		*count = zend_hash_num_elements(aht);
		return SUCCESS;
	}
} /* }}} */

int spl_array_object_count_elements(zval *object, zend_long *count TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (Z_TYPE(rv) != IS_UNDEF) {
			zval_ptr_dtor(&intern->retval);
			ZVAL_ZVAL(&intern->retval, &rv, 0, 0);
			convert_to_long(&intern->retval);
			*count = (zend_long)Z_LVAL(intern->retval);
			return SUCCESS;
		}
		*count = 0;
		return FAILURE;
	}
	return spl_array_object_count_elements_helper(intern, count TSRMLS_CC);
} /* }}} */

/* {{{ proto int ArrayObject::count()
       proto int ArrayIterator::count()
   Return the number of elements in the Iterator. */
SPL_METHOD(Array, count)
{
	zend_long count;
	spl_array_object *intern = Z_SPLARRAY_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_array_object_count_elements_helper(intern, &count TSRMLS_CC);

	RETURN_LONG(count);
} /* }}} */

static void spl_array_method(INTERNAL_FUNCTION_PARAMETERS, char *fname, int fname_len, int use_arg) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(getThis());
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	zval tmp, *arg = NULL;
	zval retval;

	/* A tricky way to pass "aht" by reference, copy HashTable */
	//??? It may be not safe, if user comparison handler accesses "aht"
	ZVAL_NEW_ARR(&tmp);
	*Z_ARRVAL(tmp) = *aht;

	if (!use_arg) {
		aht->u.v.nApplyCount++;
		zend_call_method(NULL, NULL, NULL, fname, fname_len, &retval, 1, &tmp, NULL TSRMLS_CC);
		aht->u.v.nApplyCount--;
	} else if (use_arg == SPL_ARRAY_METHOD_MAY_USER_ARG) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|z", &arg) == FAILURE) {
			zval_ptr_dtor(&tmp);
			zend_throw_exception(spl_ce_BadMethodCallException, "Function expects one argument at most", 0 TSRMLS_CC);
			return;
		}
		aht->u.v.nApplyCount++;
		zend_call_method(NULL, NULL, NULL, fname, fname_len, &retval, arg? 2 : 1, &tmp, arg TSRMLS_CC);
		aht->u.v.nApplyCount--;
	} else {
		if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
			zval_ptr_dtor(&tmp);
			zend_throw_exception(spl_ce_BadMethodCallException, "Function expects exactly one argument", 0 TSRMLS_CC);
			return;
		}
		aht->u.v.nApplyCount++;
		zend_call_method(NULL, NULL, NULL, fname, fname_len, &retval, 2, &tmp, arg TSRMLS_CC);
		aht->u.v.nApplyCount--;
	}
	/* A tricky way to pass "aht" by reference, copy back and cleanup */
	if (Z_ISREF(tmp) && Z_TYPE_P(Z_REFVAL(tmp))) {
		*aht = *Z_ARRVAL_P(Z_REFVAL(tmp));
		GC_REMOVE_FROM_BUFFER(Z_ARR_P(Z_REFVAL(tmp)));
		efree(Z_ARR_P(Z_REFVAL(tmp)));
		efree(Z_REF(tmp));
	}
	if (!Z_ISUNDEF(retval)) {
		ZVAL_COPY_VALUE(return_value, &retval);
	}
} /* }}} */

#define SPL_ARRAY_METHOD(cname, fname, use_arg) \
SPL_METHOD(cname, fname) \
{ \
	spl_array_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, #fname, sizeof(#fname)-1, use_arg); \
}

/* {{{ proto int ArrayObject::asort([int $sort_flags = SORT_REGULAR ])
       proto int ArrayIterator::asort([int $sort_flags = SORT_REGULAR ])
   Sort the entries by values. */
SPL_ARRAY_METHOD(Array, asort, SPL_ARRAY_METHOD_MAY_USER_ARG) /* }}} */

/* {{{ proto int ArrayObject::ksort([int $sort_flags = SORT_REGULAR ])
       proto int ArrayIterator::ksort([int $sort_flags = SORT_REGULAR ])
   Sort the entries by key. */
SPL_ARRAY_METHOD(Array, ksort, SPL_ARRAY_METHOD_MAY_USER_ARG) /* }}} */

/* {{{ proto int ArrayObject::uasort(callback cmp_function)
       proto int ArrayIterator::uasort(callback cmp_function)
   Sort the entries by values user defined function. */
SPL_ARRAY_METHOD(Array, uasort, SPL_ARRAY_METHOD_USE_ARG) /* }}} */

/* {{{ proto int ArrayObject::uksort(callback cmp_function)
       proto int ArrayIterator::uksort(callback cmp_function)
   Sort the entries by key using user defined function. */
SPL_ARRAY_METHOD(Array, uksort, SPL_ARRAY_METHOD_USE_ARG) /* }}} */

/* {{{ proto int ArrayObject::natsort()
       proto int ArrayIterator::natsort()
   Sort the entries by values using "natural order" algorithm. */
SPL_ARRAY_METHOD(Array, natsort, SPL_ARRAY_METHOD_NO_ARG) /* }}} */

/* {{{ proto int ArrayObject::natcasesort()
       proto int ArrayIterator::natcasesort()
   Sort the entries by key using case insensitive "natural order" algorithm. */
SPL_ARRAY_METHOD(Array, natcasesort, SPL_ARRAY_METHOD_NO_ARG) /* }}} */

/* {{{ proto mixed|NULL ArrayIterator::current()
   Return current array entry */
SPL_METHOD(Array, current)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zval *entry;
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		return;
	}

	if ((entry = zend_hash_get_current_data_ex(aht, &intern->pos)) == NULL) {
		return;
	}
	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
		if (Z_TYPE_P(entry) == IS_UNDEF) {
			return;
		}
	}
	RETVAL_ZVAL(entry, 1, 0);
}
/* }}} */

/* {{{ proto mixed|NULL ArrayIterator::key()
   Return current array key */
SPL_METHOD(Array, key)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_array_iterator_key(getThis(), return_value TSRMLS_CC);
} /* }}} */

void spl_array_iterator_key(zval *object, zval *return_value TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		return;
	}

	zend_hash_get_current_key_zval_ex(aht, return_value, &intern->pos);
}
/* }}} */

/* {{{ proto void ArrayIterator::next()
   Move to next entry */
SPL_METHOD(Array, next)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		return;
	}

	spl_array_next_no_verify(intern, aht TSRMLS_CC);
}
/* }}} */

/* {{{ proto bool ArrayIterator::valid()
   Check whether array contains more entries */
SPL_METHOD(Array, valid)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	} else {
		RETURN_BOOL(zend_hash_has_more_elements_ex(aht, &intern->pos) == SUCCESS);
	}
}
/* }}} */

/* {{{ proto bool RecursiveArrayIterator::hasChildren()
   Check whether current element has children (e.g. is an array) */
SPL_METHOD(Array, hasChildren)
{
	zval *object = getThis(), *entry;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}

	if ((entry = zend_hash_get_current_data_ex(aht, &intern->pos)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_BOOL(Z_TYPE_P(entry) == IS_ARRAY || (Z_TYPE_P(entry) == IS_OBJECT && (intern->ar_flags & SPL_ARRAY_CHILD_ARRAYS_ONLY) == 0));
}
/* }}} */

/* {{{ proto object RecursiveArrayIterator::getChildren()
   Create a sub iterator for the current element (same class as $this) */
SPL_METHOD(Array, getChildren)
{
	zval *object = getThis(), *entry, flags;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht TSRMLS_CC) == FAILURE) {
		return;
	}

	if ((entry = zend_hash_get_current_data_ex(aht, &intern->pos)) == NULL) {
		return;
	}

	if (Z_TYPE_P(entry) == IS_OBJECT) {
		if ((intern->ar_flags & SPL_ARRAY_CHILD_ARRAYS_ONLY) != 0) {
			return;
		}
		if (instanceof_function(Z_OBJCE_P(entry), Z_OBJCE_P(getThis()) TSRMLS_CC)) {
			RETURN_ZVAL(entry, 1, 0);
		}
	}

	ZVAL_LONG(&flags, SPL_ARRAY_USE_OTHER | intern->ar_flags);
	spl_instantiate_arg_ex2(Z_OBJCE_P(getThis()), return_value, entry, &flags TSRMLS_CC);
}
/* }}} */

/* {{{ proto string ArrayObject::serialize()
   Serialize the object */
SPL_METHOD(Array, serialize)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	zval members, flags;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	ZVAL_LONG(&flags, (intern->ar_flags & SPL_ARRAY_CLONE_MASK));

	/* storage */
	smart_str_appendl(&buf, "x:", 2);
	//!!! php_var_serialize need to be modified
	php_var_serialize(&buf, &flags, &var_hash TSRMLS_CC);

	if (!(intern->ar_flags & SPL_ARRAY_IS_SELF)) {
		php_var_serialize(&buf, &intern->array, &var_hash TSRMLS_CC);
		smart_str_appendc(&buf, ';');
	}

	/* members */
	smart_str_appendl(&buf, "m:", 2);
	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	ZVAL_NEW_ARR(&members);
	zend_array_dup(Z_ARRVAL(members), intern->std.properties);

	php_var_serialize(&buf, &members, &var_hash TSRMLS_CC); /* finishes the string */

	zval_ptr_dtor(&members);

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.s) {
		RETURN_STR(buf.s);
	}

	RETURN_NULL();
} /* }}} */

/* {{{ proto void ArrayObject::unserialize(string serialized)
 * unserialize the object
 */
SPL_METHOD(Array, unserialize)
{
	spl_array_object *intern = Z_SPLARRAY_P(getThis());

	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval members, zflags;
	HashTable *aht;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		return;
	}

	aht = spl_array_get_hash_table(intern, 0 TSRMLS_CC);
	if (aht->u.v.nApplyCount > 0) {
		zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
		return;
	}

	/* storage */
	s = p = (const unsigned char*)buf;
	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	if (*p!= 'x' || *++p != ':') {
		goto outexcept;
	}
	++p;

	if (!php_var_unserialize(&zflags, &p, s + buf_len, &var_hash TSRMLS_CC) || Z_TYPE(zflags) != IS_LONG) {
		goto outexcept;
	}

	--p; /* for ';' */
	flags = Z_LVAL(zflags);
	/* flags needs to be verified and we also need to verify whether the next
	 * thing we get is ';'. After that we require an 'm' or somethign else
	 * where 'm' stands for members and anything else should be an array. If
	 * neither 'a' or 'm' follows we have an error. */

	if (*p != ';') {
		goto outexcept;
	}
	++p;

	if (*p!='m') {
		if (*p!='a' && *p!='O' && *p!='C' && *p!='r') {
			goto outexcept;
		}
		intern->ar_flags &= ~SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= flags & SPL_ARRAY_CLONE_MASK;
		zval_ptr_dtor(&intern->array);
		if (!php_var_unserialize(&intern->array, &p, s + buf_len, &var_hash TSRMLS_CC)) {
			goto outexcept;
		}
	}
	if (*p != ';') {
		goto outexcept;
	}
	++p;

	/* members */
	if (*p!= 'm' || *++p != ':') {
		goto outexcept;
	}
	++p;

	ZVAL_UNDEF(&members);
	if (!php_var_unserialize(&members, &p, s + buf_len, &var_hash TSRMLS_CC) || Z_TYPE(members) != IS_ARRAY) {
		zval_ptr_dtor(&members);
		goto outexcept;
	}

	/* copy members */
	object_properties_load(&intern->std, Z_ARRVAL(members) TSRMLS_CC);
	zval_ptr_dtor(&members);

	/* done reading $serialized */

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

outexcept:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Error at offset %pd of %d bytes", (zend_long)((char*)p - buf), buf_len);
	return;

} /* }}} */

/* {{{ arginfo and function tbale */
ZEND_BEGIN_ARG_INFO(arginfo_array___construct, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_offsetGet, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_append, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_seek, 0)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_exchangeArray, 0)
	ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_setFlags, 0)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_setIteratorClass, 0)
	ZEND_ARG_INFO(0, iteratorClass)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_uXsort, 0)
	ZEND_ARG_INFO(0, cmp_function)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_array_unserialize, 0)
	ZEND_ARG_INFO(0, serialized)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_array_void, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry spl_funcs_ArrayObject[] = {
	SPL_ME(Array, __construct,      arginfo_array___construct,      ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetExists,     arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetGet,        arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetSet,        arginfo_array_offsetSet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetUnset,      arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, append,           arginfo_array_append,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, getArrayCopy,     arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, count,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, getFlags,         arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, setFlags,         arginfo_array_setFlags,         ZEND_ACC_PUBLIC)
	SPL_ME(Array, asort,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, ksort,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, uasort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, uksort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, natsort,          arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, natcasesort,      arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, unserialize,      arginfo_array_unserialize,      ZEND_ACC_PUBLIC)
	SPL_ME(Array, serialize,        arginfo_array_void,             ZEND_ACC_PUBLIC)
	/* ArrayObject specific */
	SPL_ME(Array, getIterator,      arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, exchangeArray,    arginfo_array_exchangeArray,    ZEND_ACC_PUBLIC)
	SPL_ME(Array, setIteratorClass, arginfo_array_setIteratorClass, ZEND_ACC_PUBLIC)
	SPL_ME(Array, getIteratorClass, arginfo_array_void,             ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry spl_funcs_ArrayIterator[] = {
	SPL_ME(Array, __construct,      arginfo_array___construct,      ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetExists,     arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetGet,        arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetSet,        arginfo_array_offsetSet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, offsetUnset,      arginfo_array_offsetGet,        ZEND_ACC_PUBLIC)
	SPL_ME(Array, append,           arginfo_array_append,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, getArrayCopy,     arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, count,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, getFlags,         arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, setFlags,         arginfo_array_setFlags,         ZEND_ACC_PUBLIC)
	SPL_ME(Array, asort,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, ksort,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, uasort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, uksort,           arginfo_array_uXsort,           ZEND_ACC_PUBLIC)
	SPL_ME(Array, natsort,          arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, natcasesort,      arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, unserialize,      arginfo_array_unserialize,      ZEND_ACC_PUBLIC)
	SPL_ME(Array, serialize,        arginfo_array_void,             ZEND_ACC_PUBLIC)
	/* ArrayIterator specific */
	SPL_ME(Array, rewind,           arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, current,          arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, key,              arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, next,             arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, valid,            arginfo_array_void,             ZEND_ACC_PUBLIC)
	SPL_ME(Array, seek,             arginfo_array_seek,             ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry spl_funcs_RecursiveArrayIterator[] = {
	SPL_ME(Array, hasChildren,   arginfo_array_void, ZEND_ACC_PUBLIC)
	SPL_ME(Array, getChildren,   arginfo_array_void, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_array) */
PHP_MINIT_FUNCTION(spl_array)
{
	REGISTER_SPL_STD_CLASS_EX(ArrayObject, spl_array_object_new, spl_funcs_ArrayObject);
	REGISTER_SPL_IMPLEMENTS(ArrayObject, Aggregate);
	REGISTER_SPL_IMPLEMENTS(ArrayObject, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(ArrayObject, Serializable);
	REGISTER_SPL_IMPLEMENTS(ArrayObject, Countable);
	memcpy(&spl_handler_ArrayObject, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_ArrayObject.offset = XtOffsetOf(spl_array_object, std);

	spl_handler_ArrayObject.clone_obj = spl_array_object_clone;
	spl_handler_ArrayObject.read_dimension = spl_array_read_dimension;
	spl_handler_ArrayObject.write_dimension = spl_array_write_dimension;
	spl_handler_ArrayObject.unset_dimension = spl_array_unset_dimension;
	spl_handler_ArrayObject.has_dimension = spl_array_has_dimension;
	spl_handler_ArrayObject.count_elements = spl_array_object_count_elements;

	spl_handler_ArrayObject.get_properties = spl_array_get_properties;
	spl_handler_ArrayObject.get_debug_info = spl_array_get_debug_info;
	spl_handler_ArrayObject.read_property = spl_array_read_property;
	spl_handler_ArrayObject.write_property = spl_array_write_property;
	spl_handler_ArrayObject.get_property_ptr_ptr = spl_array_get_property_ptr_ptr;
	spl_handler_ArrayObject.has_property = spl_array_has_property;
	spl_handler_ArrayObject.unset_property = spl_array_unset_property;

	spl_handler_ArrayObject.compare_objects = spl_array_compare_objects;
	spl_handler_ArrayObject.dtor_obj = zend_objects_destroy_object;
	spl_handler_ArrayObject.free_obj = spl_array_object_free_storage;

	REGISTER_SPL_STD_CLASS_EX(ArrayIterator, spl_array_object_new, spl_funcs_ArrayIterator);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, Iterator);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, SeekableIterator);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, Serializable);
	REGISTER_SPL_IMPLEMENTS(ArrayIterator, Countable);
	memcpy(&spl_handler_ArrayIterator, &spl_handler_ArrayObject, sizeof(zend_object_handlers));
	spl_ce_ArrayIterator->get_iterator = spl_array_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(RecursiveArrayIterator, ArrayIterator, spl_array_object_new, spl_funcs_RecursiveArrayIterator);
	REGISTER_SPL_IMPLEMENTS(RecursiveArrayIterator, RecursiveIterator);
	spl_ce_RecursiveArrayIterator->get_iterator = spl_array_get_iterator;

	REGISTER_SPL_CLASS_CONST_LONG(ArrayObject,   "STD_PROP_LIST",    SPL_ARRAY_STD_PROP_LIST);
	REGISTER_SPL_CLASS_CONST_LONG(ArrayObject,   "ARRAY_AS_PROPS",   SPL_ARRAY_ARRAY_AS_PROPS);

	REGISTER_SPL_CLASS_CONST_LONG(ArrayIterator, "STD_PROP_LIST",    SPL_ARRAY_STD_PROP_LIST);
	REGISTER_SPL_CLASS_CONST_LONG(ArrayIterator, "ARRAY_AS_PROPS",   SPL_ARRAY_ARRAY_AS_PROPS);

	REGISTER_SPL_CLASS_CONST_LONG(RecursiveArrayIterator, "CHILD_ARRAYS_ONLY", SPL_ARRAY_CHILD_ARRAYS_ONLY);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
