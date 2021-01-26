/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: David Wang <planetbeing@gmail.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_GC_H
#define ZEND_GC_H

BEGIN_EXTERN_C()

typedef struct _zend_gc_status {
	uint32_t runs;
	uint32_t collected;
	uint32_t threshold;
	uint32_t num_roots;
} zend_gc_status;

ZEND_API extern int (*gc_collect_cycles)(void);

ZEND_API void ZEND_FASTCALL gc_possible_root(zend_refcounted *ref);
ZEND_API void ZEND_FASTCALL gc_remove_from_buffer(zend_refcounted *ref);

/* enable/disable automatic start of GC collection */
ZEND_API bool gc_enable(bool enable);
ZEND_API bool gc_enabled(void);

/* enable/disable possible root additions */
ZEND_API bool gc_protect(bool protect);
ZEND_API bool gc_protected(void);

/* The default implementation of the gc_collect_cycles callback. */
ZEND_API int  zend_gc_collect_cycles(void);

ZEND_API void zend_gc_get_status(zend_gc_status *status);

void gc_globals_ctor(void);
void gc_globals_dtor(void);
void gc_reset(void);

#ifdef ZTS
size_t zend_gc_globals_size(void);
#endif

#define GC_REMOVE_FROM_BUFFER(p) do { \
		zend_refcounted *_p = (zend_refcounted*)(p); \
		if (GC_TYPE_INFO(_p) & GC_INFO_MASK) { \
			gc_remove_from_buffer(_p); \
		} \
	} while (0)

#define GC_MAY_LEAK(ref) \
	((GC_TYPE_INFO(ref) & \
		(GC_INFO_MASK | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))) == 0)

static zend_always_inline void gc_check_possible_root(zend_refcounted *ref)
{
	if (EXPECTED(GC_TYPE_INFO(ref) == GC_REFERENCE)) {
		zval *zv = &((zend_reference*)ref)->val;

		if (!Z_COLLECTABLE_P(zv)) {
			return;
		}
		ref = Z_COUNTED_P(zv);
	}
	if (UNEXPECTED(GC_MAY_LEAK(ref))) {
		gc_possible_root(ref);
	}
}

/* These APIs can be used to simplify object get_gc implementations
 * over heterogeneous structures. See zend_generator_get_gc() for
 * a usage example. */

typedef struct {
	zval *cur;
	zval *end;
	zval *start;
} zend_get_gc_buffer;

ZEND_API zend_get_gc_buffer *zend_get_gc_buffer_create(void);
ZEND_API void zend_get_gc_buffer_grow(zend_get_gc_buffer *gc_buffer);

static zend_always_inline void zend_get_gc_buffer_add_zval(
		zend_get_gc_buffer *gc_buffer, zval *zv) {
	if (Z_REFCOUNTED_P(zv)) {
		if (UNEXPECTED(gc_buffer->cur == gc_buffer->end)) {
			zend_get_gc_buffer_grow(gc_buffer);
		}
		ZVAL_COPY_VALUE(gc_buffer->cur, zv);
		gc_buffer->cur++;
	}
}

static zend_always_inline void zend_get_gc_buffer_add_obj(
		zend_get_gc_buffer *gc_buffer, zend_object *obj) {
	if (UNEXPECTED(gc_buffer->cur == gc_buffer->end)) {
		zend_get_gc_buffer_grow(gc_buffer);
	}
	ZVAL_OBJ(gc_buffer->cur, obj);
	gc_buffer->cur++;
}

static zend_always_inline void zend_get_gc_buffer_use(
		zend_get_gc_buffer *gc_buffer, zval **table, int *n) {
	*table = gc_buffer->start;
	*n = gc_buffer->cur - gc_buffer->start;
}

END_EXTERN_C()

#endif /* ZEND_GC_H */
