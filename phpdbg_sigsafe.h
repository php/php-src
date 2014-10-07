#ifndef PHPDBG_SIGSAFE_H
#define PHPDBG_SIGSAFE_H

#include "zend_mm_structs.h"

#define PHPDBG_SIGSAFE_MEM_SIZE (1 << 20)

typedef struct {
	char *mem;
	zend_bool allocated;
	zend_mm_heap heap;
	zend_mm_heap *old_heap;
	zend_mm_storage storage;
} phpdbg_signal_safe_mem;

#include "phpdbg.h"

zend_bool phpdbg_active_sigsafe_mem(TSRMLS_D);

void phpdbg_set_sigsafe_mem(char *mem TSRMLS_DC);
void phpdbg_clear_sigsafe_mem(TSRMLS_D);

zend_mm_heap *phpdbg_original_heap_sigsafe_mem(TSRMLS_D);

#endif
