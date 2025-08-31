/*
 *  +----------------------------------------------------------------------+
 *  | Zend JIT                                                             |
 *  +----------------------------------------------------------------------+
 *  | Copyright (c) The PHP Group                                          |
 *  +----------------------------------------------------------------------+
 *  | This source file is subject to version 3.01 of the PHP license,      |
 *  | that is bundled with this package in the file LICENSE, and is        |
 *  | available through the world-wide-web at the following url:           |
 *  | https://www.php.net/license/3_01.txt                                 |
 *  | If you did not receive a copy of the PHP license and are unable to   |
 *  | obtain it through the world-wide-web, please send a note to          |
 *  | license@php.net so we can mail you a copy immediately.               |
 *  +----------------------------------------------------------------------+
 *  | Authors: Dmitry Stogov <dmitry@php.net>                              |
 *  +----------------------------------------------------------------------+
 */

#include "Zend/zend_portability.h"
#include "Zend/zend_types.h"
#include "TSRM/TSRM.h"
#include "zend_accelerator_debug.h"

#include <stdint.h>
#include <unistd.h>

TSRMLS_CACHE_EXTERN();

zend_result zend_jit_resolve_tsrm_ls_cache_offsets(
	size_t *tcb_offset,
	size_t *module_index,
	size_t *module_offset
) {
	*tcb_offset = tsrm_get_ls_cache_tcb_offset();
	if (*tcb_offset != 0) {
		return SUCCESS;
	}

#if defined(__x86_64__)
	size_t *ti;
	__asm__ __volatile__(
		"leaq __tsrm_ls_cache(%%rip),%0"
		: "=r" (ti));
	*module_offset = ti[2];
	*module_index = ti[1] * 8;

	return SUCCESS;
#endif

	return FAILURE;
}

/* Used for testing */
void *zend_jit_tsrm_ls_cache_address(
	size_t tcb_offset,
	size_t module_index,
	size_t module_offset
) {

#if defined(__x86_64__)
	if (tcb_offset) {
		char *addr;
		__asm__ __volatile__(
			"movq   %%gs:(%1), %0\n"
			: "=r" (addr)
			: "r" (tcb_offset)
		);
		return addr;
	}
	if (module_index != (size_t)-1 && module_offset != (size_t)-1) {
		char *base;
		__asm__ __volatile__(
			"movq   %%gs:(%1), %0\n"
			: "=r" (base)
			: "r" (module_index)
		);
		return base + module_offset;
	}
#endif

	return NULL;
}
