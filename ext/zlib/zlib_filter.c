/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
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

#include "php.h"
#include "php_zlib.h"

/* {{{ data structure */

/* Passed as opaque in malloc callbacks */
typedef struct _php_zlib_filter_data {
	int persistent;
	z_stream strm;
	char *inbuf;
	size_t inbuf_len;
	char *outbuf;
	size_t outbuf_len;
} php_zlib_filter_data;

/* }}} */

/* {{{ Memory management wrappers */

static voidpf php_zlib_alloc(voidpf opaque, uInt items, uInt size)
{
	return (voidpf)safe_pemalloc(items, size, 0, ((php_zlib_filter_data*)opaque)->persistent);
}

static void php_zlib_free(voidpf opaque, voidpf address)
{
	pefree((void*)address, ((php_zlib_filter_data*)opaque)->persistent);
}
/* }}} */

/* {{{ zlib.inflate filter implementation */

static php_stream_filter_status_t php_zlib_inflate_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_zlib_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0, original_out, original_in;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	z_stream *streamp;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_zlib_filter_data *)(thisfilter->abstract);
	streamp = &(data->strm);
	original_in = data->strm.total_in;
	original_out = data->strm.total_out;

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = buckets_in->head;

		if (bucket->buf_type == IS_UNICODE) {
			/* inflation not allowed for unicode data */
			return PSFS_ERR_FATAL;
		}

		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		while (bin < bucket->buflen) {
			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf.s + bin, desired);
			data->strm.avail_in = desired;

			status = inflate(&(data->strm), flags & PSFS_FLAG_FLUSH_CLOSE ? Z_FINISH : Z_SYNC_FLUSH);
			if (status != Z_OK && status != Z_STREAM_END) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
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
		consumed += bucket->buflen;
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = Z_OK;
		while (status == Z_OK) {
			status = inflate(&(data->strm), Z_FINISH);
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

static void php_zlib_inflate_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_zlib_filter_data *data = thisfilter->abstract;
		inflateEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_zlib_inflate_ops = {
	php_zlib_inflate_filter,
	php_zlib_inflate_dtor,
	"zlib.inflate",
	PSFO_FLAG_ACCEPTS_STRING | PSFO_FLAG_OUTPUTS_STRING
};
/* }}} */

/* {{{ zlib.inflate filter implementation */

static php_stream_filter_status_t php_zlib_deflate_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_zlib_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0, original_out, original_in;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	z_stream *streamp;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_zlib_filter_data *)(thisfilter->abstract);
	streamp = &(data->strm);
	original_in = data->strm.total_in;
	original_out = data->strm.total_out;

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = buckets_in->head;

		if (bucket->buf_type == IS_UNICODE) {
			/* inflation not allowed for unicode data */
			return PSFS_ERR_FATAL;
		}

		bucket = php_stream_bucket_make_writeable(bucket TSRMLS_CC);

		while (bin < bucket->buflen) {
			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf.s + bin, desired);
			data->strm.avail_in = desired;

			status = deflate(&(data->strm), flags & PSFS_FLAG_FLUSH_CLOSE ? Z_FULL_FLUSH : (flags & PSFS_FLAG_FLUSH_INC ? Z_SYNC_FLUSH : Z_NO_FLUSH));
			if (status != Z_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
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
		consumed += bucket->buflen;
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = Z_OK;
		while (status == Z_OK) {
			status = deflate(&(data->strm), Z_FINISH);
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

static void php_zlib_deflate_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_zlib_filter_data *data = thisfilter->abstract;
		deflateEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_zlib_deflate_ops = {
	php_zlib_deflate_filter,
	php_zlib_deflate_dtor,
	"zlib.deflate",
	PSFO_FLAG_ACCEPTS_STRING | PSFO_FLAG_OUTPUTS_STRING
};

/* }}} */

/* {{{ zlib.* common factory */

static php_stream_filter *php_zlib_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_stream_filter_ops *fops = NULL;
	php_zlib_filter_data *data;
	int status;

	/* Create this filter */
	data = pecalloc(1, sizeof(php_zlib_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %ld bytes.", sizeof(php_zlib_filter_data));
		return NULL;
	}

	/* Circular reference */
	data->strm.opaque = (voidpf) data;

	data->strm.zalloc = (alloc_func) php_zlib_alloc;
	data->strm.zfree = (free_func) php_zlib_free;
	data->strm.avail_out = data->outbuf_len = data->inbuf_len = 2048;
	data->strm.next_in = data->inbuf = (Bytef *) pemalloc(data->inbuf_len, persistent);
	if (!data->inbuf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %ld bytes.", data->inbuf_len);
		pefree(data, persistent);
		return NULL;
	}
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf = (Bytef *) pemalloc(data->outbuf_len, persistent);
	if (!data->outbuf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %ld bytes.", data->outbuf_len);
		pefree(data->inbuf, persistent);
		pefree(data, persistent);
		return NULL;
	}
		
	data->strm.data_type = Z_ASCII;

	if (strcasecmp(filtername, "zlib.inflate") == 0) {
		int windowBits = -MAX_WBITS;

		if (filterparams) {
			zval **tmpzval;

			if ((Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) &&
				zend_hash_find(HASH_OF(filterparams), "window", sizeof("window"), (void **) &tmpzval) == SUCCESS) {
				/* log-2 base of history window (9 - 15) */
				SEPARATE_ZVAL(tmpzval);
				convert_to_long_ex(tmpzval);
				if (Z_LVAL_PP(tmpzval) < -MAX_WBITS || Z_LVAL_PP(tmpzval) > MAX_WBITS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter give for window size. (%ld)", Z_LVAL_PP(tmpzval));
				} else {
					windowBits = Z_LVAL_PP(tmpzval);
				}
				zval_ptr_dtor(tmpzval);
			}
		}

		/* RFC 1951 Inflate */
		status = inflateInit2(&(data->strm), windowBits);
		fops = &php_zlib_inflate_ops;
	} else if (strcasecmp(filtername, "zlib.deflate") == 0) {
		/* RFC 1951 Deflate */
		int level = Z_DEFAULT_COMPRESSION;
		int windowBits = -MAX_WBITS;
		int memLevel = MAX_MEM_LEVEL;


		if (filterparams) {
			zval **tmpzval;

			/* filterparams can either be a scalar value to indicate compression level (shortcut method)
               Or can be a hash containing one or more of 'window', 'memory', and/or 'level' members. */

			switch (Z_TYPE_P(filterparams)) {
				case IS_ARRAY:
				case IS_OBJECT:
					if (zend_hash_find(HASH_OF(filterparams), "memory", sizeof("memory"), (void**) &tmpzval) == SUCCESS) {
						/* Memory Level (1 - 9) */
						SEPARATE_ZVAL(tmpzval);
						convert_to_long_ex(tmpzval);
						if (Z_LVAL_PP(tmpzval) < 1 || Z_LVAL_PP(tmpzval) > MAX_MEM_LEVEL) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter give for memory level. (%ld)", Z_LVAL_PP(tmpzval));
						} else {
							memLevel = Z_LVAL_PP(tmpzval);
						}
						zval_ptr_dtor(tmpzval);
					}

					if (zend_hash_find(HASH_OF(filterparams), "window", sizeof("window"), (void**) &tmpzval) == SUCCESS) {
						/* log-2 base of history window (9 - 15) */
						SEPARATE_ZVAL(tmpzval);
						convert_to_long_ex(tmpzval);
						if (Z_LVAL_PP(tmpzval) < -MAX_WBITS || Z_LVAL_PP(tmpzval) > MAX_WBITS) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter give for window size. (%ld)", Z_LVAL_PP(tmpzval));
						} else {
							windowBits = Z_LVAL_PP(tmpzval);
						}
						zval_ptr_dtor(tmpzval);
					}

					if (zend_hash_find(HASH_OF(filterparams), "level", sizeof("level"), (void**) &tmpzval) == SUCCESS) {
						/* Psuedo pass through to catch level validating code */
						goto factory_setlevel;
					}
					break;
				case IS_STRING:
				case IS_DOUBLE:
				case IS_LONG:
					tmpzval = &filterparams;
factory_setlevel:
					/* Set compression level within reason (-1 == default, 0 == none, 1-9 == least to most compression */
					SEPARATE_ZVAL(tmpzval);
					convert_to_long_ex(tmpzval);
					if (Z_LVAL_PP(tmpzval) < -1 || Z_LVAL_PP(tmpzval) > 9) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid compression level specified. (%ld)", Z_LVAL_PP(tmpzval));
					} else {
						level = Z_LVAL_PP(tmpzval);
					}
					zval_ptr_dtor(tmpzval);
					break;
				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid filter parameter, ignored.");
			}
		}
		status = deflateInit2(&(data->strm), level, Z_DEFLATED, windowBits, memLevel, 0);
		fops = &php_zlib_deflate_ops;
	} else {
		status = Z_DATA_ERROR;
	}

	if (status != Z_OK) {
		/* Unspecified (probably strm) error, let stream-filter error do its own whining */
		pefree(data->strm.next_in, persistent);
		pefree(data->strm.next_out, persistent);
		pefree(data, persistent);
		return NULL;
	}

	return php_stream_filter_alloc(fops, data, persistent);
}

php_stream_filter_factory php_zlib_filter_factory = {
	php_zlib_filter_create
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
