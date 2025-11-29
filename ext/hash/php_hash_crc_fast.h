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

#ifndef PHP_HASH_CRC_FAST_H
#define PHP_HASH_CRC_FAST_H

#include "php.h"

#ifdef HAVE_CRC_FAST
#include <libcrc_fast.h>

/* CRC algorithm constants - these match the actual crc-fast library enum values */
#define CRC_FAST_CRC32_BZIP2 Crc32Bzip2
#define CRC_FAST_CRC32_ISO_HDLC Crc32IsoHdlc
#define CRC_FAST_CRC32_ISCSI Crc32Iscsi
#define CRC_FAST_CRC64_NVME Crc64Nvme
#define CRC_FAST_CRC64_ECMA182 Crc64Ecma182

/* CRC-Fast context structure for hash operations */
typedef struct {
    struct CrcFastDigestHandle *digest_handle;
    enum CrcFastAlgorithm algorithm;
    bool finalized;
} php_crc_fast_context;

/* CRC-Fast wrapper functions */
PHP_HASH_API zend_result php_crc_fast_init(php_crc_fast_context *context, enum CrcFastAlgorithm algorithm);
PHP_HASH_API void php_crc_fast_update(php_crc_fast_context *context, const unsigned char *data, size_t len);
PHP_HASH_API uint64_t php_crc_fast_final(php_crc_fast_context *context);
PHP_HASH_API zend_result php_crc_fast_copy(const php_crc_fast_context *src, php_crc_fast_context *dest);
PHP_HASH_API uint64_t php_crc_fast_checksum(enum CrcFastAlgorithm algorithm, const unsigned char *data, size_t len);
PHP_HASH_API void php_crc_fast_free(php_crc_fast_context *context);

/* Optimized inline functions for hot paths */
static inline bool php_crc_fast_is_available(const php_crc_fast_context *context) {
    return context && context->digest_handle && !context->finalized;
}

static inline bool php_crc_fast_should_use_checksum(size_t len) {
    /* Use one-shot checksum for small to medium data sizes for better performance */
    return len <= 8192;  /* 8KB threshold - tuned for typical use cases */
}

/* Error handling */
PHP_HASH_API void php_crc_fast_throw_exception(const char *message);

#endif /* HAVE_CRC_FAST */

#endif /* PHP_HASH_CRC_FAST_H */
