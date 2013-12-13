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
#include "phpdbg_watch.h"
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

void phpdbg_watch_mem_dtor(void *llist_data) {
	efree(llist_data);
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

	zend_llist_init(&PHPDBG_G(watchlist_mem), 0, phpdbg_watch_mem_dtor, 0);
}

void *phpdbg_get_page_boundary(void *addr) {
	return (void *)((unsigned long)addr & ~(phpdbg_pagesize - 1));
}

size_t phpdbg_get_total_page_size(size_t size) {
	return ((size - 1)  & ~(phpdbg_pagesize - 1)) | phpdbg_pagesize;
}

phpdbg_watchpoint_t *phpdbg_check_for_watchpoint(void *addr) {
	phpdbg_watchpoint_t *watch;

	/* find nearest watchpoint */

	/* check if that watchpoint includes addr */
	if (((char *)watch->addr.ptr) + watch->size > (char *)addr) {
		/* failure */
		return NULL;
	}

	return watch;
}

int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC) {
	void *addr;
	void *page;
	phpdbg_watch_memdump *dump;
	phpdbg_watchpoint_t *watch;
	size_t size;

	addr = info->si_addr;

	watch = phpdbg_check_for_watchpoint(addr);

	if (watch == NULL) {
		return FAILURE;
	}

	page = phpdbg_get_page_boundary(addr);
	size = phpdbg_get_total_page_size(watch->size);

	/* re-enable writing */
	mprotect(page, size, PROT_NONE | PROT_READ | PROT_WRITE);

	dump = emalloc(MEMDUMP_SIZE(size));

	memcpy(&dump->data, page, size);

	zend_llist_add_element(&PHPDBG_G(watchlist_mem), dump);

	return SUCCESS;
}

int phpdbg_print_changed_zval(void *llist_data) {
	phpdbg_watch_memdump *dump = llist_data;
	phpdbg_watchpoint_t *watch;
	phpdbg_btree *tree = PHPDBG_G(watchpoint_tree);
	void *oldPtr;

	TSRMLS_FETCH();

	/* fetch all changes between dump->page and dump->page + dump->size */
	watch = tree->watchpoint;
	oldPtr = (char *)dump->data + ((size_t)watch->addr.ptr - (size_t)dump->page);
	if (memcmp(oldPtr, watch->addr.ptr, watch->size) == SUCCESS) {
		phpdbg_notice("Breaking on watchpoint %s", watch->str);
		switch (watch->type) {
			case WATCH_ON_ZVAL:
				phpdbg_write("Old value: ");
				zend_print_flat_zval_r((zval *)oldPtr TSRMLS_CC);
				phpdbg_write("New value: ");
				zend_print_flat_zval_r(watch->addr.zv TSRMLS_CC);
				break;
		}
	}
	

	return 1;
}

int phpdbg_print_changed_zvals(TSRMLS_D) {
	if (zend_llist_count(&PHPDBG_G(watchlist_mem)) == 0) {
		return FAILURE;
	}

	zend_llist_apply_with_del(&PHPDBG_G(watchlist_mem), phpdbg_print_changed_zval);

	return SUCCESS;
}

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch) {
	int m;

	watch->addr.ptr = addr;
	watch->size = size;
	watch->type = WATCH_ON_PTR;

	/* pagesize is assumed to be in the range of 2^x */
	m = mprotect(phpdbg_get_page_boundary(addr), phpdbg_get_total_page_size(size), PROT_NONE | PROT_READ);

	if (m == FAILURE) {
		phpdbg_error("Unable to set watchpoint (mprotect() failed)");
		zend_bailout();
	}
}

void phpdbg_create_zval_watchpoint(zval *zv, phpdbg_watchpoint_t *watch) {
	phpdbg_create_addr_watchpoint(zv, sizeof(zval), watch);
	watch->type = WATCH_ON_ZVAL;
}

int phpdbg_create_var_watchpoint(char *name, size_t len TSRMLS_DC) {
	zval *zv;
	phpdbg_watchpoint_t watch;


	if (!EG(active_op_array)) {
		phpdbg_error("No active op array!");
		return SUCCESS;
	}

	if (!EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);

		if (!EG(active_symbol_table)) {
			phpdbg_error("No active symbol table!");
			return SUCCESS;
		}
	}

	/* Lookup current symbol table */
	if (zend_hash_find(EG(current_execute_data)->symbol_table, name, len + 1, (void **)&zv) == SUCCESS) {
		phpdbg_create_zval_watchpoint(zv, &watch);
		zend_hash_add(&PHPDBG_G(watchpoints), name, len, &watch, sizeof(phpdbg_watchpoint_t), NULL);
		return SUCCESS;
	}

	return FAILURE;
}
