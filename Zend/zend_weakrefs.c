/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: krakjoe@php.net                                             |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_interfaces.h"
#include "zend_objects_API.h"
#include "zend_weakrefs.h"
#include "zend_weakrefs_arginfo.h"

typedef struct _zend_weakref {
	zend_object *referent;
	zend_object std;
} zend_weakref;

typedef struct _zend_weakmap {
	HashTable ht;
	zend_object std;
} zend_weakmap;

typedef struct _zend_weakmap_iterator {
	zend_object_iterator it;
	uint32_t ht_iter;
} zend_weakmap_iterator;

/* The EG(weakrefs) ht is a map from object address a tagged pointer, that may be one of
 * zend_weakref*, zend_weakmap* or HashTable*. */
#define ZEND_WEAKREF_TAG_REF 0
#define ZEND_WEAKREF_TAG_MAP 1
#define ZEND_WEAKREF_TAG_HT  2
#define ZEND_WEAKREF_GET_TAG(p) (((uintptr_t) (p)) & 3)
#define ZEND_WEAKREF_GET_PTR(p) ((void *) (((uintptr_t) (p)) & ~3))
#define ZEND_WEAKREF_ENCODE(p, t) ((void *) (((uintptr_t) (p)) | (t)))

zend_class_entry *zend_ce_weakref;
zend_class_entry *zend_ce_weakmap;
static zend_object_handlers zend_weakref_handlers;
static zend_object_handlers zend_weakmap_handlers;

#define zend_weakref_from(o) ((zend_weakref*)(((char*) o) - XtOffsetOf(zend_weakref, std)))
#define zend_weakref_fetch(z) zend_weakref_from(Z_OBJ_P(z))

#define zend_weakmap_from(o) ((zend_weakmap*)(((char*) o) - XtOffsetOf(zend_weakmap, std)))
#define zend_weakmap_fetch(z) zend_weakmap_from(Z_OBJ_P(z))

static inline void zend_weakref_unref_single(
		void *ptr, uintptr_t tag, zend_ulong obj_addr)
{
	if (tag == ZEND_WEAKREF_TAG_REF) {
		zend_weakref *wr = ptr;
		wr->referent = NULL;
	} else {
		ZEND_ASSERT(tag == ZEND_WEAKREF_TAG_MAP);
		zend_hash_index_del((HashTable *) ptr, obj_addr);
	}
}

static void zend_weakref_unref(zend_ulong obj_addr, void *tagged_ptr) {
	void *ptr = ZEND_WEAKREF_GET_PTR(tagged_ptr);
	uintptr_t tag = ZEND_WEAKREF_GET_TAG(tagged_ptr);
	if (tag == ZEND_WEAKREF_TAG_HT) {
		HashTable *ht = ptr;
		ZEND_HASH_FOREACH_PTR(ht, tagged_ptr) {
			zend_weakref_unref_single(
				ZEND_WEAKREF_GET_PTR(tagged_ptr), ZEND_WEAKREF_GET_TAG(tagged_ptr), obj_addr);
		} ZEND_HASH_FOREACH_END();
		zend_hash_destroy(ht);
		FREE_HASHTABLE(ht);
	} else {
		zend_weakref_unref_single(ptr, tag, obj_addr);
	}
}

static void zend_weakref_register(zend_object *object, void *payload) {
	GC_ADD_FLAGS(object, IS_OBJ_WEAKLY_REFERENCED);

	zend_ulong obj_addr = (zend_ulong) object;
	zval *zv = zend_hash_index_find(&EG(weakrefs), obj_addr);
	if (!zv) {
		zend_hash_index_add_new_ptr(&EG(weakrefs), obj_addr, payload);
		return;
	}

	void *tagged_ptr = Z_PTR_P(zv);
	if (ZEND_WEAKREF_GET_TAG(tagged_ptr) == ZEND_WEAKREF_TAG_HT) {
		HashTable *ht = ZEND_WEAKREF_GET_PTR(tagged_ptr);
		zend_hash_index_add_new_ptr(ht, (zend_ulong) payload, payload);
		return;
	}

	/* Convert simple pointer to hashtable. */
	HashTable *ht = emalloc(sizeof(HashTable));
	zend_hash_init(ht, 0, NULL, NULL, 0);
	zend_hash_index_add_new_ptr(ht, (zend_ulong) tagged_ptr, tagged_ptr);
	zend_hash_index_add_new_ptr(ht, (zend_ulong) payload, payload);
	zend_hash_index_update_ptr(
		&EG(weakrefs), obj_addr, ZEND_WEAKREF_ENCODE(ht, ZEND_WEAKREF_TAG_HT));
}

static void zend_weakref_unregister(zend_object *object, void *payload) {
	zend_ulong obj_addr = (zend_ulong) object;
	void *tagged_ptr = zend_hash_index_find_ptr(&EG(weakrefs), obj_addr);
	ZEND_ASSERT(tagged_ptr && "Weakref not registered?");

	void *ptr = ZEND_WEAKREF_GET_PTR(tagged_ptr);
	uintptr_t tag = ZEND_WEAKREF_GET_TAG(tagged_ptr);
	if (tag != ZEND_WEAKREF_TAG_HT) {
		ZEND_ASSERT(tagged_ptr == payload);
		zend_hash_index_del(&EG(weakrefs), obj_addr);
		GC_DEL_FLAGS(object, IS_OBJ_WEAKLY_REFERENCED);

		/* Do this last, as it may destroy the object. */
		zend_weakref_unref_single(ptr, tag, obj_addr);
		return;
	}

	HashTable *ht = ptr;
	tagged_ptr = zend_hash_index_find_ptr(ht, (zend_ulong) payload);
	ZEND_ASSERT(tagged_ptr && "Weakref not registered?");
	ZEND_ASSERT(tagged_ptr == payload);
	zend_hash_index_del(ht, (zend_ulong) payload);
	if (zend_hash_num_elements(ht) == 0) {
		GC_DEL_FLAGS(object, IS_OBJ_WEAKLY_REFERENCED);
		zend_hash_destroy(ht);
		FREE_HASHTABLE(ht);
		zend_hash_index_del(&EG(weakrefs), obj_addr);
	}

	/* Do this last, as it may destroy the object. */
	zend_weakref_unref_single(
		ZEND_WEAKREF_GET_PTR(payload), ZEND_WEAKREF_GET_TAG(payload), obj_addr);
}

ZEND_API zval *zend_weakrefs_hash_add(HashTable *ht, zend_object *key, zval *pData) {
	zval *zv = zend_hash_index_add(ht, (zend_ulong) key, pData);
	if (zv) {
		zend_weakref_register(key, ZEND_WEAKREF_ENCODE(ht, ZEND_WEAKREF_TAG_MAP));
	}
	return zv;
}

ZEND_API zend_result zend_weakrefs_hash_del(HashTable *ht, zend_object *key) {
	zval *zv = zend_hash_index_find(ht, (zend_ulong) key);
	if (zv) {
		zend_weakref_unregister(key, ZEND_WEAKREF_ENCODE(ht, ZEND_WEAKREF_TAG_MAP));
		return SUCCESS;
	}
	return FAILURE;
}

void zend_weakrefs_init(void) {
	zend_hash_init(&EG(weakrefs), 8, NULL, NULL, 0);
}

void zend_weakrefs_notify(zend_object *object) {
	/* Annoyingly we can't use the HT destructor here, because we need access to the key (which
	 * is the object address), which is not provided to the dtor. */
	zend_ulong obj_addr = (zend_ulong) object;
	void *tagged_ptr = zend_hash_index_find_ptr(&EG(weakrefs), obj_addr);
#if ZEND_DEBUG
	ZEND_ASSERT(tagged_ptr && "Tracking of the IS_OBJ_WEAKLY_REFERENCE flag should be precise");
#endif
	if (tagged_ptr) {
		zend_weakref_unref(obj_addr, tagged_ptr);
		zend_hash_index_del(&EG(weakrefs), obj_addr);
	}
}

void zend_weakrefs_shutdown(void) {
	zend_ulong obj_addr;
	void *tagged_ptr;
	ZEND_HASH_FOREACH_NUM_KEY_PTR(&EG(weakrefs), obj_addr, tagged_ptr) {
		zend_weakref_unref(obj_addr, tagged_ptr);
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&EG(weakrefs));
}

static zend_object* zend_weakref_new(zend_class_entry *ce) {
	zend_weakref *wr = zend_object_alloc(sizeof(zend_weakref), zend_ce_weakref);

	zend_object_std_init(&wr->std, zend_ce_weakref);

	wr->std.handlers = &zend_weakref_handlers;

	return &wr->std;
}

static zend_always_inline zend_bool zend_weakref_find(zval *referent, zval *return_value) {
	void *tagged_ptr = zend_hash_index_find_ptr(&EG(weakrefs), (zend_ulong) Z_OBJ_P(referent));
	if (!tagged_ptr) {
		return 0;
	}

	void *ptr = ZEND_WEAKREF_GET_PTR(tagged_ptr);
	uintptr_t tag = ZEND_WEAKREF_GET_TAG(tagged_ptr);
	if (tag == ZEND_WEAKREF_TAG_REF) {
		zend_weakref *wr;
found_weakref:
		wr = ptr;
		RETVAL_OBJ_COPY(&wr->std);
		return 1;
	}

	if (tag == ZEND_WEAKREF_TAG_HT) {
		ZEND_HASH_FOREACH_PTR(ptr, tagged_ptr) {
			if (ZEND_WEAKREF_GET_TAG(tagged_ptr) == ZEND_WEAKREF_TAG_REF) {
				ptr = ZEND_WEAKREF_GET_PTR(tagged_ptr);
				goto found_weakref;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return 0;
}

static zend_always_inline void zend_weakref_create(zval *referent, zval *return_value) {
	zend_weakref *wr;

	object_init_ex(return_value, zend_ce_weakref);

	wr = zend_weakref_fetch(return_value);
	wr->referent = Z_OBJ_P(referent);

	zend_weakref_register(wr->referent, ZEND_WEAKREF_ENCODE(wr, ZEND_WEAKREF_TAG_REF));
}

static zend_always_inline void zend_weakref_get(zval *weakref, zval *return_value) {
	zend_weakref *wr = zend_weakref_fetch(weakref);

	if (wr->referent) {
		RETVAL_OBJ_COPY(wr->referent);
	}
}

static void zend_weakref_free(zend_object *zo) {
	zend_weakref *wr = zend_weakref_from(zo);

	if (wr->referent) {
		zend_weakref_unregister(wr->referent, ZEND_WEAKREF_ENCODE(wr, ZEND_WEAKREF_TAG_REF));
	}

	zend_object_std_dtor(&wr->std);
}

ZEND_COLD ZEND_METHOD(WeakReference, __construct)
{
	zend_throw_error(NULL, "Direct instantiation of WeakReference is not allowed, use WeakReference::create instead");
}

ZEND_METHOD(WeakReference, create)
{
	zval *referent;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_OBJECT(referent)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_weakref_find(referent, return_value)) {
	    return;
	}

	zend_weakref_create(referent, return_value);
}

ZEND_METHOD(WeakReference, get)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_weakref_get(getThis(), return_value);
}

static zend_object *zend_weakmap_create_object(zend_class_entry *ce)
{
	zend_weakmap *wm = zend_object_alloc(sizeof(zend_weakmap), ce);
	zend_object_std_init(&wm->std, ce);
	wm->std.handlers = &zend_weakmap_handlers;

	zend_hash_init(&wm->ht, 0, NULL, ZVAL_PTR_DTOR, 0);
	return &wm->std;
}

static void zend_weakmap_free_obj(zend_object *object)
{
	zend_weakmap *wm = zend_weakmap_from(object);
	zend_ulong obj_addr;
	ZEND_HASH_FOREACH_NUM_KEY(&wm->ht, obj_addr) {
		zend_weakref_unregister(
			(zend_object *) obj_addr, ZEND_WEAKREF_ENCODE(&wm->ht, ZEND_WEAKREF_TAG_MAP));
	} ZEND_HASH_FOREACH_END();
	zend_hash_destroy(&wm->ht);
	zend_object_std_dtor(&wm->std);
}

static zval *zend_weakmap_read_dimension(zend_object *object, zval *offset, int type, zval *rv)
{
	if (offset == NULL) {
		zend_throw_error(NULL, "Cannot append to WeakMap");
		return NULL;
	}

	ZVAL_DEREF(offset);
	if (Z_TYPE_P(offset) != IS_OBJECT) {
		zend_type_error("WeakMap key must be an object");
		return NULL;
	}

	zend_weakmap *wm = zend_weakmap_from(object);
	zend_object *obj_key = Z_OBJ_P(offset);
	zval *zv = zend_hash_index_find(&wm->ht, (zend_ulong) obj_key);
	if (zv == NULL) {
		if (type != BP_VAR_IS) {
			zend_throw_error(NULL,
				"Object %s#%d not contained in WeakMap", ZSTR_VAL(obj_key->ce->name), obj_key->handle);
			return NULL;
		}
		return NULL;
	}

	if (type == BP_VAR_W || type == BP_VAR_RW) {
		ZVAL_MAKE_REF(zv);
	}
	return zv;
}

static void zend_weakmap_write_dimension(zend_object *object, zval *offset, zval *value)
{
	if (offset == NULL) {
		zend_throw_error(NULL, "Cannot append to WeakMap");
		return;
	}

	ZVAL_DEREF(offset);
	if (Z_TYPE_P(offset) != IS_OBJECT) {
		zend_type_error("WeakMap key must be an object");
		return;
	}

	zend_weakmap *wm = zend_weakmap_from(object);
	zend_object *obj_key = Z_OBJ_P(offset);
	Z_TRY_ADDREF_P(value);

	zval *zv = zend_hash_index_find(&wm->ht, (zend_ulong) obj_key);
	if (zv) {
		/* Because the destructors can have side effects such as resizing or rehashing the WeakMap storage,
		 * free the zval only after overwriting the original value. */
		zval zv_orig;
		ZVAL_COPY_VALUE(&zv_orig, zv);
		ZVAL_COPY_VALUE(zv, value);
		zval_ptr_dtor(&zv_orig);
		return;
	}

	zend_weakref_register(obj_key, ZEND_WEAKREF_ENCODE(&wm->ht, ZEND_WEAKREF_TAG_MAP));
	zend_hash_index_add_new(&wm->ht, (zend_ulong) obj_key, value);
}

/* int return and check_empty due to Object Handler API */
static int zend_weakmap_has_dimension(zend_object *object, zval *offset, int check_empty)
{
	ZVAL_DEREF(offset);
	if (Z_TYPE_P(offset) != IS_OBJECT) {
		zend_type_error("WeakMap key must be an object");
		return 0;
	}

	zend_weakmap *wm = zend_weakmap_from(object);
	zval *zv = zend_hash_index_find(&wm->ht, (zend_ulong) Z_OBJ_P(offset));
	if (!zv) {
		return 0;
	}

	if (check_empty) {
		return i_zend_is_true(zv);
	}
	return Z_TYPE_P(zv) != IS_NULL;
}

static void zend_weakmap_unset_dimension(zend_object *object, zval *offset)
{
	ZVAL_DEREF(offset);
	if (Z_TYPE_P(offset) != IS_OBJECT) {
		zend_type_error("WeakMap key must be an object");
		return;
	}

	zend_weakmap *wm = zend_weakmap_from(object);
	zend_object *obj_key = Z_OBJ_P(offset);
	if (!zend_hash_index_exists(&wm->ht, (zend_ulong) Z_OBJ_P(offset))) {
		/* Object not in WeakMap, do nothing. */
		return;
	}

	zend_weakref_unregister(obj_key, ZEND_WEAKREF_ENCODE(&wm->ht, ZEND_WEAKREF_TAG_MAP));
}

static int zend_weakmap_count_elements(zend_object *object, zend_long *count)
{
	zend_weakmap *wm = zend_weakmap_from(object);
	*count = zend_hash_num_elements(&wm->ht);
	return SUCCESS;
}

static HashTable *zend_weakmap_get_properties_for(zend_object *object, zend_prop_purpose purpose)
{
	if (purpose != ZEND_PROP_PURPOSE_DEBUG) {
		return NULL;
	}

	zend_weakmap *wm = zend_weakmap_from(object);
	HashTable *ht;
	ALLOC_HASHTABLE(ht);
	zend_hash_init(ht, zend_hash_num_elements(&wm->ht), NULL, ZVAL_PTR_DTOR, 0);

	zend_ulong obj_addr;
	zval *val;
	ZEND_HASH_FOREACH_NUM_KEY_VAL(&wm->ht, obj_addr, val) {
		zval pair;
		zval obj_zv;
		array_init(&pair);

		ZVAL_OBJ_COPY(&obj_zv, (zend_object *) obj_addr);
		add_assoc_zval(&pair, "key", &obj_zv);
		Z_TRY_ADDREF_P(val);
		add_assoc_zval(&pair, "value", val);

		zend_hash_next_index_insert(ht, &pair);
	} ZEND_HASH_FOREACH_END();

	return ht;
}

static HashTable *zend_weakmap_get_gc(zend_object *object, zval **table, int *n)
{
	zend_weakmap *wm = zend_weakmap_from(object);
	*table = NULL;
	*n = 0;
	return &wm->ht;
}

static zend_object *zend_weakmap_clone_obj(zend_object *old_object)
{
	zend_object *new_object = zend_weakmap_create_object(zend_ce_weakmap);
	zend_weakmap *old_wm = zend_weakmap_from(old_object);
	zend_weakmap *new_wm = zend_weakmap_from(new_object);
	zend_hash_copy(&new_wm->ht, &old_wm->ht, NULL);

	zend_ulong obj_addr;
	zval *val;
	ZEND_HASH_FOREACH_NUM_KEY_VAL(&new_wm->ht, obj_addr, val) {
		zend_weakref_register(
			(zend_object *) obj_addr, ZEND_WEAKREF_ENCODE(new_wm, ZEND_WEAKREF_TAG_MAP));
		zval_add_ref(val);
	} ZEND_HASH_FOREACH_END();
	return new_object;
}

static HashPosition *zend_weakmap_iterator_get_pos_ptr(zend_weakmap_iterator *iter) {
	ZEND_ASSERT(iter->ht_iter != (uint32_t) -1);
	return &EG(ht_iterators)[iter->ht_iter].pos;
}

static void zend_weakmap_iterator_dtor(zend_object_iterator *obj_iter)
{
	zend_weakmap_iterator *iter = (zend_weakmap_iterator *) obj_iter;
	zend_hash_iterator_del(iter->ht_iter);
	zval_ptr_dtor(&iter->it.data);
}

static int zend_weakmap_iterator_valid(zend_object_iterator *obj_iter)
{
	zend_weakmap_iterator *iter = (zend_weakmap_iterator *) obj_iter;
	zend_weakmap *wm = zend_weakmap_fetch(&iter->it.data);
	HashPosition *pos = zend_weakmap_iterator_get_pos_ptr(iter);
	return zend_hash_has_more_elements_ex(&wm->ht, pos);
}

static zval *zend_weakmap_iterator_get_current_data(zend_object_iterator *obj_iter)
{
	zend_weakmap_iterator *iter = (zend_weakmap_iterator *) obj_iter;
	zend_weakmap *wm = zend_weakmap_fetch(&iter->it.data);
	HashPosition *pos = zend_weakmap_iterator_get_pos_ptr(iter);
	return zend_hash_get_current_data_ex(&wm->ht, pos);
}

static void zend_weakmap_iterator_get_current_key(zend_object_iterator *obj_iter, zval *key)
{
	zend_weakmap_iterator *iter = (zend_weakmap_iterator *) obj_iter;
	zend_weakmap *wm = zend_weakmap_fetch(&iter->it.data);
	HashPosition *pos = zend_weakmap_iterator_get_pos_ptr(iter);

	zend_string *string_key;
	zend_ulong num_key;
	int key_type = zend_hash_get_current_key_ex(&wm->ht, &string_key, &num_key, pos);
	if (key_type != HASH_KEY_IS_LONG) {
		ZEND_ASSERT(0 && "Must have integer key");
	}

	ZVAL_OBJ_COPY(key, (zend_object *) num_key);
}

static void zend_weakmap_iterator_move_forward(zend_object_iterator *obj_iter)
{
	zend_weakmap_iterator *iter = (zend_weakmap_iterator *) obj_iter;
	zend_weakmap *wm = zend_weakmap_fetch(&iter->it.data);
	HashPosition *pos = zend_weakmap_iterator_get_pos_ptr(iter);
	zend_hash_move_forward_ex(&wm->ht, pos);
}

static void zend_weakmap_iterator_rewind(zend_object_iterator *obj_iter)
{
	zend_weakmap_iterator *iter = (zend_weakmap_iterator *) obj_iter;
	zend_weakmap *wm = zend_weakmap_fetch(&iter->it.data);
	HashPosition *pos = zend_weakmap_iterator_get_pos_ptr(iter);
	zend_hash_internal_pointer_reset_ex(&wm->ht, pos);
}

static const zend_object_iterator_funcs zend_weakmap_iterator_funcs = {
	zend_weakmap_iterator_dtor,
	zend_weakmap_iterator_valid,
	zend_weakmap_iterator_get_current_data,
	zend_weakmap_iterator_get_current_key,
	zend_weakmap_iterator_move_forward,
	zend_weakmap_iterator_rewind,
	NULL,
	NULL, /* get_gc */
};

/* by_ref is int due to Iterator API */
static zend_object_iterator *zend_weakmap_get_iterator(
		zend_class_entry *ce, zval *object, int by_ref)
{
	zend_weakmap *wm = zend_weakmap_fetch(object);
	zend_weakmap_iterator *iter = emalloc(sizeof(zend_weakmap_iterator));
	zend_iterator_init(&iter->it);
	iter->it.funcs = &zend_weakmap_iterator_funcs;
	ZVAL_COPY(&iter->it.data, object);
	iter->ht_iter = zend_hash_iterator_add(&wm->ht, 0);
	return &iter->it;
}

ZEND_METHOD(WeakMap, offsetGet)
{
	zval *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
		return;
	}

	zval *zv = zend_weakmap_read_dimension(Z_OBJ_P(ZEND_THIS), key, BP_VAR_R, NULL);
	if (!zv) {
		return;
	}

	ZVAL_COPY(return_value, zv);
}

ZEND_METHOD(WeakMap, offsetSet)
{
	zval *key, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &key, &value) == FAILURE) {
		return;
	}

	zend_weakmap_write_dimension(Z_OBJ_P(ZEND_THIS), key, value);
}

ZEND_METHOD(WeakMap, offsetExists)
{
	zval *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
		return;
	}

	RETURN_BOOL(zend_weakmap_has_dimension(Z_OBJ_P(ZEND_THIS), key, /* check_empty */ 0));
}

ZEND_METHOD(WeakMap, offsetUnset)
{
	zval *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &key) == FAILURE) {
		return;
	}

	zend_weakmap_unset_dimension(Z_OBJ_P(ZEND_THIS), key);
}

ZEND_METHOD(WeakMap, count)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_long count;
	zend_weakmap_count_elements(Z_OBJ_P(ZEND_THIS), &count);
	RETURN_LONG(count);
}

ZEND_METHOD(WeakMap, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

void zend_register_weakref_ce(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "WeakReference", class_WeakReference_methods);
	zend_ce_weakref = zend_register_internal_class(&ce);
	zend_ce_weakref->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zend_ce_weakref->create_object = zend_weakref_new;
	zend_ce_weakref->serialize = zend_class_serialize_deny;
	zend_ce_weakref->unserialize = zend_class_unserialize_deny;

	memcpy(&zend_weakref_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_weakref_handlers.offset = XtOffsetOf(zend_weakref, std);

	zend_weakref_handlers.free_obj = zend_weakref_free;
	zend_weakref_handlers.clone_obj = NULL;

	INIT_CLASS_ENTRY(ce, "WeakMap", class_WeakMap_methods);
	zend_ce_weakmap = zend_register_internal_class(&ce);
	zend_ce_weakmap->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
	zend_class_implements(
		zend_ce_weakmap, 3, zend_ce_arrayaccess, zend_ce_countable, zend_ce_aggregate);

	zend_ce_weakmap->create_object = zend_weakmap_create_object;
	zend_ce_weakmap->get_iterator = zend_weakmap_get_iterator;
	zend_ce_weakmap->serialize = zend_class_serialize_deny;
	zend_ce_weakmap->unserialize = zend_class_unserialize_deny;

	memcpy(&zend_weakmap_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_weakmap_handlers.offset = XtOffsetOf(zend_weakmap, std);
	zend_weakmap_handlers.free_obj = zend_weakmap_free_obj;
	zend_weakmap_handlers.read_dimension = zend_weakmap_read_dimension;
	zend_weakmap_handlers.write_dimension = zend_weakmap_write_dimension;
	zend_weakmap_handlers.has_dimension = zend_weakmap_has_dimension;
	zend_weakmap_handlers.unset_dimension = zend_weakmap_unset_dimension;
	zend_weakmap_handlers.count_elements = zend_weakmap_count_elements;
	zend_weakmap_handlers.get_properties_for = zend_weakmap_get_properties_for;
	zend_weakmap_handlers.get_gc = zend_weakmap_get_gc;
	zend_weakmap_handlers.clone_obj = zend_weakmap_clone_obj;
}
/* }}} */

