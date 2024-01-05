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
  | Author: Antony Dovgal <tony@daylessday.org>                          |
  |         Etienne Kneuss <colder@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "php_spl.h"
#include "spl_fixedarray_arginfo.h"
#include "spl_functions.h"
#include "spl_engine.h"
#include "spl_fixedarray.h"
#include "spl_exceptions.h"
#include "spl_iterators.h"
#include "ext/json/php_json.h"

zend_object_handlers spl_handler_SplFixedArray;
PHPAPI zend_class_entry *spl_ce_SplFixedArray;

#ifdef COMPILE_DL_SPL_FIXEDARRAY
ZEND_GET_MODULE(spl_fixedarray)
#endif

/* Check if the object is an instance of a subclass of SplFixedArray that overrides method's implementation.
 * Expect subclassing SplFixedArray to be rare and check that first. */
#define HAS_FIXEDARRAY_ARRAYACCESS_OVERRIDE(object, method) UNEXPECTED((object)->ce != spl_ce_SplFixedArray && (object)->ce->arrayaccess_funcs_ptr->method->common.scope != spl_ce_SplFixedArray)

typedef struct _spl_fixedarray {
	zend_long size;
	/* It is possible to resize this, so this can't be combined with the object */
	zval *elements;
	/* If positive, it's a resize within a resize and the value gives the desired size. If -1, it's not. */
	zend_long cached_resize;
} spl_fixedarray;

typedef struct _spl_fixedarray_object {
	spl_fixedarray          array;
	zend_function          *fptr_count;
	zend_object             std;
} spl_fixedarray_object;

typedef struct _spl_fixedarray_it {
	zend_object_iterator intern;
	zend_long            current;
} spl_fixedarray_it;

static spl_fixedarray_object *spl_fixed_array_from_obj(zend_object *obj)
{
	return (spl_fixedarray_object*)((char*)(obj) - XtOffsetOf(spl_fixedarray_object, std));
}

#define Z_SPLFIXEDARRAY_P(zv)  spl_fixed_array_from_obj(Z_OBJ_P((zv)))

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then elements == NULL
 *   - if size > 0, then elements != NULL
 *   - size is not less than 0
 */
static bool spl_fixedarray_empty(spl_fixedarray *array)
{
	if (array->elements) {
		ZEND_ASSERT(array->size > 0);
		return false;
	}
	ZEND_ASSERT(array->size == 0);
	return true;
}

static void spl_fixedarray_default_ctor(spl_fixedarray *array)
{
	array->size = 0;
	array->elements = NULL;
}

/* Initializes the range [from, to) to null. Does not dtor existing elements. */
static void spl_fixedarray_init_elems(spl_fixedarray *array, zend_long from, zend_long to)
{
	ZEND_ASSERT(from <= to);
	zval *begin = array->elements + from, *end = array->elements + to;

	while (begin != end) {
		ZVAL_NULL(begin++);
	}
}

static void spl_fixedarray_init_non_empty_struct(spl_fixedarray *array, zend_long size)
{
	array->size = 0; /* reset size in case ecalloc() fails */
	array->elements = size ? safe_emalloc(size, sizeof(zval), 0) : NULL;
	array->size = size;
}

static void spl_fixedarray_init(spl_fixedarray *array, zend_long size)
{
	if (size > 0) {
		spl_fixedarray_init_non_empty_struct(array, size);
		spl_fixedarray_init_elems(array, 0, size);
	} else {
		spl_fixedarray_default_ctor(array);
	}
	array->cached_resize = -1;
}

/* Copies the range [begin, end) into the fixedarray, beginning at `offset`.
 * Does not dtor the existing elements.
 */
static void spl_fixedarray_copy_range(spl_fixedarray *array, zend_long offset, zval *begin, zval *end)
{
	ZEND_ASSERT(offset >= 0);
	ZEND_ASSERT(array->size - offset >= end - begin);

	zval *to = &array->elements[offset];
	while (begin != end) {
		ZVAL_COPY(to++, begin++);
	}
}

static void spl_fixedarray_copy_ctor(spl_fixedarray *to, spl_fixedarray *from)
{
	zend_long size = from->size;
	spl_fixedarray_init(to, size);
	if (size != 0) {
		zval *begin = from->elements, *end = from->elements + size;
		spl_fixedarray_copy_range(to, 0, begin, end);
	}
}

/* Destructs the elements in the range [from, to).
 * Caller is expected to bounds check.
 */
static void spl_fixedarray_dtor_range(spl_fixedarray *array, zend_long from, zend_long to)
{
	array->size = from;
	zval *begin = array->elements + from, *end = array->elements + to;
	while (begin != end) {
		zval_ptr_dtor(begin++);
	}
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void spl_fixedarray_dtor(spl_fixedarray *array)
{
	if (!spl_fixedarray_empty(array)) {
		zval *begin = array->elements, *end = array->elements + array->size;
		array->elements = NULL;
		array->size = 0;
		while (begin != end) {
			zval_ptr_dtor(--end);
		}
		efree(begin);
	}
}

static void spl_fixedarray_resize(spl_fixedarray *array, zend_long size)
{
	if (size == array->size) {
		/* nothing to do */
		return;
	}

	/* first initialization */
	if (array->size == 0) {
		spl_fixedarray_init(array, size);
		return;
	}

	if (UNEXPECTED(array->cached_resize >= 0)) {
		/* We're already resizing, so just remember the desired size.
		 * The resize will happen later. */
		array->cached_resize = size;
		return;
	}
	array->cached_resize = size;

	/* clearing the array */
	if (size == 0) {
		spl_fixedarray_dtor(array);
		array->elements = NULL;
		array->size = 0;
	} else if (size > array->size) {
		array->elements = safe_erealloc(array->elements, size, sizeof(zval), 0);
		spl_fixedarray_init_elems(array, array->size, size);
		array->size = size;
	} else { /* size < array->size */
		/* Size set in spl_fixedarray_dtor_range() */
		spl_fixedarray_dtor_range(array, size, array->size);
		array->elements = erealloc(array->elements, sizeof(zval) * size);
	}

	/* If resized within the destructor, take the last resize command and perform it */
	zend_long cached_resize = array->cached_resize;
	array->cached_resize = -1;
	if (cached_resize != size) {
		spl_fixedarray_resize(array, cached_resize);
	}
}

static HashTable* spl_fixedarray_object_get_gc(zend_object *obj, zval **table, int *n)
{
	spl_fixedarray_object *intern = spl_fixed_array_from_obj(obj);
	HashTable *ht = zend_std_get_properties(obj);

	*table = intern->array.elements;
	*n = (int)intern->array.size;

	return ht;
}

static HashTable* spl_fixedarray_object_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	/* This has __serialize, so the purpose is not ZEND_PROP_PURPOSE_SERIALIZE, which would expect a non-null return value */
	ZEND_ASSERT(purpose != ZEND_PROP_PURPOSE_SERIALIZE);

	const spl_fixedarray_object *intern = spl_fixed_array_from_obj(obj);
	/*
	 * SplFixedArray can be subclassed or have dynamic properties (With or without AllowDynamicProperties in subclasses).
	 * Instances of subclasses with declared properties may have properties but not yet have a property table.
	 */
	HashTable *source_properties = obj->properties ? obj->properties : (obj->ce->default_properties_count ? zend_std_get_properties(obj) : NULL);

	const zend_long size = intern->array.size;
	if (size == 0 && (!source_properties || !zend_hash_num_elements(source_properties))) {
		return NULL;
	}
	zval *const elements = intern->array.elements;
	HashTable *ht = zend_new_array(size);

	for (zend_long i = 0; i < size; i++) {
		Z_TRY_ADDREF_P(&elements[i]);
		zend_hash_next_index_insert(ht, &elements[i]);
	}
	if (source_properties && zend_hash_num_elements(source_properties) > 0) {
		zend_long nkey;
		zend_string *skey;
		zval *value;
		ZEND_HASH_MAP_FOREACH_KEY_VAL_IND(source_properties, nkey, skey, value) {
			Z_TRY_ADDREF_P(value);
			if (skey) {
				zend_hash_add_new(ht, skey, value);
			} else {
				zend_hash_index_update(ht, nkey, value);
			}
		} ZEND_HASH_FOREACH_END();
	}

	return ht;
}

static void spl_fixedarray_object_free_storage(zend_object *object)
{
	spl_fixedarray_object *intern = spl_fixed_array_from_obj(object);
	spl_fixedarray_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *spl_fixedarray_object_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	spl_fixedarray_object *intern;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(spl_fixedarray_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);

	if (orig && clone_orig) {
		spl_fixedarray_object *other = spl_fixed_array_from_obj(orig);
		spl_fixedarray_copy_ctor(&intern->array, &other->array);
	}

	inherited = class_type != spl_ce_SplFixedArray;

	if (UNEXPECTED(inherited)) {
		/* Find count() method */
		zend_function *fptr_count = zend_hash_find_ptr(&class_type->function_table, ZSTR_KNOWN(ZEND_STR_COUNT));
		if (fptr_count->common.scope == spl_ce_SplFixedArray) {
			fptr_count = NULL;
		}
		intern->fptr_count = fptr_count;
	}

	return &intern->std;
}

static zend_object *spl_fixedarray_new(zend_class_entry *class_type)
{
	return spl_fixedarray_object_new_ex(class_type, NULL, 0);
}

static zend_object *spl_fixedarray_object_clone(zend_object *old_object)
{
	zend_object *new_object = spl_fixedarray_object_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static zend_long spl_offset_convert_to_long(zval *offset) /* {{{ */
{
	try_again:
	switch (Z_TYPE_P(offset)) {
		case IS_STRING: {
			zend_ulong index;
			if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), index)) {
				return (zend_long) index;
			}
			break;
		}
		case IS_DOUBLE:
			return zend_dval_to_lval_safe(Z_DVAL_P(offset));
		case IS_LONG:
			return Z_LVAL_P(offset);
		case IS_FALSE:
			return 0;
		case IS_TRUE:
			return 1;
		case IS_REFERENCE:
			offset = Z_REFVAL_P(offset);
			goto try_again;
		case IS_RESOURCE:
			zend_use_resource_as_offset(offset);
			return Z_RES_HANDLE_P(offset);
	}

	/* Use SplFixedArray name from the CE */
	zend_illegal_container_offset(spl_ce_SplFixedArray->name, offset, BP_VAR_R);
	return 0;
}

static zval *spl_fixedarray_object_read_dimension_helper(spl_fixedarray_object *intern, zval *offset)
{
	zend_long index;

	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (!offset) {
		zend_throw_error(NULL, "[] operator not supported for SplFixedArray");
		return NULL;
	}

	index = spl_offset_convert_to_long(offset);
	if (EG(exception)) {
		return NULL;
	}

	if (index < 0 || index >= intern->array.size) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index invalid or out of range", 0);
		return NULL;
	} else {
		return &intern->array.elements[index];
	}
}

static int spl_fixedarray_object_has_dimension(zend_object *object, zval *offset, int check_empty);

static zval *spl_fixedarray_object_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (type == BP_VAR_IS && !spl_fixedarray_object_has_dimension(object, offset, 0)) {
		return &EG(uninitialized_zval);
	}

	if (HAS_FIXEDARRAY_ARRAYACCESS_OVERRIDE(object, zf_offsetget)) {
		zval tmp;
		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		}
		zend_call_known_instance_method_with_1_params(object->ce->arrayaccess_funcs_ptr->zf_offsetget, object, rv, offset);
		if (!Z_ISUNDEF_P(rv)) {
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	spl_fixedarray_object *intern = spl_fixed_array_from_obj(object);
	return spl_fixedarray_object_read_dimension_helper(intern, offset);
}

static void spl_fixedarray_object_write_dimension_helper(spl_fixedarray_object *intern, zval *offset, zval *value)
{
	zend_long index;

	if (!offset) {
		/* '$array[] = value' syntax is not supported */
		zend_throw_error(NULL, "[] operator not supported for SplFixedArray");
		return;
	}

	index = spl_offset_convert_to_long(offset);
	if (EG(exception)) {
		return;
	}

	if (index < 0 || index >= intern->array.size) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index invalid or out of range", 0);
		return;
	} else {
		/* Fix #81429 */
		zval *ptr = &(intern->array.elements[index]);
		zval tmp;
		ZVAL_COPY_VALUE(&tmp, ptr);
		ZVAL_COPY_DEREF(ptr, value);
		zval_ptr_dtor(&tmp);
	}
}

static void spl_fixedarray_object_write_dimension(zend_object *object, zval *offset, zval *value)
{
	if (HAS_FIXEDARRAY_ARRAYACCESS_OVERRIDE(object, zf_offsetset)) {
		zval tmp;

		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		}
		zend_call_known_instance_method_with_2_params(object->ce->arrayaccess_funcs_ptr->zf_offsetset, object, NULL, offset, value);
		return;
	}

	spl_fixedarray_object *intern = spl_fixed_array_from_obj(object);
	spl_fixedarray_object_write_dimension_helper(intern, offset, value);
}

static void spl_fixedarray_object_unset_dimension_helper(spl_fixedarray_object *intern, zval *offset)
{
	zend_long index;

	index = spl_offset_convert_to_long(offset);
	if (EG(exception)) {
		return;
	}

	if (index < 0 || index >= intern->array.size) {
		zend_throw_exception(spl_ce_OutOfBoundsException, "Index invalid or out of range", 0);
		return;
	} else {
		zval_ptr_dtor(&(intern->array.elements[index]));
		ZVAL_NULL(&intern->array.elements[index]);
	}
}

static void spl_fixedarray_object_unset_dimension(zend_object *object, zval *offset)
{
	if (UNEXPECTED(HAS_FIXEDARRAY_ARRAYACCESS_OVERRIDE(object, zf_offsetunset))) {
		zend_call_known_instance_method_with_1_params(object->ce->arrayaccess_funcs_ptr->zf_offsetunset, object, NULL, offset);
		return;
	}

	spl_fixedarray_object *intern = spl_fixed_array_from_obj(object);
	spl_fixedarray_object_unset_dimension_helper(intern, offset);
}

static bool spl_fixedarray_object_has_dimension_helper(spl_fixedarray_object *intern, zval *offset, bool check_empty)
{
	zend_long index;

	index = spl_offset_convert_to_long(offset);
	if (EG(exception)) {
		return false;
	}

	if (index < 0 || index >= intern->array.size) {
		return false;
	}

	if (check_empty) {
		return zend_is_true(&intern->array.elements[index]);
	}

	return Z_TYPE(intern->array.elements[index]) != IS_NULL;
}

static int spl_fixedarray_object_has_dimension(zend_object *object, zval *offset, int check_empty)
{
	if (HAS_FIXEDARRAY_ARRAYACCESS_OVERRIDE(object, zf_offsetexists)) {
		zval rv;

		zend_call_known_instance_method_with_1_params(object->ce->arrayaccess_funcs_ptr->zf_offsetexists, object, &rv, offset);
		bool result = zend_is_true(&rv);
		zval_ptr_dtor(&rv);
		return result;
	}

	spl_fixedarray_object *intern = spl_fixed_array_from_obj(object);

	return spl_fixedarray_object_has_dimension_helper(intern, offset, check_empty);
}

static zend_result spl_fixedarray_object_count_elements(zend_object *object, zend_long *count)
{
	spl_fixedarray_object *intern;

	intern = spl_fixed_array_from_obj(object);
	if (UNEXPECTED(intern->fptr_count)) {
		zval rv;
		zend_call_known_instance_method_with_0_params(intern->fptr_count, object, &rv);
		if (!Z_ISUNDEF(rv)) {
			*count = zval_get_long(&rv);
			zval_ptr_dtor(&rv);
		} else {
			*count = 0;
		}
	} else {
		*count = intern->array.size;
	}
	return SUCCESS;
}

PHP_METHOD(SplFixedArray, __construct)
{
	zval *object = ZEND_THIS;
	spl_fixedarray_object *intern;
	zend_long size = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &size) == FAILURE) {
		RETURN_THROWS();
	}

	if (size < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(object);

	if (!spl_fixedarray_empty(&intern->array)) {
		/* called __construct() twice, bail out */
		return;
	}

	spl_fixedarray_init(&intern->array, size);
}

PHP_METHOD(SplFixedArray, __wakeup)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	HashTable *intern_ht = zend_std_get_properties(Z_OBJ_P(ZEND_THIS));
	zval *data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->array.size == 0) {
		int index = 0;
		int size = zend_hash_num_elements(intern_ht);

		spl_fixedarray_init(&intern->array, size);

		ZEND_HASH_FOREACH_VAL(intern_ht, data) {
			ZVAL_COPY(&intern->array.elements[index], data);
			index++;
		} ZEND_HASH_FOREACH_END();

		/* Remove the unserialised properties, since we now have the elements
		 * within the spl_fixedarray_object structure. */
		zend_hash_clean(intern_ht);
	}
}

PHP_METHOD(SplFixedArray, __serialize)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	zval *current;
	zend_string *key;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	HashTable *ht = zend_std_get_properties(&intern->std);
	uint32_t num_properties = zend_hash_num_elements(ht);
	array_init_size(return_value, intern->array.size + num_properties);

	/* elements */
	for (zend_long i = 0; i < intern->array.size; i++) {
		current = &intern->array.elements[i];
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), current);
		Z_TRY_ADDREF_P(current);
	}

	/* members */
	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(ht, key, current) {
		/* If the properties table was already rebuild, it will also contain the
		 * array elements. The array elements are already added in the above loop.
		 * We can detect array elements by the fact that their key == NULL. */
		if (key != NULL) {
			zend_hash_add_new(Z_ARRVAL_P(return_value), key, current);
			Z_TRY_ADDREF_P(current);
		}
	} ZEND_HASH_FOREACH_END();
}

PHP_METHOD(SplFixedArray, __unserialize)
{
	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	HashTable *data;
	zval members_zv, *elem;
	zend_string *key;
	zend_long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &data) == FAILURE) {
		RETURN_THROWS();
	}

	if (intern->array.size == 0) {
		size = zend_hash_num_elements(data);
		spl_fixedarray_init_non_empty_struct(&intern->array, size);
		if (!size) {
			return;
		}
		array_init(&members_zv);

		intern->array.size = 0;
		ZEND_HASH_FOREACH_STR_KEY_VAL(data, key, elem) {
			if (key == NULL) {
				ZVAL_COPY(&intern->array.elements[intern->array.size], elem);
				intern->array.size++;
			} else {
				Z_TRY_ADDREF_P(elem);
				zend_hash_add(Z_ARRVAL(members_zv), key, elem);
			}
		} ZEND_HASH_FOREACH_END();

		if (intern->array.size != size) {
			if (intern->array.size) {
				intern->array.elements = erealloc(intern->array.elements, sizeof(zval) * intern->array.size);
			} else {
				efree(intern->array.elements);
				intern->array.elements = NULL;
			}
		}

		object_properties_load(&intern->std, Z_ARRVAL(members_zv));
		zval_ptr_dtor(&members_zv);
	}
}

PHP_METHOD(SplFixedArray, count)
{
	zval *object = ZEND_THIS;
	spl_fixedarray_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(object);
	RETURN_LONG(intern->array.size);
}

PHP_METHOD(SplFixedArray, toArray)
{
	spl_fixedarray_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);

	if (!spl_fixedarray_empty(&intern->array)) {
		array_init(return_value);
		for (zend_long i = 0; i < intern->array.size; i++) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), i, &intern->array.elements[i]);
			Z_TRY_ADDREF(intern->array.elements[i]);
		}
	} else {
		RETURN_EMPTY_ARRAY();
	}
}

PHP_METHOD(SplFixedArray, fromArray)
{
	zval *data;
	spl_fixedarray array;
	spl_fixedarray_object *intern;
	int num;
	bool save_indexes = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &data, &save_indexes) == FAILURE) {
		RETURN_THROWS();
	}

	num = zend_hash_num_elements(Z_ARRVAL_P(data));

	if (num > 0 && save_indexes) {
		zval *element;
		zend_string *str_index;
		zend_ulong num_index, max_index = 0;
		zend_long tmp;

		ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(data), num_index, str_index) {
			if (str_index != NULL || (zend_long)num_index < 0) {
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "array must contain only positive integer keys");
				RETURN_THROWS();
			}

			if (num_index > max_index) {
				max_index = num_index;
			}
		} ZEND_HASH_FOREACH_END();

		tmp = max_index + 1;
		if (tmp <= 0) {
			zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "integer overflow detected");
			RETURN_THROWS();
		}
		spl_fixedarray_init(&array, tmp);

		ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(data), num_index, element) {
			ZVAL_COPY_DEREF(&array.elements[num_index], element);
		} ZEND_HASH_FOREACH_END();

	} else if (num > 0 && !save_indexes) {
		zval *element;
		zend_long i = 0;

		spl_fixedarray_init(&array, num);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(data), element) {
			ZVAL_COPY_DEREF(&array.elements[i], element);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		spl_fixedarray_init(&array, 0);
	}

	object_init_ex(return_value, spl_ce_SplFixedArray);

	intern = Z_SPLFIXEDARRAY_P(return_value);
	intern->array = array;
}

PHP_METHOD(SplFixedArray, getSize)
{
	zval *object = ZEND_THIS;
	spl_fixedarray_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(object);
	RETURN_LONG(intern->array.size);
}

PHP_METHOD(SplFixedArray, setSize)
{
	zval *object = ZEND_THIS;
	spl_fixedarray_object *intern;
	zend_long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		RETURN_THROWS();
	}

	if (size < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(object);

	spl_fixedarray_resize(&intern->array, size);
	RETURN_TRUE;
}

/* Returns whether the requested $index exists. */
PHP_METHOD(SplFixedArray, offsetExists)
{
	zval                  *zindex;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);

	RETURN_BOOL(spl_fixedarray_object_has_dimension_helper(intern, zindex, 0));
}

/* Returns the value at the specified $index. */
PHP_METHOD(SplFixedArray, offsetGet)
{
	zval *zindex, *value;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	value = spl_fixedarray_object_read_dimension_helper(intern, zindex);

	if (value) {
		RETURN_COPY_DEREF(value);
	} else {
		RETURN_NULL();
	}
}

/* Sets the value at the specified $index to $newval. */
PHP_METHOD(SplFixedArray, offsetSet)
{
	zval                  *zindex, *value;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zindex, &value) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	spl_fixedarray_object_write_dimension_helper(intern, zindex, value);

}

/* Unsets the value at the specified $index. */
PHP_METHOD(SplFixedArray, offsetUnset)
{
	zval                  *zindex;
	spl_fixedarray_object  *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zindex) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	spl_fixedarray_object_unset_dimension_helper(intern, zindex);

}

/* Create a new iterator from a SplFixedArray instance. */
PHP_METHOD(SplFixedArray, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

PHP_METHOD(SplFixedArray, jsonSerialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	spl_fixedarray_object *intern = Z_SPLFIXEDARRAY_P(ZEND_THIS);
	array_init_size(return_value, intern->array.size);
	for (zend_long i = 0; i < intern->array.size; i++) {
		zend_hash_next_index_insert_new(Z_ARR_P(return_value), &intern->array.elements[i]);
		Z_TRY_ADDREF(intern->array.elements[i]);
	}
}

static void spl_fixedarray_it_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
}

static void spl_fixedarray_it_rewind(zend_object_iterator *iter)
{
	((spl_fixedarray_it*)iter)->current = 0;
}

static int spl_fixedarray_it_valid(zend_object_iterator *iter)
{
	spl_fixedarray_it     *iterator = (spl_fixedarray_it*)iter;
	spl_fixedarray_object *object   = Z_SPLFIXEDARRAY_P(&iter->data);

	if (iterator->current >= 0 && iterator->current < object->array.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval *spl_fixedarray_it_get_current_data(zend_object_iterator *iter)
{
	zval zindex, *data;
	spl_fixedarray_it     *iterator = (spl_fixedarray_it*)iter;
	spl_fixedarray_object *object   = Z_SPLFIXEDARRAY_P(&iter->data);

	ZVAL_LONG(&zindex, iterator->current);
	data = spl_fixedarray_object_read_dimension_helper(object, &zindex);

	if (data == NULL) {
		data = &EG(uninitialized_zval);
	}
	return data;
}

static void spl_fixedarray_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	ZVAL_LONG(key, ((spl_fixedarray_it*)iter)->current);
}

static void spl_fixedarray_it_move_forward(zend_object_iterator *iter)
{
	((spl_fixedarray_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs spl_fixedarray_it_funcs = {
	spl_fixedarray_it_dtor,
	spl_fixedarray_it_valid,
	spl_fixedarray_it_get_current_data,
	spl_fixedarray_it_get_current_key,
	spl_fixedarray_it_move_forward,
	spl_fixedarray_it_rewind,
	NULL,
	NULL, /* get_gc */
};

static zend_object_iterator *spl_fixedarray_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	spl_fixedarray_it *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(spl_fixedarray_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &spl_fixedarray_it_funcs;

	return &iterator->intern;
}

PHP_MINIT_FUNCTION(spl_fixedarray)
{
	spl_ce_SplFixedArray = register_class_SplFixedArray(
		zend_ce_aggregate, zend_ce_arrayaccess, zend_ce_countable, php_json_serializable_ce);
	spl_ce_SplFixedArray->create_object = spl_fixedarray_new;
	spl_ce_SplFixedArray->default_object_handlers = &spl_handler_SplFixedArray;
	spl_ce_SplFixedArray->get_iterator = spl_fixedarray_get_iterator;

	memcpy(&spl_handler_SplFixedArray, &std_object_handlers, sizeof(zend_object_handlers));

	spl_handler_SplFixedArray.offset          = XtOffsetOf(spl_fixedarray_object, std);
	spl_handler_SplFixedArray.clone_obj       = spl_fixedarray_object_clone;
	spl_handler_SplFixedArray.read_dimension  = spl_fixedarray_object_read_dimension;
	spl_handler_SplFixedArray.write_dimension = spl_fixedarray_object_write_dimension;
	spl_handler_SplFixedArray.unset_dimension = spl_fixedarray_object_unset_dimension;
	spl_handler_SplFixedArray.has_dimension   = spl_fixedarray_object_has_dimension;
	spl_handler_SplFixedArray.count_elements  = spl_fixedarray_object_count_elements;
	spl_handler_SplFixedArray.get_properties_for = spl_fixedarray_object_get_properties_for;
	spl_handler_SplFixedArray.get_gc          = spl_fixedarray_object_get_gc;
	spl_handler_SplFixedArray.free_obj        = spl_fixedarray_object_free_storage;

	return SUCCESS;
}
