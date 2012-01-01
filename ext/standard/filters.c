/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   | Sara Golemon (pollita@php.net)                                       |
   | Moriyoshi Koizumi (moriyoshi@php.net)                                |
   | Marcus Boerger (helly@php.net)                                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_smart_str.h"

/* {{{ rot13 stream filter implementation */
static char rot13_from[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char rot13_to[] = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

static php_stream_filter_status_t strfilter_rot13_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t consumed = 0;

	while (buckets_in->head) {
		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		
		php_strtr(bucket->buf, bucket->buflen, rot13_from, rot13_to, 52);
		consumed += bucket->buflen;
		
		php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	
	return PSFS_PASS_ON;
}

static php_stream_filter_ops strfilter_rot13_ops = {
	strfilter_rot13_filter,
	NULL,
	"string.rot13"
};

static php_stream_filter *strfilter_rot13_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	return php_stream_filter_alloc(&strfilter_rot13_ops, NULL, persistent);
}

static php_stream_filter_factory strfilter_rot13_factory = {
	strfilter_rot13_create
};
/* }}} */

/* {{{ string.toupper / string.tolower stream filter implementation */
static char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
static char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static php_stream_filter_status_t strfilter_toupper_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t consumed = 0;

	while (buckets_in->head) {
		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		
		php_strtr(bucket->buf, bucket->buflen, lowercase, uppercase, 26);
		consumed += bucket->buflen;
		
		php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return PSFS_PASS_ON;
}

static php_stream_filter_status_t strfilter_tolower_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t consumed = 0;

	while (buckets_in->head) {
		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		
		php_strtr(bucket->buf, bucket->buflen, uppercase, lowercase, 26);
		consumed += bucket->buflen;
		
		php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return PSFS_PASS_ON;
}

static php_stream_filter_ops strfilter_toupper_ops = {
	strfilter_toupper_filter,
	NULL,
	"string.toupper"
};

static php_stream_filter_ops strfilter_tolower_ops = {
	strfilter_tolower_filter,
	NULL,
	"string.tolower"
};

static php_stream_filter *strfilter_toupper_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	return php_stream_filter_alloc(&strfilter_toupper_ops, NULL, persistent);
}

static php_stream_filter *strfilter_tolower_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	return php_stream_filter_alloc(&strfilter_tolower_ops, NULL, persistent);
}

static php_stream_filter_factory strfilter_toupper_factory = {
	strfilter_toupper_create
};

static php_stream_filter_factory strfilter_tolower_factory = {
	strfilter_tolower_create
};
/* }}} */

/* {{{ strip_tags filter implementation */
typedef struct _php_strip_tags_filter {
	const char *allowed_tags;
	int allowed_tags_len;
	int state;
	int persistent;
} php_strip_tags_filter;

static int php_strip_tags_filter_ctor(php_strip_tags_filter *inst, const char *allowed_tags, int allowed_tags_len, int persistent)
{
	if (allowed_tags != NULL) {
		if (NULL == (inst->allowed_tags = pemalloc(allowed_tags_len, persistent))) {
			return FAILURE;
		}
		memcpy((char *)inst->allowed_tags, allowed_tags, allowed_tags_len);
		inst->allowed_tags_len = allowed_tags_len; 
	} else {
		inst->allowed_tags = NULL;
	}
	inst->state = 0;
	inst->persistent = persistent;

	return SUCCESS;
}	

static void php_strip_tags_filter_dtor(php_strip_tags_filter *inst)
{
	if (inst->allowed_tags != NULL) {
		pefree((void *)inst->allowed_tags, inst->persistent);
	}
}

static php_stream_filter_status_t strfilter_strip_tags_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t consumed = 0;
	php_strip_tags_filter *inst = (php_strip_tags_filter *) thisfilter->abstract;

	while (buckets_in->head) {
		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		consumed = bucket->buflen;
		
		bucket->buflen = php_strip_tags(bucket->buf, bucket->buflen, &(inst->state), (char *)inst->allowed_tags, inst->allowed_tags_len);
	
		php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	
	return PSFS_PASS_ON;
}

static void strfilter_strip_tags_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	assert(thisfilter->abstract != NULL);

	php_strip_tags_filter_dtor((php_strip_tags_filter *)thisfilter->abstract);

	pefree(thisfilter->abstract, ((php_strip_tags_filter *)thisfilter->abstract)->persistent);
}

static php_stream_filter_ops strfilter_strip_tags_ops = {
	strfilter_strip_tags_filter,
	strfilter_strip_tags_dtor,
	"string.strip_tags"
};

static php_stream_filter *strfilter_strip_tags_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_strip_tags_filter *inst;
	smart_str tags_ss = { 0, 0, 0 };
	
	inst = pemalloc(sizeof(php_strip_tags_filter), persistent);

	if (inst == NULL) { /* it's possible pemalloc returns NULL
						   instead of causing it to bail out */
		return NULL;
	}
	
	if (filterparams != NULL) {
		if (Z_TYPE_P(filterparams) == IS_ARRAY) {
			HashPosition pos;
			zval **tmp;

			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(filterparams), &pos);
			while (zend_hash_get_current_data_ex(Z_ARRVAL_P(filterparams), (void **) &tmp, &pos) == SUCCESS) {
				convert_to_string_ex(tmp);
				smart_str_appendc(&tags_ss, '<');
				smart_str_appendl(&tags_ss, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
				smart_str_appendc(&tags_ss, '>');
				zend_hash_move_forward_ex(Z_ARRVAL_P(filterparams), &pos);
			}
			smart_str_0(&tags_ss);
		} else {
			/* FIXME: convert_to_* may clutter zvals and lead it into segfault ? */
			convert_to_string_ex(&filterparams);

			tags_ss.c = Z_STRVAL_P(filterparams);
			tags_ss.len = Z_STRLEN_P(filterparams);
			tags_ss.a = 0;
		}
	}

	if (php_strip_tags_filter_ctor(inst, tags_ss.c, tags_ss.len, persistent) != SUCCESS) {
		if (tags_ss.a != 0) {
			STR_FREE(tags_ss.c);
		}
		pefree(inst, persistent);
		return NULL;
	}

	if (tags_ss.a != 0) {
		STR_FREE(tags_ss.c);
	}

	return php_stream_filter_alloc(&strfilter_strip_tags_ops, inst, persistent);
}

static php_stream_filter_factory strfilter_strip_tags_factory = {
	strfilter_strip_tags_create
};

/* }}} */

/* {{{ base64 / quoted_printable stream filter implementation */

typedef enum _php_conv_err_t {
	PHP_CONV_ERR_SUCCESS = SUCCESS,
	PHP_CONV_ERR_UNKNOWN,
	PHP_CONV_ERR_TOO_BIG,
	PHP_CONV_ERR_INVALID_SEQ,
	PHP_CONV_ERR_UNEXPECTED_EOS,
	PHP_CONV_ERR_EXISTS,
	PHP_CONV_ERR_MORE,
	PHP_CONV_ERR_ALLOC,
	PHP_CONV_ERR_NOT_FOUND
} php_conv_err_t;

typedef struct _php_conv php_conv;

typedef php_conv_err_t (*php_conv_convert_func)(php_conv *, const char **, size_t *, char **, size_t *);
typedef void (*php_conv_dtor_func)(php_conv *);

struct _php_conv {
	php_conv_convert_func convert_op;
	php_conv_dtor_func dtor;
};

#define php_conv_convert(a, b, c, d, e) ((php_conv *)(a))->convert_op((php_conv *)(a), (b), (c), (d), (e))
#define php_conv_dtor(a) ((php_conv *)a)->dtor((a))

/* {{{ php_conv_base64_encode */
typedef struct _php_conv_base64_encode {
	php_conv _super;

	unsigned char erem[3];
	size_t erem_len;
	unsigned int line_ccnt;
	unsigned int line_len;
	const char *lbchars;
	int lbchars_dup;
	size_t lbchars_len;
	int persistent;
} php_conv_base64_encode;

static php_conv_err_t php_conv_base64_encode_convert(php_conv_base64_encode *inst, const char **in_p, size_t *in_left, char **out_p, size_t *out_left);
static void php_conv_base64_encode_dtor(php_conv_base64_encode *inst);

static unsigned char b64_tbl_enc[256] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

static php_conv_err_t php_conv_base64_encode_ctor(php_conv_base64_encode *inst, unsigned int line_len, const char *lbchars, size_t lbchars_len, int lbchars_dup, int persistent)
{
	inst->_super.convert_op = (php_conv_convert_func) php_conv_base64_encode_convert;
	inst->_super.dtor = (php_conv_dtor_func) php_conv_base64_encode_dtor;
	inst->erem_len = 0;
	inst->line_ccnt = line_len;
	inst->line_len = line_len;
	if (lbchars != NULL) {
		inst->lbchars = (lbchars_dup ? pestrdup(lbchars, persistent) : lbchars);
		inst->lbchars_len = lbchars_len;
	} else {
		inst->lbchars = NULL;
	}
	inst->lbchars_dup = lbchars_dup;
	inst->persistent = persistent;
	return PHP_CONV_ERR_SUCCESS;
}

static void php_conv_base64_encode_dtor(php_conv_base64_encode *inst)
{
	assert(inst != NULL);
	if (inst->lbchars_dup && inst->lbchars != NULL) {
		pefree((void *)inst->lbchars, inst->persistent);
	}
}

static php_conv_err_t php_conv_base64_encode_flush(php_conv_base64_encode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	volatile php_conv_err_t err = PHP_CONV_ERR_SUCCESS;
	register unsigned char *pd;
	register size_t ocnt;
	unsigned int line_ccnt;

	pd = (unsigned char *)(*out_pp);
	ocnt = *out_left_p;
	line_ccnt = inst->line_ccnt;

	switch (inst->erem_len) {
		case 0:
			/* do nothing */
			break;

		case 1:
			if (line_ccnt < 4 && inst->lbchars != NULL) {
				if (ocnt < inst->lbchars_len) {
					return PHP_CONV_ERR_TOO_BIG;
				}
				memcpy(pd, inst->lbchars, inst->lbchars_len);
				pd += inst->lbchars_len;
				ocnt -= inst->lbchars_len;
				line_ccnt = inst->line_len;
			}
			if (ocnt < 4) {
				err = PHP_CONV_ERR_TOO_BIG;
				goto out;
			}
			*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
			*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4)];
			*(pd++) = '=';
			*(pd++) = '=';
			inst->erem_len = 0;
			ocnt -= 4;
			line_ccnt -= 4;
			break;

		case 2: 
			if (line_ccnt < 4 && inst->lbchars != NULL) {
				if (ocnt < inst->lbchars_len) {
					return PHP_CONV_ERR_TOO_BIG;
				}
				memcpy(pd, inst->lbchars, inst->lbchars_len);
				pd += inst->lbchars_len;
				ocnt -= inst->lbchars_len;
				line_ccnt = inst->line_len;
			}
			if (ocnt < 4) {
				err = PHP_CONV_ERR_TOO_BIG;
				goto out;
			}
			*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
			*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (inst->erem[1] >> 4)];
			*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[1] << 2)];
			*(pd++) = '=';
			inst->erem_len = 0;
			ocnt -=4;
			line_ccnt -= 4;
			break;

		default:
			/* should not happen... */
			err = PHP_CONV_ERR_UNKNOWN;
			break;
	}
out:
	*out_pp = (char *)pd;
	*out_left_p = ocnt;
	inst->line_ccnt = line_ccnt;
	return err;
}

static php_conv_err_t php_conv_base64_encode_convert(php_conv_base64_encode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	volatile php_conv_err_t err = PHP_CONV_ERR_SUCCESS;
	register size_t ocnt, icnt;
	register unsigned char *ps, *pd;
	register unsigned int line_ccnt;

	if (in_pp == NULL || in_left_p == NULL) { 
		return php_conv_base64_encode_flush(inst, in_pp, in_left_p, out_pp, out_left_p);
	}

	pd = (unsigned char *)(*out_pp);
	ocnt = *out_left_p;
	ps = (unsigned char *)(*in_pp);
	icnt = *in_left_p;
	line_ccnt = inst->line_ccnt;

	/* consume the remainder first */
	switch (inst->erem_len) {
		case 1:
			if (icnt >= 2) {
				if (line_ccnt < 4 && inst->lbchars != NULL) {
					if (ocnt < inst->lbchars_len) {
						return PHP_CONV_ERR_TOO_BIG;
					}
					memcpy(pd, inst->lbchars, inst->lbchars_len);
					pd += inst->lbchars_len;
					ocnt -= inst->lbchars_len;
					line_ccnt = inst->line_len;
				}
				if (ocnt < 4) {
					err = PHP_CONV_ERR_TOO_BIG;
					goto out;
				}
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (ps[0] >> 4)];
				*(pd++) = b64_tbl_enc[(unsigned char)(ps[0] << 2) | (ps[1] >> 6)];
				*(pd++) = b64_tbl_enc[ps[1]];
				ocnt -= 4;
				ps += 2;
				icnt -= 2;
				inst->erem_len = 0;
				line_ccnt -= 4;
			}
			break;

		case 2: 
			if (icnt >= 1) {
				if (inst->line_ccnt < 4 && inst->lbchars != NULL) {
					if (ocnt < inst->lbchars_len) {
						return PHP_CONV_ERR_TOO_BIG;
					}
					memcpy(pd, inst->lbchars, inst->lbchars_len);
					pd += inst->lbchars_len;
					ocnt -= inst->lbchars_len;
					line_ccnt = inst->line_len;
				}
				if (ocnt < 4) {
					err = PHP_CONV_ERR_TOO_BIG;
					goto out;
				}
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (inst->erem[1] >> 4)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[1] << 2) | (ps[0] >> 6)];
				*(pd++) = b64_tbl_enc[ps[0]];
				ocnt -= 4;
				ps += 1;
				icnt -= 1;
				inst->erem_len = 0;
				line_ccnt -= 4;
			}
			break;
	}

	while (icnt >= 3) {
		if (line_ccnt < 4 && inst->lbchars != NULL) {
			if (ocnt < inst->lbchars_len) {
				err = PHP_CONV_ERR_TOO_BIG;
				goto out;
			}
			memcpy(pd, inst->lbchars, inst->lbchars_len);
			pd += inst->lbchars_len;
			ocnt -= inst->lbchars_len;
			line_ccnt = inst->line_len;
		}
		if (ocnt < 4) {
			err = PHP_CONV_ERR_TOO_BIG;
			goto out;
		}
		*(pd++) = b64_tbl_enc[ps[0] >> 2];
		*(pd++) = b64_tbl_enc[(unsigned char)(ps[0] << 4) | (ps[1] >> 4)];
		*(pd++) = b64_tbl_enc[(unsigned char)(ps[1] << 2) | (ps[2] >> 6)];
		*(pd++) = b64_tbl_enc[ps[2]];

		ps += 3;
		icnt -=3;
		ocnt -= 4;
		line_ccnt -= 4;
	}
	for (;icnt > 0; icnt--) {
		inst->erem[inst->erem_len++] = *(ps++);
	}

out:
	*in_pp = (const char *)ps;
	*in_left_p = icnt;
	*out_pp = (char *)pd;
	*out_left_p = ocnt;
	inst->line_ccnt = line_ccnt;

	return err;
}

/* }}} */

/* {{{ php_conv_base64_decode */
typedef struct _php_conv_base64_decode {
	php_conv _super;

	unsigned int urem;
	unsigned int urem_nbits;
	unsigned int ustat;
	int eos;
} php_conv_base64_decode;

static php_conv_err_t php_conv_base64_decode_convert(php_conv_base64_decode *inst, const char **in_p, size_t *in_left, char **out_p, size_t *out_left);
static void php_conv_base64_decode_dtor(php_conv_base64_decode *inst);

static unsigned int b64_tbl_dec[256] = {
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64,128, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static int php_conv_base64_decode_ctor(php_conv_base64_decode *inst)
{
	inst->_super.convert_op = (php_conv_convert_func) php_conv_base64_decode_convert;
	inst->_super.dtor = (php_conv_dtor_func) php_conv_base64_decode_dtor;

	inst->urem = 0;
	inst->urem_nbits = 0;
	inst->ustat = 0;
	inst->eos = 0;
	return SUCCESS;
}

static void php_conv_base64_decode_dtor(php_conv_base64_decode *inst)
{
	/* do nothing */
}

#define bmask(a) (0xffff >> (16 - a))
static php_conv_err_t php_conv_base64_decode_convert(php_conv_base64_decode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	php_conv_err_t err;

	unsigned int urem, urem_nbits;
	unsigned int pack, pack_bcnt;
	unsigned char *ps, *pd;
	size_t icnt, ocnt;
	unsigned int ustat;

	static const unsigned int nbitsof_pack = 8;

	if (in_pp == NULL || in_left_p == NULL) {
		if (inst->eos || inst->urem_nbits == 0) { 
			return PHP_CONV_ERR_SUCCESS;
		}
		return PHP_CONV_ERR_UNEXPECTED_EOS;
	}

	err = PHP_CONV_ERR_SUCCESS;

	ps = (unsigned char *)*in_pp;
	pd = (unsigned char *)*out_pp;
	icnt = *in_left_p;
	ocnt = *out_left_p;

	urem = inst->urem;
	urem_nbits = inst->urem_nbits;
	ustat = inst->ustat;

	pack = 0;
	pack_bcnt = nbitsof_pack;

	for (;;) {
		if (pack_bcnt >= urem_nbits) {
			pack_bcnt -= urem_nbits;
			pack |= (urem << pack_bcnt);
			urem_nbits = 0;
		} else {
			urem_nbits -= pack_bcnt;
			pack |= (urem >> urem_nbits);
			urem &= bmask(urem_nbits);
			pack_bcnt = 0;
		}
		if (pack_bcnt > 0) {
			unsigned int i;

			if (icnt < 1) {
				break;
			}

			i = b64_tbl_dec[(unsigned int)*(ps++)];
			icnt--;
			ustat |= i & 0x80;

			if (!(i & 0xc0)) {
				if (ustat) {
					err = PHP_CONV_ERR_INVALID_SEQ;
					break;
				}
				if (6 <= pack_bcnt) {
					pack_bcnt -= 6;
					pack |= (i << pack_bcnt);
					urem = 0;
				} else {
					urem_nbits = 6 - pack_bcnt;
					pack |= (i >> urem_nbits);
					urem = i & bmask(urem_nbits);
					pack_bcnt = 0;
				}
			} else if (ustat) {
				if (pack_bcnt == 8 || pack_bcnt == 2) {
					err = PHP_CONV_ERR_INVALID_SEQ;
					break;
				}
				inst->eos = 1;
			}
		}
		if ((pack_bcnt | ustat) == 0) {
			if (ocnt < 1) {
				err = PHP_CONV_ERR_TOO_BIG;
				break;
			}
			*(pd++) = pack;
			ocnt--;
			pack = 0;
			pack_bcnt = nbitsof_pack;
		}
	}

	if (urem_nbits >= pack_bcnt) {
		urem |= (pack << (urem_nbits - pack_bcnt));
		urem_nbits += (nbitsof_pack - pack_bcnt);
	} else {
		urem |= (pack >> (pack_bcnt - urem_nbits));
		urem_nbits += (nbitsof_pack - pack_bcnt);
	}

	inst->urem = urem;
	inst->urem_nbits = urem_nbits;
	inst->ustat = ustat;

	*in_pp = (const char *)ps;
	*in_left_p = icnt;
	*out_pp = (char *)pd;
	*out_left_p = ocnt;

	return err;
}
#undef bmask
/* }}} */

/* {{{ php_conv_qprint_encode */
typedef struct _php_conv_qprint_encode {
	php_conv _super;

	int opts;
	unsigned int line_ccnt;
	unsigned int line_len;
	const char *lbchars;
	int lbchars_dup;
	size_t lbchars_len;
	int persistent;
	unsigned int lb_ptr;
	unsigned int lb_cnt;
} php_conv_qprint_encode;

#define PHP_CONV_QPRINT_OPT_BINARY             0x00000001
#define PHP_CONV_QPRINT_OPT_FORCE_ENCODE_FIRST 0x00000002

static void php_conv_qprint_encode_dtor(php_conv_qprint_encode *inst);
static php_conv_err_t php_conv_qprint_encode_convert(php_conv_qprint_encode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p);

static void php_conv_qprint_encode_dtor(php_conv_qprint_encode *inst)
{
	assert(inst != NULL);
	if (inst->lbchars_dup && inst->lbchars != NULL) {
		pefree((void *)inst->lbchars, inst->persistent);
	}
}

#define NEXT_CHAR(ps, icnt, lb_ptr, lb_cnt, lbchars) \
	((lb_ptr) < (lb_cnt) ? (lbchars)[(lb_ptr)] : *(ps)) 

#define CONSUME_CHAR(ps, icnt, lb_ptr, lb_cnt) \
	if ((lb_ptr) < (lb_cnt)) { \
		(lb_ptr)++; \
	} else { \
		(lb_cnt) = (lb_ptr) = 0; \
		--(icnt); \
		(ps)++; \
	}

static php_conv_err_t php_conv_qprint_encode_convert(php_conv_qprint_encode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	php_conv_err_t err = PHP_CONV_ERR_SUCCESS;
	unsigned char *ps, *pd;
	size_t icnt, ocnt;
	unsigned int c;
	unsigned int line_ccnt;
	unsigned int lb_ptr;
	unsigned int lb_cnt;
	int opts;
	static char qp_digits[] = "0123456789ABCDEF";

	line_ccnt = inst->line_ccnt;
	opts = inst->opts;
	lb_ptr = inst->lb_ptr;
	lb_cnt = inst->lb_cnt;

	if ((in_pp == NULL || in_left_p == NULL) && (lb_ptr >=lb_cnt)) {
		return PHP_CONV_ERR_SUCCESS;
	}

	ps = (unsigned char *)(*in_pp);
	icnt = *in_left_p;
	pd = (unsigned char *)(*out_pp);
	ocnt = *out_left_p;

	for (;;) {
		if (!(opts & PHP_CONV_QPRINT_OPT_BINARY) && inst->lbchars != NULL && inst->lbchars_len > 0) {
			/* look ahead for the line break chars to make a right decision
			 * how to consume incoming characters */

			if (icnt > 0 && *ps == inst->lbchars[lb_cnt]) {
				lb_cnt++;

				if (lb_cnt >= inst->lbchars_len) {
					unsigned int i;

					if (ocnt < lb_cnt) {
						lb_cnt--;
						err = PHP_CONV_ERR_TOO_BIG;
						break;
					}

					for (i = 0; i < lb_cnt; i++) {
						*(pd++) = inst->lbchars[i];
						ocnt--;
					}
					line_ccnt = inst->line_len;
					lb_ptr = lb_cnt = 0;
				}
				ps++, icnt--;
				continue;
			}
		}

		if (lb_ptr >= lb_cnt && icnt <= 0) {
			break;
		} 

		c = NEXT_CHAR(ps, icnt, lb_ptr, lb_cnt, inst->lbchars);

		if (!(opts & PHP_CONV_QPRINT_OPT_BINARY) && (c == '\t' || c == ' ')) {
			if (line_ccnt < 2 && inst->lbchars != NULL) {
				if (ocnt < inst->lbchars_len + 1) {
					err = PHP_CONV_ERR_TOO_BIG;
					break;
				}

				*(pd++) = '=';
				ocnt--;
				line_ccnt--;

				memcpy(pd, inst->lbchars, inst->lbchars_len);
				pd += inst->lbchars_len;
				ocnt -= inst->lbchars_len;
				line_ccnt = inst->line_len;
			} else {
				if (ocnt < 1) {
					err = PHP_CONV_ERR_TOO_BIG;
					break;
				}
				*(pd++) = c;
				ocnt--;
				line_ccnt--;
				CONSUME_CHAR(ps, icnt, lb_ptr, lb_cnt);
			}
		} else if ((!(opts & PHP_CONV_QPRINT_OPT_FORCE_ENCODE_FIRST) || line_ccnt < inst->line_len) && ((c >= 33 && c <= 60) || (c >= 62 && c <= 126))) { 
			if (line_ccnt < 2 && inst->lbchars != NULL) {
				if (ocnt < inst->lbchars_len + 1) {
					err = PHP_CONV_ERR_TOO_BIG;
					break;
				}
				*(pd++) = '=';
				ocnt--;
				line_ccnt--;

				memcpy(pd, inst->lbchars, inst->lbchars_len);
				pd += inst->lbchars_len;
				ocnt -= inst->lbchars_len;
				line_ccnt = inst->line_len;
			}
			if (ocnt < 1) {
				err = PHP_CONV_ERR_TOO_BIG;
				break;
			}
			*(pd++) = c;
			ocnt--;
			line_ccnt--;
			CONSUME_CHAR(ps, icnt, lb_ptr, lb_cnt);
		} else {
			if (line_ccnt < 4) {
				if (ocnt < inst->lbchars_len + 1) {
					err = PHP_CONV_ERR_TOO_BIG;
					break;
				}
				*(pd++) = '=';
				ocnt--;
				line_ccnt--;

				memcpy(pd, inst->lbchars, inst->lbchars_len);
				pd += inst->lbchars_len;
				ocnt -= inst->lbchars_len;
				line_ccnt = inst->line_len;
			}
			if (ocnt < 3) {
				err = PHP_CONV_ERR_TOO_BIG;
				break;
			}
			*(pd++) = '=';
			*(pd++) = qp_digits[(c >> 4)];
			*(pd++) = qp_digits[(c & 0x0f)]; 
			ocnt -= 3;
			line_ccnt -= 3;
			CONSUME_CHAR(ps, icnt, lb_ptr, lb_cnt);
		}
	}

	*in_pp = (const char *)ps;
	*in_left_p = icnt;
	*out_pp = (char *)pd;
	*out_left_p = ocnt; 
	inst->line_ccnt = line_ccnt;
	inst->lb_ptr = lb_ptr;
	inst->lb_cnt = lb_cnt;
	return err;
}
#undef NEXT_CHAR
#undef CONSUME_CHAR

static php_conv_err_t php_conv_qprint_encode_ctor(php_conv_qprint_encode *inst, unsigned int line_len, const char *lbchars, size_t lbchars_len, int lbchars_dup, int opts, int persistent)
{
	if (line_len < 4 && lbchars != NULL) {
		return PHP_CONV_ERR_TOO_BIG;
	}
	inst->_super.convert_op = (php_conv_convert_func) php_conv_qprint_encode_convert;
	inst->_super.dtor = (php_conv_dtor_func) php_conv_qprint_encode_dtor;
	inst->line_ccnt = line_len;
	inst->line_len = line_len;
	if (lbchars != NULL) {
		inst->lbchars = (lbchars_dup ? pestrdup(lbchars, persistent) : lbchars);
		inst->lbchars_len = lbchars_len;
	} else {
		inst->lbchars = NULL;
	}
	inst->lbchars_dup = lbchars_dup;
	inst->persistent = persistent;
	inst->opts = opts;
	inst->lb_cnt = inst->lb_ptr = 0;
	return PHP_CONV_ERR_SUCCESS;
}
/* }}} */

/* {{{ php_conv_qprint_decode */
typedef struct _php_conv_qprint_decode {
	php_conv _super;

	int scan_stat;
	unsigned int next_char;
	const char *lbchars;
	int lbchars_dup;
	size_t lbchars_len;
	int persistent;
	unsigned int lb_ptr;
	unsigned int lb_cnt;	
} php_conv_qprint_decode;

static void php_conv_qprint_decode_dtor(php_conv_qprint_decode *inst)
{
	assert(inst != NULL);
	if (inst->lbchars_dup && inst->lbchars != NULL) {
		pefree((void *)inst->lbchars, inst->persistent);
	}
}

static php_conv_err_t php_conv_qprint_decode_convert(php_conv_qprint_decode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	php_conv_err_t err = PHP_CONV_ERR_SUCCESS;
	size_t icnt, ocnt;
	unsigned char *ps, *pd;
	unsigned int scan_stat;
	unsigned int next_char;
	unsigned int lb_ptr, lb_cnt;

	lb_ptr = inst->lb_ptr;
	lb_cnt = inst->lb_cnt;

	if ((in_pp == NULL || in_left_p == NULL) && lb_cnt == lb_ptr) {
		if (inst->scan_stat != 0) {
			return PHP_CONV_ERR_UNEXPECTED_EOS;
		}
		return PHP_CONV_ERR_SUCCESS;
	}

	ps = (unsigned char *)(*in_pp);
	icnt = *in_left_p;
	pd = (unsigned char *)(*out_pp);
	ocnt = *out_left_p;
	scan_stat = inst->scan_stat;
	next_char = inst->next_char;

	for (;;) {
		switch (scan_stat) {
			case 0: {
				if (icnt <= 0) {
					goto out;
				}
				if (*ps == '=') {
					scan_stat = 1;
				} else {
					if (ocnt < 1) {
						err = PHP_CONV_ERR_TOO_BIG;
						goto out;
					}
					*(pd++) = *ps;
					ocnt--;
				}
				ps++, icnt--;
			} break;

			case 1: {
				if (icnt <= 0) {
					goto out;
				}
				if (*ps == ' ' || *ps == '\t') {
					scan_stat = 4;
					ps++, icnt--;
					break;
				} else if (!inst->lbchars && lb_cnt == 0 && *ps == '\r') {
					/* auto-detect line endings, looks like network line ending \r\n (could be mac \r) */
					lb_cnt++;
					scan_stat = 5;
					ps++, icnt--;
					break;
				} else if (!inst->lbchars && lb_cnt == 0 && *ps == '\n') {
					/* auto-detect line endings, looks like unix-lineendings, not to spec, but it is seem in the wild, a lot */
					lb_cnt = lb_ptr = 0;
					scan_stat = 0;
					ps++, icnt--;
					break;
				} else if (lb_cnt < inst->lbchars_len &&
							*ps == (unsigned char)inst->lbchars[lb_cnt]) {
					lb_cnt++;
					scan_stat = 5;
					ps++, icnt--;
					break;
				}
			} /* break is missing intentionally */

			case 2: {	
				if (icnt <= 0) {
					goto out;
				}

				if (!isxdigit((int) *ps)) {
					err = PHP_CONV_ERR_INVALID_SEQ;
					goto out;
				}
				next_char = (next_char << 4) | (*ps >= 'A' ? *ps - 0x37 : *ps - 0x30);
				scan_stat++;
				ps++, icnt--;
				if (scan_stat != 3) {
					break;
				}
			} /* break is missing intentionally */

			case 3: {
				if (ocnt < 1) {
					err = PHP_CONV_ERR_TOO_BIG;
					goto out;
				}
				*(pd++) = next_char;
				ocnt--;
				scan_stat = 0;
			} break;

			case 4: {
				if (icnt <= 0) {
					goto out;
				}
				if (lb_cnt < inst->lbchars_len &&
					*ps == (unsigned char)inst->lbchars[lb_cnt]) {
					lb_cnt++;
					scan_stat = 5;
				}
				if (*ps != '\t' && *ps != ' ') {
					err = PHP_CONV_ERR_INVALID_SEQ;
					goto out;
				}
				ps++, icnt--;
			} break;

			case 5: {
				if (!inst->lbchars && lb_cnt == 1 && *ps == '\n') {
					/* auto-detect soft line breaks, found network line break */
					lb_cnt = lb_ptr = 0;
					scan_stat = 0;
					ps++, icnt--; /* consume \n */
				} else if (!inst->lbchars && lb_cnt > 0) {
					/* auto-detect soft line breaks, found mac line break */
					lb_cnt = lb_ptr = 0;
					scan_stat = 0;
				} else if (lb_cnt >= inst->lbchars_len) {
					/* soft line break */
					lb_cnt = lb_ptr = 0;
					scan_stat = 0;
				} else if (icnt > 0) {
					if (*ps == (unsigned char)inst->lbchars[lb_cnt]) {
						lb_cnt++;
						ps++, icnt--;
					} else {
						scan_stat = 6; /* no break for short-cut */
					}
				} else {
					goto out;
				}
			} break;

			case 6: {
				if (lb_ptr < lb_cnt) {
					if (ocnt < 1) {
						err = PHP_CONV_ERR_TOO_BIG;
						goto out;
					}
					*(pd++) = inst->lbchars[lb_ptr++];
					ocnt--;
				} else {
					scan_stat = 0;
					lb_cnt = lb_ptr = 0;
				}
			} break;
		}
	}
out:
	*in_pp = (const char *)ps;
	*in_left_p = icnt;
	*out_pp = (char *)pd;
	*out_left_p = ocnt;
	inst->scan_stat = scan_stat;
	inst->lb_ptr = lb_ptr;
	inst->lb_cnt = lb_cnt;
	inst->next_char = next_char;

	return err;
}
static php_conv_err_t php_conv_qprint_decode_ctor(php_conv_qprint_decode *inst, const char *lbchars, size_t lbchars_len, int lbchars_dup, int persistent)
{
	inst->_super.convert_op = (php_conv_convert_func) php_conv_qprint_decode_convert;
	inst->_super.dtor = (php_conv_dtor_func) php_conv_qprint_decode_dtor;
	inst->scan_stat = 0;
	inst->next_char = 0;
	inst->lb_ptr = inst->lb_cnt = 0;
	if (lbchars != NULL) {
		inst->lbchars = (lbchars_dup ? pestrdup(lbchars, persistent) : lbchars);
		inst->lbchars_len = lbchars_len;
	} else {
		inst->lbchars = NULL;
		inst->lbchars_len = 0;
	}
	inst->lbchars_dup = lbchars_dup;
	inst->persistent = persistent;
	return PHP_CONV_ERR_SUCCESS;
}
/* }}} */

typedef struct _php_convert_filter {
	php_conv *cd;
	int persistent;
	char *filtername;
	char stub[128];
	size_t stub_len;
} php_convert_filter;

#define PHP_CONV_BASE64_ENCODE 1
#define PHP_CONV_BASE64_DECODE 2
#define PHP_CONV_QPRINT_ENCODE 3 
#define PHP_CONV_QPRINT_DECODE 4

static php_conv_err_t php_conv_get_string_prop_ex(const HashTable *ht, char **pretval, size_t *pretval_len, char *field_name, size_t field_name_len, int persistent)
{
	zval **tmpval;

	*pretval = NULL;
	*pretval_len = 0;
 
	if (zend_hash_find((HashTable *)ht, field_name, field_name_len, (void **)&tmpval) == SUCCESS) {
		if (Z_TYPE_PP(tmpval) != IS_STRING) {
			zval zt = **tmpval;

			convert_to_string(&zt);

			if (NULL == (*pretval = pemalloc(Z_STRLEN(zt) + 1, persistent))) {
				return PHP_CONV_ERR_ALLOC;
			}

			*pretval_len = Z_STRLEN(zt);
			memcpy(*pretval, Z_STRVAL(zt), Z_STRLEN(zt) + 1);
			zval_dtor(&zt);
		} else {
			if (NULL == (*pretval = pemalloc(Z_STRLEN_PP(tmpval) + 1, persistent))) {
				return PHP_CONV_ERR_ALLOC;
			}
			*pretval_len = Z_STRLEN_PP(tmpval);
			memcpy(*pretval, Z_STRVAL_PP(tmpval), Z_STRLEN_PP(tmpval) + 1);
		}
	} else {
		return PHP_CONV_ERR_NOT_FOUND;
	}
	return PHP_CONV_ERR_SUCCESS;
}

#if IT_WAS_USED
static php_conv_err_t php_conv_get_long_prop_ex(const HashTable *ht, long *pretval, char *field_name, size_t field_name_len)
{
	zval **tmpval;

	*pretval = 0;

	if (zend_hash_find((HashTable *)ht, field_name, field_name_len, (void **)&tmpval) == SUCCESS) {
		zval tmp, *ztval = *tmpval;

		if (Z_TYPE_PP(tmpval) != IS_LONG) {
			tmp = *ztval;
			zval_copy_ctor(&tmp);
			convert_to_long(&tmp);
			ztval = &tmp;
		}
		*pretval = Z_LVAL_P(ztval);
	} else {
		return PHP_CONV_ERR_NOT_FOUND;
	} 
	return PHP_CONV_ERR_SUCCESS;
}
#endif

static php_conv_err_t php_conv_get_ulong_prop_ex(const HashTable *ht, unsigned long *pretval, char *field_name, size_t field_name_len)
{
	zval **tmpval;

	*pretval = 0;

	if (zend_hash_find((HashTable *)ht, field_name, field_name_len, (void **)&tmpval) == SUCCESS) {
		zval tmp, *ztval = *tmpval;

		if (Z_TYPE_PP(tmpval) != IS_LONG) {
			tmp = *ztval;
			zval_copy_ctor(&tmp);
			convert_to_long(&tmp);
			ztval = &tmp;
		}
		if (Z_LVAL_P(ztval) < 0) {
			*pretval = 0;
		} else {
			*pretval = Z_LVAL_P(ztval);
		}
	} else {
		return PHP_CONV_ERR_NOT_FOUND;
	} 
	return PHP_CONV_ERR_SUCCESS;
}

static php_conv_err_t php_conv_get_bool_prop_ex(const HashTable *ht, int *pretval, char *field_name, size_t field_name_len)
{
	zval **tmpval;

	*pretval = 0;

	if (zend_hash_find((HashTable *)ht, field_name, field_name_len, (void **)&tmpval) == SUCCESS) {
		zval tmp, *ztval = *tmpval;

		if (Z_TYPE_PP(tmpval) != IS_BOOL) {
			tmp = *ztval;
			zval_copy_ctor(&tmp);
			convert_to_boolean(&tmp);
			ztval = &tmp;
		}
		*pretval = Z_BVAL_P(ztval);
	} else {
		return PHP_CONV_ERR_NOT_FOUND;
	} 
	return PHP_CONV_ERR_SUCCESS;
}


#if IT_WAS_USED
static int php_conv_get_int_prop_ex(const HashTable *ht, int *pretval, char *field_name, size_t field_name_len)
{
	long l;
	php_conv_err_t err;

	*pretval = 0;

	if ((err = php_conv_get_long_prop_ex(ht, &l, field_name, field_name_len)) == PHP_CONV_ERR_SUCCESS) {
		*pretval = l;
	}
	return err;
}
#endif

static int php_conv_get_uint_prop_ex(const HashTable *ht, unsigned int *pretval, char *field_name, size_t field_name_len)
{
	long l;
	php_conv_err_t err;

	*pretval = 0;

	if ((err = php_conv_get_ulong_prop_ex(ht, &l, field_name, field_name_len)) == PHP_CONV_ERR_SUCCESS) {
		*pretval = l;
	}
	return err;
}

#define GET_STR_PROP(ht, var, var_len, fldname, persistent) \
	php_conv_get_string_prop_ex(ht, &var, &var_len, fldname, sizeof(fldname), persistent) 

#define GET_INT_PROP(ht, var, fldname) \
	php_conv_get_int_prop_ex(ht, &var, fldname, sizeof(fldname))

#define GET_UINT_PROP(ht, var, fldname) \
	php_conv_get_uint_prop_ex(ht, &var, fldname, sizeof(fldname))

#define GET_BOOL_PROP(ht, var, fldname) \
	php_conv_get_bool_prop_ex(ht, &var, fldname, sizeof(fldname))

static php_conv *php_conv_open(int conv_mode, const HashTable *options, int persistent)
{
	/* FIXME: I'll have to replace this ugly code by something neat
	   (factories?) in the near future. */ 
	php_conv *retval = NULL;

	switch (conv_mode) {
		case PHP_CONV_BASE64_ENCODE: {
			unsigned int line_len = 0;
			char *lbchars = NULL;
			size_t lbchars_len;

			if (options != NULL) {
				GET_STR_PROP(options, lbchars, lbchars_len, "line-break-chars", 0);
				GET_UINT_PROP(options, line_len, "line-length");
				if (line_len < 4) {
					if (lbchars != NULL) {
						pefree(lbchars, 0);
					}
					lbchars = NULL;
				} else {
					if (lbchars == NULL) {
						lbchars = pestrdup("\r\n", 0);
						lbchars_len = 2;
					}
				}
			}
			retval = pemalloc(sizeof(php_conv_base64_encode), persistent);
			if (lbchars != NULL) {
				if (php_conv_base64_encode_ctor((php_conv_base64_encode *)retval, line_len, lbchars, lbchars_len, 1, persistent)) {
					if (lbchars != NULL) {
						pefree(lbchars, 0);
					}
					goto out_failure;
				}
				pefree(lbchars, 0);
			} else {
				if (php_conv_base64_encode_ctor((php_conv_base64_encode *)retval, 0, NULL, 0, 0, persistent)) {
					goto out_failure;
				}
			}
		} break;

		case PHP_CONV_BASE64_DECODE:
			retval = pemalloc(sizeof(php_conv_base64_decode), persistent);
			if (php_conv_base64_decode_ctor((php_conv_base64_decode *)retval)) {
				goto out_failure;
			}
			break;

		case PHP_CONV_QPRINT_ENCODE: {
			unsigned int line_len = 0;
			char *lbchars = NULL;
			size_t lbchars_len;
			int opts = 0;

			if (options != NULL) {
				int opt_binary = 0;
				int opt_force_encode_first = 0;

				GET_STR_PROP(options, lbchars, lbchars_len, "line-break-chars", 0);
				GET_UINT_PROP(options, line_len, "line-length");
				GET_BOOL_PROP(options, opt_binary, "binary"); 
				GET_BOOL_PROP(options, opt_force_encode_first, "force-encode-first"); 

				if (line_len < 4) {
					if (lbchars != NULL) {
						pefree(lbchars, 0);
					}
					lbchars = NULL;
				} else {
					if (lbchars == NULL) {
						lbchars = pestrdup("\r\n", 0);
						lbchars_len = 2;
					}
				}
				opts |= (opt_binary ? PHP_CONV_QPRINT_OPT_BINARY : 0);
				opts |= (opt_force_encode_first ? PHP_CONV_QPRINT_OPT_FORCE_ENCODE_FIRST : 0);
			}
			retval = pemalloc(sizeof(php_conv_qprint_encode), persistent);
			if (lbchars != NULL) {
				if (php_conv_qprint_encode_ctor((php_conv_qprint_encode *)retval, line_len, lbchars, lbchars_len, 1, opts, persistent)) {
					pefree(lbchars, 0);
					goto out_failure;
				}
				pefree(lbchars, 0);
			} else {
				if (php_conv_qprint_encode_ctor((php_conv_qprint_encode *)retval, 0, NULL, 0, 0, opts, persistent)) {
					goto out_failure;
				}
			}
		} break;
	
		case PHP_CONV_QPRINT_DECODE: {
			char *lbchars = NULL;
			size_t lbchars_len;

			if (options != NULL) {
				/* If line-break-chars are not specified, filter will attempt to detect line endings (\r, \n, or \r\n) */
				GET_STR_PROP(options, lbchars, lbchars_len, "line-break-chars", 0);
			}

			retval = pemalloc(sizeof(php_conv_qprint_decode), persistent);
			if (lbchars != NULL) {
				if (php_conv_qprint_decode_ctor((php_conv_qprint_decode *)retval, lbchars, lbchars_len, 1, persistent)) {
					pefree(lbchars, 0);
					goto out_failure;
				}
				pefree(lbchars, 0);
			} else {
				if (php_conv_qprint_decode_ctor((php_conv_qprint_decode *)retval, NULL, 0, 0, persistent)) {
					goto out_failure;
				}
			}
		} break;

		default:
			retval = NULL;
			break;
	}
	return retval;

out_failure:
	if (retval != NULL) {
		pefree(retval, persistent);
	}
	return NULL;	
}

#undef GET_STR_PROP
#undef GET_INT_PROP
#undef GET_UINT_PROP
#undef GET_BOOL_PROP

static int php_convert_filter_ctor(php_convert_filter *inst,
	int conv_mode, HashTable *conv_opts,
	const char *filtername, int persistent)
{
	inst->persistent = persistent;
	inst->filtername = pestrdup(filtername, persistent);
	inst->stub_len = 0;

	if ((inst->cd = php_conv_open(conv_mode, conv_opts, persistent)) == NULL) {
		goto out_failure;
	}

	return SUCCESS;

out_failure:
	if (inst->cd != NULL) {
		php_conv_dtor(inst->cd);
		pefree(inst->cd, persistent);
	}
	if (inst->filtername != NULL) {
		pefree(inst->filtername, persistent);
	}
	return FAILURE;
}

static void php_convert_filter_dtor(php_convert_filter *inst)
{
	if (inst->cd != NULL) {
		php_conv_dtor(inst->cd);
		pefree(inst->cd, inst->persistent);
	}

	if (inst->filtername != NULL) {
		pefree(inst->filtername, inst->persistent);
	}
}

/* {{{ strfilter_convert_append_bucket */
static int strfilter_convert_append_bucket(
		php_convert_filter *inst,
		php_stream *stream, php_stream_filter *filter,
		php_stream_bucket_brigade *buckets_out,
		const char *ps, size_t buf_len, size_t *consumed,
		int persistent TSRMLS_DC)
{
	php_conv_err_t err;
	php_stream_bucket *new_bucket;
	char *out_buf = NULL;
	size_t out_buf_size;
	char *pd;
	const char *pt;
	size_t ocnt, icnt, tcnt;
	size_t initial_out_buf_size;
	
	if (ps == NULL) {
		initial_out_buf_size = 64;
		icnt = 1;
	} else {
		initial_out_buf_size = buf_len;
		icnt = buf_len;
	}

	out_buf_size = ocnt = initial_out_buf_size; 
	if (NULL == (out_buf = pemalloc(out_buf_size, persistent))) {
		return FAILURE;
	}

	pd = out_buf;

	if (inst->stub_len > 0) {
		pt = inst->stub;
		tcnt = inst->stub_len;

		while (tcnt > 0) {
			err = php_conv_convert(inst->cd, &pt, &tcnt, &pd, &ocnt);

			switch (err) {
				case PHP_CONV_ERR_INVALID_SEQ:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): invalid byte sequence", inst->filtername);
					goto out_failure;

				case PHP_CONV_ERR_MORE:
					if (ps != NULL) {
						if (icnt > 0) {
							if (inst->stub_len >= sizeof(inst->stub)) {
								php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): insufficient buffer", inst->filtername);
								goto out_failure;
							}
							inst->stub[inst->stub_len++] = *(ps++);
							icnt--;
							pt = inst->stub;
							tcnt = inst->stub_len;
						} else {
							tcnt = 0;
							break;
						}
					}
					break;

				case PHP_CONV_ERR_UNEXPECTED_EOS:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): unexpected end of stream", inst->filtername);
					goto out_failure;

				case PHP_CONV_ERR_TOO_BIG: {
					char *new_out_buf;
					size_t new_out_buf_size;

					new_out_buf_size = out_buf_size << 1;

					if (new_out_buf_size < out_buf_size) {
						/* whoa! no bigger buckets are sold anywhere... */
						if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent TSRMLS_CC))) {
							goto out_failure;
						}

						php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);

						out_buf_size = ocnt = initial_out_buf_size;
						if (NULL == (out_buf = pemalloc(out_buf_size, persistent))) {
							return FAILURE;
						}
						pd = out_buf;
					} else {
						if (NULL == (new_out_buf = perealloc(out_buf, new_out_buf_size, persistent))) {
							if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent TSRMLS_CC))) {
								goto out_failure;
							}

							php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);
							return FAILURE;
						}

						pd = new_out_buf + (pd - out_buf);
						ocnt += (new_out_buf_size - out_buf_size);
						out_buf = new_out_buf;
						out_buf_size = new_out_buf_size;
					}
				} break;

				case PHP_CONV_ERR_UNKNOWN:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): unknown error", inst->filtername);
					goto out_failure;

				default:
					break;
			}
		}
		memmove(inst->stub, pt, tcnt);
		inst->stub_len = tcnt;
	}

	while (icnt > 0) {
		err = ((ps == NULL ? php_conv_convert(inst->cd, NULL, NULL, &pd, &ocnt):
				php_conv_convert(inst->cd, &ps, &icnt, &pd, &ocnt)));
		switch (err) {
			case PHP_CONV_ERR_INVALID_SEQ:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): invalid byte sequence", inst->filtername);
				goto out_failure;

			case PHP_CONV_ERR_MORE:
				if (ps != NULL) {
					if (icnt > sizeof(inst->stub)) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): insufficient buffer", inst->filtername);
						goto out_failure;
					}
					memcpy(inst->stub, ps, icnt);
					inst->stub_len = icnt;
					ps += icnt;
					icnt = 0;
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): unexpected octet values", inst->filtername);
					goto out_failure;
				}
				break;

			case PHP_CONV_ERR_TOO_BIG: {
				char *new_out_buf;
				size_t new_out_buf_size;

				new_out_buf_size = out_buf_size << 1;

				if (new_out_buf_size < out_buf_size) {
					/* whoa! no bigger buckets are sold anywhere... */
					if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent TSRMLS_CC))) {
						goto out_failure;
					}

					php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);

					out_buf_size = ocnt = initial_out_buf_size;
					if (NULL == (out_buf = pemalloc(out_buf_size, persistent))) {
						return FAILURE;
					}
					pd = out_buf;
				} else {
					if (NULL == (new_out_buf = perealloc(out_buf, new_out_buf_size, persistent))) {
						if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent TSRMLS_CC))) {
							goto out_failure;
						}

						php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);
						return FAILURE;
					}
					pd = new_out_buf + (pd - out_buf);
					ocnt += (new_out_buf_size - out_buf_size);
					out_buf = new_out_buf;
					out_buf_size = new_out_buf_size;
				}
			} break;

			case PHP_CONV_ERR_UNKNOWN:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): unknown error", inst->filtername);
				goto out_failure;

			default:
				if (ps == NULL) {
					icnt = 0;
				}
				break;
		}
	}

	if (out_buf_size - ocnt > 0) {
		if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent TSRMLS_CC))) {
			goto out_failure;
		}
		php_stream_bucket_append(buckets_out, new_bucket TSRMLS_CC);
	} else {
		pefree(out_buf, persistent);
	}
	*consumed += buf_len - icnt;

	return SUCCESS;

out_failure:
	pefree(out_buf, persistent);
	return FAILURE;
}
/* }}} */

static php_stream_filter_status_t strfilter_convert_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_stream_bucket *bucket = NULL;
	size_t consumed = 0;
	php_convert_filter *inst = (php_convert_filter *)thisfilter->abstract;

	while (buckets_in->head != NULL) {
		bucket = buckets_in->head;

		php_stream_bucket_unlink(bucket TSRMLS_CC);

		if (strfilter_convert_append_bucket(inst, stream, thisfilter,
				buckets_out, bucket->buf, bucket->buflen, &consumed,
				php_stream_is_persistent(stream) TSRMLS_CC) != SUCCESS) {
			goto out_failure;
		}

		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags != PSFS_FLAG_NORMAL) {
		if (strfilter_convert_append_bucket(inst, stream, thisfilter,
				buckets_out, NULL, 0, &consumed,
				php_stream_is_persistent(stream) TSRMLS_CC) != SUCCESS) {
			goto out_failure;
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return PSFS_PASS_ON;

out_failure:
	if (bucket != NULL) {
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}
	return PSFS_ERR_FATAL;
}

static void strfilter_convert_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	assert(thisfilter->abstract != NULL);

	php_convert_filter_dtor((php_convert_filter *)thisfilter->abstract);
	pefree(thisfilter->abstract, ((php_convert_filter *)thisfilter->abstract)->persistent);
}

static php_stream_filter_ops strfilter_convert_ops = {
	strfilter_convert_filter,
	strfilter_convert_dtor,
	"convert.*"
};

static php_stream_filter *strfilter_convert_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_convert_filter *inst;
	php_stream_filter *retval = NULL;

	char *dot;
	int conv_mode = 0;

	if (filterparams != NULL && Z_TYPE_P(filterparams) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "stream filter (%s): invalid filter parameter", filtername);
		return NULL;
	}

	if ((dot = strchr(filtername, '.')) == NULL) {
		return NULL;
	}
	++dot;

	inst = pemalloc(sizeof(php_convert_filter), persistent);

	if (strcasecmp(dot, "base64-encode") == 0) {
		conv_mode = PHP_CONV_BASE64_ENCODE;
	} else if (strcasecmp(dot, "base64-decode") == 0) {
		conv_mode = PHP_CONV_BASE64_DECODE;
	} else if (strcasecmp(dot, "quoted-printable-encode") == 0) {
		conv_mode = PHP_CONV_QPRINT_ENCODE;
	} else if (strcasecmp(dot, "quoted-printable-decode") == 0) {
		conv_mode = PHP_CONV_QPRINT_DECODE;
	}
	
	if (php_convert_filter_ctor(inst, conv_mode,
		(filterparams != NULL ? Z_ARRVAL_P(filterparams) : NULL),
		filtername, persistent) != SUCCESS) {
		goto out;
	}	

	retval = php_stream_filter_alloc(&strfilter_convert_ops, inst, persistent);
out:
	if (retval == NULL) {
		pefree(inst, persistent);
	}

	return retval;
}

static php_stream_filter_factory strfilter_convert_factory = {
	strfilter_convert_create
};
/* }}} */

/* {{{ consumed filter implementation */
typedef struct _php_consumed_filter_data {
	int persistent;
	size_t consumed;
	off_t offset;
} php_consumed_filter_data;

static php_stream_filter_status_t consumed_filter_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_consumed_filter_data *data = (php_consumed_filter_data *)(thisfilter->abstract);
	php_stream_bucket *bucket;
	size_t consumed = 0;

	if (data->offset == ~0) {
		data->offset = php_stream_tell(stream);
	}
	while ((bucket = buckets_in->head) != NULL) {
		php_stream_bucket_unlink(bucket TSRMLS_CC);
		consumed += bucket->buflen;
		php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
	}
	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		php_stream_seek(stream, data->offset + data->consumed, SEEK_SET);
	}
	data->consumed += consumed;
	
	return PSFS_PASS_ON;
}

static void consumed_filter_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_consumed_filter_data *data = (php_consumed_filter_data*)thisfilter->abstract;
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops consumed_filter_ops = {
	consumed_filter_filter,
	consumed_filter_dtor,
	"consumed"
};

static php_stream_filter *consumed_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_stream_filter_ops *fops = NULL;
	php_consumed_filter_data *data;

	if (strcasecmp(filtername, "consumed")) {
		return NULL;
	}

	/* Create this filter */
	data = pecalloc(1, sizeof(php_consumed_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zd bytes", sizeof(php_consumed_filter_data));
		return NULL;
	}
	data->persistent = persistent;
	data->consumed = 0;
	data->offset = ~0;
	fops = &consumed_filter_ops;

	return php_stream_filter_alloc(fops, data, persistent);
}

php_stream_filter_factory consumed_filter_factory = {
	consumed_filter_create
};

/* }}} */

/* {{{ chunked filter implementation */
typedef enum _php_chunked_filter_state {
	CHUNK_SIZE_START,
	CHUNK_SIZE,
	CHUNK_SIZE_EXT,
	CHUNK_SIZE_CR,
	CHUNK_SIZE_LF,
	CHUNK_BODY,
	CHUNK_BODY_CR,
	CHUNK_BODY_LF,
	CHUNK_TRAILER,
	CHUNK_ERROR
} php_chunked_filter_state;

typedef struct _php_chunked_filter_data {
	php_chunked_filter_state state;
	size_t chunk_size;
	int persistent;
} php_chunked_filter_data;

static int php_dechunk(char *buf, int len, php_chunked_filter_data *data)
{
	char *p = buf;
	char *end = p + len;
	char *out = buf;
	int out_len = 0;

	while (p < end) {
		switch (data->state) {
			case CHUNK_SIZE_START:
				data->chunk_size = 0;
			case CHUNK_SIZE:
				while (p < end) {
					if (*p >= '0' && *p <= '9') {
						data->chunk_size = (data->chunk_size * 16) + (*p - '0');
					} else if (*p >= 'A' && *p <= 'F') {
						data->chunk_size = (data->chunk_size * 16) + (*p - 'A' + 10);
					} else if (*p >= 'a' && *p <= 'f') {
						data->chunk_size = (data->chunk_size * 16) + (*p - 'a' + 10);
					} else if (data->state == CHUNK_SIZE_START) {
						data->state = CHUNK_ERROR;
						break;
					} else {
						data->state = CHUNK_SIZE_EXT;
						break;
					}
					data->state = CHUNK_SIZE;
					p++;
				}
				if (data->state == CHUNK_ERROR) {
					continue;
				} else if (p == end) {
					return out_len;
				}
			case CHUNK_SIZE_EXT:
				/* skip extension */
				while (p < end && *p != '\r' && *p != '\n') {
					p++;
				}
				if (p == end) {
					return out_len;
				}
			case CHUNK_SIZE_CR:
				if (*p == '\r') {
					p++;
					if (p == end) {
						data->state = CHUNK_SIZE_LF;
						return out_len;
					}
				}
			case CHUNK_SIZE_LF:
				if (*p == '\n') {
					p++;
					if (data->chunk_size == 0) {
						/* last chunk */
						data->state = CHUNK_TRAILER;
						continue;
					} else if (p == end) {
						data->state = CHUNK_BODY;
						return out_len;
					}
				} else {
					data->state = CHUNK_ERROR;
					continue;
				}
			case CHUNK_BODY:
				if ((size_t) (end - p) >= data->chunk_size) {
					if (p != out) {
						memmove(out, p, data->chunk_size);
					}
					out += data->chunk_size;
					out_len += data->chunk_size;
					p += data->chunk_size;
					if (p == end) {
						data->state = CHUNK_BODY_CR;
						return out_len;
					}
				} else {
					if (p != out) {
						memmove(out, p, end - p);
					}
					data->chunk_size -= end - p;
					data->state=CHUNK_BODY;
					out_len += end - p;
					return out_len;
				}
			case CHUNK_BODY_CR:
				if (*p == '\r') {
					p++;
					if (p == end) {
						data->state = CHUNK_BODY_LF;
						return out_len;
					}
				}
			case CHUNK_BODY_LF:
				if (*p == '\n') {
					p++;
					data->state = CHUNK_SIZE_START;
					continue;
				} else {
					data->state = CHUNK_ERROR;
					continue;
				}
			case CHUNK_TRAILER:
				/* ignore trailer */
				p = end;
				continue;
			case CHUNK_ERROR:
				if (p != out) {
					memmove(out, p, end - p);
				}
				out_len += end - p;
				return out_len;	
		}
	}
	return out_len;
}

static php_stream_filter_status_t php_chunked_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_stream_bucket *bucket;
	size_t consumed = 0;
	php_chunked_filter_data *data = (php_chunked_filter_data *) thisfilter->abstract;

	while (buckets_in->head) {
		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		consumed += bucket->buflen;
		bucket->buflen = php_dechunk(bucket->buf, bucket->buflen, data);	
		php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	
	return PSFS_PASS_ON;
}

static void php_chunked_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_chunked_filter_data *data = (php_chunked_filter_data *) thisfilter->abstract;
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops chunked_filter_ops = {
	php_chunked_filter,
	php_chunked_dtor,
	"dechunk"
};

static php_stream_filter *chunked_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_stream_filter_ops *fops = NULL;
	php_chunked_filter_data *data;

	if (strcasecmp(filtername, "dechunk")) {
		return NULL;
	}

	/* Create this filter */
	data = (php_chunked_filter_data *)pecalloc(1, sizeof(php_chunked_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zd bytes", sizeof(php_chunked_filter_data));
		return NULL;
	}
	data->state = CHUNK_SIZE_START;
	data->chunk_size = 0;
	data->persistent = persistent;
	fops = &chunked_filter_ops;

	return php_stream_filter_alloc(fops, data, persistent);
}

static php_stream_filter_factory chunked_filter_factory = {
	chunked_filter_create
};
/* }}} */

static const struct {
	php_stream_filter_ops *ops;
	php_stream_filter_factory *factory;
} standard_filters[] = {
	{ &strfilter_rot13_ops, &strfilter_rot13_factory },
	{ &strfilter_toupper_ops, &strfilter_toupper_factory },
	{ &strfilter_tolower_ops, &strfilter_tolower_factory },
	{ &strfilter_strip_tags_ops, &strfilter_strip_tags_factory },
	{ &strfilter_convert_ops, &strfilter_convert_factory },
	{ &consumed_filter_ops, &consumed_filter_factory },
	{ &chunked_filter_ops, &chunked_filter_factory },
	/* additional filters to go here */
	{ NULL, NULL }
};

/* {{{ filter MINIT and MSHUTDOWN */
PHP_MINIT_FUNCTION(standard_filters)
{
	int i;

	for (i = 0; standard_filters[i].ops; i++) {
		if (FAILURE == php_stream_filter_register_factory(
					standard_filters[i].ops->label,
					standard_filters[i].factory
					TSRMLS_CC)) {
			return FAILURE;
		}
	}
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(standard_filters)
{
	int i;

	for (i = 0; standard_filters[i].ops; i++) {
		php_stream_filter_unregister_factory(standard_filters[i].ops->label TSRMLS_CC);
	}
	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
