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

#include "Zend/zend_portability.h"
#include "Zend/zend_types.h"
#include "TSRM/TSRM.h"
#include "zend_accelerator_debug.h"

#include <stdint.h>
#include <unistd.h>

TSRMLS_CACHE_EXTERN();

/* https://developer.arm.com/documentation/ddi0602/2025-03/Base-Instructions/ADRP--Form-PC-relative-address-to-4KB-page- */
#define AARCH64_ADRP_IMM_MASK          0x60ffffe0 /* bits 30-29, 23-5 */
#define AARCH64_ADRP_IMMHI_MASK        0x00ffffe0 /* bits 23-5 */
#define AARCH64_ADRP_IMMLO_MASK        0x60000000 /* bits 30-29 */
#define AARCH64_ADRP_IMMHI_START       5
#define AARCH64_ADRP_IMMLO_START       29
#define AARCH64_ADRP_IMMLO_WIDTH       2

#define AARCH64_LDR_UNSIGNED_IMM_MASK  0x003ffc00 /* bits 21-10 */
#define AARCH64_ADD_IMM_MASK           0x003ffc00 /* bits 21-10 */
#define AARCH64_MOVZ_IMM_MASK          0x001fffe0 /* bits 20-5 */
#define AARCH64_MOVZ_HW_MASK           0x00600000 /* bits 22-21 */
#define AARCH64_MOVK_IMM_MASK          0x001fffe0 /* bits 20-5 */
#define AARCH64_MOVK_HW_MASK           0x00600000 /* bits 22-21 */
#define AARCH64_NOP                    0xd503201f

#undef USE_FALLBACK

#ifdef __MUSL__

# define DTV_OFFSET -8
# define DTV_INDEX_GAP 0

typedef struct _dtv_pointer_t {
	uintptr_t val;
} dtv_pointer_t;

typedef struct _tls_descriptor {
	size_t index;
	size_t offset;
} tls_descriptor;

#elif defined(__FreeBSD__)

# define DTV_OFFSET 0
/* Index is offset by 1 on FreeBSD (https://github.com/freebsd/freebsd-src/blob/22ca6db50f4e6bd75a141f57cf953d8de6531a06/lib/libc/gen/tls.c#L88) */
# define DTV_INDEX_GAP 1

typedef struct _dtv_pointer_t {
	uintptr_t val;
} dtv_pointer_t;

/* https://github.com/freebsd/freebsd-src/blob/c52ca7dd09066648b1cc40f758289404d68ab886/libexec/rtld-elf/aarch64/reloc.c#L180-L184 */
typedef struct _tls_descriptor {
	void*   thunk;
	int     index;
	size_t  offset;
} tls_descriptor;

#elif defined(__GLIBC__)

# define DTV_OFFSET 0
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
	uint32_t *insn;
	void *thread_pointer;

	__asm__ __volatile__(
		/* Load thread pointer address */
		"mrs   %0, tpidr_el0\n"
		/* Load next instruction address */
		"adr   %1, .+4\n\t"
		/* General Dynamic code sequence as expected by linkers */
		"adrp  x0, :tlsdesc:_tsrm_ls_cache\n"
		"ldr   x1, [x0, #:tlsdesc_lo12:_tsrm_ls_cache]\n"
		"add   x0, x0, :tlsdesc_lo12:_tsrm_ls_cache\n"
		".tlsdesccall    _tsrm_ls_cache\n"
		"blr   x1\n"
		"mrs   x8, tpidr_el0\n"
		"add   %2, x8, x0\n"
		: "=r" (thread_pointer), "=r" (insn), "=r" (addr)
		:
		: "x0", "x1", "x8");

	ZEND_ASSERT(addr == &_tsrm_ls_cache);

	/* Check if the general dynamic code was relaxed by the linker */

	// adrp x0, #any
	if ((insn[0] & ~AARCH64_ADRP_IMM_MASK) != 0x90000000) {
		zend_accel_error(ACCEL_LOG_DEBUG, "adrp insn does not match: 0x%08" PRIx32 "\n", insn[0]);
		goto code_changed;
	}

	// ldr x1, [x0, #any]
	if ((insn[1] & ~AARCH64_LDR_UNSIGNED_IMM_MASK) != 0xf9400001) {
		zend_accel_error(ACCEL_LOG_DEBUG, "ldr insn does not match: 0x%08" PRIx32 "\n", insn[1]);
		goto code_changed;
	}

	// add x0, x0, any
	if ((insn[2] & ~AARCH64_ADD_IMM_MASK) != 0x91000000) {
		zend_accel_error(ACCEL_LOG_DEBUG, "add insn does not match: 0x%08" PRIx32 "x\n", insn[2]);
		goto code_changed;
	}

	/* Code is intact, we can extract immediate values */

	uint64_t adrp_immhi = (uint64_t)((insn[0] & AARCH64_ADRP_IMMHI_MASK) >> AARCH64_ADRP_IMMHI_START);
	uint64_t adrp_immlo = (uint64_t)((insn[0] & AARCH64_ADRP_IMMLO_MASK) >> AARCH64_ADRP_IMMLO_START);
	uint64_t adrp_imm = ((adrp_immhi << AARCH64_ADRP_IMMLO_WIDTH) | adrp_immlo) << 12;
	uint64_t add_imm = (uint64_t)(insn[2] & AARCH64_ADD_IMM_MASK) >> 10;
	uint64_t pc = (uint64_t)insn;
	uintptr_t **where = (uintptr_t**)((pc & ~(4096-1)) + adrp_imm + add_imm);

	/* See https://github.com/ARM-software/abi-aa/blob/2a70c42d62e9c3eb5887fa50b71257f20daca6f9/aaelf64/aaelf64.rst
	 * section "Relocations for thread-local storage".
	 * The first entry holds a pointer to the variable's TLS descriptor resolver
	 * function and the second entry holds a platform-specific offset or
	 * pointer. */
	tls_descriptor *tlsdesc = (tls_descriptor*)(where[1]);

	if ((uintptr_t)&_tsrm_ls_cache - (uintptr_t)thread_pointer == (uintptr_t)tlsdesc) {
		zend_accel_error(ACCEL_LOG_DEBUG, "static tls at offset %p from thread pointer (inferred from tlsdesc)\n", tlsdesc);
		*tcb_offset = (uintptr_t)tlsdesc;
		return SUCCESS;
	}

	*module_index = (tlsdesc->index + DTV_INDEX_GAP) * sizeof(dtv_pointer_t);
	*module_offset = tlsdesc->offset;

# if ZEND_DEBUG
	/* We've got the TLS descriptor. Double check: */

	dtv_pointer_t *dtv = *(dtv_pointer_t**)((uintptr_t)thread_pointer + DTV_OFFSET);
	addr = (void*)(((dtv_pointer_t*)((char*)dtv + *module_index))->val + *module_offset);

	ZEND_ASSERT(addr == &_tsrm_ls_cache);
# endif

	zend_accel_error(ACCEL_LOG_DEBUG, "dynamic tls module idx %zu offset %zu (inferred from code)\n", (size_t)tlsdesc->index, tlsdesc->offset);

	return SUCCESS;

code_changed:

	/* Code was changed by the linker. Check if we recognize the updated code */

	// movz	x0, #0, lsl #16
	if ((insn[0] & ~AARCH64_MOVZ_IMM_MASK) != 0xd2a00000) {
		zend_accel_error(ACCEL_LOG_DEBUG, "movz insn does not match: 0x%08" PRIx32 "\n", insn[0]);
		return FAILURE;
	}

	// movk	x0, #0x10
	if ((insn[1] & ~AARCH64_MOVK_IMM_MASK) != 0xf2800000) {
		zend_accel_error(ACCEL_LOG_DEBUG, "movk insn does not match: 0x%08" PRIx32 "\n", insn[1]);
		return FAILURE;
	}

	// nop
	for (int i = 0; i < 2; i++) {
		if (insn[2+i] != AARCH64_NOP) {
			zend_accel_error(ACCEL_LOG_DEBUG, "nop(%d) insn does not match: 0x%08" PRIx32 "\n", i, insn[2+i]);
			return FAILURE;
		}
	}

	/* Extract immediate values */

	uint64_t movz_imm = (insn[0] & AARCH64_MOVZ_IMM_MASK) >> 5;
	uint64_t movz_shift = (((insn[0] & AARCH64_MOVZ_HW_MASK) >> 21) << 4);
	uint64_t movk_imm = (insn[1] & AARCH64_MOVK_IMM_MASK) >> 5;
	uint64_t offset = (movz_imm << movz_shift) | movk_imm;

	if ((uintptr_t)&_tsrm_ls_cache - (uintptr_t)thread_pointer == offset) {
		zend_accel_error(ACCEL_LOG_DEBUG, "static tls at offset %" PRIxPTR " from thread pointer (inferred from code)\n", offset);
		*tcb_offset = offset;
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
		"mrs   %0, tpidr_el0\n"
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
