/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if defined(__aarch64__) && defined(HAVE_SYS_AUXV_H)
# include <arm_acle.h>
# if defined(__linux__)
#  include <sys/auxv.h>
#  include <asm/hwcap.h>
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
# else
	res = 0;
	return res;
# endif
}

# pragma GCC push_options
# pragma GCC target ("+nothing+crc")
static uint32_t crc32_aarch64(uint32_t crc, char *p, size_t nr) {
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
# pragma GCC pop_options
#endif

/* {{{ proto string crc32(string str)
   Calculate the crc32 polynomial of a string */
PHP_NAMED_FUNCTION(php_if_crc32)
{
	char *p;
	size_t nr;
	uint32_t crcinit = 0;
	register uint32_t crc;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(p, nr)
	ZEND_PARSE_PARAMETERS_END();

	crc = crcinit^0xFFFFFFFF;

#if defined(__aarch64__) && defined(HAVE_SYS_AUXV_H)
	if (has_crc32_insn()) {
		crc = crc32_aarch64(crc, p, nr);
		RETURN_LONG(crc^0xFFFFFFFF);
	}
#endif

	for (; nr--; ++p) {
		crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32tab[(crc ^ (*p)) & 0xFF ];
	}
	RETURN_LONG(crc^0xFFFFFFFF);
}
/* }}} */
