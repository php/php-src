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

#define CHOOSE_BRANCH(n) \
	addr = (addr << 1) + !!(n); \
	branch = branch->branches[!!(n)];

void phpdbg_watch_mem_dtor(void *llist_data) {
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
}

void *phpdbg_get_page_boundary(void *addr) {
	return (void *)((size_t)addr & ~(phpdbg_pagesize - 1));
}

size_t phpdbg_get_total_page_size(void *addr, size_t size) {
	return (size_t)phpdbg_get_page_boundary(addr + size - 1) - (size_t)phpdbg_get_page_boundary(addr) + phpdbg_pagesize;
}

phpdbg_watchpoint_t *phpdbg_check_for_watchpoint(void *watch_addr) {
	phpdbg_watchpoint_t *watch;
	phpdbg_btree *branch = PHPDBG_G(watchpoint_tree);
	int i = sizeof(void *) * 8 - 1, last_superior_i = -1;
	size_t addr = 0;
	size_t opline = (size_t)phpdbg_get_page_boundary(watch_addr) + phpdbg_pagesize - 1;

	/* find nearest watchpoint */
	do {
		/* an impossible branch was found if: */
		if ((opline >> i) % 2 == 0 && !branch->branches[0]) {
			/* there's no lower branch than opline */
			if (last_superior_i == -1) {
				/* failure */
				return NULL;
			}
			/* reset state */
			branch = PHPDBG_G(watchpoint_tree);
			addr = 0;
			i = sizeof(void *) * 8 - 1;
			/* follow branch according to bits in opline until the last lower branch before the impossible branch */
			do {
				CHOOSE_BRANCH((opline >> i) % 2 == 1 && branch->branches[1]);
			} while (--i > last_superior_i);
			/* use now the lower branch of which we can be sure that it contains only branches lower than opline */
			CHOOSE_BRANCH(0);
			/* and choose the highest possible branch in the branch containing only branches lower than opline */
			while (i--) {
				CHOOSE_BRANCH(branch->branches[1]);
			}
			break;
		}
		/* follow branch according to bits in opline until having found an impossible branch */
		if ((opline >> i) % 2 == 1 && branch->branches[1]) {
			if (branch->branches[0]) {
				last_superior_i = i;
			}
			CHOOSE_BRANCH(1);
		} else {
			CHOOSE_BRANCH(0);
		}
	} while (i--);

	watch = branch->watchpoint;

	/* check if that addr is in a mprotect()'ed memory area */
	if ((char *)phpdbg_get_page_boundary(watch->addr.ptr) + phpdbg_get_total_page_size(watch->addr.ptr, watch->size) < (char *)addr) {
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

int phpdbg_print_changed_zval(void *llist_data) {
	phpdbg_watch_memdump *dump = *(phpdbg_watch_memdump **)llist_data;
	void *oldPtr;
	size_t opline;

	TSRMLS_FETCH();

	/* fetch all changes between dump->page and dump->page + dump->size */
	opline = (size_t)dump->page + dump->size - 1;

	while (1) {
		phpdbg_btree *branch = PHPDBG_G(watchpoint_tree);
		phpdbg_watchpoint_t *watch;
		int i = sizeof(void *) * 8 - 1, last_superior_i = -1;
		size_t addr = 0;

		do {
			/* an impossible branch was found if: */
			if ((opline >> i) % 2 == 0 && !branch->branches[0]) {
				/* there's no lower branch than opline */
				if (last_superior_i == -1) {
					return 1;
				}
				/* reset state */
				branch = PHPDBG_G(watchpoint_tree);
				addr = 0;
				i = sizeof(void *) * 8 - 1;
				/* follow branch according to bits in opline until the last lower branch before the impossible branch */
				do {
					CHOOSE_BRANCH((opline >> i) % 2 == 1 && branch->branches[1]);
				} while (--i > last_superior_i);
				/* use now the lower branch of which we can be sure that it contains only branches lower than opline */
				CHOOSE_BRANCH(0);
				/* and choose the highest possible branch in the branch containing only branches lower than opline */
				while (i--) {
					CHOOSE_BRANCH(branch->branches[1]);
				}
				break;
			}
			/* follow branch according to bits in opline until having found an impossible branch */
			if ((opline >> i) % 2 == 1 && branch->branches[1]) {
				if (branch->branches[0]) {
					last_superior_i = i;
				}
				CHOOSE_BRANCH(1);
			} else {
				CHOOSE_BRANCH(0);
			}
		} while (i--);

		if (watch == branch->watchpoint)
			return 1; /* TODO: there's sometime wrong with the breaking condition ... */

		watch = branch->watchpoint;
		oldPtr = (char *)&dump->data + ((size_t)watch->addr.ptr - (size_t)dump->page);
		if (memcmp(oldPtr, watch->addr.ptr, watch->size) != SUCCESS) {
			PHPDBG_G(watchpoint_hit) = 1;

			phpdbg_notice("Breaking on watchpoint %s", watch->str);
			switch (watch->type) {
				case WATCH_ON_ZVAL:
					phpdbg_write("Old value: ");
					zend_print_flat_zval_r((zval *)oldPtr TSRMLS_CC);
					phpdbg_writeln("");
					phpdbg_write("New value: ");
					zend_print_flat_zval_r(watch->addr.zv TSRMLS_CC);
					phpdbg_writeln("");
					break;
			}
		} else {
			break;
		}

		opline = (size_t)watch->addr.ptr - 1;
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

void phpdbg_store_watchpoint(phpdbg_watchpoint_t *watch TSRMLS_DC) {
	phpdbg_btree **branch = &PHPDBG_G(watchpoint_tree);
	int i = sizeof(void *) * 8 - 1;

	do {
		if (*branch == NULL) {
			break;
		}
		branch = &(*branch)->branches[((size_t)watch->addr.ptr >> i) % 2];
	} while (i--);

	if (*branch == NULL) {
		phpdbg_btree *memory = *branch = emalloc((i + 2) * sizeof(phpdbg_btree));
		do {
			(*branch)->branches[!(((size_t)watch->addr.ptr >> i) % 2)] = NULL;
			branch = &(*branch)->branches[((size_t)watch->addr.ptr >> i) % 2];
			*branch = ++memory;
		} while (i--);
	}

	(*branch)->watchpoint = watch;
}

void phpdbg_create_addr_watchpoint(void *addr, size_t size, phpdbg_watchpoint_t *watch) {
	int m;

	watch->addr.ptr = addr;
	watch->size = size;
	watch->type = WATCH_ON_PTR;

	/* pagesize is assumed to be in the range of 2^x */
	m = mprotect(phpdbg_get_page_boundary(addr), phpdbg_get_total_page_size(addr, size), PROT_NONE | PROT_READ);

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
	zval **zv;
	phpdbg_watchpoint_t *watch = emalloc(sizeof(phpdbg_watchpoint_t));

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

	watch->str = estrndup(name, len);

	/* Lookup current symbol table */
	if (zend_hash_find(EG(current_execute_data)->symbol_table, name, len + 1, (void **)&zv) == SUCCESS) {
		zend_hash_add(&PHPDBG_G(watchpoints), name, len, &watch, sizeof(phpdbg_watchpoint_t *), NULL);
		phpdbg_store_watchpoint(watch TSRMLS_CC);
		phpdbg_create_zval_watchpoint(*zv, watch);
		return SUCCESS;
	}

	return FAILURE;
}
