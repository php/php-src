/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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

#define GC_COLOR  0x03

#define GC_BLACK  0x00
#define GC_WHITE  0x01
#define GC_GREY   0x02
#define GC_PURPLE 0x03

#define GC_ADDRESS(v) \
	((gc_root_buffer*)(((zend_uintptr_t)(v)) & ~GC_COLOR))
#define GC_SET_ADDRESS(v, a) \
	(v) = ((gc_root_buffer*)((((zend_uintptr_t)(v)) & GC_COLOR) | ((zend_uintptr_t)(a))))
#define GC_GET_COLOR(v) \
	(((zend_uintptr_t)(v)) & GC_COLOR)
#define GC_SET_COLOR(v, c) \
	(v) = ((gc_root_buffer*)((((zend_uintptr_t)(v)) & ~GC_COLOR) | (c)))
#define GC_SET_BLACK(v) \
	(v) = ((gc_root_buffer*)(((zend_uintptr_t)(v)) & ~GC_COLOR))
#define GC_SET_PURPLE(v) \
	(v) = ((gc_root_buffer*)(((zend_uintptr_t)(v)) | GC_PURPLE))

#define GC_ZVAL_INIT(z) \
	((zval_gc_info*)(z))->u.buffered = NULL
#define GC_ZVAL_ADDRESS(v) \
	GC_ADDRESS(((zval_gc_info*)(v))->u.buffered)
#define GC_ZVAL_SET_ADDRESS(v, a) \
	GC_SET_ADDRESS(((zval_gc_info*)(v))->u.buffered, (a))
#define GC_ZVAL_GET_COLOR(v) \
	GC_GET_COLOR(((zval_gc_info*)(v))->u.buffered)
#define GC_ZVAL_SET_COLOR(v, c) \
	GC_SET_COLOR(((zval_gc_info*)(v))->u.buffered, (c))
#define GC_ZVAL_SET_BLACK(v) \
	GC_SET_BLACK(((zval_gc_info*)(v))->u.buffered)
#define GC_ZVAL_SET_PURPLE(v) \
	GC_SET_PURPLE(((zval_gc_info*)(v))->u.buffered)

#define GC_OBJ_INIT(z) \
	(z)->buffered = NULL

typedef struct _gc_root_buffer {
	struct _gc_root_buffer   *prev;		/* double-linked list               */
	struct _gc_root_buffer   *next;
	zend_object_handle        handle;	/* must be 0 for zval               */
	union {
		zval                 *pz;
		const zend_object_handlers *handlers;
	} u;
} gc_root_buffer;

typedef struct _zval_gc_info {
	zval z;
	union {
		gc_root_buffer       *buffered;
		struct _zval_gc_info *next;
	} u;
} zval_gc_info;

typedef struct _zend_gc_globals {
	zend_bool         gc_enabled;
	zend_bool         gc_active;

	gc_root_buffer   *buf;				/* preallocated arrays of buffers   */
	gc_root_buffer    roots;			/* list of possible roots of cycles */
	gc_root_buffer   *unused;			/* list of unused buffers           */
	gc_root_buffer   *first_unused;		/* pointer to first unused buffer   */
	gc_root_buffer   *last_unused;		/* pointer to last unused buffer    */

	zval_gc_info     *zval_to_free;		/* temporary list of zvals to free */
	zval_gc_info     *free_list;
	zval_gc_info     *next_to_free;

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
ZEND_API void gc_zval_possible_root(zval *zv TSRMLS_DC);
ZEND_API void gc_zobj_possible_root(zval *zv TSRMLS_DC);
ZEND_API void gc_remove_zval_from_buffer(zval *zv TSRMLS_DC);
ZEND_API void gc_globals_ctor(TSRMLS_D);
ZEND_API void gc_globals_dtor(TSRMLS_D);
ZEND_API void gc_init(TSRMLS_D);
ZEND_API void gc_reset(TSRMLS_D);
END_EXTERN_C()

#define GC_ZVAL_CHECK_POSSIBLE_ROOT(z) \
	gc_zval_check_possible_root((z) TSRMLS_CC)

#define GC_REMOVE_FROM_BUFFER(current) \
	gc_remove_from_buffer((current) TSRMLS_CC)

#define GC_REMOVE_ZVAL_FROM_BUFFER(z)					\
	if (GC_ADDRESS(((zval_gc_info*)z)->u.buffered)) {	\
		gc_remove_zval_from_buffer(z TSRMLS_CC);		\
	}

#define GC_ZOBJ_CHECK_POSSIBLE_ROOT(zobject)									\
	do {																		\
		if (EXPECTED(EG(objects_store).object_buckets != NULL) &&				\
		    EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(zobject)].valid) {	\
			gc_zobj_possible_root(zobject TSRMLS_CC);							\
		}																		\
	} while (0)

#define GC_REMOVE_ZOBJ_FROM_BUFFER(obj)									\
	do {																\
		if (GC_ADDRESS((obj)->buffered) && !GC_G(gc_active)) {			\
			GC_BENCH_INC(zobj_remove_from_buffer);						\
			GC_REMOVE_FROM_BUFFER(GC_ADDRESS((obj)->buffered));			\
			(obj)->buffered = NULL;										\
		}																\
	} while (0)

static zend_always_inline void gc_zval_check_possible_root(zval *z TSRMLS_DC)
{
	if (z->type == IS_ARRAY || z->type == IS_OBJECT) {
		gc_zval_possible_root(z TSRMLS_CC);
	}
}

static zend_always_inline void gc_remove_from_buffer(gc_root_buffer *root TSRMLS_DC)
{
	root->next->prev = root->prev;
	root->prev->next = root->next;
	root->prev = GC_G(unused);
	GC_G(unused) = root;
	GC_BENCH_DEC(root_buf_length);
}

#define ALLOC_PERMANENT_ZVAL(z)							\
	do {												\
		(z) = (zval*)malloc(sizeof(zval_gc_info));		\
		GC_ZVAL_INIT(z);								\
	} while (0)

/* The following macros override macros from zend_alloc.h */
#undef  ALLOC_ZVAL
#define ALLOC_ZVAL(z) 									\
	do {												\
		(z) = (zval*)emalloc(sizeof(zval_gc_info));		\
		GC_ZVAL_INIT(z);								\
	} while (0)

#undef  FREE_ZVAL
#define FREE_ZVAL(z) 									\
	do {												\
	    GC_REMOVE_ZVAL_FROM_BUFFER(z);					\
		efree(z);										\
	} while (0)

#undef  ALLOC_ZVAL_REL
#define ALLOC_ZVAL_REL(z)								\
	do {												\
		(z) = (zval*)emalloc_rel(sizeof(zval_gc_info));	\
		GC_ZVAL_INIT(z);								\
	} while (0)

#undef  FREE_ZVAL_REL
#define FREE_ZVAL_REL(z)								\
	do {												\
	    GC_REMOVE_ZVAL_FROM_BUFFER(z);					\
		efree_rel(z);									\
	} while (0)

#define FREE_ZVAL_EX(z)									\
	efree(z)

#define FREE_ZVAL_REL_EX(z)								\
	efree_rel(z)

#endif /* ZEND_GC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
