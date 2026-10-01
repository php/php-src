/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_SIGSAFE_H
#define PHPDBG_SIGSAFE_H

#define PHPDBG_SIGSAFE_MEM_SIZE (ZEND_MM_CHUNK_SIZE * 2)

#include "zend.h"

typedef struct {
	char *mem;
	bool allocated;
	zend_mm_heap *heap;
	zend_mm_heap *old_heap;
} phpdbg_signal_safe_mem;

#include "phpdbg.h"

bool phpdbg_active_sigsafe_mem(void);

void phpdbg_set_sigsafe_mem(char *mem);
void phpdbg_clear_sigsafe_mem(void);

zend_mm_heap *phpdbg_original_heap_sigsafe_mem(void);

#endif
