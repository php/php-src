/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Rasmus Lerdorf <rasmus@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "basic_functions.h"
#include "crc32.h"
#include "crc32_x86.h"

#ifdef HAVE_AARCH64_CRC32
# include <arm_acle.h>
# if defined(__linux__)
#  include <sys/auxv.h>
#  include <asm/hwcap.h>
# elif defined(__APPLE__)
#  include <sys/sysctl.h>
# elif defined(__FreeBSD__)
#  include <sys/auxv.h>

static unsigned long getauxval(unsigned long key) {
	unsigned long ret = 0;
	if (elf_aux_info(key, &ret, sizeof(ret)) != 0)
		return 0;
	return ret;
}
# endif

static inline int has_crc32_insn() {
	/* Only go through the runtime detection once. */
	static int res = -1;
	if (res != -1)
		return res;
# if defined(HWCAP_CRC32)
	res = getauxval(AT_HWCAP) & HWCAP_CRC32;
	return res;
# elif defined(HWCAP2_CRC32)
	res = getauxval(AT_HWCAP2) & HWCAP2_CRC32;
	return res;
# elif defined(__APPLE__)
	size_t reslen = sizeof(res);
	if (sysctlbyname("hw.optional.armv8_crc32", &res, &reslen, NULL, 0) < 0)
		res = 0;
	return res;
# else
	res = 0;
	return res;
# endif
}

# if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC push_options
#  pragma GCC target ("+nothing+crc")
#ifdef HAVE_BOLT
#  pragma GCC optimize("no-reorder-blocks-and-partition")
# endif
# endif
static uint32_t crc32_aarch64(uint32_t crc, const char *p, size_t nr) {
	while (nr >= sizeof(uint64_t)) {
		crc = __crc32d(crc, *(uint64_t *)p);
		p += sizeof(uint64_t);
		nr -= sizeof(uint64_t);
	}
	if (nr >= sizeof(int32_t)) {
		crc = __crc32w(crc, *(uint32_t *)p);
		p += sizeof(uint32_t);
		nr -= sizeof(uint32_t);
	}
	if (nr >= sizeof(int16_t)) {
		crc = __crc32h(crc, *(uint16_t *)p);
		p += sizeof(uint16_t);
		nr -= sizeof(uint16_t);
	}
	if (nr) {
		crc = __crc32b(crc, *p);
	}
	return crc;
}
# if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC pop_options
# endif
#endif

PHPAPI uint32_t php_crc32_bulk_update(uint32_t crc, const char *p, size_t nr)
{
#ifdef HAVE_AARCH64_CRC32
	if (has_crc32_insn()) {
		crc = crc32_aarch64(crc, p, nr);
		return crc;
	}
#endif

#if ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE || ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER
	size_t nr_simd = crc32_x86_simd_update(X86_CRC32B, &crc, (const unsigned char *)p, nr);
	nr -= nr_simd;
	p += nr_simd;
#endif

	/* The trailing part */
	for (; nr--; ++p) {
		crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32tab[(crc ^ (*p)) & 0xFF ];
	}

	return crc;
}

PHPAPI int php_crc32_stream_bulk_update(uint32_t *crc, php_stream *fp, size_t nr)
{
	size_t handled = 0, n;
	char buf[1024];

	while (handled < nr) {
		n = nr - handled;
		n = (n < sizeof(buf)) ? n : sizeof(buf); /* tweak to buf size */

		n = php_stream_read(fp, buf, n);
		if (n > 0) {
			*crc = php_crc32_bulk_update(*crc, buf, n);
			handled += n;
		} else { /* EOF */
			return FAILURE;
		}
	}

	return SUCCESS;
}

/* {{{ Calculate the crc32 polynomial of a string */
PHP_FUNCTION(crc32)
{
	char *p;
	size_t nr;
	uint32_t crc = php_crc32_bulk_init();

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(p, nr)
	ZEND_PARSE_PARAMETERS_END();

	crc = php_crc32_bulk_update(crc, p, nr);

	RETURN_LONG(php_crc32_bulk_end(crc));
}
/* }}} */
