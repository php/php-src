/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
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
#include "spl_iterators.h"
#include "spl_array.h"
#include "spl_array_arginfo.h"
#include "spl_exceptions.h"

/* Defined later in the file */
zend_object_handlers spl_handler_ArrayIterator;
PHPAPI zend_class_entry  *spl_ce_ArrayIterator;
PHPAPI zend_class_entry  *spl_ce_RecursiveArrayIterator;

/* ArrayObject class */
zend_object_handlers spl_handler_ArrayObject;
PHPAPI zend_class_entry  *spl_ce_ArrayObject;

typedef struct _spl_array_object {
	zval              array;
	uint32_t          ht_iter;
	int               ar_flags;
	unsigned char	  nApplyCount;
	bool			  is_child;
	Bucket			  *bucket;
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

static inline bool spl_array_is_object(spl_array_object *intern) /* {{{ */
{
	while (intern->ar_flags & SPL_ARRAY_USE_OTHER) {
		intern = Z_SPLARRAY_P(&intern->array);
	}
	return (intern->ar_flags & SPL_ARRAY_IS_SELF) || Z_TYPE(intern->array) == IS_OBJECT;
}
/* }}} */

static zend_result spl_array_skip_protected(spl_array_object *intern, HashTable *aht);

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

/* {{{ spl_array_object_new_ex */
static zend_object *spl_array_object_new_ex(zend_class_entry *class_type, zend_object *orig, int clone_orig)
{
	spl_array_object *intern;
	zend_class_entry *parent = class_type;
	int inherited = 0;

	intern = zend_object_alloc(sizeof(spl_array_object), parent);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	intern->ar_flags = 0;
	intern->is_child = false;
	intern->bucket = NULL;
	intern->ce_get_iterator = spl_ce_ArrayIterator;
	if (orig) {
		spl_array_object *other = spl_array_from_obj(orig);

		intern->ar_flags &= ~ SPL_ARRAY_CLONE_MASK;
		intern->ar_flags |= (other->ar_flags & SPL_ARRAY_CLONE_MASK);
		intern->ce_get_iterator = other->ce_get_iterator;
		if (clone_orig) {
			if (other->ar_flags & SPL_ARRAY_IS_SELF) {
				ZVAL_UNDEF(&intern->array);
			} else if (orig->handlers == &spl_handler_ArrayObject) {
				ZVAL_ARR(&intern->array,
					zend_array_dup(spl_array_get_hash_table(other)));
			} else {
				ZEND_ASSERT(orig->handlers == &spl_handler_ArrayIterator);
				ZVAL_OBJ_COPY(&intern->array, orig);
				intern->ar_flags |= SPL_ARRAY_USE_OTHER;
			}
		} else {
			ZVAL_OBJ_COPY(&intern->array, orig);
			intern->ar_flags |= SPL_ARRAY_USE_OTHER;
		}
	} else {
		array_init(&intern->array);
	}

	while (parent) {
		if (parent == spl_ce_ArrayIterator || parent == spl_ce_RecursiveArrayIterator || parent == spl_ce_ArrayObject) {
			break;
		}
		parent = parent->parent;
		inherited = 1;
	}

	ZEND_ASSERT(parent);

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
		/* Find count() method */
		intern->fptr_count = zend_hash_find_ptr(&class_type->function_table, ZSTR_KNOWN(ZEND_STR_COUNT));
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
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
static zend_object *spl_array_object_clone(zend_object *old_object)
{
	zend_object *new_object;

	new_object = spl_array_object_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}
/* }}} */

typedef struct {
	zend_string *key;
	zend_ulong h;
	bool release_key;
} spl_hash_key;

static void spl_hash_key_release(spl_hash_key *key) {
	if (key->release_key) {
		zend_string_release_ex(key->key, 0);
	}
}

/* This function does not throw any exceptions for illegal offsets, calls to
 * zend_illegal_container_offset(); need to be made if the return value is FAILURE */
static zend_result get_hash_key(spl_hash_key *key, spl_array_object *intern, zval *offset)
{
	key->release_key = false;
try_again:
	switch (Z_TYPE_P(offset)) {
	case IS_NULL:
		key->key = ZSTR_EMPTY_ALLOC();
		return SUCCESS;
	case IS_STRING:
		key->key = Z_STR_P(offset);
		if (ZEND_HANDLE_NUMERIC(key->key, key->h)) {
			key->key = NULL;
			break;
		}
		return SUCCESS;
	case IS_RESOURCE:
		zend_use_resource_as_offset(offset);
		key->key = NULL;
		key->h = Z_RES_P(offset)->handle;
		break;
	case IS_DOUBLE:
		key->key = NULL;
		key->h = zend_dval_to_lval_safe(Z_DVAL_P(offset));
		break;
	case IS_FALSE:
		key->key = NULL;
		key->h = 0;
		break;
	case IS_TRUE:
		key->key = NULL;
		key->h = 1;
		break;
	case IS_LONG:
		key->key = NULL;
		key->h = Z_LVAL_P(offset);
		break;
	case IS_REFERENCE:
		ZVAL_DEREF(offset);
		goto try_again;
	default:
		return FAILURE;
	}

	if (spl_array_is_object(intern)) {
		key->key = zend_long_to_str(key->h);
		key->release_key = true;
	}
	return SUCCESS;
}

static zval *spl_array_get_dimension_ptr(bool check_inherited, spl_array_object *intern, const zend_string *ce_name,
	zval *offset, int type) /* {{{ */
{
	zval *retval;
	spl_hash_key key;
	HashTable *ht = spl_array_get_hash_table(intern);

	if (!offset || Z_ISUNDEF_P(offset) || !ht) {
		return &EG(uninitialized_zval);
	}

	if ((type == BP_VAR_W || type == BP_VAR_RW) && intern->nApplyCount > 0) {
		zend_throw_error(NULL, "Modification of ArrayObject during sorting is prohibited");
		return &EG(error_zval);
	}

	if (get_hash_key(&key, intern, offset) == FAILURE) {
		zend_illegal_container_offset(ce_name, offset, type);
		return (type == BP_VAR_W || type == BP_VAR_RW) ?
			&EG(error_zval) : &EG(uninitialized_zval);
	}

	if (key.key) {
		retval = zend_hash_find(ht, key.key);
		if (retval) {
			if (Z_TYPE_P(retval) == IS_INDIRECT) {
				retval = Z_INDIRECT_P(retval);
				if (Z_TYPE_P(retval) == IS_UNDEF) {
					switch (type) {
						case BP_VAR_R:
							zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(key.key));
							ZEND_FALLTHROUGH;
						case BP_VAR_UNSET:
						case BP_VAR_IS:
							retval = &EG(uninitialized_zval);
							break;
						case BP_VAR_RW:
							zend_error(E_WARNING,"Undefined array key \"%s\"", ZSTR_VAL(key.key));
							ZEND_FALLTHROUGH;
						case BP_VAR_W: {
							ZVAL_NULL(retval);
						}
					}
				}
			}
		} else {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_WARNING, "Undefined array key \"%s\"", ZSTR_VAL(key.key));
					ZEND_FALLTHROUGH;
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_WARNING,"Undefined array key \"%s\"", ZSTR_VAL(key.key));
					ZEND_FALLTHROUGH;
				case BP_VAR_W: {
				    zval value;
					ZVAL_NULL(&value);
				    retval = zend_hash_update(ht, key.key, &value);
				}
			}
		}
		spl_hash_key_release(&key);
	} else {
		if ((retval = zend_hash_index_find(ht, key.h)) == NULL) {
			switch (type) {
				case BP_VAR_R:
					zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, key.h);
					ZEND_FALLTHROUGH;
				case BP_VAR_UNSET:
				case BP_VAR_IS:
					retval = &EG(uninitialized_zval);
					break;
				case BP_VAR_RW:
					zend_error(E_WARNING, "Undefined array key " ZEND_LONG_FMT, key.h);
					ZEND_FALLTHROUGH;
				case BP_VAR_W: {
				    zval value;
					ZVAL_NULL(&value);
					retval = zend_hash_index_update(ht, key.h, &value);
			   }
			}
		}
	}
	return retval;
} /* }}} */

static int spl_array_has_dimension(zend_object *object, zval *offset, int check_empty);

static zval *spl_array_read_dimension_ex(int check_inherited, zend_object *object, zval *offset, int type, zval *rv) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);
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
			}
			zend_call_method_with_1_params(object, object->ce, &intern->fptr_offset_get, "offsetGet", rv, offset);

			if (!Z_ISUNDEF_P(rv)) {
				return rv;
			}
			return &EG(uninitialized_zval);
		}
	}

	ret = spl_array_get_dimension_ptr(check_inherited, intern, object->ce->name, offset, type);

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

static zval *spl_array_read_dimension(zend_object *object, zval *offset, int type, zval *rv) /* {{{ */
{
	return spl_array_read_dimension_ex(1, object, offset, type, rv);
} /* }}} */

/*
 * The assertion(HT_ASSERT_RC1(ht)) failed because the refcount was increased manually when intern->is_child is true.
 * We have to set the refcount to 1 to make assertion success and restore the refcount to the original value after
 * modifying the array when intern->is_child is true.
 */
static uint32_t spl_array_set_refcount(bool is_child, HashTable *ht, uint32_t refcount) /* {{{ */
{
	uint32_t old_refcount = 0;
	if (is_child) {
		old_refcount = GC_REFCOUNT(ht);
		GC_SET_REFCOUNT(ht, refcount);
	}

	return old_refcount;
} /* }}} */

static void spl_array_write_dimension_ex(int check_inherited, zend_object *object, zval *offset, zval *value) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);
	HashTable *ht;
	spl_hash_key key;

	/* Cannot append if backing value is an object */
	if (offset == NULL && spl_array_is_object(intern)) {
		zend_throw_error(NULL, "Cannot append properties to objects, use %s::offsetSet() instead", ZSTR_VAL(object->ce->name));
		return;
	}

	if (check_inherited && intern->fptr_offset_set) {
		zval tmp;

		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		}
		zend_call_method_with_2_params(object, object->ce, &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		return;
	}

	if (intern->nApplyCount > 0) {
		zend_throw_error(NULL, "Modification of ArrayObject during sorting is prohibited");
		return;
	}

	Z_TRY_ADDREF_P(value);

	uint32_t refcount = 0;
	if (!offset || Z_TYPE_P(offset) == IS_NULL) {
		ht = spl_array_get_hash_table(intern);
		refcount = spl_array_set_refcount(intern->is_child, ht, 1);
		zend_hash_next_index_insert(ht, value);

		if (refcount) {
			spl_array_set_refcount(intern->is_child, ht, refcount);
		}
		return;
	}

	if (get_hash_key(&key, intern, offset) == FAILURE) {
		zend_illegal_container_offset(object->ce->name, offset, BP_VAR_W);
		zval_ptr_dtor(value);
		return;
	}

	ht = spl_array_get_hash_table(intern);
	refcount = spl_array_set_refcount(intern->is_child, ht, 1);
	if (key.key) {
		zend_hash_update_ind(ht, key.key, value);
		spl_hash_key_release(&key);
	} else {
		zend_hash_index_update(ht, key.h, value);
	}

	if (refcount) {
		spl_array_set_refcount(intern->is_child, ht, refcount);
	}
} /* }}} */

static void spl_array_write_dimension(zend_object *object, zval *offset, zval *value) /* {{{ */
{
	spl_array_write_dimension_ex(1, object, offset, value);
} /* }}} */

static void spl_array_unset_dimension_ex(int check_inherited, zend_object *object, zval *offset) /* {{{ */
{
	HashTable *ht;
	spl_array_object *intern = spl_array_from_obj(object);
	spl_hash_key key;

	if (check_inherited && intern->fptr_offset_del) {
		zend_call_method_with_1_params(object, object->ce, &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		return;
	}

	if (intern->nApplyCount > 0) {
		zend_throw_error(NULL, "Modification of ArrayObject during sorting is prohibited");
		return;
	}

	if (get_hash_key(&key, intern, offset) == FAILURE) {
		zend_illegal_container_offset(object->ce->name, offset, BP_VAR_UNSET);
		return;
	}

	ht = spl_array_get_hash_table(intern);
	uint32_t refcount = spl_array_set_refcount(intern->is_child, ht, 1);

	if (key.key) {
		zval *data = zend_hash_find(ht, key.key);
		if (data) {
			if (Z_TYPE_P(data) == IS_INDIRECT) {
				data = Z_INDIRECT_P(data);
				if (Z_TYPE_P(data) != IS_UNDEF) {
					zval_ptr_dtor(data);
					ZVAL_UNDEF(data);
					HT_FLAGS(ht) |= HASH_FLAG_HAS_EMPTY_IND;
					zend_hash_move_forward_ex(ht, spl_array_get_pos_ptr(ht, intern));
					if (spl_array_is_object(intern)) {
						spl_array_skip_protected(intern, ht);
					}
				}
			} else {
				zend_hash_del(ht, key.key);
			}
		}
		spl_hash_key_release(&key);
	} else {
		zend_hash_index_del(ht, key.h);
	}

	if (refcount) {
		spl_array_set_refcount(intern->is_child, ht, refcount);
	}
} /* }}} */

static void spl_array_unset_dimension(zend_object *object, zval *offset) /* {{{ */
{
	spl_array_unset_dimension_ex(1, object, offset);
} /* }}} */

/* check_empty can take value 0, 1, or 2
 * 0/1 are used as normal boolean, but 2 is used for the case when this function is called from
 * the offsetExists() method, in which case it needs to report the offset exist even if the value is null */
static bool spl_array_has_dimension_ex(bool check_inherited, zend_object *object, zval *offset, int check_empty) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);
	zval rv, *value = NULL, *tmp;

	if (check_inherited && intern->fptr_offset_has) {
		zend_call_method_with_1_params(object, object->ce, &intern->fptr_offset_has, "offsetExists", &rv, offset);

		if (!zend_is_true(&rv)) {
			zval_ptr_dtor(&rv);
			return 0;
		}
		zval_ptr_dtor(&rv);

		/* For isset calls we don't need to check the value, so return early */
		if (!check_empty) {
			return 1;
		} else if (intern->fptr_offset_get) {
			value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R, &rv);
		}
	}

	if (!value) {
		HashTable *ht = spl_array_get_hash_table(intern);
		spl_hash_key key;

		if (get_hash_key(&key, intern, offset) == FAILURE) {
			zend_illegal_container_offset(object->ce->name, offset, BP_VAR_IS);
			return 0;
		}

		if (key.key) {
			tmp = zend_hash_find(ht, key.key);
			spl_hash_key_release(&key);
		} else {
			tmp = zend_hash_index_find(ht, key.h);
		}

		if (!tmp) {
			return 0;
		}

		/* check_empty is only equal to 2 if it is called from offsetExists on this class,
		 * where it needs to report an offset exists even if the value is null */
		if (check_empty == 2) {
			return 1;
		}

		if (check_empty && check_inherited && intern->fptr_offset_get) {
			value = spl_array_read_dimension_ex(1, object, offset, BP_VAR_R, &rv);
		} else {
			value = tmp;
		}
	}

	if (value == &rv) {
		zval_ptr_dtor(&rv);
	}

	/* empty() check the value is not falsy, isset() only check it is not null */
	return check_empty ? zend_is_true(value) : Z_TYPE_P(value) != IS_NULL;
} /* }}} */

static int spl_array_has_dimension(zend_object *object, zval *offset, int check_empty) /* {{{ */
{
	return spl_array_has_dimension_ex(/* check_inherited */ true, object, offset, check_empty);
} /* }}} */

/* {{{ Returns whether the requested $index exists. */
PHP_METHOD(ArrayObject, offsetExists)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &index) == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_BOOL(spl_array_has_dimension_ex(/* check_inherited */ false, Z_OBJ_P(ZEND_THIS), index, 2));
} /* }}} */

/* {{{ Returns the value at the specified $index. */
PHP_METHOD(ArrayObject, offsetGet)
{
	zval *value, *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &index) == FAILURE) {
		RETURN_THROWS();
	}
	value = spl_array_read_dimension_ex(0, Z_OBJ_P(ZEND_THIS), index, BP_VAR_R, return_value);
	if (value != return_value) {
		RETURN_COPY_DEREF(value);
	}
} /* }}} */

/* {{{ Sets the value at the specified $index to $newval. */
PHP_METHOD(ArrayObject, offsetSet)
{
	zval *index, *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &index, &value) == FAILURE) {
		RETURN_THROWS();
	}
	spl_array_write_dimension_ex(0, Z_OBJ_P(ZEND_THIS), index, value);
} /* }}} */

void spl_array_iterator_append(zval *object, zval *append_value) /* {{{ */
{
	spl_array_write_dimension(Z_OBJ_P(object), NULL, append_value);
} /* }}} */

/* {{{ Appends the value (cannot be called for objects). */
PHP_METHOD(ArrayObject, append)
{
	zval *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
		RETURN_THROWS();
	}
	spl_array_iterator_append(ZEND_THIS, value);
} /* }}} */

/* {{{ Unsets the value at the specified $index. */
PHP_METHOD(ArrayObject, offsetUnset)
{
	zval *index;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &index) == FAILURE) {
		RETURN_THROWS();
	}
	spl_array_unset_dimension_ex(0, Z_OBJ_P(ZEND_THIS), index);
} /* }}} */

/* {{{ Return a copy of the contained array */
PHP_METHOD(ArrayObject, getArrayCopy)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_ARR(zend_array_dup(spl_array_get_hash_table(intern)));
} /* }}} */

static HashTable *spl_array_get_properties_for(zend_object *object, zend_prop_purpose purpose) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);
	HashTable *ht;
	bool dup;

	if (intern->ar_flags & SPL_ARRAY_STD_PROP_LIST) {
		return zend_std_get_properties_for(object, purpose);
	}

	/* We are supposed to be the only owner of the internal hashtable.
	 * The "dup" flag decides whether this is a "long-term" use where
	 * we need to duplicate, or a "temporary" one, where we can expect
	 * that no operations on the ArrayObject will be performed in the
	 * meantime. */
	switch (purpose) {
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
			dup = 1;
			break;
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
			dup = 0;
			break;
		default:
			return zend_std_get_properties_for(object, purpose);
	}

	ht = spl_array_get_hash_table(intern);
	if (dup) {
		ht = zend_array_dup(ht);
	} else {
		GC_ADDREF(ht);
	}
	return ht;
} /* }}} */

static inline HashTable* spl_array_get_debug_info(zend_object *obj) /* {{{ */
{
	zval *storage;
	zend_string *zname;
	zend_class_entry *base;
	spl_array_object *intern = spl_array_from_obj(obj);

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	if (intern->ar_flags & SPL_ARRAY_IS_SELF) {
		return zend_array_dup(intern->std.properties);
	} else {
		HashTable *debug_info;

		debug_info = zend_new_array(zend_hash_num_elements(intern->std.properties) + 1);
		zend_hash_copy(debug_info, intern->std.properties, (copy_ctor_func_t) zval_add_ref);

		storage = &intern->array;
		Z_TRY_ADDREF_P(storage);

		base = obj->handlers == &spl_handler_ArrayIterator
			? spl_ce_ArrayIterator : spl_ce_ArrayObject;
		zname = spl_gen_private_prop_name(base, "storage", sizeof("storage")-1);
		zend_symtable_update(debug_info, zname, storage);
		zend_string_release_ex(zname, 0);

		return debug_info;
	}
}
/* }}} */

static HashTable *spl_array_get_gc(zend_object *obj, zval **gc_data, int *gc_data_count) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(obj);
	*gc_data = &intern->array;
	*gc_data_count = 1;
	return zend_std_get_properties(obj);
}
/* }}} */

static zval *spl_array_read_property(zend_object *object, zend_string *name, int type, void **cache_slot, zval *rv) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)) {
		zval member;
		ZVAL_STR(&member, name);
		return spl_array_read_dimension(object, &member, type, rv);
	}
	return zend_std_read_property(object, name, type, cache_slot, rv);
} /* }}} */

static zval *spl_array_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
	&& !zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)) {
		zval member;
		ZVAL_STR(&member, name);
		spl_array_write_dimension(object, &member, value);
		return value;
	}
	return zend_std_write_property(object, name, value, cache_slot);
} /* }}} */

static zval *spl_array_get_property_ptr_ptr(zend_object *object, zend_string *name, int type, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)) {
		/* If object has offsetGet() overridden, then fallback to read_property,
		 * which will call offsetGet(). */
		zval member;
		if (intern->fptr_offset_get) {
			return NULL;
		}
		ZVAL_STR(&member, name);
		return spl_array_get_dimension_ptr(1, intern, object->ce->name, &member, type);
	}
	return zend_std_get_property_ptr_ptr(object, name, type, cache_slot);
} /* }}} */

static int spl_array_has_property(zend_object *object, zend_string *name, int has_set_exists, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)) {
		zval member;
		ZVAL_STR(&member, name);
		return spl_array_has_dimension(object, &member, has_set_exists);
	}
	return zend_std_has_property(object, name, has_set_exists, cache_slot);
} /* }}} */

static void spl_array_unset_property(zend_object *object, zend_string *name, void **cache_slot) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);

	if ((intern->ar_flags & SPL_ARRAY_ARRAY_AS_PROPS) != 0
		&& !zend_std_has_property(object, name, ZEND_PROPERTY_EXISTS, NULL)) {
		zval member;
		ZVAL_STR(&member, name);
		spl_array_unset_dimension(object, &member);
		return;
	}
	zend_std_unset_property(object, name, cache_slot);
} /* }}} */

static int spl_array_compare_objects(zval *o1, zval *o2) /* {{{ */
{
	HashTable			*ht1,
						*ht2;
	spl_array_object	*intern1,
						*intern2;
	int					result	= 0;

	ZEND_COMPARE_OBJECTS_FALLBACK(o1, o2);

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

static zend_result spl_array_skip_protected(spl_array_object *intern, HashTable *aht) /* {{{ */
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

/* {{{ spl_array_set_array */
static void spl_array_set_array(zval *object, spl_array_object *intern, zval *array, zend_long ar_flags, bool just_array) {
	/* Handled by ZPP prior to this, or for __unserialize() before passing to here */
	ZEND_ASSERT(Z_TYPE_P(array) == IS_ARRAY || Z_TYPE_P(array) == IS_OBJECT);
	if (Z_TYPE_P(array) == IS_ARRAY) {
		zval_ptr_dtor(&intern->array);
		if (Z_REFCOUNT_P(array) == 1) {
			ZVAL_COPY(&intern->array, array);
		} else {
			//??? TODO: try to avoid array duplication
			ZVAL_ARR(&intern->array, zend_array_dup(Z_ARR_P(array)));

			if (intern->is_child) {
				Z_TRY_DELREF(intern->bucket->val);
				/*
				 * replace bucket->val with copied array, so the changes between
				 * parent and child object can affect each other.
				 */
				ZVAL_COPY(&intern->bucket->val, &intern->array);
			}
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
	if (intern->ht_iter != (uint32_t)-1) {
		zend_hash_iterator_del(intern->ht_iter);
		intern->ht_iter = (uint32_t)-1;
	}
}
/* }}} */

/* {{{ Constructs a new array object from an array or object. */
PHP_METHOD(ArrayObject, __construct)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern;
	zval *array;
	zend_long ar_flags = 0;
	zend_class_entry *ce_get_iterator = spl_ce_ArrayIterator;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|AlC", &array, &ar_flags, &ce_get_iterator) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLARRAY_P(object);

	if (ZEND_NUM_ARGS() > 2) {
		intern->ce_get_iterator = ce_get_iterator;
	}

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	spl_array_set_array(object, intern, array, ar_flags, ZEND_NUM_ARGS() == 1);
}
/* }}} */

/* {{{ Set the class used in getIterator. */
PHP_METHOD(ArrayObject, setIteratorClass)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_class_entry *ce_get_iterator = spl_ce_ArrayIterator;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_CLASS(ce_get_iterator)
	ZEND_PARSE_PARAMETERS_END();

	intern->ce_get_iterator = ce_get_iterator;
}
/* }}} */

/* {{{ Get the class used in getIterator. */
PHP_METHOD(ArrayObject, getIteratorClass)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_string_addref(intern->ce_get_iterator->name);
	RETURN_STR(intern->ce_get_iterator->name);
}
/* }}} */

/* {{{ Get flags */
PHP_METHOD(ArrayObject, getFlags)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->ar_flags & ~SPL_ARRAY_INT_MASK);
}
/* }}} */

/* {{{ Set flags */
PHP_METHOD(ArrayObject, setFlags)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zend_long ar_flags = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &ar_flags) == FAILURE) {
		RETURN_THROWS();
	}

	intern->ar_flags = (intern->ar_flags & SPL_ARRAY_INT_MASK) | (ar_flags & ~SPL_ARRAY_INT_MASK);
}
/* }}} */

/* {{{ Replace the referenced array or object with a new one and return the old one (right now copy - to be changed) */
PHP_METHOD(ArrayObject, exchangeArray)
{
	zval *object = ZEND_THIS, *array;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "A", &array) == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->nApplyCount > 0) {
		zend_throw_error(NULL, "Modification of ArrayObject during sorting is prohibited");
		RETURN_THROWS();
	}

	RETVAL_ARR(zend_array_dup(spl_array_get_hash_table(intern)));
	spl_array_set_array(object, intern, array, 0L, 1);
}
/* }}} */

/* {{{ Create a new iterator from a ArrayObject instance */
PHP_METHOD(ArrayObject, getIterator)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_OBJ(spl_array_object_new_ex(intern->ce_get_iterator, Z_OBJ_P(object), 0));
}
/* }}} */

static zend_long spl_array_object_count_elements_helper(spl_array_object *intern) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern);
	if (spl_array_is_object(intern)) {
		zend_long count = 0;
		zend_string *key;
		zval *val;
		/* Count public/dynamic properties */
		ZEND_HASH_FOREACH_STR_KEY_VAL(aht, key, val) {
			if (Z_TYPE_P(val) == IS_INDIRECT) {
				if (Z_TYPE_P(Z_INDIRECT_P(val)) == IS_UNDEF) continue;
				if (key && ZSTR_VAL(key)[0] == '\0') continue;
			}
			count++;
		} ZEND_HASH_FOREACH_END();
		return count;
	} else {
		return zend_hash_num_elements(aht);
	}
} /* }}} */

static zend_result spl_array_object_count_elements(zend_object *object, zend_long *count) /* {{{ */
{
	spl_array_object *intern = spl_array_from_obj(object);

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
	*count = spl_array_object_count_elements_helper(intern);
	return SUCCESS;
} /* }}} */

/* {{{ Return the number of elements in the Iterator. */
PHP_METHOD(ArrayObject, count)
{
	spl_array_object *intern = Z_SPLARRAY_P(ZEND_THIS);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(spl_array_object_count_elements_helper(intern));
} /* }}} */

static void spl_array_method(INTERNAL_FUNCTION_PARAMETERS, char *fname, size_t fname_len, int use_arg) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(ZEND_THIS);
	HashTable **ht_ptr = spl_array_get_hash_table_ptr(intern);
	HashTable *aht = *ht_ptr;
	zval function_name, params[2], *arg = NULL;

	ZVAL_STRINGL(&function_name, fname, fname_len);

	ZVAL_NEW_EMPTY_REF(&params[0]);
	ZVAL_ARR(Z_REFVAL(params[0]), aht);
	GC_ADDREF(aht);

	if (!use_arg) {
		if (zend_parse_parameters_none() == FAILURE) {
			goto exit;
		}

		intern->nApplyCount++;
		call_user_function(EG(function_table), NULL, &function_name, return_value, 1, params);
		intern->nApplyCount--;
	} else if (use_arg == SPL_ARRAY_METHOD_SORT_FLAGS_ARG) {
		zend_long sort_flags = 0;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &sort_flags) == FAILURE) {
			goto exit;
		}
		ZVAL_LONG(&params[1], sort_flags);
		intern->nApplyCount++;
		call_user_function(EG(function_table), NULL, &function_name, return_value, 2, params);
		intern->nApplyCount--;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &arg) == FAILURE) {
			goto exit;
		}
		ZVAL_COPY_VALUE(&params[1], arg);
		intern->nApplyCount++;
		call_user_function(EG(function_table), NULL, &function_name, return_value, 2, params);
		intern->nApplyCount--;
	}

exit:
	{
		zval *ht_zv = Z_REFVAL(params[0]);
		zend_array_release(*ht_ptr);
		SEPARATE_ARRAY(ht_zv);
		*ht_ptr = Z_ARRVAL_P(ht_zv);
		ZVAL_NULL(ht_zv);
		zval_ptr_dtor(&params[0]);
		zend_string_free(Z_STR(function_name));
	}
} /* }}} */

#define SPL_ARRAY_METHOD(cname, fname, use_arg) \
PHP_METHOD(cname, fname) \
{ \
	spl_array_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, #fname, sizeof(#fname)-1, use_arg); \
}

/* {{{ Sort the entries by values. */
SPL_ARRAY_METHOD(ArrayObject, asort, SPL_ARRAY_METHOD_SORT_FLAGS_ARG) /* }}} */

/* {{{ Sort the entries by key. */
SPL_ARRAY_METHOD(ArrayObject, ksort, SPL_ARRAY_METHOD_SORT_FLAGS_ARG) /* }}} */

/* {{{ Sort the entries by values user defined function. */
SPL_ARRAY_METHOD(ArrayObject, uasort, SPL_ARRAY_METHOD_CALLBACK_ARG) /* }}} */

/* {{{ Sort the entries by key using user defined function. */
SPL_ARRAY_METHOD(ArrayObject, uksort, SPL_ARRAY_METHOD_CALLBACK_ARG) /* }}} */

/* {{{ Sort the entries by values using "natural order" algorithm. */
SPL_ARRAY_METHOD(ArrayObject, natsort, SPL_ARRAY_METHOD_NO_ARG) /* }}} */

/* {{{ Sort the entries by key using case insensitive "natural order" algorithm. */
SPL_ARRAY_METHOD(ArrayObject, natcasesort, SPL_ARRAY_METHOD_NO_ARG) /* }}} */

/* {{{ Serialize the object */
PHP_METHOD(ArrayObject, serialize)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zval members, flags;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

	RETURN_STR(smart_str_extract(&buf));
} /* }}} */

/* {{{ unserialize the object */
PHP_METHOD(ArrayObject, unserialize)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	char *buf;
	size_t buf_len;
	const unsigned char *p, *s;
	php_unserialize_data_t var_hash;
	zval *members, *zflags, *array;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &buf, &buf_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (buf_len == 0) {
		return;
	}

	if (intern->nApplyCount > 0) {
		zend_throw_error(NULL, "Modification of ArrayObject during sorting is prohibited");
		RETURN_THROWS();
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
	RETURN_THROWS();

} /* }}} */

/* {{{ */
PHP_METHOD(ArrayObject, __serialize)
{
	spl_array_object *intern = Z_SPLARRAY_P(ZEND_THIS);
	zval tmp;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	/* flags */
	ZVAL_LONG(&tmp, (intern->ar_flags & SPL_ARRAY_CLONE_MASK));
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* storage */
	if (intern->ar_flags & SPL_ARRAY_IS_SELF) {
		ZVAL_NULL(&tmp);
	} else {
		ZVAL_COPY(&tmp, &intern->array);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* members */
	ZVAL_ARR(&tmp, zend_proptable_to_symtable(
		zend_std_get_properties(&intern->std), /* always_duplicate */ 1));
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* iterator class */
	if (intern->ce_get_iterator == spl_ce_ArrayIterator) {
		ZVAL_NULL(&tmp);
	} else {
		ZVAL_STR_COPY(&tmp, intern->ce_get_iterator->name);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
}
/* }}} */


/* {{{ */
PHP_METHOD(ArrayObject, __unserialize)
{
	spl_array_object *intern = Z_SPLARRAY_P(ZEND_THIS);
	HashTable *data;
	zval *flags_zv, *storage_zv, *members_zv, *iterator_class_zv;
	zend_long flags;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &data) == FAILURE) {
		RETURN_THROWS();
	}

	flags_zv          = zend_hash_index_find(data, 0);
	storage_zv        = zend_hash_index_find(data, 1);
	members_zv        = zend_hash_index_find(data, 2);
	iterator_class_zv = zend_hash_index_find(data, 3);

	if (!flags_zv || !storage_zv || !members_zv ||
			Z_TYPE_P(flags_zv) != IS_LONG || Z_TYPE_P(members_zv) != IS_ARRAY ||
			(iterator_class_zv && (Z_TYPE_P(iterator_class_zv) != IS_NULL &&
				Z_TYPE_P(iterator_class_zv) != IS_STRING))) {
		zend_throw_exception(spl_ce_UnexpectedValueException,
			"Incomplete or ill-typed serialization data", 0);
		RETURN_THROWS();
	}

	flags = Z_LVAL_P(flags_zv);
	intern->ar_flags &= ~SPL_ARRAY_CLONE_MASK;
	intern->ar_flags |= flags & SPL_ARRAY_CLONE_MASK;

	if (flags & SPL_ARRAY_IS_SELF) {
		zval_ptr_dtor(&intern->array);
		ZVAL_UNDEF(&intern->array);
	} else {
		if (Z_TYPE_P(storage_zv) != IS_OBJECT && Z_TYPE_P(storage_zv) != IS_ARRAY) {
			/* TODO Use UnexpectedValueException instead? And better error message? */
			zend_throw_exception(spl_ce_InvalidArgumentException, "Passed variable is not an array or object", 0);
			RETURN_THROWS();
		}
		spl_array_set_array(ZEND_THIS, intern, storage_zv, 0L, 1);
	}

	object_properties_load(&intern->std, Z_ARRVAL_P(members_zv));
	if (EG(exception)) {
		RETURN_THROWS();
	}

	if (iterator_class_zv && Z_TYPE_P(iterator_class_zv) == IS_STRING) {
		zend_class_entry *ce = zend_lookup_class(Z_STR_P(iterator_class_zv));

		if (!ce) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Cannot deserialize ArrayObject with iterator class '%s'; no such class exists",
				ZSTR_VAL(Z_STR_P(iterator_class_zv)));
			RETURN_THROWS();
		}

		if (!instanceof_function(ce, zend_ce_iterator)) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Cannot deserialize ArrayObject with iterator class '%s'; this class does not implement the Iterator interface",
				ZSTR_VAL(Z_STR_P(iterator_class_zv)));
			RETURN_THROWS();
		}

		intern->ce_get_iterator = ce;
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(ArrayObject, __debugInfo)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_ARR(spl_array_get_debug_info(Z_OBJ_P(ZEND_THIS)));
} /* }}} */

/*** ArrayIterator class ***/
typedef struct _spl_array_iterator {
	zend_object_iterator it;
	bool by_ref;
} spl_array_iterator;

static zend_result spl_array_next_ex(spl_array_object *intern, HashTable *aht) /* {{{ */
{
	uint32_t *pos_ptr = spl_array_get_pos_ptr(aht, intern);

	zend_hash_move_forward_ex(aht, pos_ptr);
	if (spl_array_is_object(intern)) {
		return spl_array_skip_protected(intern, aht);
	} else {
		return zend_hash_has_more_elements_ex(aht, pos_ptr);
	}
} /* }}} */

static zend_result spl_array_next(spl_array_object *intern) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern);

	return spl_array_next_ex(intern, aht);

} /* }}} */

static void spl_array_it_dtor(zend_object_iterator *iter) /* {{{ */
{
	zval_ptr_dtor(&iter->data);
}
/* }}} */

static int spl_array_it_valid(zend_object_iterator *iter) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);
	return zend_hash_has_more_elements_ex(aht, spl_array_get_pos_ptr(aht, object));
}
/* }}} */

static zval *spl_array_it_get_current_data(zend_object_iterator *iter) /* {{{ */
{
	spl_array_iterator *array_iter = (spl_array_iterator*)iter;
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);
	zval *data = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, object));
	if (data && Z_TYPE_P(data) == IS_INDIRECT) {
		data = Z_INDIRECT_P(data);
	}
	// ZEND_FE_FETCH_RW converts the value to a reference but doesn't know the source is a property.
	// Typed properties must add a type source to the reference, and readonly properties must fail.
	if (array_iter->by_ref
	 && Z_TYPE_P(data) != IS_REFERENCE
	 && Z_TYPE(object->array) == IS_OBJECT
	 && !(object->ar_flags & (SPL_ARRAY_IS_SELF|SPL_ARRAY_USE_OTHER))) {
		zend_string *key;
		zend_hash_get_current_key_ex(aht, &key, NULL, spl_array_get_pos_ptr(aht, object));
		zend_class_entry *ce = Z_OBJCE(object->array);
		zend_property_info *prop_info = zend_get_property_info(ce, key, true);
		ZEND_ASSERT(prop_info != ZEND_WRONG_PROPERTY_INFO);
		if (EXPECTED(prop_info != NULL) && ZEND_TYPE_IS_SET(prop_info->type)) {
			if (prop_info->flags & ZEND_ACC_READONLY) {
				zend_throw_error(NULL,
					"Cannot acquire reference to readonly property %s::$%s",
					ZSTR_VAL(prop_info->ce->name), ZSTR_VAL(key));
				return NULL;
			}
			ZVAL_NEW_REF(data, data);
			ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(data), prop_info);
		}
	}
	return data;
}
/* }}} */

static void spl_array_it_get_current_key(zend_object_iterator *iter, zval *key) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);
	zend_hash_get_current_key_zval_ex(aht, key, spl_array_get_pos_ptr(aht, object));
}
/* }}} */

static void spl_array_it_move_forward(zend_object_iterator *iter) /* {{{ */
{
	spl_array_object *object = Z_SPLARRAY_P(&iter->data);
	HashTable *aht = spl_array_get_hash_table(object);
	spl_array_next_ex(object, aht);
}
/* }}} */

static void spl_array_rewind(spl_array_object *intern) /* {{{ */
{
	HashTable *aht = spl_array_get_hash_table(intern);

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
	spl_array_rewind(object);
}
/* }}} */

static HashTable *spl_array_it_get_gc(zend_object_iterator *iter, zval **table, int *n)
{
	*n = 1;
	*table = &iter->data;
	return NULL;
}

/* iterator handler table */
static const zend_object_iterator_funcs spl_array_it_funcs = {
	spl_array_it_dtor,
	spl_array_it_valid,
	spl_array_it_get_current_data,
	spl_array_it_get_current_key,
	spl_array_it_move_forward,
	spl_array_it_rewind,
	NULL,
	spl_array_it_get_gc,
};

static zend_object_iterator *spl_array_get_iterator(zend_class_entry *ce, zval *object, int by_ref) /* {{{ */
{
	spl_array_iterator *iterator = emalloc(sizeof(spl_array_iterator));
	zend_iterator_init(&iterator->it);

	ZVAL_OBJ_COPY(&iterator->it.data, Z_OBJ_P(object));
	iterator->it.funcs = &spl_array_it_funcs;
	iterator->by_ref = by_ref;

	return &iterator->it;
}
/* }}} */

/* {{{ Constructs a new array iterator from an array or object. */
PHP_METHOD(ArrayIterator, __construct)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern;
	zval *array;
	zend_long ar_flags = 0;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|Al", &array, &ar_flags) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLARRAY_P(object);

	ar_flags &= ~SPL_ARRAY_INT_MASK;

	spl_array_set_array(object, intern, array, ar_flags, ZEND_NUM_ARGS() == 1);
}
/* }}} */

/* {{{ Rewind array back to the start */
PHP_METHOD(ArrayIterator, rewind)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_array_rewind(intern);
}
/* }}} */

/* {{{ Seek to position. */
PHP_METHOD(ArrayIterator, seek)
{
	zend_long opos, position;
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &position) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Return current array entry */
PHP_METHOD(ArrayIterator, current)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	zval *entry;
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if ((entry = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, intern))) == NULL) {
		RETURN_NULL();
	}
	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
		if (Z_TYPE_P(entry) == IS_UNDEF) {
			RETURN_NULL();
		}
	}
	RETURN_COPY_DEREF(entry);
}
/* }}} */

void spl_array_iterator_key(zval *object, zval *return_value) /* {{{ */
{
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	zend_hash_get_current_key_zval_ex(aht, return_value, spl_array_get_pos_ptr(aht, intern));
}
/* }}} */

/* {{{ Return current array key */
PHP_METHOD(ArrayIterator, key)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_array_iterator_key(ZEND_THIS, return_value);
} /* }}} */

/* {{{ Move to next entry */
PHP_METHOD(ArrayIterator, next)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	spl_array_next_ex(intern, aht);
}
/* }}} */

/* {{{ Check whether array contains more entries */
PHP_METHOD(ArrayIterator, valid)
{
	zval *object = ZEND_THIS;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(zend_hash_has_more_elements_ex(aht, spl_array_get_pos_ptr(aht, intern)) == SUCCESS);
}
/* }}} */

/*** RecursiveArrayIterator methods ***/

/* {{{ Check whether current element has children (e.g. is an array) */
PHP_METHOD(RecursiveArrayIterator, hasChildren)
{
	zval *object = ZEND_THIS, *entry;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

static void spl_instantiate_child_arg(zend_class_entry *pce, zval *retval, zval *arg1, zval *arg2) /* {{{ */
{
	object_init_ex(retval, pce);
	spl_array_object *new_intern = Z_SPLARRAY_P(retval);
	/*
	 * set new_intern->is_child is true to indicate that the object was created by
	 * RecursiveArrayIterator::getChildren() method.
	 */
	new_intern->is_child = true;

	/* find the bucket of parent object. */
	new_intern->bucket = (Bucket *)((char *)(arg1) - XtOffsetOf(Bucket, val));;
	zend_call_known_instance_method_with_2_params(pce->constructor, Z_OBJ_P(retval), NULL, arg1, arg2);
}
/* }}} */

/* {{{ Create a sub iterator for the current element (same class as $this) */
PHP_METHOD(RecursiveArrayIterator, getChildren)
{
	zval *object = ZEND_THIS, *entry, flags;
	spl_array_object *intern = Z_SPLARRAY_P(object);
	HashTable *aht = spl_array_get_hash_table(intern);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if ((entry = zend_hash_get_current_data_ex(aht, spl_array_get_pos_ptr(aht, intern))) == NULL) {
		RETURN_NULL();
	}

	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
	}

	ZVAL_DEREF(entry);
	if (Z_TYPE_P(entry) == IS_OBJECT) {
		if ((intern->ar_flags & SPL_ARRAY_CHILD_ARRAYS_ONLY) != 0) {
			RETURN_NULL();
		}
		if (instanceof_function(Z_OBJCE_P(entry), Z_OBJCE_P(ZEND_THIS))) {
			RETURN_OBJ_COPY(Z_OBJ_P(entry));
		}
	}

	ZVAL_LONG(&flags, intern->ar_flags);
	spl_instantiate_child_arg(Z_OBJCE_P(ZEND_THIS), return_value, entry, &flags);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(spl_array) */
PHP_MINIT_FUNCTION(spl_array)
{
	spl_ce_ArrayObject = register_class_ArrayObject(zend_ce_aggregate, zend_ce_arrayaccess, zend_ce_serializable, zend_ce_countable);
	spl_ce_ArrayObject->create_object = spl_array_object_new;
	spl_ce_ArrayObject->default_object_handlers = &spl_handler_ArrayObject;

	memcpy(&spl_handler_ArrayObject, &std_object_handlers, sizeof(zend_object_handlers));

	spl_handler_ArrayObject.offset = XtOffsetOf(spl_array_object, std);

	spl_handler_ArrayObject.clone_obj = spl_array_object_clone;
	spl_handler_ArrayObject.read_dimension = spl_array_read_dimension;
	spl_handler_ArrayObject.write_dimension = spl_array_write_dimension;
	spl_handler_ArrayObject.unset_dimension = spl_array_unset_dimension;
	spl_handler_ArrayObject.has_dimension = spl_array_has_dimension;
	spl_handler_ArrayObject.count_elements = spl_array_object_count_elements;

	spl_handler_ArrayObject.get_properties_for = spl_array_get_properties_for;
	spl_handler_ArrayObject.get_gc = spl_array_get_gc;
	spl_handler_ArrayObject.read_property = spl_array_read_property;
	spl_handler_ArrayObject.write_property = spl_array_write_property;
	spl_handler_ArrayObject.get_property_ptr_ptr = spl_array_get_property_ptr_ptr;
	spl_handler_ArrayObject.has_property = spl_array_has_property;
	spl_handler_ArrayObject.unset_property = spl_array_unset_property;

	spl_handler_ArrayObject.compare = spl_array_compare_objects;
	spl_handler_ArrayObject.free_obj = spl_array_object_free_storage;

	spl_ce_ArrayIterator = register_class_ArrayIterator(spl_ce_SeekableIterator, zend_ce_arrayaccess, zend_ce_serializable, zend_ce_countable);
	spl_ce_ArrayIterator->create_object = spl_array_object_new;
	spl_ce_ArrayIterator->default_object_handlers = &spl_handler_ArrayIterator;
	spl_ce_ArrayIterator->get_iterator = spl_array_get_iterator;

	memcpy(&spl_handler_ArrayIterator, &spl_handler_ArrayObject, sizeof(zend_object_handlers));

	spl_ce_RecursiveArrayIterator = register_class_RecursiveArrayIterator(spl_ce_ArrayIterator, spl_ce_RecursiveIterator);
	spl_ce_RecursiveArrayIterator->create_object = spl_array_object_new;
	spl_ce_RecursiveArrayIterator->get_iterator = spl_array_get_iterator;

	return SUCCESS;
}
/* }}} */
