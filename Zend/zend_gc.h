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
   | Authors: David Wang <planetbeing@gmail.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_GC_H
#define ZEND_GC_H

#ifndef GC_BENCH
# define GC_BENCH 0
#endif

#if GC_BENCH
# define GC_BENCH_INC(counter) GC_G(counter)++
# define GC_BENCH_DEC(counter) GC_G(counter)--
# define GC_BENCH_PEAK(peak, counter) do {		\
		if (GC_G(counter) > GC_G(peak)) {		\
			GC_G(peak) = GC_G(counter);			\
		}										\
	} while (0)
#else
# define GC_BENCH_INC(counter)
# define GC_BENCH_DEC(counter)
# define GC_BENCH_PEAK(peak, counter)
#endif

typedef struct _gc_root_buffer {
	zend_refcounted *ref;
	uint32_t         next;
	uint32_t         prev;
	uint32_t         refcount;
} gc_root_buffer;

typedef struct _zend_gc_globals {
	zend_bool         gc_enabled;
	zend_bool         gc_active;        /* GC currently running, forbid nested GC */
	zend_bool         gc_protected;     /* GC protected, forbid root additions */

	gc_root_buffer   *buf;				/* preallocated arrays of buffers   */
	uint32_t          buf_size;			/* size of the GC buffer            */
	uint32_t          num_roots;        /* number of roots in GC buffer     */
	uint32_t          unused;			/* linked list of unused buffers    */
	uint32_t          first_unused;		/* first unused buffer              */
	uint32_t          next_to_free;     /* next to free in to_free list     */
	uint32_t          gc_threshold;     /* GC collection threshold          */

	uint32_t gc_runs;
	uint32_t collected;

#if GC_BENCH
	uint32_t root_buf_length;
	uint32_t root_buf_peak;
	uint32_t zval_possible_root;
	uint32_t zval_buffered;
	uint32_t zval_remove_from_buffer;
	uint32_t zval_marked_grey;
#endif
} zend_gc_globals;

#ifdef ZTS
BEGIN_EXTERN_C()
ZEND_API extern int gc_globals_id;
END_EXTERN_C()
#define GC_G(v) ZEND_TSRMG(gc_globals_id, zend_gc_globals *, v)
#else
#define GC_G(v) (gc_globals.v)
extern ZEND_API zend_gc_globals gc_globals;
#endif

BEGIN_EXTERN_C()
ZEND_API extern int (*gc_collect_cycles)(void);

ZEND_API void ZEND_FASTCALL gc_possible_root(zend_refcounted *ref);
ZEND_API void ZEND_FASTCALL gc_remove_from_buffer(zend_refcounted *ref);
ZEND_API void gc_globals_ctor(void);
ZEND_API void gc_globals_dtor(void);
ZEND_API void gc_init(void);
ZEND_API void gc_reset(void);

/* The default implementation of the gc_collect_cycles callback. */
ZEND_API int  zend_gc_collect_cycles(void);
END_EXTERN_C()

#define GC_REMOVE_FROM_BUFFER(p) do { \
		zend_refcounted *_p = (zend_refcounted *)(p); \
		if (GC_ADDRESS(_p)) { \
			gc_remove_from_buffer(_p); \
		} \
	} while (0)

#define GC_MAY_LEAK(ref) \
	((GC_FLAGS(ref) & GC_COLLECTABLE) && GC_ADDRESS(ref) == 0)

static zend_always_inline void gc_check_possible_root(zend_refcounted *ref)
{
	if (GC_TYPE(ref) == IS_REFERENCE) {
		zval *zv = &((zend_reference*)ref)->val;

		if (!Z_REFCOUNTED_P(zv)) {
			return;
		}
		ref = Z_COUNTED_P(zv);
	}
	if (UNEXPECTED(GC_MAY_LEAK(ref))) {
		gc_possible_root(ref);
	}
}

#endif /* ZEND_GC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
