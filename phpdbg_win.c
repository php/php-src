/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

phpdbg_btree phpdbg_memory_tree;

int mprotect(void *addr, size_t size, int protection) {
	int var;
	printf("Set protection of %p to %s\n", addr, protection == (PROT_READ | PROT_WRITE) ? "rw": "r-");
	return (int)VirtualProtect(addr, size, protection == (PROT_READ | PROT_WRITE) ? PAGE_READWRITE : PAGE_READONLY, &var);
}

size_t virtual_size(void *ptr) {
	return (size_t)phpdbg_btree_find(&phpdbg_memory_tree, (zend_ulong)ptr)->ptr;
}

/*void *virtual_malloc(size_t size) {
	size_t real_size = phpdbg_get_total_page_size(NULL, size);
	void *addr = VirtualAlloc(NULL, real_size, MEM_COMMIT, PAGE_READWRITE);
	phpdbg_btree_insert(&phpdbg_memory_tree, (zend_ulong)addr, (void *)real_size);
	return addr;
}

void virtual_free(void *ptr) {
	phpdbg_watch_efree(ptr);
	VirtualFree(ptr, virtual_size(ptr), MEM_RELEASE);
	phpdbg_btree_delete(&phpdbg_memory_tree, (zend_ulong)ptr);
}

void *virtual_realloc(void *ptr, size_t size) {
	void *ret;
	size_t original_size = virtual_size(ptr);

	if (original_size >= size) {
		return ptr;
	}
	
	ret = virtual_malloc(size);
	memcpy(ret, ptr, original_size);
	virtual_free(ptr);
	return ret;
}*/

/*void phpdbg_win_set_mm_heap(zend_mm_heap *heap) {
	phpdbg_btree_init(&phpdbg_memory_tree, sizeof(void *) * 8);
	heap->_free = virtual_free;
	heap->_realloc = virtual_realloc;
	heap->_malloc = virtual_malloc;
}

void phpdbg_win_set_mm_storage(zend_mm_storage *storage) {
	phpdbg_btree_init(&phpdbg_memory_tree, sizeof(void *) * 8);
	storage->_free = virtual_free;
	storage->_realloc = virtual_realloc;
	storage->_malloc = virtual_malloc;
}*/

int phpdbg_exception_handler_win32(EXCEPTION_POINTERS *xp) {
	EXCEPTION_RECORD *xr = xp->ExceptionRecord;
	CONTEXT *xc = xp->ContextRecord;
	if(xr->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		TSRMLS_FETCH();
		//printf("Watchpoint hit at: %p\n", xr->ExceptionInformation[1]);
		if (phpdbg_watchpoint_segfault_handler((void *)xr->ExceptionInformation[1] TSRMLS_CC) == SUCCESS) {
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}