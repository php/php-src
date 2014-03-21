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

#include "zend.h"
#include "zend_API.h"

/* one (0) is reserved */
#define GC_ROOT_BUFFER_MAX_ENTRIES 10001

#ifdef ZTS
ZEND_API int gc_globals_id;
#else
ZEND_API zend_gc_globals gc_globals;
#endif

#define GC_REMOVE_FROM_ROOTS(current) \
	gc_remove_from_roots((current) TSRMLS_CC)

static zend_always_inline void gc_remove_from_roots(gc_root_buffer *root TSRMLS_DC)
{
	root->next->prev = root->prev;
	root->prev->next = root->next;
	root->prev = GC_G(unused);
	GC_G(unused) = root;
	GC_BENCH_DEC(root_buf_length);
}

static void root_buffer_dtor(zend_gc_globals *gc_globals TSRMLS_DC)
{
	if (gc_globals->buf) {
		free(gc_globals->buf);
		gc_globals->buf = NULL;
	}	
}

static void gc_globals_ctor_ex(zend_gc_globals *gc_globals TSRMLS_DC)
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

ZEND_API void gc_globals_ctor(TSRMLS_D)
{
#ifdef ZTS
	ts_allocate_id(&gc_globals_id, sizeof(zend_gc_globals), (ts_allocate_ctor) gc_globals_ctor_ex, (ts_allocate_dtor) root_buffer_dtor);
#else
	gc_globals_ctor_ex(&gc_globals);
#endif
}

ZEND_API void gc_globals_dtor(TSRMLS_D)
{
#ifndef ZTS
	root_buffer_dtor(&gc_globals TSRMLS_DC);
#endif
}

ZEND_API void gc_reset(TSRMLS_D)
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

ZEND_API void gc_init(TSRMLS_D)
{
	if (GC_G(buf) == NULL && GC_G(gc_enabled)) {
		GC_G(buf) = (gc_root_buffer*) malloc(sizeof(gc_root_buffer) * GC_ROOT_BUFFER_MAX_ENTRIES);
		GC_G(last_unused) = &GC_G(buf)[GC_ROOT_BUFFER_MAX_ENTRIES];
		gc_reset(TSRMLS_C);
	}
}

ZEND_API void gc_possible_root(zend_refcounted *ref TSRMLS_DC)
{
	if (UNEXPECTED(GC_ADDRESS(ref->u.v.gc_info) &&
		           GC_GET_COLOR(ref->u.v.gc_info) == GC_BLACK &&
		           GC_ADDRESS(ref->u.v.gc_info) >= GC_G(last_unused) - GC_G(buf))) {
		/* The given zval is a garbage that is going to be deleted by
		 * currently running GC */
		return;
	}

	GC_BENCH_INC(zval_possible_root);

	if (GC_GET_COLOR(ref->u.v.gc_info) == GC_BLACK) {
		GC_SET_PURPLE(ref->u.v.gc_info);

		if (!GC_ADDRESS(ref->u.v.gc_info)) {
			gc_root_buffer *newRoot = GC_G(unused);

			if (newRoot) {
				GC_G(unused) = newRoot->prev;
			} else if (GC_G(first_unused) != GC_G(last_unused)) {
				newRoot = GC_G(first_unused);
				GC_G(first_unused)++;
			} else {
				if (!GC_G(gc_enabled)) {
					GC_SET_BLACK(ref->u.v.gc_info);
					return;
				}
				ref->refcount++;
				gc_collect_cycles(TSRMLS_C);
				ref->refcount--;
				newRoot = GC_G(unused);
				if (!newRoot) {
					return;
				}
				GC_SET_PURPLE(ref->u.v.gc_info);
				GC_G(unused) = newRoot->prev;
			}

			newRoot->next = GC_G(roots).next;
			newRoot->prev = &GC_G(roots);
			GC_G(roots).next->prev = newRoot;
			GC_G(roots).next = newRoot;

			GC_SET_ADDRESS(ref->u.v.gc_info, newRoot - GC_G(buf));

			newRoot->ref = ref;

			GC_BENCH_INC(zval_buffered);
			GC_BENCH_INC(root_buf_length);
			GC_BENCH_PEAK(root_buf_peak, root_buf_length);
		}
	}
}

ZEND_API void gc_remove_from_buffer(zend_refcounted *ref TSRMLS_DC)
{
	gc_root_buffer *root;

	if (UNEXPECTED(GC_ADDRESS(ref->u.v.gc_info) &&
	               GC_GET_COLOR(ref->u.v.gc_info) == GC_BLACK &&
		           GC_ADDRESS(ref->u.v.gc_info) >= GC_G(last_unused) - GC_G(buf))) {
		/* The given zval is a garbage that is going to be deleted by
		 * currently running GC */
		return;
	}

	root = GC_G(buf) + GC_ADDRESS(ref->u.v.gc_info);
	GC_BENCH_INC(zval_remove_from_buffer);
	GC_REMOVE_FROM_ROOTS(root);
	ref->u.v.gc_info = 0;

	/* updete next root that is going to be freed */
	if (GC_G(next_to_free) == root) {
		GC_G(next_to_free) = root->next;
	}
}

static void gc_scan_black(zend_refcounted *ref TSRMLS_DC)
{
	HashTable *ht;
	uint idx;
	Bucket *p;

tail_call:
	ht = NULL;
	GC_SET_BLACK(ref->u.v.gc_info);

	if (ref->u.v.type == IS_OBJECT && EG(objects_store).object_buckets) {
		zend_object_get_gc_t get_gc;
		zend_object *obj = (zend_object*)ref;

		if (EXPECTED(IS_VALID(EG(objects_store).object_buckets[obj->handle]) &&
		             (get_gc = obj->handlers->get_gc) != NULL)) {
			int i, n;
			zval *table;
			zval tmp;
			HashTable *props;

			ZVAL_OBJ(&tmp, obj);
			props = get_gc(&tmp, &table, &n TSRMLS_CC);
			while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
			for (i = 0; i < n; i++) {
				if (Z_REFCOUNTED(table[i])) {
					ref = Z_COUNTED(table[i]);
					if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
						ref->refcount++;
					}
					if (GC_GET_COLOR(ref->u.v.gc_info) != GC_BLACK) {
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							gc_scan_black(ref TSRMLS_CC);
						}
					}
				}
			}
			if (!props) {
				return;
			}
			ht = props;
		}
	} else if (ref->u.v.type == IS_ARRAY) {
		if ((zend_array*)ref != &EG(symbol_table)) {
			ht = &((zend_array*)ref)->ht;
		}
	} else if (ref->u.v.type == IS_REFERENCE) {
		if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
			if (UNEXPECTED(!EG(objects_store).object_buckets) &&
			    Z_TYPE(((zend_reference*)ref)->val) == IS_OBJECT) {
				return;
			}
			ref = Z_COUNTED(((zend_reference*)ref)->val);
			if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
				ref->refcount++;
			}
			if (GC_GET_COLOR(ref->u.v.gc_info) != GC_BLACK) {
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
		if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
			ref->refcount++;
		}
		if (GC_GET_COLOR(ref->u.v.gc_info) != GC_BLACK) {
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				gc_scan_black(ref TSRMLS_CC);
			}
		}
	}
}

static void gc_mark_grey(zend_refcounted *ref TSRMLS_DC)
{
    HashTable *ht;
    uint idx;
	Bucket *p;

tail_call:
	if (GC_GET_COLOR(ref->u.v.gc_info) != GC_GREY) {
		ht = NULL;
		GC_BENCH_INC(zval_marked_grey);
		GC_SET_COLOR(ref->u.v.gc_info, GC_GREY);

		if (ref->u.v.type == IS_OBJECT && EG(objects_store).object_buckets) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			if (EXPECTED(IS_VALID(EG(objects_store).object_buckets[obj->handle]) &&
		             (get_gc = obj->handlers->get_gc) != NULL)) {
				int i, n;
				zval *table;
				zval tmp;

				ZVAL_OBJ(&tmp, obj);
				HashTable *props = get_gc(&tmp, &table, &n TSRMLS_CC);

				while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
				for (i = 0; i < n; i++) {
					if (Z_REFCOUNTED(table[i])) {
						ref = Z_COUNTED(table[i]);
						if (ref->u.v.type != IS_ARRAY || ((zend_array*)ref) != &EG(symbol_table)) {
							ref->refcount--;
						}
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							gc_mark_grey(ref TSRMLS_CC);
						}
					}
				}
				if (!props) {
					return;
				}
				ht = props;
			}
		} else if (ref->u.v.type == IS_ARRAY) {
			if (((zend_array*)ref) == &EG(symbol_table)) {
				GC_SET_BLACK(ref->u.v.gc_info);
			} else {
				ht = &((zend_array*)ref)->ht;
			}
		} else if (ref->u.v.type == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				if (UNEXPECTED(!EG(objects_store).object_buckets) &&
				    Z_TYPE(((zend_reference*)ref)->val) == IS_OBJECT) {
					return;
				}
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
					ref->refcount--;
				}
				goto tail_call;
			}
			return;
		}
		if (!ht) return;
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (!Z_REFCOUNTED(p->val)) continue;
			ref = Z_COUNTED(p->val);
			if (ref->u.v.type != IS_ARRAY || ((zend_array*)ref) != &EG(symbol_table)) {
				ref->refcount--;
			}
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				gc_mark_grey(ref TSRMLS_CC);
			}
		}
	}
}

static void gc_mark_roots(TSRMLS_D)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		if (GC_GET_COLOR(current->ref->u.v.gc_info) == GC_PURPLE) {
			gc_mark_grey(current->ref TSRMLS_CC);
//???		} else {
//???			GC_SET_ADDRESS(current->ref->u.v.gc_info, 0);
//???			GC_REMOVE_FROM_ROOTS(current);
		}
		current = current->next;
	}
}

static void gc_scan(zend_refcounted *ref TSRMLS_DC)
{
    HashTable *ht;
    uint idx;
	Bucket *p;

tail_call:	
	if (GC_GET_COLOR(ref->u.v.gc_info) == GC_GREY) {
		ht = NULL;
		if (ref->refcount > 0) {
			gc_scan_black(ref TSRMLS_CC);
		} else {
			GC_SET_COLOR(ref->u.v.gc_info, GC_WHITE);
			if (ref->u.v.type == IS_OBJECT && EG(objects_store).object_buckets) {
				zend_object_get_gc_t get_gc;
				zend_object *obj = (zend_object*)ref;

				if (EXPECTED(IS_VALID(EG(objects_store).object_buckets[obj->handle]) &&
				             (get_gc = obj->handlers->get_gc) != NULL)) {
					int i, n;
					zval *table;
					zval tmp;
					HashTable *props;
					
					ZVAL_OBJ(&tmp, obj);
					props = get_gc(&tmp, &table, &n TSRMLS_CC);
					while (n > 0 && !Z_REFCOUNTED(table[n-1])) n--;
					for (i = 0; i < n; i++) {
						if (Z_REFCOUNTED(table[i])) {
							ref = Z_COUNTED(table[i]);
							if (!props && i == n - 1) {
								goto tail_call;
							} else {
								gc_scan(ref TSRMLS_CC);
							}
						}
					}
					if (!props) {
						return;
					}
					ht = props;
				}
			} else if (ref->u.v.type == IS_ARRAY) {
				if ((zend_array*)ref == &EG(symbol_table)) {
					GC_SET_BLACK(ref->u.v.gc_info);
				} else {
					ht = &((zend_array*)ref)->ht;
				}
			} else if (ref->u.v.type == IS_REFERENCE) {
				if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
					if (UNEXPECTED(!EG(objects_store).object_buckets) &&
					    Z_TYPE(((zend_reference*)ref)->val) == IS_OBJECT) {
						return;
					}
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
				gc_scan(ref TSRMLS_CC);
			}
		}
	}
}

static void gc_scan_roots(TSRMLS_D)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		gc_scan(current->ref TSRMLS_CC);
		current = current->next;
	}
}

static int gc_collect_white(zend_refcounted *ref TSRMLS_DC)
{
	int count = 0;
	HashTable *ht;
	uint idx;
	Bucket *p;

tail_call:
	if (ref->u.v.gc_info == GC_WHITE) {
		ht = NULL;
		GC_SET_BLACK(ref->u.v.gc_info);

		/* don't count references for compatibilty ??? */
		if (ref->u.v.type != IS_REFERENCE) {
			count++;
		}

		if (ref->u.v.type == IS_OBJECT && EG(objects_store).object_buckets) {
			zend_object_get_gc_t get_gc;
			zend_object *obj = (zend_object*)ref;

			/* PURPLE instead of BLACK to prevent buffering in nested gc calls */
//???			GC_SET_PURPLE(obj->gc.u.v.gc_info);

			if (EXPECTED(IS_VALID(EG(objects_store).object_buckets[obj->handle]) &&
			             (get_gc = obj->handlers->get_gc) != NULL)) {
				int i, n;
				zval *table;
				zval tmp;
				HashTable *props;
				
				ZVAL_OBJ(&tmp, obj);
				props = get_gc(&tmp, &table, &n TSRMLS_CC);
				while (n > 0 && !Z_REFCOUNTED(table[n-1])) {
					/* count non-refcounted for compatibilty ??? */
					if (Z_TYPE(table[n-1]) != IS_UNDEF) {
						count++;
					}					
					n--;
				}
				for (i = 0; i < n; i++) {
					if (Z_REFCOUNTED(table[i])) {
						ref = Z_COUNTED(table[i]);
						if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
							ref->refcount++;
						}
						if (!props && i == n - 1) {
							goto tail_call;
						} else {
							count += gc_collect_white(ref TSRMLS_CC);
						}
					/* count non-refcounted for compatibilty ??? */
					} else if (Z_TYPE(table[i]) != IS_UNDEF) {
						count++;
					}					
				}
				if (!props) {
					return count;
				}
				ht = props;
			}
		} else if (ref->u.v.type == IS_ARRAY) {
			ht = &((zend_array*)ref)->ht;
		} else if (ref->u.v.type == IS_REFERENCE) {
			if (Z_REFCOUNTED(((zend_reference*)ref)->val)) {
				if (UNEXPECTED(!EG(objects_store).object_buckets) &&
				    Z_TYPE(((zend_reference*)ref)->val) == IS_OBJECT) {
					return count;
				}
				ref = Z_COUNTED(((zend_reference*)ref)->val);
				if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
					ref->refcount++;
				}
				goto tail_call;
			}
			return count;
		}

		if (!ht) return count;
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (!Z_REFCOUNTED(p->val)) {
				/* count non-refcounted for compatibilty ??? */
				if (Z_TYPE(p->val) != IS_UNDEF) {
					count++;
				}
				continue;
			}
			ref = Z_COUNTED(p->val);
			if (ref->u.v.type != IS_ARRAY || (zend_array*)ref != &EG(symbol_table)) {
				ref->refcount++;
			}
			if (idx == ht->nNumUsed-1) {
				goto tail_call;
			} else {
				count += gc_collect_white(ref TSRMLS_CC);
			}
		}
	}
	return count;
}

static int gc_collect_roots(TSRMLS_D)
{
	int count = 0;
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		GC_SET_ADDRESS(current->ref->u.v.gc_info, 0);
		if (current->ref->u.v.gc_info == GC_WHITE) {
			count += gc_collect_white(current->ref TSRMLS_CC);
			GC_SET_ADDRESS(current->ref->u.v.gc_info, current - GC_G(buf));
		} else {
			GC_REMOVE_FROM_ROOTS(current);
		}
		current = current->next;
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

ZEND_API int gc_collect_cycles(TSRMLS_D)
{
	int count = 0;

	if (GC_G(roots).next != &GC_G(roots)) {
		gc_root_buffer *current, *orig_next_to_free;
		zend_refcounted *p;

		if (GC_G(gc_active)) {
			return 0;
		}
		GC_G(gc_runs)++;
		GC_G(gc_active) = 1;
		gc_mark_roots(TSRMLS_C);
		gc_scan_roots(TSRMLS_C);
		count = gc_collect_roots(TSRMLS_C);

		orig_next_to_free = GC_G(next_to_free);
		GC_G(gc_active) = 0;

		/* First call destructors */
		current = GC_G(to_free).next;
		while (current != &GC_G(to_free)) {
			p = current->ref;
			GC_G(next_to_free) = current->next;
			if (p->u.v.type == IS_OBJECT) {
				zend_object *obj = (zend_object*)p;

				if (EG(objects_store).object_buckets &&
				    obj->handlers->dtor_obj &&
				    IS_VALID(EG(objects_store).object_buckets[obj->handle]) &&
					!(obj->gc.u.v.flags & IS_OBJ_DESTRUCTOR_CALLED)) {

					obj->gc.u.v.flags |= IS_OBJ_DESTRUCTOR_CALLED;
					obj->gc.refcount++;
					obj->handlers->dtor_obj(obj TSRMLS_CC);
					obj->gc.refcount--;
				}
			}
			current = GC_G(next_to_free);
		}

		/* Destroy zvals */
		current = GC_G(to_free).next;
		while (current != &GC_G(to_free)) {
			p = current->ref;
			GC_G(next_to_free) = current->next;
			if (p->u.v.type == IS_OBJECT) {
				zend_object *obj = (zend_object*)p;

				if (EG(objects_store).object_buckets &&
					IS_VALID(EG(objects_store).object_buckets[obj->handle])) {
					obj->gc.u.v.type = IS_NULL;
					zend_objects_store_free(obj TSRMLS_CC);
				}
			} else if (p->u.v.type == IS_ARRAY) {
				zend_array *arr = (zend_array*)p;

				arr->gc.u.v.type = IS_NULL;
				zend_hash_destroy(&arr->ht);
				GC_REMOVE_FROM_BUFFER(arr);
				efree(arr);
			} else if (p->u.v.type == IS_REFERENCE) {
				zend_reference *ref = (zend_reference*)p;

				ref->gc.u.v.type = IS_NULL;
				if (EXPECTED(EG(objects_store).object_buckets != NULL) ||
				    Z_TYPE(ref->val) != IS_OBJECT) {
					zval_dtor(&ref->val);
				}
				GC_REMOVE_FROM_BUFFER(ref);
				efree(ref);
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
