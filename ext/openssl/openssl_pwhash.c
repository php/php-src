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
   | Authors: Remi Collet <remi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_password.h"
#include "php_openssl.h"

#if defined(HAVE_OPENSSL_ARGON2)
#include "Zend/zend_attributes.h"
#include "openssl_pwhash_arginfo.h"
#include <ext/standard/base64.h>
#include "ext/random/php_random_csprng.h"
#include <openssl/params.h>
#include <openssl/core_names.h>
#include <openssl/kdf.h>
#include <openssl/thread.h>
#include <openssl/rand.h>

#define PHP_OPENSSL_MEMLIMIT_MIN  8u
#define PHP_OPENSSL_MEMLIMIT_MAX  UINT32_MAX
#define PHP_OPENSSL_ITERLIMIT_MIN 1u
#define PHP_OPENSSL_ITERLIMIT_MAX UINT32_MAX
#define PHP_OPENSSL_THREADS_MIN   1u
#define PHP_OPENSSL_THREADS_MAX   UINT32_MAX

#define PHP_OPENSSL_ARGON_VERSION 0x13

#define PHP_OPENSSL_SALT_SIZE     16
#define PHP_OPENSSL_HASH_SIZE     32
#define PHP_OPENSSL_DIGEST_SIZE  128

static inline zend_result get_options(zend_array *options, uint32_t *memlimit, uint32_t *iterlimit, uint32_t *threads)
{
	zval *opt;

	*iterlimit = PHP_OPENSSL_PWHASH_ITERLIMIT;
	*memlimit  = PHP_OPENSSL_PWHASH_MEMLIMIT;
	*threads   = PHP_OPENSSL_PWHASH_THREADS;

	if (!options) {
		return SUCCESS;
	}
	if ((opt = zend_hash_str_find(options, "memory_cost", strlen("memory_cost")))) {
		zend_long smemlimit = zval_get_long(opt);

		if ((smemlimit < 0) || (smemlimit < PHP_OPENSSL_MEMLIMIT_MIN) || (smemlimit > (PHP_OPENSSL_MEMLIMIT_MAX))) {
			zend_value_error("Memory cost is outside of allowed memory range");
			return FAILURE;
		}
		*memlimit = smemlimit;
	}
	if ((opt = zend_hash_str_find(options, "time_cost", strlen("time_cost")))) {
		zend_long siterlimit = zval_get_long(opt);
		if ((siterlimit < PHP_OPENSSL_ITERLIMIT_MIN) || (siterlimit > PHP_OPENSSL_ITERLIMIT_MAX)) {
			zend_value_error("Time cost is outside of allowed time range");
			return FAILURE;
		}
		*iterlimit = siterlimit;
	}
	if ((opt = zend_hash_str_find(options, "threads", strlen("threads"))) && (zval_get_long(opt) != 1)) {
		zend_long sthreads = zval_get_long(opt);
		if ((sthreads < PHP_OPENSSL_THREADS_MIN) || (sthreads > PHP_OPENSSL_THREADS_MAX)) {
			zend_value_error("Invalid number of threads");
			return FAILURE;
		}
		*threads = sthreads;
	}
	return SUCCESS;
}

static bool php_openssl_argon2_compute_hash(
	const char *algo,
	uint32_t version, uint32_t memlimit, uint32_t iterlimit, uint32_t threads,
	const char *pass, size_t pass_len,
	const unsigned char *salt, size_t salt_len,
	unsigned char *hash, size_t hash_len)
{
	OSSL_PARAM params[7], *p = params;
	EVP_KDF *kdf = NULL;
	EVP_KDF_CTX *kctx = NULL;
	uint32_t oldthreads;
	bool ret = false;

	oldthreads = OSSL_get_max_threads(NULL);
	if (OSSL_set_max_threads(NULL, threads) != 1) {
		goto fail;
	}
	p = params;
	*p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_THREADS, &threads);
	*p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_LANES,	&threads);
	*p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ITER, &iterlimit);
	*p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_MEMCOST, &memlimit);
	*p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, (void *)salt, salt_len);
	*p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD, (void *)pass, pass_len);
	*p++ = OSSL_PARAM_construct_end();

	if ((kdf = EVP_KDF_fetch(NULL, algo, NULL)) == NULL) {
		goto fail;
	}
	if ((kctx = EVP_KDF_CTX_new(kdf)) == NULL) {
		goto fail;
	}
	if (EVP_KDF_derive(kctx, hash, hash_len, params) != 1) {
		zend_value_error("Unexpected failure hashing password");
		goto fail;
	}

	ret = true;

fail:
	EVP_KDF_free(kdf);
	EVP_KDF_CTX_free(kctx);
	OSSL_set_max_threads(NULL, oldthreads);

	return ret;
}

static zend_string *php_openssl_argon2_hash(const zend_string *password, zend_array *options, const char *algo)
{
	uint32_t iterlimit, memlimit, threads, version = PHP_OPENSSL_ARGON_VERSION;
	zend_string *digest = NULL, *salt64 = NULL, *hash64 = NULL;
	unsigned char hash[PHP_OPENSSL_HASH_SIZE+1], salt[PHP_OPENSSL_SALT_SIZE+1];

	if ((ZSTR_LEN(password) >= UINT32_MAX)) {
		zend_value_error("Password is too long");
		return NULL;
	}
	if (get_options(options, &memlimit, &iterlimit, &threads) == FAILURE) {
		return NULL;
	}
	if (RAND_bytes(salt, PHP_OPENSSL_SALT_SIZE) <= 0) {
		return NULL;
	}

	if (!php_openssl_argon2_compute_hash(algo, version, memlimit, iterlimit, threads,
			ZSTR_VAL(password), ZSTR_LEN(password),	salt, PHP_OPENSSL_SALT_SIZE, hash, PHP_OPENSSL_HASH_SIZE)) {
		return NULL;
	}

	hash64 = php_base64_encode_ex(hash, PHP_OPENSSL_HASH_SIZE, PHP_BASE64_NO_PADDING);

	salt64 = php_base64_encode_ex(salt, PHP_OPENSSL_SALT_SIZE, PHP_BASE64_NO_PADDING);

	digest = zend_string_alloc(PHP_OPENSSL_DIGEST_SIZE, 0);
	ZSTR_LEN(digest) = snprintf(ZSTR_VAL(digest), ZSTR_LEN(digest), "$%s$v=%d$m=%u,t=%u,p=%u$%s$%s",
		algo, version, memlimit, iterlimit, threads, ZSTR_VAL(salt64), ZSTR_VAL(hash64));

	zend_string_release(salt64);
	zend_string_release(hash64);

	return digest;
}

static int php_openssl_argon2_extract(
	const zend_string *digest, uint32_t *version, uint32_t *memlimit, uint32_t *iterlimit,
	uint32_t *threads, zend_string **salt, zend_string **hash)
{
	const char *p;
	char *hash64, *salt64;

	if (!digest || (ZSTR_LEN(digest) < sizeof("$argon2id$"))) {
		return FAILURE;
	}
	p = ZSTR_VAL(digest);
	if (!memcmp(p, "$argon2i$", strlen("$argon2i$"))) {
		p += strlen("$argon2i$");
	} else if (!memcmp(p, "$argon2id$", strlen("$argon2id$"))) {
		p += strlen("$argon2id$");
	} else {
		return FAILURE;
	}
	if (sscanf(p, "v=%" PRIu32 "$m=%" PRIu32 ",t=%" PRIu32 ",p=%" PRIu32,
			version, memlimit, iterlimit, threads) != 4) {
		return FAILURE;
	}
	if (salt && hash) {
		/* start of param */
		p = strchr(p, '$');
		if (!p) {
			return FAILURE;
		}
		/* start of salt */
		p = strchr(p+1, '$');
		if (!p) {
			return FAILURE;
		}
		salt64 = estrdup(p+1);
		/* start of hash */
		hash64 = strchr(salt64, '$');
		if (!hash64) {
			efree(salt64);
			return FAILURE;
		}
		*hash64++ = 0;
		*salt = php_base64_decode((unsigned char *)salt64, strlen(salt64));
		*hash = php_base64_decode((unsigned char *)hash64, strlen(hash64));
		efree(salt64);
	}
	return SUCCESS;
}

static bool php_openssl_argon2_verify(const zend_string *password, const zend_string *digest, const char *algo)
{
	uint32_t version, iterlimit, memlimit, threads;
	zend_string *salt, *hash, *new;
	bool ret = false;

	if ((ZSTR_LEN(password) >= UINT32_MAX) || (ZSTR_LEN(digest) >= UINT32_MAX)) {
		return false;
	}
	if (FAILURE == php_openssl_argon2_extract(digest, &version, &memlimit, &iterlimit, &threads, &salt, &hash)) {
		return false;
	}

	new = zend_string_alloc(ZSTR_LEN(hash), 0);
	if (php_openssl_argon2_compute_hash(algo, version, memlimit, iterlimit, threads,
			ZSTR_VAL(password), ZSTR_LEN(password),	(unsigned char *)ZSTR_VAL(salt),
			ZSTR_LEN(salt), (unsigned char *)ZSTR_VAL(new), ZSTR_LEN(new))) {
		ret = (php_safe_bcmp(hash, new) == 0);
	}

	zend_string_release(new);
	zend_string_release(salt);
	zend_string_release(hash);

	return ret;
}

static bool php_openssl_argon2i_verify(const zend_string *password, const zend_string *digest)
{
	return php_openssl_argon2_verify(password, digest, "argon2i");
}

static bool php_openssl_argon2id_verify(const zend_string *password, const zend_string *digest)
{
	return php_openssl_argon2_verify(password, digest, "argon2id");
}

static bool php_openssl_argon2_needs_rehash(const zend_string *hash, zend_array *options)
{
	uint32_t version, iterlimit, memlimit, threads;
	uint32_t new_version = PHP_OPENSSL_ARGON_VERSION, new_iterlimit, new_memlimit, new_threads;

	if (FAILURE == get_options(options, &new_memlimit, &new_iterlimit, &new_threads)) {
		return true;
	}
	if (FAILURE == php_openssl_argon2_extract(hash, &version, &memlimit, &iterlimit, &threads, NULL, NULL)) {
		return true;
	}

	// Algo already checked in pasword_needs_rehash implementation
	return (version != new_version) ||
		(iterlimit != new_iterlimit) ||
		(memlimit != new_memlimit) ||
		(threads != new_threads);
}

static int php_openssl_argon2_get_info(zval *return_value, const zend_string *hash)
{
	uint32_t v, threads;
	uint32_t memory_cost;
	uint32_t time_cost;

	if (FAILURE == php_openssl_argon2_extract(hash, &v, &memory_cost, &time_cost, &threads, NULL, NULL)) {
		return FAILURE;
	}
	add_assoc_long(return_value, "memory_cost", memory_cost);
	add_assoc_long(return_value, "time_cost", time_cost);
	add_assoc_long(return_value, "threads", threads);

	return SUCCESS;
}


static zend_string *php_openssl_argon2i_hash(const zend_string *password, zend_array *options)
{
	return php_openssl_argon2_hash(password, options, "argon2i");
}

static const php_password_algo openssl_algo_argon2i = {
	"argon2i",
	php_openssl_argon2i_hash,
	php_openssl_argon2i_verify,
	php_openssl_argon2_needs_rehash,
	php_openssl_argon2_get_info,
	NULL,
};

static zend_string *php_openssl_argon2id_hash(const zend_string *password, zend_array *options)
{
	return php_openssl_argon2_hash(password, options, "argon2id");
}

static const php_password_algo openssl_algo_argon2id = {
	"argon2id",
	php_openssl_argon2id_hash,
	php_openssl_argon2id_verify,
	php_openssl_argon2_needs_rehash,
	php_openssl_argon2_get_info,
	NULL,
};

PHP_FUNCTION(openssl_password_hash)
{
	zend_string *password, *algo, *digest;
	zend_array *options = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(algo)
		Z_PARAM_STR(password)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT(options)
	ZEND_PARSE_PARAMETERS_END();

	if (strcmp(ZSTR_VAL(algo), "argon2i") && strcmp(ZSTR_VAL(algo), "argon2id")) {
		zend_argument_value_error(1, "must be a valid password openssl hashing algorithm");
		RETURN_THROWS();
	}

	digest = php_openssl_argon2_hash(password, options, ZSTR_VAL(algo));
	if (!digest) {
		if (!EG(exception)) {
			zend_throw_error(NULL, "Password hashing failed for unknown reason");
		}
		RETURN_THROWS();
	}

	RETURN_NEW_STR(digest);
}

PHP_FUNCTION(openssl_password_verify)
{
	zend_string *password, *algo, *digest;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(algo)
		Z_PARAM_STR(password)
		Z_PARAM_STR(digest)
	ZEND_PARSE_PARAMETERS_END();

	if (strcmp(ZSTR_VAL(algo), "argon2i") && strcmp(ZSTR_VAL(algo), "argon2id")) {
		zend_argument_value_error(1, "must be a valid password openssl hashing algorithm");
		RETURN_THROWS();
	}

	RETURN_BOOL(php_openssl_argon2_verify(password, digest, ZSTR_VAL(algo)));
}

PHP_MINIT_FUNCTION(openssl_pwhash)
{
	zend_string *argon2i = ZSTR_INIT_LITERAL("argon2i", 1);

	if (php_password_algo_find(argon2i)) {
		/* Nothing to do. Core or sodium has registered these algorithms for us. */
		zend_string_release(argon2i);
		return SUCCESS;
	}
	zend_string_release(argon2i);

	register_openssl_pwhash_symbols(module_number);

	if (FAILURE == php_password_algo_register("argon2i", &openssl_algo_argon2i)) {
		return FAILURE;
	}
	if (FAILURE == php_password_algo_register("argon2id", &openssl_algo_argon2id)) {
		return FAILURE;
	}

	return SUCCESS;
}
#endif /* HAVE_OPENSSL_ARGON2 */
