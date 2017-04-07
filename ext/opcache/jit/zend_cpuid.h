/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_CPUID
#define ZEND_CPUID

#define ZEND_CPUID_EDX_FPU    (1<<0)
#define ZEND_CPUID_EDX_MMX    (1<<23)
#define ZEND_CPUID_EDX_SSE    (1<<25)
#define ZEND_CPUID_EDX_SSE2   (1<<26)

#define ZEND_CPUID_ECX_SSE3   (1<<0)
#define ZEND_CPUID_ECX_SSSE3  (1<<9)
#define ZEND_CPUID_ECX_FMA3   (1<<12)
#define ZEND_CPUID_ECX_SSE41  (1<<19)
#define ZEND_CPUID_ECX_SSE42  (1<<20)
#define ZEND_CPUID_ECX_AES    (1<<25)
#define ZEND_CPUID_ECX_AVX    (1<<28)
#define ZEND_CPUID_ECX_RDRAND (1<<30)

typedef struct _zend_cpuid_info {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} zend_cpuid_info;

static inline void zend_cpuid(uint32_t func, uint32_t subfunc, zend_cpuid_info *info)
{
	__asm__ __volatile__ (
		"cpuid"
		: "=a"(info->eax), "=b"(info->ebx), "=c"(info->ecx), "=d"(info->edx)
		: "a"(func), "c"(subfunc)
	);
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
