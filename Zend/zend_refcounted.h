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
*/

#ifndef ZEND_REFCOUNTED_H
#define ZEND_REFCOUNTED_H

#include "zend_portability.h"
#include "zend_rc_debug.h"
#include "zend_type_code.h"

#include <stdint.h>

#define GC_TYPE_MASK				0x0000000f
#define GC_FLAGS_MASK				0x000003f0
#define GC_INFO_MASK				0xfffffc00
#define GC_FLAGS_SHIFT				0
#define GC_INFO_SHIFT				10

/* zval_gc_flags(zval.value->gc.u.type_info) (common flags) */
#define GC_NOT_COLLECTABLE			(1<<4)
#define GC_PROTECTED                (1<<5) /* used for recursion detection */
#define GC_IMMUTABLE                (1<<6) /* can't be changed in place */
#define GC_PERSISTENT               (1<<7) /* allocated using malloc */
#define GC_PERSISTENT_LOCAL         (1<<8) /* persistent, but thread-local */

#define GC_TYPE_INFO(p)				(p)->gc.u.type_info
#define GC_TYPE(p)					zval_gc_type(GC_TYPE_INFO(p))
#define GC_FLAGS(p)					zval_gc_flags(GC_TYPE_INFO(p))
#define GC_INFO(p)					zval_gc_info(GC_TYPE_INFO(p))

#define GC_ADD_FLAGS(p, flags) do { \
		GC_TYPE_INFO(p) |= (flags) << GC_FLAGS_SHIFT; \
	} while (0)
#define GC_DEL_FLAGS(p, flags) do { \
		GC_TYPE_INFO(p) &= ~((flags) << GC_FLAGS_SHIFT); \
	} while (0)

#define GC_REFCOUNT(p)				zend_gc_refcount(&(p)->gc)
#define GC_SET_REFCOUNT(p, rc)		zend_gc_set_refcount(&(p)->gc, rc)
#define GC_ADDREF(p)				zend_gc_addref(&(p)->gc)
#define GC_DELREF(p)				zend_gc_delref(&(p)->gc)
#define GC_ADDREF_EX(p, rc)			zend_gc_addref_ex(&(p)->gc, rc)
#define GC_DELREF_EX(p, rc)			zend_gc_delref_ex(&(p)->gc, rc)
#define GC_TRY_ADDREF(p)			zend_gc_try_addref(&(p)->gc)
#define GC_TRY_DELREF(p)			zend_gc_try_delref(&(p)->gc)
#define GC_DTOR(p) \
	do { \
		zend_refcounted_h *_p = &(p)->gc; \
		if (zend_gc_delref(_p) == 0) { \
			rc_dtor_func((zend_refcounted *)_p); \
		} else { \
			gc_check_possible_root((zend_refcounted *)_p); \
		} \
	} while (0)
#define GC_DTOR_NO_REF(p) \
	do { \
		zend_refcounted_h *_p = &(p)->gc; \
		if (zend_gc_delref(_p) == 0) { \
			rc_dtor_func((zend_refcounted *)_p); \
		} else { \
			gc_check_possible_root_no_ref((zend_refcounted *)_p); \
		} \
	} while (0)

#define GC_NULL						(IS_NULL         | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_STRING					(IS_STRING       | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_ARRAY					IS_ARRAY
#define GC_OBJECT					IS_OBJECT
#define GC_RESOURCE					(IS_RESOURCE     | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_REFERENCE				(IS_REFERENCE    | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))
#define GC_CONSTANT_AST				(IS_CONSTANT_AST | (GC_NOT_COLLECTABLE << GC_FLAGS_SHIFT))

typedef struct _zend_refcounted_h {
	uint32_t         refcount;			/* reference counter 32-bit */
	union {
		uint32_t type_info;
	} u;
} zend_refcounted_h;

typedef struct _zend_refcounted {
	zend_refcounted_h gc;
} zend_refcounted;

static zend_always_inline uint8_t zval_gc_type(uint32_t gc_type_info) {
	return (gc_type_info & GC_TYPE_MASK);
}

static zend_always_inline uint32_t zval_gc_flags(uint32_t gc_type_info) {
	return (gc_type_info >> GC_FLAGS_SHIFT) & (GC_FLAGS_MASK >> GC_FLAGS_SHIFT);
}

static zend_always_inline uint32_t zval_gc_info(uint32_t gc_type_info) {
	return (gc_type_info >> GC_INFO_SHIFT);
}

static zend_always_inline uint32_t zend_gc_refcount(const zend_refcounted_h *p) {
	return p->refcount;
}

static zend_always_inline uint32_t zend_gc_set_refcount(zend_refcounted_h *p, uint32_t rc) {
	p->refcount = rc;
	return p->refcount;
}

static zend_always_inline uint32_t zend_gc_addref(zend_refcounted_h *p) {
	ZEND_RC_MOD_CHECK(p);
	return ++(p->refcount);
}

static zend_always_inline void zend_gc_try_addref(zend_refcounted_h *p) {
	if (!(p->u.type_info & GC_IMMUTABLE)) {
		ZEND_RC_MOD_CHECK(p);
		++p->refcount;
	}
}

static zend_always_inline void zend_gc_try_delref(zend_refcounted_h *p) {
	if (!(p->u.type_info & GC_IMMUTABLE)) {
		ZEND_RC_MOD_CHECK(p);
		--p->refcount;
	}
}

static zend_always_inline uint32_t zend_gc_delref(zend_refcounted_h *p) {
	ZEND_ASSERT(p->refcount > 0);
	ZEND_RC_MOD_CHECK(p);
	return --(p->refcount);
}

static zend_always_inline uint32_t zend_gc_addref_ex(zend_refcounted_h *p, uint32_t rc) {
	ZEND_RC_MOD_CHECK(p);
	p->refcount += rc;
	return p->refcount;
}

static zend_always_inline uint32_t zend_gc_delref_ex(zend_refcounted_h *p, uint32_t rc) {
	ZEND_RC_MOD_CHECK(p);
	p->refcount -= rc;
	return p->refcount;
}

#endif /* ZEND_REFCOUNTED_H */
