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

#define GC_HAS_DESTRUCTORS  (1<<0)

#ifndef ZEND_GC_DEBUG
# define ZEND_GC_DEBUG 0
#endif

#ifdef ZTS
ZEND_API int gc_globals_id;
#else
ZEND_API zend_gc_globals gc_globals;
#endif

ZEND_API int (*gc_collect_cycles)(void);

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

#define GC_INVALID          ((uint32_t) -1)
#define GC_ROOTS_SENTINEL   ((uint32_t) 0)
#define GC_TO_FREE_SENTINEL ((uint32_t) 1)
#define GC_FIRST_REAL_ROOT  ((uint32_t) 2)
#define GC_DUMMY_BUF_SIZE   GC_FIRST_REAL_ROOT

#define GC_DEFAULT_COLLECTION_THRESHOLD 10000
#define GC_ROOT_BUFFER_DEFAULT_SIZE \
	(GC_DEFAULT_COLLECTION_THRESHOLD + GC_FIRST_REAL_ROOT)

#define GC_TO_BUF(addr) (GC_G(buf) + (addr))
#define GC_TO_ADDR(buffer) ((buffer) - GC_G(buf))

#define GC_ROOTS() GC_TO_BUF(GC_ROOTS_SENTINEL)
#define GC_TO_FREE() GC_TO_BUF(GC_TO_FREE_SENTINEL)

#define GC_NEXT_BUF(buf) GC_TO_BUF((buf)->next)
#define GC_PREV_BUF(buf) GC_TO_BUF((buf)->prev)

#define GC_REF_GET_COLOR(ref) \
	(GC_FLAGS(ref) & GC_COLOR)
#define GC_REF_SET_COLOR_EX(ref, c) \
	do { GC_FLAGS(ref) = (GC_FLAGS(ref) & ~GC_COLOR) | (c); } while (0);
#define GC_REF_SET_BLACK_EX(ref) \
	do { GC_FLAGS(ref) = GC_FLAGS(ref) & ~GC_COLOR; } while (0);
#define GC_REF_SET_PURPLE_EX(ref) \
	do { GC_FLAGS(ref) |= GC_COLOR; } while (0);

#define GC_REF_SET_COLOR(ref, c) \
	do { GC_TRACE_SET_COLOR(ref, c); GC_REF_SET_COLOR_EX(ref, c); } while (0)
#define GC_REF_SET_BLACK(ref) \
	do { GC_TRACE_SET_COLOR(ref, GC_BLACK); GC_REF_SET_BLACK_EX(ref); } while (0)
#define GC_REF_SET_PURPLE(ref) \
	do { GC_TRACE_SET_COLOR(ref, GC_PURPLE); GC_REF_SET_PURPLE_EX(ref); } while (0)

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
			ref, GC_REFCOUNT(ref), GC_ADDRESS(ref),
			gc_color_name(GC_REF_GET_COLOR(ref)),
			obj->ce->name->val, obj->handle);
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		zend_array *arr = (zend_array *) ref;
		fprintf(stderr, "[%p] rc=%d addr=%d %s array(%d) ",
			ref, GC_REFCOUNT(ref), GC_ADDRESS(ref),
			gc_color_name(GC_REF_GET_COLOR(ref)),
			zend_hash_num_elements(arr));
	} else {
		fprintf(stderr, "[%p] rc=%d %s %s ",
			ref, GC_REFCOUNT(ref),
			gc_color_name(GC_REF_GET_COLOR(ref)),
			zend_get_type_by_const(GC_TYPE(ref)));
	}
}
#endif

static const gc_root_buffer gc_dummy_buffer[GC_DUMMY_BUF_SIZE] = {
	{ NULL, GC_ROOTS_SENTINEL, GC_ROOTS_SENTINEL, 0 },
	{ NULL, GC_TO_FREE_SENTINEL, GC_TO_FREE_SENTINEL, 0 },
};
#define GC_DUMMY_BUF() ((gc_root_buffer *) &gc_dummy_buffer)

static zend_always_inline void gc_remove_from_roots(gc_root_buffer *root)
{
	GC_NEXT_BUF(root)->prev = root->prev;
	GC_PREV_BUF(root)->next = root->next;
	root->prev = GC_G(unused);
	GC_G(unused) = GC_TO_ADDR(root);

	GC_G(num_roots)--;
	GC_BENCH_DEC(root_buf_length);
}

static void root_buffer_dtor(zend_gc_globals *gc_globals)
{
	if (gc_globals->buf != GC_DUMMY_BUF()) {
		free(gc_globals->buf);
		gc_globals->buf = GC_DUMMY_BUF();
	}
}

static void gc_globals_ctor_ex(zend_gc_globals *gc_globals)
{
	gc_globals->gc_enabled = 0;
	gc_globals->gc_active = 0;
	gc_globals->gc_protected = 0;

	gc_globals->buf = GC_DUMMY_BUF();
	gc_globals->buf_size = GC_DUMMY_BUF_SIZE;
	gc_globals->gc_threshold = GC_DEFAULT_COLLECTION_THRESHOLD;

	gc_globals->num_roots = 0;
	gc_globals->unused = GC_INVALID;
	gc_globals->first_unused = GC_FIRST_REAL_ROOT;
	gc_globals->next_to_free = GC_INVALID;

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

	if (GC_G(buf) != GC_DUMMY_BUF()) {
		GC_ROOTS()->next = GC_ROOTS_SENTINEL;
		GC_ROOTS()->prev = GC_ROOTS_SENTINEL;
		GC_TO_FREE()->next = GC_TO_FREE_SENTINEL;
		GC_TO_FREE()->prev = GC_TO_FREE_SENTINEL;
	}

	GC_G(unused) = GC_INVALID;
	GC_G(first_unused) = GC_FIRST_REAL_ROOT;
}

ZEND_API void gc_init(void)
{
	if (GC_G(buf) == GC_DUMMY_BUF() && GC_G(gc_enabled)) {
		GC_G(buf) = malloc(sizeof(gc_root_buffer) * GC_ROOT_BUFFER_DEFAULT_SIZE);
		if (GC_G(buf)) {
			GC_G(buf_size) = GC_ROOT_BUFFER_DEFAULT_SIZE;
		} else {
			GC_G(buf) = GC_DUMMY_BUF();
			GC_G(buf_size) = GC_DUMMY_BUF_SIZE;
		}
		gc_reset();
	}
}

static void gc_grow_root_buffer() {
	/* Double the buffer size, taking into account the reserved roots */
	size_t new_size = (GC_G(buf_size) - GC_FIRST_REAL_ROOT) * 2 + GC_FIRST_REAL_ROOT;
	GC_G(buf) = perealloc(GC_G(buf), sizeof(gc_root_buffer) * new_size, 1);
	GC_G(buf_size) = new_size;
}

ZEND_API void ZEND_FASTCALL gc_possible_root(zend_refcounted *ref)
{
	uint32_t newRootAddr;
	gc_root_buffer *newRoot;

	if (UNEXPECTED(CG(unclean_shutdown)) || UNEXPECTED(GC_G(gc_protected))) {
		return;
	}

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	ZEND_ASSERT(EXPECTED(GC_REF_GET_COLOR(ref) == GC_BLACK));
	ZEND_ASSERT(!GC_ADDRESS(ref));

	GC_BENCH_INC(zval_possible_root);

	/* TODO It would be more elegant to move this to the end and avoid all the
	 * special cases here. I'm keeping it for now to preserve exact collection point. */
	if (UNEXPECTED(GC_G(num_roots) >= GC_G(gc_threshold)
			&& GC_G(gc_enabled) && !GC_G(gc_active))) {
		GC_ADDREF(ref);
		gc_collect_cycles();
		GC_DELREF(ref);
		if (UNEXPECTED(GC_REFCOUNT(ref)) == 0) {
			zval_dtor_func(ref);
			return;
		}
		if (UNEXPECTED(GC_ADDRESS(ref))) {
			return;
		}
	}

	newRootAddr = GC_G(unused);
	if (newRootAddr != GC_INVALID) {
		newRoot = GC_TO_BUF(newRootAddr);
		GC_G(unused) = newRoot->prev;
	} else if (GC_G(first_unused) != GC_G(buf_size)) {
		newRootAddr = GC_G(first_unused);
		newRoot = GC_TO_BUF(newRootAddr);
		GC_G(first_unused)++;
	} else {
		if (GC_G(buf) == GC_DUMMY_BUF()) {
			/* This means that the GC is completely disabled, rather than just disabled
			 * temporarily at run-time. In this case we just let things leak. */
			return;
		}

		gc_grow_root_buffer();
		newRootAddr = GC_G(first_unused);
		newRoot = GC_TO_BUF(newRootAddr);
		GC_G(first_unused)++;
	}

	GC_TRACE_SET_COLOR(ref, GC_PURPLE);
	GC_SET_ADDRESS(ref, newRootAddr);
	GC_REF_SET_PURPLE(ref);
	newRoot->ref = ref;

	{
		gc_root_buffer *roots = GC_ROOTS();
		newRoot->next = roots->next;
		newRoot->prev = GC_ROOTS_SENTINEL;
		GC_NEXT_BUF(roots)->prev = newRootAddr;
		roots->next = newRootAddr;
	}

	GC_G(num_roots)++;
	GC_BENCH_INC(zval_buffered);
	GC_BENCH_INC(root_buf_length);
	GC_BENCH_PEAK(root_buf_peak, root_buf_length);
}

ZEND_API void ZEND_FASTCALL gc_remove_from_buffer(zend_refcounted *ref)
{
	uint32_t addr;
	gc_root_buffer *root;

	ZEND_ASSERT(GC_ADDRESS(ref));

	GC_BENCH_INC(zval_remove_from_buffer);

	addr = GC_ADDRESS(ref);
	root = GC_TO_BUF(addr);
	gc_remove_from_roots(root);

	if (GC_REF_GET_COLOR(ref) != GC_BLACK) {
		GC_TRACE_SET_COLOR(ref, GC_PURPLE);
	}
	GC_SET_ADDRESS(ref, 0);

	/* updete next root that is going to be freed */
	if (GC_G(next_to_free) == addr) {
		GC_G(next_to_free) = root->next;
	}
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
					if (GC_REF_GET_COLOR(ref) != GC_BLACK) {
						gc_scan_black(ref);
					}
				}
				zv++;
			}
			if (EXPECTED(!ht)) {
				ref = Z_COUNTED_P(zv);
				GC_ADDREF(ref);
				if (GC_REF_GET_COLOR(ref) != GC_BLACK) {
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
			if (GC_REF_GET_COLOR(ref) != GC_BLACK) {
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
			if (GC_REF_GET_COLOR(ref) != GC_BLACK) {
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
	if (GC_REF_GET_COLOR(ref) != GC_BLACK) {
		goto tail_call;
	}
}

static void gc_mark_grey(zend_refcounted *ref)
{
    HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (GC_REF_GET_COLOR(ref) != GC_GREY) {
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

static void gc_mark_roots(void)
{
	gc_root_buffer *roots = GC_ROOTS();
	gc_root_buffer *current = GC_NEXT_BUF(roots);

	while (current != roots) {
		if (GC_REF_GET_COLOR(current->ref) == GC_PURPLE) {
			gc_mark_grey(current->ref);
		}
		current = GC_NEXT_BUF(current);
	}
}

static void gc_scan(zend_refcounted *ref)
{
    HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (GC_REF_GET_COLOR(ref) == GC_GREY) {
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
	gc_root_buffer *roots = GC_ROOTS();
	gc_root_buffer *current = GC_NEXT_BUF(roots);

	while (current != roots) {
		gc_scan(current->ref);
		current = GC_NEXT_BUF(current);
	}
}

static void gc_add_garbage(zend_refcounted *ref)
{
	gc_root_buffer *roots;
	uint32_t addr;
	gc_root_buffer *buf;

	if (GC_G(unused) != GC_INVALID) {
		addr = GC_G(unused);
		buf = GC_TO_BUF(addr);
		GC_G(unused) = buf->prev;
	} else if (GC_G(first_unused) != GC_G(buf_size)) {
		addr = GC_G(first_unused);
		buf = GC_TO_BUF(addr);
		GC_G(first_unused)++;
	} else {
		gc_grow_root_buffer();
		addr = GC_G(first_unused);
		buf = GC_TO_BUF(addr);
		GC_G(first_unused)++;
	}

	/* optimization: color is already GC_BLACK (0) */
	GC_SET_ADDRESS(ref, addr);

	roots = GC_ROOTS();
	buf->ref = ref;
	buf->next = roots->next;
	buf->prev = GC_ROOTS_SENTINEL;
	GC_NEXT_BUF(roots)->prev = addr;
	roots->next = addr;

	GC_G(num_roots)++;
}

static int gc_collect_white(zend_refcounted *ref, uint32_t *flags)
{
	int count = 0;
	HashTable *ht;
	Bucket *p, *end;
	zval *zv;

tail_call:
	if (GC_REF_GET_COLOR(ref) == GC_WHITE) {
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

				if (!GC_ADDRESS(ref)) {
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
			if (!GC_ADDRESS(ref)) {
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
	int count = 0;
	gc_root_buffer *roots = GC_ROOTS();
	gc_root_buffer *current = GC_NEXT_BUF(roots);

	/* remove non-garbage from the list */
	while (current != roots) {
		gc_root_buffer *next = GC_NEXT_BUF(current);
		if (GC_REF_GET_COLOR(current->ref) == GC_BLACK) {
			gc_remove_from_roots(current);
			GC_SET_ADDRESS(current->ref, 0); /* reset GC_ADDRESS() and keep GC_BLACK */
		}
		current = next;
	}

	/* Root buffer might be reallocated during gc_collect_white,
	 * make sure to reload pointers. */
	current = GC_NEXT_BUF(roots);
	while (current != GC_ROOTS()) {
		uint32_t next = current->next;
		if (GC_REF_GET_COLOR(current->ref) == GC_WHITE) {
			count += gc_collect_white(current->ref, flags);
		}
		current = GC_TO_BUF(next);
	}

	/* relink remaining roots into list to free */
	roots = GC_ROOTS();
	if (roots->next != GC_ROOTS_SENTINEL) {
		gc_root_buffer *to_free = GC_TO_FREE();
		if (to_free->next == GC_TO_FREE_SENTINEL) {
			/* move roots into list to free */
			to_free->next = roots->next;
			to_free->prev = roots->prev;
			GC_NEXT_BUF(to_free)->prev = GC_TO_FREE_SENTINEL;
			GC_PREV_BUF(to_free)->next = GC_TO_FREE_SENTINEL;
		} else {
			/* add roots into list to free */
			GC_PREV_BUF(to_free)->next = roots->next;
			GC_NEXT_BUF(roots)->prev = to_free->prev;
			GC_PREV_BUF(roots)->next = GC_TO_FREE_SENTINEL;
			to_free->prev = roots->prev;
		}

		roots->next = GC_ROOTS_SENTINEL;
		roots->prev = GC_ROOTS_SENTINEL;
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
	    ((GC_FLAGS(ref) & GC_COLLECTABLE) &&
		 GC_ADDRESS(ref) != 0 &&
	     GC_REF_GET_COLOR(ref) == GC_BLACK)) {
		GC_TRACE_REF(ref, "removing from buffer");
		if (root) {
			gc_remove_from_roots(root);
			GC_SET_ADDRESS(ref, 0);
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

static void gc_adjust_threshold(int count) {
	/* TODO Very simple heuristic for dynamic GC buffer resizing:
	 * If there are "too few" collections, increase the collection threshold
	 * by a factor of two. */
	if (count < 100) {
		GC_G(gc_threshold) *= 2;
	} else if (GC_G(gc_threshold) > GC_DEFAULT_COLLECTION_THRESHOLD) {
		GC_G(gc_threshold) /= 2;
	}
}

ZEND_API int zend_gc_collect_cycles(void)
{
	int count = 0;
	gc_root_buffer *roots = GC_ROOTS();

	if (roots->next != GC_ROOTS_SENTINEL) {
		gc_root_buffer *current, *next;
		zend_refcounted *p;
		gc_root_buffer *to_free;
		uint32_t gc_flags = 0;

		if (GC_G(gc_active)) {
			return 0;
		}

		GC_TRACE("Collecting cycles");
		GC_G(gc_runs)++;
		GC_G(gc_active) = 1;
		GC_G(gc_protected) = 1;

		GC_TRACE("Marking roots");
		gc_mark_roots();
		GC_TRACE("Scanning roots");
		gc_scan_roots();

		GC_TRACE("Collecting roots");
		count = gc_collect_roots(&gc_flags);

		GC_G(gc_protected) = 0;

		to_free = GC_TO_FREE();
		if (to_free->next == GC_TO_FREE_SENTINEL) {
			/* nothing to free */
			GC_TRACE("Nothing to free");
			GC_G(gc_active) = 0;
			gc_adjust_threshold(count);
			return 0;
		}

		if (gc_flags & GC_HAS_DESTRUCTORS) {
			GC_TRACE("Calling destructors");

			/* The root buffer might be reallocated during destructors calls,
			 * make sure to reload pointers as necessary. */

			/* Remember reference counters before calling destructors */
			current = GC_NEXT_BUF(to_free);
			while (current != to_free) {
				current->refcount = GC_REFCOUNT(current->ref);
				current = GC_NEXT_BUF(current);
			}

			/* Call destructors */
			current = GC_NEXT_BUF(to_free);
			while (current != GC_TO_FREE()) {
				p = current->ref;
				GC_G(next_to_free) = current->next;
				if (GC_TYPE(p) == IS_OBJECT) {
					zend_object *obj = (zend_object*)p;

					if (!(OBJ_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)) {
						GC_TRACE_REF(obj, "calling destructor");
						OBJ_FLAGS(obj) |= IS_OBJ_DESTRUCTOR_CALLED;
						if (obj->handlers->dtor_obj
						 && (obj->handlers->dtor_obj != zend_objects_destroy_object
						  || obj->ce->destructor)) {
							GC_ADDREF(obj);
							obj->handlers->dtor_obj(obj);
							GC_DELREF(obj);
						}
					}
				}
				current = GC_TO_BUF(GC_G(next_to_free));
			}

			/* Remove values captured in destructors */
			current = GC_NEXT_BUF(to_free);
			while (current != GC_TO_FREE()) {
				GC_G(next_to_free) = current->next;
				if (GC_REFCOUNT(current->ref) > current->refcount) {
					gc_remove_nested_data_from_buffer(current->ref, current);
				}
				current = GC_TO_BUF(GC_G(next_to_free));
			}
		}

		/* Destroy zvals */
		GC_TRACE("Destroying zvals");
		GC_G(gc_protected) = 1;
		to_free = GC_TO_FREE();
		current = GC_NEXT_BUF(to_free);
		while (current != to_free) {
			p = current->ref;
			GC_G(next_to_free) = current->next;
			GC_TRACE_REF(p, "destroying");
			if (GC_TYPE(p) == IS_OBJECT) {
				zend_object *obj = (zend_object*)p;

				EG(objects_store).object_buckets[obj->handle] = SET_OBJ_INVALID(obj);
				GC_TYPE(obj) = IS_NULL;
				if (!(OBJ_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {
					OBJ_FLAGS(obj) |= IS_OBJ_FREE_CALLED;
					if (obj->handlers->free_obj) {
						GC_ADDREF(obj);
						obj->handlers->free_obj(obj);
						GC_DELREF(obj);
					}
				}
				SET_OBJ_BUCKET_NUMBER(EG(objects_store).object_buckets[obj->handle], EG(objects_store).free_list_head);
				EG(objects_store).free_list_head = obj->handle;
				p = current->ref = (zend_refcounted*)(((char*)obj) - obj->handlers->offset);
			} else if (GC_TYPE(p) == IS_ARRAY) {
				zend_array *arr = (zend_array*)p;

				GC_TYPE(arr) = IS_NULL;

				/* GC may destroy arrays with rc>1. This is valid and safe. */
				HT_ALLOW_COW_VIOLATION(arr);

				zend_hash_destroy(arr);
			}
			current = GC_TO_BUF(GC_G(next_to_free));
		}

		/* Free objects */
		current = GC_NEXT_BUF(to_free);
		while (current != to_free) {
			next = GC_NEXT_BUF(current);
			p = current->ref;
			current->prev = GC_G(unused);
			GC_G(unused) = GC_TO_ADDR(current);
			GC_G(num_roots)--;

			efree(p);
			current = next;
		}

		to_free->next = GC_TO_FREE_SENTINEL;
		to_free->prev = GC_TO_FREE_SENTINEL;

		GC_TRACE("Collection finished");
		GC_G(collected) += count;
		GC_G(next_to_free) = GC_INVALID;
		GC_G(gc_protected) = 0;
		GC_G(gc_active) = 0;

		gc_adjust_threshold(count);
	}

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
