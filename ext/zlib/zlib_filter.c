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

#include "php.h"
#include "php_zlib.h"

/* {{{ data structure */

/* Passed as opaque in malloc callbacks */
typedef struct _php_zlib_filter_data {
	z_stream strm;
	unsigned char *inbuf;
	size_t inbuf_len;
	unsigned char *outbuf;
	size_t outbuf_len;
	int persistent;
	zend_bool finished;
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
	)
{
	php_zlib_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;

	if (!thisfilter || !Z_PTR(thisfilter->abstract)) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_zlib_filter_data *)(Z_PTR(thisfilter->abstract));

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head);

		while (bin < (unsigned int) bucket->buflen && !data->finished) {

			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			status = inflate(&(data->strm), flags & PSFS_FLAG_FLUSH_CLOSE ? Z_FINISH : Z_SYNC_FLUSH);
			if (status == Z_STREAM_END) {
				inflateEnd(&(data->strm));
				data->finished = '\1';
				exit_status = PSFS_PASS_ON;
			} else if (status != Z_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket);
				/* reset these because despite the error the filter may be used again */
				data->strm.next_in = data->inbuf;
				data->strm.avail_in = 0;
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
			bin += desired;

			if (data->strm.avail_out < data->outbuf_len) {
				php_stream_bucket *out_bucket;
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;
				out_bucket = php_stream_bucket_new(
					stream, estrndup((char *) data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, out_bucket);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}

		}
		consumed += bucket->buflen;
		php_stream_bucket_delref(bucket);
	}

	if (!data->finished && flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = Z_OK;
		while (status == Z_OK) {
			status = inflate(&(data->strm), Z_FINISH);
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(
					stream, estrndup((char *) data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, bucket);
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

static void php_zlib_inflate_dtor(php_stream_filter *thisfilter)
{
	if (thisfilter && Z_PTR(thisfilter->abstract)) {
		php_zlib_filter_data *data = Z_PTR(thisfilter->abstract);
		if (!data->finished) {
			inflateEnd(&(data->strm));
		}
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static const php_stream_filter_ops php_zlib_inflate_ops = {
	php_zlib_inflate_filter,
	php_zlib_inflate_dtor,
	"zlib.inflate"
};
/* }}} */

/* {{{ zlib.deflate filter implementation */

static php_stream_filter_status_t php_zlib_deflate_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	)
{
	php_zlib_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;

	if (!thisfilter || !Z_PTR(thisfilter->abstract)) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_zlib_filter_data *)(Z_PTR(thisfilter->abstract));

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = buckets_in->head;

		bucket = php_stream_bucket_make_writeable(bucket);

		while (bin < (unsigned int) bucket->buflen) {
			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			status = deflate(&(data->strm), flags & PSFS_FLAG_FLUSH_CLOSE ? Z_FULL_FLUSH : (flags & PSFS_FLAG_FLUSH_INC ? Z_SYNC_FLUSH : Z_NO_FLUSH));
			if (status != Z_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
			bin += desired;

			if (data->strm.avail_out < data->outbuf_len) {
				php_stream_bucket *out_bucket;
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				out_bucket = php_stream_bucket_new(
					stream, estrndup((char *) data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, out_bucket);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
		consumed += bucket->buflen;
		php_stream_bucket_delref(bucket);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = Z_OK;
		while (status == Z_OK) {
			status = deflate(&(data->strm), Z_FINISH);
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(
					stream, estrndup((char *) data->outbuf, bucketlen), bucketlen, 1, 0);
				php_stream_bucket_append(buckets_out, bucket);
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

static void php_zlib_deflate_dtor(php_stream_filter *thisfilter)
{
	if (thisfilter && Z_PTR(thisfilter->abstract)) {
		php_zlib_filter_data *data = Z_PTR(thisfilter->abstract);
		deflateEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static const php_stream_filter_ops php_zlib_deflate_ops = {
	php_zlib_deflate_filter,
	php_zlib_deflate_dtor,
	"zlib.deflate"
};

/* }}} */

/* {{{ zlib.* common factory */

static php_stream_filter *php_zlib_filter_create(const char *filtername, zval *filterparams, uint8_t persistent)
{
	const php_stream_filter_ops *fops = NULL;
	php_zlib_filter_data *data;
	int status;

	/* Create this filter */
	data = pecalloc(1, sizeof(php_zlib_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL, E_WARNING, "Failed allocating %zd bytes", sizeof(php_zlib_filter_data));
		return NULL;
	}

	/* Circular reference */
	data->strm.opaque = (voidpf) data;

	data->strm.zalloc = (alloc_func) php_zlib_alloc;
	data->strm.zfree = (free_func) php_zlib_free;
	data->strm.avail_out = data->outbuf_len = data->inbuf_len = 0x8000;
	data->strm.next_in = data->inbuf = (Bytef *) pemalloc(data->inbuf_len, persistent);
	if (!data->inbuf) {
		php_error_docref(NULL, E_WARNING, "Failed allocating %zd bytes", data->inbuf_len);
		pefree(data, persistent);
		return NULL;
	}
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf = (Bytef *) pemalloc(data->outbuf_len, persistent);
	if (!data->outbuf) {
		php_error_docref(NULL, E_WARNING, "Failed allocating %zd bytes", data->outbuf_len);
		pefree(data->inbuf, persistent);
		pefree(data, persistent);
		return NULL;
	}

	data->strm.data_type = Z_ASCII;

	if (strcasecmp(filtername, "zlib.inflate") == 0) {
		int windowBits = -MAX_WBITS;

		if (filterparams) {
			zval *tmpzval;

			if ((Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) &&
				(tmpzval = zend_hash_str_find(HASH_OF(filterparams), "window", sizeof("window") - 1))) {
				/* log-2 base of history window (9 - 15) */
				zend_long tmp = zval_get_long(tmpzval);
				if (tmp < -MAX_WBITS || tmp > MAX_WBITS + 32) {
					php_error_docref(NULL, E_WARNING, "Invalid parameter give for window size. (" ZEND_LONG_FMT ")", tmp);
				} else {
					windowBits = tmp;
				}
			}
		}

		/* RFC 1951 Inflate */
		data->finished = '\0';
		status = inflateInit2(&(data->strm), windowBits);
		fops = &php_zlib_inflate_ops;
	} else if (strcasecmp(filtername, "zlib.deflate") == 0) {
		/* RFC 1951 Deflate */
		int level = Z_DEFAULT_COMPRESSION;
		int windowBits = -MAX_WBITS;
		int memLevel = MAX_MEM_LEVEL;


		if (filterparams) {
			zval *tmpzval;
			zend_long tmp;

			/* filterparams can either be a scalar value to indicate compression level (shortcut method)
               Or can be a hash containing one or more of 'window', 'memory', and/or 'level' members. */

			switch (Z_TYPE_P(filterparams)) {
				case IS_ARRAY:
				case IS_OBJECT:
					if ((tmpzval = zend_hash_str_find(HASH_OF(filterparams), "memory", sizeof("memory") -1))) {
						/* Memory Level (1 - 9) */
						tmp = zval_get_long(tmpzval);
						if (tmp < 1 || tmp > MAX_MEM_LEVEL) {
							php_error_docref(NULL, E_WARNING, "Invalid parameter give for memory level. (" ZEND_LONG_FMT ")", tmp);
						} else {
							memLevel = tmp;
						}
					}

					if ((tmpzval = zend_hash_str_find(HASH_OF(filterparams), "window", sizeof("window") - 1))) {
						/* log-2 base of history window (9 - 15) */
						tmp = zval_get_long(tmpzval);
						if (tmp < -MAX_WBITS || tmp > MAX_WBITS + 16) {
							php_error_docref(NULL, E_WARNING, "Invalid parameter give for window size. (" ZEND_LONG_FMT ")", tmp);
						} else {
							windowBits = tmp;
						}
					}

					if ((tmpzval = zend_hash_str_find(HASH_OF(filterparams), "level", sizeof("level") - 1))) {
						tmp = zval_get_long(tmpzval);

						/* Pseudo pass through to catch level validating code */
						goto factory_setlevel;
					}
					break;
				case IS_STRING:
				case IS_DOUBLE:
				case IS_LONG:
					tmp = zval_get_long(filterparams);
factory_setlevel:
					/* Set compression level within reason (-1 == default, 0 == none, 1-9 == least to most compression */
					if (tmp < -1 || tmp > 9) {
						php_error_docref(NULL, E_WARNING, "Invalid compression level specified. (" ZEND_LONG_FMT ")", tmp);
					} else {
						level = tmp;
					}
					break;
				default:
					php_error_docref(NULL, E_WARNING, "Invalid filter parameter, ignored");
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

const php_stream_filter_factory php_zlib_filter_factory = {
	php_zlib_filter_create
};
/* }}} */
