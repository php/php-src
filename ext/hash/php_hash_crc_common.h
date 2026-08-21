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

/* CRC64 result storage helper - stores 64-bit results in big-endian format */
PHP_HASH_API void php_crc_store_result(unsigned char *digest, uint64_t result, size_t digest_size);

/*
 * Common CRC context initialization helper
 * Initializes both software state and crc-fast context if available
 * Returns true if crc-fast is being used, false otherwise
 */
#ifdef HAVE_CRC_FAST
static inline bool php_crc_init_context64(uint64_t *state, uint64_t initial_state,
                                          bool *using_crc_fast, php_crc_fast_context *crc_fast_ctx,
                                          int crc_fast_algo)
{
	*state = initial_state;
	*using_crc_fast = false;
	if (php_crc_fast_init(crc_fast_ctx, crc_fast_algo) == SUCCESS) {
		*using_crc_fast = true;
	}
	return *using_crc_fast;
}

static inline bool php_crc_init_context32(uint32_t *state, uint32_t initial_state,
                                          bool *using_crc_fast, php_crc_fast_context *crc_fast_ctx,
                                          int crc_fast_algo)
{
	*state = initial_state;
	*using_crc_fast = false;
	if (php_crc_fast_init(crc_fast_ctx, crc_fast_algo) == SUCCESS) {
		*using_crc_fast = true;
	}
	return *using_crc_fast;
}
#else
static inline void php_crc_init_context64(uint64_t *state, uint64_t initial_state)
{
	*state = initial_state;
}

static inline void php_crc_init_context32(uint32_t *state, uint32_t initial_state)
{
	*state = initial_state;
}
#endif

/*
 * Common CRC context update helper
 * Routes to crc-fast implementation if available
 * Returns true if crc-fast handled the update (caller should return early)
 */
#ifdef HAVE_CRC_FAST
static inline bool php_crc_update_context(bool using_crc_fast, php_crc_fast_context *crc_fast_ctx,
                                          const unsigned char *input, size_t len)
{
	if (EXPECTED(using_crc_fast)) {
		php_crc_fast_update(crc_fast_ctx, input, len);
		return true;
	}
	return false;
}
#else
static inline bool php_crc_update_context(const unsigned char *input, size_t len)
{
	return false;
}
#endif

/*
 * Common CRC context finalization helper for CRC64
 * Finalizes crc-fast context if available
 * Returns true if crc-fast handled the finalization (caller should return early)
 */
#ifdef HAVE_CRC_FAST
static inline bool php_crc_finalize_context(bool using_crc_fast, php_crc_fast_context *crc_fast_ctx,
                                            unsigned char *digest, size_t digest_size, uint64_t *state)
{
	if (EXPECTED(using_crc_fast)) {
		uint64_t result = php_crc_fast_final(crc_fast_ctx);
		php_crc_store_result(digest, result, digest_size);
		php_crc_fast_free(crc_fast_ctx);
		*state = 0;
		return true;
	}
	return false;
}
#else
static inline bool php_crc_finalize_context(unsigned char *digest, size_t digest_size, uint64_t *state)
{
	return false;
}
#endif

/*
 * Common CRC context copy helper
 * Copies crc-fast context if available, otherwise just copies state
 */
#ifdef HAVE_CRC_FAST
static inline void php_crc_copy_context64(bool src_using_crc_fast,
                                          const php_crc_fast_context *src_crc_fast_ctx,
                                          uint64_t src_state,
                                          bool *dest_using_crc_fast,
                                          php_crc_fast_context *dest_crc_fast_ctx,
                                          uint64_t *dest_state)
{
	if (EXPECTED(src_using_crc_fast)) {
		*dest_using_crc_fast = false;
		dest_crc_fast_ctx->digest_handle = NULL;
		dest_crc_fast_ctx->finalized = false;
		if (EXPECTED(php_crc_fast_copy(src_crc_fast_ctx, dest_crc_fast_ctx) == SUCCESS)) {
			*dest_using_crc_fast = true;
			*dest_state = crc_fast_digest_get_state(src_crc_fast_ctx->digest_handle);
		} else {
			*dest_state = src_state;
		}
	} else {
		*dest_using_crc_fast = false;
		dest_crc_fast_ctx->digest_handle = NULL;
		dest_crc_fast_ctx->finalized = false;
		*dest_state = src_state;
	}
}

static inline void php_crc_copy_context32(bool src_using_crc_fast,
                                          const php_crc_fast_context *src_crc_fast_ctx,
                                          uint32_t src_state,
                                          bool *dest_using_crc_fast,
                                          php_crc_fast_context *dest_crc_fast_ctx,
                                          uint32_t *dest_state)
{
	if (EXPECTED(src_using_crc_fast)) {
		*dest_using_crc_fast = false;
		dest_crc_fast_ctx->digest_handle = NULL;
		dest_crc_fast_ctx->finalized = false;
		if (EXPECTED(php_crc_fast_copy(src_crc_fast_ctx, dest_crc_fast_ctx) == SUCCESS)) {
			*dest_using_crc_fast = true;
			*dest_state = (uint32_t)crc_fast_digest_get_state(src_crc_fast_ctx->digest_handle);
		} else {
			*dest_state = src_state;
		}
	} else {
		*dest_using_crc_fast = false;
		dest_crc_fast_ctx->digest_handle = NULL;
		dest_crc_fast_ctx->finalized = false;
		*dest_state = src_state;
	}
}
#else
static inline void php_crc_copy_context64(uint64_t src_state, uint64_t *dest_state)
{
	*dest_state = src_state;
}

static inline void php_crc_copy_context32(uint32_t src_state, uint32_t *dest_state)
{
	*dest_state = src_state;
}
#endif

/*
 * Common serialization state extraction helper
 * Returns the current CRC state for serialization
 */
#ifdef HAVE_CRC_FAST
static inline uint64_t php_crc_get_serialization_state(bool using_crc_fast,
                                                       const php_crc_fast_context *crc_fast_ctx,
                                                       uint64_t software_state)
{
	if (using_crc_fast && crc_fast_ctx->digest_handle) {
		return crc_fast_digest_get_state(crc_fast_ctx->digest_handle);
	}
	return software_state;
}
#else
static inline uint64_t php_crc_get_serialization_state(uint64_t software_state)
{
	return software_state;
}
#endif

/*
 * Common unserialization context reset helper
 * Resets crc-fast context fields after unserialization
 */
#ifdef HAVE_CRC_FAST
static inline void php_crc_reset_context_for_unserialize(bool *using_crc_fast,
                                                         php_crc_fast_context *crc_fast_ctx)
{
	*using_crc_fast = false;
	crc_fast_ctx->digest_handle = NULL;
	crc_fast_ctx->finalized = false;
}
#else
static inline void php_crc_reset_context_for_unserialize(void)
{
	/* No-op for non-crc-fast builds */
}
#endif

#endif /* PHP_HASH_CRC_COMMON_H */
