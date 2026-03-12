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
#include <mach-o/dyld.h>

TSRMLS_CACHE_EXTERN();

/* Thunk format used since dydl 1284 (approx. MacOS 15)
 * https://github.com/apple-oss-distributions/dyld/blob/9307719dd8dc9b385daa412b03cfceb897b2b398/libdyld/ThreadLocalVariables.h#L146 */
#if defined(__x86_64__) || defined(__aarch64__)
struct TLV_Thunkv2
{
	void*        func;
	uint32_t     key;
	uint32_t     offset;
};
#else
struct TLV_Thunkv2
{
	void*        func;
	uint16_t     key;
	uint16_t     offset;
};
#endif

/* Thunk format used in earlier versions */
struct TLV_Thunkv1
{
	void*       func;
	size_t      key;
	size_t      offset;
};

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
	struct TLV_Thunkv2 *thunk;
	__asm__ __volatile__(
		"leaq __tsrm_ls_cache(%%rip),%0"
		: "=r" (thunk));

	/* Detect dyld 1284: With dyld 1284, thunk->func will be _tlv_get_addr.
	 * Unfortunately this symbol is private, but we can find it
	 * as _tlv_bootstrap+8: https://github.com/apple-oss-distributions/dyld/blob/9307719dd8dc9b385daa412b03cfceb897b2b398/libdyld/threadLocalHelpers.s#L54
	 * In earlier versions, thunk->func will be tlv_get_addr, which is not
	 * _tlv_bootstrap+8.
	 */
	if (thunk->func == (void*)((char*)_tlv_bootstrap + 8)) {
		*module_offset = thunk->offset;
		*module_index = (size_t)thunk->key * 8;
	} else {
		struct TLV_Thunkv1 *thunkv1 = (struct TLV_Thunkv1*) thunk;
		*module_offset = thunkv1->offset;
		*module_index = thunkv1->key * 8;
	}

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
