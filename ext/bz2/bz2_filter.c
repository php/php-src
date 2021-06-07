/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Sara Golemon (pollita@php.net)                              |
   +----------------------------------------------------------------------+
*/

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
	bz_stream strm;
	char *inbuf;
	char *outbuf;
	size_t inbuf_len;
	size_t outbuf_len;

	enum strm_status status;              /* Decompress option */
	unsigned int small_footprint : 1;     /* Decompress option */
	unsigned int expect_concatenated : 1; /* Decompress option */
	unsigned int is_flushed : 1;          /* only for compression */

	int persistent;
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
	)
{
	php_bz2_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	bz_stream *streamp;

	if (!Z_PTR(thisfilter->abstract)) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_bz2_filter_data *)Z_PTR(thisfilter->abstract);
	streamp = &(data->strm);

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head);
		while (bin < bucket->buflen) {
			if (data->status == PHP_BZ2_UNITIALIZED) {
				status = BZ2_bzDecompressInit(streamp, 0, data->small_footprint);

				if (BZ_OK != status) {
					php_stream_bucket_delref(bucket);
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
				php_stream_bucket_delref(bucket);
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
				out_bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, out_bucket);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			} else if (status == BZ_STREAM_END && data->strm.avail_out >= data->outbuf_len) {
				/* no more data to decompress, and nothing was spat out */
				php_stream_bucket_delref(bucket);
				return PSFS_PASS_ON;
			}
		}

		php_stream_bucket_delref(bucket);
	}

	if ((data->status == PHP_BZ2_RUNNING) && (flags & PSFS_FLAG_FLUSH_CLOSE)) {
		/* Spit it out! */
		status = BZ_OK;
		while (status == BZ_OK) {
			status = BZ2_bzDecompress(&(data->strm));
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, bucket);
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

static void php_bz2_decompress_dtor(php_stream_filter *thisfilter)
{
	if (thisfilter && Z_PTR(thisfilter->abstract)) {
		php_bz2_filter_data *data = Z_PTR(thisfilter->abstract);
		if (data->status == PHP_BZ2_RUNNING) {
			BZ2_bzDecompressEnd(&(data->strm));
		}
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static const php_stream_filter_ops php_bz2_decompress_ops = {
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
	)
{
	php_bz2_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;

	if (!Z_PTR(thisfilter->abstract)) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_bz2_filter_data *)Z_PTR(thisfilter->abstract);

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head);

		while (bin < bucket->buflen) {
			int flush_mode;

			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			flush_mode = flags & PSFS_FLAG_FLUSH_CLOSE ? BZ_FINISH : (flags & PSFS_FLAG_FLUSH_INC ? BZ_FLUSH : BZ_RUN);
			data->is_flushed = flush_mode != BZ_RUN;
			status = BZ2_bzCompress(&(data->strm), flush_mode);
			if (status != BZ_RUN_OK && status != BZ_FLUSH_OK && status != BZ_FINISH_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket);
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

				out_bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, out_bucket);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
		php_stream_bucket_delref(bucket);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE || ((flags & PSFS_FLAG_FLUSH_INC) && !data->is_flushed)) {
		/* Spit it out! */
		do  {
			status = BZ2_bzCompress(&(data->strm), (flags & PSFS_FLAG_FLUSH_CLOSE ? BZ_FINISH : BZ_FLUSH));
			data->is_flushed = 1;
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, bucket);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		} while (status == (flags & PSFS_FLAG_FLUSH_CLOSE ? BZ_FINISH_OK : BZ_FLUSH_OK));
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	return exit_status;
}

static void php_bz2_compress_dtor(php_stream_filter *thisfilter)
{
	if (Z_PTR(thisfilter->abstract)) {
		php_bz2_filter_data *data = Z_PTR(thisfilter->abstract);
		BZ2_bzCompressEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static const php_stream_filter_ops php_bz2_compress_ops = {
	php_bz2_compress_filter,
	php_bz2_compress_dtor,
	"bzip2.compress"
};

/* }}} */

/* {{{ bzip2.* common factory */

static php_stream_filter *php_bz2_filter_create(const char *filtername, zval *filterparams, uint8_t persistent)
{
	const php_stream_filter_ops *fops = NULL;
	php_bz2_filter_data *data;
	int status = BZ_OK;

	/* Create this filter */
	data = pecalloc(1, sizeof(php_bz2_filter_data), persistent);

	/* Circular reference */
	data->strm.opaque = (void *) data;

	data->strm.bzalloc = php_bz2_alloc;
	data->strm.bzfree = php_bz2_free;
	data->persistent = persistent;
	data->strm.avail_out = data->outbuf_len = data->inbuf_len = 2048;
	data->strm.next_in = data->inbuf = (char *) pemalloc(data->inbuf_len, persistent);
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf = (char *) pemalloc(data->outbuf_len, persistent);

	if (strcasecmp(filtername, "bzip2.decompress") == 0) {
		data->small_footprint = 0;
		data->expect_concatenated = 0;

		if (filterparams) {
			zval *tmpzval = NULL;

			if (Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) {
				if ((tmpzval = zend_hash_str_find(HASH_OF(filterparams), "concatenated", sizeof("concatenated")-1))) {
					data->expect_concatenated = zend_is_true(tmpzval);
					tmpzval = NULL;
				}

				tmpzval = zend_hash_str_find(HASH_OF(filterparams), "small", sizeof("small")-1);
			} else {
				tmpzval = filterparams;
			}

			if (tmpzval) {
				data->small_footprint = zend_is_true(tmpzval);
			}
		}

		data->status = PHP_BZ2_UNITIALIZED;
		fops = &php_bz2_decompress_ops;
	} else if (strcasecmp(filtername, "bzip2.compress") == 0) {
		int blockSize100k = PHP_BZ2_FILTER_DEFAULT_BLOCKSIZE;
		int workFactor = PHP_BZ2_FILTER_DEFAULT_WORKFACTOR;

		if (filterparams) {
			zval *tmpzval;

			if (Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) {
				if ((tmpzval = zend_hash_str_find(HASH_OF(filterparams), "blocks", sizeof("blocks")-1))) {
					/* How much memory to allocate (1 - 9) x 100kb */
					zend_long blocks = zval_get_long(tmpzval);
					if (blocks < 1 || blocks > 9) {
						php_error_docref(NULL, E_WARNING, "Invalid parameter given for number of blocks to allocate. (" ZEND_LONG_FMT ")", blocks);
					} else {
						blockSize100k = (int) blocks;
					}
				}

				if ((tmpzval = zend_hash_str_find(HASH_OF(filterparams), "work", sizeof("work")-1))) {
					/* Work Factor (0 - 250) */
					zend_long work = zval_get_long(tmpzval);
					if (work < 0 || work > 250) {
						php_error_docref(NULL, E_WARNING, "Invalid parameter given for work factor. (" ZEND_LONG_FMT ")", work);
					} else {
						workFactor = (int) work;
					}
				}
			}
		}

		status = BZ2_bzCompressInit(&(data->strm), blockSize100k, 0, workFactor);
		data->is_flushed = 1;
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

const php_stream_filter_factory php_bz2_filter_factory = {
	php_bz2_filter_create
};
/* }}} */
