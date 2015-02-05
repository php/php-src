/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  |         Scott MacVicar <scottmac@php.net>                            |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include "php_hash.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"

static int php_hash_le_hash;
HashTable php_hash_hashtable;

#if (PHP_MAJOR_VERSION >= 5)
# define DEFAULT_CONTEXT FG(default_context)
#else
# define DEFAULT_CONTEXT NULL
#endif

#ifdef PHP_MHASH_BC
struct mhash_bc_entry {
	char *mhash_name;
	char *hash_name;
	int value;
};

#define MHASH_NUM_ALGOS 34

static struct mhash_bc_entry mhash_to_hash[MHASH_NUM_ALGOS] = {
	{"CRC32", "crc32", 0},
	{"MD5", "md5", 1},
	{"SHA1", "sha1", 2},
	{"HAVAL256", "haval256,3", 3},
	{NULL, NULL, 4},
	{"RIPEMD160", "ripemd160", 5},
	{NULL, NULL, 6},
	{"TIGER", "tiger192,3", 7},
	{"GOST", "gost", 8},
	{"CRC32B", "crc32b", 9},
	{"HAVAL224", "haval224,3", 10},
	{"HAVAL192", "haval192,3", 11},
	{"HAVAL160", "haval160,3", 12},
	{"HAVAL128", "haval128,3", 13},
	{"TIGER128", "tiger128,3", 14},
	{"TIGER160", "tiger160,3", 15},
	{"MD4", "md4", 16},
	{"SHA256", "sha256", 17},
	{"ADLER32", "adler32", 18},
	{"SHA224", "sha224", 19},
	{"SHA512", "sha512", 20},
	{"SHA384", "sha384", 21},
	{"WHIRLPOOL", "whirlpool", 22},
	{"RIPEMD128", "ripemd128", 23},
	{"RIPEMD256", "ripemd256", 24},
	{"RIPEMD320", "ripemd320", 25},
	{NULL, NULL, 26}, /* support needs to be added for snefru 128 */
	{"SNEFRU256", "snefru256", 27},
	{"MD2", "md2", 28},
	{"FNV132", "fnv132", 29},
	{"FNV1A32", "fnv1a32", 30},
	{"FNV164", "fnv164", 31},
	{"FNV1A64", "fnv1a64", 32},
	{"JOAAT", "joaat", 33},
};
#endif

/* Hash Registry Access */

PHP_HASH_API const php_hash_ops *php_hash_fetch_ops(const char *algo, size_t algo_len) /* {{{ */
{
	char *lower = zend_str_tolower_dup(algo, algo_len);
	php_hash_ops *ops = zend_hash_str_find_ptr(&php_hash_hashtable, lower, algo_len);
	efree(lower);

	return ops;
}
/* }}} */

PHP_HASH_API void php_hash_register_algo(const char *algo, const php_hash_ops *ops) /* {{{ */
{
	int algo_len = strlen(algo);
	char *lower = zend_str_tolower_dup(algo, algo_len);
	zend_hash_str_add_ptr(&php_hash_hashtable, lower, algo_len, (void *) ops);
	efree(lower);
}
/* }}} */

PHP_HASH_API int php_hash_copy(const void *ops, void *orig_context, void *dest_context) /* {{{ */
{
	php_hash_ops *hash_ops = (php_hash_ops *)ops;

	memcpy(dest_context, orig_context, hash_ops->context_size);
	return SUCCESS;
}
/* }}} */

/* Userspace */

static void php_hash_do_hash(INTERNAL_FUNCTION_PARAMETERS, int isfilename, zend_bool raw_output_default) /* {{{ */
{
	zend_string *digest;
	char *algo, *data;
	size_t algo_len, data_len;
	zend_bool raw_output = raw_output_default;
	const php_hash_ops *ops;
	void *context;
	php_stream *stream = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|b", &algo, &algo_len, &data, &data_len, &raw_output) == FAILURE) {
		return;
	}

	ops = php_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		php_error_docref(NULL, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}
	if (isfilename) {
		if (CHECK_NULL_PATH(data, data_len)) {
			RETURN_FALSE;
		}
		stream = php_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS, NULL, DEFAULT_CONTEXT);
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	if (isfilename) {
		char buf[1024];
		int n;

		while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, (unsigned char *) buf, n);
		}
		php_stream_close(stream);
	} else {
		ops->hash_update(context, (unsigned char *) data, data_len);
	}

	digest = zend_string_alloc(ops->digest_size, 0);
	ops->hash_final((unsigned char *) digest->val, context);
	efree(context);

	if (raw_output) {
		digest->val[ops->digest_size] = 0;
		RETURN_STR(digest);
	} else {
		zend_string *hex_digest = zend_string_safe_alloc(ops->digest_size, 2, 0, 0);

		php_hash_bin2hex(hex_digest->val, (unsigned char *) digest->val, ops->digest_size);
		hex_digest->val[2 * ops->digest_size] = 0;
		zend_string_release(digest);
		RETURN_STR(hex_digest);
	}
}
/* }}} */

/* {{{ proto string hash(string algo, string data[, bool raw_output = false])
Generate a hash of a given input string
Returns lowercase hexits by default */
PHP_FUNCTION(hash)
{
	php_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto string hash_file(string algo, string filename[, bool raw_output = false])
Generate a hash of a given file
Returns lowercase hexits by default */
PHP_FUNCTION(hash_file)
{
	php_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

static inline void php_hash_string_xor_char(unsigned char *out, const unsigned char *in, const unsigned char xor_with, const int length) {
	int i;
	for (i=0; i < length; i++) {
		out[i] = in[i] ^ xor_with;
	}
}

static inline void php_hash_string_xor(unsigned char *out, const unsigned char *in, const unsigned char *xor_with, const int length) {
	int i;
	for (i=0; i < length; i++) {
		out[i] = in[i] ^ xor_with[i];
	}
}

static inline void php_hash_hmac_prep_key(unsigned char *K, const php_hash_ops *ops, void *context, const unsigned char *key, const size_t key_len) {
	memset(K, 0, ops->block_size);
	if (key_len > ops->block_size) {
		/* Reduce the key first */
		ops->hash_init(context);
		ops->hash_update(context, key, key_len);
		ops->hash_final(K, context);
	} else {
		memcpy(K, key, key_len);
	}
	/* XOR the key with 0x36 to get the ipad) */
	php_hash_string_xor_char(K, K, 0x36, ops->block_size);
}

static inline void php_hash_hmac_round(unsigned char *final, const php_hash_ops *ops, void *context, const unsigned char *key, const unsigned char *data, const zend_long data_size) {
	ops->hash_init(context);
	ops->hash_update(context, key, ops->block_size);
	ops->hash_update(context, data, data_size);
	ops->hash_final(final, context);
}

static void php_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAMETERS, int isfilename, zend_bool raw_output_default) /* {{{ */
{
	zend_string *digest;
	char *algo, *data, *key;
	unsigned char *K;
	size_t algo_len, data_len, key_len;
	zend_bool raw_output = raw_output_default;
	const php_hash_ops *ops;
	void *context;
	php_stream *stream = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|b", &algo, &algo_len, &data, &data_len,
																  &key, &key_len, &raw_output) == FAILURE) {
		return;
	}

	ops = php_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		php_error_docref(NULL, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}
	if (isfilename) {
		stream = php_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS, NULL, DEFAULT_CONTEXT);
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = emalloc(ops->context_size);

	K = emalloc(ops->block_size);
	digest = zend_string_alloc(ops->digest_size, 0);

	php_hash_hmac_prep_key(K, ops, context, (unsigned char *) key, key_len);

	if (isfilename) {
		char buf[1024];
		int n;
		ops->hash_init(context);
		ops->hash_update(context, K, ops->block_size);
		while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, (unsigned char *) buf, n);
		}
		php_stream_close(stream);
		ops->hash_final((unsigned char *) digest->val, context);
	} else {
		php_hash_hmac_round((unsigned char *) digest->val, ops, context, K, (unsigned char *) data, data_len);
	}

	php_hash_string_xor_char(K, K, 0x6A, ops->block_size);

	php_hash_hmac_round((unsigned char *) digest->val, ops, context, K, (unsigned char *) digest->val, ops->digest_size);

	/* Zero the key */
	ZEND_SECURE_ZERO(K, ops->block_size);
	efree(K);
	efree(context);

	if (raw_output) {
		digest->val[ops->digest_size] = 0;
		RETURN_STR(digest);
	} else {
		zend_string *hex_digest = zend_string_safe_alloc(ops->digest_size, 2, 0, 0);

		php_hash_bin2hex(hex_digest->val, (unsigned char *) digest->val, ops->digest_size);
		hex_digest->val[2 * ops->digest_size] = 0;
		zend_string_release(digest);
		RETURN_STR(hex_digest);
	}
}
/* }}} */

/* {{{ proto string hash_hmac(string algo, string data, string key[, bool raw_output = false])
Generate a hash of a given input string with a key using HMAC
Returns lowercase hexits by default */
PHP_FUNCTION(hash_hmac)
{
	php_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto string hash_hmac_file(string algo, string filename, string key[, bool raw_output = false])
Generate a hash of a given file with a key using HMAC
Returns lowercase hexits by default */
PHP_FUNCTION(hash_hmac_file)
{
	php_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */


/* {{{ proto resource hash_init(string algo[, int options, string key])
Initialize a hashing context */
PHP_FUNCTION(hash_init)
{
	char *algo, *key = NULL;
	size_t algo_len, key_len = 0;
	int argc = ZEND_NUM_ARGS();
	zend_long options = 0;
	void *context;
	const php_hash_ops *ops;
	php_hash_data *hash;

	if (zend_parse_parameters(argc, "s|ls", &algo, &algo_len, &options, &key, &key_len) == FAILURE) {
		return;
	}

	ops = php_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		php_error_docref(NULL, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}

	if (options & PHP_HASH_HMAC &&
		key_len <= 0) {
		/* Note: a zero length key is no key at all */
		php_error_docref(NULL, E_WARNING, "HMAC requested without a key");
		RETURN_FALSE;
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	hash = emalloc(sizeof(php_hash_data));
	hash->ops = ops;
	hash->context = context;
	hash->options = options;
	hash->key = NULL;

	if (options & PHP_HASH_HMAC) {
		char *K = emalloc(ops->block_size);
		int i;

		memset(K, 0, ops->block_size);

		if (key_len > ops->block_size) {
			/* Reduce the key first */
			ops->hash_update(context, (unsigned char *) key, key_len);
			ops->hash_final((unsigned char *) K, context);
			/* Make the context ready to start over */
			ops->hash_init(context);
		} else {
			memcpy(K, key, key_len);
		}

		/* XOR ipad */
		for(i=0; i < ops->block_size; i++) {
			K[i] ^= 0x36;
		}
		ops->hash_update(context, (unsigned char *) K, ops->block_size);
		hash->key = (unsigned char *) K;
	}

	RETURN_RES(zend_register_resource(hash, php_hash_le_hash));
}
/* }}} */

/* {{{ proto bool hash_update(resource context, string data)
Pump data into the hashing algorithm */
PHP_FUNCTION(hash_update)
{
	zval *zhash;
	php_hash_data *hash;
	char *data;
	size_t data_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs", &zhash, &data, &data_len) == FAILURE) {
		return;
	}

	if ((hash = (php_hash_data *)zend_fetch_resource(Z_RES_P(zhash), PHP_HASH_RESNAME, php_hash_le_hash)) == NULL) {
		RETURN_FALSE;
	}

	hash->ops->hash_update(hash->context, (unsigned char *) data, data_len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int hash_update_stream(resource context, resource handle[, integer length])
Pump data into the hashing algorithm from an open stream */
PHP_FUNCTION(hash_update_stream)
{
	zval *zhash, *zstream;
	php_hash_data *hash;
	php_stream *stream = NULL;
	zend_long length = -1, didread = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rr|l", &zhash, &zstream, &length) == FAILURE) {
		return;
	}

	if ((hash = (php_hash_data *)zend_fetch_resource(Z_RES_P(zhash), PHP_HASH_RESNAME, php_hash_le_hash)) == NULL) {
		RETURN_FALSE;
	}

	php_stream_from_zval(stream, zstream);

	while (length) {
		char buf[1024];
		zend_long n, toread = 1024;

		if (length > 0 && toread > length) {
			toread = length;
		}

		if ((n = php_stream_read(stream, buf, toread)) <= 0) {
			/* Nada mas */
			RETURN_LONG(didread);
		}
		hash->ops->hash_update(hash->context, (unsigned char *) buf, n);
		length -= n;
		didread += n;
	}

	RETURN_LONG(didread);
}
/* }}} */

/* {{{ proto bool hash_update_file(resource context, string filename[, resource context])
Pump data into the hashing algorithm from a file */
PHP_FUNCTION(hash_update_file)
{
	zval *zhash, *zcontext = NULL;
	php_hash_data *hash;
	php_stream_context *context;
	php_stream *stream;
	char *filename, buf[1024];
	size_t filename_len, n;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "rs|r", &zhash, &filename, &filename_len, &zcontext) == FAILURE) {
		return;
	}

	if ((hash = (php_hash_data *)zend_fetch_resource(Z_RES_P(zhash), PHP_HASH_RESNAME, php_hash_le_hash)) == NULL) {
		RETURN_FALSE;
	}
	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(filename, "rb", REPORT_ERRORS, NULL, context);
	if (!stream) {
		/* Stream will report errors opening file */
		RETURN_FALSE;
	}

	while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
		hash->ops->hash_update(hash->context, (unsigned char *) buf, n);
	}
	php_stream_close(stream);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hash_final(resource context[, bool raw_output=false])
Output resulting digest */
PHP_FUNCTION(hash_final)
{
	zval *zhash;
	php_hash_data *hash;
	zend_bool raw_output = 0;
	zend_string *digest;
	int digest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r|b", &zhash, &raw_output) == FAILURE) {
		return;
	}

	if ((hash = (php_hash_data *)zend_fetch_resource(Z_RES_P(zhash), PHP_HASH_RESNAME, php_hash_le_hash)) == NULL) {
		RETURN_FALSE;
	}

	digest_len = hash->ops->digest_size;
	digest = zend_string_alloc(digest_len, 0);
	hash->ops->hash_final((unsigned char *) digest->val, hash->context);
	if (hash->options & PHP_HASH_HMAC) {
		int i;

		/* Convert K to opad -- 0x6A = 0x36 ^ 0x5C */
		for(i=0; i < hash->ops->block_size; i++) {
			hash->key[i] ^= 0x6A;
		}

		/* Feed this result into the outter hash */
		hash->ops->hash_init(hash->context);
		hash->ops->hash_update(hash->context, hash->key, hash->ops->block_size);
		hash->ops->hash_update(hash->context, (unsigned char *) digest->val, hash->ops->digest_size);
		hash->ops->hash_final((unsigned char *) digest->val, hash->context);

		/* Zero the key */
		ZEND_SECURE_ZERO(hash->key, hash->ops->block_size);
		efree(hash->key);
		hash->key = NULL;
	}
	digest->val[digest_len] = 0;
	efree(hash->context);
	hash->context = NULL;

	//???
	//??? /* zend_list_REAL_delete() */
	//??? if (zend_hash_index_find(&EG(regular_list), Z_RESVAL_P(zhash), (void *) &le)==SUCCESS) {
	//??? 	/* This is a hack to avoid letting the resource hide elsewhere (like in separated vars)
	//??? 		FETCH_RESOURCE is intelligent enough to handle dealing with any issues this causes */
	//??? 	le->refcount = 1;
	//??? } /* FAILURE is not an option */
	zend_list_close(Z_RES_P(zhash));

	if (raw_output) {
		RETURN_STR(digest);
	} else {
		zend_string *hex_digest = zend_string_safe_alloc(digest_len, 2, 0, 0);

		php_hash_bin2hex(hex_digest->val, (unsigned char *) digest->val, digest_len);
		hex_digest->val[2 * digest_len] = 0;
		zend_string_release(digest);
		RETURN_STR(hex_digest);
	}
}
/* }}} */

/* {{{ proto resource hash_copy(resource context)
Copy hash resource */
PHP_FUNCTION(hash_copy)
{
	zval *zhash;
	php_hash_data *hash, *copy_hash;
	void *context;
	int res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &zhash) == FAILURE) {
		return;
	}

	if ((hash = (php_hash_data *)zend_fetch_resource(Z_RES_P(zhash), PHP_HASH_RESNAME, php_hash_le_hash)) == NULL) {
		RETURN_FALSE;
	}


	context = emalloc(hash->ops->context_size);
	hash->ops->hash_init(context);

	res = hash->ops->hash_copy(hash->ops, hash->context, context);
	if (res != SUCCESS) {
		efree(context);
		RETURN_FALSE;
	}

	copy_hash = emalloc(sizeof(php_hash_data));
	copy_hash->ops = hash->ops;
	copy_hash->context = context;
	copy_hash->options = hash->options;
	copy_hash->key = ecalloc(1, hash->ops->block_size);
	if (hash->key) {
		memcpy(copy_hash->key, hash->key, hash->ops->block_size);
	}
	RETURN_RES(zend_register_resource(copy_hash, php_hash_le_hash));
}
/* }}} */

/* {{{ proto array hash_algos(void)
Return a list of registered hashing algorithms */
PHP_FUNCTION(hash_algos)
{
	zend_string *str;

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY(&php_hash_hashtable, str) {
		add_next_index_str(return_value, zend_string_copy(str));
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto string hash_pbkdf2(string algo, string password, string salt, int iterations [, int length = 0, bool raw_output = false])
Generate a PBKDF2 hash of the given password and salt
Returns lowercase hexits by default */
PHP_FUNCTION(hash_pbkdf2)
{
	zend_string *returnval;
	char *algo, *salt, *pass = NULL;
	unsigned char *computed_salt, *digest, *temp, *result, *K1, *K2 = NULL;
	zend_long loops, i, j, iterations, digest_length = 0, length = 0;
	size_t algo_len, pass_len, salt_len = 0;
	zend_bool raw_output = 0;
	const php_hash_ops *ops;
	void *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sssl|lb", &algo, &algo_len, &pass, &pass_len, &salt, &salt_len, &iterations, &length, &raw_output) == FAILURE) {
		return;
	}

	ops = php_hash_fetch_ops(algo, algo_len);
	if (!ops) {
		php_error_docref(NULL, E_WARNING, "Unknown hashing algorithm: %s", algo);
		RETURN_FALSE;
	}

	if (iterations <= 0) {
		php_error_docref(NULL, E_WARNING, "Iterations must be a positive integer: " ZEND_LONG_FMT, iterations);
		RETURN_FALSE;
	}

	if (length < 0) {
		php_error_docref(NULL, E_WARNING, "Length must be greater than or equal to 0: " ZEND_LONG_FMT, length);
		RETURN_FALSE;
	}

	if (salt_len > INT_MAX - 4) {
		php_error_docref(NULL, E_WARNING, "Supplied salt is too long, max of INT_MAX - 4 bytes: %zd supplied", salt_len);
		RETURN_FALSE;
	}

	context = emalloc(ops->context_size);
	ops->hash_init(context);

	K1 = emalloc(ops->block_size);
	K2 = emalloc(ops->block_size);
	digest = emalloc(ops->digest_size);
	temp = emalloc(ops->digest_size);

	/* Setup Keys that will be used for all hmac rounds */
	php_hash_hmac_prep_key(K1, ops, context, (unsigned char *) pass, pass_len);
	/* Convert K1 to opad -- 0x6A = 0x36 ^ 0x5C */
	php_hash_string_xor_char(K2, K1, 0x6A, ops->block_size);

	/* Setup Main Loop to build a long enough result */
	if (length == 0) {
		length = ops->digest_size;
		if (!raw_output) {
			length = length * 2;
		}
	}
	digest_length = length;
	if (!raw_output) {
		digest_length = (zend_long) ceil((float) length / 2.0);
	}

	loops = (zend_long) ceil((float) digest_length / (float) ops->digest_size);

	result = safe_emalloc(loops, ops->digest_size, 0);

	computed_salt = safe_emalloc(salt_len, 1, 4);
	memcpy(computed_salt, (unsigned char *) salt, salt_len);

	for (i = 1; i <= loops; i++) {
		/* digest = hash_hmac(salt + pack('N', i), password) { */

		/* pack("N", i) */
		computed_salt[salt_len] = (unsigned char) (i >> 24);
		computed_salt[salt_len + 1] = (unsigned char) ((i & 0xFF0000) >> 16);
		computed_salt[salt_len + 2] = (unsigned char) ((i & 0xFF00) >> 8);
		computed_salt[salt_len + 3] = (unsigned char) (i & 0xFF);

		php_hash_hmac_round(digest, ops, context, K1, computed_salt, (zend_long) salt_len + 4);
		php_hash_hmac_round(digest, ops, context, K2, digest, ops->digest_size);
		/* } */

		/* temp = digest */
		memcpy(temp, digest, ops->digest_size);

		/*
		 * Note that the loop starting at 1 is intentional, since we've already done
		 * the first round of the algorithm.
		 */
		for (j = 1; j < iterations; j++) {
			/* digest = hash_hmac(digest, password) { */
			php_hash_hmac_round(digest, ops, context, K1, digest, ops->digest_size);
			php_hash_hmac_round(digest, ops, context, K2, digest, ops->digest_size);
			/* } */
			/* temp ^= digest */
			php_hash_string_xor(temp, temp, digest, ops->digest_size);
		}
		/* result += temp */
		memcpy(result + ((i - 1) * ops->digest_size), temp, ops->digest_size);
	}
	/* Zero potentially sensitive variables */
	ZEND_SECURE_ZERO(K1, ops->block_size);
	ZEND_SECURE_ZERO(K2, ops->block_size);
	ZEND_SECURE_ZERO(computed_salt, salt_len + 4);
	efree(K1);
	efree(K2);
	efree(computed_salt);
	efree(context);
	efree(digest);
	efree(temp);

	returnval = zend_string_alloc(length, 0);
	if (raw_output) {
		memcpy(returnval->val, result, length);
	} else {
		php_hash_bin2hex(returnval->val, result, digest_length);
	}
	returnval->val[length] = 0;
	efree(result);
	RETURN_STR(returnval);
}
/* }}} */

/* {{{ proto bool hash_equals(string known_string, string user_string)
   Compares two strings using the same time whether they're equal or not.
   A difference in length will leak */
PHP_FUNCTION(hash_equals)
{
	zval *known_zval, *user_zval;
	char *known_str, *user_str;
	int result = 0, j;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &known_zval, &user_zval) == FAILURE) {
		return;
	}

	/* We only allow comparing string to prevent unexpected results. */
	if (Z_TYPE_P(known_zval) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expected known_string to be a string, %s given", zend_zval_type_name(known_zval));
		RETURN_FALSE;
	}

	if (Z_TYPE_P(user_zval) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Expected user_string to be a string, %s given", zend_zval_type_name(user_zval));
		RETURN_FALSE;
	}

	if (Z_STRLEN_P(known_zval) != Z_STRLEN_P(user_zval)) {
		RETURN_FALSE;
	}

	known_str = Z_STRVAL_P(known_zval);
	user_str = Z_STRVAL_P(user_zval);

	/* This is security sensitive code. Do not optimize this for speed. */
	for (j = 0; j < Z_STRLEN_P(known_zval); j++) {
		result |= known_str[j] ^ user_str[j];
	}

	RETURN_BOOL(0 == result);
}
/* }}} */

/* Module Housekeeping */

static void php_hash_dtor(zend_resource *rsrc) /* {{{ */
{
	php_hash_data *hash = (php_hash_data*)rsrc->ptr;

	/* Just in case the algo has internally allocated resources */
	if (hash->context) {
		unsigned char *dummy = emalloc(hash->ops->digest_size);
		hash->ops->hash_final(dummy, hash->context);
		efree(dummy);
		efree(hash->context);
	}

	if (hash->key) {
		memset(hash->key, 0, hash->ops->block_size);
		efree(hash->key);
	}
	efree(hash);
}
/* }}} */

#define PHP_HASH_HAVAL_REGISTER(p,b)	php_hash_register_algo("haval" #b "," #p , &php_hash_##p##haval##b##_ops);

#ifdef PHP_MHASH_BC

PHP_MINFO_FUNCTION(mhash)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "MHASH support", "Enabled");
	php_info_print_table_row(2, "MHASH API Version", "Emulated Support");
	php_info_print_table_end();
}

zend_module_entry mhash_module_entry = {
	STANDARD_MODULE_HEADER,
	"mhash",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(mhash),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES,
};

static void mhash_init(INIT_FUNC_ARGS)
{
	char buf[128];
	int len;
	int algo_number = 0;

	for (algo_number = 0; algo_number < MHASH_NUM_ALGOS; algo_number++) {
		struct mhash_bc_entry algorithm = mhash_to_hash[algo_number];
		if (algorithm.mhash_name == NULL) {
			continue;
		}

		len = slprintf(buf, 127, "MHASH_%s", algorithm.mhash_name, strlen(algorithm.mhash_name));
		zend_register_long_constant(buf, len, algorithm.value, CONST_CS | CONST_PERSISTENT, module_number);
	}
	zend_register_internal_module(&mhash_module_entry);
}

/* {{{ proto string mhash(int hash, string data [, string key])
   Hash data with hash */
PHP_FUNCTION(mhash)
{
	zval *z_algorithm;
	zend_long algorithm;

	if (zend_parse_parameters(1, "z", &z_algorithm) == FAILURE) {
		return;
	}

	SEPARATE_ZVAL(z_algorithm);
	convert_to_long_ex(z_algorithm);
	algorithm = Z_LVAL_P(z_algorithm);

	/* need to convert the first parameter from int constant to string algorithm name */
	if (algorithm >= 0 && algorithm < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.hash_name) {
			ZVAL_STRING(z_algorithm, algorithm_lookup.hash_name);
		}
	}

	if (ZEND_NUM_ARGS() == 3) {
		php_hash_do_hash_hmac(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
	} else if (ZEND_NUM_ARGS() == 2) {
		php_hash_do_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto string mhash_get_hash_name(int hash)
   Gets the name of hash */
PHP_FUNCTION(mhash_get_hash_name)
{
	zend_long algorithm;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &algorithm) == FAILURE) {
		return;
	}

	if (algorithm >= 0 && algorithm  < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			RETURN_STRING(algorithm_lookup.mhash_name);
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int mhash_count(void)
   Gets the number of available hashes */
PHP_FUNCTION(mhash_count)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_LONG(MHASH_NUM_ALGOS - 1);
}
/* }}} */

/* {{{ proto int mhash_get_block_size(int hash)
   Gets the block size of hash */
PHP_FUNCTION(mhash_get_block_size)
{
	zend_long algorithm;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &algorithm) == FAILURE) {
		return;
	}
	RETVAL_FALSE;

	if (algorithm >= 0 && algorithm  < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			const php_hash_ops *ops = php_hash_fetch_ops(algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name));
			if (ops) {
				RETVAL_LONG(ops->digest_size);
			}
		}
	}
}
/* }}} */

#define SALT_SIZE 8

/* {{{ proto string mhash_keygen_s2k(int hash, string input_password, string salt, int bytes)
   Generates a key using hash functions */
PHP_FUNCTION(mhash_keygen_s2k)
{
	zend_long algorithm, l_bytes;
	int bytes;
	char *password, *salt;
	size_t password_len, salt_len;
	char padded_salt[SALT_SIZE];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lssl", &algorithm, &password, &password_len, &salt, &salt_len, &l_bytes) == FAILURE) {
		return;
	}

	bytes = (int)l_bytes;
	if (bytes <= 0){
		php_error_docref(NULL, E_WARNING, "the byte parameter must be greater than 0");
		RETURN_FALSE;
	}

	salt_len = MIN(salt_len, SALT_SIZE);

	memcpy(padded_salt, salt, salt_len);
	if (salt_len < SALT_SIZE) {
		memset(padded_salt + salt_len, 0, SALT_SIZE - salt_len);
	}
	salt_len = SALT_SIZE;

	RETVAL_FALSE;
	if (algorithm >= 0 && algorithm < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			const php_hash_ops *ops = php_hash_fetch_ops(algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name));
			if (ops) {
				unsigned char null = '\0';
				void *context;
				char *key, *digest;
				int i = 0, j = 0;
				int block_size = ops->digest_size;
				int times = bytes / block_size;
				if (bytes % block_size  != 0) times++;

				context = emalloc(ops->context_size);
				ops->hash_init(context);

				key = ecalloc(1, times * block_size);
				digest = emalloc(ops->digest_size + 1);

				for (i = 0; i < times; i++) {
					ops->hash_init(context);

					for (j=0;j<i;j++) {
						ops->hash_update(context, &null, 1);
					}
					ops->hash_update(context, (unsigned char *)padded_salt, salt_len);
					ops->hash_update(context, (unsigned char *)password, password_len);
					ops->hash_final((unsigned char *)digest, context);
					memcpy( &key[i*block_size], digest, block_size);
				}

				RETVAL_STRINGL(key, bytes);
				ZEND_SECURE_ZERO(key, bytes);
				efree(digest);
				efree(context);
				efree(key);
			}
		}
	}
}
/* }}} */

#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hash)
{
	php_hash_le_hash = zend_register_list_destructors_ex(php_hash_dtor, NULL, PHP_HASH_RESNAME, module_number);

	zend_hash_init(&php_hash_hashtable, 35, NULL, NULL, 1);

	php_hash_register_algo("md2",			&php_hash_md2_ops);
	php_hash_register_algo("md4",			&php_hash_md4_ops);
	php_hash_register_algo("md5",			&php_hash_md5_ops);
	php_hash_register_algo("sha1",			&php_hash_sha1_ops);
	php_hash_register_algo("sha224",		&php_hash_sha224_ops);
	php_hash_register_algo("sha256",		&php_hash_sha256_ops);
	php_hash_register_algo("sha384",		&php_hash_sha384_ops);
	php_hash_register_algo("sha512",		&php_hash_sha512_ops);
	php_hash_register_algo("ripemd128",		&php_hash_ripemd128_ops);
	php_hash_register_algo("ripemd160",		&php_hash_ripemd160_ops);
	php_hash_register_algo("ripemd256",		&php_hash_ripemd256_ops);
	php_hash_register_algo("ripemd320",		&php_hash_ripemd320_ops);
	php_hash_register_algo("whirlpool",		&php_hash_whirlpool_ops);
	php_hash_register_algo("tiger128,3",	&php_hash_3tiger128_ops);
	php_hash_register_algo("tiger160,3",	&php_hash_3tiger160_ops);
	php_hash_register_algo("tiger192,3",	&php_hash_3tiger192_ops);
	php_hash_register_algo("tiger128,4",	&php_hash_4tiger128_ops);
	php_hash_register_algo("tiger160,4",	&php_hash_4tiger160_ops);
	php_hash_register_algo("tiger192,4",	&php_hash_4tiger192_ops);
	php_hash_register_algo("snefru",		&php_hash_snefru_ops);
	php_hash_register_algo("snefru256",		&php_hash_snefru_ops);
	php_hash_register_algo("gost",			&php_hash_gost_ops);
	php_hash_register_algo("gost-crypto",		&php_hash_gost_crypto_ops);
	php_hash_register_algo("adler32",		&php_hash_adler32_ops);
	php_hash_register_algo("crc32",			&php_hash_crc32_ops);
	php_hash_register_algo("crc32b",		&php_hash_crc32b_ops);
	php_hash_register_algo("fnv132",		&php_hash_fnv132_ops);
	php_hash_register_algo("fnv1a32",		&php_hash_fnv1a32_ops);
	php_hash_register_algo("fnv164",		&php_hash_fnv164_ops);
	php_hash_register_algo("fnv1a64",		&php_hash_fnv1a64_ops);
	php_hash_register_algo("joaat",			&php_hash_joaat_ops);

	PHP_HASH_HAVAL_REGISTER(3,128);
	PHP_HASH_HAVAL_REGISTER(3,160);
	PHP_HASH_HAVAL_REGISTER(3,192);
	PHP_HASH_HAVAL_REGISTER(3,224);
	PHP_HASH_HAVAL_REGISTER(3,256);

	PHP_HASH_HAVAL_REGISTER(4,128);
	PHP_HASH_HAVAL_REGISTER(4,160);
	PHP_HASH_HAVAL_REGISTER(4,192);
	PHP_HASH_HAVAL_REGISTER(4,224);
	PHP_HASH_HAVAL_REGISTER(4,256);

	PHP_HASH_HAVAL_REGISTER(5,128);
	PHP_HASH_HAVAL_REGISTER(5,160);
	PHP_HASH_HAVAL_REGISTER(5,192);
	PHP_HASH_HAVAL_REGISTER(5,224);
	PHP_HASH_HAVAL_REGISTER(5,256);

	REGISTER_LONG_CONSTANT("HASH_HMAC",		PHP_HASH_HMAC,	CONST_CS | CONST_PERSISTENT);

#ifdef PHP_MHASH_BC
	mhash_init(INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hash)
{
	zend_hash_destroy(&php_hash_hashtable);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hash)
{
	char buffer[2048];
	zend_string *str;
	char *s = buffer, *e = s + sizeof(buffer);

	ZEND_HASH_FOREACH_STR_KEY(&php_hash_hashtable, str) {
		s += slprintf(s, e - s, "%s ", str->val);
	} ZEND_HASH_FOREACH_END();
	*s = 0;

	php_info_print_table_start();
	php_info_print_table_row(2, "hash support", "enabled");
	php_info_print_table_row(2, "Hashing Engines", buffer);
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo */
#ifdef PHP_HASH_MD5_NOT_IN_CORE
ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_md5, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_md5_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()
#endif

#ifdef PHP_HASH_SHA1_NOT_IN_CORE
ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_sha1, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_sha1_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash, 0, 0, 2)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_file, 0, 0, 2)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_hmac, 0, 0, 3)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_hmac_file, 0, 0, 3)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_init, 0, 0, 1)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_update, 0)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_update_stream, 0, 0, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, handle)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_update_file, 0, 0, 2)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_final, 0, 0, 1)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_copy, 0)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_algos, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_pbkdf2, 0, 0, 4)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, salt)
	ZEND_ARG_INFO(0, iterations)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hash_equals, 0)
	ZEND_ARG_INFO(0, known_string)
	ZEND_ARG_INFO(0, user_string)
ZEND_END_ARG_INFO()

/* BC Land */
#ifdef PHP_MHASH_BC
ZEND_BEGIN_ARG_INFO(arginfo_mhash_get_block_size, 0)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mhash_get_hash_name, 0)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mhash_keygen_s2k, 0)
	ZEND_ARG_INFO(0, hash)
	ZEND_ARG_INFO(0, input_password)
	ZEND_ARG_INFO(0, salt)
	ZEND_ARG_INFO(0, bytes)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mhash_count, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mhash, 0, 0, 2)
	ZEND_ARG_INFO(0, hash)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()
#endif

/* }}} */

/* {{{ hash_functions[]
 */
const zend_function_entry hash_functions[] = {
	PHP_FE(hash,									arginfo_hash)
	PHP_FE(hash_file,								arginfo_hash_file)

	PHP_FE(hash_hmac,								arginfo_hash_hmac)
	PHP_FE(hash_hmac_file,							arginfo_hash_hmac_file)

	PHP_FE(hash_init,								arginfo_hash_init)
	PHP_FE(hash_update,								arginfo_hash_update)
	PHP_FE(hash_update_stream,						arginfo_hash_update_stream)
	PHP_FE(hash_update_file,						arginfo_hash_update_file)
	PHP_FE(hash_final,								arginfo_hash_final)
	PHP_FE(hash_copy,								arginfo_hash_copy)

	PHP_FE(hash_algos,								arginfo_hash_algos)
	PHP_FE(hash_pbkdf2,								arginfo_hash_pbkdf2)
	PHP_FE(hash_equals,								arginfo_hash_equals)

	/* BC Land */
#ifdef PHP_HASH_MD5_NOT_IN_CORE
	PHP_NAMED_FE(md5, php_if_md5,					arginfo_hash_md5)
	PHP_NAMED_FE(md5_file, php_if_md5_file,			arginfo_hash_md5_file)
#endif /* PHP_HASH_MD5_NOT_IN_CORE */

#ifdef PHP_HASH_SHA1_NOT_IN_CORE
	PHP_NAMED_FE(sha1, php_if_sha1,					arginfo_hash_sha1)
	PHP_NAMED_FE(sha1_file, php_if_sha1_file,		arginfo_hash_sha1_file)
#endif /* PHP_HASH_SHA1_NOT_IN_CORE */

#ifdef PHP_MHASH_BC
	PHP_FE(mhash_keygen_s2k, arginfo_mhash_keygen_s2k)
	PHP_FE(mhash_get_block_size, arginfo_mhash_get_block_size)
	PHP_FE(mhash_get_hash_name, arginfo_mhash_get_hash_name)
	PHP_FE(mhash_count, arginfo_mhash_count)
	PHP_FE(mhash, arginfo_mhash)
#endif

	PHP_FE_END
};
/* }}} */

/* {{{ hash_module_entry
 */
zend_module_entry hash_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_HASH_EXTNAME,
	hash_functions,
	PHP_MINIT(hash),
	PHP_MSHUTDOWN(hash),
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	PHP_MINFO(hash),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_HASH_EXTVER, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HASH
ZEND_GET_MODULE(hash)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

