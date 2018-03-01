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

/* GC buffer size */
#define GC_INVALID           0
#define GC_FIRST_REAL_ROOT   1

#define GC_DEFAULT_BUF_SIZE  (16 * 1024)
#define GC_BUF_GROW_STEP     (128 * 1024)

#define GC_COMPRESS_FACTOR   4096 /* shold be 0 to disable compression */
#define GC_MAX_UNCOMPRESSED  ((1024 * 1024) - GC_COMPRESS_FACTOR)
#define GC_MAX_BUF_SIZE      (GC_COMPRESS_FACTOR ? (0x40000000) : GC_MAX_UNCOMPRESSED)

#define GC_NEED_COMPRESSION(addr) \
	((GC_COMPRESS_FACTOR > 0) && (addr >= GC_MAX_UNCOMPRESSED))

#define GC_THRESHOLD_DEFAULT 10000
#define GC_THRESHOLD_STEP    10000
#define GC_THRESHOLD_MAX     (GC_COMPRESS_FACTOR ? 1000000000 : 1000000)
#define GC_THRESHOLD_TRIGGER 100

/* GC flags */
#define GC_HAS_DESTRUCTORS  (1<<0)

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
	uint32_t          buf_size;			/* size of the GC buffer            */
	uint32_t          gc_threshold;     /* GC collection threshold          */
	uint32_t          unused;			/* linked list of unused buffers    */
	uint32_t          first_unused;		/* first unused buffer              */
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
	ZEND_ASSERT(GC_NEED_COMPRESSION(idx));
	return GC_MAX_UNCOMPRESSED + ((idx - GC_MAX_UNCOMPRESSED) % GC_COMPRESS_FACTOR);
}

static zend_always_inline gc_root_buffer* gc_decompress(zend_refcounted *ref, uint32_t idx)
{
	ZEND_ASSERT(GC_NEED_COMPRESSION(idx));
	while (idx < GC_G(first_unused)) {
		gc_root_buffer *root = GC_G(buf) + idx;

		if (root->ref == ref) {
			return root;
		}
		idx += GC_COMPRESS_FACTOR;
	}
	ZEND_ASSERT(0);
	return NULL;
}

static zend_always_inline uint32_t gc_fetch_unused(void)
{
	uint32_t addr;
	gc_root_buffer *root;

	ZEND_ASSERT(GC_HAS_UNUSED());
	addr = GC_G(unused);
	root = GC_G(buf) + addr;
	ZEND_ASSERT(GC_IS_UNUSED(root->ref));
	/* optimization: GC_GET_PTR(root->ref) is not necessary because it shifted anyway */
	GC_G(unused) = (uint32_t)(uintptr_t)root->ref / sizeof(void*);
	return addr;
}

static zend_always_inline void gc_link_unused(gc_root_buffer *root)
{
	root->ref = (void*)(uintptr_t)((GC_G(unused) * sizeof(void*)) | GC_UNUSED);
	GC_G(unused) = root - GC_G(buf);
}

static zend_always_inline uint32_t gc_fetch_next_unused(void)
{
	uint32_t addr;

	ZEND_ASSERT(GC_HAS_NEXT_UNUSED());
	addr = GC_G(first_unused);
	GC_G(first_unused)++;
	return addr;
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
	gc_globals->buf_size = 0;
	gc_globals->gc_threshold = 0;
	gc_globals->unused = GC_INVALID;
	gc_globals->first_unused = 0;
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

ZEND_API void gc_globals_ctor(void)
{
#ifdef ZTS
	ts_allocate_id(&gc_globals_id, sizeof(zend_gc_globals), (ts_allocate_ctor) gc_globals_ctor_ex, (ts_allocate_dtor) root_buffer_dtor);
#else
	gc_globals_ctor_ex(&gc_globals);
#endif
}

ZEND_API void gc_globals_dtor(void)
{
#ifndef ZTS
	root_buffer_dtor(&gc_globals);
#endif
}

ZEND_API void gc_reset(void)
{
	if (GC_G(buf)) {
		GC_G(gc_active) = 0;
		GC_G(gc_protected) = 0;
		GC_G(gc_full) = 0;
		GC_G(unused) = GC_INVALID;
		GC_G(first_unused) = GC_FIRST_REAL_ROOT;
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

ZEND_API zend_bool gc_set_enabled(zend_bool enable)
{
	zend_bool old_enabled = GC_G(gc_enabled);
	GC_G(gc_enabled) = enable;
	if (enable && !old_enabled && GC_G(buf) == NULL) {
		GC_G(buf) = (gc_root_buffer*) malloc(sizeof(gc_root_buffer) * GC_DEFAULT_BUF_SIZE);
		GC_G(buf_size) = GC_DEFAULT_BUF_SIZE;
		GC_G(gc_threshold) = GC_THRESHOLD_DEFAULT + GC_FIRST_REAL_ROOT;
		gc_reset();
	}
	return old_enabled;
}

ZEND_API zend_bool gc_enabled(void)
{
	return GC_G(gc_enabled);
}

static void gc_grow_root_buffer(void)
{
	size_t new_size;

	if (GC_G(buf_size) >= GC_MAX_BUF_SIZE) {
		if (!GC_G(gc_full)) {
			zend_error(E_WARNING, "GC buffer overflow (GC disabled)\n");
			//???GC_G(gc_enabled) = 0;
			GC_G(gc_active) = 1;
			GC_G(gc_protected) = 1;
			GC_G(gc_full) = 1;
			CG(unclean_shutdown) = 1;
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

static zend_never_inline void ZEND_FASTCALL gc_add_compressed(zend_refcounted *ref, uint32_t addr)
{
	addr = gc_compress(addr);

	GC_REF_SET_INFO(ref, addr | GC_PURPLE);
	GC_G(num_roots)++;

	GC_BENCH_INC(zval_buffered);
	GC_BENCH_INC(root_buf_length);
	GC_BENCH_PEAK(root_buf_peak, root_buf_length);
}

static zend_never_inline void ZEND_FASTCALL gc_possible_root_when_full(zend_refcounted *ref)
{
	uint32_t addr;
	gc_root_buffer *newRoot;

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	ZEND_ASSERT(GC_INFO(ref) == 0);

	if (GC_G(gc_enabled) && !GC_G(gc_active)) {
		GC_ADDREF(ref);
		gc_adjust_threshold(gc_collect_cycles());
		if (UNEXPECTED(GC_DELREF(ref)) == 0) {
			zval_dtor_func(ref);
			return;
		} else if (UNEXPECTED(GC_INFO(ref))) {
			return;
		}
	}

	if (GC_HAS_UNUSED()) {
		addr = GC_FETCH_UNUSED();
	} else if (EXPECTED(GC_HAS_NEXT_UNUSED())) {
		addr = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_grow_root_buffer();
		ZEND_ASSERT(GC_HAS_NEXT_UNUSED());
		addr = GC_FETCH_NEXT_UNUSED();
	}

	newRoot = GC_G(buf) + addr;
	newRoot->ref = ref; /* GC_ROOT tag is 0 */
	GC_TRACE_SET_COLOR(ref, GC_PURPLE);

	if (UNEXPECTED(GC_NEED_COMPRESSION(addr))) {
		gc_add_compressed(ref, addr);
		return;
	}

	GC_REF_SET_INFO(ref, addr | GC_PURPLE);
	GC_G(num_roots)++;

	GC_BENCH_INC(zval_buffered);
	GC_BENCH_INC(root_buf_length);
	GC_BENCH_PEAK(root_buf_peak, root_buf_length);
}

ZEND_API void ZEND_FASTCALL gc_possible_root(zend_refcounted *ref)
{
	uint32_t addr;
	gc_root_buffer *newRoot;

	if (UNEXPECTED(GC_G(gc_protected)) || UNEXPECTED(CG(unclean_shutdown))) {
		return;
	}

	GC_BENCH_INC(zval_possible_root);

	if (EXPECTED(GC_HAS_UNUSED())) {
		addr = GC_FETCH_UNUSED();
	} else if (EXPECTED(GC_HAS_NEXT_UNUSED_UNDER_THRESHOLD())) {
		addr = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_possible_root_when_full(ref);
		return;
	}

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	ZEND_ASSERT(GC_INFO(ref) == 0);

	newRoot = GC_G(buf) + addr;
	newRoot->ref = ref; /* GC_ROOT tag is 0 */
	GC_TRACE_SET_COLOR(ref, GC_PURPLE);

	if (UNEXPECTED(GC_NEED_COMPRESSION(addr))) {
		gc_add_compressed(ref, addr);
		return;
	}

	GC_REF_SET_INFO(ref, addr | GC_PURPLE);
	GC_G(num_roots)++;

	GC_BENCH_INC(zval_buffered);
	GC_BENCH_INC(root_buf_length);
	GC_BENCH_PEAK(root_buf_peak, root_buf_length);
}

static zend_never_inline void ZEND_FASTCALL gc_remove_compressed(zend_refcounted *ref, uint32_t addr)
{
	gc_root_buffer *root = gc_decompress(ref, addr);
	gc_remove_from_roots(root);
}

ZEND_API void ZEND_FASTCALL gc_remove_from_buffer(zend_refcounted *ref)
{
	gc_root_buffer *root;
	uint32_t addr = GC_REF_ADDRESS(ref);

	ZEND_ASSERT(addr);

	GC_BENCH_INC(zval_remove_from_buffer);

	if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
		GC_TRACE_SET_COLOR(ref, GC_BLACK);
	}
	GC_REF_SET_INFO(ref, 0);

	if (UNEXPECTED(GC_NEED_COMPRESSION(addr))) {
		gc_remove_compressed(ref, addr);
		return;
	}

	root = GC_G(buf) + addr;
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
	if (GC_G(num_roots) + GC_FIRST_REAL_ROOT != GC_G(first_unused)) {
		if (GC_G(num_roots)) {
			gc_root_buffer *buf = GC_G(buf);
			uint32_t free = GC_FIRST_REAL_ROOT;
			uint32_t scan = GC_G(first_unused) - 1;
			uint32_t addr;
			zend_refcounted *p;

			while (free < scan) {
				while (!GC_IS_UNUSED(buf[free].ref)) {
					free++;
				}
				while (GC_IS_UNUSED(buf[scan].ref)) {
					scan--;
				}
				if (scan > free) {
					p = buf[scan].ref;
					buf[free].ref = p;
					p = GC_GET_PTR(p);
					addr = free;
					if (UNEXPECTED(GC_NEED_COMPRESSION(addr))) {
						addr = gc_compress(addr);
					}
					GC_REF_SET_INFO(p, addr | GC_REF_COLOR(p));
					free++;
					scan--;
					if (scan <= GC_G(num_roots)) {
						break;
					}
				} else {
					break;
				}
			}
		}
		GC_G(unused) = GC_INVALID;
		GC_G(first_unused) = GC_G(num_roots) + GC_FIRST_REAL_ROOT;
	}
}

static void gc_mark_roots(void)
{
	gc_root_buffer *current, *last;

	gc_compact();

	current = GC_G(buf) + GC_FIRST_REAL_ROOT;
	last = GC_G(buf) + GC_G(first_unused);
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
	gc_root_buffer *current = GC_G(buf) + GC_FIRST_REAL_ROOT;
	gc_root_buffer *last = GC_G(buf) + GC_G(first_unused);

	while (current != last) {
		if (GC_IS_ROOT(current->ref)) {
			gc_scan(current->ref);
		}
		current++;
	}
}

static void gc_add_garbage(zend_refcounted *ref)
{
	uint32_t addr;
	gc_root_buffer *buf;

	if (GC_HAS_UNUSED()) {
		addr = GC_FETCH_UNUSED();
	} else if (GC_HAS_NEXT_UNUSED()) {
		addr = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_grow_root_buffer();
		ZEND_ASSERT(GC_HAS_NEXT_UNUSED());
		addr = GC_FETCH_NEXT_UNUSED();
	}

	buf = GC_G(buf) + addr;
	buf->ref = GC_MAKE_GARBAGE(ref);

	if (UNEXPECTED(GC_NEED_COMPRESSION(addr))) {
		addr = gc_compress(addr);
	}
	GC_REF_SET_INFO(ref, addr | GC_BLACK);
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
	uint32_t n, end;
	zend_refcounted *ref;
	int count = 0;
	gc_root_buffer *current = GC_G(buf) + GC_FIRST_REAL_ROOT;
	gc_root_buffer *last = GC_G(buf) + GC_G(first_unused);

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
	n = GC_FIRST_REAL_ROOT;
	end = GC_G(first_unused);
	while (n != end) {
		current = GC_G(buf) + n;
		ref = current->ref;
		if (GC_IS_ROOT(ref)) {
			if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
				current->ref = GC_MAKE_GARBAGE(ref);
				count += gc_collect_white(ref, flags);
			}
		}
		n++;
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
		uint32_t n, end;

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
			refcounts = pemalloc(sizeof(uint32_t) * GC_G(first_unused), 1);

			/* Remember reference counters before calling destructors */
			n = GC_FIRST_REAL_ROOT;
			current = GC_G(buf) + GC_FIRST_REAL_ROOT;
			while (n != end) {
				if (GC_IS_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					refcounts[n] = GC_REFCOUNT(p);
				}
				current++;
				n++;
			}

			/* Call destructors
			 *
			 * The root buffer might be reallocated during destructors calls,
			 * make sure to reload pointers as necessary. */
			n = GC_FIRST_REAL_ROOT;
			while (n != end) {
				current = GC_G(buf) + n;
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
				n++;
			}

			/* Remove values captured in destructors */
			n = GC_FIRST_REAL_ROOT;
			current = GC_G(buf) + GC_FIRST_REAL_ROOT;
			while (n != end) {
				if (GC_IS_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					if (GC_REFCOUNT(p) > refcounts[n]) {
						gc_remove_nested_data_from_buffer(p, current);
					}
				}
				current++;
				n++;
			}

			pefree(refcounts, 1);
		}

		/* Destroy zvals */
		GC_TRACE("Destroying zvals");
		GC_G(gc_protected) = 1;
		current = GC_G(buf) + GC_FIRST_REAL_ROOT;
		last = GC_G(buf) + GC_G(first_unused);
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
					SET_OBJ_BUCKET_NUMBER(EG(objects_store).object_buckets[obj->handle], EG(objects_store).free_list_head);
					EG(objects_store).free_list_head = obj->handle;
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
		current = GC_G(buf) + GC_FIRST_REAL_ROOT;
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
