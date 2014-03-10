/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sara Golemon (pollita@php.net)                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_bz2.h"

/* {{{ data structure */

enum strm_status {
    PHP_BZ2_UNITIALIZED,
    PHP_BZ2_RUNNING,
    PHP_BZ2_FINISHED
};

typedef struct _php_bz2_filter_data {
	int persistent;
	bz_stream strm;
	char *inbuf;
	size_t inbuf_len;
	char *outbuf;
	size_t outbuf_len;
	
	/* Decompress options */
	enum strm_status status;
	unsigned int small_footprint : 1;
	unsigned int expect_concatenated : 1;
} php_bz2_filter_data;

/* }}} */

/* {{{ Memory management wrappers */

static void *php_bz2_alloc(void *opaque, int items, int size)
{
	return (void *)safe_pemalloc(items, size, 0, ((php_bz2_filter_data*)opaque)->persistent);
}

static void php_bz2_free(void *opaque, void *address)
{
	pefree((void *)address, ((php_bz2_filter_data*)opaque)->persistent);
}
/* }}} */

/* {{{ bzip2.decompress filter implementation */

static php_stream_filter_status_t php_bz2_decompress_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_bz2_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	bz_stream *streamp;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_bz2_filter_data *)(thisfilter->abstract);
	streamp = &(data->strm);

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		while (bin < bucket->buflen) {
			if (data->status == PHP_BZ2_UNITIALIZED) {
				status = BZ2_bzDecompressInit(streamp, 0, data->small_footprint);

				if (BZ_OK != status) {
					php_stream_bucket_delref(bucket TSRMLS_CC);
					return PSFS_ERR_FATAL;
				}

				data->status = PHP_BZ2_RUNNING;
			}

			if (data->status != PHP_BZ2_RUNNING) {
				consumed += bucket->buflen;
				break;
			}

			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			status = BZ2_bzDecompress(&(data->strm));

			if (status == BZ_STREAM_END) {
				BZ2_bzDecompressEnd(&(data->strm));
				if (data->expect_concatenated) {
					data->status = PHP_BZ2_UNITIALIZED;
				} else {
					data->status = PHP_BZ2_FINISHED;
				}
			} else if (status != BZ_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
			consumed += desired;
			bin += desired;

			if (data->strm.avail_out < data->outbuf_len) {
				php_stream_bucket *out_bucket;
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;
				out_bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, out_bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			} else if (status == BZ_STREAM_END && data->strm.avail_out >= data->outbuf_len) {
				/* no more data to decompress, and nothing was spat out */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_PASS_ON;
			}
		}

		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if ((data->status == PHP_BZ2_RUNNING) && (flags & PSFS_FLAG_FLUSH_CLOSE)) {
		/* Spit it out! */
		status = BZ_OK;
		while (status == BZ_OK) {
			status = BZ2_bzDecompress(&(data->strm));
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			} else if (status == BZ_OK) {
				break;
			}
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return exit_status;
}

static void php_bz2_decompress_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_bz2_filter_data *data = thisfilter->abstract;
		if (data->status == PHP_BZ2_RUNNING) {
			BZ2_bzDecompressEnd(&(data->strm));
		}
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_bz2_decompress_ops = {
	php_bz2_decompress_filter,
	php_bz2_decompress_dtor,
	"bzip2.decompress"
};
/* }}} */

/* {{{ bzip2.compress filter implementation */

static php_stream_filter_status_t php_bz2_compress_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_bz2_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_bz2_filter_data *)(thisfilter->abstract);

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);

		while (bin < bucket->buflen) {
			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			status = BZ2_bzCompress(&(data->strm), flags & PSFS_FLAG_FLUSH_CLOSE ? BZ_FINISH : (flags & PSFS_FLAG_FLUSH_INC ? BZ_FLUSH : BZ_RUN));
			if (status != BZ_RUN_OK && status != BZ_FLUSH_OK && status != BZ_FINISH_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
			consumed += desired;
			bin += desired;

			if (data->strm.avail_out < data->outbuf_len) {
				php_stream_bucket *out_bucket;
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				out_bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, out_bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = BZ_FINISH_OK;
		while (status == BZ_FINISH_OK) {
			status = BZ2_bzCompress(&(data->strm), BZ_FINISH);
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	return exit_status;
}

static void php_bz2_compress_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_bz2_filter_data *data = thisfilter->abstract;
		BZ2_bzCompressEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_bz2_compress_ops = {
	php_bz2_compress_filter,
	php_bz2_compress_dtor,
	"bzip2.compress"
};

/* }}} */

/* {{{ bzip2.* common factory */

static php_stream_filter *php_bz2_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_stream_filter_ops *fops = NULL;
	php_bz2_filter_data *data;
	int status = BZ_OK;

	/* Create this filter */
	data = pecalloc(1, sizeof(php_bz2_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zu bytes", sizeof(php_bz2_filter_data));
		return NULL;
	}

	/* Circular reference */
	data->strm.opaque = (void *) data;

	data->strm.bzalloc = php_bz2_alloc;
	data->strm.bzfree = php_bz2_free;
	data->persistent = persistent;
	data->strm.avail_out = data->outbuf_len = data->inbuf_len = 2048;
	data->strm.next_in = data->inbuf = (char *) pemalloc(data->inbuf_len, persistent);
	if (!data->inbuf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zu bytes", data->inbuf_len);
		pefree(data, persistent);
		return NULL;
	}
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf = (char *) pemalloc(data->outbuf_len, persistent);
	if (!data->outbuf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zu bytes", data->outbuf_len);
		pefree(data->inbuf, persistent);
		pefree(data, persistent);
		return NULL;
	}

	if (strcasecmp(filtername, "bzip2.decompress") == 0) {
		data->small_footprint = 0;
		data->expect_concatenated = 0;

		if (filterparams) {
			zval **tmpzval = NULL;

			if (Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) {

				if (SUCCESS == zend_hash_find(HASH_OF(filterparams), "concatenated", sizeof("concatenated"), (void **) &tmpzval) ) {
					zval tmp, *tmp2;

					tmp = **tmpzval;
					zval_copy_ctor(&tmp);
					tmp2 = &tmp;
					convert_to_boolean_ex(&tmp2);
					data->expect_concatenated = Z_LVAL(tmp);
					tmpzval = NULL;
				}

				zend_hash_find(HASH_OF(filterparams), "small", sizeof("small"), (void **) &tmpzval);
			} else {
				tmpzval = &filterparams;
			}

			if (tmpzval) {
				zval tmp, *tmp2;

				tmp = **tmpzval;
				zval_copy_ctor(&tmp);
				tmp2 = &tmp;
				convert_to_boolean_ex(&tmp2);
				data->small_footprint = Z_LVAL(tmp);
			}
		}

		data->status = PHP_BZ2_UNITIALIZED;
		fops = &php_bz2_decompress_ops;
	} else if (strcasecmp(filtername, "bzip2.compress") == 0) {
		int blockSize100k = PHP_BZ2_FILTER_DEFAULT_BLOCKSIZE;
		int workFactor = PHP_BZ2_FILTER_DEFAULT_WORKFACTOR;

		if (filterparams) {
			zval **tmpzval;

			if (Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) {
				if (zend_hash_find(HASH_OF(filterparams), "blocks", sizeof("blocks"), (void**) &tmpzval) == SUCCESS) {
					/* How much memory to allocate (1 - 9) x 100kb */
					zval tmp;
	
					tmp = **tmpzval;
					zval_copy_ctor(&tmp);
					convert_to_long(&tmp);
					if (Z_LVAL(tmp) < 1 || Z_LVAL(tmp) > 9) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter given for number of blocks to allocate. (%ld)", Z_LVAL_PP(tmpzval));
					} else {
						blockSize100k = Z_LVAL(tmp);
					}
				}

				if (zend_hash_find(HASH_OF(filterparams), "work", sizeof("work"), (void**) &tmpzval) == SUCCESS) {
					/* Work Factor (0 - 250) */
					zval tmp;
	
					tmp = **tmpzval;
					zval_copy_ctor(&tmp);
					convert_to_long(&tmp);

					if (Z_LVAL(tmp) < 0 || Z_LVAL(tmp) > 250) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter given for work factor. (%ld)", Z_LVAL(tmp));
					} else {
						workFactor = Z_LVAL(tmp);
					}
				}
			}
		}

		status = BZ2_bzCompressInit(&(data->strm), blockSize100k, 0, workFactor);
		fops = &php_bz2_compress_ops;
	} else {
		status = BZ_DATA_ERROR;
	}

	if (status != BZ_OK) {
		/* Unspecified (probably strm) error, let stream-filter error do its own whining */
		pefree(data->strm.next_in, persistent);
		pefree(data->strm.next_out, persistent);
		pefree(data, persistent);
		return NULL;
	}

	return php_stream_filter_alloc(fops, data, persistent);
}

php_stream_filter_factory php_bz2_filter_factory = {
	php_bz2_filter_create
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
