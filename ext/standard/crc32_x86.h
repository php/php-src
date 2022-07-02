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
  | Author: Frank Du <frank.du@intel.com>                                |
  +----------------------------------------------------------------------+
*/

#ifndef _CRC32_X86_HEADER_H_
#define _CRC32_X86_HEADER_H_

#include "php.h"

typedef enum {
	/* polynomial: 0x04C11DB7, used by bzip */
	X86_CRC32 = 0,
	/*
	 polynomial: 0x04C11DB7 with reversed ordering,
	 used by ethernet (IEEE 802.3), gzip, zip, png, etc
	*/
	X86_CRC32B,
	/*
	 polynomial: 0x1EDC6F41 with reversed ordering,
	 used by iSCSI, SCTP, Btrfs, ext4, etc
	*/
	X86_CRC32C,
	X86_CRC32_MAX,
} X86_CRC32_TYPE;

#if ZEND_INTRIN_SSE4_2_PCLMUL_FUNC_PTR
PHP_MINIT_FUNCTION(crc32_x86_intrin);
#endif

#if ZEND_INTRIN_SSE4_2_PCLMUL_NATIVE || ZEND_INTRIN_SSE4_2_PCLMUL_RESOLVER
/* Return the size processed by SIMD routine */
size_t crc32_x86_simd_update(X86_CRC32_TYPE type, uint32_t *crc, const unsigned char *p, size_t nr);
#else
static inline size_t crc32_x86_simd_update(X86_CRC32_TYPE type, uint32_t *crc, const unsigned char *p, size_t nr)
{
	return 0;
}
#endif

#endif
