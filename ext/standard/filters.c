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
typedef struct _php_base64_filter
{
	unsigned char erem[3];
	size_t erem_len;
	unsigned int urem;
	unsigned int urem_nbits;
	unsigned int ustat;
} php_base64_filter;

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

static unsigned int b64_tbl_unenc[256] = {
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


static int php_base64_filter_ctor(php_base64_filter *inst)
{
	inst->erem_len = 0;
	inst->urem_nbits = 0;
	inst->ustat = 0;
	return SUCCESS;
}

static void php_base64_filter_dtor(php_base64_filter *inst)
{
	assert(inst != NULL);
	efree(inst);
}

static size_t strfilter_base64_write(php_stream *stream, php_stream_filter *thisfilter,
			const char *buf, size_t count TSRMLS_DC)
{
	register size_t chunk_left, bcnt;
	register unsigned char *ps, *pd;
	size_t nbytes_written;
	char chunk[1024];
	php_base64_filter *inst = (php_base64_filter *)thisfilter->abstract;

	bcnt = count;
	ps = (unsigned char *)buf;
	pd = chunk;
	chunk_left = sizeof(chunk);
	nbytes_written = 0;
 
	/* consume the remainder first */
	switch (inst->erem_len) {
		case 1:
			if (bcnt >= 2) {
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (ps[0] >> 4)];
				*(pd++) = b64_tbl_enc[(unsigned char)(ps[0] << 2) | (ps[1] >> 6)];
				*(pd++) = b64_tbl_enc[ps[1]];
				chunk_left -= 4;
				ps += 2;
				bcnt -= 2;
				inst->erem_len = 0;
			}
			break;

		case 2: 
			if (bcnt >= 1) {
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (inst->erem[1] >> 4)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[1] << 2) | (ps[0] >> 6)];
				*(pd++) = b64_tbl_enc[ps[0]];
				chunk_left -= 4;
				ps += 1;
				bcnt -= 1;
				inst->erem_len = 0;
			}
			break;
	}

	while (bcnt >= 3) {
		*(pd++) = b64_tbl_enc[ps[0] >> 2];
		*(pd++) = b64_tbl_enc[(unsigned char)(ps[0] << 4) | (ps[1] >> 4)];
		*(pd++) = b64_tbl_enc[(unsigned char)(ps[1] << 2) | (ps[2] >> 6)];
		*(pd++) = b64_tbl_enc[ps[2]];

		ps += 3;
		bcnt -=3;
		chunk_left -= 4;

		if (chunk_left < 4) {
			nbytes_written += php_stream_filter_write_next(stream, thisfilter, chunk, sizeof(chunk) - chunk_left);
			pd = chunk;
			chunk_left = sizeof(chunk);
		}
	}

	nbytes_written += php_stream_filter_write_next(stream, thisfilter, chunk, sizeof(chunk) - chunk_left);

	for (;bcnt > 0; bcnt--) {
		inst->erem[inst->erem_len++] = *(ps++);
	}

	return count;
}

static size_t strfilter_base64_read(php_stream *stream, php_stream_filter *thisfilter,
			char *buf, size_t count TSRMLS_DC)
{
	unsigned int urem, urem_nbits;
	unsigned int pack, pack_bcnt;
	size_t nbytes_consumed, nbytes_decoded;
	unsigned int ustat;

	const static unsigned int nbitsof_pack = 8;

	static unsigned char bmask[9] = {
		0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
	};

	count = php_stream_filter_read_next(stream, thisfilter, buf, count);

	urem = ((php_base64_filter *)thisfilter->abstract)->urem;
	urem_nbits = ((php_base64_filter *)thisfilter->abstract)->urem_nbits;
	ustat = ((php_base64_filter *)thisfilter->abstract)->ustat;

	pack = 0;
	pack_bcnt = nbitsof_pack;
	nbytes_consumed = nbytes_decoded = 0;

	while (nbytes_consumed < count) {
		pack_bcnt -= urem_nbits;
		pack |= (urem << pack_bcnt);
		urem_nbits = 0;

		if (pack_bcnt > 0) {
			unsigned int i = b64_tbl_unenc[(unsigned int)((unsigned char *)buf)[nbytes_consumed++]];

			ustat |= i & 0x80;

			if (!(i & 0xc0)) {
				if (ustat) {
					php_error(E_WARNING, "stream filter(%s): invalid base64 sequence", thisfilter->fops->label);
					return 0;
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
					php_error(E_WARNING, "stream filter(%s): unexpected end of stream", thisfilter->fops->label);
					return 0;
				}
			}
		}

		if ((pack_bcnt | ustat) == 0) {
			((unsigned char *)buf)[nbytes_decoded++] = pack;
			pack = 0;
			pack_bcnt = nbitsof_pack;
		}
	}
	((php_base64_filter *)thisfilter->abstract)->urem = urem;
	((php_base64_filter *)thisfilter->abstract)->urem_nbits = urem_nbits;
	((php_base64_filter *)thisfilter->abstract)->ustat = ustat;

	return nbytes_decoded;
}

static int strfilter_base64_flush(php_stream *stream, php_stream_filter *thisfilter, int closing TSRMLS_DC)
{
	if (closing) {
		php_base64_filter *inst = (php_base64_filter *)thisfilter->abstract;

		char chunk[4];
		unsigned char *pd = chunk;

		switch (inst->erem_len) {
			case 1:
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4)];
				*(pd++) = '=';
				*(pd++) = '=';
				inst->erem_len = 0;
				php_stream_filter_write_next(stream, thisfilter, chunk, sizeof(chunk));
				break;

			case 2: 
				*(pd++) = b64_tbl_enc[(inst->erem[0] >> 2)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[0] << 4) | (inst->erem[1] >> 4)];
				*(pd++) = b64_tbl_enc[(unsigned char)(inst->erem[1] << 2)];
				*(pd++) = '=';
				inst->erem_len = 0;
				php_stream_filter_write_next(stream, thisfilter, chunk, sizeof(chunk));
				break;
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
	php_base64_filter_dtor((php_base64_filter *)thisfilter->abstract);
}

static php_stream_filter_ops strfilter_base64_ops = {
	strfilter_base64_write,
	strfilter_base64_read,
	strfilter_base64_flush,
	strfilter_base64_eof,
	strfilter_base64_dtor,
	"string.base64"
};

static php_stream_filter *strfilter_base64_create(const char *filtername, const char *filterparams,
		int filterparamslen, int persistent TSRMLS_DC)
{
	php_base64_filter *inst;

	inst = emalloc(sizeof(php_base64_filter));
	assert(inst);

	if (php_base64_filter_ctor(inst) != SUCCESS) {
		efree(inst);
		return NULL;
	}	
	
	return php_stream_filter_alloc(&strfilter_base64_ops, inst, persistent);
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
	unsigned int bcnt;
	static char qp_digits[] = "0123456789ABCDEF";

	chunk_len = 0;

	ps = (unsigned char *)buf;
	for (bcnt = count; bcnt > 0; bcnt--) {
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
	size_t bcnt;
	unsigned char *ps, *pd;
	unsigned int scan_stat = (unsigned int)thisfilter->abstract;
	unsigned int v;

	bcnt = php_stream_filter_read_next(stream, thisfilter, buf, count);
	nbytes_decoded = 0; 
	ps = pd = (unsigned char *)buf;
	v = 0;

	for (;bcnt > 0; bcnt--) {
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
	{ &strfilter_base64_ops, &strfilter_base64_factory },
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
