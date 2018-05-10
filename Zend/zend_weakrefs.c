/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 Zend Technologies Ltd. (http://www.zend.com) |
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

/* $Id$ */

#include "zend.h"
#include "zend_interfaces.h"
#include "zend_objects_API.h"
#include "zend_weakrefs.h"

typedef struct _zend_weakref {
	zval referent;
	zend_array *root;
	zend_object std;
} zend_weakref;

zend_class_entry *zend_ce_weakref;
zend_object_handlers zend_weakref_handlers;

#define zend_weakref_from(o) ((zend_weakref*)(((char*) o) - XtOffsetOf(zend_weakref, std)))
#define zend_weakref_fetch(z) zend_weakref_from(Z_OBJ_P(z))

static inline zend_array* zend_weakrefs_fetch(zend_object *ro, zend_bool create);

static void zend_weakrefs_dtor(zval *zv) {
	zend_hash_destroy(Z_PTR_P(zv));
	FREE_HASHTABLE(Z_PTR_P(zv));
}

void zend_weakrefs_init() {
	zend_hash_init(&EG(weakrefs), 8, NULL, zend_weakrefs_dtor, 0);
}

static void zend_weakref_unref(zval *zv) {
	zend_weakref *wr = (zend_weakref*) Z_PTR_P(zv);

	ZVAL_NULL(&wr->referent);

	wr->root = NULL;
}

void zend_weakrefs_notify(zend_object *object) {
	zend_array *refs = zend_weakrefs_fetch(object, 0);

	if (!refs) {
		return;
	}

	zend_hash_index_del(&EG(weakrefs), (zend_ulong) object);
}

void zend_weakrefs_shutdown() {
	zend_hash_destroy(&EG(weakrefs));
}

static zend_object* zend_weakref_new(zend_class_entry *ce) {
	zend_weakref *wr =
		(zend_weakref*) 
			zend_object_alloc(sizeof(zend_weakref), ce);

	zend_object_std_init(&wr->std, ce);

	wr->std.handlers = &zend_weakref_handlers;

	ZVAL_NULL(&wr->referent);

	return &wr->std;
}

static zend_object* zend_weakref_clone(zval *zv) {
	zval rv;
	zend_weakref *wr = zend_weakref_fetch(zv);

	object_init_ex(&rv, Z_OBJCE_P(zv));

	zend_weakref_construct(&rv, &wr->referent);

	return Z_OBJ(rv);
}

static void zend_weakref_free(zend_object *zo) {
	zend_weakref *wr = zend_weakref_from(zo);

	if (!wr->root) {
		zend_object_std_dtor(&wr->std);
		return;
	}

	zend_hash_index_del(wr->root, (zend_ulong) wr);
	zend_object_std_dtor(&wr->std);
}

static inline zend_array* zend_weakrefs_fetch(zend_object *ro, zend_bool create) {
	zend_array *refs = zend_hash_index_find_ptr(&EG(weakrefs), (zend_ulong) ro);

	if (!refs && create) {
		ALLOC_HASHTABLE(refs);
		zend_hash_init(refs, 8, NULL, zend_weakref_unref, 0);
		zend_hash_index_update_ptr(&EG(weakrefs), (zend_ulong) ro, refs);
	}

	return refs;
}

void zend_weakref_construct(zval *weakref, zval *referent) {
	zend_weakref *wr = zend_weakref_fetch(weakref);

	wr->root = zend_weakrefs_fetch(Z_OBJ_P(referent), 1);

	zend_hash_index_add_ptr(wr->root, (zend_ulong) wr, wr);

	ZVAL_COPY_VALUE(&wr->referent, referent);

	GC_ADD_FLAGS(Z_OBJ_P(referent), IS_OBJ_WEAKLY_REFERENCED);
}

#define zend_weakref_unsupported(thing) \
	zend_throw_error(NULL, "weakref objects do not support " thing);

static void zend_weakref_no_write(zval *object, zval *member, zval *value, void **rtc) {
	zend_weakref_unsupported("properties");
}

static zval* zend_weakref_no_read(zval *object, zval *member, int type, void **rtc, zval *rv) {
	if (!EG(exception)) {
		zend_weakref_unsupported("properties");
	}

	return &EG(uninitialized_zval);
}

static zval *zend_weakref_no_read_ptr(zval *object, zval *member, int type, void **rtc) {
	zend_weakref_unsupported("property references");
	return NULL;
}

static int zend_weakref_no_isset(zval *object, zval *member, int hse, void **rtc) {
	if (hse != 2) {
		zend_weakref_unsupported("properties");
	}
	return 0;
}

static void zend_weakref_no_unset(zval *object, zval *member, void **rtc) {
	zend_weakref_unsupported("properties");
}

ZEND_BEGIN_ARG_INFO_EX(zend_weakref_construct_arginfo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, referent, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(zend_weakref_get_arginfo, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

ZEND_METHOD(WeakRef, __construct)
{
	zval *referent;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "o", &referent) != SUCCESS) {
		return;
	}

	zend_weakref_construct(getThis(), referent);
}

ZEND_METHOD(WeakRef, get)
{
	zend_weakref *wr;

	if (zend_parse_parameters_none_throw() != SUCCESS) {
		return;
	}

	wr = zend_weakref_fetch(getThis());

	ZVAL_COPY(return_value, &wr->referent);
}

static const zend_function_entry zend_weakref_methods[] = {
	ZEND_ME(WeakRef, __construct, zend_weakref_construct_arginfo, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakRef, get, zend_weakref_get_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_weakref_ce(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "WeakRef", zend_weakref_methods);
	zend_ce_weakref = zend_register_internal_class(&ce);
	zend_ce_weakref->ce_flags |= ZEND_ACC_FINAL;

	zend_ce_weakref->create_object = zend_weakref_new;
	zend_ce_weakref->serialize = zend_class_serialize_deny;
	zend_ce_weakref->unserialize = zend_class_unserialize_deny;

	memcpy(&zend_weakref_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	zend_weakref_handlers.offset = XtOffsetOf(zend_weakref, std);

	zend_weakref_handlers.free_obj = zend_weakref_free;
	zend_weakref_handlers.read_property = zend_weakref_no_read;
	zend_weakref_handlers.write_property = zend_weakref_no_write;
	zend_weakref_handlers.has_property = zend_weakref_no_isset;
	zend_weakref_handlers.unset_property = zend_weakref_no_unset;
	zend_weakref_handlers.get_property_ptr_ptr = zend_weakref_no_read_ptr;

	zend_weakref_handlers.clone_obj = zend_weakref_clone;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
