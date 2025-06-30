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

# define DTV_OFFSET 8
# define DTV_INDEX_GAP 0

typedef struct _dtv_pointer_t {
	uintptr_t val;
} dtv_pointer_t;

typedef struct _tls_descriptor {
	size_t index;
	size_t offset;
} tls_descriptor;

#elif defined(__FreeBSD__)

# define DTV_OFFSET 8
# define DTV_INDEX_GAP 1

typedef struct _dtv_pointer_t {
	uintptr_t val;
} dtv_pointer_t;

/* https://github.com/freebsd/freebsd-src/blob/6b94546a7ea2dc593f5765bd5465a8b7bb80c325/libexec/rtld-elf/amd64/rtld_machdep.h#L65 */
typedef struct _tls_descriptor {
	unsigned long index;
	unsigned long offset;
} tls_descriptor;

#elif defined(__GLIBC__)

# define DTV_OFFSET 8
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

	void *addr;
	unsigned char *code;
	void *thread_pointer;

	__asm__ __volatile__(
		/* Load next instruction address */
		"leaq   (%%rip), %%rbx\n"
		/* General Dynamic code sequence as expected by linkers */
		".byte  0x66\n"
		"leaq   _tsrm_ls_cache@tlsgd(%%rip), %%rdi\n"
		".word  0x6666\n"
		"rex64\n"
		"call   __tls_get_addr\n"
		/* Load thread pointer address */
		"movq   %%fs:0, %%rsi\n"
		: "=a" (addr), "=b" (code), "=S" (thread_pointer)
	);

	ZEND_ASSERT(addr == &_tsrm_ls_cache);

	/* Check if the general dynamic code was relaxed by the linker */

	// data16 leaq any(%rip),%rdi
	if (memcmp(&code[0], "\x66\x48\x8d\x3d", 4) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[0], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "leaq insn does not match: 0x%016" PRIx64 "\n", bytes);
		goto code_changed;
	}

	// data16 data16 rex.W call any
	if (memcmp(&code[8], "\x66\x66\x48\xe8", 4) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[8], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "call insn does not match: 0x%016" PRIx64 "\n", bytes);
		goto code_changed;
	}

	/* Code is intact, we can extract immediate values */

	uintptr_t leaq_imm = (uintptr_t)(int32_t)((uint32_t)code[7] << 24)
		| ((uint32_t)code[6] << 16)
		| ((uint32_t)code[5] << 8)
		| ((uint32_t)code[4]);

	tls_descriptor *tlsdesc = (tls_descriptor*)(leaq_imm + (uintptr_t)code + 8 /* leaq */);

	*module_index = ((size_t)tlsdesc->index + DTV_INDEX_GAP) * sizeof(dtv_pointer_t);
	*module_offset = (size_t)tlsdesc->offset;

# if ZEND_DEBUG
	/* We've got the TLS descriptor. Double check: */

	dtv_pointer_t *dtv = *(dtv_pointer_t**)((uintptr_t)thread_pointer + DTV_OFFSET);
	addr = (void*)(((dtv_pointer_t*)((char*)dtv + *module_index))->val + *module_offset);

	ZEND_ASSERT(addr == &_tsrm_ls_cache);
# endif

	zend_accel_error(ACCEL_LOG_DEBUG, "dynamic tls module idx %zu offset %zu (inferred from code)\n",
			(size_t)tlsdesc->index, (size_t)tlsdesc->offset);

	return SUCCESS;

code_changed:

	/* Code was changed by the linker. Check if we recognize the updated code */

	/*
	 * 64 48 8b 04 25 00 00 00 00	movq    %fs:0x0,%rax
	 * 48 8d 80 f8 ff ff ff	        leaq    -0x8(%rax),%rax
	 */

	// movq %fs:0x0,%rax
	if (memcmp(&code[0], "\x64\x48\x8b\x04\x25\x00\x00\x00\x00", 9) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[0], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "movq insn does not match: 0x%016" PRIx64 "\n", bytes);
		return FAILURE;
	}

	// leaq any(%rax),$rax
	if (memcmp(&code[9], "\x48\x8d\x80", 3) != 0) {
		uint64_t bytes;
		memcpy(&bytes, &code[10], 8);
		zend_accel_error(ACCEL_LOG_DEBUG, "leaq insn does not match: 0x%016" PRIx64 "\n", bytes);
		return FAILURE;
	}

	/* Extract immediate values */

	uintptr_t offset = (uintptr_t)(int32_t)(((uint32_t)code[15] << 24)
		| ((uint32_t)code[14] << 16)
		| ((uint32_t)code[13] << 8)
		| ((uint32_t)code[12]));

	if ((uintptr_t)&_tsrm_ls_cache - (uintptr_t)thread_pointer == offset) {
		*tcb_offset = offset;
		zend_accel_error(ACCEL_LOG_DEBUG, "static tls at offset %" PRIxPTR " from thread pointer (inferred from code)\n", offset);
		return SUCCESS;
	}

	zend_accel_error(ACCEL_LOG_DEBUG, "static tls offset does not match: %" PRIxPTR " (expected %" PRIxPTR ")\n",
			offset, ((uintptr_t)&_tsrm_ls_cache - (uintptr_t)thread_pointer));

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
		"movq   %%fs:0, %0\n"
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
