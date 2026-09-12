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
  | Author: Don MacAskill <don@smugmug.com>                              |
  +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_crc_common.h"

/* CRC64 result storage helper - stores 64-bit results in big-endian format */
PHP_HASH_API void php_crc_store_result(unsigned char *digest, uint64_t result, size_t digest_size)
{
    /* This function is only used by CRC64 algorithms, which expect big-endian output */
    if (digest_size == 8) {
        digest[0] = (unsigned char) ((result >> 56) & 0xff);
        digest[1] = (unsigned char) ((result >> 48) & 0xff);
        digest[2] = (unsigned char) ((result >> 40) & 0xff);
        digest[3] = (unsigned char) ((result >> 32) & 0xff);
        digest[4] = (unsigned char) ((result >> 24) & 0xff);
        digest[5] = (unsigned char) ((result >> 16) & 0xff);
        digest[6] = (unsigned char) ((result >> 8) & 0xff);
        digest[7] = (unsigned char) (result & 0xff);
    }
}