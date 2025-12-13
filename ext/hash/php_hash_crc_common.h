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

#ifndef PHP_HASH_CRC_COMMON_H
#define PHP_HASH_CRC_COMMON_H

#include "php.h"

#ifdef HAVE_CRC_FAST
#include "php_hash_crc_fast.h"
#endif

/* Common CRC context initialization helper
 * Initializes both software state and crc-fast context if available
 * Parameters:
 *   ctx - CRC context to initialize
 *   initial_state - Initial state value for software implementation
 *   crc_fast_algo - crc-fast algorithm identifier
 */
#ifdef HAVE_CRC_FAST
#define PHP_CRC_INIT_CONTEXT(ctx, initial_state, crc_fast_algo) do { \
    (ctx)->state = (initial_state); \
    (ctx)->using_crc_fast = false; \
    if (php_crc_fast_init(&(ctx)->crc_fast_ctx, (crc_fast_algo)) == SUCCESS) { \
        (ctx)->using_crc_fast = true; \
    } \
} while (0)
#else
#define PHP_CRC_INIT_CONTEXT(ctx, initial_state, crc_fast_algo) do { \
    (ctx)->state = (initial_state); \
} while (0)
#endif

/* Common CRC context update helper
 * Routes to crc-fast or software implementation based on context state
 * Parameters:
 *   ctx - CRC context to update
 *   input - Input data buffer
 *   len - Length of input data
 *   software_update_func - Function to call for software implementation
 */
#ifdef HAVE_CRC_FAST
#define PHP_CRC_UPDATE_CONTEXT(ctx, input, len, software_update_func) do { \
    if (EXPECTED((ctx)->using_crc_fast)) { \
        php_crc_fast_update(&(ctx)->crc_fast_ctx, (input), (len)); \
        return; \
    } \
    software_update_func((ctx), (input), (len)); \
} while (0)
#else
#define PHP_CRC_UPDATE_CONTEXT(ctx, input, len, software_update_func) do { \
    software_update_func((ctx), (input), (len)); \
} while (0)
#endif

/* Common CRC context finalization helper
 * Finalizes crc-fast context or calls software implementation
 * Parameters:
 *   ctx - CRC context to finalize
 *   digest - Output buffer for digest
 *   digest_size - Size of digest (8 for CRC64)
 *   software_final_func - Function to call for software implementation
 */
#ifdef HAVE_CRC_FAST
#define PHP_CRC_FINALIZE_CONTEXT(ctx, digest, digest_size, software_final_func) do { \
    if (EXPECTED((ctx)->using_crc_fast)) { \
        uint64_t result = php_crc_fast_final(&(ctx)->crc_fast_ctx); \
        php_crc_store_result((digest), result, (digest_size)); \
        php_crc_fast_free(&(ctx)->crc_fast_ctx); \
        (ctx)->state = 0; \
        return; \
    } \
    software_final_func((digest), (ctx)); \
} while (0)
#else
#define PHP_CRC_FINALIZE_CONTEXT(ctx, digest, digest_size, software_final_func) do { \
    software_final_func((digest), (ctx)); \
} while (0)
#endif

/* Common CRC context copy helper */
#ifdef HAVE_CRC_FAST
#define PHP_CRC_COPY_CONTEXT(src_ctx, dest_ctx, state_type) do { \
    if (EXPECTED((src_ctx)->using_crc_fast)) { \
        (dest_ctx)->using_crc_fast = false; \
        (dest_ctx)->crc_fast_ctx.digest_handle = NULL; \
        (dest_ctx)->crc_fast_ctx.finalized = false; \
        if (EXPECTED(php_crc_fast_copy(&(src_ctx)->crc_fast_ctx, &(dest_ctx)->crc_fast_ctx) == SUCCESS)) { \
            (dest_ctx)->using_crc_fast = true; \
            (dest_ctx)->state = (state_type)crc_fast_digest_get_state((src_ctx)->crc_fast_ctx.digest_handle); \
        } else { \
            (dest_ctx)->state = (src_ctx)->state; \
        } \
    } else { \
        (dest_ctx)->using_crc_fast = false; \
        (dest_ctx)->crc_fast_ctx.digest_handle = NULL; \
        (dest_ctx)->crc_fast_ctx.finalized = false; \
        (dest_ctx)->state = (src_ctx)->state; \
    } \
} while (0)
#else
#define PHP_CRC_COPY_CONTEXT(src_ctx, dest_ctx, state_type) do { \
    (dest_ctx)->state = (src_ctx)->state; \
} while (0)
#endif

/* CRC64 result storage helper - stores 64-bit results in big-endian format */
PHP_HASH_API void php_crc_store_result(unsigned char *digest, uint64_t result, size_t digest_size);

/* Common serialization state extraction helper */
#ifdef HAVE_CRC_FAST
static inline uint64_t php_crc_get_serialization_state(const void *ctx, bool using_crc_fast, 
                                                       const php_crc_fast_context *crc_fast_ctx, 
                                                       uint64_t software_state) {
    if (using_crc_fast && crc_fast_ctx->digest_handle) {
        return crc_fast_digest_get_state(crc_fast_ctx->digest_handle);
    }
    return software_state;
}
#else
static inline uint64_t php_crc_get_serialization_state(const void *ctx, bool using_crc_fast, 
                                                       const void *crc_fast_ctx, 
                                                       uint64_t software_state) {
    return software_state;
}
#endif

/* Common unserialization context reset helper */
#ifdef HAVE_CRC_FAST
#define PHP_CRC_RESET_CONTEXT_FOR_UNSERIALIZE(ctx) do { \
    (ctx)->using_crc_fast = false; \
    (ctx)->crc_fast_ctx.digest_handle = NULL; \
    (ctx)->crc_fast_ctx.finalized = false; \
} while (0)
#else
#define PHP_CRC_RESET_CONTEXT_FOR_UNSERIALIZE(ctx) do { \
    /* No-op for non-crc-fast builds */ \
} while (0)
#endif

#endif /* PHP_HASH_CRC_COMMON_H */
