/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* Some information for the reader...
 *
 * Watchpoints are either simple, recursive or implicit (PHPDBG_WATCH_* flags)
 * Simple means that a particular watchpoint was explicitely defined
 * Recursive watchpoints are created recursively and substitute simple watchpoints
 * Implicit watchpoints are implicitely created on all ancestors of simple or recursive watchpoints
 * Recursive and (simple or implicit) watchpoints are mutually exclusive
 *
 * PHPDBG_G(watchpoint_tree) contains all watchpoints identified by the watch target address
 * PHPDBG_G(watch_HashTables) contains the dtors of the HashTables to call in our custom dtor (we substitute the dtor of HashTables containing watched zvals by our own dtor)
 * PHPDBG_G(watchpoints) contains all watchpoints (except the ones managed by watch collision)
 * PHPDBG_G(watch_collisions) is indexed by a zend_reference * pointer. It stores information about collisions (everything which contains a zend_reference * may be referenced by multiple watches)
 *
 * Creating a watchpoint:
 * * Create watchpoints with PHPDBG_WATCH_IMPLICIT set on each zval and HashTable in hierarchy except the last zval or HashTable fetch. (if already existing PHPDBG_WATCH_IMPLICIT flag is added)
 * * Create a PHPDBG_WATCH_SIMPLE watch for simple watches or a PHPDBG_WATCH_RECURSIVE watch for recursive watches
 * * When the target zval is an IS_REFERENCE, create a watchpoint on it too
 * * Each time a watchpoints parent is a zval and it is Z_REFCOUNTED(), put a watchpoint (WATCH_ON_REFCOUNTED) on it and add a watchpoint collision
 * * When in recursive mode and encountering a not-refcounted PHPDBG_WATCH_SIMPLE, remove it and recreate it with a PHPDBG_WATCH_RECURSIVE (handled via watch collision)
 * * Make attention to not add something twice or iterate over it twice
 *
 * Deleting a watchpoint:
 * * Only allow deletion of recursive watches at their root and simple watches
 * * Go to referenced variable. And remove watch collision on *parent* (if there is a parent)
 * * If it is Z_REFCOUNTED(), remove that watch collision
 *
 * Watch collisions:
 * * hold a counter for recursive, if it is incremented from 0 to 1, create recursive watchpoint
 * * holds a HashTable for normal (not implicit) watchpoints ... it is used to get the fetch type of the HashTable (depending on whether it is empty or not)
 * * holds a HashTable for implicit watchpoints ... (some sort of a refcounter, but ensure that there are no duplicates)
 * * if normal and implicit watchpoints are empty, drop that watch collision and remove WATCH_ON_REFCOUNT alongside with watchpoint on an eventual reference
 *
 * Watching on addresses:
 * * Address and size are transformed into memory page aligned address and size
 * * mprotect() enables or disables them (depending on flags) - Windows has a transparent compatibility layer in phpdbg_win.c
 * * segfault handler dumps watched memory segment and deactivates watchpoint
 * * later watches inside these memory segments are compared against their current value and eventually reactivated (or deleted)
 *
 * A watched zval was removed:
 * * trigger a memory copy (in segv handler) and an automatic deactivation
 * * change a type flag _zval_struct.u1.v.type_flags (add PHPDBG_DESTRUCTED_ZVAL flag) in memory dump
 *
 * A watched zval was changed:
 * * check if parent container has a different reference for referenced zval - recursively update all watches and drop them if necessary
 * * if _zval_struct.u1.v.type_flags & PHPDBG_DESTRUCTED_ZVAL, add it to a list of zvals to be handled at the end (if location was not changed, remove it finally)
 * * display changes if watch->flags & PHPDBG_WATCH_NORMAL (means: not implicit)
 * * handle case where Z_RECOUNTED() or Z_PTR() changed (remove/add collison(s))
 * * if necessary ... on zvals: handle references, if recursive also objects and arrays ... on arrays: if recursive, add new elements
 * * drop destructed zval watchpoints which were not updated
 */

#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_btree.h"
#include "phpdbg_watch.h"
#include "phpdbg_utils.h"
#include "phpdbg_prompt.h"
#ifndef _WIN32
# include <unistd.h>
# include <sys/mman.h>
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

const phpdbg_command_t phpdbg_watch_commands[] = {
	PHPDBG_COMMAND_D_EX(array,      "create watchpoint on an array", 'a', watch_array,     &phpdbg_prompt_commands[24], "s", 0),
	PHPDBG_COMMAND_D_EX(delete,     "delete watchpoint",             'd', watch_delete,    &phpdbg_prompt_commands[24], "s", 0),
	PHPDBG_COMMAND_D_EX(recursive,  "create recursive watchpoints",  'r', watch_recursive, &phpdbg_prompt_commands[24], "s", 0),
	PHPDBG_END_COMMAND
};

//#define HT_FROM_WATCH(watch) (watch->type == WATCH_ON_OBJECT ? watch->addr.obj->handlers->get_properties(watch->parent_container.zv) : watch->type == WATCH_ON_ARRAY ? &watch->addr.arr->ht : NULL)
#define HT_FROM_ZVP(zvp) (Z_TYPE_P(zvp) == IS_OBJECT ? Z_OBJPROP_P(zvp) : Z_TYPE_P(zvp) == IS_ARRAY ? Z_ARRVAL_P(zvp) : NULL)

#define HT_WATCH_OFFSET (sizeof(zend_refcounted *) + sizeof(uint32_t)) /* we are not interested in gc and flags */
#define HT_PTR_HT(ptr) ((HashTable *) (((char *) (ptr)) - HT_WATCH_OFFSET))
#define HT_WATCH_HT(watch) HT_PTR_HT((watch)->addr.ptr)

typedef struct {
	void *page;
	size_t size;
	char reenable_writing;
	/* data must be last element */
	void *data;
} phpdbg_watch_memdump;

#define MEMDUMP_SIZE(size) (sizeof(phpdbg_watch_memdump) - sizeof(void *) + (size))


static phpdbg_watchpoint_t *phpdbg_check_for_watchpoint(void *addr) {
	phpdbg_watchpoint_t *watch;
	phpdbg_btree_result *result = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), (zend_ulong)phpdbg_get_page_boundary(addr) + phpdbg_pagesize - 1);

	if (result == NULL) {
		return NULL;
	}

	watch = result->ptr;

	/* check if that addr is in a mprotect()'ed memory area */
	if ((char *) phpdbg_get_page_boundary(watch->addr.ptr) > (char *) addr || (char *) phpdbg_get_page_boundary(watch->addr.ptr) + phpdbg_get_total_page_size(watch->addr.ptr, watch->size) < (char *) addr) {
		/* failure */
		return NULL;
	}

	return watch;
}

static void phpdbg_change_watchpoint_access(phpdbg_watchpoint_t *watch, int access) {
	/* pagesize is assumed to be in the range of 2^x */
	mprotect(phpdbg_get_page_boundary(watch->addr.ptr), phpdbg_get_total_page_size(watch->addr.ptr, watch->size), access);
}

static inline void phpdbg_activate_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_change_watchpoint_access(watch, PROT_READ);
}

static inline void phpdbg_deactivate_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_change_watchpoint_access(watch, PROT_READ | PROT_WRITE);
}

static inline void phpdbg_store_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_btree_insert(&PHPDBG_G(watchpoint_tree), (zend_ulong) watch->addr.ptr, watch);
}

static inline void phpdbg_remove_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_btree_delete(&PHPDBG_G(watchpoint_tree), (zend_ulong) watch->addr.ptr);
}

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch) {
	watch->addr.ptr = addr;
	watch->size = size;
}

void phpdbg_create_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch) {
	phpdbg_create_addr_watchpoint(zv, sizeof(zval), watch);
	watch->type = WATCH_ON_ZVAL;
}

void phpdbg_create_ht_watchpoint(HashTable *ht, phpdbg_watchpoint_t *watch) {
	phpdbg_create_addr_watchpoint(((char *) ht) + HT_WATCH_OFFSET, sizeof(HashTable) - HT_WATCH_OFFSET, watch);
	watch->type = WATCH_ON_HASHTABLE;
	watch->implicit_ht_count = 0;
}

static int phpdbg_create_recursive_ht_watch(phpdbg_watchpoint_t *watch);
static int phpdbg_create_recursive_zval_watch(phpdbg_watchpoint_t *watch);

void phpdbg_watch_HashTable_dtor(zval *ptr);

static void phpdbg_free_watch(phpdbg_watchpoint_t *watch) {
	zend_string_release(watch->str);
	zend_string_release(watch->name_in_parent);
}

static int phpdbg_delete_watchpoint(phpdbg_watchpoint_t *tmp_watch);
static void phpdbg_delete_ht_watchpoints_recursive(phpdbg_watchpoint_t *watch);
static void phpdbg_delete_zval_watchpoints_recursive(phpdbg_watchpoint_t *watch);
static void phpdbg_delete_watchpoints_recursive(phpdbg_watchpoint_t *watch);

/* Store all the possible watches the refcounted may refer to (for displaying & deleting by identifier) [collision] */
static phpdbg_watchpoint_t *phpdbg_create_refcounted_watchpoint(phpdbg_watchpoint_t *parent, zend_refcounted *ref) {
	phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));
	watch->flags = parent->flags;
	watch->parent = parent;
	watch->str = parent->str;
	++GC_REFCOUNT(parent->str);
	phpdbg_create_addr_watchpoint(&GC_REFCOUNT(ref), sizeof(uint32_t), watch);
	watch->type = WATCH_ON_REFCOUNTED;

	return watch;
}

/* Must prevent duplicates ... if there are duplicates, replace new by old! */
static void phpdbg_add_watch_collision(phpdbg_watchpoint_t *watch) {
	phpdbg_watch_collision *cur;

	/* Check for either recursive or (simple and/or implicit) */
	ZEND_ASSERT(((watch->flags & PHPDBG_WATCH_RECURSIVE) == 0) ^ ((watch->flags & (PHPDBG_WATCH_IMPLICIT | PHPDBG_WATCH_SIMPLE)) == 0));

	if ((cur = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->addr.ref))) {
		phpdbg_watchpoint_t *old;
		int flags = 0;
		if ((old = zend_hash_find_ptr(&cur->watches, watch->str)) || (old = zend_hash_find_ptr(&cur->implicit_watches, watch->str))) {
			if (((old->flags ^ watch->flags) & (PHPDBG_WATCH_NORMAL|PHPDBG_WATCH_IMPLICIT)) == 0) {
				return; /* there was no change ... */
			}

			flags = old->flags;

			if (flags & PHPDBG_WATCH_RECURSIVE) {
				if (!(watch->flags & PHPDBG_WATCH_RECURSIVE) && !--cur->refs) {
					phpdbg_delete_watchpoints_recursive(watch);
				}
			}
			if (flags & PHPDBG_WATCH_NORMAL) {
				zend_hash_del(&cur->watches, watch->str);
				if (zend_hash_num_elements(&cur->watches) > 0) {
					cur->watch = Z_PTR_P(zend_hash_get_current_data_ex(&cur->watches, NULL));
				} else {
					cur->watch = Z_PTR_P(zend_hash_get_current_data_ex(&cur->implicit_watches, NULL));
				}
			}
			if (flags & PHPDBG_WATCH_IMPLICIT) {
				zend_hash_del(&cur->implicit_watches, watch->str);
			}

			old->flags = watch->flags;
			phpdbg_free_watch(watch);
			efree(watch);
			watch = old;
		}
		if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
			if (!(flags & PHPDBG_WATCH_RECURSIVE) && !cur->refs++) {
				phpdbg_create_recursive_zval_watch(watch->parent);
			}
		}
	} else {
		phpdbg_watch_collision coll;
		coll.refs = (watch->flags & PHPDBG_WATCH_RECURSIVE) != 0;
		coll.watch = watch;
		zend_hash_init(&coll.watches, 8, arghs, NULL, 0);
		zend_hash_init(&coll.implicit_watches, 8, ..., NULL, 0);
		cur = zend_hash_index_add_mem(&PHPDBG_G(watch_collisions), (zend_ulong) watch->addr.ref, &coll, sizeof(phpdbg_watch_collision));
		phpdbg_store_watchpoint(cur->watch);
		phpdbg_activate_watchpoint(cur->watch);
		if (coll.refs) {
			phpdbg_create_recursive_zval_watch(watch->parent);
		}
	}

	if (watch->flags & PHPDBG_WATCH_NORMAL) {
		cur->watch = watch;
		zend_hash_add_ptr(&cur->watches, watch->str, watch->parent);
	}
	if (watch->flags & PHPDBG_WATCH_IMPLICIT) {
		zend_hash_add_ptr(&cur->implicit_watches, watch->str, watch->parent);
	}
}

static void phpdbg_remove_watch_collision(phpdbg_watchpoint_t *watch) {
	phpdbg_watch_collision *cur;
	if ((cur = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) Z_COUNTED_P(watch->addr.zv)))) {
		if (cur->refs && !--cur->refs) {
			phpdbg_delete_watchpoints_recursive(watch);
		}

		zend_hash_del(&cur->watches, watch->str);
		zend_hash_del(&cur->implicit_watches, watch->str);

		if (zend_hash_num_elements(&cur->watches) > 0) {
			cur->watch = Z_PTR_P(zend_hash_get_current_data_ex(&cur->watches, NULL));
		} else if (zend_hash_num_elements(&cur->implicit_watches) > 0) {
			cur->watch = Z_PTR_P(zend_hash_get_current_data_ex(&cur->implicit_watches, NULL));
		} else {
			phpdbg_deactivate_watchpoint(cur->watch);
			phpdbg_remove_watchpoint(cur->watch);

			zend_hash_index_del(&PHPDBG_G(watch_collisions), (zend_ulong) Z_COUNTED_P(watch->addr.zv));
		}
	}
}

static phpdbg_watchpoint_t *phpdbg_create_watchpoint(phpdbg_watchpoint_t *watch);

static phpdbg_watchpoint_t *phpdbg_create_reference_watch(phpdbg_watchpoint_t *watch) {
	phpdbg_watchpoint_t *ref = emalloc(sizeof(phpdbg_watchpoint_t));
	watch->reference = ref;
	ref->flags = watch->flags;
	ref->str = watch->str;
	++GC_REFCOUNT(ref->str);
	ref->parent = watch;
	ref->parent_container = NULL;
	phpdbg_create_zval_watchpoint(Z_REFVAL_P(watch->addr.zv), ref);

	phpdbg_create_watchpoint(ref);

	return ref;
}

static phpdbg_watchpoint_t *phpdbg_get_refcount_watch(phpdbg_watchpoint_t *parent) {
	zend_refcounted *ref;
	phpdbg_btree_result *res;

	if (parent->type == WATCH_ON_HASHTABLE) {
		parent = parent->parent;
		if (!parent) {
			return NULL;
		}
	}

	ZEND_ASSERT(parent->type == WATCH_ON_ZVAL);
	ref = Z_COUNTED_P(parent->addr.zv);

	res = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) ref);
	if (res) {
		return res->ptr;
	}
	return NULL;
}

static phpdbg_watchpoint_t *phpdbg_create_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_watchpoint_t *ret = watch;

	if (watch->type == WATCH_ON_ZVAL) {
		switch (Z_TYPE_P(watch->addr.zv)) {
			case IS_NULL:
			case IS_UNDEF:
			case IS_TRUE:
			case IS_FALSE:
				memset(watch->addr.zv, 0, sizeof(zend_value));
		}
	}

	/* exclude references & refcounted */
	if (!watch->parent || watch->parent->type != WATCH_ON_ZVAL || watch->type == WATCH_ON_HASHTABLE) {
		phpdbg_watchpoint_t *old_watch = zend_hash_find_ptr(&PHPDBG_G(watchpoints), watch->str);

		if (old_watch) {
#define return_and_free_watch(x) { \
	phpdbg_watchpoint_t *ref = phpdbg_get_refcount_watch(old_watch); \
	if (ref) { \
		phpdbg_add_watch_collision(ref); \
	} \
	if (watch != old_watch) { \
		phpdbg_free_watch(watch); \
		efree(watch); \
	} \
	return (x); \
}
			if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
				if (old_watch->flags & PHPDBG_WATCH_RECURSIVE) {
					return_and_free_watch(NULL);
				} else {
					old_watch->flags &= ~(PHPDBG_WATCH_SIMPLE | PHPDBG_WATCH_IMPLICIT);
					old_watch->flags |= PHPDBG_WATCH_RECURSIVE;
					return_and_free_watch(old_watch);
				}
			} else {
				if (!(old_watch->flags & PHPDBG_WATCH_RECURSIVE)) {
					old_watch->flags |= watch->flags & (PHPDBG_WATCH_IMPLICIT | PHPDBG_WATCH_SIMPLE);
				}
				return_and_free_watch(NULL);
			}
		} else {
			if (watch->parent && watch->parent->type == WATCH_ON_HASHTABLE) {
				watch->parent->implicit_ht_count++;
			}
			zend_hash_add_ptr(&PHPDBG_G(watchpoints), watch->str, watch);
		}
	}

	phpdbg_store_watchpoint(watch);

	if (watch->parent && watch->parent->type == WATCH_ON_ZVAL && Z_REFCOUNTED_P(watch->parent->addr.zv)) {
		phpdbg_add_watch_collision(phpdbg_create_refcounted_watchpoint(watch, Z_COUNTED_P(watch->parent->addr.zv)));
	}

	if (watch->type == WATCH_ON_ZVAL) {
		if (watch->parent_container) {
			HashTable *ht_watches;
			phpdbg_btree_result *find;
			if (!(find = phpdbg_btree_find(&PHPDBG_G(watch_HashTables), (zend_ulong) watch->parent_container))) {
				phpdbg_watch_ht_info *hti = emalloc(sizeof(*hti));
				hti->dtor = watch->parent_container->pDestructor;
				ht_watches = &hti->watches;
				zend_hash_init(ht_watches, 0, grrrrr, ZVAL_PTR_DTOR, 0);
				phpdbg_btree_insert(&PHPDBG_G(watch_HashTables), (zend_ulong) watch->parent_container, hti);
				watch->parent_container->pDestructor = (dtor_func_t) phpdbg_watch_HashTable_dtor;
			} else {
				ht_watches = &((phpdbg_watch_ht_info *) find->ptr)->watches;
			}
			zend_hash_add_ptr(ht_watches, watch->name_in_parent, watch);
		}

		if (Z_ISREF_P(watch->addr.zv)) {
			ret = phpdbg_create_reference_watch(watch);
		}
	}

	phpdbg_activate_watchpoint(watch);

	return ret;
}

static int phpdbg_create_simple_watchpoint(phpdbg_watchpoint_t *watch) {
	watch->flags |= PHPDBG_WATCH_SIMPLE;

	phpdbg_create_watchpoint(watch);

	return SUCCESS;
}

static int phpdbg_create_array_watchpoint(phpdbg_watchpoint_t *zv_watch) {
	zval *zv = zv_watch->addr.zv;
	phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));
	HashTable *ht = HT_FROM_ZVP(zv);

	watch->parent = zv_watch;

	if (!ht) {
		return FAILURE;
	}

	phpdbg_create_ht_watchpoint(ht, watch);

	if (phpdbg_create_watchpoint(watch) == NULL) {
		return SUCCESS;
	}

	if (Z_TYPE_P(zv) == IS_ARRAY) {
		watch->flags |= PHPDBG_WATCH_ARRAY;
	} else {
		watch->flags |= PHPDBG_WATCH_OBJECT;
	}

	phpdbg_add_watch_collision(phpdbg_create_refcounted_watchpoint(watch, Z_COUNTED_P(zv)));

	return SUCCESS;
}

static int phpdbg_create_recursive_watchpoint(phpdbg_watchpoint_t *watch) {
	if (watch->type != WATCH_ON_ZVAL) {
		return FAILURE;
	}

	watch->flags |= PHPDBG_WATCH_RECURSIVE;
	watch = phpdbg_create_watchpoint(watch);

	return SUCCESS;
}

static int phpdbg_create_recursive_ht_watch(phpdbg_watchpoint_t *watch) {
	zval *zv;
	zend_string *key;
	zend_long h;

	ZEND_ASSERT(watch->type == WATCH_ON_HASHTABLE);

	ZEND_HASH_FOREACH_KEY_VAL(HT_WATCH_HT(watch), h, key, zv) {
		phpdbg_watchpoint_t *new_watch = emalloc(sizeof(phpdbg_watchpoint_t));

		new_watch->flags = PHPDBG_WATCH_RECURSIVE;
		new_watch->parent = watch;
		new_watch->parent_container = HT_WATCH_HT(watch);

		if (key) {
			new_watch->name_in_parent = key;
			++GC_REFCOUNT(key);
		} else {
			new_watch->name_in_parent = strpprintf(0, ZEND_LONG_FMT, h);
		}

		new_watch->str = strpprintf(0, "%.*s%s%s%s", (int) ZSTR_LEN(watch->str) - 2, ZSTR_VAL(watch->str), (watch->flags & PHPDBG_WATCH_ARRAY) ? "[" : "->", phpdbg_get_property_key(ZSTR_VAL(new_watch->name_in_parent)), (watch->flags & PHPDBG_WATCH_ARRAY) ? "]" : "");

		while (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
		}

		phpdbg_create_zval_watchpoint(zv, new_watch);
		new_watch = phpdbg_create_watchpoint(new_watch);
		phpdbg_create_recursive_zval_watch(new_watch);
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

static int phpdbg_create_recursive_zval_watch(phpdbg_watchpoint_t *watch) {
	HashTable *ht;
	zval *zvp;

	ZEND_ASSERT(watch->type == WATCH_ON_ZVAL);

	zvp = watch->addr.zv;
	ZVAL_DEREF(zvp);

	if ((ht = HT_FROM_ZVP(zvp))) {
		phpdbg_watchpoint_t *new_watch = emalloc(sizeof(phpdbg_watchpoint_t));

		new_watch->flags = PHPDBG_WATCH_RECURSIVE;
		new_watch->parent = watch;
		new_watch->parent_container = watch->parent_container;
		new_watch->name_in_parent = watch->name_in_parent;
		++GC_REFCOUNT(new_watch->name_in_parent);
		new_watch->str = strpprintf(0, "%.*s[]", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str));

		if (Z_TYPE_P(zvp) == IS_ARRAY) {
			new_watch->flags |= PHPDBG_WATCH_ARRAY;
		} else {
			new_watch->flags |= PHPDBG_WATCH_OBJECT;
		}

		phpdbg_create_ht_watchpoint(ht, new_watch);

		phpdbg_create_recursive_ht_watch(new_watch);

		phpdbg_create_watchpoint(new_watch);
	}

	return SUCCESS;
}

static void phpdbg_delete_implicit_parents(phpdbg_watchpoint_t *watch) {
	phpdbg_watchpoint_t *parent = watch->parent;
	if (!parent) {
		return;
	}

	ZEND_ASSERT(!(parent->flags & PHPDBG_WATCH_RECURSIVE));
	ZEND_ASSERT(parent->flags & PHPDBG_WATCH_IMPLICIT);

	if (parent->type == WATCH_ON_HASHTABLE && --parent->implicit_ht_count) {
		return;
	}

	parent->flags &= ~PHPDBG_WATCH_IMPLICIT;
	if (!(parent->flags & PHPDBG_WATCH_SIMPLE)) {
		if (parent->type == WATCH_ON_ZVAL && Z_REFCOUNTED_P(watch->addr.zv)) {
			phpdbg_remove_watch_collision(parent);
		}
		zend_hash_del(&PHPDBG_G(watchpoints), parent->str);
	}
}

/* delete watchpoint, recursively (and inclusively) */
static int phpdbg_delete_watchpoint_recursive(phpdbg_watchpoint_t *watch, zend_bool user_request) {
	if (watch->type == WATCH_ON_HASHTABLE) {
		if (user_request) {
			phpdbg_delete_ht_watchpoints_recursive(watch);
		} else {
			HashTable *ht;
			phpdbg_btree_result *result;

			ht = HT_FROM_ZVP(watch->addr.zv);

			if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) ht))) {
				phpdbg_delete_watchpoint_recursive((phpdbg_watchpoint_t *) result->ptr, user_request);
			}
		}
	} else if (watch->type == WATCH_ON_ZVAL) {
		phpdbg_delete_zval_watchpoints_recursive(watch);
	}

	return zend_hash_del(&PHPDBG_G(watchpoints), watch->str);
}

static void phpdbg_delete_ht_watchpoints_recursive(phpdbg_watchpoint_t *watch) {
	zend_string *str, *strkey;
	zend_long numkey;
	phpdbg_watchpoint_t *watchpoint;

	ZEND_HASH_FOREACH_KEY(HT_WATCH_HT(watch), numkey, strkey) {
		if (strkey) {
			str = strpprintf(0, "%.*s%s%s%s", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str), (watch->flags & PHPDBG_WATCH_ARRAY) ? "[" : "->", phpdbg_get_property_key(ZSTR_VAL(strkey)), (watch->flags & PHPDBG_WATCH_ARRAY) ? "]" : "");
		} else {
			str = strpprintf(0, "%.*s%s" ZEND_LONG_FMT "%s", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str), (watch->flags & PHPDBG_WATCH_ARRAY) ? "[" : "->", numkey, (watch->flags & PHPDBG_WATCH_ARRAY) ? "]" : "");
		}

		if ((watchpoint = zend_hash_find_ptr(&PHPDBG_G(watchpoints), str))) {
			phpdbg_delete_watchpoint_recursive(watchpoint, 1);
		}

		zend_string_release(str);
	} ZEND_HASH_FOREACH_END();
}

static void phpdbg_delete_zval_watchpoints_recursive(phpdbg_watchpoint_t *watch) {
	if (Z_REFCOUNTED_P(watch->addr.zv)) {
		phpdbg_remove_watch_collision(watch);
	}
}

/* delete watchpoints recusively (exclusively!) */
static void phpdbg_delete_watchpoints_recursive(phpdbg_watchpoint_t *watch) {
	if (watch->type == WATCH_ON_ZVAL) {
		phpdbg_delete_zval_watchpoints_recursive(watch);
	} else if (watch->type == WATCH_ON_HASHTABLE) {
		phpdbg_delete_ht_watchpoints_recursive(watch);
	}
}

static int phpdbg_delete_watchpoint(phpdbg_watchpoint_t *tmp_watch) {
	int ret;
	phpdbg_watchpoint_t *watch;
	phpdbg_btree_result *result;

	if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) tmp_watch->addr.ptr)) == NULL) {
		return FAILURE;
	}

	watch = result->ptr;

	if (!(watch->flags & PHPDBG_WATCH_NORMAL) || (watch->parent && (watch->parent->flags & PHPDBG_WATCH_RECURSIVE))) {
		return FAILURE; /* TODO: better error message for recursive (??) */
	}

	if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
		ret = phpdbg_delete_watchpoint_recursive(watch, 1);
	} else {
		ret = zend_hash_del(&PHPDBG_G(watchpoints), watch->str);
	}

	phpdbg_free_watch(tmp_watch);
	efree(tmp_watch);

	return ret;
}

static int phpdbg_watchpoint_parse_wrapper(char *name, size_t namelen, char *key, size_t keylen, HashTable *parent, zval *zv, int (*callback)(phpdbg_watchpoint_t *)) {
	int ret;
	phpdbg_watchpoint_t *watch = ecalloc(1, sizeof(phpdbg_watchpoint_t));
	watch->str = zend_string_init(name, namelen, 0);
	watch->name_in_parent = zend_string_init(key, keylen, 0);
	watch->parent_container = parent;
	phpdbg_create_zval_watchpoint(zv, watch);

	ret = callback(watch);

	efree(name);
	efree(key);

	if (ret != SUCCESS) {
		phpdbg_free_watch(watch);
		efree(watch);
	}

	PHPDBG_G(watch_tmp) = NULL;

	return ret;
}

PHPDBG_API int phpdbg_watchpoint_parse_input(char *input, size_t len, HashTable *parent, size_t i, int (*callback)(phpdbg_watchpoint_t *), zend_bool silent) {
	return phpdbg_parse_variable_with_arg(input, len, parent, i, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_wrapper, NULL, 0, callback);
}

static int phpdbg_watchpoint_parse_step(char *name, size_t namelen, char *key, size_t keylen, HashTable *parent, zval *zv, int (*callback)(phpdbg_watchpoint_t *)) {
	phpdbg_watchpoint_t *watch;

	if ((watch = zend_hash_str_find_ptr(&PHPDBG_G(watchpoints), name, namelen))) {
		watch->flags |= PHPDBG_WATCH_IMPLICIT;
		PHPDBG_G(watch_tmp) = watch;
		return SUCCESS;
	}

	watch = ecalloc(1, sizeof(phpdbg_watchpoint_t));
	watch->flags = PHPDBG_WATCH_IMPLICIT;
	watch->str = zend_string_init(name, namelen, 0);
	watch->name_in_parent = zend_string_init(key, keylen, 0);
	watch->parent_container = parent;
	watch->parent = PHPDBG_G(watch_tmp);
	phpdbg_create_zval_watchpoint(zv, watch);

	phpdbg_create_watchpoint(watch);

	efree(name);
	efree(key);

	PHPDBG_G(watch_tmp) = watch;
	return SUCCESS;
}

static int phpdbg_watchpoint_parse_symtables(char *input, size_t len, int (*callback)(phpdbg_watchpoint_t *)) {
	if (EG(scope) && len >= 5 && !memcmp("$this", input, 5)) {
		zend_hash_str_add(EG(current_execute_data)->symbol_table, ZEND_STRL("this"), &EG(current_execute_data)->This);
	}

	if (phpdbg_is_auto_global(input, len) && phpdbg_watchpoint_parse_input(input, len, &EG(symbol_table), 0, callback, 1) != FAILURE) {
		return SUCCESS;
	}

	return phpdbg_parse_variable_with_arg(input, len, EG(current_execute_data)->symbol_table, 0, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_wrapper, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_step, 0, callback);
}

PHPDBG_WATCH(delete) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			if (phpdbg_delete_var_watchpoint(param->str, param->len) == FAILURE) {
				phpdbg_error("watchdelete", "type=\"nowatch\"", "Nothing was deleted, no corresponding watchpoint found");
			} else {
				phpdbg_notice("watchdelete", "variable=\"%.*s\"", "Removed watchpoint %.*s", (int) param->len, param->str);
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_WATCH(recursive) /* {{{ */
{
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			if (phpdbg_watchpoint_parse_symtables(param->str, param->len, phpdbg_create_recursive_watchpoint) != FAILURE) {
				phpdbg_notice("watchrecursive", "variable=\"%.*s\"", "Set recursive watchpoint on %.*s", (int)param->len, param->str);
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_WATCH(array) /* {{{ */
{
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			if (phpdbg_watchpoint_parse_symtables(param->str, param->len, phpdbg_create_array_watchpoint) != FAILURE) {
				phpdbg_notice("watcharray", "variable=\"%.*s\"", "Set array watchpoint on %.*s", (int)param->len, param->str);
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

void phpdbg_watch_HashTable_dtor(zval *zv) {
	phpdbg_btree_result *result;
	zval *orig_zv = zv;

	while (Z_TYPE_P(zv) == IS_INDIRECT) {
		zv = Z_INDIRECT_P(zv);
	}

	if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) zv))) {
		phpdbg_watchpoint_t *watch = result->ptr;

		if (watch->flags & PHPDBG_WATCH_NORMAL) {
			PHPDBG_G(watchpoint_hit) = 1;

			phpdbg_notice("watchdelete", "variable=\"%.*s\" recursive=\"%s\"", "%.*s was removed, removing watchpoint%s", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str), (watch->flags & PHPDBG_WATCH_RECURSIVE) ? " recursively" : "");
		}

		if ((result = phpdbg_btree_find(&PHPDBG_G(watch_HashTables), (zend_ulong) watch->parent_container))) {
			phpdbg_watch_ht_info *hti = result->ptr;
			hti->dtor(orig_zv);
			zend_hash_del(&hti->watches, watch->name_in_parent);
			if (zend_hash_num_elements(&hti->watches) == 0) {
				watch->parent_container->pDestructor = hti->dtor;
				zend_hash_destroy(&hti->watches);
				phpdbg_btree_delete(&PHPDBG_G(watch_HashTables), (zend_ulong) watch->parent_container);
				efree(hti);
			}
		} else {
			zval_ptr_dtor_wrapper(orig_zv);
		}

		if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
			phpdbg_delete_watchpoint_recursive(watch, 0);
		} else {
			zend_hash_del(&PHPDBG_G(watchpoints), watch->str);
		}
	}
}

int phpdbg_create_var_watchpoint(char *input, size_t len) {
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return FAILURE;
	}

	return phpdbg_watchpoint_parse_symtables(input, len, phpdbg_create_simple_watchpoint);
}

int phpdbg_delete_var_watchpoint(char *input, size_t len) {
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return FAILURE;
	}

	return phpdbg_watchpoint_parse_input(input, len, EG(current_execute_data)->symbol_table, 0, phpdbg_delete_watchpoint, 0);
}

#ifdef _WIN32
int phpdbg_watchpoint_segfault_handler(void *addr) {
#else
int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context) {
#endif
	void *page;
	phpdbg_watch_memdump *dump;
	phpdbg_watchpoint_t *watch;
	size_t size;

	watch = phpdbg_check_for_watchpoint(
#ifdef _WIN32
		addr
#else
		info->si_addr
#endif
		);

	if (watch == NULL) {
		return FAILURE;
	}

	page = phpdbg_get_page_boundary(watch->addr.ptr);
	size = phpdbg_get_total_page_size(watch->addr.ptr, watch->size);

	/* re-enable writing */
	mprotect(page, size, PROT_READ | PROT_WRITE);

	dump = malloc(MEMDUMP_SIZE(size));
	dump->page = page;
	dump->size = size;
	dump->reenable_writing = 0;

	memcpy(&dump->data, page, size);

	zend_llist_add_element(&PHPDBG_G(watchlist_mem), &dump);

	return SUCCESS;
}

void phpdbg_watchpoints_clean(void) {
	zend_hash_clean(&PHPDBG_G(watchpoints));
}

/* due to implicit delete... MUST BE DESTROYED MANUALLY */
static void phpdbg_watch_dtor(zval *pDest) {
	phpdbg_watchpoint_t *watch = (phpdbg_watchpoint_t *) Z_PTR_P(pDest);

	if (watch->flags & PHPDBG_WATCH_IMPLICIT) {
		watch->flags = PHPDBG_WATCH_SIMPLE; // tiny hack for delete_implicit_parents

		if (watch->type == WATCH_ON_ZVAL) {
			phpdbg_delete_zval_watchpoints_recursive(watch);
		} else if (watch->type == WATCH_ON_HASHTABLE) {
			phpdbg_watchpoint_t *watchpoint;

			watch->implicit_ht_count++;

			ZEND_HASH_FOREACH_PTR(&((phpdbg_watch_ht_info *) phpdbg_btree_find(&PHPDBG_G(watch_HashTables), (zend_ulong) HT_WATCH_HT(watch))->ptr)->watches, watchpoint) {
				phpdbg_delete_watchpoint_recursive(watchpoint, 1);
			} ZEND_HASH_FOREACH_END();
		}
	}

	phpdbg_delete_implicit_parents(watch);

	phpdbg_deactivate_watchpoint(watch);
	phpdbg_remove_watchpoint(watch);

	phpdbg_free_watch(watch);
	efree(watch);
}

static void phpdbg_watch_mem_dtor(void *llist_data) {
	phpdbg_watch_memdump *dump = *(phpdbg_watch_memdump **) llist_data;

	/* Disble writing again */
	if (dump->reenable_writing) {
		mprotect(dump->page, dump->size, PROT_READ);
	}

	free(dump);
}

static void phpdbg_watch_free_ptr_dtor(zval *ptr) {
	efree(Z_PTR_P(ptr));
}

void phpdbg_setup_watchpoints(void) {
#if _SC_PAGE_SIZE
	phpdbg_pagesize = sysconf(_SC_PAGE_SIZE);
#elif _SC_PAGESIZE
	phpdbg_pagesize = sysconf(_SC_PAGESIZE);
#elif _SC_NUTC_OS_PAGESIZE
	phpdbg_pagesize = sysconf(_SC_NUTC_OS_PAGESIZE);
#else
	phpdbg_pagesize = 4096; /* common pagesize */
#endif

	zend_llist_init(&PHPDBG_G(watchlist_mem), sizeof(void *), phpdbg_watch_mem_dtor, 1);
	phpdbg_btree_init(&PHPDBG_G(watchpoint_tree), sizeof(void *) * 8);
	phpdbg_btree_init(&PHPDBG_G(watch_HashTables), sizeof(void *) * 8);
	zend_hash_init(&PHPDBG_G(watchpoints), 8, NULL, phpdbg_watch_dtor, 0);
	zend_hash_init(&PHPDBG_G(watch_collisions), 8, NULL, phpdbg_watch_free_ptr_dtor, 0);
}

static void phpdbg_print_changed_zval(phpdbg_watch_memdump *dump) {
	/* fetch all changes between dump->page and dump->page + dump->size */
	phpdbg_btree_position pos = phpdbg_btree_find_between(&PHPDBG_G(watchpoint_tree), (zend_ulong) dump->page, (zend_ulong) dump->page + dump->size);
	phpdbg_btree_result *result;
	int elementDiff;
	void *curTest;

	dump->reenable_writing = 0;

	while ((result = phpdbg_btree_next(&pos))) {
		phpdbg_watchpoint_t *watch = result->ptr;
		void *oldPtr = (char *) &dump->data + ((size_t) watch->addr.ptr - (size_t) dump->page);
		char reenable = 1;
		int removed = 0;

		if ((size_t) watch->addr.ptr < (size_t) dump->page || (size_t) watch->addr.ptr + watch->size > (size_t) dump->page + dump->size) {
			continue;
		}

		/* Test if the zval was separated or replaced and if necessary move the watchpoint */
		if ((watch->type == WATCH_ON_HASHTABLE || watch->type == WATCH_ON_ZVAL) && watch->parent_container) {
			if ((curTest = zend_symtable_find(watch->parent_container, watch->name_in_parent))) {
				while (Z_TYPE_P((zval *) curTest) == IS_INDIRECT) {
					curTest = Z_INDIRECT_P((zval *) curTest);
				}

				if (watch->type == WATCH_ON_HASHTABLE) {
					switch (Z_TYPE_P((zval *) curTest)) {
						case IS_ARRAY:
							curTest = (void *) Z_ARRVAL_P((zval *) curTest);
							break;
						case IS_OBJECT:
							curTest = (void *) Z_OBJPROP_P((zval *) curTest);
							break;
					}
				}

				if (curTest != watch->addr.ptr) {
					phpdbg_deactivate_watchpoint(watch);
					phpdbg_remove_watchpoint(watch);
					watch->addr.ptr = curTest;
					phpdbg_store_watchpoint(watch);
					phpdbg_activate_watchpoint(watch);

					reenable = 0;
				}
			} else {
				removed = 1;
			}
		}

		/* Show to the user what changed and delete watchpoint upon removal */
		{
			zend_bool do_break = 0;

			switch (watch->type) {
				case WATCH_ON_ZVAL:
					do_break = memcmp(oldPtr, watch->addr.zv, sizeof(zend_value) + sizeof(uint32_t) /* value + typeinfo */) != 0;
					if (!do_break) {
						goto end;
					}
					break;
				case WATCH_ON_HASHTABLE:
					do_break = zend_hash_num_elements(HT_PTR_HT(oldPtr)) != zend_hash_num_elements(HT_WATCH_HT(watch));
					if (!do_break) {
						goto end;
					}
					break;
				case WATCH_ON_REFCOUNTED:
					do_break = memcmp(oldPtr, watch->addr.ref, sizeof(uint32_t) /* no zend_refcounted metadata info */) != 0;
					if (!do_break) {
						goto end;
					}
					if (!(PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS)) {
						do_break = 0;
					}
					break;
			}

			if (!(watch->flags & PHPDBG_WATCH_NORMAL)) {
				do_break = 0;
			}

			if (do_break) {
				PHPDBG_G(watchpoint_hit) = 1;

				phpdbg_notice("watchhit", "variable=\"%s\"", "Breaking on watchpoint %.*s", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str));
				phpdbg_xml("<watchdata %r>");
			}

			switch (watch->type) {
				case WATCH_ON_ZVAL: {
					zend_bool show_value = memcmp(oldPtr, watch->addr.zv, sizeof(zend_value) + sizeof(uint32_t) /* no metadata info */) != 0;

					if ((watch->flags & PHPDBG_WATCH_NORMAL) && (removed || show_value)) {
/* TODO: Merge with refcounting watches, store if watched ref value is to be dropped etc. [for example: manually increment refcount transparently for displaying and drop it if it decrements to 1] */
						if (Z_REFCOUNTED_P((zval *) oldPtr)) {
							phpdbg_writeln("watchvalue", "type=\"old\" inaccessible=\"inaccessible\"", "Old value inaccessible or destroyed");
						} else {
							phpdbg_out("Old value: ");
							phpdbg_xml("<watchvalue %r type=\"old\">");
							zend_print_flat_zval_r((zval *) oldPtr);
							phpdbg_xml("</watchvalue>");
							phpdbg_out("\n");
						}
					}

					/* check if zval was removed */
					if (removed) {
						if (watch->flags & PHPDBG_WATCH_NORMAL) {
							phpdbg_notice("watchdelete", "variable=\"%.*s\"", "Watchpoint %.*s was unset, removing watchpoint", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str));
						}
						zend_hash_del(&PHPDBG_G(watchpoints), watch->str);

						reenable = 0;

						if (Z_REFCOUNTED_P((zval *) oldPtr)) {
							phpdbg_remove_watch_collision(watch);
						}
						break;
					}

					if ((watch->flags & PHPDBG_WATCH_NORMAL) && show_value) {
						phpdbg_out("New value%s: ", Z_ISREF_P(watch->addr.zv) ? " (reference)" : "");
						phpdbg_xml("<watchvalue %r%s type=\"new\">", Z_ISREF_P(watch->addr.zv) ? " reference=\"reference\"" : "");
						zend_print_flat_zval_r(watch->addr.zv);
						phpdbg_xml("</watchvalue>");
						phpdbg_out("\n");
					}

					/* add new watchpoints if necessary */
					if (Z_PTR_P(watch->addr.zv) != Z_PTR_P((zval *) oldPtr) || Z_TYPE_P(watch->addr.zv) != Z_TYPE_P((zval *) oldPtr)) {
						if (Z_REFCOUNTED_P((zval *) oldPtr)) {
							zval *new_zv = watch->addr.zv;
							watch->addr.ptr = oldPtr;
							phpdbg_remove_watch_collision(watch);
							watch->addr.zv = new_zv;
						}
						if (Z_REFCOUNTED_P(watch->addr.zv)) {
							if ((watch->flags & PHPDBG_WATCH_NORMAL) && (PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS)) {
								phpdbg_writeln("watchrefcount", "type=\"new\" refcount=\"%d\"", "New refcount: %d", Z_REFCOUNT_P(watch->addr.zv));
							}
							if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
								phpdbg_create_recursive_watchpoint(watch);
							} else if (Z_ISREF_P(watch->addr.zv)) {
								phpdbg_create_reference_watch(watch);
							}
						}
					}

					break;
				}
				case WATCH_ON_HASHTABLE:
					/* We should be safely able to assume the HashTable to be consistent (inconsistent HashTables should have been caught by phpdbg_watch_efree() */
					elementDiff = zend_hash_num_elements(HT_PTR_HT(oldPtr)) - zend_hash_num_elements(HT_WATCH_HT(watch));
					if ((watch->flags & PHPDBG_WATCH_NORMAL) && elementDiff) {
						if (elementDiff > 0) {
							phpdbg_writeln("watchsize", "removed=\"%d\"", "%d elements were removed from the array", elementDiff);
						} else {
							phpdbg_writeln("watchsize", "added=\"%d\"", "%d elements were added to the array", -elementDiff);
						}
					}
					/* add new watchpoints if necessary */
					if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
						phpdbg_create_recursive_ht_watch(watch);
					}
					if ((watch->flags & PHPDBG_WATCH_NORMAL) && HT_WATCH_HT(watch)->nInternalPointer != HT_PTR_HT(oldPtr)->nInternalPointer) {
						phpdbg_writeln("watcharrayptr", "", "Internal pointer of array was changed");
					}
					break;
				case WATCH_ON_REFCOUNTED: {
					if ((watch->flags & PHPDBG_WATCH_NORMAL) && (PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS)) {
						phpdbg_writeln("watchrefcount", "type=\"old\" refcount=\"%d\"", "Old refcount: %d", GC_REFCOUNT((zend_refcounted *) oldPtr));
						phpdbg_writeln("watchrefcount", "type=\"new\" refcount=\"%d\"", "New refcount: %d", GC_REFCOUNT(watch->addr.ref));
					}
					break;
				}
			}

			if (do_break) {
				phpdbg_xml("</watchdata>");
			}
		} end:

		dump->reenable_writing = dump->reenable_writing | reenable;
	}
}

int phpdbg_print_changed_zvals(void) {
	zend_llist_position pos;
	phpdbg_watch_memdump **dump;
	int ret;

	if (zend_llist_count(&PHPDBG_G(watchlist_mem)) == 0) {
		return FAILURE;
	}

	dump = (phpdbg_watch_memdump **) zend_llist_get_last_ex(&PHPDBG_G(watchlist_mem), &pos);

	do {
		phpdbg_print_changed_zval(*dump);
	} while ((dump = (phpdbg_watch_memdump **) zend_llist_get_prev_ex(&PHPDBG_G(watchlist_mem), &pos)));

	zend_llist_clean(&PHPDBG_G(watchlist_mem));

	ret = PHPDBG_G(watchpoint_hit) ? SUCCESS : FAILURE;
	PHPDBG_G(watchpoint_hit) = 0;

	return ret;
}

void phpdbg_list_watchpoints(void) {
	phpdbg_watchpoint_t *watch;

	phpdbg_xml("<watchlist %r>");

	ZEND_HASH_FOREACH_PTR(&PHPDBG_G(watchpoints), watch) {
		if (watch->flags & PHPDBG_WATCH_NORMAL) {
			phpdbg_writeln("watchvariable", "variable=\"%.*s\" on=\"%s\" type=\"%s\"", "%.*s (%s, %s)", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str), watch->type == WATCH_ON_HASHTABLE ? "array" : watch->type == WATCH_ON_REFCOUNTED ? "refcount" : "variable", watch->flags == PHPDBG_WATCH_RECURSIVE ? "recursive" : "simple");
		}
	} ZEND_HASH_FOREACH_END();

	phpdbg_xml("</watchlist>");
}

void phpdbg_watch_efree(void *ptr) {
	phpdbg_btree_result *result;

	result = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), (zend_ulong) ptr);

	if (result) {
		phpdbg_watchpoint_t *watch = result->ptr;

		if ((size_t) watch->addr.ptr + watch->size > (size_t) ptr) {
			if (watch->type == WATCH_ON_REFCOUNTED) {
				/* remove watchpoint here from btree, zval watchpoint will remove it via remove_watch_collison */
				phpdbg_deactivate_watchpoint(watch);
				phpdbg_remove_watchpoint(watch);
			} else {
				if (watch->type == WATCH_ON_ZVAL) {
					phpdbg_remove_watch_collision(watch);
				}
				if (watch->type == WATCH_ON_HASHTABLE && (watch->flags & PHPDBG_WATCH_SIMPLE)) {
					/* when a HashTable is freed, we can safely assume the other zvals all were dtor'ed */
					phpdbg_notice("watchdelete", "variable=\"%.*s\" recursive=\"%s\"", "Array %.*s was removed, removing watchpoint%s", (int) ZSTR_LEN(watch->str), ZSTR_VAL(watch->str), (watch->flags & PHPDBG_WATCH_RECURSIVE) ? " recursively" : "");
				}
				if (watch->type == WATCH_ON_HASHTABLE || watch->parent == NULL || watch->parent->type != WATCH_ON_ZVAL) { /* no references */
					zend_hash_del(&PHPDBG_G(watchpoints), watch->str);
				}
			}
		}
	}

	if (PHPDBG_G(original_free_function)) {
		PHPDBG_G(original_free_function)(ptr);
	}
}
