/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
#include <unistd.h>
#include <sys/mman.h>

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

long phpdbg_pagesize;

typedef struct {
	void *page;
	size_t size;
	/* data must be last element */
	void *data;
} phpdbg_watch_memdump;

#define MEMDUMP_SIZE(size) (sizeof(phpdbg_watch_memdump) - sizeof(void *) + (size))

static zend_always_inline void *phpdbg_get_page_boundary(void *addr) {
	return (void *)((size_t)addr & ~(phpdbg_pagesize - 1));
}

static zend_always_inline size_t phpdbg_get_total_page_size(void *addr, size_t size) {
	return (size_t)phpdbg_get_page_boundary(addr + size - 1) - (size_t)phpdbg_get_page_boundary(addr) + phpdbg_pagesize;
}

static phpdbg_watchpoint_t *phpdbg_check_for_watchpoint(void *addr TSRMLS_DC) {
	phpdbg_watchpoint_t *watch;
	phpdbg_btree_result *result = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), (zend_ulong)phpdbg_get_page_boundary(addr) + phpdbg_pagesize - 1);

	if (result == NULL) {
		return NULL;
	}

	watch = result->ptr;

	/* check if that addr is in a mprotect()'ed memory area */
	if ((char *)phpdbg_get_page_boundary(watch->addr.ptr) > (char *)addr || (char *)phpdbg_get_page_boundary(watch->addr.ptr) + phpdbg_get_total_page_size(watch->addr.ptr, watch->size) < (char *)addr) {
		/* failure */
		return NULL;
	}

	return watch;
}

static void phpdbg_store_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	phpdbg_btree_insert(&PHPDBG_G(watchpoint_tree), (zend_ulong)watch->addr.ptr, watch);
}

static void phpdbg_change_watchpoint_access(phpdbg_watchpoint_t *watch, int access TSRMLS_DC) {
	int m;

	/* pagesize is assumed to be in the range of 2^x */
	m = mprotect(phpdbg_get_page_boundary(watch->addr.ptr), phpdbg_get_total_page_size(watch->addr.ptr, watch->size), access);

	if (m == FAILURE) {
		phpdbg_error("Unable to (un)set watchpoint (mprotect() failed)");
		zend_bailout();
	}
}

static inline void phpdbg_activate_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	phpdbg_change_watchpoint_access(watch, PROT_READ TSRMLS_CC);
}

static inline void phpdbg_deactivate_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	phpdbg_change_watchpoint_access(watch, PROT_READ | PROT_WRITE TSRMLS_CC);
}

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch) {
	watch->addr.ptr = addr;
	watch->size = size;
	watch->type = WATCH_ON_PTR;
}

void phpdbg_create_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch) {
	phpdbg_create_addr_watchpoint(zv, sizeof(zval), watch);
	watch->type = WATCH_ON_ZVAL;
}

void phpdbg_create_ht_watchpoint(HashTable *ht, phpdbg_watchpoint_t *watch) {
	phpdbg_create_addr_watchpoint(ht, sizeof(HashTable), watch);
	watch->type = WATCH_ON_HASHTABLE;
}

static int phpdbg_create_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	phpdbg_store_watchpoint(watch TSRMLS_CC);
	zend_hash_add(&PHPDBG_G(watchpoints), watch->str, watch->str_len, &watch, sizeof(phpdbg_watchpoint_t *), NULL);

	phpdbg_activate_watchpoint(watch TSRMLS_CC);

	return SUCCESS;
}

static int phpdbg_create_array_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	HashTable *ht;

	switch (Z_TYPE_P(watch->addr.zv)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(watch->addr.zv);
			break;
		case IS_OBJECT:
			ht = Z_OBJPROP_P(watch->addr.zv);
			break;
		default:
			return FAILURE;
	}

	phpdbg_create_ht_watchpoint(Z_ARRVAL_P(watch->addr.zv), watch);

	phpdbg_create_watchpoint(watch TSRMLS_CC);

	return SUCCESS;
}

static int phpdbg_create_recursive_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	HashTable *ht;

	if (watch->type != WATCH_ON_ZVAL) {
		return FAILURE;
	}

	phpdbg_create_watchpoint(watch TSRMLS_CC);

	switch (Z_TYPE_P(watch->addr.zv)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(watch->addr.zv);
			break;
		case IS_OBJECT:
			ht = Z_OBJPROP_P(watch->addr.zv);
			break;
		default:
			return SUCCESS;
	}

	{
		HashPosition position;
		zval **zv;
		zval key;

		for (zend_hash_internal_pointer_reset_ex(ht, &position);
		     zend_hash_get_current_data_ex(ht, (void **)&zv, &position) == SUCCESS;
		     zend_hash_move_forward_ex(ht, &position)) {
			phpdbg_watchpoint_t *new_watch = emalloc(sizeof(phpdbg_watchpoint_t));

			new_watch->parent = watch;
			new_watch->parent_container = ht;

			zend_hash_get_current_key_zval_ex(ht, &key, &position);
			if (Z_TYPE(key) == IS_STRING) {
				new_watch->name_in_parent = Z_STRVAL(key);
				new_watch->name_in_parent_len = Z_STRLEN(key);
			} else {
				new_watch->name_in_parent = NULL;
				new_watch->name_in_parent_len = asprintf(&new_watch->name_in_parent, "%ld", Z_LVAL(key));
			}

			new_watch->str = NULL;
			new_watch->str_len = asprintf(&new_watch->str, "%.*s%s%.*s%s", watch->str_len, watch->str, Z_TYPE_P(watch->addr.zv) == IS_ARRAY?"[":"->", new_watch->name_in_parent_len, new_watch->name_in_parent, Z_TYPE_P(watch->addr.zv) == IS_ARRAY?"]":"");

			phpdbg_create_zval_watchpoint(*zv, new_watch);
			phpdbg_create_recursive_watchpoint(new_watch TSRMLS_CC);
		}
	}

	{
		phpdbg_watchpoint_t *new_watch = emalloc(sizeof(phpdbg_watchpoint_t));

		new_watch->parent = watch;
		new_watch->parent_container = ht;
		new_watch->name_in_parent = watch->name_in_parent;
		new_watch->name_in_parent_len = watch->name_in_parent_len;
		new_watch->str = watch->str;
		new_watch->str_len = watch->str_len;

		phpdbg_create_ht_watchpoint(ht, new_watch);
		phpdbg_create_watchpoint(new_watch TSRMLS_CC);
	}

	return SUCCESS;
}

static int phpdbg_delete_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	int ret = zend_hash_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);

	efree(watch);

	return ret;
}

static int phpdbg_watchpoint_parse_input(char *input, size_t len, HashTable *parent, int i, int (*callback)(phpdbg_watchpoint_t * TSRMLS_DC) TSRMLS_DC) {
	int ret = FAILURE;
	zend_bool new_index = 1;
	char *last_index;
	int index_len = 0;
	zval **zv;

	if (len < 2 || *input != '$') {
		goto error;
	}

	while (i++ < len) {
		if (i == len) {
			new_index = 1;
		} else {
			switch (input[i]) {
				case '[':
					new_index = 1;
					break;
				case ']':
					break;
				case '>':
					if (last_index[index_len - 1] == '-') {
						new_index = 1;
						index_len--;
					}
					break;

				default:
					if (new_index) {
						last_index = input + i;
						new_index = 0;
					}
					if (input[i - 1] == ']') {
						goto error;
					}
					index_len++;
			}
		}

		if (new_index && index_len == 0) {
			HashPosition position;
			for (zend_hash_internal_pointer_reset_ex(parent, &position);
			     zend_hash_get_current_data_ex(parent, (void **)&zv, &position) == SUCCESS;
			     zend_hash_move_forward_ex(parent, &position)) {
				if (i == len || (i == len - 1 && input[len - 1] == ']')) {
					zval *key = emalloc(sizeof(zval));
					phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));
					zend_hash_get_current_key_zval_ex(parent, key, &position);
					convert_to_string(key);
					watch->str = emalloc(i + Z_STRLEN_P(key) + 2);
					watch->str_len = sprintf(watch->str, "%.*s%.*s%s", i, input, Z_STRLEN_P(key), Z_STRVAL_P(key), input[len - 1] == ']'?"]":"");
					efree(key);
					watch->name_in_parent = estrndup(last_index, index_len);
					watch->name_in_parent_len = index_len;
					watch->parent_container = parent;
					phpdbg_create_zval_watchpoint(*zv, watch);

					ret = callback(watch TSRMLS_CC) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
				} else if (Z_TYPE_PP(zv) == IS_OBJECT) {
					phpdbg_watchpoint_parse_input(input, len, Z_OBJPROP_PP(zv), i, callback TSRMLS_CC);
				} else if (Z_TYPE_PP(zv) == IS_ARRAY) {
					phpdbg_watchpoint_parse_input(input, len, Z_ARRVAL_PP(zv), i, callback TSRMLS_CC);
				} else {
					/* Ignore silently */
				}
			}
			return ret;
		} else if (new_index) {
			char last_chr = last_index[index_len];
			last_index[index_len] = 0;
			if (zend_symtable_find(parent, last_index, index_len + 1, (void **)&zv) == FAILURE) {
				phpdbg_error("%.*s is undefined", i, input);
				return FAILURE;
			}
			last_index[index_len] = last_chr;
			if (i == len) {
				phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));
				watch->str = estrndup(input, len);
				watch->str_len = len;
				watch->name_in_parent = estrndup(last_index, index_len);
				watch->name_in_parent_len = index_len;
				watch->parent_container = parent;
				phpdbg_create_zval_watchpoint(*zv, watch);

				ret = callback(watch TSRMLS_CC) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
			} else if (Z_TYPE_PP(zv) == IS_OBJECT) {
				parent = Z_OBJPROP_PP(zv);
			} else if (Z_TYPE_PP(zv) == IS_ARRAY) {
				parent = Z_ARRVAL_PP(zv);
			} else {
				phpdbg_error("%.*s is nor an array nor an object", i, input);
				return FAILURE;
			}
			index_len = 0;
		}
	}

	return ret;
	error:
		phpdbg_error("Malformed input");
		return FAILURE;
}

PHPDBG_WATCH(delete) /* {{{ */
{
	switch (param->type) {
		case STR_PARAM:
			if (phpdbg_delete_var_watchpoint(param->str, param->len TSRMLS_CC) == FAILURE) {
				phpdbg_error("Nothing was deleted, no corresponding watchpoint found");
			}
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_WATCH(recursive) /* {{{ */
{
	if (phpdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			phpdbg_watchpoint_parse_input(param->str, param->len, EG(active_symbol_table), 0, phpdbg_create_recursive_watchpoint TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

PHPDBG_WATCH(array) /* {{{ */
{
	if (phpdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return SUCCESS;
	}

	switch (param->type) {
		case STR_PARAM:
			phpdbg_watchpoint_parse_input(param->str, param->len, EG(active_symbol_table), 0, phpdbg_create_array_watchpoint TSRMLS_CC);
			break;

		phpdbg_default_switch_case();
	}

	return SUCCESS;
} /* }}} */

int phpdbg_create_var_watchpoint(char *input, size_t len TSRMLS_DC) {
	if (phpdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return SUCCESS;
	}

	return phpdbg_watchpoint_parse_input(input, len, EG(active_symbol_table), 0, phpdbg_create_watchpoint TSRMLS_CC);
}

int phpdbg_delete_var_watchpoint(char *input, size_t len TSRMLS_DC) {
	if (phpdbg_rebuild_symtable(TSRMLS_C) == FAILURE) {
		return SUCCESS;
	}

	return phpdbg_watchpoint_parse_input(input, len, EG(active_symbol_table), 0, phpdbg_delete_watchpoint TSRMLS_CC);
}

int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC) {
	void *page;
	phpdbg_watch_memdump *dump;
	phpdbg_watchpoint_t *watch;
	size_t size;

	watch = phpdbg_check_for_watchpoint(info->si_addr TSRMLS_CC);

	if (watch == NULL) {
		return FAILURE;
	}

	page = phpdbg_get_page_boundary(watch->addr.ptr);
	size = phpdbg_get_total_page_size(watch->addr.ptr, watch->size);

	/* re-enable writing */
	mprotect(page, size, PROT_NONE | PROT_READ | PROT_WRITE);

	dump = malloc(MEMDUMP_SIZE(size));
	dump->page = page;
	dump->size = size;

	memcpy(&dump->data, page, size);

	zend_llist_add_element(&PHPDBG_G(watchlist_mem), &dump);

	return SUCCESS;
}

void phpdbg_watchpoints_clean(TSRMLS_D) {
	zend_hash_clean(&PHPDBG_G(watchpoints));
}

static void phpdbg_watch_dtor(void *pDest) {
	TSRMLS_FETCH();

	phpdbg_watchpoint_t *watch = *(phpdbg_watchpoint_t **)pDest;

	phpdbg_deactivate_watchpoint(watch TSRMLS_CC);
	phpdbg_btree_delete(&PHPDBG_G(watchpoint_tree), (zend_ulong)watch->addr.ptr);

	efree(watch->str);
	efree(watch->name_in_parent);

	efree(watch);
}

static void phpdbg_watch_mem_dtor(void *llist_data) {
	void *page = (*(phpdbg_watch_memdump **)llist_data)->page;
	size_t size = (*(phpdbg_watch_memdump **)llist_data)->size;

	free(*(void **)llist_data);

	/* Disble writing again */
	mprotect(page, size, PROT_NONE | PROT_READ);
}

void phpdbg_setup_watchpoints(TSRMLS_D) {
#ifdef _SC_PAGE_SIZE
	phpdbg_pagesize = sysconf(_SC_PAGE_SIZE);
#endif
#ifdef _SC_PAGESIZE
	phpdbg_pagesize = sysconf(_SC_PAGESIZE);
#endif
#ifdef _SC_NUTC_OS_PAGESIZE
	phpdbg_pagesize = sysconf(_SC_NUTC_OS_PAGESIZE);
#endif

	zend_llist_init(&PHPDBG_G(watchlist_mem), sizeof(void *), phpdbg_watch_mem_dtor, 1);
	phpdbg_btree_init(&PHPDBG_G(watchpoint_tree), sizeof(void *) * 8);
	_zend_hash_init(&PHPDBG_G(watchpoints), 8, phpdbg_watch_dtor, 0 ZEND_FILE_LINE_CC);
}

static void phpdbg_print_changed_zval(phpdbg_watch_memdump *dump TSRMLS_DC) {
	/* fetch all changes between dump->page and dump->page + dump->size */
	phpdbg_btree_position pos = phpdbg_btree_find_between(&PHPDBG_G(watchpoint_tree), (zend_ulong)dump->page, (zend_ulong)dump->page + dump->size);
	phpdbg_btree_result *result, *htresult;
	int elementDiff;
	void *curTest;

	while ((result = phpdbg_btree_next(&pos))) {
		phpdbg_watchpoint_t *watch = result->ptr, *htwatch;
		void *oldPtr = (char *)&dump->data + ((size_t)watch->addr.ptr - (size_t)dump->page);

		/* Test if the zval was separated and if necessary move the watchpoint */
		if (zend_hash_find(watch->parent_container, watch->name_in_parent, watch->name_in_parent_len, &curTest) == SUCCESS) {
			if (curTest != watch->addr.ptr) {
				phpdbg_deactivate_watchpoint(watch);
				watch->addr.ptr = curTest;
				phpdbg_activate_watchpoint(watch);
			}
		}

		/* Show to the user what changed and delete watchpoint upon removal */
		if (memcmp(oldPtr, watch->addr.ptr, watch->size) != SUCCESS) {
			PHPDBG_G(watchpoint_hit) = 1;

			phpdbg_notice("Breaking on watchpoint %s", watch->str);
			switch (watch->type) {
				case WATCH_ON_ZVAL:
					
					phpdbg_write("Old value: ");
					zend_print_flat_zval_r((zval *)oldPtr TSRMLS_CC);
					phpdbg_writeln("\nOld refcount: %d; Old is_ref: %d", ((zval *)oldPtr)->refcount__gc, ((zval *)oldPtr)->is_ref__gc);

					/* check if zval was removed */
					if (((zval *)oldPtr)->refcount__gc != watch->addr.zv->refcount__gc && !zend_symtable_exists(watch->parent_container, watch->name_in_parent, watch->name_in_parent_len + 1)) {
						phpdbg_notice("Watchpoint %s was unset, removing watchpoint", watch->str);
						zend_hash_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);

						if (Z_TYPE_P((zval *)oldPtr) == IS_ARRAY) {
							goto remove_ht_watch;
						}

						break;
					}

					phpdbg_write("New value: ");
					zend_print_flat_zval_r(watch->addr.zv TSRMLS_CC);
					phpdbg_writeln("\nNew refcount: %d; New is_ref: %d", watch->addr.zv->refcount__gc, watch->addr.zv->is_ref__gc);

					if (Z_TYPE_P((zval *)oldPtr) != IS_ARRAY || Z_ARRVAL_P(watch->addr.zv) == Z_ARRVAL_P((zval *)oldPtr)) {
						break;
					}

remove_ht_watch:
					if ((htresult = phpdbg_btree_find(&PHPDBG_G(watchpoint_tree), (zend_ulong)Z_ARRVAL_P((zval *)oldPtr)))) {
						htwatch = htresult->ptr;
						zend_hash_del(&PHPDBG_G(watchpoints), htwatch->str, htwatch->str_len);
					}
					
					break;

				case WATCH_ON_HASHTABLE:

					elementDiff = zend_hash_num_elements((HashTable *)oldPtr) - zend_hash_num_elements(watch->addr.ht);
					if (elementDiff) {
						if (elementDiff < 0) {
							phpdbg_writeln("%d elements were removed from the array", -elementDiff);
						} else {
							phpdbg_writeln("%d elements were added to the array", elementDiff);
							/* TODO: add, if recursive watchpoint, the new elements to the elements to watch */
						}
					}
					if (((HashTable *)oldPtr)->pInternalPointer != watch->addr.ht->pInternalPointer) {
						phpdbg_writeln("Internal pointer of array was changed");
					}
					break;
			}
		}
	}
}

int phpdbg_print_changed_zvals(TSRMLS_D) {
	zend_llist_position pos;
	phpdbg_watch_memdump **dump;

	if (zend_llist_count(&PHPDBG_G(watchlist_mem)) == 0) {
		return FAILURE;
	}

	PHPDBG_G(watchpoint_hit) = 0;

	dump = (phpdbg_watch_memdump **)zend_llist_get_last_ex(&PHPDBG_G(watchlist_mem), &pos);

	do {
		phpdbg_print_changed_zval(*dump TSRMLS_CC);
	} while ((dump = (phpdbg_watch_memdump **)zend_llist_get_prev_ex(&PHPDBG_G(watchlist_mem), &pos)));

	zend_llist_clean(&PHPDBG_G(watchlist_mem));

	return PHPDBG_G(watchpoint_hit)?SUCCESS:FAILURE;
}

void phpdbg_list_watchpoints(TSRMLS_D) {
	HashPosition position;
	phpdbg_watchpoint_t **watch;

	for (zend_hash_internal_pointer_reset_ex(&PHPDBG_G(watchpoints), &position);
	     zend_hash_get_current_data_ex(&PHPDBG_G(watchpoints), (void**) &watch, &position) == SUCCESS;
	     zend_hash_move_forward_ex(&PHPDBG_G(watchpoints), &position)) {
		phpdbg_writeln("%.*s", (int)(*watch)->str_len, (*watch)->str);
	}
}

void phpdbg_watch_efree(void *ptr) {
	TSRMLS_FETCH();
	phpdbg_btree_result *result = phpdbg_btree_find_closest(&PHPDBG_G(watchpoint_tree), (zend_ulong)ptr);

	if (result) {
		phpdbg_watchpoint_t *watch = result->ptr;

		if (watch->addr.ptr + watch->size > ptr) {
			zend_hash_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);
		}
	}

	PHPDBG_G(original_free_function)(ptr);
}
