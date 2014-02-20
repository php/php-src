/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
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

#define GC_COLOR  0xc000

#define GC_BLACK  0x0000
#define GC_WHITE  0x8000
#define GC_GREY   0x4000
#define GC_PURPLE 0xc000

#define GC_ADDRESS(v) \
	((v) & ~GC_COLOR)
#define GC_SET_ADDRESS(v, a) \
	do {(v) = ((v) & GC_COLOR) | (a);} while (0)
#define GC_GET_COLOR(v) \
	(((zend_uintptr_t)(v)) & GC_COLOR)
#define GC_SET_COLOR(v, c) \
	do {(v) = ((v) & ~GC_COLOR) | (c);} while (0)
#define GC_SET_BLACK(v) \
	do {(v) = (v) & ~GC_COLOR;} while (0)
#define GC_SET_PURPLE(v) \
	do {(v) = (v) | GC_COLOR;} while (0)

#define GC_ZVAL_ADDRESS(v) \
	GC_ADDRESS(Z_GC_BUFFER_P(v))
#define GC_ZVAL_SET_ADDRESS(v, a) \
	GC_SET_ADDRESS(Z_GC_BUFFER_P(v), (a))
#define GC_ZVAL_GET_COLOR(v) \
	GC_GET_COLOR(Z_GC_BUFFER_P(v))
#define GC_ZVAL_SET_COLOR(v, c) \
	GC_SET_COLOR(Z_GC_BUFFER_P(v), (c))
#define GC_ZVAL_SET_BLACK(v) \
	GC_SET_BLACK(Z_GC_BUFFER_P(v))
#define GC_ZVAL_SET_PURPLE(v) \
	GC_SET_PURPLE(Z_GC_BUFFER_P(v))

typedef struct _gc_root_buffer {
	struct _gc_root_buffer   *prev;		/* double-linked list               */
	struct _gc_root_buffer   *next;
	zend_refcounted          *ref;
} gc_root_buffer;

typedef struct _zend_gc_globals {
	zend_bool         gc_enabled;
	zend_bool         gc_active;

	gc_root_buffer   *buf;				/* preallocated arrays of buffers   */
	gc_root_buffer    roots;			/* list of possible roots of cycles */
	gc_root_buffer   *unused;			/* list of unused buffers           */
	gc_root_buffer   *first_unused;		/* pointer to first unused buffer   */
	gc_root_buffer   *last_unused;		/* pointer to last unused buffer    */

	zend_refcounted  *zval_to_free;		/* temporary list of zvals to free */
	zend_refcounted  *free_list;
	zend_refcounted  *next_to_free;

	zend_uint gc_runs;
	zend_uint collected;

#if GC_BENCH
	zend_uint root_buf_length;
	zend_uint root_buf_peak;
	zend_uint zval_possible_root;
	zend_uint zobj_possible_root;
	zend_uint zval_buffered;
	zend_uint zobj_buffered;
	zend_uint zval_remove_from_buffer;
	zend_uint zobj_remove_from_buffer;
	zend_uint zval_marked_grey;
	zend_uint zobj_marked_grey;
#endif

} zend_gc_globals;

#ifdef ZTS
BEGIN_EXTERN_C()
ZEND_API extern int gc_globals_id;
END_EXTERN_C()
#define GC_G(v) TSRMG(gc_globals_id, zend_gc_globals *, v)
#else
#define GC_G(v) (gc_globals.v)
extern ZEND_API zend_gc_globals gc_globals;
#endif

BEGIN_EXTERN_C()
ZEND_API int  gc_collect_cycles(TSRMLS_D);
ZEND_API void gc_zval_possible_root(zend_refcounted *ref TSRMLS_DC);
ZEND_API void gc_remove_zval_from_buffer(zend_refcounted *ref TSRMLS_DC);
ZEND_API void gc_globals_ctor(TSRMLS_D);
ZEND_API void gc_globals_dtor(TSRMLS_D);
ZEND_API void gc_init(TSRMLS_D);
ZEND_API void gc_reset(TSRMLS_D);
END_EXTERN_C()

#define GC_ZVAL_CHECK_POSSIBLE_ROOT(z) \
	gc_zval_check_possible_root((z) TSRMLS_CC)

#define GC_REMOVE_ZVAL_FROM_BUFFER(z) do { \
		if (GC_ZVAL_ADDRESS(z)) { \
			gc_remove_zval_from_buffer(Z_COUNTED_P(z) TSRMLS_CC); \
		} \
	} while (0)

#define GC_REMOVE_ZOBJ_FROM_BUFFER(z) do { \
		if (GC_ADDRESS((z)->gc.u.v.buffer)) { \
			gc_remove_zval_from_buffer(&(z)->gc TSRMLS_CC); \
		} \
	} while (0)

static zend_always_inline void gc_zval_check_possible_root(zval *z TSRMLS_DC)
{
	if (Z_TYPE_P(z) == IS_ARRAY || Z_TYPE_P(z) == IS_OBJECT) {
		gc_zval_possible_root(Z_COUNTED_P(z) TSRMLS_CC);
	}
}

#endif /* ZEND_GC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
