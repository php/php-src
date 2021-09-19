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
   | Author: Tyson Andre <tandre@php.net>                                 |
   +----------------------------------------------------------------------+
*/

/* This is based on spl_fixedarray.c but has lower overhead (when size is known) and is more efficient to add and remove elements from the start of the Deque  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_collections.h"
#include "collections_deque_arginfo.h"
#include "collections_deque.h"
#include "collections_internaliterator.h"
#include "collections_util.h"
// #include "ext/collections/collections_functions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"
#include "ext/json/php_json.h"
#include "ext/spl/spl_util.h"

#include <stdbool.h>

static const zval collections_empty_entry_list[1];

/* Common functionality */
#define CONVERT_OFFSET_TO_LONG_OR_THROW(index, zv) do { \
       if (Z_TYPE_P(offset_zv) != IS_LONG) { \
               index = spl_offset_convert_to_long(offset_zv); \
               if (UNEXPECTED(EG(exception))) { \
                       return; \
               } \
       } else { \
               index = Z_LVAL_P(offset_zv); \
       } \
} while(0)

#define CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(index, zv) do { \
       if (Z_TYPE_P(offset_zv) != IS_LONG) { \
               index = spl_offset_convert_to_long(offset_zv); \
               if (UNEXPECTED(EG(exception))) { \
                       return NULL; \
               } \
       } else { \
               index = Z_LVAL_P(offset_zv); \
       } \
} while(0)

static void collections_throw_invalid_sequence_index_exception(void)
{
	zend_throw_exception(spl_ce_OutOfBoundsException, "Index out of range", 0);
}

#define COLLECTIONS_THROW_INVALID_SEQUENCE_INDEX_EXCEPTION() do { collections_throw_invalid_sequence_index_exception(); RETURN_THROWS(); } while (0)
/* End common functionality */

#define COLLECTIONS_DEQUE_MIN_CAPACITY 4
#define COLLECTIONS_DEQUE_MIN_MASK (COLLECTIONS_DEQUE_MIN_CAPACITY - 1)

static zend_always_inline size_t collections_deque_next_pow2_capacity(size_t nSize) {
	return collections_next_pow2_capacity(nSize, COLLECTIONS_DEQUE_MIN_CAPACITY);
}

zend_object_handlers collections_handler_Deque;
zend_class_entry *collections_ce_Deque;

typedef struct _collections_deque_entries {
	/** This is a circular buffer with an offset, size, and capacity(mask + 1) */
	zval *circular_buffer;
	/* The number of elements in the Deque. */
	uint32_t size;
	/* One less than a power of two (the capacity) */
	uint32_t mask;
	collections_intrusive_dllist active_iterators;
	/* The offset of the start of the deque in the circular buffer. */
	uint32_t offset;
} collections_deque_entries;

/* Is this 0 or a power of 2? */
static zend_always_inline bool collections_is_valid_uint32_capacity(uint32_t size) {
	return (size & (size-1)) == 0;
}

static zend_always_inline uint32_t collections_deque_entries_get_capacity(const collections_deque_entries *array)
{
	return array->mask ? array->mask + 1 : 0;
}

static zend_always_inline void collections_deque_entries_try_shrink_capacity(collections_deque_entries *array, uint32_t old_size);

#if ZEND_DEBUG
static void DEBUG_ASSERT_CONSISTENT_DEQUE(const collections_deque_entries *array) {
	const uint32_t capacity = collections_deque_entries_get_capacity(array);
	ZEND_ASSERT(array->size <= capacity);
	ZEND_ASSERT(array->offset < capacity || capacity == 0);
	ZEND_ASSERT(array->mask == 0 || (array->circular_buffer != NULL && array->circular_buffer != collections_empty_entry_list));
	ZEND_ASSERT(collections_is_valid_uint32_capacity(capacity));
	ZEND_ASSERT(array->circular_buffer != NULL || ((array->size == 0 && array->offset == 0) || capacity == 0));
}
#else
#define DEBUG_ASSERT_CONSISTENT_DEQUE(array) do {} while(0)
#endif

static zend_always_inline zval* collections_deque_get_entry_at_offset(const collections_deque_entries *array, uint32_t offset) {
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
	ZEND_ASSERT(offset < array->size);
	return &array->circular_buffer[(array->offset + offset) & array->mask];
}

typedef struct _collections_deque {
	collections_deque_entries		array;
	zend_object				std;
} collections_deque;

/* Used by InternalIterator returned by Deque->getIterator() */
typedef struct _collections_deque_it {
	zend_object_iterator intern;
	collections_intrusive_dllist_node dllist_node;
	uint32_t current;
} collections_deque_it;

static zend_always_inline void collections_deque_entries_push_back(collections_deque_entries *array, zval *value);
static void collections_deque_entries_raise_capacity(collections_deque_entries *array, const size_t new_capacity);
static void collections_deque_entries_shrink_capacity(collections_deque_entries *array, const uint32_t new_capacity);

static zend_always_inline collections_deque *collections_deque_from_object(zend_object *obj)
{
	return (collections_deque*)((char*)(obj) - XtOffsetOf(collections_deque, std));
}

static zend_always_inline zend_object *collections_deque_to_object(collections_deque_entries *array)
{
	return (zend_object*)((char*)(array) + XtOffsetOf(collections_deque, std));
}

static zend_always_inline collections_deque_it *collections_deque_it_from_node(collections_intrusive_dllist_node *node)
{
	return (collections_deque_it*)((char*)(node) - XtOffsetOf(collections_deque_it, dllist_node));
}

#define collections_deque_entries_from_object(obj) (&collections_deque_from_object((obj))->array)

#define Z_DEQUE_P(zv)  collections_deque_from_object(Z_OBJ_P((zv)))
#define Z_DEQUE_ENTRIES_P(zv)  collections_deque_entries_from_object(Z_OBJ_P((zv)))


static zend_always_inline bool collections_deque_entries_empty_capacity(const collections_deque_entries *array)
{
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
	return array->mask == 0;
}

static zend_always_inline bool collections_deque_entries_uninitialized(const collections_deque_entries *array)
{
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
	return array->circular_buffer == NULL;
}

static void collections_deque_entries_init_from_array(collections_deque_entries *array, zend_array *values)
{
	zend_long size = zend_hash_num_elements(values);
	array->offset = 0;
	array->size = 0; /* reset size and capacity in case emalloc() fails */
	array->mask = 0;
	if (size > 0) {
		zval *val;
		zval *circular_buffer;
		int i = 0;

		const uint32_t capacity = collections_deque_next_pow2_capacity(size);
		array->circular_buffer = circular_buffer = safe_emalloc(capacity, sizeof(zval), 0);
		array->size = size;
		array->mask = capacity - 1;
		ZEND_HASH_FOREACH_VAL(values, val)  {
			ZEND_ASSERT(i < size);
			/* This circular buffer is being initialized with an array->offset of 0. */
			ZVAL_COPY_DEREF(&circular_buffer[i], val);
			i++;
		} ZEND_HASH_FOREACH_END();
	} else {
		array->circular_buffer = (zval *)collections_empty_entry_list;
	}
}

static void collections_deque_entries_init_from_traversable(collections_deque_entries *array, zend_object *obj)
{
	zend_class_entry *ce = obj->ce;
	zend_object_iterator *iter;
	uint32_t size = 0;
	size_t capacity = 0;
	array->size = 0;
	array->offset = 0;
	array->circular_buffer = NULL;
	zval *circular_buffer = NULL;
	zval tmp_obj;
	ZVAL_OBJ(&tmp_obj, obj);
	iter = ce->get_iterator(ce, &tmp_obj, 0);

	if (UNEXPECTED(EG(exception))) {
		return;
	}

	const zend_object_iterator_funcs *funcs = iter->funcs;

	if (funcs->rewind) {
		funcs->rewind(iter);
		if (UNEXPECTED(EG(exception))) {
			goto cleanup_iter;
		}
	}

	while (funcs->valid(iter) == SUCCESS) {
		if (UNEXPECTED(EG(exception))) {
			break;
		}
		zval *value = funcs->get_current_data(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}

		if (size >= capacity) {
			/* TODO: Could use countable and get_count handler to estimate the size of the array to allocate but there's no guarantee count is supported */
			if (circular_buffer) {
				capacity *= 2;
				circular_buffer = safe_erealloc(circular_buffer, capacity, sizeof(zval), 0);
			} else {
				capacity = 4;
				circular_buffer = safe_emalloc(capacity, sizeof(zval), 0);
			}
		}
		ZVAL_COPY_DEREF(&circular_buffer[size], value);
		size++;

		iter->index++;
		funcs->move_forward(iter);
		if (UNEXPECTED(EG(exception))) {
			break;
		}
	}

	array->size = size;
	array->mask = capacity > 0 ? capacity - 1 : 0;
	array->circular_buffer = circular_buffer;
cleanup_iter:
	if (iter) {
		zend_iterator_dtor(iter);
	}
}

static void collections_deque_entries_copy_ctor(collections_deque_entries *to, const collections_deque_entries *from)
{
	zend_long size = from->size;
	to->size = 0; /* reset size in case emalloc() fails */
	to->mask = 0;
	to->offset = 0;
	if (!size) {
		to->circular_buffer = (zval *)collections_empty_entry_list;
		return;
	}

	const uint32_t capacity = collections_deque_next_pow2_capacity(size);
	to->circular_buffer = safe_emalloc(size, sizeof(zval), 0);
	to->size = size;
	to->mask = capacity - 1;
	ZEND_ASSERT(to->mask <= from->mask);
	ZEND_ASSERT(from->mask > 0);

	// account for offsets
	zval *const from_buffer_start = from->circular_buffer;
	zval *from_begin = &from_buffer_start[from->offset];
	zval *const from_end = &from_buffer_start[from->mask + 1];

	zval *p_dest = to->circular_buffer;
	zval *p_end = p_dest + size;
	do {
		if (from_begin == from_end) {
			from_begin = from_buffer_start;
		}
		ZVAL_COPY(p_dest, from_begin);
		from_begin++;
		p_dest++;
	} while (p_dest < p_end);
}

/* Destructs and frees contents but not the array itself.
 * If you want to re-use the array then you need to re-initialize it.
 */
static void collections_deque_entries_dtor(collections_deque_entries *array)
{
	if (collections_deque_entries_empty_capacity(array)) {
		return;
	}
	uint32_t remaining = array->size;
	zval *const circular_buffer = array->circular_buffer;
	if (remaining > 0) {
		zval *const end = circular_buffer + array->mask + 1;
		zval *p = circular_buffer + array->offset;
		ZEND_ASSERT(p < end);
		array->circular_buffer = NULL;
		array->offset = 0;
		array->size = 0;
		array->mask = 0;
		do {
			if (p == end) {
				p = circular_buffer;
			}
			zval_ptr_dtor(p);
			p++;
			remaining--;
		} while (remaining > 0);
	}
	efree(circular_buffer);
}

static HashTable* collections_deque_get_gc(zend_object *obj, zval **table, int *n)
{
	collections_deque *intern = collections_deque_from_object(obj);

	if (!intern->array.mask) {
		ZEND_ASSERT(intern->array.size == 0);
		*n = 0;
		return obj->properties;
	}
	const uint32_t size = intern->array.size;
	const uint32_t capacity = intern->array.mask + 1;
	const uint32_t offset = intern->array.offset;
	zval * const circular_buffer = intern->array.circular_buffer;
	if (capacity - offset >= size) {
		*table = &circular_buffer[offset];
		*n = (int)size;
		return obj->properties;
	}

	// Based on spl_dllist.c
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	for (uint32_t i = offset; i < capacity; i++) {
		zend_get_gc_buffer_add_zval(gc_buffer, &circular_buffer[i]);
	}

	for (uint32_t i = 0, len = offset + size - capacity; i < len; i++) {
		zend_get_gc_buffer_add_zval(gc_buffer, &circular_buffer[len]);
	}

	/* This replaces table and n. */
	zend_get_gc_buffer_use(gc_buffer, table, n);
	return obj->properties;
}

static zend_array* collections_deque_entries_to_refcounted_array(const collections_deque_entries *array);

static HashTable* collections_deque_get_properties_for(zend_object *obj, zend_prop_purpose purpose)
{
	collections_deque_entries *array = &collections_deque_from_object(obj)->array;
	if (!array->size && !obj->properties) {
		/* Similar to ext/ffi/ffi.c zend_fake_get_properties */
		/* debug_zval_dump DEBUG purpose requires null or a refcounted array. */
		return NULL;
	}
	switch (purpose) {
		case ZEND_PROP_PURPOSE_VAR_EXPORT: /* In php 8.3+, can return brand new arrays for var_export/debug_zval_dump and have infinite recursion work */
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
		case ZEND_PROP_PURPOSE_SERIALIZE:
			/* Return null or a brand new array that will be garbage collected, to avoid increasing memory usage after the call finishes */
			if (!array->size) {
				return NULL;
			}
			return collections_deque_entries_to_refcounted_array(array);
		case ZEND_PROP_PURPOSE_JSON: /* jsonSerialize(alias of toArray) is used instead. */
		default:
			ZEND_UNREACHABLE();
			return NULL;
	}
}

static void collections_deque_free_storage(zend_object *object)
{
	collections_deque *intern = collections_deque_from_object(object);
	collections_deque_entries_dtor(&intern->array);
	zend_object_std_dtor(&intern->std);
}

static zend_object *collections_deque_new_ex(zend_class_entry *class_type, zend_object *orig, bool clone_orig)
{
	collections_deque *intern;

	intern = zend_object_alloc(sizeof(collections_deque), class_type);
	/* This is a final class */
	ZEND_ASSERT(class_type == collections_ce_Deque);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &collections_handler_Deque;

	if (orig && clone_orig) {
		collections_deque *other = collections_deque_from_object(orig);
		collections_deque_entries_copy_ctor(&intern->array, &other->array);
	} else {
		intern->array.circular_buffer = NULL;
	}

	return &intern->std;
}

static zend_object *collections_deque_new(zend_class_entry *class_type)
{
	return collections_deque_new_ex(class_type, NULL, 0);
}

static zend_object *collections_deque_clone(zend_object *old_object)
{
	zend_object *new_object = collections_deque_new_ex(old_object->ce, old_object, 1);

	return new_object;
}

static zend_result collections_deque_count_elements(zend_object *object, zend_long *count)
{
	const collections_deque *intern = collections_deque_from_object(object);
	*count = intern->array.size;
	return SUCCESS;
}

/* Get the number of elements in this deque */
PHP_METHOD(Collections_Deque, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const collections_deque *intern = Z_DEQUE_P(ZEND_THIS);
	RETURN_LONG(intern->array.size);
}

/* Returns true if there are 0 elements in this Deque. */
PHP_METHOD(Collections_Deque, isEmpty)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_BOOL(!Z_DEQUE_ENTRIES_P(ZEND_THIS)->size);
}

/* Get the capacity of this deque. Internal api meant for unit tests of Collections\Deque itself.. */
PHP_METHOD(Collections_Deque, capacity)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(collections_deque_entries_get_capacity(Z_DEQUE_ENTRIES_P(ZEND_THIS)));
}

/* Free elements and backing storage of this deque */
PHP_METHOD(Collections_Deque, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	collections_deque *intern = Z_DEQUE_P(ZEND_THIS);
	if (intern->array.mask == 0) {
		/* No backing storage to clear */
		return;
	}
	/* Immediately make the original storage inaccessible and set count/capacity to 0 in case destructors modify the deque */
	collections_deque_entries old_array = intern->array;
	memset(&intern->array, 0, sizeof(intern->array));
	collections_deque_entries_dtor(&old_array);
	if (intern->std.properties) {
		zend_hash_clean(intern->std.properties);
	}
}

/* Create this from an iterable */
PHP_METHOD(Collections_Deque, __construct)
{
	zval *object = ZEND_THIS;
	zval* iterable = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ITERABLE(iterable)
	ZEND_PARSE_PARAMETERS_END();

	collections_deque *intern = Z_DEQUE_P(object);

	if (UNEXPECTED(!collections_deque_entries_uninitialized(&intern->array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Called Collections\\Deque::__construct twice", 0);
		/* called __construct() twice, bail out */
		RETURN_THROWS();
	}

	if (iterable == NULL) {
		intern->array.offset = 0;
		intern->array.size = 0;
		intern->array.mask = 0;
		intern->array.circular_buffer = (zval *)collections_empty_entry_list;
		return;
	}

	switch (Z_TYPE_P(iterable)) {
		case IS_ARRAY:
			collections_deque_entries_init_from_array(&intern->array, Z_ARRVAL_P(iterable));
			return;
		case IS_OBJECT:
			collections_deque_entries_init_from_traversable(&intern->array, Z_OBJ_P(iterable));
			return;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

PHP_METHOD(Collections_Deque, getIterator)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

static void collections_deque_it_dtor(zend_object_iterator *iter)
{
	collections_intrusive_dllist_node *node = &((collections_deque_it*)iter)->dllist_node;
	collections_intrusive_dllist_remove(&Z_DEQUE_ENTRIES_P(&iter->data)->active_iterators, node);
	zval_ptr_dtor(&iter->data);
}

static void collections_deque_it_rewind(zend_object_iterator *iter)
{
	((collections_deque_it*)iter)->current = 0;
}

static int collections_deque_it_valid(zend_object_iterator *iter)
{
	const collections_deque_it *iterator = (collections_deque_it*)iter;
	const collections_deque *object = Z_DEQUE_P(&iter->data);
	return iterator->current < object->array.size ? SUCCESS : FAILURE;
}

static zval *collections_deque_it_get_current_data(zend_object_iterator *iter)
{
	const collections_deque_it     *iterator = (collections_deque_it*)iter;
	collections_deque_entries *array  = Z_DEQUE_ENTRIES_P(&iter->data);
	const uint32_t offset = iterator->current;

	if (UNEXPECTED(offset >= array->size)) {
		collections_throw_invalid_sequence_index_exception();
		return &EG(uninitialized_zval);
	} else {
		return collections_deque_get_entry_at_offset(array, offset);
	}
}

static void collections_deque_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	const collections_deque_it     *iterator = (collections_deque_it*)iter;
	const collections_deque_entries *array   = Z_DEQUE_ENTRIES_P(&iter->data);
	const uint32_t offset = iterator->current;

	if (offset >= array->size) {
		ZVAL_NULL(key);
	} else {
		ZVAL_LONG(key, offset);
	}
}

static void collections_deque_it_move_forward(zend_object_iterator *iter)
{
	((collections_deque_it*)iter)->current++;
}

/* iterator handler table */
static const zend_object_iterator_funcs collections_deque_it_funcs = {
	collections_deque_it_dtor,
	collections_deque_it_valid,
	collections_deque_it_get_current_data,
	collections_deque_it_get_current_key,
	collections_deque_it_move_forward,
	collections_deque_it_rewind,
	NULL,
	collections_internaliterator_get_gc,
};


zend_object_iterator *collections_deque_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	if (UNEXPECTED(by_ref)) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	collections_deque_it *iterator = emalloc(sizeof(collections_deque_it));

	zend_iterator_init((zend_object_iterator*)iterator);

	zend_object *obj = Z_OBJ_P(object);
	ZVAL_OBJ_COPY(&iterator->intern.data, obj);
	iterator->intern.funcs = &collections_deque_it_funcs;
	collections_intrusive_dllist_prepend(&collections_deque_entries_from_object(obj)->active_iterators, &iterator->dllist_node);

	(void) ce;

	return &iterator->intern;
}

PHP_METHOD(Collections_Deque, __unserialize)
{
	HashTable *raw_data;
	zval *val;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &raw_data) == FAILURE) {
		RETURN_THROWS();
	}

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(!collections_deque_entries_uninitialized(array))) {
		zend_throw_exception(spl_ce_RuntimeException, "Already unserialized", 0);
		RETURN_THROWS();
	}
	const uint32_t num_entries = zend_hash_num_elements(raw_data);
	ZEND_ASSERT(array->circular_buffer == NULL);

	if (num_entries == 0) {
		array->offset = 0;
		array->size = 0;
		array->mask = 0;
		array->circular_buffer = (zval *)collections_empty_entry_list;
		return;
	}

	const uint32_t capacity = collections_deque_next_pow2_capacity(num_entries);
	zval *const circular_buffer = safe_emalloc(capacity, sizeof(zval), 0);
	zval *it = circular_buffer;

	zend_string *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL(raw_data, str, val) {
		if (UNEXPECTED(str)) {
			for (zval *deleteIt = circular_buffer; deleteIt < it; deleteIt++) {
				zval_ptr_dtor_nogc(deleteIt);
			}
			efree(circular_buffer);
			zend_throw_exception(spl_ce_UnexpectedValueException, "Collections\\Deque::__unserialize saw unexpected string key, expected sequence of values", 0);
			RETURN_THROWS();
		}
		ZVAL_COPY_DEREF(it++, val);
	} ZEND_HASH_FOREACH_END();
	ZEND_ASSERT(it <= circular_buffer + num_entries);

	array->size = it - circular_buffer;
	array->mask = capacity - 1;
	array->circular_buffer = circular_buffer;
}

static void collections_deque_entries_init_from_array_values(collections_deque_entries *array, zend_array *raw_data)
{
	uint32_t num_entries = zend_hash_num_elements(raw_data);
	if (num_entries == 0) {
		array->size = 0;
		array->mask = 0;
		array->circular_buffer = NULL;
		return;
	}
	const size_t capacity = collections_deque_next_pow2_capacity(num_entries);
	ZEND_ASSERT(capacity >= num_entries);
	zval * circular_buffer = safe_emalloc(capacity, sizeof(zval), 0);
	uint32_t actual_size = 0;
	zval *val;
	ZEND_HASH_FOREACH_VAL(raw_data, val) {
		ZVAL_COPY_DEREF(&circular_buffer[actual_size], val);
		actual_size++;
	} ZEND_HASH_FOREACH_END();

	ZEND_ASSERT(actual_size <= num_entries);

	array->circular_buffer = circular_buffer;
	array->size = actual_size;
	array->mask = capacity - 1;
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
}

PHP_METHOD(Collections_Deque, __set_state)
{
	zend_array *array_ht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(array_ht)
	ZEND_PARSE_PARAMETERS_END();
	zend_object *object = collections_deque_new(collections_ce_Deque);
	collections_deque *intern = collections_deque_from_object(object);
	collections_deque_entries_init_from_array_values(&intern->array, array_ht);

	RETURN_OBJ(object);
}

static zend_array* collections_deque_entries_to_refcounted_array(const collections_deque_entries *array) {
	ZEND_ASSERT(array->mask > 0);
	zval *const circular_buffer = array->circular_buffer;
	zval *p = circular_buffer + array->offset;
	zval *const end = circular_buffer + array->mask + 1;
	uint32_t len = array->size;
	zend_array *values = zend_new_array(len);
	/* Initialize return array */
	zend_hash_real_init_packed(values);

	/* Go through values and add values to the return array */
	ZEND_HASH_FILL_PACKED(values) {
		do {
			Z_TRY_ADDREF_P(p);
			ZEND_HASH_FILL_ADD(p);
			p++;
			if (p == end) {
				p = circular_buffer;
			}
			len--;
		} while (len > 0);
	} ZEND_HASH_FILL_END();
	return values;
}

PHP_METHOD(Collections_Deque, toArray)
{
	ZEND_PARSE_PARAMETERS_NONE();
	collections_deque *intern = Z_DEQUE_P(ZEND_THIS);
	uint32_t len = intern->array.size;
	if (!len) {
		RETURN_EMPTY_ARRAY();
	}
	RETURN_ARR(collections_deque_entries_to_refcounted_array(&intern->array));
}

static zend_always_inline void collections_deque_get_value_at_offset(zval *return_value, const zval *zval_this, zend_long offset)
{
	const collections_deque *intern = Z_DEQUE_P(zval_this);
	uint32_t len = intern->array.size;
	if (UNEXPECTED((zend_ulong) offset >= len)) {
		collections_throw_invalid_sequence_index_exception();
		RETURN_THROWS();
	}
	RETURN_COPY(collections_deque_get_entry_at_offset(&intern->array, offset));
}

PHP_METHOD(Collections_Deque, offsetGet)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	/**
	 * NOTE: converting offset to long may have side effects such as emitting notices that mutate the deque.
	 * Do that before getting the state of the Deque.
	 */
	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	collections_deque_get_value_at_offset(return_value, ZEND_THIS, offset);
}

PHP_METHOD(Collections_Deque, offsetExists)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	const collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);

	if ((zend_ulong) offset >= array->size) {
		RETURN_FALSE;
	}
	RETURN_BOOL(Z_TYPE_P(collections_deque_get_entry_at_offset(array, offset)) != IS_NULL);
}

PHP_METHOD(Collections_Deque, containsKey)
{
	zval *offset_zv;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	RETURN_LONG(((zend_ulong) offset) < Z_DEQUE_ENTRIES_P(ZEND_THIS)->size);
}

static zval *collections_deque_read_dimension(zend_object *object, zval *offset_zv, int type, zval *rv)
{
	if (UNEXPECTED(!offset_zv)) {
handle_missing_key:
		if (type != BP_VAR_IS) {
			collections_throw_invalid_sequence_index_exception();
		return NULL;
	}
		return &EG(uninitialized_zval);
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW_RETURN_NULLPTR(offset, offset_zv);

	const collections_deque *intern = collections_deque_from_object(object);

	(void) rv; /* rv is not used */

	if (UNEXPECTED(offset < 0 || (zend_ulong) offset >= intern->array.size)) {
		goto handle_missing_key;
	} else {
		return collections_deque_get_entry_at_offset(&intern->array, offset);
	}
}

static zend_always_inline void collections_deque_entries_set_value_at_offset(collections_deque_entries *array, zend_long offset, zval *value) {
	if (UNEXPECTED((zend_ulong) offset >= array->size)) {
		collections_throw_invalid_sequence_index_exception();
		return;
	}
	zval *const ptr = collections_deque_get_entry_at_offset(array, offset);
	zval tmp;
	ZVAL_COPY_VALUE(&tmp, ptr);
	ZVAL_COPY(ptr, value);
	zval_ptr_dtor(&tmp);
}

static zend_always_inline void collections_deque_entries_push_back(collections_deque_entries *array, zval *value) {
	const uint32_t old_size = array->size;
	const uint32_t old_mask = array->mask;
	const size_t old_capacity = old_mask ? old_mask + 1 : 0;

	if (old_size >= old_capacity) {
		ZEND_ASSERT(old_size == old_capacity);
		collections_deque_entries_raise_capacity(array, old_capacity ? old_capacity * 2 : COLLECTIONS_DEQUE_MIN_CAPACITY);
	}
	array->size++;
	zval *dest = collections_deque_get_entry_at_offset(array, old_size);
	ZVAL_COPY(dest, value);
}

static void collections_deque_write_dimension(zend_object *object, zval *offset_zv, zval *value)
{
	collections_deque_entries *array = collections_deque_entries_from_object(object);
	if (!offset_zv) {
		collections_deque_entries_push_back(array, value);
		return;
	}

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	if ((zend_ulong) offset >= array->size || offset < 0) {
		zend_throw_exception(spl_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	}
	ZVAL_DEREF(value);
	collections_deque_entries_set_value_at_offset(array, offset, value);
}

PHP_METHOD(Collections_Deque, offsetSet)
{
	zval                  *offset_zv, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(offset_zv)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	collections_deque_entries_set_value_at_offset(Z_DEQUE_ENTRIES_P(ZEND_THIS), offset, value);
}

/* Copies all entries in the circular buffer in source starting at offset to *destination. The caller sets the new_capacity after calling this. */
static void collections_deque_move_circular_buffer_to_new_buffer_of_capacity(collections_deque_entries *array, const size_t new_capacity)
{
	zval *const circular_buffer = array->circular_buffer;
	const uint32_t size = array->size;
	ZEND_ASSERT(array->mask > 0);
	const size_t old_capacity = array->mask + 1;
	const uint32_t first_len = old_capacity - array->offset;
	ZEND_ASSERT(new_capacity >= size);
	ZEND_ASSERT(old_capacity >= size);
	zval *new_entries = safe_emalloc(new_capacity, sizeof(zval), 0);
	/* There are 1 or 2 continuous segments of the circular buffer to copy to the start of the new circular buffer */
	if (size <= first_len) {
		memcpy(new_entries, circular_buffer + array->offset, size * sizeof(zval));
	} else {
		memcpy(new_entries, circular_buffer + array->offset, first_len * sizeof(zval));
		memcpy(new_entries + first_len, circular_buffer, (size - first_len) * sizeof(zval));
	}
	efree(circular_buffer);
	array->circular_buffer = new_entries;
	array->offset = 0;
}

static void collections_deque_entries_raise_capacity(collections_deque_entries *array, const size_t new_capacity)
{
	if (UNEXPECTED(new_capacity > COLLECTIONS_MAX_ZVAL_COLLECTION_SIZE)) {
		/* This is a fatal error, because userland code might expect any catchable throwable
		 * from userland, not from the internal implementation. */
		zend_error_noreturn(E_ERROR, "Exceeded max valid Collections\\Deque capacity");
		ZEND_UNREACHABLE();
	}
	const uint32_t old_mask = array->mask;
	ZEND_ASSERT(new_capacity > 0 && collections_is_valid_uint32_capacity(new_capacity));
	ZEND_ASSERT(new_capacity > old_mask + 1);
	if (collections_deque_entries_empty_capacity(array)) {
		array->circular_buffer = safe_emalloc(new_capacity, sizeof(zval), 0);
	} else if (array->offset + array->size <= old_mask + 1) {
		array->circular_buffer = safe_erealloc(array->circular_buffer, new_capacity, sizeof(zval), 0);
	} else {
		collections_deque_move_circular_buffer_to_new_buffer_of_capacity(array, new_capacity);
	}
	array->mask = new_capacity - 1;
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
}

static void collections_deque_entries_shrink_capacity(collections_deque_entries *array, uint32_t new_capacity)
{
	ZEND_ASSERT(collections_is_valid_uint32_capacity(new_capacity));
	ZEND_ASSERT(array->mask >= COLLECTIONS_DEQUE_MIN_MASK);
	ZEND_ASSERT(new_capacity < array->mask + 1);
	/* Callers leave some spare capacity for future additions */
	ZEND_ASSERT(new_capacity > COLLECTIONS_DEQUE_MIN_MASK);
	ZEND_ASSERT(!collections_deque_entries_empty_capacity(array));

	if (array->offset + array->size < new_capacity) {
		/* Shrink the array, probably without copying any data */
		array->circular_buffer = safe_erealloc(array->circular_buffer, new_capacity, sizeof(zval), 0);
	} else {
		collections_deque_move_circular_buffer_to_new_buffer_of_capacity(array, new_capacity);
	}
	array->mask = new_capacity - 1;
}


PHP_METHOD(Collections_Deque, push)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	uint32_t old_size = array->size;
	const size_t new_size = old_size + argc;
	uint32_t mask = array->mask;
	const uint32_t old_capacity = mask ? mask + 1 : 0;

	if (new_size > old_capacity) {
		const uint32_t new_capacity = collections_deque_next_pow2_capacity(new_size);
		collections_deque_entries_raise_capacity(array, new_capacity);
		mask = array->mask;
	}
	zval *const circular_buffer = array->circular_buffer;
	const uint32_t old_offset = array->offset;

	while (1) {
		zval *dest = &circular_buffer[(old_offset + old_size) & mask];
		ZVAL_COPY(dest, args);
		if (++old_size >= new_size) {
			break;
		}
		args++;
	}
	array->size = new_size;
}

static void collections_deque_adjust_iterators_before_remove(collections_deque_entries *array, collections_intrusive_dllist_node *node, const uint32_t removed_offset) {
	const zend_object *const obj = collections_deque_to_object(array);
	const uint32_t old_size = array->size;
	ZEND_ASSERT(removed_offset < old_size);
	do {
		collections_deque_it *it = collections_deque_it_from_node(node);
		if (Z_OBJ(it->intern.data) == obj) {
			if (it->current >= removed_offset && it->current < old_size) {
				it->current--;
			}
		}
		ZEND_ASSERT(node != node->next);
		node = node->next;
	} while (node != NULL);
}

static zend_always_inline void collections_deque_maybe_adjust_iterators_before_remove(collections_deque_entries *array, const uint32_t removed_offset)
{
	if (UNEXPECTED(array->active_iterators.first)) {
		collections_deque_adjust_iterators_before_remove(array, array->active_iterators.first, removed_offset);
	}
}

static void collections_deque_adjust_iterators_before_insert(collections_deque_entries *const array, collections_intrusive_dllist_node *node, const uint32_t inserted_offset, uint32_t n) {
	const zend_object *const obj = collections_deque_to_object(array);
	const uint32_t old_size = array->size;
	ZEND_ASSERT(inserted_offset <= old_size);
	do {
		collections_deque_it *it = collections_deque_it_from_node(node);
		if (Z_OBJ(it->intern.data) == obj) {
			if (it->current >= inserted_offset && it->current < old_size) {
				it->current += n;
			}
		}
		ZEND_ASSERT(node != node->next);
		node = node->next;
	} while (node != NULL);
}

static zend_always_inline void collections_deque_maybe_adjust_iterators_before_insert(collections_deque_entries *const array, const uint32_t inserted_offset, const uint32_t n)
{
	ZEND_ASSERT(inserted_offset <= array->size);
	if (UNEXPECTED(array->active_iterators.first)) {
		collections_deque_adjust_iterators_before_insert(array, array->active_iterators.first, inserted_offset, n);
	}
}

PHP_METHOD(Collections_Deque, unshift)
{
	const zval *args;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(argc == 0)) {
		return;
	}

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	collections_deque_maybe_adjust_iterators_before_insert(array, 0, argc);

	uint32_t old_size = array->size;
	const size_t new_size = old_size + argc;
	uint32_t mask = array->mask;
	const uint32_t old_capacity = mask ? mask + 1 : 0;

	if (new_size > old_capacity) {
		const size_t new_capacity = collections_deque_next_pow2_capacity(new_size);
		collections_deque_entries_raise_capacity(array, new_capacity);
		mask = array->mask;
	}
	uint32_t offset = array->offset;
	zval *const circular_buffer = array->circular_buffer;

	do {
		offset = (offset - 1) & mask;
		zval *dest = &circular_buffer[offset];
		ZVAL_COPY(dest, args);
		if (--argc == 0) {
			break;
		}
		args++;
	} while (1);

	array->offset = offset;
	array->size = new_size;

	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
}

static zend_always_inline void collections_deque_entries_insert_values(collections_deque_entries *const array, const uint32_t inserted_offset, uint32_t argc, const zval *args) {
	const uint32_t old_size = array->size;
	const size_t new_size = old_size + argc;
	uint32_t mask = array->mask;
	const uint32_t old_capacity = mask ? mask + 1 : 0;
	ZEND_ASSERT(argc > 0);

	if (new_size > old_capacity) {
		const size_t new_capacity = collections_deque_next_pow2_capacity(new_size);
		collections_deque_entries_raise_capacity(array, new_capacity);
		mask = array->mask;
	}
	const uint32_t offset = array->offset;
	zval *const circular_buffer = array->circular_buffer;

	collections_deque_maybe_adjust_iterators_before_insert(array, inserted_offset, argc);

	/* Move elements to the end of the deque */
	/* TODO move the start instead when there are less elements. */
	uint32_t src_offset = (offset + old_size) & mask; /* Masked in do-while loop. */
	uint32_t dst_offset = src_offset + argc;
	const uint32_t src_end = (offset + inserted_offset) & mask;

	while (src_offset != src_end) {
		src_offset = (src_offset - 1) & mask;
		dst_offset = (dst_offset - 1) & mask;
		ZEND_ASSERT(Z_TYPE(circular_buffer[src_offset]) != IS_UNDEF);
		ZVAL_COPY_VALUE(&circular_buffer[dst_offset], &circular_buffer[src_offset]);
	}

	dst_offset = (offset + inserted_offset) & mask;
	do {
		zval *dest = &circular_buffer[dst_offset];
		ZVAL_COPY(dest, args);
		if (--argc == 0) {
			break;
		}
		args++;
		dst_offset = (dst_offset + 1) & mask;
	} while (1);

	array->size = new_size;

	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
}

PHP_METHOD(Collections_Deque, insert)
{
	const zval *args;
	zend_long inserted_offset;
	uint32_t argc;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_LONG(inserted_offset)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	if (UNEXPECTED(((zend_ulong) inserted_offset) > array->size)) {
		collections_throw_invalid_sequence_index_exception();
		return;
	}
	ZEND_ASSERT(inserted_offset >= 0);

	if (UNEXPECTED(argc == 0)) {
		return;
	}

	collections_deque_entries_insert_values(array, inserted_offset, argc, args);
}

static zend_always_inline void collections_deque_entries_remove_offset(collections_deque_entries *const array, const uint32_t removed_offset) {
	const uint32_t old_size = array->size;
	uint32_t mask = array->mask;
	ZEND_ASSERT(removed_offset < old_size);

	const uint32_t offset = array->offset;
	zval *const circular_buffer = array->circular_buffer;

	/* Move elements from the end of the deque to replace the removed element */
	/* TODO: Remove from the front instead if there are fewer elements to remove, adjust iterators */
	uint32_t it_offset = (offset + removed_offset) & mask;

	collections_deque_maybe_adjust_iterators_before_remove(array, removed_offset);

	zval removed_val;
	ZVAL_COPY_VALUE(&removed_val, &circular_buffer[it_offset]);
	const uint32_t it_end = (offset + old_size - 1) & mask;
	ZEND_ASSERT(Z_TYPE(circular_buffer[it_offset]) != IS_UNDEF);

	while (it_offset != it_end) {
		const uint32_t next_offset = (it_offset + 1) & mask;
		ZEND_ASSERT(Z_TYPE(circular_buffer[next_offset]) != IS_UNDEF);
		ZVAL_COPY_VALUE(&circular_buffer[it_offset], &circular_buffer[next_offset]);
		it_offset = next_offset;
	}

	const uint32_t new_size = old_size - 1;
	array->size = new_size;
	collections_deque_entries_try_shrink_capacity(array, new_size);
	zval_ptr_dtor(&removed_val);

	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
}

PHP_METHOD(Collections_Deque, offsetUnset)
{
	zval *offset_zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(offset_zv)
	ZEND_PARSE_PARAMETERS_END();

	zend_long offset;
	CONVERT_OFFSET_TO_LONG_OR_THROW(offset, offset_zv);

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	const uint32_t old_size = array->size;
	if (UNEXPECTED((zend_ulong) offset >= old_size)) {
		COLLECTIONS_THROW_INVALID_SEQUENCE_INDEX_EXCEPTION();
	}
	collections_deque_entries_remove_offset(array, offset);
}

static zend_always_inline void collections_deque_entries_try_shrink_capacity(collections_deque_entries *array, uint32_t old_size)
{
	const uint32_t old_mask = array->mask;
	if (old_size - 1 <= ((old_mask) >> 2) && old_mask > COLLECTIONS_DEQUE_MIN_MASK) {
		collections_deque_entries_shrink_capacity(array, (old_mask >> 1) + 1);
	}
}

PHP_METHOD(Collections_Deque, pop)
{
	ZEND_PARSE_PARAMETERS_NONE();

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	const uint32_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot pop from empty Collections\\Deque", 0);
		RETURN_THROWS();
	}

	collections_deque_maybe_adjust_iterators_before_remove(array, old_size - 1);

	zval *val = collections_deque_get_entry_at_offset(array, old_size - 1);

	array->size--;
	/* This is being removed. Use a macro that doesn't change the total reference count. */
	RETVAL_COPY_VALUE(val);

	ZEND_ASSERT(array->mask >= COLLECTIONS_DEQUE_MIN_MASK);
	collections_deque_entries_try_shrink_capacity(array, old_size);
}

PHP_METHOD(Collections_Deque, last)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const collections_deque *intern = Z_DEQUE_P(ZEND_THIS);
	const uint32_t old_size = intern->array.size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read last value of empty Collections\\Deque", 0);
		RETURN_THROWS();
	}

	/* This is being copied. Use a macro that increases the total reference count. */
	RETVAL_COPY(collections_deque_get_entry_at_offset(&intern->array, old_size - 1));
}

PHP_METHOD(Collections_Deque, shift)
{
	ZEND_PARSE_PARAMETERS_NONE();

	collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
	const uint32_t old_size = array->size;
	if (old_size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot shift from empty Collections\\Deque", 0);
		RETURN_THROWS();
	}
	collections_deque_maybe_adjust_iterators_before_remove(array, 0);

	array->size--;
	const uint32_t old_offset = array->offset;
	const uint32_t old_mask = array->mask;
	array->offset = (old_offset + 1) & old_mask;
	RETVAL_COPY_VALUE(&array->circular_buffer[old_offset]);
	collections_deque_entries_try_shrink_capacity(array, old_size);
}

PHP_METHOD(Collections_Deque, first)
{
	ZEND_PARSE_PARAMETERS_NONE();

	const collections_deque_entries *array = Z_DEQUE_ENTRIES_P(ZEND_THIS);
	DEBUG_ASSERT_CONSISTENT_DEQUE(array);
	if (array->size == 0) {
		zend_throw_exception(spl_ce_UnderflowException, "Cannot read first value of empty Collections\\Deque", 0);
		RETURN_THROWS();
	}

	RETVAL_COPY(&array->circular_buffer[array->offset]);
}

PHP_MINIT_FUNCTION(collections_deque)
{
	collections_ce_Deque = register_class_Collections_Deque(zend_ce_aggregate, zend_ce_countable, php_json_serializable_ce, zend_ce_arrayaccess);
	collections_ce_Deque->create_object = collections_deque_new;

	memcpy(&collections_handler_Deque, &std_object_handlers, sizeof(zend_object_handlers));

	collections_handler_Deque.offset          = XtOffsetOf(collections_deque, std);
	collections_handler_Deque.clone_obj       = collections_deque_clone;
	collections_handler_Deque.count_elements  = collections_deque_count_elements;
	/* Deliberately use default get_properties implementation for infinite recursion detection, creating empty table if one didn't exist. */
	collections_handler_Deque.get_properties_for  = collections_deque_get_properties_for;
	collections_handler_Deque.get_gc          = collections_deque_get_gc;
	collections_handler_Deque.dtor_obj        = zend_objects_destroy_object;
	collections_handler_Deque.free_obj        = collections_deque_free_storage;

	collections_handler_Deque.read_dimension  = collections_deque_read_dimension;
	collections_handler_Deque.write_dimension = collections_deque_write_dimension;
	//collections_handler_Deque.unset_dimension = collections_deque_unset_dimension;
	//collections_handler_Deque.has_dimension   = collections_deque_has_dimension;

	collections_ce_Deque->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	collections_ce_Deque->get_iterator = collections_deque_get_iterator;

	return SUCCESS;
}
