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
 *  | Authors: Arnaud Le Blanc <arnaud.lb@gmail.com>                       |
 *  +----------------------------------------------------------------------+
 */

#include "zend_portability.h"
#include "zend_types.h"
#include "TSRM/TSRM.h"
#include "zend_accelerator_debug.h"
#include "zend_jit_tls.h"

#include <stdint.h>
#include <unistd.h>

TSRMLS_CACHE_EXTERN();

#undef USE_FALLBACK

#ifdef __MUSL__

# define DTV_OFFSET 4
# define DTV_INDEX_GAP 0

typedef struct _dtv_pointer_t {
	uintptr_t val;
} dtv_pointer_t;

typedef struct _tls_descriptor {
	size_t index;
	size_t offset;
} tls_descriptor;

#elif defined(__FreeBSD__)

# define DTV_OFFSET 4
# define DTV_INDEX_GAP 1

typedef struct _dtv_pointer_t {
	uintptr_t val;
} dtv_pointer_t;

/* https://github.com/freebsd/freebsd-src/blob/6b94546a7ea2dc593f5765bd5465a8b7bb80c325/libexec/rtld-elf/i386/rtld_machdep.h#L65 */
typedef struct _tls_descriptor {
	unsigned long index;
	unsigned long offset;
} tls_descriptor;

#elif defined(__GLIBC__)

# define DTV_OFFSET 4
# define DTV_INDEX_GAP 0

typedef struct _dtv_pointer_t {
	uintptr_t val;
	uintptr_t _;
} dtv_pointer_t;

typedef struct _tls_descriptor {
	size_t index;
	size_t offset;
} tls_descriptor;

#else
# define USE_FALLBACK 1
#endif

zend_result zend_jit_resolve_tsrm_ls_cache_offsets(
	size_t *tcb_offset,
	size_t *module_index,
	size_t *module_offset
) {
#ifdef USE_FALLBACK
	return FAILURE;
#else
	*tcb_offset = tsrm_get_ls_cache_tcb_offset();
	if (*tcb_offset != 0) {
		return SUCCESS;
	}

	void *t_addr;
	unsigned char *code;
	void *thread_pointer;

	__asm__ __volatile__(
		/* Load next instruction address */
		"call   1f\n"
		".subsection 1\n"
		"1:\n"
		"movl (%%esp), %%ebx\n"
		"movl %%ebx, %%esi\n"
		"ret\n"
		".previous\n"
		/* General Dynamic code sequence as expected by linkers */
		"addl   $_GLOBAL_OFFSET_TABLE_, %%ebx\n"
		"leal   _tsrm_ls_cache@TLSGD(,%%ebx,1), %%eax\n"
		"call   ___tls_get_addr@PLT\n"
		/* Load thread pointer address */
		"movl   %%gs:0, %%ebx\n"
		: "=a" (t_addr), "=S" (code), "=b" (thread_pointer)
	);

	ZEND_ASSERT(t_addr == &_tsrm_ls_cache);

	/* Check if the general dynamic code was relaxed by the linker */

	// addl any,%ebx
	if (memcmp(&code[0], "\x81\xc3", 2) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[0], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "addl insn does not match: 0x%16" PRIx64 "\n", bytes);
		goto code_changed;
	}

	// leal any(,%ebx,1),%eax
	if (memcmp(&code[6], "\x8d\x04\x1d", 3) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[6], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "leal insn does not match: 0x%16" PRIx64 "\n", bytes);
		goto code_changed;
	}

	// call any
	if (memcmp(&code[13], "\xe8", 1) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[13], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "call insn does not match: 0x%16" PRIx64 "\n", bytes);
		goto code_changed;
	}

	/* Code is intact, we can extract immediate values */

	uint32_t addl_imm = ((uint32_t)code[5] << 24)
		| ((uint32_t)code[4] << 16)
		| ((uint32_t)code[3] << 8)
		| ((uint32_t)code[2]);
	uint32_t leal_imm = ((uint32_t)code[12] << 24)
		| ((uint32_t)code[11] << 16)
		| ((uint32_t)code[10] << 8)
		| ((uint32_t)code[9]);

	tls_descriptor *tlsdesc = (tls_descriptor*)(leal_imm + addl_imm + (uintptr_t)code);

	*module_index = (tlsdesc->index + DTV_INDEX_GAP) * sizeof(dtv_pointer_t);
	*module_offset = tlsdesc->offset;

# if ZEND_DEBUG
	/* We've got the TLS descriptor. Double check: */

	dtv_pointer_t *dtv = *(dtv_pointer_t**)((uintptr_t)thread_pointer + DTV_OFFSET);
	void *addr = (void*)(((dtv_pointer_t*)((char*)dtv + *module_index))->val + *module_offset);

	ZEND_ASSERT(addr == &_tsrm_ls_cache);
# endif

	zend_accel_error(ACCEL_LOG_DEBUG, "dynamic tls module idx %zu offset %zu (inferred from code)\n",
			(size_t)tlsdesc->index, (size_t)tlsdesc->offset);

	return SUCCESS;

code_changed:

	/* Code was changed by the linker. Check if we recognize the updated code */

	/*
	 * 81 c3 98 2d 00 00  	addl    $0x2d98,%ebx
	 * 65 a1 00 00 00 00  	movl    %gs:0x0,%eax
	 * 81 e8 04 00 00 00  	subl    $0x4,%eax
	 */

	// movl %gs:0x0,%eax
	if (memcmp(&code[6], "\x65\xa1\x00\x00\x00\x00", 6) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[6], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "movl insn does not match: 0x%16" PRIx64 "\n", bytes);
		return FAILURE;
	}

	// subl $any,%eax
	if (memcmp(&code[12], "\x81\xe8", 2) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[6], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "subl insn does not match: 0x%16" PRIx64 "\n", bytes);
		return FAILURE;
	}

	/* Extract immediate values */

	uint32_t offset = -(((uint32_t)code[17] << 24)
		| ((uint32_t)code[16] << 16)
		| ((uint32_t)code[15] << 8)
		| ((uint32_t)code[14]));

	if ((uintptr_t)&_tsrm_ls_cache - (uintptr_t)thread_pointer == offset) {
		zend_accel_error(ACCEL_LOG_DEBUG, "static tls at offset %" PRIx32 " from thread pointer (inferred from code)\n", offset);
		*tcb_offset = offset;
		return SUCCESS;
	}

	zend_accel_error(ACCEL_LOG_DEBUG, "static tls offset does not match: 0x%" PRIx32 " (expected 0x%" PRIx32 ")\n",
			offset, (uint32_t)((uintptr_t)&_tsrm_ls_cache - (uintptr_t)thread_pointer));

	return FAILURE;
#endif
}

/* Used for testing */
void *zend_jit_tsrm_ls_cache_address(
	size_t tcb_offset,
	size_t module_index,
	size_t module_offset
) {
	char *thread_pointer;
	__asm__ __volatile__(
		"movl   %%gs:0, %0\n"
		: "=r" (thread_pointer)
	);

	if (tcb_offset) {
		return thread_pointer + tcb_offset;
	}
	if (module_index != (size_t)-1 && module_offset != (size_t)-1) {
		dtv_pointer_t *dtv = *(dtv_pointer_t**)((uintptr_t)thread_pointer + DTV_OFFSET);
		return (void*)(((dtv_pointer_t*)((char*)dtv + module_index))->val + module_offset);
	}
	return NULL;
}
