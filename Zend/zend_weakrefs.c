/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

typedef struct _zend_weakref {
	zend_object *referent;
	zend_object std;
} zend_weakref;

zend_class_entry *zend_ce_weakref;
static zend_object_handlers zend_weakref_handlers;

#define zend_weakref_from(o) ((zend_weakref*)(((char*) o) - XtOffsetOf(zend_weakref, std)))
#define zend_weakref_fetch(z) zend_weakref_from(Z_OBJ_P(z))

static void zend_weakref_unref(zval *zv) {
	zend_weakref *wr = (zend_weakref*) Z_PTR_P(zv);

	GC_DEL_FLAGS(wr->referent, IS_OBJ_WEAKLY_REFERENCED);

	wr->referent = NULL;
}

void zend_weakrefs_init() {
	zend_hash_init(&EG(weakrefs), 8, NULL, zend_weakref_unref, 0);
}

void zend_weakrefs_notify(zend_object *object) {
	zend_hash_index_del(&EG(weakrefs), (zend_ulong) object);
}

void zend_weakrefs_shutdown() {
	zend_hash_destroy(&EG(weakrefs));
}

static zend_object* zend_weakref_new(zend_class_entry *ce) {
	zend_weakref *wr = zend_object_alloc(sizeof(zend_weakref), zend_ce_weakref);

	zend_object_std_init(&wr->std, zend_ce_weakref);

	wr->std.handlers = &zend_weakref_handlers;

	return &wr->std;
}

static zend_always_inline zend_bool zend_weakref_find(zval *referent, zval *return_value) {
	zend_weakref *wr = zend_hash_index_find_ptr(&EG(weakrefs), (zend_ulong) Z_OBJ_P(referent));

	if (!wr) {
		return 0;
	}

	GC_ADDREF(&wr->std);
	ZVAL_OBJ(return_value, &wr->std);

	return 1;
}

static zend_always_inline void zend_weakref_create(zval *referent, zval *return_value) {
	zend_weakref *wr;

	object_init_ex(return_value, zend_ce_weakref);

	wr = zend_weakref_fetch(return_value);

	wr->referent = Z_OBJ_P(referent);

	zend_hash_index_add_ptr(&EG(weakrefs), (zend_ulong) wr->referent, wr);

	GC_ADD_FLAGS(wr->referent, IS_OBJ_WEAKLY_REFERENCED);
}

static zend_always_inline void zend_weakref_get(zval *weakref, zval *return_value) {
	zend_weakref *wr = zend_weakref_fetch(weakref);

	if (wr->referent) {
		ZVAL_OBJ(return_value, wr->referent);
		Z_ADDREF_P(return_value);
	}
}

static void zend_weakref_free(zend_object *zo) {
	zend_weakref *wr = zend_weakref_from(zo);

	if (wr->referent) {
		zend_hash_index_del(
			&EG(weakrefs), (zend_ulong) wr->referent);
	}

	zend_object_std_dtor(&wr->std);
}

#define zend_weakref_unsupported(thing) \
	zend_throw_error(NULL, "WeakReference objects do not support " thing);

static ZEND_COLD zval* zend_weakref_no_write(zval *object, zval *member, zval *value, void **rtc) {
	zend_weakref_unsupported("properties");

	return &EG(uninitialized_zval);
}

static ZEND_COLD zval* zend_weakref_no_read(zval *object, zval *member, int type, void **rtc, zval *rv) {
	if (!EG(exception)) {
		zend_weakref_unsupported("properties");
	}

	return &EG(uninitialized_zval);
}

static ZEND_COLD zval *zend_weakref_no_read_ptr(zval *object, zval *member, int type, void **rtc) {
	zend_weakref_unsupported("property references");
	return NULL;
}

static ZEND_COLD int zend_weakref_no_isset(zval *object, zval *member, int hse, void **rtc) {
	if (hse != 2) {
		zend_weakref_unsupported("properties");
	}
	return 0;
}

static ZEND_COLD void zend_weakref_no_unset(zval *object, zval *member, void **rtc) {
	zend_weakref_unsupported("properties");
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(zend_weakref_create_arginfo, 0, 1, WeakReference, 0)
	ZEND_ARG_TYPE_INFO(0, referent, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(zend_weakref_get_arginfo, 0, 0, IS_OBJECT, 1)
ZEND_END_ARG_INFO()

ZEND_COLD ZEND_METHOD(WeakReference, __construct)
{
	zend_throw_error(NULL,
	    "Direct instantiation of 'WeakReference' is not allowed, "
	    "use WeakReference::create instead");
}

ZEND_METHOD(WeakReference, create)
{
	zval *referent;

	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1,1)
		Z_PARAM_OBJECT(referent)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_weakref_find(referent, return_value)) {
	    return;
	}

	zend_weakref_create(referent, return_value);
}

ZEND_METHOD(WeakReference, get)
{
	ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 0)
	ZEND_PARSE_PARAMETERS_END();

	zend_weakref_get(getThis(), return_value);
}

static const zend_function_entry zend_weakref_methods[] = {
	ZEND_ME(WeakReference, __construct, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(WeakReference, create,  zend_weakref_create_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(WeakReference, get, zend_weakref_get_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

void zend_register_weakref_ce(void) /* {{{ */
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "WeakReference", zend_weakref_methods);
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
	zend_weakref_handlers.clone_obj = NULL;
}
/* }}} */

