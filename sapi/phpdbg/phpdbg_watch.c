/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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

#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_btree.h"
#include "phpdbg_watch.h"
#include "phpdbg_utils.h"
#ifndef _WIN32
# include <unistd.h>
# include <sys/mman.h>
#endif

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

const phpdbg_command_t phpdbg_watch_commands[] = {
	PHPDBG_COMMAND_D_EX(array,      "create watchpoint on an array", 'a', watch_array,     NULL, "s", 0),
	PHPDBG_COMMAND_D_EX(delete,     "delete watchpoint",             'd', watch_delete,    NULL, "s", 0),
	PHPDBG_COMMAND_D_EX(recursive,  "create recursive watchpoints",  'r', watch_recursive, NULL, "s", 0),
	PHPDBG_END_COMMAND
};

//#define HT_FROM_WATCH(watch) (watch->type == WATCH_ON_OBJECT ? watch->addr.obj->handlers->get_properties(watch->parent_container.zv) : watch->type == WATCH_ON_ARRAY ? &watch->addr.arr->ht : NULL)
#define HT_FROM_ZVP(zvp) (Z_TYPE_P(zvp) == IS_OBJECT ? Z_OBJPROP_P(zvp) : Z_TYPE_P(zvp) == IS_ARRAY ? Z_ARRVAL_P(zvp) : NULL)

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
	phpdbg_create_addr_watchpoint(ht, sizeof(HashTable), watch);
	watch->type = WATCH_ON_HASHTABLE;
}

void phpdbg_watch_HashTable_dtor(zval *ptr);

static int phpdbg_delete_watchpoint(phpdbg_watchpoint_t *tmp_watch);
static void phpdbg_delete_ht_watchpoints_recursive(phpdbg_watchpoint_t *watch);
static void phpdbg_delete_zval_watchpoints_recursive(phpdbg_watchpoint_t *watch);
static void phpdbg_delete_watchpoints_recursive(phpdbg_watchpoint_t *watch);

/* TODO: Store all the possible watches the refcounted may refer to (for displaying & deleting by identifier) */

static phpdbg_watchpoint_t *phpdbg_create_refcounted_watchpoint(phpdbg_watchpoint_t *parent, zend_refcounted *ref) {
	phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));
	watch->flags = parent->flags;
	watch->parent = parent;
	phpdbg_create_addr_watchpoint(&ref->refcount, sizeof(uint32_t), watch);
	watch->type = WATCH_ON_REFCOUNTED;

	return watch;
}

static void phpdbg_add_watch_collision(phpdbg_watchpoint_t *watch) {
	phpdbg_watch_collision *cur;
	if ((cur = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) watch->addr.ref))) {
		cur->num++;
		if (watch->flags == PHPDBG_WATCH_RECURSIVE) {
			cur->refs++;
		}
	} else {
		phpdbg_watch_collision coll;
		coll.num = 1;
		coll.refs = watch->flags == PHPDBG_WATCH_RECURSIVE;
		coll.watch = *watch;
		zend_hash_init(&coll.watches, 8, NULL, NULL, 0);
		cur = zend_hash_index_add_mem(&PHPDBG_G(watch_collisions), (zend_ulong) watch->addr.ref, &coll, sizeof(phpdbg_watch_collision));
		phpdbg_store_watchpoint(&cur->watch);
		phpdbg_activate_watchpoint(&cur->watch);
	}

	zend_hash_str_add_ptr(&cur->watches, watch->parent->str, watch->parent->str_len, watch->parent);
}

static void phpdbg_remove_watch_collision(zend_refcounted *ref) {
	phpdbg_watch_collision *cur;
	if ((cur = zend_hash_index_find_ptr(&PHPDBG_G(watch_collisions), (zend_ulong) ref))) {
		phpdbg_watchpoint_t *watch = cur->watch.parent;

		if (watch->flags == PHPDBG_WATCH_RECURSIVE && !--cur->refs) {
			phpdbg_delete_watchpoints_recursive(watch);
		}

		zend_hash_str_del(&cur->watches, watch->str, watch->str_len);

		if (!--cur->num) {
			phpdbg_deactivate_watchpoint(&cur->watch);
			phpdbg_remove_watchpoint(&cur->watch);

			phpdbg_delete_watchpoint(watch);

			zend_hash_index_del(&PHPDBG_G(watch_collisions), (zend_ulong) ref);
		}
	}
}

static int phpdbg_create_watchpoint(phpdbg_watchpoint_t *watch) {
	watch->flags |= PHPDBG_WATCH_SIMPLE;

	phpdbg_store_watchpoint(watch);
	zend_hash_str_add_ptr(&PHPDBG_G(watchpoints), watch->str, watch->str_len, watch);

	if (watch->parent && watch->parent->type == WATCH_ON_ZVAL && Z_REFCOUNTED_P(watch->parent->addr.zv)) {
		phpdbg_add_watch_collision(phpdbg_create_refcounted_watchpoint(watch, Z_COUNTED_P(watch->parent->addr.zv)));
	}

	if (watch->type == WATCH_ON_ZVAL) {
		if (watch->parent_container) {
			phpdbg_btree_insert(&PHPDBG_G(watch_HashTables), (zend_ulong) watch->parent_container, watch->parent_container->pDestructor);
			watch->parent_container->pDestructor = (dtor_func_t) phpdbg_watch_HashTable_dtor;
		}

		if (Z_ISREF_P(watch->addr.zv)) {
			phpdbg_watchpoint_t *ref = emalloc(sizeof(phpdbg_watchpoint_t));
			ref->flags = watch->flags;
			ref->str = watch->str;
			ref->str_len = watch->str_len;
			ref->parent = watch;
			ref->parent_container = NULL;
			phpdbg_create_zval_watchpoint(Z_REFVAL_P(watch->addr.zv), ref);

			phpdbg_create_watchpoint(ref);
		}
	}

	phpdbg_activate_watchpoint(watch);

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

	phpdbg_create_watchpoint(watch);

	if (Z_TYPE_P(zv) == IS_ARRAY) {
		watch->flags |= PHPDBG_WATCH_ARRAY;
	} else {
		watch->flags |= PHPDBG_WATCH_OBJECT;
	}

	phpdbg_add_watch_collision(phpdbg_create_refcounted_watchpoint(watch, Z_COUNTED_P(zv)));

	return SUCCESS;
}

static int phpdbg_create_recursive_watchpoint(phpdbg_watchpoint_t *watch) {
	HashTable *ht;
	zval *zvp = watch->addr.zv;

	if (watch->type != WATCH_ON_ZVAL) {
		return FAILURE;
	}

	watch->flags |= PHPDBG_WATCH_RECURSIVE;
	phpdbg_create_watchpoint(watch);

	ZVAL_DEREF(zvp);

	if (!(ht = HT_FROM_ZVP(zvp))) {
		return SUCCESS;
	}

	{
		HashPosition position;
		zval *zv;
		zval key;

		ZEND_HASH_FOREACH_VAL(ht, zv) {
			phpdbg_watchpoint_t *new_watch = emalloc(sizeof(phpdbg_watchpoint_t));

			new_watch->flags = PHPDBG_WATCH_RECURSIVE;
			new_watch->parent = watch;
			new_watch->parent_container = ht;

			zend_hash_get_current_key_zval_ex(ht, &key, &position);
			if (Z_TYPE(key) == IS_STRING) {
				new_watch->name_in_parent = estrndup(Z_STRVAL(key), Z_STRLEN(key));
				new_watch->name_in_parent_len = Z_STRLEN(key);
			} else {
				new_watch->name_in_parent_len = spprintf(&new_watch->name_in_parent, 0, "%lld", Z_LVAL(key));
			}

			new_watch->str_len = spprintf(&new_watch->str, 0, "%.*s%s%s%s", (int) watch->str_len, watch->str, Z_TYPE_P(zvp) == IS_ARRAY ? "[" : "->", phpdbg_get_property_key(new_watch->name_in_parent), Z_TYPE_P(zvp) == IS_ARRAY ? "]" : "");

			while (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}

			phpdbg_create_zval_watchpoint(zv, new_watch);
			phpdbg_create_recursive_watchpoint(new_watch);
		} ZEND_HASH_FOREACH_END();
	}

	{
		phpdbg_watchpoint_t *new_watch = emalloc(sizeof(phpdbg_watchpoint_t));

		new_watch->parent = watch;
		new_watch->parent_container = watch->parent_container;
		new_watch->name_in_parent = estrndup(watch->name_in_parent, watch->name_in_parent_len);
		new_watch->name_in_parent_len = watch->name_in_parent_len;
		new_watch->str = NULL;
		new_watch->str_len = spprintf(&new_watch->str, 0, "%.*s[]", (int) watch->str_len, watch->str);
		new_watch->flags = PHPDBG_WATCH_RECURSIVE;

		if (Z_TYPE_P(zvp) == IS_ARRAY) {
			new_watch->flags |= PHPDBG_WATCH_ARRAY;
		} else {
			new_watch->flags |= PHPDBG_WATCH_OBJECT;
		}

		phpdbg_create_ht_watchpoint(ht, new_watch);
		phpdbg_create_watchpoint(new_watch);
	}

	return SUCCESS;
}

static int phpdbg_delete_watchpoint_recursive(phpdbg_watchpoint_t *watch, zend_bool user_request) {
	if (watch->type == WATCH_ON_HASHTABLE) {
		HashTable *ht;
		phpdbg_btree_result *result;

		if (watch->type == WATCH_ON_HASHTABLE && user_request) {
			phpdbg_delete_ht_watchpoints_recursive(watch);
		} else {
			ht = HT_FROM_ZVP(watch->addr.zv);

			if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) ht))) {
				phpdbg_delete_watchpoint_recursive((phpdbg_watchpoint_t *) result->ptr, user_request);
			}
		}
	} else if (watch->type == WATCH_ON_ZVAL) {
		phpdbg_delete_zval_watchpoints_recursive(watch);
	}

	return zend_hash_str_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);
}

static void phpdbg_delete_ht_watchpoints_recursive(phpdbg_watchpoint_t *watch) {
	zend_string *strkey;
	zend_long numkey;
	char *str;
	int str_len;
	phpdbg_watchpoint_t *watchpoint;

	ZEND_HASH_FOREACH_KEY(watch->addr.ht, numkey, strkey) {
		if (strkey) {
			str_len = asprintf(&str, "%.*s%s%s%s", (int) watch->str_len, watch->str, (watch->flags & PHPDBG_WATCH_ARRAY) ? "[" : "->", phpdbg_get_property_key(strkey->val), (watch->flags & PHPDBG_WATCH_ARRAY) ? "]" : "");
		} else {
			str_len = asprintf(&str, "%.*s%s" ZEND_LONG_FMT "%s", (int) watch->str_len, watch->str, (watch->flags & PHPDBG_WATCH_ARRAY) ? "[" : "->", numkey, (watch->flags & PHPDBG_WATCH_ARRAY) ? "]" : "");
		}

		if ((watchpoint = zend_hash_str_find_ptr(&PHPDBG_G(watchpoints), str, str_len))) {
			phpdbg_delete_watchpoint_recursive(watchpoint, 1);
		}
	} ZEND_HASH_FOREACH_END();
}

static void phpdbg_delete_zval_watchpoints_recursive(phpdbg_watchpoint_t *watch) {
	if (Z_REFCOUNTED_P(watch->addr.zv)) {
		phpdbg_remove_watch_collision(Z_COUNTED_P(watch->addr.zv));
	}
}

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

	if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
		ret = phpdbg_delete_watchpoint_recursive(watch, 1);
	} else {
		ret = zend_hash_str_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);
	}

	efree(tmp_watch->str);
	efree(tmp_watch->name_in_parent);
	efree(tmp_watch);

	return ret;
}

static int phpdbg_watchpoint_parse_wrapper(char *name, size_t len, char *keyname, size_t keylen, HashTable *parent, zval *zv, int (*callback)(phpdbg_watchpoint_t *)) {
	int ret;
	phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));
	watch->flags = 0;
	watch->str = name;
	watch->str_len = len;
	watch->name_in_parent = keyname;
	watch->name_in_parent_len = keylen;
	watch->parent_container = parent;
	phpdbg_create_zval_watchpoint(zv, watch);

	ret = callback(watch);

	if (ret != SUCCESS) {
		efree(watch);
		efree(name);
		efree(keyname);
	}

	return ret;
}

PHPDBG_API int phpdbg_watchpoint_parse_input(char *input, size_t len, HashTable *parent, size_t i, int (*callback)(phpdbg_watchpoint_t *), zend_bool silent) {
	return phpdbg_parse_variable_with_arg(input, len, parent, i, (phpdbg_parse_var_with_arg_func) phpdbg_watchpoint_parse_wrapper, 0, callback);
}

static int phpdbg_watchpoint_parse_symtables(char *input, size_t len, int (*callback)(phpdbg_watchpoint_t *)) {
	if (EG(scope) && len >= 5 && !memcmp("$this", input, 5)) {
		zend_hash_str_add(EG(current_execute_data)->symbol_table, ZEND_STRL("this"), &EG(current_execute_data)->This);
	}

	if (phpdbg_is_auto_global(input, len) && phpdbg_watchpoint_parse_input(input, len, &EG(symbol_table), 0, callback, 1) != FAILURE) {
		return SUCCESS;
	}

	return phpdbg_watchpoint_parse_input(input, len, EG(current_execute_data)->symbol_table, 0, callback, 0);
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

	zval_ptr_dtor_wrapper(zv);

	if ((result = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong) zv))) {
		phpdbg_watchpoint_t *watch = result->ptr;

		PHPDBG_G(watchpoint_hit) = 1;

		phpdbg_notice("watchdelete", "variable=\"%.*s\" recursive=\"%s\"", "%.*s was removed, removing watchpoint%s", (int) watch->str_len, watch->str, (watch->flags & PHPDBG_WATCH_RECURSIVE) ? " recursively" : "");

		if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
			phpdbg_delete_watchpoint_recursive(watch, 0);
		} else {
			zend_hash_str_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);
		}
	}
}


int phpdbg_create_var_watchpoint(char *input, size_t len) {
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return FAILURE;
	}

	return phpdbg_watchpoint_parse_symtables(input, len, phpdbg_create_watchpoint);
}

int phpdbg_delete_var_watchpoint(char *input, size_t len) {
	if (phpdbg_rebuild_symtable() == FAILURE) {
		return FAILURE;
	}

	return phpdbg_watchpoint_parse_symtables(input, len, phpdbg_delete_watchpoint);
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

	memcpy(&dump->data, page, size);

	zend_llist_add_element(&PHPDBG_G(watchlist_mem), &dump);

	return SUCCESS;
}

void phpdbg_watchpoints_clean(void) {
	zend_hash_clean(&PHPDBG_G(watchpoints));
}

static void phpdbg_watch_dtor(zval *pDest) {
	phpdbg_watchpoint_t *watch = (phpdbg_watchpoint_t *) Z_PTR_P(pDest);

	phpdbg_deactivate_watchpoint(watch);
	phpdbg_remove_watchpoint(watch);

	efree(watch->str);
	efree(watch->name_in_parent);
}

static void phpdbg_watch_mem_dtor(void *llist_data) {
	phpdbg_watch_memdump *dump = *(phpdbg_watch_memdump **) llist_data;

	/* Disble writing again */
	if (dump->reenable_writing) {
		mprotect(dump->page, dump->size, PROT_READ);
	}

	free(*(void **) llist_data);
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
	zend_hash_init(&PHPDBG_G(watch_collisions), 8, NULL, NULL, 0);
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

		/* Test if the zval was separated and if necessary move the watchpoint */
		if ((watch->type == WATCH_ON_HASHTABLE || watch->type == WATCH_ON_ZVAL) && watch->parent_container) {
			if ((curTest = zend_hash_str_find(watch->parent_container, watch->name_in_parent, watch->name_in_parent_len))) {
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
		if (memcmp(oldPtr, watch->addr.ptr, watch->size) != SUCCESS) {
			zend_bool do_break = 0;

			switch (watch->type) {
				case WATCH_ON_ZVAL:
					do_break = memcmp(oldPtr, watch->addr.zv, sizeof(zend_value) + sizeof(uint32_t) /* value + typeinfo */);
					break;
				case WATCH_ON_HASHTABLE:
					do_break = zend_hash_num_elements((HashTable *) oldPtr) != zend_hash_num_elements(watch->addr.ht);
					break;
				case WATCH_ON_REFCOUNTED:
					if (PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS) {
						do_break = memcmp(oldPtr, watch->addr.ref, sizeof(uint32_t) /* no zend_refcounted metadata info */);
					}
					break;
			}


			if (do_break) {
				PHPDBG_G(watchpoint_hit) = 1;

				phpdbg_notice("watchhit", "variable=\"%s\"", "Breaking on watchpoint %.*s", (int) watch->str_len, watch->str);
				phpdbg_xml("<watchdata %r>");
			}

			switch (watch->type) {
				case WATCH_ON_ZVAL: {
					int show_value = memcmp(oldPtr, watch->addr.zv, sizeof(zval) - sizeof(uint32_t) /* no metadata info */);

					if (removed || show_value) {
						if (removed && (Z_TYPE_P((zval *) oldPtr) == IS_ARRAY || Z_TYPE_P((zval *) oldPtr) == IS_OBJECT)) {
							phpdbg_writeln("watchvalue", "type=\"old\" inaccessible=\"inaccessible\"", "Old value inaccessible, array or object (HashTable) already destroyed");
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
						phpdbg_notice("watchdelete", "variable=\"%.*s\"", "Watchpoint %.*s was unset, removing watchpoint", (int) watch->str_len, watch->str);
						zend_hash_str_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);

						reenable = 0;

						if (Z_REFCOUNTED_P((zval *) oldPtr)) {
							phpdbg_remove_watch_collision(Z_COUNTED_P((zval *) oldPtr));
						}
						break;
					}

					if (show_value) {
						phpdbg_out("New value: ");
						phpdbg_xml("<watchvalue %r type=\"new\">");
						zend_print_flat_zval_r(watch->addr.zv);
						phpdbg_xml("</watchvalue>");
						phpdbg_out("\n");
					}

					/* add new watchpoints if necessary */
					if (Z_PTR_P(watch->addr.zv) != Z_PTR_P((zval *) oldPtr)) {
						if (Z_REFCOUNTED_P((zval *) oldPtr)) {
							phpdbg_remove_watch_collision(Z_COUNTED_P((zval *) oldPtr));
						}
						if (Z_REFCOUNTED_P(watch->addr.zv)) {
							if (PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS) {
								phpdbg_writeln("watchrefcount", "type=\"new\" refcount=\"%d\"", "New refcount: %d", Z_COUNTED_P(watch->addr.zv)->refcount);
							}
							if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
								phpdbg_create_recursive_watchpoint(watch);
							}
						}
					}

					break;
				}
				case WATCH_ON_HASHTABLE:
#if 0 && ZEND_DEBUG
					if (watch->addr.arr->ht->inconsistent) {
						phpdbg_notice("watchdelete", "variable=\"%.*s\"", "Watchpoint %.*s was unset, removing watchpoint", (int) watch->str_len, watch->str);
						zend_hash_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);

						reenable = 0;

						break;
					}
#endif
					elementDiff = zend_hash_num_elements((HashTable *) oldPtr) - zend_hash_num_elements(watch->addr.ht);
					if (elementDiff) {
						if (elementDiff > 0) {
							phpdbg_writeln("watchsize", "removed=\"%d\"", "%d elements were removed from the array", elementDiff);
						} else {
							phpdbg_writeln("watchsize", "added=\"%d\"", "%d elements were added to the array", -elementDiff);

							/* add new watchpoints if necessary */
							if (watch->flags & PHPDBG_WATCH_RECURSIVE) {
								phpdbg_create_recursive_watchpoint(watch);
							}
						}
					}
					if (watch->addr.ht->nInternalPointer != ((HashTable *) oldPtr)->nInternalPointer) {
						phpdbg_writeln("watcharrayptr", "", "Internal pointer of array was changed");
					}
					break;
				case WATCH_ON_REFCOUNTED: {
					if (PHPDBG_G(flags) & PHPDBG_SHOW_REFCOUNTS) {
						phpdbg_writeln("watchrefcount", "type=\"old\" refcount=\"%d\"", "Old refcount: %d", ((zend_refcounted *) oldPtr)->refcount);

						if (!removed) {
							phpdbg_writeln("watchrefcount", "type=\"old\" refcount=\"%d\"", "Old refcount: %d", ((zend_refcounted *) oldPtr)->refcount);
						}
					}

					break;
				}
			}

			if (do_break) {
				phpdbg_xml("</watchdata>");
			}
		}

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
		phpdbg_writeln("watchvariable", "variable=\"%.*s\" on=\"%s\" type=\"%s\"", "%.*s (%s, %s)", (int) watch->str_len, watch->str, watch->type == WATCH_ON_HASHTABLE ? "array" : watch->type == WATCH_ON_REFCOUNTED ? "refcount" : "variable", watch->flags == PHPDBG_WATCH_RECURSIVE ? "recursive" : "simple");
	} ZEND_HASH_FOREACH_END();

	phpdbg_xml("</watchlist>");
}

void phpdbg_watch_efree(void *ptr) {
	phpdbg_btree_result *result;

	result = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), (zend_ulong) ptr);

	if (result) {
		phpdbg_watchpoint_t *watch = result->ptr;

		if ((size_t) watch->addr.ptr + watch->size > (size_t) ptr) {
			if (watch->type == WATCH_ON_ZVAL) {
				phpdbg_remove_watch_collision(Z_COUNTED_P(watch->addr.zv));
			}
			zend_hash_str_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);
		}
	}

	PHPDBG_G(original_free_function)(ptr);
}
