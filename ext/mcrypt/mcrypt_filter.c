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
  +----------------------------------------------------------------------+

  $Id$
*/

#include "php.h"

#include "php_mcrypt_filter.h"
#include "php_ini.h"
#include <mcrypt.h>

typedef struct _php_mcrypt_filter_data {
	MCRYPT module;
	char encrypt;
	int blocksize;
	char *block_buffer;
	int block_used;
	char persistent;
} php_mcrypt_filter_data;

static php_stream_filter_status_t php_mcrypt_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags)
{
	php_mcrypt_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;

	if (!thisfilter || !Z_PTR(thisfilter->abstract)) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_mcrypt_filter_data *)(Z_PTR(thisfilter->abstract));
	while(buckets_in->head) {
		bucket = buckets_in->head;

		consumed += bucket->buflen;

		if (data->blocksize) {
			/* Blockmode cipher */
			char *outchunk;
			int chunklen = (int)(bucket->buflen + data->block_used), n;
			php_stream_bucket *newbucket;

			outchunk = pemalloc(chunklen, data->persistent);
			if (data->block_used) {
				memcpy(outchunk, data->block_buffer, data->block_used);
			}
			memcpy(outchunk + data->block_used, bucket->buf, bucket->buflen);

			for(n=0; (n + data->blocksize) <= chunklen; n += data->blocksize) {

				if (data->encrypt) {
					mcrypt_generic(data->module, outchunk + n, data->blocksize);
				} else {
					mdecrypt_generic(data->module, outchunk + n, data->blocksize);
				}
			}
			data->block_used = chunklen - n;
			memcpy(data->block_buffer, outchunk + n, data->block_used);

			newbucket = php_stream_bucket_new(stream, outchunk, n, 1, data->persistent);
			php_stream_bucket_append(buckets_out, newbucket);

			exit_status = PSFS_PASS_ON;

			php_stream_bucket_unlink(bucket);
			php_stream_bucket_delref(bucket);
		} else {
			/* Stream cipher */
			php_stream_bucket_make_writeable(bucket);
			if (data->encrypt) {
				mcrypt_generic(data->module, bucket->buf, (int)bucket->buflen);
			} else {
				mdecrypt_generic(data->module, bucket->buf, (int)bucket->buflen);
			}
			php_stream_bucket_append(buckets_out, bucket);

			exit_status = PSFS_PASS_ON;
		}
	}

	if ((flags & PSFS_FLAG_FLUSH_CLOSE) && data->blocksize && data->block_used) {
		php_stream_bucket *newbucket;

		memset(data->block_buffer + data->block_used, 0, data->blocksize - data->block_used);
		if (data->encrypt) {
			mcrypt_generic(data->module, data->block_buffer, data->blocksize);
		} else {
			mdecrypt_generic(data->module, data->block_buffer, data->blocksize);
		}

		newbucket = php_stream_bucket_new(stream, data->block_buffer, data->blocksize, 0, data->persistent);
		php_stream_bucket_append(buckets_out, newbucket);

		exit_status = PSFS_PASS_ON;
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return exit_status;
}

static void php_mcrypt_filter_dtor(php_stream_filter *thisfilter)
{
	if (thisfilter && Z_PTR(thisfilter->abstract)) {
		php_mcrypt_filter_data *data = (php_mcrypt_filter_data*) Z_PTR(thisfilter->abstract);

		if (data->block_buffer) {
			pefree(data->block_buffer, data->persistent);
		}

		mcrypt_generic_deinit(data->module);
		mcrypt_module_close(data->module);

		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_mcrypt_filter_ops = {
    php_mcrypt_filter,
    php_mcrypt_filter_dtor,
    "mcrypt.*"
};

/* {{{ php_mcrypt_filter_create
 * Instantiate mcrypt filter
 */
static php_stream_filter *php_mcrypt_filter_create(const char *filtername, zval *filterparams, int persistent)
{
	int encrypt = 1, iv_len, key_len, keyl, result;
	const char *cipher = filtername + sizeof("mcrypt.") - 1;
	zval *tmpzval;
	MCRYPT mcrypt_module;
	char *iv = NULL, *key = NULL;
	char *algo_dir = INI_STR("mcrypt.algorithms_dir");
	char *mode_dir = INI_STR("mcrypt.modes_dir");
	char *mode = "cbc";
	php_mcrypt_filter_data *data;

	if (strncasecmp(filtername, "mdecrypt.", sizeof("mdecrypt.") - 1) == 0) {
		encrypt = 0;
		cipher += sizeof("de") - 1;
	} else if (strncasecmp(filtername, "mcrypt.", sizeof("mcrypt.") - 1) != 0) {
		/* Should never happen */
		return NULL;
	}

	if (!filterparams || Z_TYPE_P(filterparams) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING, "Filter parameters for %s must be an array", filtername);
		return NULL;
	}

	if ((tmpzval = zend_hash_str_find(Z_ARRVAL_P(filterparams), ZEND_STRL("mode")))) {
		if (Z_TYPE_P(tmpzval) == IS_STRING) {
			mode = Z_STRVAL_P(tmpzval);
		} else {
			php_error_docref(NULL, E_WARNING, "mode is not a string, ignoring");
		}
	}

	if ((tmpzval=zend_hash_str_find(Z_ARRVAL_P(filterparams), ZEND_STRL("algorithms_dir")))) {
		if (Z_TYPE_P(tmpzval) == IS_STRING) {
			algo_dir = Z_STRVAL_P(tmpzval);
		} else {
			php_error_docref(NULL, E_WARNING, "algorithms_dir is not a string, ignoring");
		}
	}

	if ((tmpzval=zend_hash_str_find(Z_ARRVAL_P(filterparams), ZEND_STRL("modes_dir")))) {
		if (Z_TYPE_P(tmpzval) == IS_STRING) {
			mode_dir = Z_STRVAL_P(tmpzval);
		} else {
			php_error_docref(NULL, E_WARNING, "modes_dir is not a string, ignoring");
		}
	}

	if ((tmpzval = zend_hash_str_find(Z_ARRVAL_P(filterparams), ZEND_STRL("key"))) &&
		Z_TYPE_P(tmpzval) == IS_STRING) {
		key = Z_STRVAL_P(tmpzval);
		key_len = (int)Z_STRLEN_P(tmpzval);
	} else {
		php_error_docref(NULL, E_WARNING, "key not specified or is not a string");
		return NULL;
	}

	mcrypt_module = mcrypt_module_open((char *)cipher, algo_dir, mode, mode_dir);
	if (mcrypt_module == MCRYPT_FAILED) {
		php_error_docref(NULL, E_WARNING, "Could not open encryption module");
		return NULL;
	}
	iv_len = mcrypt_enc_get_iv_size(mcrypt_module);
	keyl = mcrypt_enc_get_key_size(mcrypt_module);
	if (keyl < key_len) {
		key_len = keyl;
	}

	if (!(tmpzval = zend_hash_str_find(Z_ARRVAL_P(filterparams), ZEND_STRL("iv"))) ||
		Z_TYPE_P(tmpzval) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "Filter parameter[iv] not provided or not of type: string");
		mcrypt_module_close(mcrypt_module);
		return NULL;
	}

	iv = emalloc(iv_len + 1);
	if (iv_len <= Z_STRLEN_P(tmpzval)) {
		memcpy(iv, Z_STRVAL_P(tmpzval), iv_len);
	} else {
		memcpy(iv, Z_STRVAL_P(tmpzval), Z_STRLEN_P(tmpzval));
		memset(iv + Z_STRLEN_P(tmpzval), 0, iv_len - Z_STRLEN_P(tmpzval));
	}

	result = mcrypt_generic_init(mcrypt_module, key, key_len, iv);
	efree(iv);
	if (result < 0) {
		switch (result) {
			case -3:
				php_error_docref(NULL, E_WARNING, "Key length incorrect");
				break;
			case -4:
				php_error_docref(NULL, E_WARNING, "Memory allocation error");
				break;
			case -1:
			default:
				php_error_docref(NULL, E_WARNING, "Unknown error");
				break;
		}
		mcrypt_module_close(mcrypt_module);
		return NULL;
	}

	data = pemalloc(sizeof(php_mcrypt_filter_data), persistent);
	data->module = mcrypt_module;
	data->encrypt = encrypt;
	if (mcrypt_enc_is_block_mode(mcrypt_module)) {
		data->blocksize = mcrypt_enc_get_block_size(mcrypt_module);
		data->block_buffer = pemalloc(data->blocksize, persistent);
	} else {
		data->blocksize = 0;
		data->block_buffer = NULL;
	}
	data->block_used = 0;
	data->persistent = persistent;

	return php_stream_filter_alloc(&php_mcrypt_filter_ops, data, persistent);
}
/* }}} */

php_stream_filter_factory php_mcrypt_filter_factory = {
	php_mcrypt_filter_create
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
