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

#include "zend.h"
#include "zend_API.h"

#define GC_ROOT_BUFFER_MAX_ENTRIES 10000

#ifdef ZTS
ZEND_API int gc_globals_id;
#else
ZEND_API zend_gc_globals gc_globals;
#endif

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
	gc_globals->zval_to_free = NULL;
	gc_globals->free_list = NULL;
	gc_globals->next_to_free = NULL;

	gc_globals->gc_runs = 0;
	gc_globals->collected = 0;

#if GC_BENCH
	gc_globals->root_buf_length = 0;
	gc_globals->root_buf_peak = 0;
	gc_globals->zval_possible_root = 0;
	gc_globals->zobj_possible_root = 0;
	gc_globals->zval_buffered = 0;
	gc_globals->zobj_buffered = 0;
	gc_globals->zval_remove_from_buffer = 0;
	gc_globals->zobj_remove_from_buffer = 0;
	gc_globals->zval_marked_grey = 0;
	gc_globals->zobj_marked_grey = 0;
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
	GC_G(zobj_possible_root) = 0;
	GC_G(zval_buffered) = 0;
	GC_G(zobj_buffered) = 0;
	GC_G(zval_remove_from_buffer) = 0;
	GC_G(zobj_remove_from_buffer) = 0;
	GC_G(zval_marked_grey) = 0;
	GC_G(zobj_marked_grey) = 0;
#endif

	GC_G(roots).next = &GC_G(roots);
	GC_G(roots).prev = &GC_G(roots);

	if (GC_G(buf)) {
		GC_G(unused) = NULL;
		GC_G(first_unused) = GC_G(buf);

		GC_G(zval_to_free) = NULL;
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

ZEND_API void gc_zval_possible_root(zval *zv TSRMLS_DC)
{
	if (UNEXPECTED(GC_G(free_list) != NULL &&
	               GC_ZVAL_ADDRESS(zv) != NULL &&
		           GC_ZVAL_GET_COLOR(zv) == GC_BLACK) &&
		           (GC_ZVAL_ADDRESS(zv) < GC_G(buf) ||
		            GC_ZVAL_ADDRESS(zv) >= GC_G(last_unused))) {
		/* The given zval is a garbage that is going to be deleted by
		 * currently running GC */
		return;
	}

	if (zv->type == IS_OBJECT) {
		GC_ZOBJ_CHECK_POSSIBLE_ROOT(zv);
		return;
	}

	GC_BENCH_INC(zval_possible_root);

	if (GC_ZVAL_GET_COLOR(zv) != GC_PURPLE) {
		if (!GC_ZVAL_ADDRESS(zv)) {
			gc_root_buffer *newRoot = GC_G(unused);

			if (newRoot) {
				GC_G(unused) = newRoot->prev;
			} else if (GC_G(first_unused) != GC_G(last_unused)) {
				newRoot = GC_G(first_unused);
				GC_G(first_unused)++;
			} else {
				if (!GC_G(gc_enabled)) {
					return;
				}
				zv->refcount__gc++;
				gc_collect_cycles(TSRMLS_C);
				zv->refcount__gc--;
				newRoot = GC_G(unused);
				if (!newRoot) {
					return;
				}
				GC_G(unused) = newRoot->prev;
			}

			GC_ZVAL_SET_PURPLE(zv);
			newRoot->next = GC_G(roots).next;
			newRoot->prev = &GC_G(roots);
			GC_G(roots).next->prev = newRoot;
			GC_G(roots).next = newRoot;

			GC_ZVAL_SET_ADDRESS(zv, newRoot);

			newRoot->handle = 0;
			newRoot->u.pz = zv;

			GC_BENCH_INC(zval_buffered);
			GC_BENCH_INC(root_buf_length);
			GC_BENCH_PEAK(root_buf_peak, root_buf_length);
		}
	}
}

ZEND_API void gc_zobj_possible_root(zval *zv TSRMLS_DC)
{
	struct _store_object *obj;

	if (UNEXPECTED(Z_OBJ_HT_P(zv)->get_gc == NULL ||
	    EG(objects_store).object_buckets == NULL)) {
		return;
	}

	GC_BENCH_INC(zobj_possible_root);

	obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(zv)].bucket.obj;
	if (GC_GET_COLOR(obj->buffered) != GC_PURPLE) {
		if (!GC_ADDRESS(obj->buffered)) {
			gc_root_buffer *newRoot = GC_G(unused);

			if (newRoot) {
				GC_G(unused) = newRoot->prev;
			} else if (GC_G(first_unused) != GC_G(last_unused)) {
				newRoot = GC_G(first_unused);
				GC_G(first_unused)++;
			} else {
				if (!GC_G(gc_enabled)) {
					return;
				}
				zv->refcount__gc++;
				gc_collect_cycles(TSRMLS_C);
				zv->refcount__gc--;
				newRoot = GC_G(unused);
				if (!newRoot) {
					return;
				}
				obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(zv)].bucket.obj;
				GC_G(unused) = newRoot->prev;
			}

			GC_SET_PURPLE(obj->buffered);
			newRoot->next = GC_G(roots).next;
			newRoot->prev = &GC_G(roots);
			GC_G(roots).next->prev = newRoot;
			GC_G(roots).next = newRoot;

			GC_SET_ADDRESS(obj->buffered, newRoot);

			newRoot->handle = Z_OBJ_HANDLE_P(zv);
			newRoot->u.handlers = Z_OBJ_HT_P(zv);

			GC_BENCH_INC(zobj_buffered);
			GC_BENCH_INC(root_buf_length);
			GC_BENCH_PEAK(root_buf_peak, root_buf_length);
		}
	}
}

ZEND_API void gc_remove_zval_from_buffer(zval *zv TSRMLS_DC)
{
	gc_root_buffer* root_buffer = GC_ADDRESS(((zval_gc_info*)zv)->u.buffered);

	if (UNEXPECTED(GC_G(free_list) != NULL &&
		           GC_ZVAL_GET_COLOR(zv) == GC_BLACK) &&
		           (GC_ZVAL_ADDRESS(zv) < GC_G(buf) ||
		            GC_ZVAL_ADDRESS(zv) >= GC_G(last_unused))) {
		/* The given zval is a garbage that is going to be deleted by
		 * currently running GC */
		if (GC_G(next_to_free) == (zval_gc_info*)zv) {
			GC_G(next_to_free) = ((zval_gc_info*)zv)->u.next;
		}
		return;
	}
	GC_BENCH_INC(zval_remove_from_buffer);
	GC_REMOVE_FROM_BUFFER(root_buffer);
	((zval_gc_info*)zv)->u.buffered = NULL;
}

static void zval_scan_black(zval *pz TSRMLS_DC)
{
	Bucket *p;

tail_call:
	p = NULL;
	GC_ZVAL_SET_BLACK(pz);

	if (Z_TYPE_P(pz) == IS_OBJECT && EG(objects_store).object_buckets) {
		zend_object_get_gc_t get_gc;
		struct _store_object *obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].bucket.obj;

		obj->refcount++;
		if (GC_GET_COLOR(obj->buffered) != GC_BLACK) {
			GC_SET_BLACK(obj->buffered);
			if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
			             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
				int i, n;
				zval **table;
				HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

				while (n > 0 && !table[n-1]) n--;
				for (i = 0; i < n; i++) {
					if (table[i]) {
						pz = table[i];
						if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
							pz->refcount__gc++;
						}
						if (GC_ZVAL_GET_COLOR(pz) != GC_BLACK) {
							if (!props && i == n - 1) {
								goto tail_call;
							} else {
								zval_scan_black(pz TSRMLS_CC);
							}
						}
					}
				}
				if (!props) {
					return;
				}
				p = props->pListHead;
			}
		}
	} else if (Z_TYPE_P(pz) == IS_ARRAY) {
		if (Z_ARRVAL_P(pz) != &EG(symbol_table)) {
			p = Z_ARRVAL_P(pz)->pListHead;
		}
	}
	while (p != NULL) {
		pz = *(zval**)p->pData;
		if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
			pz->refcount__gc++;
		}
		if (GC_ZVAL_GET_COLOR(pz) != GC_BLACK) {
			if (p->pListNext == NULL) {
				goto tail_call;
			} else {
				zval_scan_black(pz TSRMLS_CC);
			}
		}
		p = p->pListNext;
	}
}

static void zobj_scan_black(struct _store_object *obj, zval *pz TSRMLS_DC)
{
	Bucket *p;
	zend_object_get_gc_t get_gc;

	GC_SET_BLACK(obj->buffered);
	if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
	             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
		int i, n;
		zval **table;
		HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

		for (i = 0; i < n; i++) {
			if (table[i]) {
				pz = table[i];
				if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
					pz->refcount__gc++;
				}
				if (GC_ZVAL_GET_COLOR(pz) != GC_BLACK) {
					zval_scan_black(pz TSRMLS_CC);
				}
			}
		}
		if (!props) {
			return;
		}
		p = props->pListHead;
		while (p != NULL) {
			pz = *(zval**)p->pData;
			if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
				pz->refcount__gc++;
			}
			if (GC_ZVAL_GET_COLOR(pz) != GC_BLACK) {
				zval_scan_black(pz TSRMLS_CC);
			}
			p = p->pListNext;
		}
	}
}

static void zval_mark_grey(zval *pz TSRMLS_DC)
{
	Bucket *p;

tail_call:
	if (GC_ZVAL_GET_COLOR(pz) != GC_GREY) {
		p = NULL;
		GC_BENCH_INC(zval_marked_grey);
		GC_ZVAL_SET_COLOR(pz, GC_GREY);

		if (Z_TYPE_P(pz) == IS_OBJECT && EG(objects_store).object_buckets) {
			zend_object_get_gc_t get_gc;
			struct _store_object *obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].bucket.obj;

			obj->refcount--;
			if (GC_GET_COLOR(obj->buffered) != GC_GREY) {
				GC_BENCH_INC(zobj_marked_grey);
				GC_SET_COLOR(obj->buffered, GC_GREY);
				if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
				             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
					int i, n;
					zval **table;
					HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

					while (n > 0 && !table[n-1]) n--;
					for (i = 0; i < n; i++) {
						if (table[i]) {
							pz = table[i];
							if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
								pz->refcount__gc--;
							}
							if (!props && i == n - 1) {
								goto tail_call;
							} else {
								zval_mark_grey(pz TSRMLS_CC);
							}
						}
					}
					if (!props) {
						return;
					}
					p = props->pListHead;
				}
			}
		} else if (Z_TYPE_P(pz) == IS_ARRAY) {
			if (Z_ARRVAL_P(pz) == &EG(symbol_table)) {
				GC_ZVAL_SET_BLACK(pz);
			} else {
				p = Z_ARRVAL_P(pz)->pListHead;
			}
		}
		while (p != NULL) {
			pz = *(zval**)p->pData;
			if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
				pz->refcount__gc--;
			}
			if (p->pListNext == NULL) {
				goto tail_call;
			} else {
				zval_mark_grey(pz TSRMLS_CC);
			}
			p = p->pListNext;
		}
	}
}

static void zobj_mark_grey(struct _store_object *obj, zval *pz TSRMLS_DC)
{
	Bucket *p;
	zend_object_get_gc_t get_gc;

	if (GC_GET_COLOR(obj->buffered) != GC_GREY) {
		GC_BENCH_INC(zobj_marked_grey);
		GC_SET_COLOR(obj->buffered, GC_GREY);
		if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
		             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
			int i, n;
			zval **table;
			HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

			for (i = 0; i < n; i++) {
				if (table[i]) {
					pz = table[i];
					if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
						pz->refcount__gc--;
					}
					zval_mark_grey(pz TSRMLS_CC);
				}
			}
			if (!props) {
				return;
			}
			p = props->pListHead;
			while (p != NULL) {
				pz = *(zval**)p->pData;
				if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
					pz->refcount__gc--;
				}
				zval_mark_grey(pz TSRMLS_CC);
				p = p->pListNext;
			}
		}
	}
}

static void gc_mark_roots(TSRMLS_D)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		if (current->handle) {
			if (EG(objects_store).object_buckets) {
				struct _store_object *obj = &EG(objects_store).object_buckets[current->handle].bucket.obj;

				if (GC_GET_COLOR(obj->buffered) == GC_PURPLE) {
					zval z;

					INIT_PZVAL(&z);
					Z_OBJ_HANDLE(z) = current->handle;
					Z_OBJ_HT(z) = current->u.handlers;
					zobj_mark_grey(obj, &z TSRMLS_CC);
				} else {
					GC_SET_ADDRESS(obj->buffered, NULL);
					GC_REMOVE_FROM_BUFFER(current);
				}
			}
		} else {
			if (GC_ZVAL_GET_COLOR(current->u.pz) == GC_PURPLE) {
				zval_mark_grey(current->u.pz TSRMLS_CC);
			} else {
				GC_ZVAL_SET_ADDRESS(current->u.pz, NULL);
				GC_REMOVE_FROM_BUFFER(current);
			}
		}
		current = current->next;
	}
}

static void zval_scan(zval *pz TSRMLS_DC)
{
	Bucket *p;

tail_call:	
	if (GC_ZVAL_GET_COLOR(pz) == GC_GREY) {
		p = NULL;
		if (pz->refcount__gc > 0) {
			zval_scan_black(pz TSRMLS_CC);
		} else {
			GC_ZVAL_SET_COLOR(pz, GC_WHITE);
			if (Z_TYPE_P(pz) == IS_OBJECT && EG(objects_store).object_buckets) {
				zend_object_get_gc_t get_gc;
				struct _store_object *obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].bucket.obj;

				if (GC_GET_COLOR(obj->buffered) == GC_GREY) {
					if (obj->refcount > 0) {
						zobj_scan_black(obj, pz TSRMLS_CC);
					} else {
						GC_SET_COLOR(obj->buffered, GC_WHITE);
						if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
						             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
							int i, n;
							zval **table;
							HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

							while (n > 0 && !table[n-1]) n--;
							for (i = 0; i < n; i++) {
								if (table[i]) {
									pz = table[i];
									if (!props && i == n - 1) {
										goto tail_call;
									} else {
										zval_scan(pz TSRMLS_CC);
									}
								}
							}
							if (!props) {
								return;
							}
							p = props->pListHead;
						}
					}
				}
			} else if (Z_TYPE_P(pz) == IS_ARRAY) {
				if (Z_ARRVAL_P(pz) == &EG(symbol_table)) {
					GC_ZVAL_SET_BLACK(pz);
				} else {
					p = Z_ARRVAL_P(pz)->pListHead;
				}
			}
		}
		while (p != NULL) {
			if (p->pListNext == NULL) {
				pz = *(zval**)p->pData;
				goto tail_call;
			} else {
				zval_scan(*(zval**)p->pData TSRMLS_CC);
			}
			p = p->pListNext;
		}
	}
}

static void zobj_scan(zval *pz TSRMLS_DC)
{
	Bucket *p;
	zend_object_get_gc_t get_gc;

	if (EG(objects_store).object_buckets) {
		struct _store_object *obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].bucket.obj;

		if (GC_GET_COLOR(obj->buffered) == GC_GREY) {
			if (obj->refcount > 0) {
				zobj_scan_black(obj, pz TSRMLS_CC);
			} else {
				GC_SET_COLOR(obj->buffered, GC_WHITE);
				if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
				             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
					int i, n;
					zval **table;
					HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

					for (i = 0; i < n; i++) {
						if (table[i]) {
							pz = table[i];
							zval_scan(pz TSRMLS_CC);
                    	}
					}
					if (!props) {
						return;
					}
					p = props->pListHead;
					while (p != NULL) {
						zval_scan(*(zval**)p->pData TSRMLS_CC);
						p = p->pListNext;
					}
				}
			}
		}
	}
}

static void gc_scan_roots(TSRMLS_D)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		if (current->handle) {
			zval z;

			INIT_PZVAL(&z);
			Z_OBJ_HANDLE(z) = current->handle;
			Z_OBJ_HT(z) = current->u.handlers;
			zobj_scan(&z TSRMLS_CC);
		} else {
			zval_scan(current->u.pz TSRMLS_CC);
		}
		current = current->next;
	}
}

static void zval_collect_white(zval *pz TSRMLS_DC)
{
	Bucket *p;

tail_call:
	if (((zval_gc_info*)(pz))->u.buffered == (gc_root_buffer*)GC_WHITE) {
		p = NULL;
		GC_ZVAL_SET_BLACK(pz);

		if (Z_TYPE_P(pz) == IS_OBJECT && EG(objects_store).object_buckets) {
			zend_object_get_gc_t get_gc;
			struct _store_object *obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].bucket.obj;

			if (obj->buffered == (gc_root_buffer*)GC_WHITE) {
				/* PURPLE instead of BLACK to prevent buffering in nested gc calls */
				GC_SET_PURPLE(obj->buffered);

				if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
				             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
					int i, n;
					zval **table, *zv;
					HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

					if (!props) {
						/* restore refcount and put into list to free */
						pz->refcount__gc++;
						((zval_gc_info*)pz)->u.next = GC_G(zval_to_free);
						GC_G(zval_to_free) = (zval_gc_info*)pz;
					}

					while (n > 0 && !table[n-1]) n--;
					for (i = 0; i < n; i++) {
						if (table[i]) {
							zv = table[i];
							if (Z_TYPE_P(zv) != IS_ARRAY || Z_ARRVAL_P(zv) != &EG(symbol_table)) {
								zv->refcount__gc++;
							}
							if (!props && i == n - 1) {
								pz = zv;
								goto tail_call;
							} else {
								zval_collect_white(zv TSRMLS_CC);
							}
						}
					}
					if (!props) {
						return;
					}
					p = props->pListHead;
				}
			}
		} else {
			if (Z_TYPE_P(pz) == IS_ARRAY) {
				p = Z_ARRVAL_P(pz)->pListHead;
			}
		}

		/* restore refcount and put into list to free */
		pz->refcount__gc++;
		((zval_gc_info*)pz)->u.next = GC_G(zval_to_free);
		GC_G(zval_to_free) = (zval_gc_info*)pz;

		while (p != NULL) {
			pz = *(zval**)p->pData;
			if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
				pz->refcount__gc++;
			}
			if (p->pListNext == NULL) {
				goto tail_call;
			} else {
				zval_collect_white(pz TSRMLS_CC);
			}
			p = p->pListNext;
		}
	}
}

static void zobj_collect_white(zval *pz TSRMLS_DC)
{
	Bucket *p;

	if (EG(objects_store).object_buckets) {
		zend_object_get_gc_t get_gc;
		struct _store_object *obj = &EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].bucket.obj;

		if (obj->buffered == (gc_root_buffer*)GC_WHITE) {
			/* PURPLE instead of BLACK to prevent buffering in nested gc calls */
			GC_SET_PURPLE(obj->buffered);

			if (EXPECTED(EG(objects_store).object_buckets[Z_OBJ_HANDLE_P(pz)].valid &&
			             (get_gc = Z_OBJ_HANDLER_P(pz, get_gc)) != NULL)) {
				int i, n;
				zval **table;
				HashTable *props = get_gc(pz, &table, &n TSRMLS_CC);

				for (i = 0; i < n; i++) {
					if (table[i]) {
						pz = table[i];
						if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
							pz->refcount__gc++;
						}
						zval_collect_white(pz TSRMLS_CC);
					}
				}
				if (!props) {
					return;
				}
				p = props->pListHead;
				while (p != NULL) {
					pz = *(zval**)p->pData;
					if (Z_TYPE_P(pz) != IS_ARRAY || Z_ARRVAL_P(pz) != &EG(symbol_table)) {
						pz->refcount__gc++;
					}
					zval_collect_white(pz TSRMLS_CC);
					p = p->pListNext;
				}
			}
		}
	}
}

static void gc_collect_roots(TSRMLS_D)
{
	gc_root_buffer *current = GC_G(roots).next;

	while (current != &GC_G(roots)) {
		if (current->handle) {
			if (EG(objects_store).object_buckets) {
				struct _store_object *obj = &EG(objects_store).object_buckets[current->handle].bucket.obj;
				zval z;

				GC_SET_ADDRESS(obj->buffered, NULL);
				INIT_PZVAL(&z);
				Z_OBJ_HANDLE(z) = current->handle;
				Z_OBJ_HT(z) = current->u.handlers;
				zobj_collect_white(&z TSRMLS_CC);
			}
		} else {
			GC_ZVAL_SET_ADDRESS(current->u.pz, NULL);
			zval_collect_white(current->u.pz TSRMLS_CC);
		}

		GC_REMOVE_FROM_BUFFER(current);
		current = current->next;
	}
}

#define FREE_LIST_END ((zval_gc_info*)(~(zend_uintptr_t)GC_COLOR))

ZEND_API int gc_collect_cycles(TSRMLS_D)
{
	int count = 0;

	if (GC_G(roots).next != &GC_G(roots)) {
		zval_gc_info *p, *q, *orig_free_list, *orig_next_to_free;

		if (GC_G(gc_active)) {
			return 0;
		}
		GC_G(gc_runs)++;
		GC_G(zval_to_free) = FREE_LIST_END;
		GC_G(gc_active) = 1;
		gc_mark_roots(TSRMLS_C);
		gc_scan_roots(TSRMLS_C);
		gc_collect_roots(TSRMLS_C);

		orig_free_list = GC_G(free_list);
		orig_next_to_free = GC_G(next_to_free);
		p = GC_G(free_list) = GC_G(zval_to_free);
		GC_G(zval_to_free) = NULL;
		GC_G(gc_active) = 0;

		/* First call destructors */
		while (p != FREE_LIST_END) {
			if (Z_TYPE(p->z) == IS_OBJECT) {
				if (EG(objects_store).object_buckets &&
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].valid &&
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.refcount <= 0 &&
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.dtor &&
					!EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].destructor_called) {

					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].destructor_called = 1;
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.refcount++;
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.dtor(EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.object, Z_OBJ_HANDLE(p->z) TSRMLS_CC);
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.refcount--;
				}
			}
			count++;
			p = p->u.next;
		}

		/* Destroy zvals */
		p = GC_G(free_list);
		while (p != FREE_LIST_END) {
			GC_G(next_to_free) = p->u.next;
			if (Z_TYPE(p->z) == IS_OBJECT) {
				if (EG(objects_store).object_buckets &&
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].valid &&
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.refcount <= 0) {
					EG(objects_store).object_buckets[Z_OBJ_HANDLE(p->z)].bucket.obj.refcount = 1;
					Z_TYPE(p->z) = IS_NULL;
					zend_objects_store_del_ref_by_handle_ex(Z_OBJ_HANDLE(p->z), Z_OBJ_HT(p->z) TSRMLS_CC);
				}
			} else if (Z_TYPE(p->z) == IS_ARRAY) {
				Z_TYPE(p->z) = IS_NULL;
				zend_hash_destroy(Z_ARRVAL(p->z));
				FREE_HASHTABLE(Z_ARRVAL(p->z));
			} else {
				zval_dtor(&p->z);
				Z_TYPE(p->z) = IS_NULL;
			}
			p = GC_G(next_to_free);
		}

		/* Free zvals */
		p = GC_G(free_list);
		while (p != FREE_LIST_END) {
			q = p->u.next;
			FREE_ZVAL_EX(&p->z);
			p = q;
		}
		GC_G(collected) += count;
		GC_G(free_list) = orig_free_list;
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
