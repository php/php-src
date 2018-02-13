/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 2018-2018 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Xinchen Hui <xinchen.h@zend.com>                            |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CPU_INFO_H
#define ZEND_CPU_INFO_H

#include "zend.h"

#define ZEND_CPU_EBX_MASK     (1<<30)
#define ZEND_CPU_EDX_MASK     (1<<31)

typedef enum _zend_cpu_feature {
	/* ECX */
	ZEND_CPU_FEATURE_SSE3			= (1<<0),
	ZEND_CPU_FEATURE_PCLMULQDQ		= (1<<1),
	ZEND_CPU_FEATURE_DTES64			= (1<<2),
	ZEND_CPU_FEATURE_MONITOR		= (1<<3),
	ZEND_CPU_FEATURE_DSCPL			= (1<<4),
	ZEND_CPU_FEATURE_VMX			= (1<<5),
	ZEND_CPU_FEATURE_SMX			= (1<<6),
	ZEND_CPU_FEATURE_EST			= (1<<7),
	ZEND_CPU_FEATURE_TM2			= (1<<8),
	ZEND_CPU_FEATURE_SSSE3			= (1<<9),
	ZEND_CPU_FEATURE_CID			= (1<<10),
	ZEND_CPU_FEATURE_SDBG			= (1<<11),
	ZEND_CPU_FEATURE_FMA			= (1<<12),
	ZEND_CPU_FEATURE_CX16			= (1<<13),
	ZEND_CPU_FEATURE_XTPR			= (1<<14),
	ZEND_CPU_FEATURE_PDCM			= (1<<15),
	/* reserved						= (1<<16),*/
	ZEND_CPU_FEATURE_PCID			= (1<<17),
	ZEND_CPU_FEATURE_DCA			= (1<<18),
	ZEND_CPU_FEATURE_SSE41			= (1<<19),
	ZEND_CPU_FEATURE_SSE42			= (1<<20),
	ZEND_CPU_FEATURE_X2APIC			= (1<<21),
	ZEND_CPU_FEATURE_MOVBE			= (1<<22),
	ZEND_CPU_FEATURE_POPCNT			= (1<<23),
	ZEND_CPU_FEATURE_TSC_DEADLINE 	= (1<<24),
	ZEND_CPU_FEATURE_AES			= (1<<25),
	ZEND_CPU_FEATURE_XSAVE			= (1<<26),
	ZEND_CPU_FEATURE_OSXSAVE		= (1<<27) ,
	ZEND_CPU_FEATURE_AVX			= (1<<28),
	ZEND_CPU_FEATURE_F16C			= (1<<29),
	/* intentionally don't support	= (1<<30) */
	/* intentionally don't support	= (1<<31) */

	/* EBX */
	ZEND_CPU_FEATURE_AVX2			= (1<<5 | ZEND_CPU_EBX_MASK),

	/* EDX */
	ZEND_CPU_FEATURE_FPU			= (1<<0 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_VME			= (1<<1 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_DE				= (1<<2 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_PSE			= (1<<3 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_TSC			= (1<<4 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_MSR			= (1<<5 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_PAE			= (1<<6 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_MCE			= (1<<7 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_CX8			= (1<<8 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_APIC			= (1<<9 | ZEND_CPU_EDX_MASK),
	/* reserved						= (1<<10 | ZEND_CPU_EDX_MASK),*/
	ZEND_CPU_FEATURE_SEP			= (1<<11 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_MTRR			= (1<<12 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_PGE			= (1<<13 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_MCA			= (1<<14 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_CMOV			= (1<<15 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_PAT			= (1<<16 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_PSE36			= (1<<17 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_PN				= (1<<18 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_CLFLUSH		= (1<<19 | ZEND_CPU_EDX_MASK),
	/* reserved						= (1<<20 | ZEND_CPU_EDX_MASK),*/
	ZEND_CPU_FEATURE_DS				= (1<<21 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_ACPI			= (1<<22 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_MMX			= (1<<23 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_FXSR			= (1<<24 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_SSE			= (1<<25 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_SSE2			= (1<<26 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_SS				= (1<<27 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_HT				= (1<<28 | ZEND_CPU_EDX_MASK),
	ZEND_CPU_FEATURE_TM				= (1<<29 | ZEND_CPU_EDX_MASK)
	/*intentionally don't support   = (1<<30 | ZEND_CPU_EDX_MASK)*/
	/*intentionally don't support   = (1<<31 | ZEND_CPU_EDX_MASK)*/
} zend_cpu_feature;

void zend_cpu_startup();
ZEND_API int zend_cpu_supports(zend_cpu_feature feature);

#if PHP_HAVE_BUILTIN_CPU_SUPPORTS
/* NOTE: you should use following inline function in
 * resolver functions (ifunc), as it could be called
 * before all PLT symbols are resloved. in other words,
 * resolver functions should not depends any external
 * functions */
static zend_always_inline int zend_cpu_supports_sse2() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("sse2");
}

static zend_always_inline int zend_cpu_supports_sse3() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("sse3");
}

static zend_always_inline int zend_cpu_supports_ssse3() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("ssse3");
}

static zend_always_inline int zend_cpu_supports_sse41() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("sse4.1");
}

static zend_always_inline int zend_cpu_supports_sse42() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("sse4.2");
}

static zend_always_inline int zend_cpu_supports_avx() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("avx");
}

static zend_always_inline int zend_cpu_supports_avx2() {
#if PHP_HAVE_BUILTIN_CPU_INIT
	__builtin_cpu_init();
#endif
	return __builtin_cpu_supports("avx2");
}
#else

static zend_always_inline int zend_cpu_supports_sse2() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_SSE2);
}

static zend_always_inline int zend_cpu_supports_sse3() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_SSE3);
}

static zend_always_inline int zend_cpu_supports_ssse3() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_SSSE3);
}

static zend_always_inline int zend_cpu_supports_sse41() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_SSE41);
}

static zend_always_inline int zend_cpu_supports_sse42() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_SSE42);
}

static zend_always_inline int zend_cpu_supports_avx() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_AVX);
}

static zend_always_inline int zend_cpu_supports_avx2() {
	return zend_cpu_supports(ZEND_CPU_FEATURE_AVX2);
}

#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
