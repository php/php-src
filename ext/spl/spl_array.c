/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
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
#define SPL_ARRAY_IS_SELF            0x01000000
#define SPL_ARRAY_USE_OTHER          0x02000000
#define SPL_ARRAY_INT_MASK           0xFFFF0000
#define SPL_ARRAY_CLONE_MASK         0x0100FFFF

#define SPL_ARRAY_METHOD_NO_ARG				0
#define SPL_ARRAY_METHOD_USE_ARG    		1
#define SPL_ARRAY_METHOD_MAY_USER_ARG 		2

typedef struct _spl_array_object {
	zval              array;
	uint32_t          ht_iter;
	int               ar_flags;
	unsigned char	  nApplyCount;
	zend_function     *fptr_offset_get;
	zend_function     *fptr_offset_set;
	zend_function     *fptr_offset_has;
	zend_function     *fptr_offset_del;
	zend_function     *fptr_count;
	zend_class_entry* ce_get_iterator;
	zend_object       std;
} spl_array_object;

static inline spl_array_object *spl_array_from_obj(zend_object *obj) /* {{{ */ {
	return (spl_array_object*)((char*)(obj) - XtOffsetOf(spl_array_object, std));
}
/* }}} */

#define Z_SPLARRAY_P(zv)  spl_array_from_obj(Z_OBJ_P((zv)))

static inline HashTable **spl_array_get_hash_table_ptr(spl_array_object* intern) { /* {{{ */
	//??? TODO: Delay duplication for arrays; only duplicate for write operations
	if (intern->ar_flags & SPL_ARRAY_IS_SELF) {
		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		return &intern->std.properties;
	} else if (intern->ar_flags & SPL_ARRAY_USE_OTHER) {
		spl_array_object *other = Z_SPLARRAY_P(&intern->array);
		return spl_array_get_hash_table_ptr(other);
	} else if (Z_TYPE(intern->array) == IS_ARRAY) {
		return &Z_ARRVAL(intern->array);
	} else {
		zend_object *obj = Z_OBJ(intern->array);
		if (!obj->properties) {
			rebuild_object_properties(obj);
		} else if (GC_REFCOUNT(obj->properties) > 1) {
			if (EXPECTED(!(GC_FLAGS(obj->properties) & IS_ARRAY_IMMUTABLE))) {
				GC_DELREF(obj->properties);
			}
			obj->properties = zend_array_dup(obj->properties);
		}
		return &obj->properties;
	}
}
/* }}} */

static inline HashTable *spl_array_get_hash_table(spl_array_object* intern) { /* {{{ */
	return *spl_array_get_hash_table_ptr(intern);
}
/* }}} */

static inline void spl_array_replace_hash_table(spl_array_object* intern, HashTable *ht) { /* {{{ */
	HashTable **ht_ptr = spl_array_get_hash_table_ptr(intern);
	zend_array_destroy(*ht_ptr);
	*ht_ptr = ht;
}
/* }}} */

static inline zend_bool spl_array_is_object(spl_array_object *intern) /* {{{ */
{
	while (intern->ar_flags & SPL_ARRAY_USE_OTHER) {
		intern = Z_SPLARRAY_P(&intern->array);
	}
	return (intern->ar_flags & SPL_ARRAY_IS_SELF) || Z_TYPE(intern->array) == IS_OBJECT;
}
/* }}} */

static int spl_array_skip_protected(spl_array_object *intern, HashTable *aht);

static zend_never_inline void spl_array_create_ht_iter(HashTable *ht, spl_array_object* intern) /* {{{ */
{
	intern->ht_iter = zend_hash_iterator_add(ht, zend_hash_get_current_pos(ht));
	zend_hash_internal_pointer_reset_ex(ht, &EG(ht_iterators)[intern->ht_iter].pos);
	spl_array_skip_protected(intern, ht);
}
/* }}} */

static zend_always_inline uint32_t *spl_array_get_pos_ptr(HashTable *ht, spl_array_object* intern) /* {{{ */
{
	if (UNEXPECTED(intern->ht_iter == (uint32_t)-1)) {
		spl_array_create_ht_iter(ht, intern);
	}
	return &EG(ht_iterators)[intern->ht_iter].pos;
}
/* }}} */

/* {{{ spl_array_object_free_storage */
static void spl_array_object_free_storage(zend_object *object)
{
	spl_array_object *intern = spl_array_from_obj(object);

	if (intern->ht_iter != (uint32_t) -1) {
		zend_hash_iterator_del(intern->ht_iter);
	}

	zend_object_std_dtor(&intern->std);

	zval_ptr_dtor(&intern->array);
}
/* }}} */

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref);

/* {{{ spl_array_object_new_ex */
static zend_object *spl_array_object_new_ex(zend_class_entry *class_type, zval *orig, int clone_orig)
{
	spl_array_object *intern;
	zend_class_entry *parent = class_type;
	int inherited = 0;

	intern = zend_object_alloc(sizeof(spl_array_object), parent);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->ar_flags = 0;
	intern->ce_get_iterator = spl_ce_ArrayIterator;
	if (orig) {
		spl_array_object *other = Z_SPLARRAY_P(orig);

		intern->ar_flags &= ~ SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= (other->ar_flags & SPL_ARRAY_CLONE_MASK);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (clone_orig) {
			if (other->ar_flags & SPL_ARRAY_IS_SELF) {
				ZVAL_UNDEF(&intern->array);
			} else if (Z_OBJ_HT_P(orig) == &spl_handler_ArrayObject) {
				ZVAL_ARR(&intern->array,
					zend_array_dup(spl_array_get_hash_table(other)));
			} else {
				ZEND_ASSERT(Z_OBJ_HT_P(orig) == &spl_handler_ArrayIterator);
				ZVAL_COPY(&intern->array, orig);
				intern->ar_flags |= SPL_ARRAY_USE_OTHER;
			}
		} else {
			ZVAL_COPY(&intern->array, orig);
			intern->ar_flags |= SPL_ARRAY_USE_OTHER;
		}
	} else {
		array_init(&intern->array);
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
		php_error_docref(NULL, E_COMPILE_ERROR, "Internal compiler error, Class is not child of ArrayObject or ArrayIterator");
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
		if (!class_type->iterator_funcs_ptr->zf_current) {
			class_type->iterator_funcs_ptr->zf_rewind = zend_hash_str_find_ptr(&class_type->function_table, "rewind", sizeof("rewind") - 1);
			class_type->iterator_funcs_ptr->zf_valid = zend_hash_str_find_ptr(&class_type->function_table, "valid", sizeof("valid") - 1);
			class_type->iterator_funcs_ptr->zf_key = zend_hash_str_find_ptr(&class_type->function_table, "key", sizeof("key") - 1);
			class_type->iterator_funcs_ptr->zf_current = zend_hash_str_find_ptr(&class_type->function_table, "current", sizeof("current") - 1);
			class_type->iterator_funcs_ptr->zf_next = zend_hash_str_find_ptr(&class_type->function_table, "next", sizeof("next") - 1);
		}
		if (inherited) {
			if (class_type->iterator_funcs_ptr->zf_rewind->common.scope  != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_REWIND;
			if (class_type->iterator_funcs_ptr->zf_valid->common.scope   != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_VALID;
			if (class_type->iterator_funcs_ptr->zf_key->common.scope     != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_KEY;
			if (class_type->iterator_funcs_ptr->zf_current->common.scope != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_CURRENT;
			if (class_type->iterator_funcs_ptr->zf_next->common.scope    != parent) intern->ar_flags |= SPL_ARRAY_OVERLOADED_NEXT;
		}
	}

	intern->ht_iter = (uint32_t)-1;
	return &intern->std;
}
/* }}} */

/* {{{ spl_array_object_new */
static zend_object *spl_array_object_new(zend_class_entry *class_type)
{
	return spl_array_object_new_ex(class_type, NULL, 0);
}
/* }}} */

/* {{{ spl_array_object_clone */
static zend_object *spl_array_object_clone(zval *zobject)
{
	zend_object *old_object;
	zend_object *new_object;

	old_object = Z_OBJ_P(zobject);
	new_object = spl_array_object_new_ex(old_object->ce, zobject, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

static zval *spl_array_get_dimension_ptr(int check_inherited, spl_array_object *intern, zval *offset, int type) /* {{{ */
{
	zval *retval;
	zend_long index;
	zend_string *offset_key;
	HashTable *ht = spl_array_get_hash_table(intern);

	if (!offset || Z_ISUNDEF_P(offset) || !ht) {
		return &EG(uninitialized_zval);
	}

	if ((type == BP_VAR_W || type == BP_VAR_RW) && intern->nApplyCount > 0) {
		zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
		return &EG(error_zval);
	}

try_again:
	switch (Z_TYPE_P(offset)) {
	case IS_NULL:
	   offset_key = ZSTR_EMPTY_ALLOC();
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
							zend_error(E_NOTICE, "Undefined index: %s", ZSTR_VAL(offset_key));
						case BP_VAR_UNSET:
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval);
							break;
						case BP_VAR_RW:
							zend_error(E_NOTICE,"Undefined index: %s", ZSTR_VAL(offset_key));
						case BP_VAR_W: {
							ZVAL_NULL(retval);
						}
					}
				}
			}
		} else {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_NOTICE, "Undefined index: %s", ZSTR_VAL(offset_key));
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE,"Undefined index: %s", ZSTR_VAL(offset_key));
				case BP_VAR_W: {
				    zval value;
					ZVAL_NULL(&value);
				    retval = zend_symtable_update(ht, offset_key, &value);
				}
			}
		}
		return retval;
	case IS_RESOURCE:
		zend_error(E_NOTICE, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_P(offset)->handle, Z_RES_P(offset)->handle);
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
					zend_error(E_NOTICE, "Undefined offset: " ZEND_LONG_FMT, index);
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_NOTICE, "Undefined offset: " ZEND_LONG_FMT, index);
				case BP_VAR_W: {
				    zval value;
					ZVAL_UNDEF(&value);
					retval = zend_hash_index_update(ht, index, &value);
			   }
			}
		}
		return retval;
	case IS_REFERENCE:
		ZVAL_DEREF(offset);
		goto try_again;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return (type == BP_VAR_W || type == BP_VAR_RW) ?
			&EG(error_zval) : &EG(uninitialized_zval);
	}
} /* }}} */

static int spl_array_has_dimension(zval *object, zval *offset, int check_empty);

static zval *spl_array_read_dimension_ex(int check_inherited, zval *object, zval *offset, int type, zval *rv) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zval *ret;

	if (check_inherited &&
			(intern->fptr_offset_get || (type == BP_VAR_IS && intern->fptr_offset_has))) {
		if (type == BP_VAR_IS) {
			if (!spl_array_has_dimension(object, offset, 0)) {
				return &EG(uninitialized_zval);
			}
		}

		if (intern->fptr_offset_get) {
			zval tmp;
			if (!offset) {
				ZVAL_UNDEF(&tmp);
				offset = &tmp;
			} else {
				SEPARATE_ARG_IF_REF(offset);
			}
			zend_call_method_with_1_params(object, Z_OBJCE_P(object), &intern->fptr_offset_get, "offsetGet", rv, offset);
			zval_ptr_dtor(offset);

			if (!Z_ISUNDEF_P(rv)) {
				return rv;
			}
			return &EG(uninitialized_zval);
		}
	}

	ret = spl_array_get_dimension_ptr(check_inherited, intern, offset, type);

	/* When in a write context,
	 * ZE has to be fooled into thinking this is in a reference set
	 * by separating (if necessary) and returning as IS_REFERENCE (with refcount == 1)
	 */

	if ((type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) &&
	    !Z_ISREF_P(ret) &&
	    EXPECTED(ret != &EG(uninitialized_zval))) {
		ZVAL_NEW_REF(ret, ret);
	}

	return ret;
} /* }}} */

static zval *spl_array_read_dimension(zval *object, zval *offset, int type, zval *rv) /* {{{ */
{
	return spl_array_read_dimension_ex(1, object, offset, type, rv);
} /* }}} */

static void spl_array_write_dimension_ex(int check_inherited, zval *object, zval *offset, zval *value) /* {{{ */
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

	if (intern->nApplyCount > 0) {
		zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
		return;
	}

	Z_TRY_ADDREF_P(value);
	if (!offset) {
		ht = spl_array_get_hash_table(intern);
		zend_hash_next_index_insert(ht, value);
		return;
	}

try_again:
	switch (Z_TYPE_P(offset)) {
		case IS_STRING:
			ht = spl_array_get_hash_table(intern);
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
			ht = spl_array_get_hash_table(intern);
			zend_hash_index_update(ht, index, value);
			return;
		case IS_NULL:
			ht = spl_array_get_hash_table(intern);
			zend_hash_next_index_insert(ht, value);
			return;
		case IS_REFERENCE:
			ZVAL_DEREF(offset);
			goto try_again;
		default:
			zend_error(E_WARNING, "Illegal offset type");
			zval_ptr_dtor(value);
			return;
	}
} /* }}} */

static void spl_array_write_dimension(zval *object, zval *offset, zval *value) /* {{{ */
{
	spl_array_write_dimension_ex(1, object, offset, value);
} /* }}} */

static void spl_array_unset_dimension_ex(int check_inherited, zval *object, zval *offset) /* {{{ */
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

	if (intern->nApplyCount > 0) {
		zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
		return;
	}

try_again:
	switch (Z_TYPE_P(offset)) {
	case IS_STRING:
		ht = spl_array_get_hash_table(intern);
		if (ht == &EG(symbol_table)) {
			if (zend_delete_global_variable(Z_STR_P(offset))) {
				zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
			}
		} else {
			zval *data = zend_symtable_find(ht, Z_STR_P(offset));

			if (data) {
				if (Z_TYPE_P(data) == IS_INDIRECT) {
					data = Z_INDIRECT_P(data);
					if (Z_TYPE_P(data) == IS_UNDEF) {
						zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
					} else {
						zval_ptr_dtor(data);
						ZVAL_UNDEF(data);
						HT_FLAGS(ht) |= HASH_FLAG_HAS_EMPTY_IND;
						zend_hash_move_forward_ex(ht, spl_array_get_pos_ptr(ht, intern));
						if (spl_array_is_object(intern)) {
							spl_array_skip_protected(intern, ht);
						}
					}
				} else if (zend_symtable_del(ht, Z_STR_P(offset)) == FAILURE) {
					zend_error(E_NOTICE,"Undefined index: %s", Z_STRVAL_P(offset));
				}
			} else {
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
		ht = spl_array_get_hash_table(intern);
		if (zend_hash_index_del(ht, index) == FAILURE) {
			zend_error(E_NOTICE,"Undefined offset: " ZEND_LONG_FMT, index);
		}
		break;
	case IS_REFERENCE:
		ZVAL_DEREF(offset);
		goto try_again;
	default:
		zend_error(E_WARNING, "Illegal offset type");
		return;
	}
} /* }}} */

static void spl_array_unset_dimension(zval *object, zval *offset) /* {{{ */
{
	spl_array_unset_dimension_ex(1, object, offset);
} /* }}} */

static int spl_array_has_dimension_ex(int check_inherited, zval *object, zval *offset, int check_empty) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_long index;
	zval rv, *value = NULL, *tmp;

	if (check_inherited && intern->fptr_offset_has) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, Z_OBJCE_P(object), &intern->fptr_offset_has, "offsetExists", &rv, offset);
		zval_ptr_dtor(offset);

		if (zend_is_true(&rv)) {
			zval_ptr_dtor(&rv);
			if (check_empty != 1) {
				return 1;
			} else if (intern->fptr_offset_get) {
				value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R, &rv);
			}
		} else {
			zval_ptr_dtor(&rv);
			return 0;
		}
	}

	if (!value) {
		HashTable *ht = spl_array_get_hash_table(intern);

try_again:
		switch (Z_TYPE_P(offset)) {
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
			case IS_REFERENCE:
				ZVAL_DEREF(offset);
				goto try_again;
			default:
				zend_error(E_WARNING, "Illegal offset type");
				return 0;
		}

		if (check_empty && check_inherited && intern->fptr_offset_get) {
			value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R, &rv);
		} else {
			value = tmp;
		}
	}

	{
		zend_bool result = check_empty ? zend_is_true(value) : Z_TYPE_P(value) != IS_NULL;
		if (value == &rv) {
			zval_ptr_dtor(&rv);
		}
		return result;
	}
} /* }}} */

static int spl_array_has_dimension(zval *object, zval *offset, int check_empty) /* {{{ */
{
	return spl_array_has_dimension_ex(1, object, offset, check_empty);
} /* }}} */

/* {{{ spl_array_object_verify_pos_ex */
static inline int spl_array_object_verify_pos_ex(spl_array_object *object, HashTable *ht, const char *msg_prefix)
{
	if (!ht) {
		php_error_docref(NULL, E_NOTICE, "%sArray was modified outside object and is no longer an array", msg_prefix);
		return FAILURE;
	}

	return SUCCESS;
} /* }}} */

/* {{{ spl_array_object_verify_pos */
static inline int spl_array_object_verify_pos(spl_array_object *object, HashTable *ht)
{
	return spl_array_object_verify_pos_ex(object, ht, "");
} /* }}} */

/* {{{ proto bool ArrayObject::offsetExists(mixed $index)
       proto bool ArrayIterator::offsetExists(mixed $index)
   Returns whether the requested $index exists. */
SPL_METHOD(Array, offsetExists)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &index) == FAILURE) {
		return;
	}
	RETURN_BOOL(spl_array_has_dimension_ex(0, getThis(), index, 2));
} /* }}} */

/* {{{ proto mixed ArrayObject::offsetGet(mixed $index)
       proto mixed ArrayIterator::offsetGet(mixed $index)
   Returns the value at the specified $index. */
SPL_METHOD(Array, offsetGet)
{
	zval *value, *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &index) == FAILURE) {
		return;
	}
	value = spl_array_read_dimension_ex(0, getThis(), index, BP_VAR_R, return_value);
	if (value != return_value) {
		ZVAL_COPY_DEREF(return_value, value);
	}
} /* }}} */

/* {{{ proto void ArrayObject::offsetSet(mixed $index, mixed $newval)
       proto void ArrayIterator::offsetSet(mixed $index, mixed $newval)
   Sets the value at the specified $index to $newval. */
SPL_METHOD(Array, offsetSet)
{
	zval *index, *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &index, &value) == FAILURE) {
		return;
	}
	spl_array_write_dimension_ex(0, getThis(), index, value);
} /* }}} */

void spl_array_iterator_append(zval *object, zval *append_value) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (!aht) {
		php_error_docref(NULL, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	if (spl_array_is_object(intern)) {
		zend_throw_error(NULL, "Cannot append properties to objects, use %s::offsetSet() instead", ZSTR_VAL(Z_OBJCE_P(object)->name));
		return;
	}

	spl_array_write_dimension(object, NULL, append_value);
} /* }}} */

/* {{{ proto void ArrayObject::append(mixed $newval)
       proto void ArrayIterator::append(mixed $newval)
   Appends the value (cannot be called for objects). */
SPL_METHOD(Array, append)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		return;
	}
	spl_array_iterator_append(getThis(), value);
} /* }}} */

/* {{{ proto void ArrayObject::offsetUnset(mixed $index)
       proto void ArrayIterator::offsetUnset(mixed $index)
   Unsets the value at the specified $index. */
SPL_METHOD(Array, offsetUnset)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &index) == FAILURE) {
		return;
	}
	spl_array_unset_dimension_ex(0, getThis(), index);
} /* }}} */

/* {{{ proto array ArrayObject::getArrayCopy()
      proto array ArrayIterator::getArrayCopy()
   Return a copy of the contained array */
SPL_METHOD(Array, getArrayCopy)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);

	RETURN_ARR(zend_array_dup(spl_array_get_hash_table(intern)));
} /* }}} */

static HashTable *spl_array_get_properties(zval *object) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (intern->ar_flags & SPL_ARRAY_STD_PROP_LIST) {
		if (!intern->std.properties) {
			rebuild_object_properties(&intern->std);
		}
		return intern->std.properties;
	}

	return spl_array_get_hash_table(intern);
} /* }}} */

static HashTable* spl_array_get_debug_info(zval *obj, int *is_temp) /* {{{ */
{
	zval *storage;
	zend_string *zname;
	zend_class_entry *base;
	spl_array_object *intern = Z_SPLARRAY_P(obj);

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (intern->ar_flags & SPL_ARRAY_IS_SELF) {
		*is_temp = 0;
		return intern->std.properties;
	} else {
		HashTable *debug_info;
		*is_temp = 1;

		debug_info = zend_new_array(zend_hash_num_elements(intern->std.properties) + 1);
		zend_hash_copy(debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref);

		storage = &intern->array;
		Z_TRY_ADDREF_P(storage);

		base = Z_OBJ_HT_P(obj) == &spl_handler_ArrayIterator
			? spl_ce_ArrayIterator : spl_ce_ArrayObject;
		zname = spl_gen_private_prop_name(base, "storage", sizeof("storage")-1);
		zend_symtable_update(debug_info, zname, storage);
		zend_string_release_ex(zname, 0);

		return debug_info;
	}
}
/* }}} */

static HashTable *spl_array_get_gc(zval *obj, zval **gc_data, int *gc_data_count) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(obj);
	*gc_data = &intern->array;
	*gc_data_count = 1;
	return zend_std_get_properties(obj);
}
/* }}} */

static zval *spl_array_read_property(zval *object, zval *member, int type, void **cache_slot, zval *rv) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, member, ZEND_PROPERTY_EXISTS, NULL)) {
		return spl_array_read_dimension(object, member, type, rv);
	}
	return zend_std_read_property(object, member, type, cache_slot, rv);
} /* }}} */

static void spl_array_write_property(zval *object, zval *member, zval *value, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !zend_std_has_property(object, member, ZEND_PROPERTY_EXISTS, NULL)) {
		spl_array_write_dimension(object, member, value);
		return;
	}
	zend_std_write_property(object, member, value, cache_slot);
} /* }}} */

static zval *spl_array_get_property_ptr_ptr(zval *object, zval *member, int type, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, member, ZEND_PROPERTY_EXISTS, NULL)) {
		/* If object has offsetGet() overridden, then fallback to read_property,
		 * which will call offsetGet(). */
		if (intern->fptr_offset_get) {
			return NULL;
		}
		return spl_array_get_dimension_ptr(1, intern, member, type);
	}
	return zend_std_get_property_ptr_ptr(object, member, type, cache_slot);
} /* }}} */

static int spl_array_has_property(zval *object, zval *member, int has_set_exists, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, member, ZEND_PROPERTY_EXISTS, NULL)) {
		return spl_array_has_dimension(object, member, has_set_exists);
	}
	return zend_std_has_property(object, member, has_set_exists, cache_slot);
} /* }}} */

static void spl_array_unset_property(zval *object, zval *member, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, member, ZEND_PROPERTY_EXISTS, NULL)) {
		spl_array_unset_dimension(object, member);
		return;
	}
	zend_std_unset_property(object, member, cache_slot);
} /* }}} */

static int spl_array_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	HashTable			*ht1,
						*ht2;
	spl_array_object	*intern1,
						*intern2;
	int					result	= 0;

	intern1	= Z_SPLARRAY_P(o1);
	intern2	= Z_SPLARRAY_P(o2);
	ht1		= spl_array_get_hash_table(intern1);
	ht2		= spl_array_get_hash_table(intern2);

	result = zend_compare_symbol_tables(ht1, ht2);
	/* if we just compared std.properties, don't do it again */
	if (result == 0 &&
			!(ht1 == intern1->std.properties && ht2 == intern2->std.properties)) {
		result = zend_std_compare_objects(o1, o2);
	}
	return result;
} /* }}} */

static int spl_array_skip_protected(spl_array_object *intern, HashTable *aht) /* {{{ */
{
	zend_string *string_key;
	zend_ulong num_key;
	zval *data;

	if (spl_array_is_object(intern)) {
		uint32_t *pos_ptr = spl_array_get_pos_ptr(aht, intern);

		do {
			if (zend_hash_get_current_key_ex(aht, &string_key, &num_key, pos_ptr) == HASH_KEY_IS_STRING) {
				data = zend_hash_get_current_data_ex(aht, pos_ptr);
				if (data && Z_TYPE_P(data) == IS_INDIRECT &&
				    Z_TYPE_P(data = Z_INDIRECT_P(data)) == IS_UNDEF) {
					/* skip */
				} else if (!ZSTR_LEN(string_key) || ZSTR_VAL(string_key)[0]) {
					return SUCCESS;
				}
			} else {
				return SUCCESS;
			}
			if (zend_hash_has_more_elements_ex(aht, pos_ptr) != SUCCESS) {
				return FAILURE;
			}
			zend_hash_move_forward_ex(aht, pos_ptr);
		} while (1);
	}
	return FAILURE;
} /* }}} */

static int spl_array_next_ex(spl_array_object *intern, HashTable *aht) /* {{{ */
{
	uint32_t *pos_ptr = spl_array_get_pos_ptr(aht, intern);

	zend_hash_move_forward_ex(aht, pos_ptr);
	if (spl_array_is_object(intern)) {
		return spl_array_skip_protected(intern, aht);
	} else {
		return zend_hash_has_more_elements_ex(aht, pos_ptr);
	}
} /* }}} */

static int spl_array_next(spl_array_object *intern) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern);

	return spl_array_next_ex(intern, aht);

} /* }}} */

static void spl_array_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	zend_user_it_invalidate_current(iter);
	zval_ptr_dtor(&iter->data);
}
/* }}} */

static int spl_array_it_valid(zend_object_iterator *iter) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_VALID) {
		return zend_user_it_valid(iter);
	} else {
		if (spl_array_object_verify_pos_ex(object, aht, "ArrayIterator::valid(): ") == FAILURE) {
			return FAILURE;
		}

		return zend_hash_has_more_elements_ex(aht, spl_array_get_pos_ptr(aht, object));
	}
}
/* }}} */

static zval *spl_array_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT) {
		return zend_user_it_get_current_data(iter);
	} else {
		zval *data = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, object));
		if (data && Z_TYPE_P(data) == IS_INDIRECT) {
			data = Z_INDIRECT_P(data);
		}
		return data;
	}
}
/* }}} */

static void spl_array_it_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_KEY) {
		zend_user_it_get_current_key(iter, key);
	} else {
		if (spl_array_object_verify_pos_ex(object, aht, "ArrayIterator::current(): ") == FAILURE) {
			ZVAL_NULL(key);
		} else {
			zend_hash_get_current_key_zval_ex(aht, key, spl_array_get_pos_ptr(aht, object));
		}
	}
}
/* }}} */

static void spl_array_it_move_forward(zend_object_iterator *iter) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_NEXT) {
		zend_user_it_move_forward(iter);
	} else {
		zend_user_it_invalidate_current(iter);
		if (!aht) {
			php_error_docref(NULL, E_NOTICE, "ArrayIterator::current(): Array was modified outside object and is no longer an array");
			return;
		}

		spl_array_next_ex(object, aht);
	}
}
/* }}} */

static void spl_array_rewind(spl_array_object *intern) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern);

	if (!aht) {
		php_error_docref(NULL, E_NOTICE, "ArrayIterator::rewind(): Array was modified outside object and is no longer an array");
		return;
	}

	if (intern->ht_iter == (uint32_t)-1) {
		spl_array_get_pos_ptr(aht, intern);
	} else {
		zend_hash_internal_pointer_reset_ex(aht, spl_array_get_pos_ptr(aht, intern));
		spl_array_skip_protected(intern, aht);
	}
}
/* }}} */

static void spl_array_it_rewind(zend_object_iterator *iter) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);

	if (object->ar_flags & SPL_ARRAY_OVERLOADED_REWIND) {
		zend_user_it_rewind(iter);
	} else {
		zend_user_it_invalidate_current(iter);
		spl_array_rewind(object);
	}
}
/* }}} */

/* {{{ spl_array_set_array */
static void spl_array_set_array(zval *object, spl_array_object *intern, zval *array, zend_long ar_flags, int just_array) {
	if (Z_TYPE_P(array) != IS_OBJECT && Z_TYPE_P(array) != IS_ARRAY) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Passed variable is not an array or object", 0);
		return;
	}

	if (Z_TYPE_P(array) == IS_ARRAY) {
		zval_ptr_dtor(&intern->array);
		if (Z_REFCOUNT_P(array) == 1) {
			ZVAL_COPY(&intern->array, array);
		} else {
			//??? TODO: try to avoid array duplication
			ZVAL_ARR(&intern->array, zend_array_dup(Z_ARR_P(array)));
		}
	} else {
		if (Z_OBJ_HT_P(array) == &spl_handler_ArrayObject || Z_OBJ_HT_P(array) == &spl_handler_ArrayIterator) {
			zval_ptr_dtor(&intern->array);
			if (just_array)	{
				spl_array_object *other = Z_SPLARRAY_P(array);
				ar_flags = other->ar_flags & ~SPL_ARRAY_INT_MASK;
			}
			if (Z_OBJ_P(object) == Z_OBJ_P(array)) {
				ar_flags |= SPL_ARRAY_IS_SELF;
				ZVAL_UNDEF(&intern->array);
			} else {
				ar_flags |= SPL_ARRAY_USE_OTHER;
				ZVAL_COPY(&intern->array, array);
			}
		} else {
			zend_object_get_properties_t handler = Z_OBJ_HANDLER_P(array, get_properties);
			if (handler != zend_std_get_properties) {
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
					"Overloaded object of type %s is not compatible with %s",
					ZSTR_VAL(Z_OBJCE_P(array)->name), ZSTR_VAL(intern->std.ce->name));
				return;
			}
			zval_ptr_dtor(&intern->array);
			ZVAL_COPY(&intern->array, array);
		}
	}

	intern->ar_flags &= ~SPL_ARRAY_IS_SELF & ~SPL_ARRAY_USE_OTHER;
	intern->ar_flags |= ar_flags;
	intern->ht_iter = (uint32_t)-1;
}
/* }}} */

/* iterator handler table */
static const zend_object_iterator_funcs spl_array_it_funcs = {
	spl_array_it_dtor,
	spl_array_it_valid,
	spl_array_it_get_current_data,
	spl_array_it_get_current_key,
	spl_array_it_move_forward,
	spl_array_it_rewind,
	NULL
};

zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	zend_user_iterator *iterator;
	spl_array_object *array_object = Z_SPLARRAY_P(object);

	if (by_ref && (array_object->ar_flags & SPL_ARRAY_OVERLOADED_CURRENT)) {
		zend_throw_exception(spl_ce_RuntimeException, "An iterator cannot be used with foreach by reference", 0);
		return NULL;
	}

	iterator = emalloc(sizeof(zend_user_iterator));

	zend_iterator_init(&iterator->it);

	ZVAL_COPY(&iterator->it.data, object);
	iterator->it.funcs = &spl_array_it_funcs;
	iterator->ce = ce;
	ZVAL_UNDEF(&iterator->value);

	return &iterator->it;
}
/* }}} */

/* {{{ proto ArrayObject::__construct([array|object ar = array() [, int flags = 0 [, string iterator_class = "ArrayIterator"]]])
   Constructs a new array object from an array or object. */
SPL_METHOD(Array, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval *array;
	zend_long ar_flags = 0;
	zend_class_entry *ce_get_iterator = spl_ce_Iterator;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z|lC", &array, &ar_flags, &ce_get_iterator) == FAILURE) {
		return;
	}

	intern = Z_SPLARRAY_P(object);

	if (ZEND_NUM_ARGS() > 2) {
		intern->ce_get_iterator = ce_get_iterator;
	}

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	spl_array_set_array(object, intern, array, ar_flags, ZEND_NUM_ARGS() == 1);
}
 /* }}} */

/* {{{ proto ArrayIterator::__construct([array|object ar = array() [, int flags = 0]])
   Constructs a new array iterator from an array or object. */
SPL_METHOD(ArrayIterator, __construct)
{
	zval *object = getThis();
	spl_array_object *intern;
	zval *array;
	zend_long ar_flags = 0;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z|l", &array, &ar_flags) == FAILURE) {
		return;
	}

	intern = Z_SPLARRAY_P(object);

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	spl_array_set_array(object, intern, array, ar_flags, ZEND_NUM_ARGS() == 1);
}
 /* }}} */

/* {{{ proto void ArrayObject::setIteratorClass(string iterator_class)
   Set the class used in getIterator. */
SPL_METHOD(Array, setIteratorClass)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_class_entry * ce_get_iterator = spl_ce_Iterator;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_CLASS(ce_get_iterator)
	ZEND_PARSE_PARAMETERS_END();

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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &ar_flags) == FAILURE) {
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &array) == FAILURE) {
		return;
	}

	if (intern->nApplyCount > 0) {
		zend_error(E_WARNING, "Modification of ArrayObject during sorting is prohibited");
		return;
	}

	RETVAL_ARR(zend_array_dup(spl_array_get_hash_table(intern)));
	spl_array_set_array(object, intern, array, 0L, 1);
}
/* }}} */

/* {{{ proto ArrayIterator ArrayObject::getIterator()
   Create a new iterator from a ArrayObject instance */
SPL_METHOD(Array, getIterator)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	ZVAL_OBJ(return_value, spl_array_object_new_ex(intern->ce_get_iterator, object, 0));
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

	spl_array_rewind(intern);
}
/* }}} */

/* {{{ proto void ArrayIterator::seek(int $position)
   Seek to position. */
SPL_METHOD(Array, seek)
{
	zend_long opos, position;
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &position) == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	opos = position;

	if (position >= 0) { /* negative values are not supported */
		spl_array_rewind(intern);
		result = SUCCESS;

		while (position-- > 0 && (result = spl_array_next(intern)) == SUCCESS);

		if (result == SUCCESS && zend_hash_has_more_elements_ex(aht, spl_array_get_pos_ptr(aht, intern)) == SUCCESS) {
			return; /* ok */
		}
	}
	zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Seek position " ZEND_LONG_FMT " is out of range", opos);
} /* }}} */

static int spl_array_object_count_elements_helper(spl_array_object *intern, zend_long *count) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern);
	HashPosition pos, *pos_ptr;

	if (!aht) {
		php_error_docref(NULL, E_NOTICE, "Array was modified outside object and is no longer an array");
		*count = 0;
		return FAILURE;
	}

	if (spl_array_is_object(intern)) {
		/* We need to store the 'pos' since we'll modify it in the functions
		 * we're going to call and which do not support 'pos' as parameter. */
		pos_ptr = spl_array_get_pos_ptr(aht, intern);
		pos = *pos_ptr;
		*count = 0;
		spl_array_rewind(intern);
		while (*pos_ptr < aht->nNumUsed && spl_array_next(intern) == SUCCESS) {
			(*count)++;
		}
		*pos_ptr = pos;
		return SUCCESS;
	} else {
		*count = zend_hash_num_elements(aht);
		return SUCCESS;
	}
} /* }}} */

int spl_array_object_count_elements(zval *object, zend_long *count) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (Z_TYPE(rv) != IS_UNDEF) {
			*count = zval_get_long(&rv);
			zval_ptr_dtor(&rv);
			return SUCCESS;
		}
		*count = 0;
		return FAILURE;
	}
	return spl_array_object_count_elements_helper(intern, count);
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

	spl_array_object_count_elements_helper(intern, &count);

	RETURN_LONG(count);
} /* }}} */

static void spl_array_method(INTERNAL_FUNCTION_PARAMETERS, char *fname, int fname_len, int use_arg) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(getThis());
	HashTable *aht = spl_array_get_hash_table(intern);
	zval function_name, params[2], *arg = NULL;

	ZVAL_STRINGL(&function_name, fname, fname_len);

	ZVAL_NEW_EMPTY_REF(&params[0]);
	ZVAL_ARR(Z_REFVAL(params[0]), aht);
	GC_ADDREF(aht);

	if (!use_arg) {
		intern->nApplyCount++;
		call_user_function(EG(function_table), NULL, &function_name, return_value, 1, params);
		intern->nApplyCount--;
	} else if (use_arg == SPL_ARRAY_METHOD_MAY_USER_ARG) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "|z", &arg) == FAILURE) {
			zend_throw_exception(spl_ce_BadMethodCallException, "Function expects one argument at most", 0);
			goto exit;
		}
		if (arg) {
			ZVAL_COPY_VALUE(&params[1], arg);
		}
		intern->nApplyCount++;
		call_user_function(EG(function_table), NULL, &function_name, return_value, arg ? 2 : 1, params);
		intern->nApplyCount--;
	} else {
		if (ZEND_NUM_ARGS() != 1 || zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
			zend_throw_exception(spl_ce_BadMethodCallException, "Function expects exactly one argument", 0);
			goto exit;
		}
		ZVAL_COPY_VALUE(&params[1], arg);
		intern->nApplyCount++;
		call_user_function(EG(function_table), NULL, &function_name, return_value, 2, params);
		intern->nApplyCount--;
	}

exit:
	{
		HashTable *new_ht = Z_ARRVAL_P(Z_REFVAL(params[0]));
		if (aht != new_ht) {
			spl_array_replace_hash_table(intern, new_ht);
		} else {
			GC_DELREF(aht);
		}
		efree(Z_REF(params[0]));
		zend_string_free(Z_STR(function_name));
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
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht) == FAILURE) {
		return;
	}

	if ((entry = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, intern))) == NULL) {
		return;
	}
	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
		if (Z_TYPE_P(entry) == IS_UNDEF) {
			return;
		}
	}
	ZVAL_COPY_DEREF(return_value, entry);
}
/* }}} */

/* {{{ proto mixed|NULL ArrayIterator::key()
   Return current array key */
SPL_METHOD(Array, key)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_array_iterator_key(getThis(), return_value);
} /* }}} */

void spl_array_iterator_key(zval *object, zval *return_value) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (spl_array_object_verify_pos(intern, aht) == FAILURE) {
		return;
	}

	zend_hash_get_current_key_zval_ex(aht, return_value, spl_array_get_pos_ptr(aht, intern));
}
/* }}} */

/* {{{ proto void ArrayIterator::next()
   Move to next entry */
SPL_METHOD(Array, next)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht) == FAILURE) {
		return;
	}

	spl_array_next_ex(intern, aht);
}
/* }}} */

/* {{{ proto bool ArrayIterator::valid()
   Check whether array contains more entries */
SPL_METHOD(Array, valid)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht) == FAILURE) {
		RETURN_FALSE;
	} else {
		RETURN_BOOL(zend_hash_has_more_elements_ex(aht, spl_array_get_pos_ptr(aht, intern)) == SUCCESS);
	}
}
/* }}} */

/* {{{ proto bool RecursiveArrayIterator::hasChildren()
   Check whether current element has children (e.g. is an array) */
SPL_METHOD(Array, hasChildren)
{
	zval *object = getThis(), *entry;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht) == FAILURE) {
		RETURN_FALSE;
	}

	if ((entry = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, intern))) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
	}

	ZVAL_DEREF(entry);
	RETURN_BOOL(Z_TYPE_P(entry) == IS_ARRAY || (Z_TYPE_P(entry) == IS_OBJECT && (intern->ar_flags & SPL_ARRAY_CHILD_ARRAYS_ONLY) == 0));
}
/* }}} */

/* {{{ proto object RecursiveArrayIterator::getChildren()
   Create a sub iterator for the current element (same class as $this) */
SPL_METHOD(Array, getChildren)
{
	zval *object = getThis(), *entry, flags;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (spl_array_object_verify_pos(intern, aht) == FAILURE) {
		return;
	}

	if ((entry = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, intern))) == NULL) {
		return;
	}

	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
	}

	ZVAL_DEREF(entry);
	if (Z_TYPE_P(entry) == IS_OBJECT) {
		if ((intern->ar_flags & SPL_ARRAY_CHILD_ARRAYS_ONLY) != 0) {
			return;
		}
		if (instanceof_function(Z_OBJCE_P(entry), Z_OBJCE_P(getThis()))) {
			ZVAL_OBJ(return_value, Z_OBJ_P(entry));
			Z_ADDREF_P(return_value);
			return;
		}
	}

	ZVAL_LONG(&flags, intern->ar_flags);
	spl_instantiate_arg_ex2(Z_OBJCE_P(getThis()), return_value, entry, &flags);
}
/* }}} */

/* {{{ proto string ArrayObject::serialize()
   Serialize the object */
SPL_METHOD(Array, serialize)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);
	zval members, flags;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!aht) {
		php_error_docref(NULL, E_NOTICE, "Array was modified outside object and is no longer an array");
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);

	ZVAL_LONG(&flags, (intern->ar_flags & SPL_ARRAY_CLONE_MASK));

	/* storage */
	smart_str_appendl(&buf, "x:", 2);
	php_var_serialize(&buf, &flags, &var_hash);

	if (!(intern->ar_flags & SPL_ARRAY_IS_SELF)) {
		php_var_serialize(&buf, &intern->array, &var_hash);
		smart_str_appendc(&buf, ';');
	}

	/* members */
	smart_str_appendl(&buf, "m:", 2);
	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	ZVAL_ARR(&members, intern->std.properties);

	php_var_serialize(&buf, &members, &var_hash); /* finishes the string */

	/* done */
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (buf.s) {
		RETURN_NEW_STR(buf.s);
	}

	RETURN_NULL();
} /* }}} */

/* {{{ proto void ArrayObject::unserialize(string serialized)
 * unserialize the object
 */
SPL_METHOD(Array, unserialize)
{
	zval *object = getThis();
	spl_array_object *intern = Z_SPLARRAY_P(object);

	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval *members, *zflags, *array;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		return;
	}

	if (buf_len == 0) {
		return;
	}

	if (intern->nApplyCount > 0) {
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

	zflags = var_tmp_var(&var_hash);
	if (!php_var_unserialize(zflags, &p, s + buf_len, &var_hash) || Z_TYPE_P(zflags) != IS_LONG) {
		goto outexcept;
	}

	--p; /* for ';' */
	flags = Z_LVAL_P(zflags);
	/* flags needs to be verified and we also need to verify whether the next
	 * thing we get is ';'. After that we require an 'm' or something else
	 * where 'm' stands for members and anything else should be an array. If
	 * neither 'a' or 'm' follows we have an error. */

	if (*p != ';') {
		goto outexcept;
	}
	++p;

	if (flags & SPL_ARRAY_IS_SELF) {
		/* If IS_SELF is used, the flags are not followed by an array/object */
		intern->ar_flags &= ~SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= flags & SPL_ARRAY_CLONE_MASK;
		zval_ptr_dtor(&intern->array);
		ZVAL_UNDEF(&intern->array);
	} else {
		if (*p!='a' && *p!='O' && *p!='C' && *p!='r') {
			goto outexcept;
		}

		array = var_tmp_var(&var_hash);
		if (!php_var_unserialize(array, &p, s + buf_len, &var_hash)
				|| (Z_TYPE_P(array) != IS_ARRAY && Z_TYPE_P(array) != IS_OBJECT)) {
			goto outexcept;
		}

		intern->ar_flags &= ~SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= flags & SPL_ARRAY_CLONE_MASK;

		if (Z_TYPE_P(array) == IS_ARRAY) {
			zval_ptr_dtor(&intern->array);
			ZVAL_COPY_VALUE(&intern->array, array);
			ZVAL_NULL(array);
			SEPARATE_ARRAY(&intern->array);
		} else {
			spl_array_set_array(object, intern, array, 0L, 1);
		}

		if (*p != ';') {
			goto outexcept;
		}
        ++p;
	}

	/* members */
	if (*p!= 'm' || *++p != ':') {
		goto outexcept;
	}
	++p;

	members = var_tmp_var(&var_hash);
	if (!php_var_unserialize(members, &p, s + buf_len, &var_hash) || Z_TYPE_P(members) != IS_ARRAY) {
		goto outexcept;
	}

	/* copy members */
	object_properties_load(&intern->std, Z_ARRVAL_P(members));

	/* done reading $serialized */
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	return;

outexcept:
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0, "Error at offset " ZEND_LONG_FMT " of %zd bytes", (zend_long)((char*)p - buf), buf_len);
	return;

} /* }}} */

/* {{{ arginfo and function table */
ZEND_BEGIN_ARG_INFO_EX(arginfo_array___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, iterator_class)
ZEND_END_ARG_INFO()

/* ArrayIterator::__construct and ArrayObject::__construct have different signatures */
ZEND_BEGIN_ARG_INFO_EX(arginfo_array_iterator___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, array)
	ZEND_ARG_INFO(0, ar_flags)
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
	SPL_ME(ArrayIterator, __construct, arginfo_array_iterator___construct,      ZEND_ACC_PUBLIC)
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
	memcpy(&spl_handler_ArrayObject, &std_object_handlers, sizeof(zend_object_handlers));

	spl_handler_ArrayObject.offset = XtOffsetOf(spl_array_object, std);

	spl_handler_ArrayObject.clone_obj = spl_array_object_clone;
	spl_handler_ArrayObject.read_dimension = spl_array_read_dimension;
	spl_handler_ArrayObject.write_dimension = spl_array_write_dimension;
	spl_handler_ArrayObject.unset_dimension = spl_array_unset_dimension;
	spl_handler_ArrayObject.has_dimension = spl_array_has_dimension;
	spl_handler_ArrayObject.count_elements = spl_array_object_count_elements;

	spl_handler_ArrayObject.get_properties = spl_array_get_properties;
	spl_handler_ArrayObject.get_debug_info = spl_array_get_debug_info;
	spl_handler_ArrayObject.get_gc = spl_array_get_gc;
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

	REGISTER_SPL_CLASS_CONST_LONG(ArrayObject,   "STD_PROP_LIST",    SPL_ARRAY_STD_PROP_LIST);
	REGISTER_SPL_CLASS_CONST_LONG(ArrayObject,   "ARRAY_AS_PROPS",   SPL_ARRAY_ARRAY_AS_PROPS);

	REGISTER_SPL_CLASS_CONST_LONG(ArrayIterator, "STD_PROP_LIST",    SPL_ARRAY_STD_PROP_LIST);
	REGISTER_SPL_CLASS_CONST_LONG(ArrayIterator, "ARRAY_AS_PROPS",   SPL_ARRAY_ARRAY_AS_PROPS);

	REGISTER_SPL_SUB_CLASS_EX(RecursiveArrayIterator, ArrayIterator, spl_array_object_new, spl_funcs_RecursiveArrayIterator);
	REGISTER_SPL_IMPLEMENTS(RecursiveArrayIterator, RecursiveIterator);
	spl_ce_RecursiveArrayIterator->get_iterator = spl_array_get_iterator;

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
