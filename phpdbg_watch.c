#include "zend.h"
#include "phpdbg.h"
#include "phpdbg_watch.h"
#include <unistd.h>
#include <sys/mman.h>

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

long phpdbg_pagesize;

void phpdbg_setup_watchpoints() {
#ifdef _SC_PAGE_SIZE
	phpdbg_pagesize = sysconf(_SC_PAGE_SIZE);
#endif
#ifdef _SC_PAGESIZE
	phpdbg_pagesize = sysconf(_SC_PAGESIZE);
#endif
#ifdef _SC_NUTC_OS_PAGESIZE
	phpdbg_pagesize = sysconf(_SC_NUTC_OS_PAGESIZE);
#endif
}

int phpdbg_check_for_watchpoint(void *addr) {
	return FAILURE;
}

int phpdbg_watchpoint_segfault_handler(siginfo_t *info, void *context TSRMLS_DC) {
	printf("Text");
	return FAILURE;
}

void phpdbg_create_addr_watchpoint(void *addr, size_t size) {
	int m;

	/* pagesize is assumed to be in the range of 2^x */
	m= mprotect((void *)((unsigned long)addr & ~(phpdbg_pagesize - 1)), ((size - 1)  & ~(phpdbg_pagesize - 1)) | phpdbg_pagesize, PROT_NONE | PROT_READ);

	printf("\n!!!!!\n%d\n!!!!!\n", m);
}

void phpdbg_create_zval_watchpoint(zval *zv) {
	phpdbg_create_addr_watchpoint(zv, sizeof(zval));
}

int phpdbg_create_var_watchpoint(char *name, size_t len TSRMLS_DC) {
	zval *zv;

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
		phpdbg_create_zval_watchpoint(zv);
		return SUCCESS;
	}

	return FAILURE;
}
