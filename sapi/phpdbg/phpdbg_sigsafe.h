#ifndef PHPDBG_SIGSAFE_H
#define PHPDBG_SIGSAFE_H

#define PHPDBG_SIGSAFE_MEM_SIZE (ZEND_MM_CHUNK_SIZE * 2)

#include "zend.h"

typedef struct {
	char *mem;
	zend_bool allocated;
	zend_mm_heap *heap;
	zend_mm_heap *old_heap;
} phpdbg_signal_safe_mem;

#include "phpdbg.h"

zend_bool phpdbg_active_sigsafe_mem(void);

void phpdbg_set_sigsafe_mem(char *mem);
void phpdbg_clear_sigsafe_mem(void);

zend_mm_heap *phpdbg_original_heap_sigsafe_mem(void);

#endif
