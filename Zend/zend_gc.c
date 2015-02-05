/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend.h"
#include "zend_API.h"

/* one (0) is reserved */
#define GC_ROOT_BUFFER_MAX_ENTRIES 10001

#ifdef ZTS
ZEND_API int gc_globals_id;
#else
ZEND_API zend_gc_globals gc_globals;
#endif

ZEND_API int (*gc_collect_cycles)(void);

#define GC_REMOVE_FROM_ROOTS(current) \
	gc_remove_from_roots((current))

static zend_always_inline void gc_remove_from_roots(gc_root_buffer *root)
{
	root->next->prev = root->prev;
	root->prev->next = root->next;
	root->prev = GC_G(unused);
	GC_G(unused) = root;
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

	gc_globals->buf = NULL;

	gc_globals->roots.next = &gc_globals->roots;
	gc_globals->roots.prev = &gc_globals->roots;
	gc_globals->unused = NULL;
	gc_globals->next_to_free = NULL;

	gc_globals->to_free.next = &gc_globals->to_free;
	gc_globals->to_free.prev = &gc_globals->to_free;

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
	GC_G(gc_full) = 0;

#if GC_BENCH
	GC_G(root_buf_length) = 0;
	GC_G(root_buf_peak) = 0;
	GC_G(zval_possible_root) = 0;
	GC_G(zval_buffered) = 0;
	GC_G(zval_remove_from_buffer) = 0;
	GC_G(zval_marked_grey) = 0;
#endif

	GC_G(roots).next = &GC_G(roots);
	GC_G(roots).prev = &GC_G(roots);

	GC_G(to_free).next = &GC_G(to_free);
	GC_G(to_free).prev = &GC_G(to_free);

	if (GC_G(buf)) {
		GC_G(unused) = NULL;
		GC_G(first_unused) = GC_G(buf) + 1;
	} else {
		GC_G(unused) = NULL;
		GC_G(first_unused) = NULL;
		GC_G(last_unused) = NULL;
	}
}

ZEND_API void gc_init(void)
{
	if (GC_G(buf) == NULL && GC_G(gc_enabled)) {
		GC_G(buf) = (gc_root_buffer*) malloc(sizeof(gc_root_buffer) * GC_ROOT_BUFFER_MAX_ENTRIES);
		GC_G(last_unused) = &GC_G(buf)[GC_ROOT_BUFFER_MAX_ENTRIES];
		gc_reset();
	}
}

ZEND_API void gc_possible_root(zend_refcounted *ref)
{
	if (UNEXPECTED(GC_TYPE(ref) == IS_NULL) || UNEXPECTED(CG(unclean_shutdown))) {
		return;
	}

	ZEND_ASSERT(GC_TYPE(ref) == IS_ARRAY || GC_TYPE(ref) == IS_OBJECT);
	GC_BENCH_INC(zval_possible_root);

	if (EXPECTED(GC_GET_COLOR(GC_INFO(ref)) == GC_BLACK)) {
		GC_SET_PURPLE(GC_INFO(ref));

		if (!GC_ADDRESS(GC_INFO(ref))) {
			gc_root_buffer *newRoot = GC_G(unused);

			if (newRoot) {
				GC_G(unused) = newRoot->prev;
			} else if (GC_G(first_unused) != GC_G(last_unused)) {
				newRoot = GC_G(first_unused);
				GC_G(first_unused)++;
			} else {
				if (!GC_G(gc_enabled)) {
					GC_SET_BLACK(GC_INFO(ref));
					return;
				}
				GC_REFCOUNT(ref)++;
				gc_collect_cycles();
				GC_REFCOUNT(ref)--;
				newRoot = GC_G(unused);
				if (!newRoot) {
					return;
				}
				GC_SET_PURPLE(GC_INFO(ref));
				GC_G(unused) = newRoot->prev;
			}

			newRoot->next = GC_G(roots).next;
			newRoot->prev = &GC_G(roots);
			GC_G(roots).next->prev = newRoot;
			GC_G(roots).next = newRoot;

			GC_SET_ADDRESS(GC_INFO(ref), newRoot - GC_G(buf));

			newRoot->ref = ref;

			GC_BENCH_INC(zval_buffered);
			GC_BENCH_INC(root_buf_length);
			GC_BENCH_PEAK(root_buf_peak, root_buf_length);
		}
	}
}

ZEND_API void gc_remove_from_buffer(zend_refcounted *ref)
{
	gc_root_buffer *root;

	root = GC_G(buf) + GC_ADDRESS(GC_INFO(ref));
	GC_BENCH_INC(zval_remove_from_buffer);
	GC_REMOVE_FROM_ROOTS(root);
	GC_INFO(ref) = 0;

	/* updete next root that is going to be freed */
	if (GC_G(next_to_free) == root) {
		GC_G(next_to_free) = root->next;
	}
}

static void gc_scan_black(zend_refcounted *ref)
{
	HashTable *ht;
	uint idx;
	Bucket *p;

tail_call:
	ht = NULL;
	GC_SET_BLACK(GC_INFO(ref));

	if (GC_TYPE(ref) == IS_OBJECT) {
		zend_object_get_gc_t get_gc;
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
		             (get_gc = obj->handlers->get_gc) != NULL)) {
			int i, n;
			zval *table;
			zval tmp;
			HashTable *props;

			ZVAL_OBJ(&tmp, obj);
			props = get_gc(&tmp, &table, &n);
			while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
			for (i = 0; i < n; i++) {
				if (Z_REFCOUNTED(table[i])) {
					ref = Z_COUNTED(table[i]);
					if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
						GC_REFCOUNT(ref)++;
					}
					if (GC_GET_COLOR(GC_INFO(ref)) != GC_BLACK) {
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							gc_scan_black(ref);
						}
					}
				}
			}
			if (!props) {
				return;
			}
			ht = props;
		}
	} else if (GC_TYPE(ref) == IS_ARRAY) {
		if ((zend_array*)ref != &EG(symbol_table)) {
			ht = &((zend_array*)ref)->ht;
		}
	} else if (GC_TYPE(ref) == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
				GC_REFCOUNT(ref)++;
			}
			if (GC_GET_COLOR(GC_INFO(ref)) != GC_BLACK) {
				goto tail_call;
			}
		}
		return;
	}
	if (!ht) return;
	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (!Z_REFCOUNTED(p->val)) continue;
		ref = Z_COUNTED(p->val);
		if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
			GC_REFCOUNT(ref)++;
		}
		if (GC_GET_COLOR(GC_INFO(ref)) != GC_BLACK) {
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				gc_scan_black(ref);
			}
		}
	}
}

static void gc_mark_grey(zend_refcounted *ref)
{
    HashTable *ht;
    uint idx;
	Bucket *p;

tail_call:
	if (GC_GET_COLOR(GC_INFO(ref)) != GC_GREY) {
		ht = NULL;
		GC_BENCH_INC(zval_marked_grey);
		GC_SET_COLOR(GC_INFO(ref), GC_GREY);

		if (GC_TYPE(ref) == IS_OBJECT) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
		             (get_gc = obj->handlers->get_gc) != NULL)) {
				int i, n;
				zval *table;
				zval tmp;
				HashTable *props;

				ZVAL_OBJ(&tmp, obj);
				props = get_gc(&tmp, &table, &n);

				while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
				for (i = 0; i < n; i++) {
					if (Z_REFCOUNTED(table[i])) {
						ref = Z_COUNTED(table[i]);
						if (GC_TYPE(ref) != IS_ARRAY || ((zend_array*)ref) != &EG(symbol_table)) {
							GC_REFCOUNT(ref)--;
						}
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							gc_mark_grey(ref);
						}
					}
				}
				if (!props) {
					return;
				}
				ht = props;
			}
		} else if (GC_TYPE(ref) == IS_ARRAY) {
			if (((zend_array*)ref) == &EG(symbol_table)) {
				GC_SET_BLACK(GC_INFO(ref));
			} else {
				ht = &((zend_array*)ref)->ht;
			}
		} else if (GC_TYPE(ref) == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				if (UNEXPECTED(!EG(objects_store).object_buckets) &&
					Z_TYPE(((zend_reference*)ref)->val) == IS_OBJECT) {
					Z_TYPE_INFO(((zend_reference*)ref)->val) = IS_NULL;
					return;
				}
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
					GC_REFCOUNT(ref)--;
				}
				goto tail_call;
			}
			return;
		}
		if (!ht) return;
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (!Z_REFCOUNTED(p->val)) continue;
			if (UNEXPECTED(!EG(objects_store).object_buckets) &&
				Z_TYPE(p->val) == IS_OBJECT) {
				Z_TYPE_INFO(p->val) = IS_NULL;
				continue;
			}
			ref = Z_COUNTED(p->val);
			if (GC_TYPE(ref) != IS_ARRAY || ((zend_array*)ref) != &EG(symbol_table)) {
				GC_REFCOUNT(ref)--;
			}
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				gc_mark_grey(ref);
			}
		}
	}
}

static void gc_mark_roots(void)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		if (GC_GET_COLOR(GC_INFO(current->ref)) == GC_PURPLE) {
			gc_mark_grey(current->ref);
		}
		current = current->next;
	}
}

static void gc_scan(zend_refcounted *ref)
{
    HashTable *ht;
    uint idx;
	Bucket *p;

tail_call:
	if (GC_GET_COLOR(GC_INFO(ref)) == GC_GREY) {
		ht = NULL;
		if (GC_REFCOUNT(ref) > 0) {
			gc_scan_black(ref);
		} else {
			GC_SET_COLOR(GC_INFO(ref), GC_WHITE);
			if (GC_TYPE(ref) == IS_OBJECT) {
				zend_object_get_gc_t get_gc;
				zend_object *obj = (zend_object*)ref;

				if (EXPECTED(IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
				             (get_gc = obj->handlers->get_gc) != NULL)) {
					int i, n;
					zval *table;
					zval tmp;
					HashTable *props;

					ZVAL_OBJ(&tmp, obj);
					props = get_gc(&tmp, &table, &n);
					while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
					for (i = 0; i < n; i++) {
						if (Z_REFCOUNTED(table[i])) {
							ref = Z_COUNTED(table[i]);
							if (!props && i == n - 1) {
								goto tail_call;
							} else {
								gc_scan(ref);
							}
						}
					}
					if (!props) {
						return;
					}
					ht = props;
				}
			} else if (GC_TYPE(ref) == IS_ARRAY) {
				if ((zend_array*)ref == &EG(symbol_table)) {
					GC_SET_BLACK(GC_INFO(ref));
				} else {
					ht = &((zend_array*)ref)->ht;
				}
			} else if (GC_TYPE(ref) == IS_REFERENCE) {
				if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
					ref = Z_COUNTED(((zend_reference*)ref)->val);
					goto tail_call;
				}
				return;
			}
		}
		if (!ht) return;
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (!Z_REFCOUNTED(p->val)) continue;
			ref = Z_COUNTED(p->val);
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				gc_scan(ref);
			}
		}
	}
}

static void gc_scan_roots(void)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		gc_scan(current->ref);
		current = current->next;
	}
}

static int gc_collect_white(zend_refcounted *ref)
{
	int count = 0;
	HashTable *ht;
	uint idx;
	Bucket *p;

tail_call:
	if (GC_GET_COLOR(GC_INFO(ref)) == GC_WHITE) {
		ht = NULL;
		GC_SET_BLACK(GC_INFO(ref));

		/* don't count references for compatibility ??? */
		if (GC_TYPE(ref) != IS_REFERENCE) {
			count++;
		}

#if 1
		if ((GC_TYPE(ref) == IS_OBJECT || GC_TYPE(ref) == IS_ARRAY) &&
		    !GC_ADDRESS(GC_INFO(ref))) {
			/* add garbage into list */
			gc_root_buffer *buf = GC_G(unused);

			if (buf) {
				GC_G(unused) = buf->prev;
			} else if (GC_G(first_unused) != GC_G(last_unused)) {
				buf = GC_G(first_unused);
				GC_G(first_unused)++;
			} else {
				/* TODO: find a perfect way to handle such case */
				GC_G(gc_full) = 1;
			}

			if (buf) {
				buf->ref = ref;
				buf->next = GC_G(roots).next;
				buf->prev = &GC_G(roots);
				GC_G(roots).next->prev = buf;
				GC_G(roots).next = buf;
				GC_SET_ADDRESS(GC_INFO(ref), buf - GC_G(buf));
			}
		}
#endif

		if (GC_TYPE(ref) == IS_OBJECT) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
			             (get_gc = obj->handlers->get_gc) != NULL)) {
				int i, n;
				zval *table;
				zval tmp;
				HashTable *props;

				ZVAL_OBJ(&tmp, obj);
				props = get_gc(&tmp, &table, &n);
				while (n > 0 && !Z_REFCOUNTED(table[n-1])) {
					/* count non-refcounted for compatibility ??? */
					if (Z_TYPE(table[n-1]) != IS_UNDEF) {
						count++;
					}
					n--;
				}
				for (i = 0; i < n; i++) {
					if (Z_REFCOUNTED(table[i])) {
						ref = Z_COUNTED(table[i]);
						if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
							GC_REFCOUNT(ref)++;
						}
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							count += gc_collect_white(ref);
						}
					/* count non-refcounted for compatibility ??? */
					} else if (Z_TYPE(table[i]) != IS_UNDEF) {
						count++;
					}
				}
				if (!props) {
					return count;
				}
				ht = props;
			}
		} else if (GC_TYPE(ref) == IS_ARRAY) {
			ht = &((zend_array*)ref)->ht;
		} else if (GC_TYPE(ref) == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
					GC_REFCOUNT(ref)++;
				}
				goto tail_call;
			}
			return count;
		}

		if (!ht) return count;
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (!Z_REFCOUNTED(p->val)) {
				/* count non-refcounted for compatibility ??? */
				if (Z_TYPE(p->val) != IS_UNDEF && Z_TYPE(p->val) != IS_INDIRECT) {
					count++;
				}
				continue;
			}
			ref = Z_COUNTED(p->val);
			if (GC_TYPE(ref) != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
				GC_REFCOUNT(ref)++;
			}
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				count += gc_collect_white(ref);
			}
		}
	}
	return count;
}

static int gc_collect_roots(void)
{
	int count = 0;
	gc_root_buffer *current = GC_G(roots).next;

	/* remove non-garbage from the list */
	while (current != &GC_G(roots)) {
		if (GC_GET_COLOR(GC_INFO(current->ref)) != GC_WHITE) {
			GC_SET_ADDRESS(GC_INFO(current->ref), 0);
			GC_REMOVE_FROM_ROOTS(current);
		}
		current = current->next;
	}

	current = GC_G(roots).next;
	while (current != &GC_G(roots)) {
		if (GC_GET_COLOR(GC_INFO(current->ref)) == GC_WHITE) {
			GC_REFCOUNT(current->ref)++;
			count += gc_collect_white(current->ref);
		}
		current = current->next;
	}

	if (GC_G(gc_full) == 1) {
		current = GC_G(roots).next;
		while (current != &GC_G(roots)) {
			GC_SET_ADDRESS(GC_INFO(current->ref), 0);
			GC_SET_BLACK(GC_INFO(current->ref));
			current = current->next;
		}
		gc_reset();
		return 0;
	}

	/* relink remaining roots into list to free */
	if (GC_G(roots).next != &GC_G(roots)) {
		if (GC_G(to_free).next == &GC_G(to_free)) {
			/* move roots into list to free */
			GC_G(to_free).next = GC_G(roots).next;
			GC_G(to_free).prev = GC_G(roots).prev;
			GC_G(to_free).next->prev = &GC_G(to_free);
			GC_G(to_free).prev->next = &GC_G(to_free);
		} else {
			/* add roots into list to free */
			GC_G(to_free).prev->next = GC_G(roots).next;
			GC_G(roots).next->prev = GC_G(to_free).prev;
			GC_G(roots).prev->next = &GC_G(to_free);
			GC_G(to_free).prev = GC_G(roots).prev;
		}

		GC_G(roots).next = &GC_G(roots);
		GC_G(roots).prev = &GC_G(roots);
	}
	return count;
}

static void gc_remove_nested_data_from_buffer(zend_refcounted *ref)
{
	HashTable *ht = NULL;
	uint idx;
	Bucket *p;

tail_call:
	if (GC_ADDRESS(GC_INFO(ref)) != 0) {
		GC_REMOVE_FROM_BUFFER(ref);

		if (GC_TYPE(ref) == IS_OBJECT) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
		             (get_gc = obj->handlers->get_gc) != NULL)) {
				int i, n;
				zval *table;
				zval tmp;
				HashTable *props;

				ZVAL_OBJ(&tmp, obj);
				props = get_gc(&tmp, &table, &n);

				while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
				for (i = 0; i < n; i++) {
					if (Z_REFCOUNTED(table[i])) {
						ref = Z_COUNTED(table[i]);
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							gc_remove_nested_data_from_buffer(ref);
						}
					}
				}
				if (!props) {
					return;
				}
				ht = props;
			}
		} else if (GC_TYPE(ref) == IS_ARRAY) {
			ht = &((zend_array*)ref)->ht;
		} else if (GC_TYPE(ref) == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				goto tail_call;
			}
			return;
		}
		if (!ht) return;
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (!Z_REFCOUNTED(p->val)) continue;
			ref = Z_COUNTED(p->val);
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				gc_remove_nested_data_from_buffer(ref);
			}
		}
	}
}

ZEND_API int zend_gc_collect_cycles(void)
{
	int count = 0;

	if (GC_G(roots).next != &GC_G(roots)) {
		gc_root_buffer *current, *orig_next_to_free;
		zend_refcounted *p;
		gc_root_buffer to_free;

		if (GC_G(gc_active)) {
			return 0;
		}
		GC_G(gc_runs)++;
		GC_G(gc_active) = 1;
		gc_mark_roots();
		gc_scan_roots();
		count = gc_collect_roots();
		GC_G(gc_active) = 0;

		if (GC_G(to_free).next == &GC_G(to_free)) {
			/* nothing to free */
			return 0;
		}

		/* Copy global to_free list into local list */
		to_free.next = GC_G(to_free).next;
		to_free.prev = GC_G(to_free).prev;
		to_free.next->prev = &to_free;
		to_free.prev->next = &to_free;

		/* Free global list */
		GC_G(to_free).next = &GC_G(to_free);
		GC_G(to_free).prev = &GC_G(to_free);

		orig_next_to_free = GC_G(next_to_free);

		/* Remember reference counters before calling destructors */
		current = to_free.next;
		while (current != &to_free) {
			current->refcount = GC_REFCOUNT(current->ref);
			current = current->next;
		}

		/* Call destructors */
		if (EG(objects_store).object_buckets) {
			current = to_free.next;
			while (current != &to_free) {
				p = current->ref;
				GC_G(next_to_free) = current->next;
				if (GC_TYPE(p) == IS_OBJECT) {
					zend_object *obj = (zend_object*)p;

					if (IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
						!(GC_FLAGS(obj) & IS_OBJ_DESTRUCTOR_CALLED)) {

						GC_FLAGS(obj) |= IS_OBJ_DESTRUCTOR_CALLED;
						if (obj->handlers->dtor_obj) {
							GC_REFCOUNT(obj)++;
							obj->handlers->dtor_obj(obj);
							GC_REFCOUNT(obj)--;
						}
					}
				}
				current = GC_G(next_to_free);
			}

			/* Remove values captured in destructors */
			current = to_free.next;
			while (current != &to_free) {
				GC_G(next_to_free) = current->next;
				if (GC_REFCOUNT(current->ref) > current->refcount) {
					gc_remove_nested_data_from_buffer(current->ref);
				}
				current = GC_G(next_to_free);
			}
		}

		/* Destroy zvals */
		current = to_free.next;
		while (current != &to_free) {
			p = current->ref;
			GC_G(next_to_free) = current->next;
			if (GC_TYPE(p) == IS_OBJECT) {
				zend_object *obj = (zend_object*)p;

				if (EG(objects_store).object_buckets &&
				    IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle]) &&
					!(GC_FLAGS(obj) & IS_OBJ_FREE_CALLED)) {

					GC_FLAGS(obj) |= IS_OBJ_FREE_CALLED;
					if (obj->handlers->free_obj) {
						GC_REFCOUNT(obj)++;
						obj->handlers->free_obj(obj);
						GC_REFCOUNT(obj)--;
					}
				}
			} else if (GC_TYPE(p) == IS_ARRAY) {
				zend_array *arr = (zend_array*)p;

				GC_TYPE(arr) = IS_NULL;
				zend_hash_destroy(&arr->ht);
			}
			current = GC_G(next_to_free);
		}

		/* Free objects */
		current = to_free.next;
		while (current != &to_free) {
			p = current->ref;
			GC_G(next_to_free) = current->next;
			if (GC_TYPE(p) == IS_OBJECT) {
				zend_object *obj = (zend_object*)p;

				if (EG(objects_store).object_buckets &&
				    IS_OBJ_VALID(EG(objects_store).object_buckets[obj->handle])) {

					zend_objects_store_free(obj);
				}
			} else {
				GC_REMOVE_FROM_BUFFER(p);
				efree(p);
			}
			current = GC_G(next_to_free);
		}

		GC_G(collected) += count;
		GC_G(next_to_free) = orig_next_to_free;
	}

	return count;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
