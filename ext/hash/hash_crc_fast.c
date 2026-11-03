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
#include "php_hash_crc_fast.h"
#include "zend_exceptions.h"

#ifdef HAVE_CRC_FAST

/* Initialize a crc-fast context with specified algorithm */
PHP_HASH_API zend_result php_crc_fast_init(php_crc_fast_context *context, enum CrcFastAlgorithm algorithm)
{
    if (!context) {
        return FAILURE;
    }

    context->digest_handle = crc_fast_digest_new(algorithm);
    if (!context->digest_handle) {
        return FAILURE;
    }

    context->algorithm = algorithm;
    context->finalized = false;

    return SUCCESS;
}

/* Update crc-fast context with new data chunk - optimized for hot path */
PHP_HASH_API void php_crc_fast_update(php_crc_fast_context *context, const unsigned char *data, size_t len)
{
    /* Fast path: inline common case checks for better performance */
    if (EXPECTED(php_crc_fast_is_available(context) && (data || len == 0))) {
        if (len > 0) {
            crc_fast_digest_update(context->digest_handle, (const char *)data, len);
        }
    }
}

/* Finalize crc-fast context and return computed checksum */
PHP_HASH_API uint64_t php_crc_fast_final(php_crc_fast_context *context)
{
    uint64_t result;

    if (!context || !context->digest_handle) {
        php_crc_fast_throw_exception("Invalid context provided to php_crc_fast_final");
        return 0;
    }

    if (context->finalized) {
        php_crc_fast_throw_exception("Context already finalized");
        return 0;
    }

    result = crc_fast_digest_finalize(context->digest_handle);
    context->finalized = true;

    return result;
}

/* Copy crc-fast context for hash_copy() support */
PHP_HASH_API zend_result php_crc_fast_copy(const php_crc_fast_context *src, php_crc_fast_context *dest)
{
    if (!src || !dest) {
        return FAILURE;
    }

    if (src->finalized || !src->digest_handle) {
        return FAILURE;
    }

    /* Extract current state from source digest */
    uint64_t current_state = crc_fast_digest_get_state(src->digest_handle);
    
    /* Create new digest with the extracted state */
    dest->digest_handle = crc_fast_digest_new_with_init_state(src->algorithm, current_state);
    if (!dest->digest_handle) {
        return FAILURE;
    }

    dest->algorithm = src->algorithm;
    dest->finalized = false;

    return SUCCESS;
}

/* One-shot checksum calculation for performance optimization */
PHP_HASH_API uint64_t php_crc_fast_checksum(enum CrcFastAlgorithm algorithm, const unsigned char *data, size_t len)
{
    /* Fast path: optimize for common case */
    if (EXPECTED(data || len == 0)) {
        return crc_fast_checksum(algorithm, (const char *)data, len);
    }
    
    /* This should not happen in normal operation since callers validate input */
    return 0;
}

/* Free crc-fast context resources */
PHP_HASH_API void php_crc_fast_free(php_crc_fast_context *context)
{
    if (context && context->digest_handle) {
        crc_fast_digest_free(context->digest_handle);
        context->digest_handle = NULL;
        context->finalized = true;
    }
}



/* Error handling function */
PHP_HASH_API void php_crc_fast_throw_exception(const char *message)
{
    zend_throw_exception(zend_ce_error, message, 0);
}

#endif /* HAVE_CRC_FAST */