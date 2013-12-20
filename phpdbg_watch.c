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

static inline void *phpdbg_get_page_boundary(void *addr) {
	return (void *)((size_t)addr & ~(phpdbg_pagesize - 1));
}

static inline size_t phpdbg_get_total_page_size(void *addr, size_t size) {
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

static void phpdbg_change_watchpoint_access(phpdbg_watchpoint_t *watch, int access) {
	int m;

	/* pagesize is assumed to be in the range of 2^x */
	m = mprotect(phpdbg_get_page_boundary(watch->addr.ptr), phpdbg_get_total_page_size(watch->addr.ptr, watch->size), access);

	if (m == FAILURE) {
		phpdbg_error("Unable to (un)set watchpoint (mprotect() failed)");
		zend_bailout();
	}
}

static inline void phpdbg_activate_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_change_watchpoint_access(watch, PROT_READ);
}

static inline void phpdbg_deactivate_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_change_watchpoint_access(watch, PROT_READ | PROT_WRITE);
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

static int phpdbg_create_watchpoint(phpdbg_watchpoint_t *watch) {
	phpdbg_store_watchpoint(watch TSRMLS_CC);
	zend_hash_add(&PHPDBG_G(watchpoints), watch->str, watch->str_len, &watch, sizeof(phpdbg_watchpoint_t *), NULL);

	phpdbg_activate_watchpoint(watch);

	return SUCCESS;
}

static int phpdbg_delete_watchpoint(phpdbg_watchpoint_t *watch) {
	int ret = zend_hash_del(&PHPDBG_G(watchpoints), watch->str, watch->str_len);

	efree(watch);

	return ret;
}

static int phpdbg_watchpoint_parse_input(char *input, size_t len, HashTable *parent, int i, int (*callback)(phpdbg_watchpoint_t *) TSRMLS_DC) {
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
					phpdbg_create_zval_watchpoint(*zv, watch TSRMLS_CC);

					ret = callback(watch) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
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
				phpdbg_create_zval_watchpoint(*zv, watch TSRMLS_CC);

				ret = callback(watch) == SUCCESS || ret == SUCCESS?SUCCESS:FAILURE;
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
	void *addr;
	void *page;
	phpdbg_watch_memdump *dump;
	phpdbg_watchpoint_t *watch;
	size_t size;

	addr = info->si_addr;

	watch = phpdbg_check_for_watchpoint(addr TSRMLS_CC);

	if (watch == NULL) {
		return FAILURE;
	}

	page = phpdbg_get_page_boundary(addr);
	size = phpdbg_get_total_page_size(addr, watch->size);

	/* re-enable writing */
	mprotect(page, size, PROT_NONE | PROT_READ | PROT_WRITE);

	dump = emalloc(MEMDUMP_SIZE(size));
	dump->page = page;
	dump->size = size;

	memcpy(&dump->data, page, size);

	zend_llist_add_element(&PHPDBG_G(watchlist_mem), &dump);

	return SUCCESS;
}

void phpdbg_watchpoints_clean(TSRMLS_DC) {
	zend_hash_clean(&PHPDBG_G(watchpoints));
}

static void phpdbg_watch_dtor(void *pDest) {
	TSRMLS_FETCH();

	phpdbg_watchpoint_t *watch = *(phpdbg_watchpoint_t **)pDest;

	phpdbg_deactivate_watchpoint(watch);
	phpdbg_btree_delete(&PHPDBG_G(watchpoint_tree), (zend_ulong)watch->addr.ptr);

	efree(watch->str);
	efree(watch->name_in_parent);

	efree(watch);
}

static void phpdbg_watch_mem_dtor(void *llist_data) {
	void *page = (*(phpdbg_watch_memdump **)llist_data)->page;
	size_t size = (*(phpdbg_watch_memdump **)llist_data)->size;

	efree(*(void **)llist_data);

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

	zend_llist_init(&PHPDBG_G(watchlist_mem), sizeof(void *), phpdbg_watch_mem_dtor, 0);
	phpdbg_btree_init(&PHPDBG_G(watchpoint_tree), sizeof(void *) * 8);
	_zend_hash_init(&PHPDBG_G(watchpoints), 8, phpdbg_watch_dtor, 0 ZEND_FILE_LINE_CC);
}

static int phpdbg_print_changed_zval(void *llist_data) {
	TSRMLS_FETCH();

	phpdbg_watch_memdump *dump = *(phpdbg_watch_memdump **)llist_data;
	/* fetch all changes between dump->page and dump->page + dump->size */
	phpdbg_btree_position pos = phpdbg_btree_find_between(&PHPDBG_G(watchpoint_tree), (zend_ulong)dump->page, (zend_ulong)dump->page + dump->size);
	phpdbg_btree_result *result;


	while ((result = phpdbg_btree_next(&pos))) {
		phpdbg_watchpoint_t *watch = result->ptr;
		void *oldPtr = (char *)&dump->data + ((size_t)watch->addr.ptr - (size_t)dump->page);
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
						break;
					}

					phpdbg_write("New value: ");
					zend_print_flat_zval_r(watch->addr.zv TSRMLS_CC);
					phpdbg_writeln("\nNew refcount: %d; New is_ref: %d", watch->addr.zv->refcount__gc, watch->addr.zv->is_ref__gc);
					break;
			}
		}
	}

	return 1;
}

int phpdbg_print_changed_zvals(TSRMLS_D) {
	if (zend_llist_count(&PHPDBG_G(watchlist_mem)) == 0) {
		return FAILURE;
	}

	PHPDBG_G(watchpoint_hit) = 0;
	zend_llist_apply_with_del(&PHPDBG_G(watchlist_mem), phpdbg_print_changed_zval);

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
