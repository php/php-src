/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "php_globals.h"
#include "ext/standard/basic_functions.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"

/* {{{ common "no-opperation" methods */
static int commonfilter_nop_flush(php_stream *stream, php_stream_filter *thisfilter, int closing TSRMLS_DC)
{
	return php_stream_filter_flush_next(stream, thisfilter, closing);
}

static int commonfilter_nop_eof(php_stream *stream, php_stream_filter *thisfilter TSRMLS_DC)
{
	return php_stream_filter_eof_next(stream, thisfilter);
}
/* }}} */

/* {{{ rot13 stream filter implementation */
static char rot13_from[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char rot13_to[] = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

static size_t strfilter_rot13_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	char rotbuf[1024];
	size_t chunk;
	size_t wrote = 0;

	while (count > 0) {
		chunk = count;
		if (chunk > sizeof(rotbuf))
			chunk = sizeof(rotbuf);

		PHP_STRLCPY(rotbuf, buf, sizeof(rotbuf), chunk);
		buf += chunk;
		count -= chunk;

		php_strtr(rotbuf, chunk, rot13_from, rot13_to, 52);
		wrote += php_stream_filter_write_next(stream, thisfilter, rotbuf, chunk);
	}

	return wrote;
}

static size_t strfilter_rot13_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	size_t read;

	read = php_stream_filter_read_next(stream, thisfilter, buf, count);
	php_strtr(buf, read, rot13_from, rot13_to, 52);

	return read;
}

static php_stream_filter_ops strfilter_rot13_ops = {
	strfilter_rot13_write,
	strfilter_rot13_read,
	commonfilter_nop_flush,
	commonfilter_nop_eof,
	NULL,
	"string.rot13"
};

static php_stream_filter *strfilter_rot13_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
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

static size_t strfilter_toupper_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	char tmpbuf[1024];
	size_t chunk;
	size_t wrote = 0;

	while (count > 0) {
		chunk = count;
		if (chunk > sizeof(tmpbuf))
			chunk = sizeof(tmpbuf);

		PHP_STRLCPY(tmpbuf, buf, sizeof(tmpbuf), chunk);
		buf += chunk;
		count -= chunk;

		php_strtr(tmpbuf, chunk, lowercase, uppercase, 26);
		wrote += php_stream_filter_write_next(stream, thisfilter, tmpbuf, chunk);
	}

	return wrote;
}

static size_t strfilter_tolower_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	char tmpbuf[1024];
	size_t chunk;
	size_t wrote = 0;

	while (count > 0) {
		chunk = count;
		if (chunk > sizeof(tmpbuf))
			chunk = sizeof(tmpbuf);

		PHP_STRLCPY(tmpbuf, buf, sizeof(tmpbuf), chunk);
		buf += chunk;
		count -= chunk;

		php_strtr(tmpbuf, chunk, uppercase, lowercase, 26);
		wrote += php_stream_filter_write_next(stream, thisfilter, tmpbuf, chunk);
	}

	return wrote;
}

static size_t strfilter_toupper_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	size_t read;

	read = php_stream_filter_read_next(stream, thisfilter, buf, count);
	php_strtr(buf, read, lowercase, uppercase, 26);

	return read;
}

static size_t strfilter_tolower_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	size_t read;

	read = php_stream_filter_read_next(stream, thisfilter, buf, count);
	php_strtr(buf, read, uppercase, lowercase, 26);

	return read;
}

static php_stream_filter_ops strfilter_toupper_ops = {
	strfilter_toupper_write,
	strfilter_toupper_read,
	commonfilter_nop_flush,
	commonfilter_nop_eof,
	NULL,
	"string.toupper"
};

static php_stream_filter_ops strfilter_tolower_ops = {
	strfilter_tolower_write,
	strfilter_tolower_read,
	commonfilter_nop_flush,
	commonfilter_nop_eof,
	NULL,
	"string.tolower"
};

static php_stream_filter *strfilter_toupper_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
{
	return php_stream_filter_alloc(&strfilter_toupper_ops, NULL, persistent);
}

static php_stream_filter *strfilter_tolower_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
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

/* {{{ base64 stream filter implementation */

typedef enum _php_conv_err_t {
	PHP_CONV_ERR_SUCCESS = SUCCESS,
	PHP_CONV_ERR_UNKNOWN,
	PHP_CONV_ERR_TOO_BIG,
	PHP_CONV_ERR_INVALID_SEQ,
	PHP_CONV_ERR_UNEXPECTED_EOS
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

static php_conv_err_t php_conv_base64_encode_ctor(php_conv_base64_encode *inst)
{
	inst->_super.convert_op = (php_conv_convert_func) php_conv_base64_encode_convert;
	inst->_super.dtor = (php_conv_dtor_func) php_conv_base64_encode_dtor;
	inst->erem_len = 0;
	return SUCCESS;
}

static void php_conv_base64_encode_dtor(php_conv_base64_encode *inst)
{
	/* do nothing */
}

static php_conv_err_t php_conv_base64_encode_convert(php_conv_base64_encode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	register size_t ocnt, icnt;
	register unsigned char *ps, *pd;
	size_t nbytes_written;

	pd = (unsigned char *)(*out_pp);
	ocnt = *out_left_p;
	nbytes_written = 0;

	if (in_pp != NULL && in_left_p != NULL) { 
		ps = (unsigned char *)(*in_pp);
		icnt = *in_left_p;
	
		/* consume the remainder first */
		switch (inst->erem_len) {
			case 1:
				if (icnt >= 2) {
					if (ocnt < 4) {
						return PHP_CONV_ERR_TOO_BIG;
					}
					*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
					*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (ps[0] >> 4)];
					*(pd++) = b64_tbl_enc[(unsigned char)(ps[0] << 2) | (ps[1] >> 6)];
					*(pd++) = b64_tbl_enc[ps[1]];
					ocnt -= 4;
					ps += 2;
					icnt -= 2;
					inst->erem_len = 0;
				}
				break;

			case 2: 
				if (icnt >= 1) {
					if (ocnt < 4) {
						return PHP_CONV_ERR_TOO_BIG;
					}
					*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
					*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (inst->erem[1] >> 4)];
					*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[1] << 2) | (ps[0] >> 6)];
					*(pd++) = b64_tbl_enc[ps[0]];
					ocnt -= 4;
					ps += 1;
					icnt -= 1;
					inst->erem_len = 0;
				}
				break;
		}

		while (icnt >= 3) {
			if (ocnt < 4) {
				*in_pp = (const char *)ps;
				*in_left_p = icnt;
				*out_pp = (char *)pd;
				*out_left_p = ocnt;

				return PHP_CONV_ERR_TOO_BIG;
			}
			*(pd++) = b64_tbl_enc[ps[0] >> 2];
			*(pd++) = b64_tbl_enc[(unsigned char)(ps[0] << 4) | (ps[1] >> 4)];
			*(pd++) = b64_tbl_enc[(unsigned char)(ps[1] << 2) | (ps[2] >> 6)];
			*(pd++) = b64_tbl_enc[ps[2]];

			ps += 3;
			icnt -=3;
			ocnt -= 4;
		}

		for (;icnt > 0; icnt--) {
			inst->erem[inst->erem_len++] = *(ps++);
		}

		*in_pp = (const char *)ps;
		*in_left_p = icnt;
		*out_pp = (char *)pd;
		*out_left_p = ocnt;
	} else {
		switch (inst->erem_len) {
			case 0:
				/* do nothing */
				break;

			case 1:
				if (ocnt < 4) {
					return PHP_CONV_ERR_TOO_BIG;
				}
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4)];
				*(pd++) = '=';
				*(pd++) = '=';
				inst->erem_len = 0;
				ocnt -= 4;
				break;

			case 2: 
				if (ocnt < 4) {
					return PHP_CONV_ERR_TOO_BIG;
				}
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (inst->erem[1] >> 4)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[1] << 2)];
				*(pd++) = '=';
				inst->erem_len = 0;
				ocnt -=4;
				break;

			default:
				/* should not happen... */
				return PHP_CONV_ERR_UNKNOWN;
		}
		*out_pp = (char *)pd;
		*out_left_p = ocnt;
	}

	return PHP_CONV_ERR_SUCCESS;
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

static php_conv_err_t php_conv_base64_decode_convert(php_conv_base64_decode *inst, const char **in_pp, size_t *in_left_p, char **out_pp, size_t *out_left_p)
{
	php_conv_err_t err;

	unsigned int urem, urem_nbits;
	unsigned int pack, pack_bcnt;
	unsigned char *ps, *pd;
	size_t icnt, ocnt;
	unsigned int ustat;

	const static unsigned int nbitsof_pack = 8;

	static unsigned int bmask[17] = {
		0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
		0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff,
		0xffff
	};

	if (in_pp == NULL || in_left_p == NULL) {
		if (inst->eos || inst->urem_nbits == 0) { 
			return SUCCESS;
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
			urem &= bmask[urem_nbits];
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
					urem = i & bmask[urem_nbits];
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
				urem |= (pack << urem_nbits);
				urem_nbits += 8;

				err = PHP_CONV_ERR_TOO_BIG;
				break;
			}
			*(pd++) = pack;
			ocnt--;
			pack = 0;
			pack_bcnt = nbitsof_pack;
		}
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
/* }}} */

typedef struct _php_base64_filter {
	php_conv *write_cd;
	php_conv *read_cd;

	/* ring buffer for read operation */
	char read_buf[4096];
	size_t read_buf_left;
} php_base64_filter;

static int php_base64_filter_ctor(php_base64_filter *inst, int write_conv_mode, int read_conv_mode)
{
	php_conv *write_cd = NULL;
	php_conv *read_cd = NULL;

	/* FIXME: I'll have to replace this ugly dup'ed code by something neat
	   (factories?) in the near future. */ 
	switch (write_conv_mode) {
		case 1:
			write_cd = emalloc(sizeof(php_conv_base64_encode));
			if (php_conv_base64_encode_ctor((php_conv_base64_encode *)write_cd)) {
				efree(write_cd);
				write_cd = NULL;
				goto out_failure;
			}
			break;

		case 2:
			write_cd = emalloc(sizeof(php_conv_base64_decode));
			if (php_conv_base64_decode_ctor((php_conv_base64_decode *)write_cd)) {
				efree(write_cd);
				write_cd = NULL;
				goto out_failure;
			}
			break;

		default:
			write_cd = NULL;
			break;
	}

	switch (read_conv_mode) {
		case 1:
			read_cd = emalloc(sizeof(php_conv_base64_encode));
			if (php_conv_base64_encode_ctor((php_conv_base64_encode *)read_cd)) {
				efree(read_cd);
				read_cd = NULL;
				goto out_failure;
			}
			break;

		case 2:
			read_cd = emalloc(sizeof(php_conv_base64_decode));
			if (php_conv_base64_decode_ctor((php_conv_base64_decode *)read_cd)) {
				efree(read_cd);
				read_cd = NULL;
				goto out_failure;
			}
			break;

		default:
			read_cd = NULL;
			break;
	}

	inst->write_cd = write_cd;
	inst->read_cd = read_cd;

	inst->read_buf_left = 0;

	return SUCCESS;

out_failure:
	if (write_cd != NULL) {
		php_conv_dtor(write_cd);
		efree(write_cd);
	}
	if (read_cd != NULL) {
		php_conv_dtor(read_cd);
		efree(read_cd);
	}
	return FAILURE;
}

static void php_base64_filter_dtor(php_base64_filter *inst)
{
	if (inst->write_cd != NULL) {
		php_conv_dtor(inst->write_cd);
		efree(inst->write_cd);
	}

	if (inst->read_cd != NULL) {
		php_conv_dtor(inst->read_cd);
		efree(inst->read_cd);
	}
}

static size_t strfilter_base64_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	php_base64_filter *inst = (php_base64_filter *)thisfilter->abstract;

	if (inst->write_cd != NULL) {
		char bucket_buf[2048];
		size_t out_left, in_left;
		const char *in_p;
		char *out_p;

		in_p = buf;
		in_left = count;
		out_p = bucket_buf;
		out_left = sizeof(bucket_buf);

		while (in_left > 0) {
			php_conv_err_t err;

			err = php_conv_convert(inst->write_cd, &in_p, &in_left, &out_p, &out_left);
			switch (err) {
				case PHP_CONV_ERR_TOO_BIG:
					php_stream_filter_write_next(stream, thisfilter, bucket_buf, sizeof(bucket_buf) - out_left);
					out_p = bucket_buf;
					out_left = sizeof(bucket_buf);
					break;

				case PHP_CONV_ERR_UNKNOWN:
					php_error(E_WARNING, "stream filter(%s): unknown error", thisfilter->fops->label, err);
					return 0;

				case PHP_CONV_ERR_INVALID_SEQ:
					php_error(E_WARNING, "stream filter(%s): invalid base64 sequence", thisfilter->fops->label, err);
					return 0;
	
 				case PHP_CONV_ERR_UNEXPECTED_EOS:
					php_error(E_WARNING, "stream filter(%s): unexpected end of stream", thisfilter->fops->label, err);
					return 0;

				default:
					break;
			}
		}
		php_stream_filter_write_next(stream, thisfilter, bucket_buf, sizeof(bucket_buf) - out_left);
		return (size_t)(count - in_left);
	}	
	return php_stream_filter_write_next(stream, thisfilter, buf, count);
}

static size_t strfilter_base64_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	php_base64_filter *inst = (php_base64_filter *)thisfilter->abstract;
	size_t retval;

	if (inst->read_cd != NULL) {
		php_conv_err_t err = PHP_CONV_ERR_SUCCESS;
		size_t out_left;
		const char *in_p;
		char *out_p;
	
		in_p = inst->read_buf + (sizeof(inst->read_buf) - inst->read_buf_left);
		out_p = buf;
		out_left = count;

		while (out_left > 0 && err != PHP_CONV_ERR_TOO_BIG) {
			if (inst->read_buf_left == 0) { 
				inst->read_buf_left = php_stream_filter_read_next(stream, thisfilter, inst->read_buf, sizeof(inst->read_buf));
				in_p = inst->read_buf;
			}
			if (inst->read_buf_left == 0) {			
				/* reached end of stream */ 
				err = php_conv_convert(inst->read_cd, NULL, NULL, &out_p, &out_left);
				if (err == PHP_CONV_ERR_SUCCESS) {
					break;
				}
			} else {
				err = php_conv_convert(inst->read_cd, &in_p, &(inst->read_buf_left), &out_p, &out_left);
			}

			switch (err) {
				case PHP_CONV_ERR_UNKNOWN:
					php_error(E_WARNING, "stream filter(%s): unknown error", thisfilter->fops->label, err);
					return 0;

				case PHP_CONV_ERR_INVALID_SEQ:
					php_error(E_WARNING, "stream filter(%s): invalid base64 sequence", thisfilter->fops->label, err);
					return 0;

				case PHP_CONV_ERR_UNEXPECTED_EOS:
					php_error(E_WARNING, "stream filter(%s): unexpected end of stream", thisfilter->fops->label, err);
					return 0;

				default:
					break;
			}
		}
		retval = (size_t)(count - out_left);
	} else {
		retval = php_stream_filter_read_next(stream, thisfilter, buf, count);
	}
	return retval;
}

static int strfilter_base64_flush(php_stream *stream, php_stream_filter *thisfilter, int closing TSRMLS_DC)
{
	if (closing) {
		php_base64_filter *inst = (php_base64_filter *)thisfilter->abstract;

		if (inst->write_cd != NULL) {
			php_conv_err_t err;
			char bucket_buf[1024];
			char *out_p;
			size_t out_left;

			out_p = bucket_buf;
			out_left = sizeof(bucket_buf);

			for (;;) {
				err = php_conv_convert(inst->write_cd, NULL, NULL, &out_p, &out_left);
				if (err == PHP_CONV_ERR_SUCCESS) {
					break;
				} else if (err == PHP_CONV_ERR_TOO_BIG) {
					php_stream_filter_write_next(stream, thisfilter, bucket_buf, sizeof(bucket_buf) - out_left);
					out_p = bucket_buf;
					out_left = sizeof(bucket_buf);
				} else {
					php_error(E_WARNING, "stream filter(%s): unknown error", thisfilter->fops->label);
					return 0;
				}
			}

			php_stream_filter_write_next(stream, thisfilter, bucket_buf, sizeof(bucket_buf) - out_left);
		}
	}
	return php_stream_filter_flush_next(stream, thisfilter, closing);
}

static int strfilter_base64_eof(php_stream *stream, php_stream_filter *thisfilter TSRMLS_DC)
{
	return php_stream_filter_eof_next(stream, thisfilter);
}

static void strfilter_base64_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	assert(thisfilter->abstract != NULL);

	php_base64_filter_dtor((php_base64_filter *)thisfilter->abstract);
	efree(thisfilter->abstract);
}

static php_stream_filter_ops strfilter_base64_encode_ops = {
	strfilter_base64_write,
	strfilter_base64_read,
	strfilter_base64_flush,
	strfilter_base64_eof,
	strfilter_base64_dtor,
	"string.base64.encode"
};

static php_stream_filter_ops strfilter_base64_decode_ops = {
	strfilter_base64_write,
	strfilter_base64_read,
	strfilter_base64_flush,
	strfilter_base64_eof,
	strfilter_base64_dtor,
	"string.base64.decode"
};

static php_stream_filter *strfilter_base64_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
{
	php_base64_filter *inst;
	php_stream_filter *retval = NULL;

	inst = emalloc(sizeof(php_base64_filter));
	assert(inst != NULL);

	if (strcasecmp(filtername, strfilter_base64_encode_ops.label) == 0) {
		if (php_base64_filter_ctor(inst, 1, 2) != SUCCESS) {
			efree(inst);
			return NULL;
		}	
		retval = php_stream_filter_alloc(&strfilter_base64_encode_ops, inst, persistent);
	} else {
		if (php_base64_filter_ctor(inst, 2, 1) != SUCCESS) {
			efree(inst);
			return NULL;
		}	
		retval = php_stream_filter_alloc(&strfilter_base64_decode_ops, inst, persistent);
	}
	
	return retval;
}

static php_stream_filter_factory strfilter_base64_factory = {
	strfilter_base64_create
};
/* }}} */

/* {{{ quoted-printable stream filter implementation */

static size_t strfilter_quoted_printable_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	char chunk[4096];
	size_t chunk_len;
	unsigned char *ps;
	unsigned int icnt;
	static char qp_digits[] = "0123456789ABCDEF";

	chunk_len = 0;

	ps = (unsigned char *)buf;
	for (icnt = count; icnt > 0; icnt--) {
		unsigned int c = *(ps++);

		if ((c >= 33 && c <= 60) || (c >= 62 && c <= 126)) { 
			if (chunk_len >= sizeof(chunk)) {
				php_stream_filter_write_next(stream, thisfilter, chunk, chunk_len);
				chunk_len = 0;
			}
			chunk[chunk_len++] = c;
		} else {
			if (chunk_len > sizeof(chunk) - 3) {
				php_stream_filter_write_next(stream, thisfilter, chunk, chunk_len);
				chunk_len = 0;
			}
			chunk[chunk_len++] = '=';
			chunk[chunk_len++] = qp_digits[(c >> 4)];
			chunk[chunk_len++] = qp_digits[(c & 0x0f)]; 
		}
	}
	php_stream_filter_write_next(stream, thisfilter, chunk, chunk_len);

	return count;
}

static size_t strfilter_quoted_printable_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	size_t nbytes_decoded;
	size_t icnt;
	unsigned char *ps, *pd;
	unsigned int scan_stat = (unsigned int)thisfilter->abstract;
	unsigned int v;

	icnt = php_stream_filter_read_next(stream, thisfilter, buf, count);
	nbytes_decoded = 0; 
	ps = pd = (unsigned char *)buf;
	v = 0;

	for (;icnt > 0; icnt--) {
		switch (scan_stat) {
			case 0:
				if (*ps == '=') {
					scan_stat = 1;
				} else {
					*(pd++) = *ps;
					nbytes_decoded++;
				}
				break;

			case 1: case 2: {
				unsigned int nbl = (*ps >= 'A' ? *ps - 0x37 : *ps - 0x30);

				if (nbl > 15) {
					php_error(E_WARNING, "stream filter(%s): invalid character sequence", thisfilter->fops->label); 
					return 0;
				}
				v = (v << 4) | nbl;

				scan_stat++;
				if (scan_stat == 3) {
					*(pd++) = v;
					nbytes_decoded++;
					scan_stat = 0;
				}
			}
		}
		ps++;
	}
	(unsigned int)thisfilter->abstract = scan_stat;
	return nbytes_decoded;
}

static php_stream_filter_ops strfilter_quoted_printable_ops = {
	strfilter_quoted_printable_write,
	strfilter_quoted_printable_read,
	commonfilter_nop_flush,
	commonfilter_nop_eof,
	NULL,
	"string.quoted-printable"
};

static php_stream_filter *strfilter_quoted_printable_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
{
	return php_stream_filter_alloc(&strfilter_quoted_printable_ops, 0, persistent);
}

static php_stream_filter_factory strfilter_quoted_printable_factory = {
	strfilter_quoted_printable_create
};
/* }}} */

static const struct {
	php_stream_filter_ops *ops;
	php_stream_filter_factory *factory;
} standard_filters[] = {
	{ &strfilter_rot13_ops, &strfilter_rot13_factory },
	{ &strfilter_toupper_ops, &strfilter_toupper_factory },
	{ &strfilter_tolower_ops, &strfilter_tolower_factory },
	{ &strfilter_base64_decode_ops, &strfilter_base64_factory },
	{ &strfilter_base64_encode_ops, &strfilter_base64_factory },
	{ &strfilter_quoted_printable_ops, &strfilter_quoted_printable_factory },
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
