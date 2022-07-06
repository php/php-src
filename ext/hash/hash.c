/*
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
  | Author: Sara Golemon <pollita@php.net>                               |
  |         Scott MacVicar <scottmac@php.net>                            |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include "php_hash.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_var.h"
#include "ext/spl/spl_exceptions.h"

#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"

#include "hash_arginfo.h"

#ifdef PHP_WIN32
# define __alignof__ __alignof
#else
# ifndef HAVE_ALIGNOF
#  include <stddef.h>
#  define __alignof__(type) offsetof (struct { char c; type member;}, member)
# endif
#endif

HashTable php_hash_hashtable;
zend_class_entry *php_hashcontext_ce;
static zend_object_handlers php_hashcontext_handlers;

#ifdef PHP_MHASH_BC
struct mhash_bc_entry {
	char *mhash_name;
	char *hash_name;
	int value;
};

#define MHASH_NUM_ALGOS 35

static struct mhash_bc_entry mhash_to_hash[MHASH_NUM_ALGOS] = {
	{"CRC32", "crc32", 0}, /* used by bzip */
	{"MD5", "md5", 1},
	{"SHA1", "sha1", 2},
	{"HAVAL256", "haval256,3", 3},
	{NULL, NULL, 4},
	{"RIPEMD160", "ripemd160", 5},
	{NULL, NULL, 6},
	{"TIGER", "tiger192,3", 7},
	{"GOST", "gost", 8},
	{"CRC32B", "crc32b", 9}, /* used by ethernet (IEEE 802.3), gzip, zip, png, etc */
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
	{"CRC32C", "crc32c", 34}, /* Castagnoli's CRC, used by iSCSI, SCTP, Btrfs, ext4, etc */
};
#endif

/* Hash Registry Access */

PHP_HASH_API const php_hash_ops *php_hash_fetch_ops(zend_string *algo) /* {{{ */
{
	zend_string *lower = zend_string_tolower(algo);
	php_hash_ops *ops = zend_hash_find_ptr(&php_hash_hashtable, lower);
	zend_string_release(lower);

	return ops;
}
/* }}} */

PHP_HASH_API void php_hash_register_algo(const char *algo, const php_hash_ops *ops) /* {{{ */
{
	size_t algo_len = strlen(algo);
	char *lower = zend_str_tolower_dup(algo, algo_len);
	zend_hash_add_ptr(&php_hash_hashtable, zend_string_init_interned(lower, algo_len, 1), (void *) ops);
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


static inline size_t align_to(size_t pos, size_t alignment) {
	size_t offset = pos & (alignment - 1);
	return pos + (offset ? alignment - offset : 0);
}

static size_t parse_serialize_spec(
		const char **specp, size_t *pos, size_t *sz, size_t *max_alignment) {
	size_t count, alignment;
	const char *spec = *specp;
	/* parse size */
	if (*spec == 's' || *spec == 'S') {
		*sz = 2;
		alignment = __alignof__(uint16_t); /* usually 2 */
	} else if (*spec == 'l' || *spec == 'L') {
		*sz = 4;
		alignment = __alignof__(uint32_t); /* usually 4 */
	} else if (*spec == 'q' || *spec == 'Q') {
		*sz = 8;
		alignment = __alignof__(uint64_t); /* usually 8 */
	} else if (*spec == 'i' || *spec == 'I') {
		*sz = sizeof(int);
		alignment = __alignof__(int);      /* usually 4 */
	} else {
		ZEND_ASSERT(*spec == 'b' || *spec == 'B');
		*sz = 1;
		alignment = 1;
	}
	/* process alignment */
	*pos = align_to(*pos, alignment);
	*max_alignment = *max_alignment < alignment ? alignment : *max_alignment;
	/* parse count */
	++spec;
	if (isdigit((unsigned char) *spec)) {
		count = 0;
		while (isdigit((unsigned char) *spec)) {
			count = 10 * count + *spec - '0';
			++spec;
		}
	} else {
		count = 1;
	}
	*specp = spec;
	return count;
}

static uint64_t one_from_buffer(size_t sz, const unsigned char *buf) {
	if (sz == 2) {
		const uint16_t *x = (const uint16_t *) buf;
		return *x;
	} else if (sz == 4) {
		const uint32_t *x = (const uint32_t *) buf;
		return *x;
	} else if (sz == 8) {
		const uint64_t *x = (const uint64_t *) buf;
		return *x;
	} else {
		ZEND_ASSERT(sz == 1);
		return *buf;
	}
}

static void one_to_buffer(size_t sz, unsigned char *buf, uint64_t val) {
	if (sz == 2) {
		uint16_t *x = (uint16_t *) buf;
		*x = val;
	} else if (sz == 4) {
		uint32_t *x = (uint32_t *) buf;
		*x = val;
	} else if (sz == 8) {
		uint64_t *x = (uint64_t *) buf;
		*x = val;
	} else {
		ZEND_ASSERT(sz == 1);
		*buf = val;
	}
}

/* Serialize a hash context according to a `spec` string.
   Spec contents:
   b[COUNT] -- serialize COUNT bytes
   s[COUNT] -- serialize COUNT 16-bit integers
   l[COUNT] -- serialize COUNT 32-bit integers
   q[COUNT] -- serialize COUNT 64-bit integers
   i[COUNT] -- serialize COUNT `int`s
   B[COUNT] -- skip COUNT bytes
   S[COUNT], L[COUNT], etc. -- uppercase versions skip instead of read
   . (must be last character) -- assert that the hash context has exactly
       this size
   Example: "llllllb64l16." is the spec for an MD5 context: 6 32-bit
   integers, followed by 64 bytes, then 16 32-bit integers, and that's
   exactly the size of the context.

   The serialization result is an array. Each integer is serialized as a
   32-bit integer, except that a run of 2 or more bytes is encoded as a
   string, and each 64-bit integer is serialized as two 32-bit integers, least
   significant bits first. This allows 32-bit and 64-bit architectures to
   interchange serialized HashContexts. */

PHP_HASH_API int php_hash_serialize_spec(const php_hashcontext_object *hash, zval *zv, const char *spec) /* {{{ */
{
	size_t pos = 0, max_alignment = 1;
	unsigned char *buf = (unsigned char *) hash->context;
	zval tmp;
	if (buf == NULL) {
		return FAILURE;
	}
	array_init(zv);
	while (*spec != '\0' && *spec != '.') {
		char spec_ch = *spec;
		size_t sz, count = parse_serialize_spec(&spec, &pos, &sz, &max_alignment);
		if (pos + count * sz > hash->ops->context_size) {
			return FAILURE;
		}
		if (isupper((unsigned char) spec_ch)) {
			pos += count * sz;
		} else if (sz == 1 && count > 1) {
			ZVAL_STRINGL(&tmp, (char *) buf + pos, count);
			zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);
			pos += count;
		} else {
			while (count > 0) {
				uint64_t val = one_from_buffer(sz, buf + pos);
				pos += sz;
				ZVAL_LONG(&tmp, (int32_t) val);
				zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);
				if (sz == 8) {
					ZVAL_LONG(&tmp, (int32_t) (val >> 32));
					zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);
				}
				--count;
			}
		}
	}
	if (*spec == '.' && align_to(pos, max_alignment) != hash->ops->context_size) {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* Unserialize a hash context serialized by `php_hash_serialize_spec` with `spec`.
   Returns SUCCESS on success and a negative error code on failure.
   Codes: FAILURE (-1) == generic failure
   -999 == spec wrong size for context
   -1000 - POS == problem at byte offset POS */

PHP_HASH_API int php_hash_unserialize_spec(php_hashcontext_object *hash, const zval *zv, const char *spec) /* {{{ */
{
	size_t pos = 0, max_alignment = 1, j = 0;
	unsigned char *buf = (unsigned char *) hash->context;
	zval *elt;
	if (Z_TYPE_P(zv) != IS_ARRAY) {
		return FAILURE;
	}
	while (*spec != '\0' && *spec != '.') {
		char spec_ch = *spec;
		size_t sz, count = parse_serialize_spec(&spec, &pos, &sz, &max_alignment);
		if (pos + count * sz > hash->ops->context_size) {
			return -999;
		}
		if (isupper((unsigned char) spec_ch)) {
			pos += count * sz;
		} else if (sz == 1 && count > 1) {
			elt = zend_hash_index_find(Z_ARRVAL_P(zv), j);
			if (!elt || Z_TYPE_P(elt) != IS_STRING || Z_STRLEN_P(elt) != count) {
				return -1000 - pos;
			}
			++j;
			memcpy(buf + pos, Z_STRVAL_P(elt), count);
			pos += count;
		} else {
			while (count > 0) {
				uint64_t val;
				elt = zend_hash_index_find(Z_ARRVAL_P(zv), j);
				if (!elt || Z_TYPE_P(elt) != IS_LONG) {
					return -1000 - pos;
				}
				++j;
				val = (uint32_t) Z_LVAL_P(elt);
				if (sz == 8) {
					elt = zend_hash_index_find(Z_ARRVAL_P(zv), j);
					if (!elt || Z_TYPE_P(elt) != IS_LONG) {
						return -1000 - pos;
					}
					++j;
					val += ((uint64_t) Z_LVAL_P(elt)) << 32;
				}
				one_to_buffer(sz, buf + pos, val);
				pos += sz;
				--count;
			}
		}
	}
	if (*spec == '.' && align_to(pos, max_alignment) != hash->ops->context_size) {
		return -999;
	}
	return SUCCESS;
}
/* }}} */

PHP_HASH_API int php_hash_serialize(const php_hashcontext_object *hash, zend_long *magic, zval *zv) /* {{{ */
{
	if (hash->ops->serialize_spec) {
		*magic = PHP_HASH_SERIALIZE_MAGIC_SPEC;
		return php_hash_serialize_spec(hash, zv, hash->ops->serialize_spec);
	} else {
		return FAILURE;
	}
}
/* }}} */

PHP_HASH_API int php_hash_unserialize(php_hashcontext_object *hash, zend_long magic, const zval *zv) /* {{{ */
{
	if (hash->ops->serialize_spec
		&& magic == PHP_HASH_SERIALIZE_MAGIC_SPEC) {
		return php_hash_unserialize_spec(hash, zv, hash->ops->serialize_spec);
	} else {
		return FAILURE;
	}
}
/* }}} */

/* Userspace */

static void php_hash_do_hash(
	zval *return_value, zend_string *algo, char *data, size_t data_len, zend_bool raw_output, bool isfilename
) /* {{{ */ {
	zend_string *digest;
	const php_hash_ops *ops;
	void *context;
	php_stream *stream = NULL;

	ops = php_hash_fetch_ops(algo);
	if (!ops) {
		zend_argument_value_error(1, "must be a valid hashing algorithm");
		RETURN_THROWS();
	}
	if (isfilename) {
		if (CHECK_NULL_PATH(data, data_len)) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		stream = php_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS, NULL, FG(default_context));
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = php_hash_alloc_context(ops);
	ops->hash_init(context);

	if (isfilename) {
		char buf[1024];
		ssize_t n;

		while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, (unsigned char *) buf, n);
		}
		php_stream_close(stream);
		if (n < 0) {
			efree(context);
			RETURN_FALSE;
		}
	} else {
		ops->hash_update(context, (unsigned char *) data, data_len);
	}

	digest = zend_string_alloc(ops->digest_size, 0);
	ops->hash_final((unsigned char *) ZSTR_VAL(digest), context);
	efree(context);

	if (raw_output) {
		ZSTR_VAL(digest)[ops->digest_size] = 0;
		RETURN_NEW_STR(digest);
	} else {
		zend_string *hex_digest = zend_string_safe_alloc(ops->digest_size, 2, 0, 0);

		php_hash_bin2hex(ZSTR_VAL(hex_digest), (unsigned char *) ZSTR_VAL(digest), ops->digest_size);
		ZSTR_VAL(hex_digest)[2 * ops->digest_size] = 0;
		zend_string_release_ex(digest, 0);
		RETURN_NEW_STR(hex_digest);
	}
}
/* }}} */

/* {{{ Generate a hash of a given input string
Returns lowercase hexits by default */
PHP_FUNCTION(hash)
{
	zend_string *algo;
	char *data;
	size_t data_len;
	zend_bool raw_output = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(algo)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(raw_output)
	ZEND_PARSE_PARAMETERS_END();

	php_hash_do_hash(return_value, algo, data, data_len, raw_output, 0);
}
/* }}} */

/* {{{ Generate a hash of a given file
Returns lowercase hexits by default */
PHP_FUNCTION(hash_file)
{
	zend_string *algo;
	char *data;
	size_t data_len;
	zend_bool raw_output = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(algo)
		Z_PARAM_STRING(data, data_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(raw_output)
	ZEND_PARSE_PARAMETERS_END();

	php_hash_do_hash(return_value, algo, data, data_len, raw_output, 1);
}
/* }}} */

static inline void php_hash_string_xor_char(unsigned char *out, const unsigned char *in, const unsigned char xor_with, const size_t length) {
	size_t i;
	for (i=0; i < length; i++) {
		out[i] = in[i] ^ xor_with;
	}
}

static inline void php_hash_string_xor(unsigned char *out, const unsigned char *in, const unsigned char *xor_with, const size_t length) {
	size_t i;
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

static void php_hash_do_hash_hmac(
	zval *return_value, zend_string *algo, char *data, size_t data_len, char *key, size_t key_len, zend_bool raw_output, bool isfilename
) /* {{{ */ {
	zend_string *digest;
	unsigned char *K;
	const php_hash_ops *ops;
	void *context;
	php_stream *stream = NULL;

	ops = php_hash_fetch_ops(algo);
	if (!ops || !ops->is_crypto) {
		zend_argument_value_error(1, "must be a valid cryptographic hashing algorithm");
		RETURN_THROWS();
	}

	if (isfilename) {
		if (CHECK_NULL_PATH(data, data_len)) {
			zend_argument_value_error(2, "must not contain any null bytes");
			RETURN_THROWS();
		}
		stream = php_stream_open_wrapper_ex(data, "rb", REPORT_ERRORS, NULL, FG(default_context));
		if (!stream) {
			/* Stream will report errors opening file */
			RETURN_FALSE;
		}
	}

	context = php_hash_alloc_context(ops);

	K = emalloc(ops->block_size);
	digest = zend_string_alloc(ops->digest_size, 0);

	php_hash_hmac_prep_key(K, ops, context, (unsigned char *) key, key_len);

	if (isfilename) {
		char buf[1024];
		ssize_t n;
		ops->hash_init(context);
		ops->hash_update(context, K, ops->block_size);
		while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
			ops->hash_update(context, (unsigned char *) buf, n);
		}
		php_stream_close(stream);
		if (n < 0) {
			efree(context);
			efree(K);
			zend_string_release(digest);
			RETURN_FALSE;
		}

		ops->hash_final((unsigned char *) ZSTR_VAL(digest), context);
	} else {
		php_hash_hmac_round((unsigned char *) ZSTR_VAL(digest), ops, context, K, (unsigned char *) data, data_len);
	}

	php_hash_string_xor_char(K, K, 0x6A, ops->block_size);

	php_hash_hmac_round((unsigned char *) ZSTR_VAL(digest), ops, context, K, (unsigned char *) ZSTR_VAL(digest), ops->digest_size);

	/* Zero the key */
	ZEND_SECURE_ZERO(K, ops->block_size);
	efree(K);
	efree(context);

	if (raw_output) {
		ZSTR_VAL(digest)[ops->digest_size] = 0;
		RETURN_NEW_STR(digest);
	} else {
		zend_string *hex_digest = zend_string_safe_alloc(ops->digest_size, 2, 0, 0);

		php_hash_bin2hex(ZSTR_VAL(hex_digest), (unsigned char *) ZSTR_VAL(digest), ops->digest_size);
		ZSTR_VAL(hex_digest)[2 * ops->digest_size] = 0;
		zend_string_release_ex(digest, 0);
		RETURN_NEW_STR(hex_digest);
	}
}
/* }}} */

/* {{{ Generate a hash of a given input string with a key using HMAC
Returns lowercase hexits by default */
PHP_FUNCTION(hash_hmac)
{
	zend_string *algo;
	char *data, *key;
	size_t data_len, key_len;
	zend_bool raw_output = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sss|b", &algo, &data, &data_len, &key, &key_len, &raw_output) == FAILURE) {
		RETURN_THROWS();
	}

	php_hash_do_hash_hmac(return_value, algo, data, data_len, key, key_len, raw_output, 0);
}
/* }}} */

/* {{{ Generate a hash of a given file with a key using HMAC
Returns lowercase hexits by default */
PHP_FUNCTION(hash_hmac_file)
{
	zend_string *algo;
	char *data, *key;
	size_t data_len, key_len;
	zend_bool raw_output = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sss|b", &algo, &data, &data_len, &key, &key_len, &raw_output) == FAILURE) {
		RETURN_THROWS();
	}

	php_hash_do_hash_hmac(return_value, algo, data, data_len, key, key_len, raw_output, 1);
}
/* }}} */

/* {{{ Initialize a hashing context */
PHP_FUNCTION(hash_init)
{
	zend_string *algo, *key = NULL;
	zend_long options = 0;
	void *context;
	const php_hash_ops *ops;
	php_hashcontext_object *hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|lS", &algo, &options, &key) == FAILURE) {
		RETURN_THROWS();
	}

	ops = php_hash_fetch_ops(algo);
	if (!ops) {
		zend_argument_value_error(1, "must be a valid hashing algorithm");
		RETURN_THROWS();
	}

	if (options & PHP_HASH_HMAC) {
		if (!ops->is_crypto) {
			zend_argument_value_error(1, "must be a cryptographic hashing algorithm if HMAC is requested");
			RETURN_THROWS();
		}
		if (!key || (ZSTR_LEN(key) == 0)) {
			/* Note: a zero length key is no key at all */
			zend_argument_value_error(3, "cannot be empty when HMAC is requested");
			RETURN_THROWS();
		}
	}

	object_init_ex(return_value, php_hashcontext_ce);
	hash = php_hashcontext_from_object(Z_OBJ_P(return_value));

	context = php_hash_alloc_context(ops);
	ops->hash_init(context);

	hash->ops = ops;
	hash->context = context;
	hash->options = options;
	hash->key = NULL;

	if (options & PHP_HASH_HMAC) {
		char *K = emalloc(ops->block_size);
		size_t i, block_size;

		memset(K, 0, ops->block_size);

		if (ZSTR_LEN(key) > ops->block_size) {
			/* Reduce the key first */
			ops->hash_update(context, (unsigned char *) ZSTR_VAL(key), ZSTR_LEN(key));
			ops->hash_final((unsigned char *) K, context);
			/* Make the context ready to start over */
			ops->hash_init(context);
		} else {
			memcpy(K, ZSTR_VAL(key), ZSTR_LEN(key));
		}

		/* XOR ipad */
		block_size = ops->block_size;
		for(i = 0; i < block_size; i++) {
			K[i] ^= 0x36;
		}
		ops->hash_update(context, (unsigned char *) K, ops->block_size);
		hash->key = (unsigned char *) K;
	}
}
/* }}} */

#define PHP_HASHCONTEXT_VERIFY(hash) { \
	if (!hash->context) { \
		zend_argument_type_error(1, "must be a valid Hash Context resource"); \
		RETURN_THROWS(); \
	} \
}

/* {{{ Pump data into the hashing algorithm */
PHP_FUNCTION(hash_update)
{
	zval *zhash;
	php_hashcontext_object *hash;
	zend_string *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OS", &zhash, php_hashcontext_ce, &data) == FAILURE) {
		RETURN_THROWS();
	}

	hash = php_hashcontext_from_object(Z_OBJ_P(zhash));
	PHP_HASHCONTEXT_VERIFY(hash);
	hash->ops->hash_update(hash->context, (unsigned char *) ZSTR_VAL(data), ZSTR_LEN(data));

	RETURN_TRUE;
}
/* }}} */

/* {{{ Pump data into the hashing algorithm from an open stream */
PHP_FUNCTION(hash_update_stream)
{
	zval *zhash, *zstream;
	php_hashcontext_object *hash;
	php_stream *stream = NULL;
	zend_long length = -1, didread = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Or|l", &zhash, php_hashcontext_ce, &zstream, &length) == FAILURE) {
		RETURN_THROWS();
	}

	hash = php_hashcontext_from_object(Z_OBJ_P(zhash));
	PHP_HASHCONTEXT_VERIFY(hash);
	php_stream_from_zval(stream, zstream);

	while (length) {
		char buf[1024];
		zend_long toread = 1024;
		ssize_t n;

		if (length > 0 && toread > length) {
			toread = length;
		}

		if ((n = php_stream_read(stream, buf, toread)) <= 0) {
			RETURN_LONG(didread);
		}
		hash->ops->hash_update(hash->context, (unsigned char *) buf, n);
		length -= n;
		didread += n;
	}

	RETURN_LONG(didread);
}
/* }}} */

/* {{{ Pump data into the hashing algorithm from a file */
PHP_FUNCTION(hash_update_file)
{
	zval *zhash, *zcontext = NULL;
	php_hashcontext_object *hash;
	php_stream_context *context = NULL;
	php_stream *stream;
	zend_string *filename;
	char buf[1024];
	ssize_t n;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OP|r!", &zhash, php_hashcontext_ce, &filename, &zcontext) == FAILURE) {
		RETURN_THROWS();
	}

	hash = php_hashcontext_from_object(Z_OBJ_P(zhash));
	PHP_HASHCONTEXT_VERIFY(hash);
	context = php_stream_context_from_zval(zcontext, 0);

	stream = php_stream_open_wrapper_ex(ZSTR_VAL(filename), "rb", REPORT_ERRORS, NULL, context);
	if (!stream) {
		/* Stream will report errors opening file */
		RETURN_FALSE;
	}

	while ((n = php_stream_read(stream, buf, sizeof(buf))) > 0) {
		hash->ops->hash_update(hash->context, (unsigned char *) buf, n);
	}
	php_stream_close(stream);

	RETURN_BOOL(n >= 0);
}
/* }}} */

/* {{{ Output resulting digest */
PHP_FUNCTION(hash_final)
{
	zval *zhash;
	php_hashcontext_object *hash;
	zend_bool raw_output = 0;
	zend_string *digest;
	size_t digest_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &zhash, php_hashcontext_ce, &raw_output) == FAILURE) {
		RETURN_THROWS();
	}

	hash = php_hashcontext_from_object(Z_OBJ_P(zhash));
	PHP_HASHCONTEXT_VERIFY(hash);

	digest_len = hash->ops->digest_size;
	digest = zend_string_alloc(digest_len, 0);
	hash->ops->hash_final((unsigned char *) ZSTR_VAL(digest), hash->context);
	if (hash->options & PHP_HASH_HMAC) {
		size_t i, block_size;

		/* Convert K to opad -- 0x6A = 0x36 ^ 0x5C */
		block_size = hash->ops->block_size;
		for(i = 0; i < block_size; i++) {
			hash->key[i] ^= 0x6A;
		}

		/* Feed this result into the outer hash */
		hash->ops->hash_init(hash->context);
		hash->ops->hash_update(hash->context, hash->key, hash->ops->block_size);
		hash->ops->hash_update(hash->context, (unsigned char *) ZSTR_VAL(digest), hash->ops->digest_size);
		hash->ops->hash_final((unsigned char *) ZSTR_VAL(digest), hash->context);

		/* Zero the key */
		ZEND_SECURE_ZERO(hash->key, hash->ops->block_size);
		efree(hash->key);
		hash->key = NULL;
	}
	ZSTR_VAL(digest)[digest_len] = 0;

	/* Invalidate the object from further use */
	efree(hash->context);
	hash->context = NULL;

	if (raw_output) {
		RETURN_NEW_STR(digest);
	} else {
		zend_string *hex_digest = zend_string_safe_alloc(digest_len, 2, 0, 0);

		php_hash_bin2hex(ZSTR_VAL(hex_digest), (unsigned char *) ZSTR_VAL(digest), digest_len);
		ZSTR_VAL(hex_digest)[2 * digest_len] = 0;
		zend_string_release_ex(digest, 0);
		RETURN_NEW_STR(hex_digest);
	}
}
/* }}} */

/* {{{ Copy hash object */
PHP_FUNCTION(hash_copy)
{
	zval *zhash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &zhash, php_hashcontext_ce) == FAILURE) {
		RETURN_THROWS();
	}

	RETVAL_OBJ(Z_OBJ_HANDLER_P(zhash, clone_obj)(Z_OBJ_P(zhash)));

	if (php_hashcontext_from_object(Z_OBJ_P(return_value))->context == NULL) {
		zval_ptr_dtor(return_value);

		zend_throw_error(NULL, "Cannot copy hash");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Return a list of registered hashing algorithms */
PHP_FUNCTION(hash_algos)
{
	zend_string *str;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY(&php_hash_hashtable, str) {
		add_next_index_str(return_value, zend_string_copy(str));
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Return a list of registered hashing algorithms suitable for hash_hmac() */
PHP_FUNCTION(hash_hmac_algos)
{
	zend_string *str;
	const php_hash_ops *ops;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&php_hash_hashtable, str, ops) {
		if (ops->is_crypto) {
			add_next_index_str(return_value, zend_string_copy(str));
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ RFC5869 HMAC-based key derivation function */
PHP_FUNCTION(hash_hkdf)
{
	zend_string *returnval, *ikm, *algo, *info = NULL, *salt = NULL;
	zend_long length = 0;
	unsigned char *prk, *digest, *K;
	size_t i;
	size_t rounds;
	const php_hash_ops *ops;
	void *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|lSS", &algo, &ikm, &length, &info, &salt) == FAILURE) {
		RETURN_THROWS();
	}

	ops = php_hash_fetch_ops(algo);
	if (!ops || !ops->is_crypto) {
		zend_argument_value_error(1, "must be a valid cryptographic hashing algorithm");
		RETURN_THROWS();
	}

	if (ZSTR_LEN(ikm) == 0) {
		zend_argument_value_error(2, "cannot be empty");
		RETURN_THROWS();
	}

	if (length < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	} else if (length == 0) {
		length = ops->digest_size;
	} else if (length > (zend_long) (ops->digest_size * 255)) {
		zend_argument_value_error(3, "must be less than or equal to %zd", ops->digest_size * 255);
		RETURN_THROWS();
	}

	context = php_hash_alloc_context(ops);

	// Extract
	ops->hash_init(context);
	K = emalloc(ops->block_size);
	php_hash_hmac_prep_key(K, ops, context,
		(unsigned char *) (salt ? ZSTR_VAL(salt) : ""), salt ? ZSTR_LEN(salt) : 0);

	prk = emalloc(ops->digest_size);
	php_hash_hmac_round(prk, ops, context, K, (unsigned char *) ZSTR_VAL(ikm), ZSTR_LEN(ikm));
	php_hash_string_xor_char(K, K, 0x6A, ops->block_size);
	php_hash_hmac_round(prk, ops, context, K, prk, ops->digest_size);
	ZEND_SECURE_ZERO(K, ops->block_size);

	// Expand
	returnval = zend_string_alloc(length, 0);
	digest = emalloc(ops->digest_size);
	for (i = 1, rounds = (length - 1) / ops->digest_size + 1; i <= rounds; i++) {
		// chr(i)
		unsigned char c[1];
		c[0] = (i & 0xFF);

		php_hash_hmac_prep_key(K, ops, context, prk, ops->digest_size);
		ops->hash_init(context);
		ops->hash_update(context, K, ops->block_size);

		if (i > 1) {
			ops->hash_update(context, digest, ops->digest_size);
		}

		if (info != NULL && ZSTR_LEN(info) > 0) {
			ops->hash_update(context, (unsigned char *) ZSTR_VAL(info), ZSTR_LEN(info));
		}

		ops->hash_update(context, c, 1);
		ops->hash_final(digest, context);
		php_hash_string_xor_char(K, K, 0x6A, ops->block_size);
		php_hash_hmac_round(digest, ops, context, K, digest, ops->digest_size);
		memcpy(
			ZSTR_VAL(returnval) + ((i - 1) * ops->digest_size),
			digest,
			(i == rounds ? length - ((i - 1) * ops->digest_size) : ops->digest_size)
		);
	}

	ZEND_SECURE_ZERO(K, ops->block_size);
	ZEND_SECURE_ZERO(digest, ops->digest_size);
	ZEND_SECURE_ZERO(prk, ops->digest_size);
	efree(K);
	efree(context);
	efree(prk);
	efree(digest);
	ZSTR_VAL(returnval)[length] = 0;
	RETURN_STR(returnval);
}

/* {{{ Generate a PBKDF2 hash of the given password and salt
Returns lowercase hexits by default */
PHP_FUNCTION(hash_pbkdf2)
{
	zend_string *returnval, *algo;
	char *salt, *pass = NULL;
	unsigned char *computed_salt, *digest, *temp, *result, *K1, *K2 = NULL;
	zend_long loops, i, j, iterations, digest_length = 0, length = 0;
	size_t pass_len, salt_len = 0;
	zend_bool raw_output = 0;
	const php_hash_ops *ops;
	void *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sssl|lb", &algo, &pass, &pass_len, &salt, &salt_len, &iterations, &length, &raw_output) == FAILURE) {
		RETURN_THROWS();
	}

	ops = php_hash_fetch_ops(algo);
	if (!ops || !ops->is_crypto) {
		zend_argument_value_error(1, "must be a valid cryptographic hashing algorithm");
		RETURN_THROWS();
	}

	if (salt_len > INT_MAX - 4) {
		zend_argument_value_error(3, "must be less than or equal to INT_MAX - 4 bytes");
		RETURN_THROWS();
	}

	if (iterations <= 0) {
		zend_argument_value_error(4, "must be greater than 0");
		RETURN_THROWS();
	}

	if (length < 0) {
		zend_argument_value_error(5, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	context = php_hash_alloc_context(ops);
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
		memcpy(ZSTR_VAL(returnval), result, length);
	} else {
		php_hash_bin2hex(ZSTR_VAL(returnval), result, digest_length);
	}
	ZSTR_VAL(returnval)[length] = 0;
	efree(result);
	RETURN_NEW_STR(returnval);
}
/* }}} */

/* {{{ Compares two strings using the same time whether they're equal or not.
   A difference in length will leak */
PHP_FUNCTION(hash_equals)
{
	zval *known_zval, *user_zval;
	char *known_str, *user_str;
	int result = 0;
	size_t j;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &known_zval, &user_zval) == FAILURE) {
		RETURN_THROWS();
	}

	/* We only allow comparing string to prevent unexpected results. */
	if (Z_TYPE_P(known_zval) != IS_STRING) {
		zend_argument_type_error(1, "must be of type string, %s given", zend_zval_type_name(known_zval));
		RETURN_THROWS();
	}

	if (Z_TYPE_P(user_zval) != IS_STRING) {
		zend_argument_type_error(2, "must be of type string, %s given", zend_zval_type_name(user_zval));
		RETURN_THROWS();
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

/* {{{ */
PHP_METHOD(HashContext, __construct) {
	/* Normally unreachable as private/final */
	zend_throw_exception(zend_ce_error, "Illegal call to private/final constructor", 0);
}
/* }}} */

/* Module Housekeeping */

#define PHP_HASH_HAVAL_REGISTER(p,b)	php_hash_register_algo("haval" #b "," #p , &php_hash_##p##haval##b##_ops);

#ifdef PHP_MHASH_BC

#if 0
/* See #69823, we should not insert module into module_registry while doing startup */

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
	PHP_MHASH_VERSION,
	STANDARD_MODULE_PROPERTIES,
};
#endif

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

		len = slprintf(buf, 127, "MHASH_%s", algorithm.mhash_name);
		zend_register_long_constant(buf, len, algorithm.value, CONST_CS | CONST_PERSISTENT, module_number);
	}

	/* TODO: this cause #69823 zend_register_internal_module(&mhash_module_entry); */
}

/* {{{ Hash data with hash */
PHP_FUNCTION(mhash)
{
	zend_long algorithm;
	zend_string *algo = NULL;
	char *data, *key = NULL;
	size_t data_len, key_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls|s!", &algorithm, &data, &data_len, &key, &key_len) == FAILURE) {
		RETURN_THROWS();
	}

	/* need to convert the first parameter from int constant to string algorithm name */
	if (algorithm >= 0 && algorithm < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.hash_name) {
			algo = zend_string_init(algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name), 0);
		}
	}

	if (key) {
		php_hash_do_hash_hmac(return_value, algo, data, data_len, key, key_len, 1, 0);
	} else {
		php_hash_do_hash(return_value, algo, data, data_len, 1, 0);
	}

	if (algo) {
		zend_string_release(algo);
	}
}
/* }}} */

/* {{{ Gets the name of hash */
PHP_FUNCTION(mhash_get_hash_name)
{
	zend_long algorithm;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &algorithm) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Gets the number of available hashes */
PHP_FUNCTION(mhash_count)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	RETURN_LONG(MHASH_NUM_ALGOS - 1);
}
/* }}} */

/* {{{ Gets the block size of hash */
PHP_FUNCTION(mhash_get_block_size)
{
	zend_long algorithm;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &algorithm) == FAILURE) {
		RETURN_THROWS();
	}
	RETVAL_FALSE;

	if (algorithm >= 0 && algorithm  < MHASH_NUM_ALGOS) {
		struct mhash_bc_entry algorithm_lookup = mhash_to_hash[algorithm];
		if (algorithm_lookup.mhash_name) {
			const php_hash_ops *ops = zend_hash_str_find_ptr(&php_hash_hashtable, algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name));
			if (ops) {
				RETVAL_LONG(ops->digest_size);
			}
		}
	}
}
/* }}} */

#define SALT_SIZE 8

/* {{{ Generates a key using hash functions */
PHP_FUNCTION(mhash_keygen_s2k)
{
	zend_long algorithm, l_bytes;
	int bytes;
	char *password, *salt;
	size_t password_len, salt_len;
	char padded_salt[SALT_SIZE];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lssl", &algorithm, &password, &password_len, &salt, &salt_len, &l_bytes) == FAILURE) {
		RETURN_THROWS();
	}

	bytes = (int)l_bytes;
	if (bytes <= 0){
		zend_argument_value_error(4, "must be a greater than 0");
		RETURN_THROWS();
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
			const php_hash_ops *ops = zend_hash_str_find_ptr(&php_hash_hashtable, algorithm_lookup.hash_name, strlen(algorithm_lookup.hash_name));
			if (ops) {
				unsigned char null = '\0';
				void *context;
				char *key, *digest;
				int i = 0, j = 0;
				size_t block_size = ops->digest_size;
				size_t times = bytes / block_size;

				if ((bytes % block_size) != 0) {
					times++;
				}

				context = php_hash_alloc_context(ops);
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

/* ----------------------------------------------------------------------- */

/* {{{ php_hashcontext_create */
static zend_object* php_hashcontext_create(zend_class_entry *ce) {
	php_hashcontext_object *objval = zend_object_alloc(sizeof(php_hashcontext_object), ce);
	zend_object *zobj = &objval->std;

	zend_object_std_init(zobj, ce);
	object_properties_init(zobj, ce);
	zobj->handlers = &php_hashcontext_handlers;

	return zobj;
}
/* }}} */

/* {{{ php_hashcontext_dtor */
static void php_hashcontext_dtor(zend_object *obj) {
	php_hashcontext_object *hash = php_hashcontext_from_object(obj);

	if (hash->context) {
		efree(hash->context);
		hash->context = NULL;
	}

	if (hash->key) {
		ZEND_SECURE_ZERO(hash->key, hash->ops->block_size);
		efree(hash->key);
		hash->key = NULL;
	}
}
/* }}} */

/* {{{ php_hashcontext_clone */
static zend_object *php_hashcontext_clone(zend_object *zobj) {
	php_hashcontext_object *oldobj = php_hashcontext_from_object(zobj);
	zend_object *znew = php_hashcontext_create(zobj->ce);
	php_hashcontext_object *newobj = php_hashcontext_from_object(znew);

	zend_objects_clone_members(znew, zobj);

	newobj->ops = oldobj->ops;
	newobj->options = oldobj->options;
	newobj->context = php_hash_alloc_context(newobj->ops);
	newobj->ops->hash_init(newobj->context);

	if (SUCCESS != newobj->ops->hash_copy(newobj->ops, oldobj->context, newobj->context)) {
		efree(newobj->context);
		newobj->context = NULL;
		return znew;
	}

	newobj->key = ecalloc(1, newobj->ops->block_size);
	if (oldobj->key) {
		memcpy(newobj->key, oldobj->key, newobj->ops->block_size);
	}

	return znew;
}
/* }}} */

/* Serialization format: 5-element array
   Index 0: hash algorithm (string)
   Index 1: options (long, 0)
   Index 2: hash-determined serialization of context state (usually array)
   Index 3: magic number defining layout of context state (long, usually 2)
   Index 4: properties (array)

   HashContext serializations are not necessarily portable between architectures or
   PHP versions. If the format of a serialized hash context changes, that should
   be reflected in either a different value of `magic` or a different format of
   the serialized context state. Most context states are unparsed and parsed using
   a spec string, such as "llb128.", using the format defined by
   `php_hash_serialize_spec`/`php_hash_unserialize_spec`. Some hash algorithms must
   also check the unserialized state for validity, to ensure that using an
   unserialized context is safe from memory errors.

   Currently HASH_HMAC contexts cannot be serialized, because serializing them
   would require serializing the HMAC key in plaintext. */

/* {{{ Serialize the object */
PHP_METHOD(HashContext, __serialize)
{
	zval *object = ZEND_THIS;
	php_hashcontext_object *hash = php_hashcontext_from_object(Z_OBJ_P(object));
	zend_long magic = 0;
	zval tmp;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	array_init(return_value);

	if (!hash->ops->hash_serialize) {
		goto serialize_failure;
	} else if (hash->options & PHP_HASH_HMAC) {
		zend_throw_exception(NULL, "HashContext with HASH_HMAC option cannot be serialized", 0);
		RETURN_THROWS();
	}

	ZVAL_STRING(&tmp, hash->ops->algo);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	ZVAL_LONG(&tmp, hash->options);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	if (hash->ops->hash_serialize(hash, &magic, &tmp) != SUCCESS) {
		goto serialize_failure;
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	ZVAL_LONG(&tmp, magic);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* members */
	ZVAL_ARR(&tmp, zend_std_get_properties(&hash->std));
	Z_TRY_ADDREF(tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	return;

serialize_failure:
	zend_throw_exception_ex(NULL, 0, "HashContext for algorithm \"%s\" cannot be serialized", hash->ops->algo);
	RETURN_THROWS();
}
/* }}} */

/* {{{ unserialize the object */
PHP_METHOD(HashContext, __unserialize)
{
	zval *object = ZEND_THIS;
	php_hashcontext_object *hash = php_hashcontext_from_object(Z_OBJ_P(object));
	HashTable *data;
	zval *algo_zv, *magic_zv, *options_zv, *hash_zv, *members_zv;
	zend_long magic, options;
	int unserialize_result;
	const php_hash_ops *ops;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &data) == FAILURE) {
		RETURN_THROWS();
	}

	if (hash->context) {
		zend_throw_exception(NULL, "HashContext::__unserialize called on initialized object", 0);
		RETURN_THROWS();
	}

	algo_zv = zend_hash_index_find(data, 0);
	options_zv = zend_hash_index_find(data, 1);
	hash_zv = zend_hash_index_find(data, 2);
	magic_zv = zend_hash_index_find(data, 3);
	members_zv = zend_hash_index_find(data, 4);

	if (!algo_zv || Z_TYPE_P(algo_zv) != IS_STRING
		|| !magic_zv || Z_TYPE_P(magic_zv) != IS_LONG
		|| !options_zv || Z_TYPE_P(options_zv) != IS_LONG
		|| !hash_zv
		|| !members_zv || Z_TYPE_P(members_zv) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}

	magic = Z_LVAL_P(magic_zv);
	options = Z_LVAL_P(options_zv);
	if (options & PHP_HASH_HMAC) {
		zend_throw_exception(NULL, "HashContext with HASH_HMAC option cannot be serialized", 0);
		RETURN_THROWS();
	}

	ops = php_hash_fetch_ops(Z_STR_P(algo_zv));
	if (!ops) {
		zend_throw_exception(NULL, "Unknown hash algorithm", 0);
		RETURN_THROWS();
	} else if (!ops->hash_unserialize) {
		zend_throw_exception_ex(NULL, 0, "Hash algorithm \"%s\" cannot be unserialized", ops->algo);
		RETURN_THROWS();
	}

	hash->ops = ops;
	hash->context = php_hash_alloc_context(ops);
	ops->hash_init(hash->context);
	hash->options = options;

	unserialize_result = ops->hash_unserialize(hash, magic, hash_zv);
	if (unserialize_result != SUCCESS) {
		zend_throw_exception_ex(NULL, 0, "Incomplete or ill-formed serialization data (\"%s\" code %d)", ops->algo, unserialize_result);
		/* free context */
		php_hashcontext_dtor(Z_OBJ_P(object));
		RETURN_THROWS();
	}

	object_properties_load(&hash->std, Z_ARRVAL_P(members_zv));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(hash)
{
	zend_class_entry ce;

	zend_hash_init(&php_hash_hashtable, 35, NULL, NULL, 1);

	php_hash_register_algo("md2",			&php_hash_md2_ops);
	php_hash_register_algo("md4",			&php_hash_md4_ops);
	php_hash_register_algo("md5",			&php_hash_md5_ops);
	php_hash_register_algo("sha1",			&php_hash_sha1_ops);
	php_hash_register_algo("sha224",		&php_hash_sha224_ops);
	php_hash_register_algo("sha256",		&php_hash_sha256_ops);
	php_hash_register_algo("sha384",		&php_hash_sha384_ops);
	php_hash_register_algo("sha512/224",            &php_hash_sha512_224_ops);
	php_hash_register_algo("sha512/256",            &php_hash_sha512_256_ops);
	php_hash_register_algo("sha512",		&php_hash_sha512_ops);
	php_hash_register_algo("sha3-224",		&php_hash_sha3_224_ops);
	php_hash_register_algo("sha3-256",		&php_hash_sha3_256_ops);
	php_hash_register_algo("sha3-384",		&php_hash_sha3_384_ops);
	php_hash_register_algo("sha3-512",		&php_hash_sha3_512_ops);
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
	php_hash_register_algo("crc32c",		&php_hash_crc32c_ops);
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

	INIT_CLASS_ENTRY(ce, "HashContext", class_HashContext_methods);
	php_hashcontext_ce = zend_register_internal_class(&ce);
	php_hashcontext_ce->ce_flags |= ZEND_ACC_FINAL;
	php_hashcontext_ce->create_object = php_hashcontext_create;

	memcpy(&php_hashcontext_handlers, &std_object_handlers,
	       sizeof(zend_object_handlers));
	php_hashcontext_handlers.offset = XtOffsetOf(php_hashcontext_object, std);
	php_hashcontext_handlers.dtor_obj = php_hashcontext_dtor;
	php_hashcontext_handlers.clone_obj = php_hashcontext_clone;

#ifdef PHP_MHASH_BC
	mhash_init(INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(hash)
{
	zend_hash_destroy(&php_hash_hashtable);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(hash)
{
	char buffer[2048];
	zend_string *str;
	char *s = buffer, *e = s + sizeof(buffer);

	ZEND_HASH_FOREACH_STR_KEY(&php_hash_hashtable, str) {
		s += slprintf(s, e - s, "%s ", ZSTR_VAL(str));
	} ZEND_HASH_FOREACH_END();
	*s = 0;

	php_info_print_table_start();
	php_info_print_table_row(2, "hash support", "enabled");
	php_info_print_table_row(2, "Hashing Engines", buffer);
	php_info_print_table_end();

#ifdef PHP_MHASH_BC
	php_info_print_table_start();
	php_info_print_table_row(2, "MHASH support", "Enabled");
	php_info_print_table_row(2, "MHASH API Version", "Emulated Support");
	php_info_print_table_end();
#endif

}
/* }}} */

/* {{{ hash_module_entry */
zend_module_entry hash_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_HASH_EXTNAME,
	ext_functions,
	PHP_MINIT(hash),
	PHP_MSHUTDOWN(hash),
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	PHP_MINFO(hash),
	PHP_HASH_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */
