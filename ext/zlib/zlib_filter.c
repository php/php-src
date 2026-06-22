/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
	int windowBits;
	bool persistent;
	bool finished; /* for zlib.deflate: signals that no flush is pending */
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
			} else if (status != Z_OK && status != Z_BUF_ERROR) {
				/* Something bad happened */
				php_error_docref(NULL, E_NOTICE, "zlib: %s", zError(status));
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

static zend_result php_zlib_inflate_seek(
	php_stream *stream,
	php_stream_filter *thisfilter,
	zend_off_t offset,
	int whence
	)
{
	int status;

	if (!thisfilter || !Z_PTR(thisfilter->abstract)) {
		return FAILURE;
	}

	php_zlib_filter_data *data = Z_PTR(thisfilter->abstract);

	if (data->finished) {
		/* Stream was ended, need to reinitialize */
		status = inflateInit2(&(data->strm), data->windowBits);
		if (status != Z_OK) {
			php_error_docref(NULL, E_WARNING, "zlib.inflate: failed to reinitialize inflation state");
			return FAILURE;
		}
	} else {
		/* Stream is active, just reset it */
		status = inflateReset(&(data->strm));
		if (status != Z_OK) {
			php_error_docref(NULL, E_WARNING, "zlib.inflate: failed to reset inflation state");
			return FAILURE;
		}
	}

	/* Reset our own state */
	data->strm.next_in = data->inbuf;
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf;
	data->strm.avail_out = data->outbuf_len;
	data->finished = false;

	return SUCCESS;
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
	php_zlib_inflate_seek,
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
			int flush_mode;

			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			flush_mode = flags & PSFS_FLAG_FLUSH_CLOSE ? Z_FULL_FLUSH : (flags & PSFS_FLAG_FLUSH_INC ? Z_SYNC_FLUSH : Z_NO_FLUSH);
			data->finished = flush_mode != Z_NO_FLUSH;
			status = deflate(&(data->strm), flush_mode);
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

	if (flags & PSFS_FLAG_FLUSH_CLOSE || ((flags & PSFS_FLAG_FLUSH_INC) && !data->finished)) {
		/* Spit it out! */
		status = Z_OK;
		while (status == Z_OK) {
			status = deflate(&(data->strm), (flags & PSFS_FLAG_FLUSH_CLOSE ? Z_FINISH : Z_SYNC_FLUSH));
			data->finished = true;
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

static zend_result php_zlib_deflate_seek(
	php_stream *stream,
	php_stream_filter *thisfilter,
	zend_off_t offset,
	int whence
	)
{
	int status;

	if (!thisfilter || !Z_PTR(thisfilter->abstract)) {
		return FAILURE;
	}

	php_zlib_filter_data *data = Z_PTR(thisfilter->abstract);

	/* Reset zlib deflation state */
	status = deflateReset(&(data->strm));
	if (status != Z_OK) {
		php_error_docref(NULL, E_WARNING, "zlib.deflate: failed to reset deflation state");
		return FAILURE;
	}

	/* Reset our own state */
	data->strm.next_in = data->inbuf;
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf;
	data->strm.avail_out = data->outbuf_len;
	data->finished = true;

	return SUCCESS;
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
	php_zlib_deflate_seek,
	php_zlib_deflate_dtor,
	"zlib.deflate"
};

/* }}} */
static php_zlib_filter_data *php_zlib_filter_data_new(bool persistent)
{
	php_zlib_filter_data *data = pecalloc(1, sizeof(php_zlib_filter_data), persistent);
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
	data->persistent = persistent;
	return data;
}

static void php_zlib_filter_data_free(php_zlib_filter_data *data, bool persistent) {
	pefree(data->strm.next_in, persistent);
	pefree(data->strm.next_out, persistent);
	pefree(data, persistent);
}

static php_stream_filter *php_zlib_deflate_filter_create(zval *filter_params, bool persistent)
{
	php_stream_filter_seekable_t write_seekable = PSFS_SEEKABLE_ALWAYS;
	/* RFC 1951 Deflate */
	int level = Z_DEFAULT_COMPRESSION;
	int windowBits = -MAX_WBITS;
	int memLevel = MAX_MEM_LEVEL;


	if (filter_params) {
		zend_long level_long;

		/* filterparams can either be a scalar value to indicate compression level (shortcut method)
		   Or can be a hash containing one or more of 'window', 'memory', and/or 'level' members. */

		switch (Z_TYPE_P(filter_params)) {
			case IS_OBJECT:
			case IS_ARRAY: {
				const HashTable *ht = HASH_OF(filter_params);
				if (php_stream_filter_parse_write_seek_mode(filter_params, &write_seekable) == FAILURE) {
					return NULL;
				}

				const zval *memory_zv = zend_hash_str_find_ind(ht, ZEND_STRL("memory"));
				if (memory_zv) {
					bool failed = false;
					/* Memory Level (1 - 9) */
					const zend_long memory = zval_try_get_long(memory_zv, &failed);
					if (UNEXPECTED(failed)) {
						php_error_docref(NULL, E_WARNING, "Memory level must be of type int, %s given", zend_zval_type_name(memory_zv));
						return NULL;
					} else if (memory < 1 || memory > MAX_MEM_LEVEL) {
						php_error_docref(NULL, E_WARNING, "Memory level must be between 1 and %d, " ZEND_LONG_FMT " given", MAX_MEM_LEVEL, memory);
						return NULL;
					} else {
						memLevel = (int) memory;
					}
				}

				const zval *window_zv = zend_hash_str_find_ind(ht, ZEND_STRL("window"));
				if (window_zv) {
					bool failed = false;
					/* log-2 base of history window (9 - 15) */
					const zend_long window = zval_try_get_long(window_zv, &failed);
					if (UNEXPECTED(failed)) {
						php_error_docref(NULL, E_WARNING, "Window size must be of type int, %s given", zend_zval_type_name(window_zv));
						return NULL;
					} else if (window < -MAX_WBITS || window > MAX_WBITS + 16) {
						php_error_docref(NULL, E_WARNING, "Window size must be between %d and %d, " ZEND_LONG_FMT " given", -MAX_WBITS, MAX_WBITS + 16, window);
						return NULL;
					} else {
						windowBits = (int) window;
					}
				}

				const zval *level_zv = zend_hash_str_find_ind(ht, ZEND_STRL("level"));
				if (level_zv) {
					bool failed = false;
					level_long = zval_try_get_long(level_zv, &failed);
					if (UNEXPECTED(failed)) {
						php_error_docref(NULL, E_WARNING, "Compression level must be of type int, %s given", zend_zval_type_name(level_zv));
						return NULL;
					}

					/* Pseudo pass through to catch level validating code */
					goto factory_setlevel;
				}
				break;
			}
			case IS_STRING:
			case IS_DOUBLE:
			case IS_LONG: {
				bool failed = false;
				level_long = zval_try_get_long(filter_params, &failed);
				if (UNEXPECTED(failed)) {
					php_error_docref(NULL, E_WARNING,
					"Filter parameters for zlib.deflate filter must be of type array|int, %s given",
						zend_zval_type_name(filter_params)
					);
					return NULL;
				}
factory_setlevel:
				/* Set compression level within reason (-1 == default, 0 == none, 1-9 == least to most compression */
				if (level_long < -1 || level_long > 9) {
					php_error_docref(NULL, E_WARNING, "Compression level must be between -1 and 9, " ZEND_LONG_FMT " given", level_long);
					return NULL;
				} else {
					level = (int) level_long;
				}
				break;
			}

			default:
				php_error_docref(NULL, E_WARNING,
				"Filter parameters for zlib.deflate filter must be of type array|int, %s given",
					zend_zval_type_name(filter_params)
				);
				return NULL;
		}
	}

	php_zlib_filter_data *data = php_zlib_filter_data_new(persistent);
	if (UNEXPECTED(data == NULL)) {
		return NULL;
	}
	/* Save configuration for reset */
	data->windowBits = windowBits;

	const int status = deflateInit2(&(data->strm), level, Z_DEFLATED, windowBits, memLevel, 0);
	if (UNEXPECTED(status != Z_OK)) {
		/* Unspecified (probably strm) error, let stream-filter error do its own whining */
		php_zlib_filter_data_free(data, persistent);
		return NULL;
	}
	data->finished = true;

	return php_stream_filter_alloc(&php_zlib_deflate_ops, data, persistent, PSFS_SEEKABLE_START, write_seekable);
}


static php_stream_filter *php_zlib_inflate_filter_create(zval *filter_params, bool persistent)
{
	php_stream_filter_seekable_t write_seekable = PSFS_SEEKABLE_ALWAYS;
	int windowBits = -MAX_WBITS;

	if (filter_params) {
		if (UNEXPECTED(Z_TYPE_P(filter_params) != IS_ARRAY && Z_TYPE_P(filter_params) != IS_OBJECT)) {
			php_error_docref(NULL, E_WARNING,
				"Filter parameters for zlib.inflate filter must be of type array, %s given",
				zend_zval_type_name(filter_params)
			);
			return NULL;
		}
		const HashTable *filter_params_ht = HASH_OF(filter_params);
		/* TODO: convert php_stream_filter_parse_write_seek_mode() to take HashTable */
		if (php_stream_filter_parse_write_seek_mode(filter_params, &write_seekable) == FAILURE) {
			return NULL;
		}

		const zval *window_zv = zend_hash_str_find_ind(filter_params_ht, ZEND_STRL("window"));
		if (window_zv) {
			bool failed = false;
			/* log-2 base of history window (9 - 15) */
			const zend_long window = zval_try_get_long(window_zv, &failed);
			if (UNEXPECTED(failed)) {
				php_error_docref(NULL, E_WARNING, "Window size must be of type int, %s given", zend_zval_type_name(window_zv));
				return NULL;
			} else if (window < -MAX_WBITS || window > MAX_WBITS + 32) {
				php_error_docref(NULL, E_WARNING, "Window size must be between %d and %d, " ZEND_LONG_FMT " given", -MAX_WBITS, MAX_WBITS + 32, window);
				return NULL;
			} else {
				windowBits = (int) window;
			}
		}
	}

	php_zlib_filter_data *data = php_zlib_filter_data_new(persistent);
	if (UNEXPECTED(data == NULL)) {
		return NULL;
	}
	/* Save configuration for reset */
	data->windowBits = windowBits;

	const int status = inflateInit2(&(data->strm), windowBits);
	if (UNEXPECTED(status != Z_OK)) {
		/* Unspecified (probably strm) error, let stream-filter error do its own whining */
		php_zlib_filter_data_free(data, persistent);
		return NULL;
	}
	/* RFC 1951 Inflate */
	data->finished = false;

	return php_stream_filter_alloc(&php_zlib_inflate_ops, data, persistent, PSFS_SEEKABLE_START, write_seekable);
}

/* {{{ zlib.* common factory */
static php_stream_filter *php_zlib_filter_create(const char *filtername, zval *filterparams, bool persistent)
{
	if (strcasecmp(filtername, "zlib.inflate") == 0) {
		return php_zlib_inflate_filter_create(filterparams, persistent);
	} else if (strcasecmp(filtername, "zlib.deflate") == 0) {
		return php_zlib_deflate_filter_create(filterparams, persistent);
	} else {
		return NULL;
	}
}

const php_stream_filter_factory php_zlib_filter_factory = {
	php_zlib_filter_create
};
/* }}} */
