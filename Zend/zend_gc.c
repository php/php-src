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
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

/**
 * zend_gc_collect_cycles
 * ======================
 *
 * Colors and its meaning
 * ----------------------
 *
 * BLACK  (GC_BLACK)   - In use or free.
 * GREY   (GC_GREY)    - Possible member of cycle.
 * WHITE  (GC_WHITE)   - Member of garbage cycle.
 * PURPLE (GC_PURPLE)  - Possible root of cycle.
 *
 * Colors described in the paper but not used
 * ------------------------------------------
 *
 * GREEN - Acyclic
 * RED   - Candidate cycle underogin
 * ORANGE - Candidate cycle awaiting epoch boundary.
 *
 *
 * Flow
 * =====
 *
 * The garbage collect cycle starts from 'gc_mark_roots', which traverses the
 * possible roots, and calls mark_grey for roots are marked purple with
 * depth-first traverse.
 *
 * After all possible roots are traversed and marked,
 * gc_scan_roots will be called, and each root will be called with
 * gc_scan(root->ref)
 *
 * gc_scan checkes the colors of possible members.
 *
 * If the node is marked as grey and the refcount > 0
 *    gc_scan_black will be called on that node to scan it's subgraph.
 * otherwise (refcount == 0), it marks the node white.
 *
 * A node MAY be added to possbile roots when ZEND_UNSET_VAR happens or
 * zend_assign_to_variable is called only when possible garbage node is
 * produced.
 * gc_possible_root() will be called to add the nodes to possible roots.
 *
 *
 * For objects, we call their get_gc handler (by default 'zend_std_get_gc') to
 * get the object properties to scan.
 *
 *
 * @see http://researcher.watson.ibm.com/researcher/files/us-bacon/Bacon01Concurrent.pdf
 */
#include "zend.h"
#include "zend_API.h"

#ifndef GC_BENCH
# define GC_BENCH 0
#endif

#ifndef ZEND_GC_DEBUG
# define ZEND_GC_DEBUG 0
#endif

/* GC_INFO layout */
#define GC_ADDRESS  0x0fffff
#define GC_COLOR    0x300000

#define GC_BLACK    0x000000 /* must be zero */
#define GC_WHITE    0x100000
#define GC_GREY     0x200000
#define GC_PURPLE   0x300000

/* GC_INFO access */
#define GC_REF_ADDRESS(ref) \
	(((GC_TYPE_INFO(ref)) & (GC_ADDRESS << GC_INFO_SHIFT)) >> GC_INFO_SHIFT)

#define GC_REF_COLOR(ref) \
	(((GC_TYPE_INFO(ref)) & (GC_COLOR << GC_INFO_SHIFT)) >> GC_INFO_SHIFT)

#define GC_REF_CHECK_COLOR(ref, color) \
	((GC_TYPE_INFO(ref) & (GC_COLOR << GC_INFO_SHIFT)) == ((color) << GC_INFO_SHIFT))

#define GC_REF_SET_INFO(ref, info) do { \
		GC_TYPE_INFO(ref) = \
			(GC_TYPE_INFO(ref) & (GC_TYPE_MASK | GC_FLAGS_MASK)) | \
			((info) << GC_INFO_SHIFT); \
	} while (0)

#define GC_REF_SET_COLOR(ref, c) do { \
		GC_TYPE_INFO(ref) = \
			(GC_TYPE_INFO(ref) & ~(GC_COLOR << GC_INFO_SHIFT)) | \
			((c) << GC_INFO_SHIFT); \
	} while (0)

#define GC_REF_SET_BLACK(ref) do { \
		GC_TYPE_INFO(ref) &= ~(GC_COLOR << GC_INFO_SHIFT); \
	} while (0)

#define GC_REF_SET_PURPLE(ref) do { \
		GC_TYPE_INFO(ref) |= (GC_COLOR << GC_INFO_SHIFT); \
	} while (0)

/* bit stealing tags for gc_root_buffer.ref */
#define GC_BITS    0x3

#define GC_ROOT    0x0 /* possible root of circular garbage     */
#define GC_UNUSED  0x1 /* part of linked list of unused buffers */
#define GC_GARBAGE 0x2 /* garbage to delete                     */

#define GC_GET_PTR(ptr) \
	((void*)(((uintptr_t)(ptr)) & ~GC_BITS))

#define GC_IS_ROOT(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_ROOT)
#define GC_IS_UNUSED(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_UNUSED)
#define GC_IS_GARBAGE(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_GARBAGE)

#define GC_MAKE_GARBAGE(ptr) \
	((void*)(((uintptr_t)(ptr)) | GC_GARBAGE))

/* GC address conversion */
#define GC_IDX2PTR(idx)      (GC_G(buf) + (idx))
#define GC_PTR2IDX(ptr)      ((ptr) - GC_G(buf))

#define GC_IDX2LIST(idx)     ((void*)(uintptr_t)(((idx) * sizeof(void*)) | GC_UNUSED))
#define GC_LIST2IDX(list)    (((uint32_t)(uintptr_t)(list)) / sizeof(void*))

/* GC buffers */
#define GC_INVALID           0
#define GC_FIRST_ROOT        1

#define GC_DEFAULT_BUF_SIZE  (16 * 1024)
#define GC_BUF_GROW_STEP     (128 * 1024)

#define GC_MAX_UNCOMPRESSED  (1024 * 1024)
#define GC_MAX_BUF_SIZE      0x40000000

#define GC_THRESHOLD_DEFAULT 10000
#define GC_THRESHOLD_STEP    10000
#define GC_THRESHOLD_MAX     1000000000
#define GC_THRESHOLD_TRIGGER 100

/* GC flags */
#define GC_HAS_DESTRUCTORS  (1<<0)

/* unused buffers */
#define GC_HAS_UNUSED() \
	(GC_G(unused) != GC_INVALID)
#define GC_FETCH_UNUSED() \
	gc_fetch_unused()
#define GC_LINK_UNUSED(root) \
	gc_link_unused(root)

#define GC_HAS_NEXT_UNUSED_UNDER_THRESHOLD() \
	(GC_G(first_unused) < GC_G(gc_threshold))
#define GC_HAS_NEXT_UNUSED() \
	(GC_G(first_unused) != GC_G(buf_size))
#define GC_FETCH_NEXT_UNUSED() \
	gc_fetch_next_unused()

ZEND_API int (*gc_collect_cycles)(void);

typedef struct _gc_root_buffer {
	zend_refcounted  *ref;
} gc_root_buffer;

typedef struct _zend_gc_globals {
	zend_bool         gc_enabled;
	zend_bool         gc_active;        /* GC currently running, forbid nested GC */
	zend_bool         gc_protected;     /* GC protected, forbid root additions */
	zend_bool         gc_full;

	gc_root_buffer   *buf;				/* preallocated arrays of buffers   */
	uint32_t          unused;			/* linked list of unused buffers    */
	uint32_t          first_unused;		/* first unused buffer              */
	uint32_t          gc_threshold;     /* GC collection threshold          */
	uint32_t          buf_size;			/* size of the GC buffer            */
	uint32_t          num_roots;		/* number of roots in GC buffer     */

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
static int gc_globals_id;
#define GC_G(v) ZEND_TSRMG(gc_globals_id, zend_gc_globals *, v)
#else
#define GC_G(v) (gc_globals.v)
static zend_gc_globals gc_globals;
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

#if ZEND_GC_DEBUG > 1
# define GC_TRACE(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__);
# define GC_TRACE_REF(ref, format, ...) \
	do { \
		gc_trace_ref((zend_refcounted *) ref); \
		fprintf(stderr, format "\n", ##__VA_ARGS__); \
	} while (0)
# define GC_TRACE_SET_COLOR(ref, color) \
	GC_TRACE_REF(ref, "->%s", gc_color_name(color))
#else
# define GC_TRACE_REF(ref, format, ...)
# define GC_TRACE_SET_COLOR(ref, new_color)
# define GC_TRACE(str)
#endif

static zend_always_inline uint32_t gc_compress(uint32_t idx)
{
	return idx % GC_MAX_UNCOMPRESSED;
}

static zend_always_inline gc_root_buffer* gc_decompress(zend_refcounted *ref, uint32_t idx)
{
	gc_root_buffer *root = GC_IDX2PTR(idx);

	if (EXPECTED(GC_GET_PTR(root->ref) == ref)) {
		return root;
	}

	while (1) {
		idx += GC_MAX_UNCOMPRESSED;
		ZEND_ASSERT(idx < GC_G(first_unused));
		root = GC_IDX2PTR(idx);
		if (GC_GET_PTR(root->ref) == ref) {
			return root;
		}
	}
}

static zend_always_inline uint32_t gc_fetch_unused(void)
{
	uint32_t idx;
	gc_root_buffer *root;

	ZEND_ASSERT(GC_HAS_UNUSED());
	idx = GC_G(unused);
	root = GC_IDX2PTR(idx);
	ZEND_ASSERT(GC_IS_UNUSED(root->ref));
	GC_G(unused) = GC_LIST2IDX(root->ref);
	return idx;
}

static zend_always_inline void gc_link_unused(gc_root_buffer *root)
{
	root->ref = GC_IDX2LIST(GC_G(unused));
	GC_G(unused) = GC_PTR2IDX(root);
}

static zend_always_inline uint32_t gc_fetch_next_unused(void)
{
	uint32_t idx;

	ZEND_ASSERT(GC_HAS_NEXT_UNUSED());
	idx = GC_G(first_unused);
	GC_G(first_unused) = GC_G(first_unused) + 1;
	return idx;
}

#if ZEND_GC_DEBUG > 1
static const char *gc_color_name(uint32_t color) {
	switch (color) {
		case GC_BLACK: return "black";
		case GC_WHITE: return "white";
		case GC_GREY: return "grey";
		case GC_PURPLE: return "purple";
		default: return "unknown";
	}
}
static void gc_trace_ref(zend_refcounted *ref) {
	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object *obj = (zend_object *) ref;
		fprintf(stderr, "[%p] rc=%d addr=%d %s object(%s)#%d ",
			ref, GC_REFCOUNT(ref), GC_REF_ADDRESS(ref),
			gc_color_name(GC_REF_COLOR(ref)),
			obj->ce->name->val, obj->handle);
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		zend_array *arr = (zend_array *) ref;
		fprintf(stderr, "[%p] rc=%d addr=%d %s array(%d) ",
			ref, GC_REFCOUNT(ref), GC_REF_ADDRESS(ref),
			gc_color_name(GC_REF_COLOR(ref)),
			zend_hash_num_elements(arr));
	} else {
		fprintf(stderr, "[%p] rc=%d addr=%d %s %s ",
			ref, GC_REFCOUNT(ref), GC_REF_ADDRESS(ref),
			gc_color_name(GC_REF_COLOR(ref)),
			zend_get_type_by_const(GC_TYPE(ref)));
	}
}
#endif

static zend_always_inline void gc_remove_from_roots(gc_root_buffer *root)
{
	GC_LINK_UNUSED(root);
	GC_G(num_roots)--;
	GC_BENCH_DEC(root_buf_length);
}

static void root_buffer_dtor(zend_gc_globals *gc_globals)
{
	if (gc_globals->buf) {
		free(gc_globals->buf);
		gc_globals->buf = NULL;
	}
}

static void gc_globals_ctor_ex(zend_gc_globals *gc_globals)
{
	gc_globals->gc_enabled = 0;
	gc_globals->gc_active = 0;
	gc_globals->gc_protected = 1;
	gc_globals->gc_full = 0;

	gc_globals->buf = NULL;
	gc_globals->unused = GC_INVALID;
	gc_globals->first_unused = GC_INVALID;
	gc_globals->gc_threshold = GC_INVALID;
	gc_globals->buf_size = GC_INVALID;
	gc_globals->num_roots = 0;

	gc_globals->gc_runs = 0;
	gc_globals->collected = 0;

#if GC_BENCH
	gc_globals->root_buf_length = 0;
	gc_globals->root_buf_peak = 0;
	gc_globals->zval_possible_root = 0;
	gc_globals->zval_buffered = 0;
	gc_globals->zval_remove_from_buffer = 0;
	gc_globals->zval_marked_grey = 0;
#endif
}

void gc_globals_ctor(void)
{
#ifdef ZTS
	ts_allocate_id(&gc_globals_id, sizeof(zend_gc_globals), (ts_allocate_ctor) gc_globals_ctor_ex, (ts_allocate_dtor) root_buffer_dtor);
#else
	gc_globals_ctor_ex(&gc_globals);
#endif
}

void gc_globals_dtor(void)
{
#ifndef ZTS
	root_buffer_dtor(&gc_globals);
#endif
}

void gc_reset(void)
{
	if (GC_G(buf)) {
		GC_G(gc_active) = 0;
		GC_G(gc_protected) = 0;
		GC_G(gc_full) = 0;
		GC_G(unused) = GC_INVALID;
		GC_G(first_unused) = GC_FIRST_ROOT;
		GC_G(num_roots) = 0;

		GC_G(gc_runs) = 0;
		GC_G(collected) = 0;

#if GC_BENCH
		GC_G(root_buf_length) = 0;
		GC_G(root_buf_peak) = 0;
		GC_G(zval_possible_root) = 0;
		GC_G(zval_buffered) = 0;
		GC_G(zval_remove_from_buffer) = 0;
		GC_G(zval_marked_grey) = 0;
#endif
	}
}

ZEND_API zend_bool gc_enable(zend_bool enable)
{
	zend_bool old_enabled = GC_G(gc_enabled);
	GC_G(gc_enabled) = enable;
	if (enable && !old_enabled && GC_G(buf) == NULL) {
		GC_G(buf) = (gc_root_buffer*) pemalloc(sizeof(gc_root_buffer) * GC_DEFAULT_BUF_SIZE, 1);
		GC_G(buf)[0].ref = NULL;
		GC_G(buf_size) = GC_DEFAULT_BUF_SIZE;
		GC_G(gc_threshold) = GC_THRESHOLD_DEFAULT + GC_FIRST_ROOT;
		gc_reset();
	}
	return old_enabled;
}

ZEND_API zend_bool gc_enabled(void)
{
	return GC_G(gc_enabled);
}

ZEND_API zend_bool gc_protect(zend_bool protect)
{
	zend_bool old_protected = GC_G(gc_protected);
	GC_G(gc_protected) = protect;
	return old_protected;
}

ZEND_API zend_bool gc_protected(void)
{
	return GC_G(gc_protected);
}

static void gc_grow_root_buffer(void)
{
	size_t new_size;

	if (GC_G(buf_size) >= GC_MAX_BUF_SIZE) {
		if (!GC_G(gc_full)) {
			zend_error(E_WARNING, "GC buffer overflow (GC disabled)\n");
			GC_G(gc_active) = 1;
			GC_G(gc_protected) = 1;
			GC_G(gc_full) = 1;
			return;
		}
	}
	if (GC_G(buf_size) < GC_BUF_GROW_STEP) {
		new_size = GC_G(buf_size) * 2;
	} else {
		new_size = GC_G(buf_size) + GC_BUF_GROW_STEP;
	}
	if (new_size > GC_MAX_BUF_SIZE) {
		new_size = GC_MAX_BUF_SIZE;
	}
	GC_G(buf) = perealloc(GC_G(buf), sizeof(gc_root_buffer) * new_size, 1);
	GC_G(buf_size) = new_size;
}

static void gc_adjust_threshold(int count)
{
	uint32_t new_threshold;

	/* TODO Very simple heuristic for dynamic GC buffer resizing:
	 * If there are "too few" collections, increase the collection threshold
	 * by a fixed step */
	if (count < GC_THRESHOLD_TRIGGER) {
		/* increase */
		if (GC_G(gc_threshold) < GC_THRESHOLD_MAX) {
			new_threshold = GC_G(gc_threshold) + GC_THRESHOLD_STEP;
			if (new_threshold > GC_THRESHOLD_MAX) {
				new_threshold = GC_THRESHOLD_MAX;
			}
			if (new_threshold > GC_G(buf_size)) {
				gc_grow_root_buffer();
			}
			if (new_threshold <= GC_G(buf_size)) {
				GC_G(gc_threshold) = new_threshold;
			}
		}
	} else if (GC_G(gc_threshold) > GC_THRESHOLD_DEFAULT) {
		new_threshold = GC_G(gc_threshold) - GC_THRESHOLD_STEP;
		if (new_threshold < GC_THRESHOLD_DEFAULT) {
			new_threshold = GC_THRESHOLD_DEFAULT;
		}
		GC_G(gc_threshold) = new_threshold;
	}
}

static zend_never_inline void ZEND_FASTCALL gc_possible_root_when_full(zend_refcounted *ref)
{
	uint32_t idx;
	gc_root_buffer *newRoot;

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	ZEND_ASSERT(GC_INFO(ref) == 0);

	if (GC_G(gc_enabled) && !GC_G(gc_active)) {
		GC_ADDREF(ref);
		gc_adjust_threshold(gc_collect_cycles());
		if (UNEXPECTED(GC_DELREF(ref)) == 0) {
			rc_dtor_func(ref);
			return;
		} else if (UNEXPECTED(GC_INFO(ref))) {
			return;
		}
	}

	if (GC_HAS_UNUSED()) {
		idx = GC_FETCH_UNUSED();
	} else if (EXPECTED(GC_HAS_NEXT_UNUSED())) {
		idx = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_grow_root_buffer();
		if (UNEXPECTED(!GC_HAS_NEXT_UNUSED())) {
			return;
		}
		idx = GC_FETCH_NEXT_UNUSED();
	}

	newRoot = GC_IDX2PTR(idx);
	newRoot->ref = ref; /* GC_ROOT tag is 0 */
	GC_TRACE_SET_COLOR(ref, GC_PURPLE);

	idx = gc_compress(idx);
	GC_REF_SET_INFO(ref, idx | GC_PURPLE);
	GC_G(num_roots)++;

	GC_BENCH_INC(zval_buffered);
	GC_BENCH_INC(root_buf_length);
	GC_BENCH_PEAK(root_buf_peak, root_buf_length);
}

ZEND_API void ZEND_FASTCALL gc_possible_root(zend_refcounted *ref)
{
	uint32_t idx;
	gc_root_buffer *newRoot;

	if (UNEXPECTED(GC_G(gc_protected))) {
		return;
	}

	GC_BENCH_INC(zval_possible_root);

	if (EXPECTED(GC_HAS_UNUSED())) {
		idx = GC_FETCH_UNUSED();
	} else if (EXPECTED(GC_HAS_NEXT_UNUSED_UNDER_THRESHOLD())) {
		idx = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_possible_root_when_full(ref);
		return;
	}

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	ZEND_ASSERT(GC_INFO(ref) == 0);

	newRoot = GC_IDX2PTR(idx);
	newRoot->ref = ref; /* GC_ROOT tag is 0 */
	GC_TRACE_SET_COLOR(ref, GC_PURPLE);

	idx = gc_compress(idx);
	GC_REF_SET_INFO(ref, idx | GC_PURPLE);
	GC_G(num_roots)++;

	GC_BENCH_INC(zval_buffered);
	GC_BENCH_INC(root_buf_length);
	GC_BENCH_PEAK(root_buf_peak, root_buf_length);
}

static zend_never_inline void ZEND_FASTCALL gc_remove_compressed(zend_refcounted *ref, uint32_t idx)
{
	gc_root_buffer *root = gc_decompress(ref, idx);
	gc_remove_from_roots(root);
}

ZEND_API void ZEND_FASTCALL gc_remove_from_buffer(zend_refcounted *ref)
{
	gc_root_buffer *root;
	uint32_t idx = GC_REF_ADDRESS(ref);

	GC_BENCH_INC(zval_remove_from_buffer);

	if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
		GC_TRACE_SET_COLOR(ref, GC_BLACK);
	}
	GC_REF_SET_INFO(ref, 0);

	/* Perform decompression only in case of large buffers */
	if (UNEXPECTED(GC_G(first_unused) >= GC_MAX_UNCOMPRESSED)) {
		gc_remove_compressed(ref, idx);
		return;
	}

	ZEND_ASSERT(idx);
	root = GC_IDX2PTR(idx);
	gc_remove_from_roots(root);
}

static void gc_scan_black(zend_refcounted *ref)
{
	HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	ht = NULL;
	GC_REF_SET_BLACK(ref);

	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object_get_gc_t get_gc;
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED) &&
		             (get_gc = obj->handlers->get_gc) != NULL)) {
			int n;
			zval *zv, *end;
			zval tmp;

			ZVAL_OBJ(&tmp, obj);
			ht = get_gc(&tmp, &zv, &n);
			end = zv + n;
			if (EXPECTED(!ht)) {
				if (!n) return;
				while (!Z_REFCOUNTED_P(--end)) {
					if (zv == end) return;
				}
			}
			while (zv != end) {
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					GC_ADDREF(ref);
					if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
						gc_scan_black(ref);
					}
				}
				zv++;
			}
			if (EXPECTED(!ht)) {
				ref = Z_COUNTED_P(zv);
				GC_ADDREF(ref);
				if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
					goto tail_call;
				}
				return;
			}
		} else {
			return;
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		if ((zend_array*)ref != &EG(symbol_table)) {
			ht = (zend_array*)ref;
		} else {
			return;
		}
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			GC_ADDREF(ref);
			if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
				goto tail_call;
			}
		}
		return;
	} else {
		return;
	}

	if (!ht->nNumUsed) return;
	p = ht->arData;
	end = p + ht->nNumUsed;
	while (1) {
		end--;
		zv = &end->val;
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}
		if (Z_REFCOUNTED_P(zv)) {
			break;
		}
		if (p == end) return;
	}
	while (p != end) {
		zv = &p->val;
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}
		if (Z_REFCOUNTED_P(zv)) {
			ref = Z_COUNTED_P(zv);
			GC_ADDREF(ref);
			if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
				gc_scan_black(ref);
			}
		}
		p++;
	}
	zv = &p->val;
	if (Z_TYPE_P(zv) == IS_INDIRECT) {
		zv = Z_INDIRECT_P(zv);
	}
	ref = Z_COUNTED_P(zv);
	GC_ADDREF(ref);
	if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
		goto tail_call;
	}
}

static void gc_mark_grey(zend_refcounted *ref)
{
    HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
		ht = NULL;
		GC_BENCH_INC(zval_marked_grey);
		GC_REF_SET_COLOR(ref, GC_GREY);

		if (GC_TYPE(ref) == IS_OBJECT) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED) &&
		                 (get_gc = obj->handlers->get_gc) != NULL)) {
				int n;
				zval *zv, *end;
				zval tmp;

				ZVAL_OBJ(&tmp, obj);
				ht = get_gc(&tmp, &zv, &n);
				end = zv + n;
				if (EXPECTED(!ht)) {
					if (!n) return;
					while (!Z_REFCOUNTED_P(--end)) {
						if (zv == end) return;
					}
				}
				while (zv != end) {
					if (Z_REFCOUNTED_P(zv)) {
						ref = Z_COUNTED_P(zv);
						GC_DELREF(ref);
						gc_mark_grey(ref);
					}
					zv++;
				}
				if (EXPECTED(!ht)) {
					ref = Z_COUNTED_P(zv);
					GC_DELREF(ref);
					goto tail_call;
				}
			} else {
				return;
			}
		} else if (GC_TYPE(ref) == IS_ARRAY) {
			if (((zend_array*)ref) == &EG(symbol_table)) {
				GC_REF_SET_BLACK(ref);
				return;
			} else {
				ht = (zend_array*)ref;
			}
		} else if (GC_TYPE(ref) == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				GC_DELREF(ref);
				goto tail_call;
			}
			return;
		} else {
			return;
		}

		if (!ht->nNumUsed) return;
		p = ht->arData;
		end = p + ht->nNumUsed;
		while (1) {
			end--;
			zv = &end->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				break;
			}
			if (p == end) return;
		}
		while (p != end) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				GC_DELREF(ref);
				gc_mark_grey(ref);
			}
			p++;
		}
		zv = &p->val;
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}
		ref = Z_COUNTED_P(zv);
		GC_DELREF(ref);
		goto tail_call;
	}
}

/* Two-Finger compaction algorithm */
static void gc_compact(void)
{
	if (GC_G(num_roots) + GC_FIRST_ROOT != GC_G(first_unused)) {
		if (GC_G(num_roots)) {
			gc_root_buffer *free = GC_IDX2PTR(GC_FIRST_ROOT);
			gc_root_buffer *scan = GC_IDX2PTR(GC_G(first_unused) - 1);
			gc_root_buffer *end  = GC_IDX2PTR(GC_G(num_roots));
			uint32_t idx;
			zend_refcounted *p;

			while (free < scan) {
				while (!GC_IS_UNUSED(free->ref)) {
					free++;
				}
				while (GC_IS_UNUSED(scan->ref)) {
					scan--;
				}
				if (scan > free) {
					p = scan->ref;
					free->ref = p;
					p = GC_GET_PTR(p);
					idx = gc_compress(GC_PTR2IDX(free));
					GC_REF_SET_INFO(p, idx | GC_REF_COLOR(p));
					free++;
					scan--;
					if (scan <= end) {
						break;
					}
				}
			}
		}
		GC_G(unused) = GC_INVALID;
		GC_G(first_unused) = GC_G(num_roots) + GC_FIRST_ROOT;
	}
}

static void gc_mark_roots(void)
{
	gc_root_buffer *current, *last;

	gc_compact();

	current = GC_IDX2PTR(GC_FIRST_ROOT);
	last = GC_IDX2PTR(GC_G(first_unused));
	while (current != last) {
		if (GC_IS_ROOT(current->ref)) {
			if (GC_REF_CHECK_COLOR(current->ref, GC_PURPLE)) {
				gc_mark_grey(current->ref);
			}
		}
		current++;
	}
}

static void gc_scan(zend_refcounted *ref)
{
    HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
		if (GC_REFCOUNT(ref) > 0) {
			gc_scan_black(ref);
		} else {
			GC_REF_SET_COLOR(ref, GC_WHITE);
			if (GC_TYPE(ref) == IS_OBJECT) {
				zend_object_get_gc_t get_gc;
				zend_object *obj = (zend_object*)ref;

				if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED) &&
				             (get_gc = obj->handlers->get_gc) != NULL)) {
					int n;
					zval *zv, *end;
					zval tmp;

					ZVAL_OBJ(&tmp, obj);
					ht = get_gc(&tmp, &zv, &n);
					end = zv + n;
					if (EXPECTED(!ht)) {
						if (!n) return;
						while (!Z_REFCOUNTED_P(--end)) {
							if (zv == end) return;
						}
					}
					while (zv != end) {
						if (Z_REFCOUNTED_P(zv)) {
							ref = Z_COUNTED_P(zv);
							gc_scan(ref);
						}
						zv++;
					}
					if (EXPECTED(!ht)) {
						ref = Z_COUNTED_P(zv);
						goto tail_call;
					}
				} else {
					return;
				}
			} else if (GC_TYPE(ref) == IS_ARRAY) {
				if ((zend_array*)ref == &EG(symbol_table)) {
					GC_REF_SET_BLACK(ref);
					return;
				} else {
					ht = (zend_array*)ref;
				}
			} else if (GC_TYPE(ref) == IS_REFERENCE) {
				if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
					ref = Z_COUNTED(((zend_reference*)ref)->val);
					goto tail_call;
				}
				return;
			} else {
				return;
			}

			if (!ht->nNumUsed) return;
			p = ht->arData;
			end = p + ht->nNumUsed;
			while (1) {
				end--;
				zv = &end->val;
				if (Z_TYPE_P(zv) == IS_INDIRECT) {
					zv = Z_INDIRECT_P(zv);
				}
				if (Z_REFCOUNTED_P(zv)) {
					break;
				}
				if (p == end) return;
			}
			while (p != end) {
				zv = &p->val;
				if (Z_TYPE_P(zv) == IS_INDIRECT) {
					zv = Z_INDIRECT_P(zv);
				}
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					gc_scan(ref);
				}
				p++;
			}
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			ref = Z_COUNTED_P(zv);
			goto tail_call;
		}
	}
}

static void gc_scan_roots(void)
{
	gc_root_buffer *current = GC_IDX2PTR(GC_FIRST_ROOT);
	gc_root_buffer *last = GC_IDX2PTR(GC_G(first_unused));

	while (current != last) {
		if (GC_IS_ROOT(current->ref)) {
			gc_scan(current->ref);
		}
		current++;
	}
}

static void gc_add_garbage(zend_refcounted *ref)
{
	uint32_t idx;
	gc_root_buffer *buf;

	if (GC_HAS_UNUSED()) {
		idx = GC_FETCH_UNUSED();
	} else if (GC_HAS_NEXT_UNUSED()) {
		idx = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_grow_root_buffer();
		if (UNEXPECTED(!GC_HAS_NEXT_UNUSED())) {
			return;
		}
		idx = GC_FETCH_NEXT_UNUSED();
	}

	buf = GC_IDX2PTR(idx);
	buf->ref = GC_MAKE_GARBAGE(ref);

	idx = gc_compress(idx);
	GC_REF_SET_INFO(ref, idx | GC_BLACK);
	GC_G(num_roots)++;
}

static int gc_collect_white(zend_refcounted *ref, uint32_t *flags)
{
	int count = 0;
	HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
		ht = NULL;
		GC_REF_SET_BLACK(ref);

		/* don't count references for compatibility ??? */
		if (GC_TYPE(ref) != IS_REFERENCE) {
			count++;
		}

		if (GC_TYPE(ref) == IS_OBJECT) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED) &&
			             (get_gc = obj->handlers->get_gc) != NULL)) {
				int n;
				zval *zv, *end;
				zval tmp;

				/* optimization: color is GC_BLACK (0) */
				if (!GC_INFO(ref)) {
					gc_add_garbage(ref);
				}
				if (obj->handlers->dtor_obj &&
				    ((obj->handlers->dtor_obj != zend_objects_destroy_object) ||
				     (obj->ce->destructor != NULL))) {
					*flags |= GC_HAS_DESTRUCTORS;
				}
				ZVAL_OBJ(&tmp, obj);
				ht = get_gc(&tmp, &zv, &n);
				end = zv + n;
				if (EXPECTED(!ht)) {
					if (!n) return count;
					while (!Z_REFCOUNTED_P(--end)) {
						/* count non-refcounted for compatibility ??? */
						if (Z_TYPE_P(zv) != IS_UNDEF) {
							count++;
						}
						if (zv == end) return count;
					}
				}
				while (zv != end) {
					if (Z_REFCOUNTED_P(zv)) {
						ref = Z_COUNTED_P(zv);
						GC_ADDREF(ref);
						count += gc_collect_white(ref, flags);
					/* count non-refcounted for compatibility ??? */
					} else if (Z_TYPE_P(zv) != IS_UNDEF) {
						count++;
					}
					zv++;
				}
				if (EXPECTED(!ht)) {
					ref = Z_COUNTED_P(zv);
					GC_ADDREF(ref);
					goto tail_call;
				}
			} else {
				return count;
			}
		} else if (GC_TYPE(ref) == IS_ARRAY) {
			/* optimization: color is GC_BLACK (0) */
			if (!GC_INFO(ref)) {
				gc_add_garbage(ref);
			}
			ht = (zend_array*)ref;
		} else if (GC_TYPE(ref) == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				GC_ADDREF(ref);
				goto tail_call;
			}
			return count;
		} else {
			return count;
		}

		if (!ht->nNumUsed) return count;
		p = ht->arData;
		end = p + ht->nNumUsed;
		while (1) {
			end--;
			zv = &end->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				break;
			}
			/* count non-refcounted for compatibility ??? */
			if (Z_TYPE_P(zv) != IS_UNDEF) {
				count++;
			}
			if (p == end) return count;
		}
		while (p != end) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				GC_ADDREF(ref);
				count += gc_collect_white(ref, flags);
				/* count non-refcounted for compatibility ??? */
			} else if (Z_TYPE_P(zv) != IS_UNDEF) {
				count++;
			}
			p++;
		}
		zv = &p->val;
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}
		ref = Z_COUNTED_P(zv);
		GC_ADDREF(ref);
		goto tail_call;
	}
	return count;
}

static int gc_collect_roots(uint32_t *flags)
{
	uint32_t idx, end;
	zend_refcounted *ref;
	int count = 0;
	gc_root_buffer *current = GC_IDX2PTR(GC_FIRST_ROOT);
	gc_root_buffer *last = GC_IDX2PTR(GC_G(first_unused));

	/* remove non-garbage from the list */
	while (current != last) {
		if (GC_IS_ROOT(current->ref)) {
			if (GC_REF_CHECK_COLOR(current->ref, GC_BLACK)) {
				GC_REF_SET_INFO(current->ref, 0); /* reset GC_ADDRESS() and keep GC_BLACK */
				gc_remove_from_roots(current);
			}
		}
		current++;
	}

	gc_compact();

	/* Root buffer might be reallocated during gc_collect_white,
	 * make sure to reload pointers. */
	idx = GC_FIRST_ROOT;
	end = GC_G(first_unused);
	while (idx != end) {
		current = GC_IDX2PTR(idx);
		ref = current->ref;
		ZEND_ASSERT(GC_IS_ROOT(ref));
		current->ref = GC_MAKE_GARBAGE(ref);
		if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
			count += gc_collect_white(ref, flags);
		}
		idx++;
	}

	return count;
}

static void gc_remove_nested_data_from_buffer(zend_refcounted *ref, gc_root_buffer *root)
{
	HashTable *ht = NULL;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (root ||
	    (GC_REF_ADDRESS(ref) != 0 &&
	     GC_REF_CHECK_COLOR(ref, GC_BLACK))) {
		GC_TRACE_REF(ref, "removing from buffer");
		if (root) {
			gc_remove_from_roots(root);
			GC_REF_SET_INFO(ref, 0);
			root = NULL;
		} else {
			GC_REMOVE_FROM_BUFFER(ref);
		}

		if (GC_TYPE(ref) == IS_OBJECT) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED) &&
		                 (get_gc = obj->handlers->get_gc) != NULL)) {
				int n;
				zval *zv, *end;
				zval tmp;

				ZVAL_OBJ(&tmp, obj);
				ht = get_gc(&tmp, &zv, &n);
				end = zv + n;
				if (EXPECTED(!ht)) {
					if (!n) return;
					while (!Z_REFCOUNTED_P(--end)) {
						if (zv == end) return;
					}
				}
				while (zv != end) {
					if (Z_REFCOUNTED_P(zv)) {
						ref = Z_COUNTED_P(zv);
						gc_remove_nested_data_from_buffer(ref, NULL);
					}
					zv++;
				}
				if (EXPECTED(!ht)) {
					ref = Z_COUNTED_P(zv);
					goto tail_call;
				}
			} else {
				return;
			}
		} else if (GC_TYPE(ref) == IS_ARRAY) {
			ht = (zend_array*)ref;
		} else if (GC_TYPE(ref) == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				goto tail_call;
			}
			return;
		} else {
			return;
		}

		if (!ht->nNumUsed) return;
		p = ht->arData;
		end = p + ht->nNumUsed;
		while (1) {
			end--;
			zv = &end->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				break;
			}
			if (p == end) return;
		}
		while (p != end) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				gc_remove_nested_data_from_buffer(ref, NULL);
			}
			p++;
		}
		zv = &p->val;
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}
		ref = Z_COUNTED_P(zv);
		goto tail_call;
	}
}

ZEND_API int zend_gc_collect_cycles(void)
{
	int count = 0;

	if (GC_G(num_roots)) {
		gc_root_buffer *current, *last;
		zend_refcounted *p;
		uint32_t gc_flags = 0;
		uint32_t idx, end;

		if (GC_G(gc_active)) {
			return 0;
		}

		GC_TRACE("Collecting cycles");
		GC_G(gc_runs)++;
		GC_G(gc_active) = 1;

		GC_TRACE("Marking roots");
		gc_mark_roots();
		GC_TRACE("Scanning roots");
		gc_scan_roots();

		GC_TRACE("Collecting roots");
		count = gc_collect_roots(&gc_flags);

		if (!GC_G(num_roots)) {
			/* nothing to free */
			GC_TRACE("Nothing to free");
			GC_G(gc_active) = 0;
			return 0;
		}

		end = GC_G(first_unused);

		if (gc_flags & GC_HAS_DESTRUCTORS) {
			uint32_t *refcounts;

			GC_TRACE("Calling destructors");

			// TODO: may be use emalloc() ???
			refcounts = pemalloc(sizeof(uint32_t) * end, 1);

			/* Remember reference counters before calling destructors */
			idx = GC_FIRST_ROOT;
			current = GC_IDX2PTR(GC_FIRST_ROOT);
			while (idx != end) {
				if (GC_IS_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					refcounts[idx] = GC_REFCOUNT(p);
				}
				current++;
				idx++;
			}

			/* Call destructors
			 *
			 * The root buffer might be reallocated during destructors calls,
			 * make sure to reload pointers as necessary. */
			idx = GC_FIRST_ROOT;
			while (idx != end) {
				current = GC_IDX2PTR(idx);
				if (GC_IS_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					if (GC_TYPE(p) == IS_OBJECT
					 && !(OBJ_FLAGS(p) & IS_OBJ_DESTRUCTOR_CALLED)) {
						zend_object *obj = (zend_object*)p;

						GC_TRACE_REF(obj, "calling destructor");
						GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
						if (obj->handlers->dtor_obj
						 && (obj->handlers->dtor_obj != zend_objects_destroy_object
						  || obj->ce->destructor)) {
							GC_ADDREF(obj);
							obj->handlers->dtor_obj(obj);
							GC_DELREF(obj);
						}
					}
				}
				idx++;
			}

			/* Remove values captured in destructors */
			idx = GC_FIRST_ROOT;
			current = GC_IDX2PTR(GC_FIRST_ROOT);
			while (idx != end) {
				if (GC_IS_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					if (GC_REFCOUNT(p) > refcounts[idx]) {
						gc_remove_nested_data_from_buffer(p, current);
					}
				}
				current++;
				idx++;
			}

			pefree(refcounts, 1);

			if (GC_G(gc_protected)) {
				/* something went wrong */
				return 0;
			}
		}

		/* Destroy zvals */
		GC_TRACE("Destroying zvals");
		GC_G(gc_protected) = 1;
		current = GC_IDX2PTR(GC_FIRST_ROOT);
		last = GC_IDX2PTR(GC_G(first_unused));
		while (current != last) {
			if (GC_IS_GARBAGE(current->ref)) {
				p = GC_GET_PTR(current->ref);
				GC_TRACE_REF(p, "destroying");
				if (GC_TYPE(p) == IS_OBJECT) {
					zend_object *obj = (zend_object*)p;

					EG(objects_store).object_buckets[obj->handle] = SET_OBJ_INVALID(obj);
					GC_TYPE_INFO(obj) = IS_NULL |
						(GC_TYPE_INFO(obj) & ~GC_TYPE_MASK);
					if (!(OBJ_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {
						GC_ADD_FLAGS(obj, IS_OBJ_FREE_CALLED);
						if (obj->handlers->free_obj) {
							GC_ADDREF(obj);
							obj->handlers->free_obj(obj);
							GC_DELREF(obj);
						}
					}

					ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(obj->handle);
					current->ref = GC_MAKE_GARBAGE(((char*)obj) - obj->handlers->offset);
				} else if (GC_TYPE(p) == IS_ARRAY) {
					zend_array *arr = (zend_array*)p;

					GC_TYPE_INFO(arr) = IS_NULL |
						(GC_TYPE_INFO(arr) & ~GC_TYPE_MASK);

					/* GC may destroy arrays with rc>1. This is valid and safe. */
					HT_ALLOW_COW_VIOLATION(arr);

					zend_hash_destroy(arr);
				}
			}
			current++;
		}

		/* Free objects */
		current = GC_IDX2PTR(GC_FIRST_ROOT);
		while (current != last) {
			if (GC_IS_GARBAGE(current->ref)) {
				p = GC_GET_PTR(current->ref);
				GC_LINK_UNUSED(current);
				GC_G(num_roots)--;
				efree(p);
			}
			current++;
		}

		GC_TRACE("Collection finished");
		GC_G(collected) += count;
		GC_G(gc_protected) = 0;
		GC_G(gc_active) = 0;
	}

	gc_compact();

	return count;
}

ZEND_API void zend_gc_get_status(zend_gc_status *status)
{
	status->runs = GC_G(gc_runs);
	status->collected = GC_G(collected);
	status->threshold = GC_G(gc_threshold);
	status->num_roots = GC_G(num_roots);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 *
 * vim:noexpandtab:
 */
