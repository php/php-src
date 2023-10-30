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
 * RED   - Candidate cycle undergoing
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
 * gc_scan checks the colors of possible members.
 *
 * If the node is marked as grey and the refcount > 0
 *    gc_scan_black will be called on that node to scan it's subgraph.
 * otherwise (refcount == 0), it marks the node white.
 *
 * A node MAY be added to possible roots when ZEND_UNSET_VAR happens or
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
#include "zend_fibers.h"
#include "zend_hrtime.h"
#include "zend_weakrefs.h"

#ifndef GC_BENCH
# define GC_BENCH 0
#endif

#ifndef ZEND_GC_DEBUG
# define ZEND_GC_DEBUG 0
#endif

/* GC_INFO layout */
#define GC_ADDRESS  0x0fffffu
#define GC_COLOR    0x300000u

#define GC_BLACK    0x000000u /* must be zero */
#define GC_WHITE    0x100000u
#define GC_GREY     0x200000u
#define GC_PURPLE   0x300000u

/* Debug tracing */
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
		GC_TRACE_SET_COLOR(ref, c); \
		GC_TYPE_INFO(ref) = \
			(GC_TYPE_INFO(ref) & ~(GC_COLOR << GC_INFO_SHIFT)) | \
			((c) << GC_INFO_SHIFT); \
	} while (0)

#define GC_REF_SET_BLACK(ref) do { \
		GC_TRACE_SET_COLOR(ref, GC_BLACK); \
		GC_TYPE_INFO(ref) &= ~(GC_COLOR << GC_INFO_SHIFT); \
	} while (0)

#define GC_REF_SET_PURPLE(ref) do { \
		GC_TRACE_SET_COLOR(ref, GC_PURPLE); \
		GC_TYPE_INFO(ref) |= (GC_COLOR << GC_INFO_SHIFT); \
	} while (0)

/* bit stealing tags for gc_root_buffer.ref */
#define GC_BITS    0x3

#define GC_ROOT    0x0 /* possible root of circular garbage     */
#define GC_UNUSED  0x1 /* part of linked list of unused buffers */
#define GC_GARBAGE 0x2 /* garbage to delete                     */
#define GC_DTOR_GARBAGE 0x3 /* garbage on which only the dtor should be invoked */

#define GC_GET_PTR(ptr) \
	((void*)(((uintptr_t)(ptr)) & ~GC_BITS))

#define GC_IS_ROOT(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_ROOT)
#define GC_IS_UNUSED(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_UNUSED)
#define GC_IS_GARBAGE(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_GARBAGE)
#define GC_IS_DTOR_GARBAGE(ptr) \
	((((uintptr_t)(ptr)) & GC_BITS) == GC_DTOR_GARBAGE)

#define GC_MAKE_GARBAGE(ptr) \
	((void*)(((uintptr_t)(ptr)) | GC_GARBAGE))
#define GC_MAKE_DTOR_GARBAGE(ptr) \
	((void*)(((uintptr_t)(ptr)) | GC_DTOR_GARBAGE))

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

#define GC_MAX_UNCOMPRESSED  (512 * 1024)
#define GC_MAX_BUF_SIZE      0x40000000

#define GC_THRESHOLD_DEFAULT (10000 + GC_FIRST_ROOT)
#define GC_THRESHOLD_STEP    10000
#define GC_THRESHOLD_MAX     1000000000
#define GC_THRESHOLD_TRIGGER 100

/* GC flags */
#define GC_HAS_DESTRUCTORS  (1<<0)

/* Weak maps */
#define Z_FROM_WEAKMAP_KEY		(1<<0)
#define Z_FROM_WEAKMAP			(1<<1)

/* The WeakMap entry zv is reachable from roots by following the virtual
 * reference from the a WeakMap key to the entry */
#define GC_FROM_WEAKMAP_KEY(zv) \
	(Z_TYPE_INFO_P((zv)) & (Z_FROM_WEAKMAP_KEY << Z_TYPE_INFO_EXTRA_SHIFT))

#define GC_SET_FROM_WEAKMAP_KEY(zv) do {									   \
	zval *_z = (zv);														   \
	Z_TYPE_INFO_P(_z) = Z_TYPE_INFO_P(_z) | (Z_FROM_WEAKMAP_KEY << Z_TYPE_INFO_EXTRA_SHIFT); \
} while (0)

#define GC_UNSET_FROM_WEAKMAP_KEY(zv) do {									   \
	zval *_z = (zv);														   \
	Z_TYPE_INFO_P(_z) = Z_TYPE_INFO_P(_z) & ~(Z_FROM_WEAKMAP_KEY << Z_TYPE_INFO_EXTRA_SHIFT); \
} while (0)

/* The WeakMap entry zv is reachable from roots by following the reference from
 * the WeakMap */
#define GC_FROM_WEAKMAP(zv) \
	(Z_TYPE_INFO_P((zv)) & (Z_FROM_WEAKMAP << Z_TYPE_INFO_EXTRA_SHIFT))

#define GC_SET_FROM_WEAKMAP(zv) do {									       \
	zval *_z = (zv);														   \
	Z_TYPE_INFO_P(_z) = Z_TYPE_INFO_P(_z) | (Z_FROM_WEAKMAP << Z_TYPE_INFO_EXTRA_SHIFT); \
} while (0)

#define GC_UNSET_FROM_WEAKMAP(zv) do {										   \
	zval *_z = (zv);														   \
	Z_TYPE_INFO_P(_z) = Z_TYPE_INFO_P(_z) & ~(Z_FROM_WEAKMAP << Z_TYPE_INFO_EXTRA_SHIFT); \
} while (0)

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
	gc_root_buffer   *buf;				/* preallocated arrays of buffers   */

	bool         gc_enabled;
	bool         gc_active;        /* GC currently running, forbid nested GC */
	bool         gc_protected;     /* GC protected, forbid root additions */
	bool         gc_full;

	uint32_t          unused;			/* linked list of unused buffers    */
	uint32_t          first_unused;		/* first unused buffer              */
	uint32_t          gc_threshold;     /* GC collection threshold          */
	uint32_t          buf_size;			/* size of the GC buffer            */
	uint32_t          num_roots;		/* number of roots in GC buffer     */

	uint32_t gc_runs;
	uint32_t collected;

	zend_hrtime_t activated_at;
	zend_hrtime_t collector_time;
	zend_hrtime_t dtor_time;
	zend_hrtime_t free_time;

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
static size_t gc_globals_offset;
#define GC_G(v) ZEND_TSRMG_FAST(gc_globals_offset, zend_gc_globals *, v)
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


#define GC_STACK_SEGMENT_SIZE (((4096 - ZEND_MM_OVERHEAD) / sizeof(void*)) - 2)

typedef struct _gc_stack gc_stack;

struct _gc_stack {
	gc_stack        *prev;
	gc_stack        *next;
	zend_refcounted *data[GC_STACK_SEGMENT_SIZE];
};

#define GC_STACK_DCL(init) \
	gc_stack *_stack = init; \
	size_t    _top = 0;

#define GC_STACK_PUSH(ref) \
	gc_stack_push(&_stack, &_top, ref);

#define GC_STACK_POP() \
	gc_stack_pop(&_stack, &_top)

static zend_never_inline gc_stack* gc_stack_next(gc_stack *stack)
{
	if (UNEXPECTED(!stack->next)) {
		gc_stack *segment = emalloc(sizeof(gc_stack));
		segment->prev = stack;
		segment->next = NULL;
		stack->next = segment;
	}
	return stack->next;
}

static zend_always_inline void gc_stack_push(gc_stack **stack, size_t *top, zend_refcounted *ref)
{
	if (UNEXPECTED(*top == GC_STACK_SEGMENT_SIZE)) {
		(*stack) = gc_stack_next(*stack);
		(*top) = 0;
	}
	(*stack)->data[(*top)++] = ref;
}

static zend_always_inline zend_refcounted* gc_stack_pop(gc_stack **stack, size_t *top)
{
	if (UNEXPECTED((*top) == 0)) {
		if (!(*stack)->prev) {
			return NULL;
		} else {
			(*stack) = (*stack)->prev;
			(*top) = GC_STACK_SEGMENT_SIZE - 1;
			return (*stack)->data[GC_STACK_SEGMENT_SIZE - 1];
		}
	} else {
		return (*stack)->data[--(*top)];
	}
}

static void gc_stack_free(gc_stack *stack)
{
	gc_stack *p = stack->next;

	while (p) {
		stack = p->next;
		efree(p);
		p = stack;
	}
}

static zend_always_inline uint32_t gc_compress(uint32_t idx)
{
	if (EXPECTED(idx < GC_MAX_UNCOMPRESSED)) {
		return idx;
	}
	return (idx % GC_MAX_UNCOMPRESSED) | GC_MAX_UNCOMPRESSED;
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
			GC_TYPE(ref) == IS_REFERENCE
				? "reference" : zend_get_type_by_const(GC_TYPE(ref)));
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
	gc_globals->collector_time = 0;
	gc_globals->dtor_time = 0;
	gc_globals->free_time = 0;
	gc_globals->activated_at = 0;

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
	ts_allocate_fast_id(&gc_globals_id, &gc_globals_offset, sizeof(zend_gc_globals), (ts_allocate_ctor) gc_globals_ctor_ex, (ts_allocate_dtor) root_buffer_dtor);
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

		GC_G(collector_time) = 0;
		GC_G(dtor_time) = 0;
		GC_G(free_time) = 0;

#if GC_BENCH
		GC_G(root_buf_length) = 0;
		GC_G(root_buf_peak) = 0;
		GC_G(zval_possible_root) = 0;
		GC_G(zval_buffered) = 0;
		GC_G(zval_remove_from_buffer) = 0;
		GC_G(zval_marked_grey) = 0;
#endif
	}

	GC_G(activated_at) = zend_hrtime();
}

ZEND_API bool gc_enable(bool enable)
{
	bool old_enabled = GC_G(gc_enabled);
	GC_G(gc_enabled) = enable;
	if (enable && !old_enabled && GC_G(buf) == NULL) {
		GC_G(buf) = (gc_root_buffer*) pemalloc(sizeof(gc_root_buffer) * GC_DEFAULT_BUF_SIZE, 1);
		GC_G(buf)[0].ref = NULL;
		GC_G(buf_size) = GC_DEFAULT_BUF_SIZE;
		GC_G(gc_threshold) = GC_THRESHOLD_DEFAULT;
		gc_reset();
	}
	return old_enabled;
}

ZEND_API bool gc_enabled(void)
{
	return GC_G(gc_enabled);
}

ZEND_API bool gc_protect(bool protect)
{
	bool old_protected = GC_G(gc_protected);
	GC_G(gc_protected) = protect;
	return old_protected;
}

ZEND_API bool gc_protected(void)
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
		if (UNEXPECTED(GC_DELREF(ref) == 0)) {
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

static void ZEND_FASTCALL gc_extra_root(zend_refcounted *ref)
{
	uint32_t idx;
	gc_root_buffer *newRoot;

	if (EXPECTED(GC_HAS_UNUSED())) {
		idx = GC_FETCH_UNUSED();
	} else if (EXPECTED(GC_HAS_NEXT_UNUSED_UNDER_THRESHOLD())) {
		idx = GC_FETCH_NEXT_UNUSED();
	} else {
		gc_grow_root_buffer();
		if (UNEXPECTED(!GC_HAS_NEXT_UNUSED())) {
			/* TODO: can this really happen? */
			return;
		}
		idx = GC_FETCH_NEXT_UNUSED();
	}

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	ZEND_ASSERT(GC_REF_ADDRESS(ref) == 0);

	newRoot = GC_IDX2PTR(idx);
	newRoot->ref = ref; /* GC_ROOT tag is 0 */

	idx = gc_compress(idx);
	GC_REF_SET_INFO(ref, idx | GC_REF_COLOR(ref));
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

static void gc_scan_black(zend_refcounted *ref, gc_stack *stack)
{
	HashTable *ht;
	Bucket *p;
	zval *zv;
	uint32_t n;
	GC_STACK_DCL(stack);

tail_call:
	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED))) {
			zval *table;
			int len;

			if (UNEXPECTED(GC_FLAGS(obj) & IS_OBJ_WEAKLY_REFERENCED)) {
				zend_weakmap_get_object_key_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n-=2) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					zval *weakmap = zv+1;
					ZEND_ASSERT(Z_REFCOUNTED_P(weakmap));
					if (Z_OPT_REFCOUNTED_P(entry)) {
						GC_UNSET_FROM_WEAKMAP_KEY(entry);
						if (GC_REF_CHECK_COLOR(Z_COUNTED_P(weakmap), GC_GREY)) {
							/* Weakmap was scanned in gc_mark_roots, we must
							 * ensure that it's eventually scanned in
							 * gc_scan_roots as well. */
							if (!GC_REF_ADDRESS(Z_COUNTED_P(weakmap))) {
								gc_extra_root(Z_COUNTED_P(weakmap));
							}
						} else if (/* GC_REF_CHECK_COLOR(Z_COUNTED_P(weakmap), GC_BLACK) && */ !GC_FROM_WEAKMAP(entry)) {
							/* Both the entry weakmap and key are BLACK, so we
							 * can mark the entry BLACK as well.
							 * !GC_FROM_WEAKMAP(entry) means that the weakmap
							 * was already scanned black (or will not be
							 * scanned), so it's our responsibility to mark the
							 * entry */
							ZEND_ASSERT(GC_REF_CHECK_COLOR(Z_COUNTED_P(weakmap), GC_BLACK));
							ref = Z_COUNTED_P(entry);
							GC_ADDREF(ref);
							if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
								GC_REF_SET_BLACK(ref);
								GC_STACK_PUSH(ref);
							}
						}
					}
					zv+=2;
				}
			}

			if (UNEXPECTED(obj->handlers->get_gc == zend_weakmap_get_gc)) {
				zend_weakmap_get_key_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n-=2) {
					ZEND_ASSERT(Z_TYPE_P(zv+1) == IS_PTR);
					zval *key = zv;
					zval *entry = (zval*) Z_PTR_P(zv+1);
					if (Z_OPT_REFCOUNTED_P(entry)) {
						GC_UNSET_FROM_WEAKMAP(entry);
						if (GC_REF_CHECK_COLOR(Z_COUNTED_P(key), GC_GREY)) {
							/* Key was scanned in gc_mark_roots, we must
							 * ensure that it's eventually scanned in
							 * gc_scan_roots as well. */
							if (!GC_REF_ADDRESS(Z_COUNTED_P(key))) {
								gc_extra_root(Z_COUNTED_P(key));
							}
						} else if (/* GC_REF_CHECK_COLOR(Z_COUNTED_P(key), GC_BLACK) && */ !GC_FROM_WEAKMAP_KEY(entry)) {
							/* Both the entry weakmap and key are BLACK, so we
							 * can mark the entry BLACK as well.
							 * !GC_FROM_WEAKMAP_KEY(entry) means that the key
							 * was already scanned black (or will not be
							 * scanned), so it's our responsibility to mark the
							 * entry */
							ZEND_ASSERT(GC_REF_CHECK_COLOR(Z_COUNTED_P(key), GC_BLACK));
							ref = Z_COUNTED_P(entry);
							GC_ADDREF(ref);
							if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
								GC_REF_SET_BLACK(ref);
								GC_STACK_PUSH(ref);
							}
						}
					}
					zv += 2;
				}
				goto next;
			}

			ht = obj->handlers->get_gc(obj, &table, &len);
			n = len;
			zv = table;
			if (UNEXPECTED(ht)) {
				GC_ADDREF(ht);
				if (!GC_REF_CHECK_COLOR(ht, GC_BLACK)) {
					GC_REF_SET_BLACK(ht);
					for (; n != 0; n--) {
						if (Z_REFCOUNTED_P(zv)) {
							ref = Z_COUNTED_P(zv);
							GC_ADDREF(ref);
							if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
								GC_REF_SET_BLACK(ref);
								GC_STACK_PUSH(ref);
							}
						}
						zv++;
					}
					goto handle_ht;
				}
			}

handle_zvals:
			for (; n != 0; n--) {
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					GC_ADDREF(ref);
					if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
						GC_REF_SET_BLACK(ref);
						zv++;
						while (--n) {
							if (Z_REFCOUNTED_P(zv)) {
								zend_refcounted *ref = Z_COUNTED_P(zv);
								GC_ADDREF(ref);
								if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
									GC_REF_SET_BLACK(ref);
									GC_STACK_PUSH(ref);
								}
							}
							zv++;
						}
						goto tail_call;
					}
				}
				zv++;
			}
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		ZEND_ASSERT((zend_array*)ref != &EG(symbol_table));
		ht = (zend_array*)ref;
handle_ht:
		n = ht->nNumUsed;
		zv = ht->arPacked;
		if (HT_IS_PACKED(ht)) {
			goto handle_zvals;
		}

		p = (Bucket*)zv;
		for (; n != 0; n--) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				GC_ADDREF(ref);
				if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
					GC_REF_SET_BLACK(ref);
					p++;
					while (--n) {
						zv = &p->val;
						if (Z_TYPE_P(zv) == IS_INDIRECT) {
							zv = Z_INDIRECT_P(zv);
						}
						if (Z_REFCOUNTED_P(zv)) {
							zend_refcounted *ref = Z_COUNTED_P(zv);
							GC_ADDREF(ref);
							if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
								GC_REF_SET_BLACK(ref);
								GC_STACK_PUSH(ref);
							}
						}
						p++;
					}
					goto tail_call;
				}
			}
			p++;
		}
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			GC_ADDREF(ref);
			if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
				GC_REF_SET_BLACK(ref);
				goto tail_call;
			}
		}
	}

next:
	ref = GC_STACK_POP();
	if (ref) {
		goto tail_call;
	}
}

static void gc_mark_grey(zend_refcounted *ref, gc_stack *stack)
{
	HashTable *ht;
	Bucket *p;
	zval *zv;
	uint32_t n;
	GC_STACK_DCL(stack);

tail_call:
	GC_BENCH_INC(zval_marked_grey);

	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED))) {
			zval *table;
			int len;

			if (UNEXPECTED(GC_FLAGS(obj) & IS_OBJ_WEAKLY_REFERENCED)) {
				zend_weakmap_get_object_key_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n-=2) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					zval *weakmap = zv+1;
					ZEND_ASSERT(Z_REFCOUNTED_P(weakmap));
					if (Z_REFCOUNTED_P(entry)) {
						GC_SET_FROM_WEAKMAP_KEY(entry);
						ref = Z_COUNTED_P(entry);
						/* Only DELREF if the contribution from the weakmap has
						 * not been cancelled yet */
						if (!GC_FROM_WEAKMAP(entry)) {
							GC_DELREF(ref);
						}
						if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
							GC_REF_SET_COLOR(ref, GC_GREY);
							GC_STACK_PUSH(ref);
						}
					}
					zv+=2;
				}
			}

			if (UNEXPECTED(obj->handlers->get_gc == zend_weakmap_get_gc)) {
				zend_weakmap_get_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n--) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					if (Z_REFCOUNTED_P(entry)) {
						GC_SET_FROM_WEAKMAP(entry);
						ref = Z_COUNTED_P(entry);
						/* Only DELREF if the contribution from the weakmap key
						 * has not been cancelled yet */
						if (!GC_FROM_WEAKMAP_KEY(entry)) {
							GC_DELREF(ref);
						}
						if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
							GC_REF_SET_COLOR(ref, GC_GREY);
							GC_STACK_PUSH(ref);
						}
					}
					zv++;
				}
				goto next;
			}

			ht = obj->handlers->get_gc(obj, &table, &len);
			n = len;
			zv = table;
			if (UNEXPECTED(ht)) {
				GC_DELREF(ht);
				if (!GC_REF_CHECK_COLOR(ht, GC_GREY)) {
					GC_REF_SET_COLOR(ht, GC_GREY);
					for (; n != 0; n--) {
						if (Z_REFCOUNTED_P(zv)) {
							ref = Z_COUNTED_P(zv);
							GC_DELREF(ref);
							if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
								GC_REF_SET_COLOR(ref, GC_GREY);
								GC_STACK_PUSH(ref);
							}
						}
						zv++;
					}
					goto handle_ht;
				}
			}
handle_zvals:
			for (; n != 0; n--) {
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					GC_DELREF(ref);
					if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
						GC_REF_SET_COLOR(ref, GC_GREY);
						zv++;
						while (--n) {
							if (Z_REFCOUNTED_P(zv)) {
								zend_refcounted *ref = Z_COUNTED_P(zv);
								GC_DELREF(ref);
								if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
									GC_REF_SET_COLOR(ref, GC_GREY);
									GC_STACK_PUSH(ref);
								}
							}
							zv++;
						}
						goto tail_call;
					}
				}
				zv++;
			}
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		ZEND_ASSERT(((zend_array*)ref) != &EG(symbol_table));
		ht = (zend_array*)ref;
handle_ht:
		n = ht->nNumUsed;
		if (HT_IS_PACKED(ht)) {
            zv = ht->arPacked;
            goto handle_zvals;
		}

		p = ht->arData;
		for (; n != 0; n--) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				GC_DELREF(ref);
				if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
					GC_REF_SET_COLOR(ref, GC_GREY);
					p++;
					while (--n) {
						zv = &p->val;
						if (Z_TYPE_P(zv) == IS_INDIRECT) {
							zv = Z_INDIRECT_P(zv);
						}
						if (Z_REFCOUNTED_P(zv)) {
							zend_refcounted *ref = Z_COUNTED_P(zv);
							GC_DELREF(ref);
							if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
								GC_REF_SET_COLOR(ref, GC_GREY);
								GC_STACK_PUSH(ref);
							}
						}
						p++;
					}
					goto tail_call;
				}
			}
			p++;
		}
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			GC_DELREF(ref);
			if (!GC_REF_CHECK_COLOR(ref, GC_GREY)) {
				GC_REF_SET_COLOR(ref, GC_GREY);
				goto tail_call;
			}
		}
	}

next:
	ref = GC_STACK_POP();
	if (ref) {
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

static void gc_mark_roots(gc_stack *stack)
{
	gc_root_buffer *current, *last;

	gc_compact();

	current = GC_IDX2PTR(GC_FIRST_ROOT);
	last = GC_IDX2PTR(GC_G(first_unused));
	while (current != last) {
		if (GC_IS_ROOT(current->ref)) {
			if (GC_REF_CHECK_COLOR(current->ref, GC_PURPLE)) {
				GC_REF_SET_COLOR(current->ref, GC_GREY);
				gc_mark_grey(current->ref, stack);
			}
		}
		current++;
	}
}

static void gc_scan(zend_refcounted *ref, gc_stack *stack)
{
	HashTable *ht;
	Bucket *p;
	zval *zv;
	uint32_t n;
	GC_STACK_DCL(stack);

tail_call:
	if (!GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
		goto next;
	}

	if (GC_REFCOUNT(ref) > 0) {
		if (!GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
			GC_REF_SET_BLACK(ref);
			if (UNEXPECTED(!_stack->next)) {
				gc_stack_next(_stack);
			}
			/* Split stack and reuse the tail */
			_stack->next->prev = NULL;
			gc_scan_black(ref, _stack->next);
			_stack->next->prev = _stack;
		}
		goto next;
	}

	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object *obj = (zend_object*)ref;
		if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED))) {
			zval *table;
			int len;

			if (UNEXPECTED(GC_FLAGS(obj) & IS_OBJ_WEAKLY_REFERENCED)) {
				zend_weakmap_get_object_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n--) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					if (Z_OPT_REFCOUNTED_P(entry)) {
						ref = Z_COUNTED_P(entry);
						if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
							GC_REF_SET_COLOR(ref, GC_WHITE);
							GC_STACK_PUSH(ref);
						}
					}
					zv++;
				}
			}

			ht = obj->handlers->get_gc(obj, &table, &len);
			n = len;
			zv = table;
			if (UNEXPECTED(ht)) {
				if (GC_REF_CHECK_COLOR(ht, GC_GREY)) {
					GC_REF_SET_COLOR(ht, GC_WHITE);
					GC_STACK_PUSH((zend_refcounted *) ht);
					for (; n != 0; n--) {
						if (Z_REFCOUNTED_P(zv)) {
							ref = Z_COUNTED_P(zv);
							if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
								GC_REF_SET_COLOR(ref, GC_WHITE);
								GC_STACK_PUSH(ref);
							}
						}
						zv++;
					}
					goto handle_ht;
				}
			}

handle_zvals:
			for (; n != 0; n--) {
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
						GC_REF_SET_COLOR(ref, GC_WHITE);
						zv++;
						while (--n) {
							if (Z_REFCOUNTED_P(zv)) {
								zend_refcounted *ref = Z_COUNTED_P(zv);
								if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
									GC_REF_SET_COLOR(ref, GC_WHITE);
									GC_STACK_PUSH(ref);
								}
							}
							zv++;
						}
						goto tail_call;
					}
				}
				zv++;
			}
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		ht = (HashTable *)ref;
		ZEND_ASSERT(ht != &EG(symbol_table));

handle_ht:
		n = ht->nNumUsed;
		if (HT_IS_PACKED(ht)) {
            zv = ht->arPacked;
            goto handle_zvals;
		}

		p = ht->arData;
		for (; n != 0; n--) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
					GC_REF_SET_COLOR(ref, GC_WHITE);
					p++;
					while (--n) {
						zv = &p->val;
						if (Z_TYPE_P(zv) == IS_INDIRECT) {
							zv = Z_INDIRECT_P(zv);
						}
						if (Z_REFCOUNTED_P(zv)) {
							zend_refcounted *ref = Z_COUNTED_P(zv);
							if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
								GC_REF_SET_COLOR(ref, GC_WHITE);
								GC_STACK_PUSH(ref);
							}
						}
						p++;
					}
					goto tail_call;
				}
			}
			p++;
		}
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			if (GC_REF_CHECK_COLOR(ref, GC_GREY)) {
				GC_REF_SET_COLOR(ref, GC_WHITE);
				goto tail_call;
			}
		}
	}

next:
	ref = GC_STACK_POP();
	if (ref) {
		goto tail_call;
	}
}

static void gc_scan_roots(gc_stack *stack)
{
	uint32_t idx, end;
	gc_root_buffer *current;

	/* Root buffer might be reallocated during gc_scan,
	 * make sure to reload pointers. */
	idx = GC_FIRST_ROOT;
	end = GC_G(first_unused);
	while (idx != end) {
		current = GC_IDX2PTR(idx);
		if (GC_IS_ROOT(current->ref)) {
			if (GC_REF_CHECK_COLOR(current->ref, GC_GREY)) {
				GC_REF_SET_COLOR(current->ref, GC_WHITE);
				gc_scan(current->ref, stack);
			}
		}
		idx++;
	}

	/* Scan extra roots added during gc_scan */
	while (idx != GC_G(first_unused)) {
		current = GC_IDX2PTR(idx);
		if (GC_IS_ROOT(current->ref)) {
			if (GC_REF_CHECK_COLOR(current->ref, GC_GREY)) {
				GC_REF_SET_COLOR(current->ref, GC_WHITE);
				gc_scan(current->ref, stack);
			}
		}
		idx++;
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

static int gc_collect_white(zend_refcounted *ref, uint32_t *flags, gc_stack *stack)
{
	int count = 0;
	HashTable *ht;
	Bucket *p;
	zval *zv;
	uint32_t n;
	GC_STACK_DCL(stack);

tail_call:
	/* don't count references for compatibility ??? */
	if (GC_TYPE(ref) != IS_REFERENCE) {
		count++;
	}

	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED))) {
			int len;
			zval *table;

			/* optimization: color is GC_BLACK (0) */
			if (!GC_INFO(ref)) {
				gc_add_garbage(ref);
			}
			if (!(OBJ_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)
			 && (obj->handlers->dtor_obj != zend_objects_destroy_object
			  || obj->ce->destructor != NULL)) {
				*flags |= GC_HAS_DESTRUCTORS;
			}

			if (UNEXPECTED(GC_FLAGS(obj) & IS_OBJ_WEAKLY_REFERENCED)) {
				zend_weakmap_get_object_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n--) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					if (Z_REFCOUNTED_P(entry) && GC_FROM_WEAKMAP_KEY(entry)) {
						GC_UNSET_FROM_WEAKMAP_KEY(entry);
						GC_UNSET_FROM_WEAKMAP(entry);
						ref = Z_COUNTED_P(entry);
						GC_ADDREF(ref);
						if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
							GC_REF_SET_BLACK(ref);
							GC_STACK_PUSH(ref);
						}
					}
					zv++;
				}
			}

			if (UNEXPECTED(obj->handlers->get_gc == zend_weakmap_get_gc)) {
				zend_weakmap_get_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n--) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					if (Z_REFCOUNTED_P(entry) && GC_FROM_WEAKMAP(entry)) {
						GC_UNSET_FROM_WEAKMAP_KEY(entry);
						GC_UNSET_FROM_WEAKMAP(entry);
						ref = Z_COUNTED_P(entry);
						GC_ADDREF(ref);
						if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
							GC_REF_SET_BLACK(ref);
							GC_STACK_PUSH(ref);
						}
					}
					zv++;
				}
				goto next;
			}

			ht = obj->handlers->get_gc(obj, &table, &len);
			n = len;
			zv = table;
			if (UNEXPECTED(ht)) {
				GC_ADDREF(ht);
				if (GC_REF_CHECK_COLOR(ht, GC_WHITE)) {
					GC_REF_SET_BLACK(ht);
					for (; n != 0; n--) {
						if (Z_REFCOUNTED_P(zv)) {
							ref = Z_COUNTED_P(zv);
							GC_ADDREF(ref);
							if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
								GC_REF_SET_BLACK(ref);
								GC_STACK_PUSH(ref);
							}
						}
						zv++;
					}
					goto handle_ht;
				}
			}

handle_zvals:
			for (; n != 0; n--) {
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					GC_ADDREF(ref);
					if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
						GC_REF_SET_BLACK(ref);
						zv++;
						while (--n) {
							if (Z_REFCOUNTED_P(zv)) {
								zend_refcounted *ref = Z_COUNTED_P(zv);
								GC_ADDREF(ref);
								if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
									GC_REF_SET_BLACK(ref);
									GC_STACK_PUSH(ref);
								}
							}
							zv++;
						}
						goto tail_call;
					}
				}
				zv++;
			}
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		/* optimization: color is GC_BLACK (0) */
		if (!GC_INFO(ref)) {
			gc_add_garbage(ref);
		}
		ht = (zend_array*)ref;

handle_ht:
		n = ht->nNumUsed;
		if (HT_IS_PACKED(ht)) {
			zv = ht->arPacked;
			goto handle_zvals;
		}

		p = ht->arData;
		for (; n != 0; n--) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				GC_ADDREF(ref);
				if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
					GC_REF_SET_BLACK(ref);
					p++;
					while (--n) {
						zv = &p->val;
						if (Z_TYPE_P(zv) == IS_INDIRECT) {
							zv = Z_INDIRECT_P(zv);
						}
						if (Z_REFCOUNTED_P(zv)) {
							zend_refcounted *ref = Z_COUNTED_P(zv);
							GC_ADDREF(ref);
							if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
								GC_REF_SET_BLACK(ref);
								GC_STACK_PUSH(ref);
							}
						}
						p++;
					}
					goto tail_call;
				}
			}
			p++;
		}
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			GC_ADDREF(ref);
			if (GC_REF_CHECK_COLOR(ref, GC_WHITE)) {
				GC_REF_SET_BLACK(ref);
				goto tail_call;
			}
		}
	}

next:
	ref = GC_STACK_POP();
	if (ref) {
		goto tail_call;
	}

	return count;
}

static int gc_collect_roots(uint32_t *flags, gc_stack *stack)
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
			GC_REF_SET_BLACK(ref);
			count += gc_collect_white(ref, flags, stack);
		}
		idx++;
	}

	return count;
}

static int gc_remove_nested_data_from_buffer(zend_refcounted *ref, gc_root_buffer *root, gc_stack *stack)
{
	HashTable *ht;
	Bucket *p;
	zval *zv;
	uint32_t n;
	int count = 0;
	GC_STACK_DCL(stack);

tail_call:
	if (root) {
		root = NULL;
		count++;
	} else if (GC_REF_ADDRESS(ref) != 0
	 && GC_REF_CHECK_COLOR(ref, GC_BLACK)) {
		GC_TRACE_REF(ref, "removing from buffer");
		GC_REMOVE_FROM_BUFFER(ref);
		count++;
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			goto tail_call;
		}
		goto next;
	} else {
		goto next;
	}

	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(!(OBJ_FLAGS(ref) & IS_OBJ_FREE_CALLED))) {
			int len;
			zval *table;

			if (UNEXPECTED(GC_FLAGS(obj) & IS_OBJ_WEAKLY_REFERENCED)) {
				zend_weakmap_get_object_entry_gc(obj, &table, &len);
				n = len;
				zv = table;
				for (; n != 0; n--) {
					ZEND_ASSERT(Z_TYPE_P(zv) == IS_PTR);
					zval *entry = (zval*) Z_PTR_P(zv);
					if (Z_OPT_REFCOUNTED_P(entry)) {
						ref = Z_COUNTED_P(entry);
						GC_STACK_PUSH(ref);
					}
					zv++;
				}
			}

			ht = obj->handlers->get_gc(obj, &table, &len);
			n = len;
			zv = table;
			if (UNEXPECTED(ht)) {
				for (; n != 0; n--) {
					if (Z_REFCOUNTED_P(zv)) {
						ref = Z_COUNTED_P(zv);
						GC_STACK_PUSH(ref);
					}
					zv++;
				}
				if (GC_REF_ADDRESS(ht) != 0 && GC_REF_CHECK_COLOR(ht, GC_BLACK)) {
					GC_TRACE_REF(ht, "removing from buffer");
					GC_REMOVE_FROM_BUFFER(ht);
				}
				goto handle_ht;
			}

handle_zvals:
			for (; n != 0; n--) {
				if (Z_REFCOUNTED_P(zv)) {
					ref = Z_COUNTED_P(zv);
					zv++;
					while (--n) {
						if (Z_REFCOUNTED_P(zv)) {
							zend_refcounted *ref = Z_COUNTED_P(zv);
							GC_STACK_PUSH(ref);
						}
						zv++;
					}
					goto tail_call;
				}
				zv++;
			}
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		ht = (zend_array*)ref;

handle_ht:
		n = ht->nNumUsed;
		if (HT_IS_PACKED(ht)) {
			zv = ht->arPacked;
			goto handle_zvals;
		}

		p = ht->arData;
		for (; n != 0; n--) {
			zv = &p->val;
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			if (Z_REFCOUNTED_P(zv)) {
				ref = Z_COUNTED_P(zv);
				p++;
				while (--n) {
					zv = &p->val;
					if (Z_TYPE_P(zv) == IS_INDIRECT) {
						zv = Z_INDIRECT_P(zv);
					}
					if (Z_REFCOUNTED_P(zv)) {
						zend_refcounted *ref = Z_COUNTED_P(zv);
						GC_STACK_PUSH(ref);
					}
					p++;
				}
				goto tail_call;
			}
			p++;
		}
	}

next:
	ref = GC_STACK_POP();
	if (ref) {
		goto tail_call;
	}

	return count;
}

static void zend_get_gc_buffer_release(void);
static void zend_gc_root_tmpvars(void);

ZEND_API int zend_gc_collect_cycles(void)
{
	int total_count = 0;
	bool should_rerun_gc = 0;
	bool did_rerun_gc = 0;

	zend_hrtime_t start_time = zend_hrtime();

rerun_gc:
	if (GC_G(num_roots)) {
		int count;
		gc_root_buffer *current, *last;
		zend_refcounted *p;
		uint32_t gc_flags = 0;
		uint32_t idx, end;
		gc_stack stack;

		stack.prev = NULL;
		stack.next = NULL;

		if (GC_G(gc_active)) {
			GC_G(collector_time) += zend_hrtime() - start_time;
			return 0;
		}

		GC_TRACE("Collecting cycles");
		GC_G(gc_runs)++;
		GC_G(gc_active) = 1;

		GC_TRACE("Marking roots");
		gc_mark_roots(&stack);
		GC_TRACE("Scanning roots");
		gc_scan_roots(&stack);

		GC_TRACE("Collecting roots");
		count = gc_collect_roots(&gc_flags, &stack);

		if (!GC_G(num_roots)) {
			/* nothing to free */
			GC_TRACE("Nothing to free");
			gc_stack_free(&stack);
			GC_G(gc_active) = 0;
			goto finish;
		}

		zend_fiber_switch_block();

		end = GC_G(first_unused);

		if (gc_flags & GC_HAS_DESTRUCTORS) {
			GC_TRACE("Calling destructors");

			/* During a destructor call, new externally visible references to nested data may
			 * be introduced. These references can be introduced in a way that does not
			 * modify any refcounts, so we have no real way to detect this situation
			 * short of rerunning full GC tracing. What we do instead is to only run
			 * destructors at this point and automatically re-run GC afterwards. */
			should_rerun_gc = 1;

			/* Mark all roots for which a dtor will be invoked as DTOR_GARBAGE. Additionally
			 * color them purple. This serves a double purpose: First, they should be
			 * considered new potential roots for the next GC run. Second, it will prevent
			 * their removal from the root buffer by nested data removal. */
			idx = GC_FIRST_ROOT;
			current = GC_IDX2PTR(GC_FIRST_ROOT);
			while (idx != end) {
				if (GC_IS_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					if (GC_TYPE(p) == IS_OBJECT && !(OBJ_FLAGS(p) & IS_OBJ_DESTRUCTOR_CALLED)) {
						zend_object *obj = (zend_object *) p;
						if (obj->handlers->dtor_obj != zend_objects_destroy_object
							|| obj->ce->destructor) {
							current->ref = GC_MAKE_DTOR_GARBAGE(obj);
							GC_REF_SET_COLOR(obj, GC_PURPLE);
						} else {
							GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
						}
					}
				}
				current++;
				idx++;
			}

			/* Remove nested data for objects on which a destructor will be called.
			 * This will not remove the objects themselves, as they have been colored
			 * purple. */
			idx = GC_FIRST_ROOT;
			current = GC_IDX2PTR(GC_FIRST_ROOT);
			while (idx != end) {
				if (GC_IS_DTOR_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					count -= gc_remove_nested_data_from_buffer(p, current, &stack);
				}
				current++;
				idx++;
			}

			/* Actually call destructors.
			 *
			 * The root buffer might be reallocated during destructors calls,
			 * make sure to reload pointers as necessary. */
			zend_hrtime_t dtor_start_time = zend_hrtime();
			idx = GC_FIRST_ROOT;
			while (idx != end) {
				current = GC_IDX2PTR(idx);
				if (GC_IS_DTOR_GARBAGE(current->ref)) {
					p = GC_GET_PTR(current->ref);
					/* Mark this is as a normal root for the next GC run,
					 * it's no longer garbage for this run. */
					current->ref = p;
					/* Double check that the destructor hasn't been called yet. It could have
					 * already been invoked indirectly by some other destructor. */
					if (!(OBJ_FLAGS(p) & IS_OBJ_DESTRUCTOR_CALLED)) {
						zend_object *obj = (zend_object*)p;
						GC_TRACE_REF(obj, "calling destructor");
						GC_ADD_FLAGS(obj, IS_OBJ_DESTRUCTOR_CALLED);
						GC_ADDREF(obj);
						obj->handlers->dtor_obj(obj);
						GC_DELREF(obj);
					}
				}
				idx++;
			}
			GC_G(dtor_time) += zend_hrtime() - dtor_start_time;

			if (GC_G(gc_protected)) {
				/* something went wrong */
				zend_get_gc_buffer_release();
				zend_fiber_switch_unblock();
				GC_G(collector_time) += zend_hrtime() - start_time;
				return 0;
			}
		}

		gc_stack_free(&stack);

		/* Destroy zvals. The root buffer may be reallocated. */
		GC_TRACE("Destroying zvals");
		zend_hrtime_t free_start_time = zend_hrtime();
		idx = GC_FIRST_ROOT;
		while (idx != end) {
			current = GC_IDX2PTR(idx);
			if (GC_IS_GARBAGE(current->ref)) {
				p = GC_GET_PTR(current->ref);
				GC_TRACE_REF(p, "destroying");
				if (GC_TYPE(p) == IS_OBJECT) {
					zend_object *obj = (zend_object*)p;

					EG(objects_store).object_buckets[obj->handle] = SET_OBJ_INVALID(obj);
					GC_TYPE_INFO(obj) = GC_NULL |
						(GC_TYPE_INFO(obj) & ~GC_TYPE_MASK);
					/* Modify current before calling free_obj (bug #78811: free_obj() can cause the root buffer (with current) to be reallocated.) */
					current->ref = GC_MAKE_GARBAGE(((char*)obj) - obj->handlers->offset);
					if (!(OBJ_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {
						GC_ADD_FLAGS(obj, IS_OBJ_FREE_CALLED);
						GC_ADDREF(obj);
						obj->handlers->free_obj(obj);
						GC_DELREF(obj);
					}

					ZEND_OBJECTS_STORE_ADD_TO_FREE_LIST(obj->handle);
				} else if (GC_TYPE(p) == IS_ARRAY) {
					zend_array *arr = (zend_array*)p;

					GC_TYPE_INFO(arr) = GC_NULL |
						(GC_TYPE_INFO(arr) & ~GC_TYPE_MASK);

					/* GC may destroy arrays with rc>1. This is valid and safe. */
					HT_ALLOW_COW_VIOLATION(arr);

					zend_hash_destroy(arr);
				}
			}
			idx++;
		}

		/* Free objects */
		current = GC_IDX2PTR(GC_FIRST_ROOT);
		last = GC_IDX2PTR(end);
		while (current != last) {
			if (GC_IS_GARBAGE(current->ref)) {
				p = GC_GET_PTR(current->ref);
				GC_LINK_UNUSED(current);
				GC_G(num_roots)--;
				efree(p);
			}
			current++;
		}

		GC_G(free_time) += zend_hrtime() - free_start_time;

		zend_fiber_switch_unblock();

		GC_TRACE("Collection finished");
		GC_G(collected) += count;
		total_count += count;
		GC_G(gc_active) = 0;
	}

	gc_compact();

	/* Objects with destructors were removed from this GC run. Rerun GC right away to clean them
	 * up. We do this only once: If we encounter more destructors on the second run, we'll not
	 * run GC another time. */
	if (should_rerun_gc && !did_rerun_gc) {
		did_rerun_gc = 1;
		goto rerun_gc;
	}

finish:
	zend_get_gc_buffer_release();
	zend_gc_root_tmpvars();
	GC_G(collector_time) += zend_hrtime() - start_time;
	return total_count;
}

ZEND_API void zend_gc_get_status(zend_gc_status *status)
{
	status->active = GC_G(gc_active);
	status->gc_protected = GC_G(gc_protected);
	status->full = GC_G(gc_full);
	status->runs = GC_G(gc_runs);
	status->collected = GC_G(collected);
	status->threshold = GC_G(gc_threshold);
	status->buf_size = GC_G(buf_size);
	status->num_roots = GC_G(num_roots);
	status->application_time = zend_hrtime() - GC_G(activated_at);
	status->collector_time = GC_G(collector_time);
	status->dtor_time = GC_G(dtor_time);
	status->free_time = GC_G(free_time);
}

ZEND_API zend_get_gc_buffer *zend_get_gc_buffer_create(void) {
	/* There can only be one get_gc() call active at a time,
	 * so there only needs to be one buffer. */
	zend_get_gc_buffer *gc_buffer = &EG(get_gc_buffer);
	gc_buffer->cur = gc_buffer->start;
	return gc_buffer;
}

ZEND_API void zend_get_gc_buffer_grow(zend_get_gc_buffer *gc_buffer) {
	size_t old_capacity = gc_buffer->end - gc_buffer->start;
	size_t new_capacity = old_capacity == 0 ? 64 : old_capacity * 2;
	gc_buffer->start = erealloc(gc_buffer->start, new_capacity * sizeof(zval));
	gc_buffer->end = gc_buffer->start + new_capacity;
	gc_buffer->cur = gc_buffer->start + old_capacity;
}

static void zend_get_gc_buffer_release(void) {
	zend_get_gc_buffer *gc_buffer = &EG(get_gc_buffer);
	efree(gc_buffer->start);
	gc_buffer->start = gc_buffer->end = gc_buffer->cur = NULL;
}

/* TMPVAR operands are destroyed using zval_ptr_dtor_nogc(), because they usually cannot contain
 * cycles. However, there are some rare exceptions where this is possible, in which case we rely
 * on the producing code to root the value. If a GC run occurs between the rooting and consumption
 * of the value, we would end up leaking it. To avoid this, root all live TMPVAR values here. */
static void zend_gc_root_tmpvars(void) {
	zend_execute_data *ex = EG(current_execute_data);
	for (; ex; ex = ex->prev_execute_data) {
		zend_function *func = ex->func;
		if (!func || !ZEND_USER_CODE(func->type)) {
			continue;
		}

		uint32_t op_num = ex->opline - ex->func->op_array.opcodes;
		for (uint32_t i = 0; i < func->op_array.last_live_range; i++) {
			const zend_live_range *range = &func->op_array.live_range[i];
			if (range->start > op_num) {
				break;
			}
			if (range->end <= op_num) {
				continue;
			}

			uint32_t kind = range->var & ZEND_LIVE_MASK;
			if (kind == ZEND_LIVE_TMPVAR || kind == ZEND_LIVE_LOOP) {
				uint32_t var_num = range->var & ~ZEND_LIVE_MASK;
				zval *var = ZEND_CALL_VAR(ex, var_num);
				if (Z_REFCOUNTED_P(var)) {
					gc_check_possible_root(Z_COUNTED_P(var));
				}
			}
		}
	}
}

#if GC_BENCH
void gc_bench_print(void)
{
	fprintf(stderr, "GC Statistics\n");
	fprintf(stderr, "-------------\n");
	fprintf(stderr, "Runs:               %d\n", GC_G(gc_runs));
	fprintf(stderr, "Collected:          %d\n", GC_G(collected));
	fprintf(stderr, "Root buffer length: %d\n", GC_G(root_buf_length));
	fprintf(stderr, "Root buffer peak:   %d\n\n", GC_G(root_buf_peak));
	fprintf(stderr, "      Possible            Remove from  Marked\n");
	fprintf(stderr, "        Root    Buffered     buffer     grey\n");
	fprintf(stderr, "      --------  --------  -----------  ------\n");
	fprintf(stderr, "ZVAL  %8d  %8d  %9d  %8d\n", GC_G(zval_possible_root), GC_G(zval_buffered), GC_G(zval_remove_from_buffer), GC_G(zval_marked_grey));
}
#endif

#ifdef ZTS
size_t zend_gc_globals_size(void)
{
	return sizeof(zend_gc_globals);
}
#endif
