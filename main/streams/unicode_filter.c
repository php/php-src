/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
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
#include <unicode/ucnv.h>

/* {{{ data structure */
typedef struct _php_unicode_filter_data {
	char is_persistent;
	UConverter *conv;

	char to_unicode;
} php_unicode_filter_data;
/* }}} */

/* {{{ unicode.* filter implementation */

/* unicode.to.* -- Expects String -- Returns Unicode */
static php_stream_filter_status_t php_unicode_to_string_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_unicode_filter_data *data;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	size_t consumed = 0;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_unicode_filter_data *)(thisfilter->abstract);
	while (buckets_in->head) {
		php_stream_bucket *bucket = buckets_in->head;
		UChar *src = bucket->buf.u;

		php_stream_bucket_unlink(bucket TSRMLS_CC);
		if (bucket->buf_type != IS_UNICODE) {
			/* Already ASCII, can't really do anything with it */
			consumed += bucket->buflen;
			php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
			exit_status = PSFS_PASS_ON;
			continue;
		}

		while (src < (bucket->buf.u + bucket->buflen)) {
			int remaining = bucket->buflen - (src - bucket->buf.u);
			char *destp, *destbuf;
			int32_t destlen = UCNV_GET_MAX_BYTES_FOR_STRING(remaining, ucnv_getMaxCharSize(data->conv));
			UErrorCode errCode = U_ZERO_ERROR;
			php_stream_bucket *new_bucket;

			destp = destbuf = (char *)pemalloc(destlen, data->is_persistent);

			ucnv_fromUnicode(data->conv, &destp, destbuf + destlen, (const UChar**)&src, src + remaining, NULL, FALSE, &errCode);
			/* UTODO: Error catching */
			new_bucket = php_stream_bucket_new(stream, destbuf, destp - destbuf, 1, data->is_persistent TSRMLS_CC);
			php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);
			exit_status = PSFS_PASS_ON;
		}
		consumed += bucket->buflen;
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		UErrorCode errCode = U_ZERO_ERROR;
		char d[64], *dest = d, *destp = d + 64;
		/* Spit it out! */

		ucnv_fromUnicode(data->conv, &dest, destp, NULL, NULL, NULL, TRUE, &errCode);
		/* UTODO: Error catching */
		if (dest > d) {
			php_stream_bucket *bucket = php_stream_bucket_new(stream, d, dest - d, 0, 0 TSRMLS_CC);
			php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
			exit_status = PSFS_PASS_ON;
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return exit_status;
}

/* unicode.from.* -- Expects Unicode -- Returns String */
static php_stream_filter_status_t php_unicode_from_string_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_unicode_filter_data *data;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	size_t consumed = 0;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_unicode_filter_data *)(thisfilter->abstract);
	while (buckets_in->head) {
		php_stream_bucket *bucket = buckets_in->head;
		char *src = bucket->buf.s;

		php_stream_bucket_unlink(bucket TSRMLS_CC);
		if (bucket->buf_type == IS_UNICODE) {
			/* already in unicode, nothing to do */
			consumed += bucket->buflen;
			php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
			exit_status = PSFS_PASS_ON;
			continue;
		}

		while (src < (bucket->buf.s + bucket->buflen)) {
			int remaining = bucket->buflen - (src - bucket->buf.s);
			UChar *destp, *destbuf;
			int32_t destlen = UCNV_GET_MAX_BYTES_FOR_STRING(remaining, ucnv_getMaxCharSize(data->conv));
			UErrorCode errCode = U_ZERO_ERROR;
			php_stream_bucket *new_bucket;

			if ((destlen & 1) != 0) {
				destlen++;
			}

			destp = destbuf = (UChar *)pemalloc(destlen, data->is_persistent);

			ucnv_toUnicode(data->conv, &destp, (UChar*)((char*)destbuf + destlen), (const char**)&src, src + remaining, NULL, FALSE, &errCode);

			if (errCode != U_ZERO_ERROR) {
				pefree(destbuf, data->is_persistent);
				break;
			}

			new_bucket = php_stream_bucket_new_unicode(stream, destbuf, destp - destbuf, 1, data->is_persistent TSRMLS_CC);
			php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);
			exit_status = PSFS_PASS_ON;
		}
		consumed += bucket->buflen;
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		UErrorCode errCode = U_ZERO_ERROR;
		UChar d[64], *dest = d, *destp = d + 64;
		/* Spit it out! */

		ucnv_toUnicode(data->conv, &dest, destp, NULL, NULL, NULL, TRUE, &errCode);
		/* UTODO: Error catching */
		if (dest > d) {
			php_stream_bucket *bucket = php_stream_bucket_new_unicode(stream, d, dest - d, 0, 0 TSRMLS_CC);
			php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
			exit_status = PSFS_PASS_ON;
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return exit_status;
}

/* unicode.tidy.* -- Expects anything -- Returns whatever is preferred by subsequent filters
   Can be used to "magically" fix-up bucket messes */
static php_stream_filter_status_t php_unicode_tidy_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_unicode_filter_data *data;
	int prefer_unicode;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	prefer_unicode = php_stream_filter_output_prefer_unicode(thisfilter);
	data = (php_unicode_filter_data *)(thisfilter->abstract);

	if (prefer_unicode) {
		if (!data->to_unicode) {
			ucnv_resetToUnicode(data->conv);
			data->to_unicode = prefer_unicode;
		}
		return php_unicode_from_string_filter(stream, thisfilter, buckets_in, buckets_out, bytes_consumed, flags TSRMLS_CC);
	} else {
		if (data->to_unicode) {
			ucnv_resetFromUnicode(data->conv);
			data->to_unicode = prefer_unicode;
		}
		return php_unicode_to_string_filter(stream, thisfilter, buckets_in, buckets_out, bytes_consumed, flags TSRMLS_CC);
	}
}

static void php_unicode_filter_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_unicode_filter_data *data = (php_unicode_filter_data *)thisfilter->abstract;
		ucnv_close(data->conv);
		pefree(data, data->is_persistent);
	}
}

php_stream_filter_ops php_unicode_to_string_filter_ops = {
	php_unicode_to_string_filter,
	php_unicode_filter_dtor,
	"unicode.to.*",
	PSFO_FLAG_ACCEPTS_UNICODE | PSFO_FLAG_OUTPUTS_STRING
};

php_stream_filter_ops php_unicode_from_string_filter_ops = {
	php_unicode_from_string_filter,
	php_unicode_filter_dtor,
	"unicode.from.*",
	PSFO_FLAG_ACCEPTS_STRING | PSFO_FLAG_OUTPUTS_UNICODE
};

php_stream_filter_ops php_unicode_tidy_filter_ops = {
	php_unicode_tidy_filter,
	php_unicode_filter_dtor,
	"unicode.tidy.*",
	PSFO_FLAG_ACCEPTS_ANY | PSFO_FLAG_OUTPUTS_ANY
};
/* }}} */

/* {{{ unicode.* factory */

static php_stream_filter *php_unicode_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_unicode_filter_data *data;
	const char *charset, *direction;
	php_stream_filter_ops *fops;
	UErrorCode ucnvError = U_ZERO_ERROR;
	/* Note: from_error_mode means from unicode to charset.  from filter means from charset to unicode */
	uint16_t err_mode = UG(from_error_mode);
	char to_unicode = 0;
	zval **tmpzval;

	if (strncasecmp(filtername, "unicode.", sizeof("unicode.") - 1)) {
		/* Never happens */
		return NULL;
	}

	direction = filtername + sizeof("unicode.") - 1;
	if (strncmp(direction, "to.", sizeof("to.") - 1) == 0) {
		fops = &php_unicode_to_string_filter_ops;
		charset = direction + sizeof("to.") - 1;
	} else if (strncmp(direction, "from.", sizeof("from.") - 1) == 0) {
		fops = &php_unicode_from_string_filter_ops;
		charset = direction + sizeof("from.") - 1;
		to_unicode = 1;
		err_mode = UG(to_error_mode);
	} else if (strncmp(direction, "tidy.", sizeof("tidy.") - 1) == 0) {
		fops = &php_unicode_tidy_filter_ops;
		charset = direction + sizeof("tidy.") - 1;
	} else if (strcmp(direction, "tidy") == 0) {
		fops = &php_unicode_tidy_filter_ops;
		charset = "utf8";
	} else {
		/* Shouldn't happen */
		return NULL;
	}

	/* Create this filter */
	data = (php_unicode_filter_data *)pecalloc(1, sizeof(php_unicode_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed allocating %d bytes", sizeof(php_unicode_filter_data));
		return NULL;
	}

	data->conv = ucnv_open(charset, &ucnvError);
	data->to_unicode = to_unicode;
	if (!data->conv) {
		char *reason = "Unknown Error";
		pefree(data, persistent);
		switch (ucnvError) {
			case U_MEMORY_ALLOCATION_ERROR:
				reason = "unable to allocate memory";
				break;
			case U_FILE_ACCESS_ERROR:
				reason = "file access error";
				break;
			default:
				;
		}
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to open charset converter, %s", reason);
		return NULL;
	}

	if (filterparams &&
		Z_TYPE_P(filterparams) == IS_ARRAY &&
		zend_ascii_hash_find(Z_ARRVAL_P(filterparams), "error_mode", sizeof("error_mode"), (void**)&tmpzval) == SUCCESS &&
		tmpzval && *tmpzval) {
		if (Z_TYPE_PP(tmpzval) == IS_LONG) {
			err_mode = Z_LVAL_PP(tmpzval);
		} else {
			zval copyval = **tmpzval;
			zval_copy_ctor(&copyval);
			convert_to_long(&copyval);
			err_mode = Z_LVAL(copyval);
		}
	}

	zend_set_converter_error_mode(data->conv, to_unicode ? ZEND_TO_UNICODE : ZEND_FROM_UNICODE, err_mode);
	if (!to_unicode) {
		UChar *freeme = NULL;
		UChar *subst_char = UG(from_subst_char);

		if (filterparams &&
			Z_TYPE_P(filterparams) == IS_ARRAY &&
			zend_ascii_hash_find(Z_ARRVAL_P(filterparams), "subst_char", sizeof("subst_char"), (void**)&tmpzval) == SUCCESS &&
			tmpzval && *tmpzval) {
			if (Z_TYPE_PP(tmpzval) == IS_UNICODE) {
				subst_char = Z_USTRVAL_PP(tmpzval);
			} else {
				zval copyval = **tmpzval;
				zval_copy_ctor(&copyval);
				convert_to_unicode(&copyval);
				subst_char = freeme = Z_USTRVAL(copyval);
			}
		}

		zend_set_converter_subst_char(data->conv, subst_char);

		if (freeme) {
			efree(freeme);
		}
	}

	return php_stream_filter_alloc(fops, data, persistent);
}

php_stream_filter_factory php_unicode_filter_factory = {
	php_unicode_filter_create
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

