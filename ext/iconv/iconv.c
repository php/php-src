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
   | Authors: Rui Hirokawa <rui_hirokawa@ybb.ne.jp>                       |
   |          Stig Bakken <ssb@fast.no>                                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/info.h"
#include "main/php_output.h"
#include "SAPI.h"
#include "php_ini.h"

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#include <errno.h>

#include "php_iconv.h"

#ifdef HAVE_ICONV

#ifdef HAVE_GICONV_H
#include <giconv.h>
#else
#include <iconv.h>
#endif

#ifdef HAVE_GLIBC_ICONV
#include <gnu/libc-version.h>
#endif

#include "ext/standard/php_smart_str.h"
#include "ext/standard/base64.h"

#ifdef HAVE_LIBICONV
#define LIBICONV_PLUG
#define icv_open(a, b) libiconv_open(a, b)
#define icv_close(a) libiconv_close(a)
#define icv(a, b, c, d, e) libiconv(a, b, c, d, e)
#else
#define icv_open(a, b) iconv_open(a, b)
#define icv_close(a) iconv_close(a)
#define icv(a, b, c, d, e) iconv(a, (char **) b, c, d, e)
#endif

#define _php_iconv_memequal(a, b, c) \
  ((c) == sizeof(unsigned long) ? *((unsigned long *)(a)) == *((unsigned long *)(b)) : ((c) == sizeof(unsigned int) ? *((unsigned int *)(a)) == *((unsigned int *)(b)) : memcmp(a, b, c) == 0))

/* {{{ iconv_functions[]
 */
function_entry iconv_functions[] = {
	PHP_NAMED_FE(iconv,php_if_iconv,				NULL)
	PHP_FE(ob_iconv_handler,						NULL)
	PHP_FE(iconv_get_encoding,						NULL)
	PHP_FE(iconv_set_encoding,						NULL)
	PHP_FE(iconv_strlen,							NULL)
	PHP_FE(iconv_substr,							NULL)
	PHP_FE(iconv_strpos,							NULL)
	PHP_FE(iconv_strrpos,							NULL)
	PHP_FE(iconv_mime_encode,						NULL)
	PHP_FE(iconv_mime_decode,						NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ iconv_module_entry
 */
zend_module_entry iconv_module_entry = {
	STANDARD_MODULE_HEADER,
	"iconv",
	iconv_functions,
	PHP_MINIT(miconv),
	PHP_MSHUTDOWN(miconv),
	NULL,
	NULL,
	PHP_MINFO(miconv),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(iconv)

#ifdef COMPILE_DL_ICONV
ZEND_GET_MODULE(iconv)
#endif

/* {{{ typedef enum php_iconv_err_t */
typedef enum _php_iconv_err_t {
	PHP_ICONV_ERR_SUCCESS           = SUCCESS,
	PHP_ICONV_ERR_CONVERTER         = 1,
	PHP_ICONV_ERR_WRONG_CHARSET     = 2,
	PHP_ICONV_ERR_TOO_BIG           = 3,
	PHP_ICONV_ERR_ILLEGAL_SEQ       = 4,
	PHP_ICONV_ERR_ILLEGAL_CHAR      = 5,
	PHP_ICONV_ERR_UNKNOWN           = 6,
	PHP_ICONV_ERR_MALFORMED         = 7
} php_iconv_err_t;
/* }}} */

/* {{{ typedef enum php_iconv_enc_scheme_t */
typedef enum _php_iconv_enc_scheme_t {
	PHP_ICONV_ENC_SCHEME_BASE64,
	PHP_ICONV_ENC_SCHEME_QPRINT
} php_iconv_enc_scheme_t;
/* }}} */

/* {{{ prototypes */ 
static php_iconv_err_t _php_iconv_appendl(smart_str *d, const char *s, size_t l, iconv_t cd);
static php_iconv_err_t _php_iconv_appendc(smart_str *d, const char c, iconv_t cd);

static php_iconv_err_t php_iconv_string(const char * in_p, size_t in_len, char **out, size_t *out_len, const char *out_charset, const char *in_charset);

static void _php_iconv_show_error(php_iconv_err_t err, const char *out_charset, const char *in_charset TSRMLS_DC);

static php_iconv_err_t _php_iconv_strlen(unsigned int *pretval, const char *str, size_t nbytes, const char *enc);

static php_iconv_err_t _php_iconv_substr(smart_str *pretval, const char *str, size_t nbytes, int offset, int len, const char *enc);

static php_iconv_err_t _php_iconv_strpos(unsigned int *pretval, const char *haystk, size_t haystk_nbytes, const char *ndl, size_t ndl_nbytes, int offset, const char *enc);

static php_iconv_err_t _php_iconv_mime_encode(smart_str *pretval, const char *fname, size_t fname_nbytes, const char *fval, size_t fval_nbytes, unsigned int max_line_len, const char *lfchars, php_iconv_enc_scheme_t enc_scheme, const char *out_charset, const char *enc);

static php_iconv_err_t _php_iconv_mime_decode(smart_str *pretval, const char *str, size_t str_nbytes, const char *enc);
/* }}} */

/* {{{ static globals */
static char _generic_superset_name[] = "UCS-4LE";
#define GENERIC_SUPERSET_NAME _generic_superset_name
#define GENERIC_SUPERSET_NBYTES 4
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("iconv.input_encoding",    ICONV_INPUT_ENCODING,    PHP_INI_ALL, OnUpdateString, input_encoding,    zend_iconv_globals, iconv_globals)
	STD_PHP_INI_ENTRY("iconv.output_encoding",   ICONV_OUTPUT_ENCODING,   PHP_INI_ALL, OnUpdateString, output_encoding,   zend_iconv_globals, iconv_globals)
	STD_PHP_INI_ENTRY("iconv.internal_encoding", ICONV_INTERNAL_ENCODING, PHP_INI_ALL, OnUpdateString, internal_encoding, zend_iconv_globals, iconv_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_iconv_init_globals */
static void php_iconv_init_globals(zend_iconv_globals *iconv_globals)
{
	iconv_globals->input_encoding = NULL;
	iconv_globals->output_encoding = NULL;
	iconv_globals->internal_encoding = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(miconv)
{
	char *version = "unknown";

	ZEND_INIT_MODULE_GLOBALS(iconv, php_iconv_init_globals, NULL);
	REGISTER_INI_ENTRIES();

#if HAVE_LIBICONV
	{
		static char buf[16];
		snprintf(buf, sizeof(buf), "%d.%d",
		    ((_libiconv_version >> 8) & 0x0f), (_libiconv_version & 0x0f)); 
		version = buf;
	}
#elif HAVE_GLIBC_ICONV
	version = (char *)gnu_get_libc_version();
#endif

#ifdef PHP_ICONV_IMPL
	REGISTER_STRING_CONSTANT("ICONV_IMPL", PHP_ICONV_IMPL, CONST_CS | CONST_PERSISTENT);
#elif HAVE_LIBICONV
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "libiconv", CONST_CS | CONST_PERSISTENT);
#else
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "unknown", CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_STRING_CONSTANT("ICONV_VERSION", version, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(miconv)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(miconv)
{
	zval iconv_impl, iconv_ver;

	zend_get_constant("ICONV_IMPL", sizeof("ICONV_IMPL")-1, &iconv_impl TSRMLS_CC);
	zend_get_constant("ICONV_VERSION", sizeof("ICONV_VERSION")-1, &iconv_ver TSRMLS_CC);

	php_info_print_table_start();
	php_info_print_table_row(2, "iconv support", "enabled");
	php_info_print_table_row(2, "iconv implementation", Z_STRVAL(iconv_impl));
	php_info_print_table_row(2, "iconv library version", Z_STRVAL(iconv_ver));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

	zval_dtor(&iconv_impl);
	zval_dtor(&iconv_ver);
}
/* }}} */

unsigned char *php_quot_print_decode(const unsigned char *str, size_t length, size_t *ret_length)
{
	register unsigned int i;
	register unsigned const char *p1;
	register unsigned char *p2;
	register unsigned int h_nbl, l_nbl;

	size_t decoded_len;
	unsigned char *retval;

	static unsigned int hexval_tbl[256] = {
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 16, 16, 16, 16, 16, 16,
		16, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
	};

	i = length, p1 = str; decoded_len = length;

	while (i > 0 && *p1 != '\0') {
		if (*p1 == '=') {
			decoded_len -= 2;
			p1 += 2;
			i -= 2;
		}
		p1++;
		i--;
	}

	retval = emalloc(decoded_len + 1);
	i = length; p1 = str; p2 = retval;

	while (i > 0 && *p1 != '\0') {
		if (*p1 == '=') {
			if (i < 2 ||
				(h_nbl = hexval_tbl[p1[1]]) > 15 ||
				(l_nbl = hexval_tbl[p1[2]]) > 15) { 

				efree(retval);
				return NULL;
			}

			*(p2++) = (h_nbl << 4) | l_nbl;
			i -= 3;
			p1 += 3;
		} else {
			*(p2++) = *p1;
			i--;
			p1++;
		}
	}

	*p2 = '\0';
	*ret_length = decoded_len;
	return retval;
}

/* {{{ _php_iconv_appendl() */
static php_iconv_err_t _php_iconv_appendl(smart_str *d, const char *s, size_t l, iconv_t cd)
{
	const char *in_p = s;
	size_t in_left = l;
	char *out_p;
	size_t out_left = 0;
	size_t buf_growth = 128;
#if !ICONV_SUPPORTS_ERRNO
	size_t prev_in_left = in_left;
#endif

	if (in_p != NULL) {
		while (in_left > 0) {
			out_left = buf_growth - out_left;
			{
				size_t newlen;
				smart_str_alloc((d), out_left, 0);
			}

			out_p = (d)->c + (d)->len;

			if (icv(cd, &in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
				switch (errno) { 
					case EINVAL:
						return PHP_ICONV_ERR_ILLEGAL_CHAR;

					case EILSEQ:
						return PHP_ICONV_ERR_ILLEGAL_SEQ;

					case E2BIG:
						break;

					default:
						return PHP_ICONV_ERR_UNKNOWN;
				}
#else
				if (prev_in_left == in_left) {
					return PHP_ICONV_ERR_UNKNOWN;		
				}
#endif
			}
#if !ICONV_SUPPORTS_ERRNO
			prev_in_left = in_left;
#endif
			(d)->len += (buf_growth - out_left);
			buf_growth <<= 1;
		}
	} else {
		for (;;) {
			out_left = buf_growth - out_left;
			{
				size_t newlen;
				smart_str_alloc((d), out_left, 0);
			}

			out_p = (d)->c + (d)->len;

			if (icv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)0) {
				(d)->len += (buf_growth - out_left);
				break;
			} else {
#if ICONV_SUPPORTS_ERRNO
				if (errno != E2BIG) {
					return PHP_ICONV_ERR_UNKNOWN;
				}
#else
				if (out_left != 0) {
					return PHP_ICONV_ERR_UNKNOWN;
				}	
#endif
			}
			(d)->len += (buf_growth - out_left);
			buf_growth <<= 1;
		}
	}
	return PHP_ICONV_ERR_SUCCESS;
}
/* }}} */

/* {{{ _php_iconv_appendc() */
static php_iconv_err_t _php_iconv_appendc(smart_str *d, const char c, iconv_t cd)
{
	return _php_iconv_appendl(d, &c, 1, cd);
}
/* }}} */

/* {{{ php_iconv_string()
 */
php_iconv_err_t php_iconv_string(const char *in_p, size_t in_len,
							char **out, size_t *out_len,
							const char *out_charset, const char *in_charset)
{
#if !ICONV_SUPPORTS_ERRNO
	unsigned int in_size, out_size, out_left;
	char *out_buffer, *out_p;
	iconv_t cd;
	size_t result;
	typedef unsigned int ucs4_t;

	*out = NULL;
	*out_len = 0;

	/*
	  This is not the right way to get output size...
	  This is not space efficient for large text.
	  This is also problem for encoding like UTF-7/UTF-8/ISO-2022 which
	  a single char can be more than 4 bytes.
	  I added 15 extra bytes for safety. <yohgaki@php.net>
	*/
	out_size = in_len * sizeof(ucs4_t) + 15;
	out_left = out_size;

	in_size = in_len;

	cd = icv_open(out_charset, in_charset);
	
	if (cd == (iconv_t)(-1)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}

	out_buffer = (char *) emalloc(out_size + 1);
	out_p = out_buffer;
	
	result = icv(cd, (const char **) &in_p, &in_size, (char **)
				&out_p, &out_left);
	
	if (result == (size_t)(-1)) {
		efree(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	if (out_left < 8) {
		out_buffer = (char *) erealloc(out_buffer, out_size + 8);
	}

	/* flush the shift-out sequences */ 
	result = icv(cd, NULL, NULL, &out_p, &out_left);

	if (result == (size_t)(-1)) {
		efree(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	*out_len = out_size - out_left;
	out_buffer[*out_len] = '\0';
	*out = out_buffer;

	icv_close(cd);

	return PHP_ICONV_ERR_SUCCESS;

#else
	/*
	  iconv supports errno. Handle it better way.
	*/
	iconv_t cd;
	size_t in_left, out_size, out_left;
	char *out_p, *out_buf, *tmp_buf;
	size_t bsz, result = 0;
	php_iconv_err_t retval = PHP_ICONV_ERR_SUCCESS;

	*out = NULL;
	*out_len = 0;

	cd = icv_open(out_charset, in_charset);

	if (cd == (iconv_t)(-1)) {
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
	}
	in_left= in_len;
	out_left = in_len + 32; /* Avoid realloc() most cases */ 
	out_size = 0;
	bsz = out_left;
	out_buf = (char *) emalloc(bsz+1); 
	out_p = out_buf;

	while (in_left > 0) {
		result = icv(cd, (const char **) &in_p, &in_left, (char **) &out_p, &out_left);
		out_size = bsz - out_left;
		if (result == (size_t)(-1)) {
			if (errno == E2BIG && in_left > 0) {
				/* converted string is longer than out buffer */
				bsz += in_len;

				tmp_buf = (char*) erealloc(out_buf, bsz+1);

				if (tmp_buf != NULL) {
					out_p = out_buf = tmp_buf;
					out_p += out_size;
					out_left = bsz - out_size;
					continue;	
				}
			}
		}
		break;
	}

	if (result != (size_t)(-1)) {
		/* flush the shift-out sequences */ 
		for (;;) {
		   	result = icv(cd, NULL, NULL, (char **) &out_p, &out_left);
			out_size = bsz - out_left;

			if (result != (size_t)(-1)) {
				break;
			}

			if (errno == E2BIG) {
				bsz += 16;
				tmp_buf = (char *) erealloc(out_buf, bsz);

				if (tmp_buf == NULL) {
					break;
				}
				
				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
			} else {
				break;
			}
		}
	}

	icv_close(cd);

	if (result == (size_t)(-1)) {
		switch (errno) {
			case EINVAL:
				retval = PHP_ICONV_ERR_ILLEGAL_CHAR;
				break;

			case EILSEQ:
				retval = PHP_ICONV_ERR_ILLEGAL_SEQ;
				break;

			case E2BIG:
				/* should not happen */
				retval = PHP_ICONV_ERR_TOO_BIG;
				break;

			default:
				/* other error */
				retval = PHP_ICONV_ERR_UNKNOWN;
				efree(out_buf);
				return PHP_ICONV_ERR_UNKNOWN;
		}
	}
	*out_p = '\0';
	*out = out_buf;
	*out_len = out_size;
	return retval;
#endif
}
/* }}} */

/* {{{ _php_iconv_strlen() */
static php_iconv_err_t _php_iconv_strlen(unsigned int *pretval, const char *str, size_t nbytes, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES*2];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	unsigned int cnt;

	*pretval = (unsigned int)-1;

	cd = icv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
#else
		return PHP_ICONV_ERR_UNKNOWN;
#endif
	}

	out_left = 0;

	for (in_p = str, in_left = nbytes, cnt = 0; in_left > 0; cnt+=2) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (icv(cd, &in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
				break;
			}
		}
	}

	if (out_left > 0) {
		cnt -= out_left / GENERIC_SUPERSET_NBYTES; 
	}

#if ICONV_SUPPORTS_ERRNO
	switch (errno) {
		case EINVAL:
			err = PHP_ICONV_ERR_ILLEGAL_CHAR;
			break;

		case EILSEQ:
			err = PHP_ICONV_ERR_ILLEGAL_SEQ;
			break;

		case E2BIG:
			*pretval = cnt;
			break;

		default:
			err = PHP_ICONV_ERR_UNKNOWN;
			break;
	}
#else
	*pretval = cnt;
#endif

	icv_close(cd);

	return err;
}

/* }}} */

/* {{{ _php_iconv_substr() */
static php_iconv_err_t _php_iconv_substr(smart_str *pretval,
	const char *str, size_t nbytes, int offset, int len, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd1, cd2;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	unsigned int cnt;

	/* normalize the offset and the length */
	if (offset < 0 || len < 0) {
		unsigned int total_len;
		err = _php_iconv_strlen(&total_len, str, nbytes, enc);
		if (err != PHP_ICONV_ERR_SUCCESS) {
			return err;
		}
		if (offset < 0) {
			if ((offset += total_len) < 0) {
				offset = 0;
			}
		}
		if (len < 0) {
			if ((len += (total_len - offset)) < 0) {
				len = 0;
			}
		}
	}

	cd1 = icv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd1 == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
#else
		return PHP_ICONV_ERR_UNKNOWN;
#endif
	}

	cd2 = NULL;

	for (in_p = str, in_left = nbytes, cnt = 0; in_left > 0 && len > 0; ++cnt) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (icv(cd1, &in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
				break;
			}
		}

		if (cnt >= offset) {
			if (cd2 == NULL) {
				cd2 = icv_open(enc, GENERIC_SUPERSET_NAME);

				if (cd2 == (iconv_t)(-1)) {
					cd2 = NULL;
#if ICONV_SUPPORTS_ERRNO
					if (errno == EINVAL) {
						err = PHP_ICONV_ERR_WRONG_CHARSET;
					} else {
						err = PHP_ICONV_ERR_CONVERTER;
					}
#else
					err = PHP_ICONV_ERR_UNKNOWN;
#endif
					break;
				}
			}

			_php_iconv_appendl(pretval, buf, sizeof(buf), cd2);
			--len;
		}

	}

#if ICONV_SUPPORTS_ERRNO
	switch (errno) {
		case EINVAL:
			err = PHP_ICONV_ERR_ILLEGAL_CHAR;
			break;

		case EILSEQ:
			err = PHP_ICONV_ERR_ILLEGAL_SEQ;
			break;

		case E2BIG:
			break;

		default:
			err = PHP_ICONV_ERR_UNKNOWN;
			break;
	}
#endif
	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (cd2 != NULL) {
			_php_iconv_appendl(pretval, NULL, 0, cd2);
		}
		smart_str_0(pretval);
	}

	if (cd1 != NULL) {
		icv_close(cd1);
	}

	if (cd2 != NULL) {
		icv_close(cd2);
	}	
	return err;
}

/* }}} */

/* {{{ _php_iconv_strpos() */
static php_iconv_err_t _php_iconv_strpos(unsigned int *pretval,
	const char *haystk, size_t haystk_nbytes,
	const char *ndl, size_t ndl_nbytes,
	int offset, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	unsigned int cnt;

	char *ndl_buf;
	const char *ndl_buf_p;
	size_t ndl_buf_len, ndl_buf_left;

	unsigned int match_ofs;

	*pretval = (unsigned int)-1;

	err = php_iconv_string(ndl, ndl_nbytes,
		&ndl_buf, &ndl_buf_len, GENERIC_SUPERSET_NAME, enc);

	if (err != PHP_ICONV_ERR_SUCCESS) {
		if (ndl_buf != NULL) {
			efree(ndl_buf);
		}
		return err;
	}

	cd = icv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
#else
		return PHP_ICONV_ERR_UNKNOWN;
#endif
	}

	ndl_buf_p = ndl_buf;
	ndl_buf_left = ndl_buf_len;
	match_ofs = (unsigned int)-1;

	for (in_p = haystk, in_left = haystk_nbytes, cnt = 0; in_left > 0; ++cnt) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (icv(cd, &in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
#if ICONV_SUPPORTS_ERRNO
				switch (errno) {
					case EINVAL:
						err = PHP_ICONV_ERR_ILLEGAL_CHAR;
						break;

					case EILSEQ:
						err = PHP_ICONV_ERR_ILLEGAL_SEQ;
						break;

					case E2BIG:
						break;

					default:
						err = PHP_ICONV_ERR_UNKNOWN;
						break;
				}
#endif
				break;
			}
		}
		if (offset >= 0) {
			if (cnt >= offset) {
				if (_php_iconv_memequal(buf, ndl_buf_p, sizeof(buf))) {
					if (match_ofs == (unsigned int)-1) {
						match_ofs = cnt;
					}
					ndl_buf_p += GENERIC_SUPERSET_NBYTES;
					ndl_buf_left -= GENERIC_SUPERSET_NBYTES;
					if (ndl_buf_left == 0) {
						*pretval = match_ofs;
						break;
					}
				} else {
					unsigned int i, j, lim;

					i = 0;
					j = GENERIC_SUPERSET_NBYTES;
					lim = (unsigned int)(ndl_buf_p - ndl_buf);

					while (j < lim) {
						if (_php_iconv_memequal(&ndl_buf[j], &ndl_buf[i],
						           GENERIC_SUPERSET_NBYTES)) {
							i += GENERIC_SUPERSET_NBYTES;
						} else {
							j -= i;
							i = 0;
						}
						j += GENERIC_SUPERSET_NBYTES;
					}

					if (!_php_iconv_memequal(buf, &ndl_buf[i], sizeof(buf))) {
						i = 0;
					}

					if (i == 0) {
						match_ofs = (unsigned int)-1;
					} else {
						match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
						i += GENERIC_SUPERSET_NBYTES;
					}
					ndl_buf_p = &ndl_buf[i];
					ndl_buf_left = ndl_buf_len - i;
				}
			}
		} else {
			if (_php_iconv_memequal(buf, ndl_buf_p, sizeof(buf))) {
				if (match_ofs == (unsigned int)-1) {
					match_ofs = cnt;
				}
				ndl_buf_p += GENERIC_SUPERSET_NBYTES;
				ndl_buf_left -= GENERIC_SUPERSET_NBYTES;
				if (ndl_buf_left == 0) {
					*pretval = match_ofs;
					ndl_buf_p = ndl_buf;
					ndl_buf_left = ndl_buf_len;
					match_ofs = -1;
				}
			} else {
				unsigned int i, j, lim;

				i = 0;
				j = GENERIC_SUPERSET_NBYTES;
				lim = (unsigned int)(ndl_buf_p - ndl_buf);

				while (j < lim) {
					if (_php_iconv_memequal(&ndl_buf[j], &ndl_buf[i],
							   GENERIC_SUPERSET_NBYTES)) {
						i += GENERIC_SUPERSET_NBYTES;
					} else {
						j -= i;
						i = 0;
					}
					j += GENERIC_SUPERSET_NBYTES;
				}

				if (!_php_iconv_memequal(buf, &ndl_buf[i], sizeof(buf))) {
					i = 0;
				}

				if (i == 0) {
					match_ofs = (unsigned int)-1;
				} else {
					match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
					i += GENERIC_SUPERSET_NBYTES;
				}
				ndl_buf_p = &ndl_buf[i];
				ndl_buf_left = ndl_buf_len - i;
			}
		}
	}

	if (ndl_buf) {
		efree(ndl_buf);
	}
	
	icv_close(cd);

	return err;
}
/* }}} */

/* {{{ _php_iconv_mime_encode() */
static php_iconv_err_t _php_iconv_mime_encode(smart_str *pretval, const char *fname, size_t fname_nbytes, const char *fval, size_t fval_nbytes, unsigned int max_line_len, const char *lfchars, php_iconv_enc_scheme_t enc_scheme, const char *out_charset, const char *enc)
{
	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;
	iconv_t cd = (iconv_t)(-1), cd_pl = (iconv_t)(-1);
	unsigned int char_cnt = 0;
	size_t out_charset_len;
	size_t lfchars_len;
	char *buf = NULL;
	char *encoded = NULL;
	size_t encoded_len;
	const char *in_p;
	size_t in_left;
	char *out_p;
	size_t out_left;

	out_charset_len = strlen(out_charset);
	lfchars_len = strlen(lfchars);

	if ((fname_nbytes + 2) >= max_line_len
		|| (out_charset_len + 12) >= max_line_len) {
		/* field name is too long */
		err = PHP_ICONV_ERR_TOO_BIG; 
		goto out;
	}

	cd_pl = icv_open("ASCII", enc);
	if (cd_pl == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			err = PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			err = PHP_ICONV_ERR_CONVERTER;
		}
#else
		err = PHP_ICONV_ERR_UNKNOWN;
#endif
		goto out;
	}

	cd = icv_open(out_charset, enc);
	if (cd == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			err = PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			err = PHP_ICONV_ERR_CONVERTER;
		}
#else
		err = PHP_ICONV_ERR_UNKNOWN;
#endif
		goto out;
	}

	buf = emalloc(max_line_len + 5);

	char_cnt = max_line_len;

	_php_iconv_appendl(pretval, fname, fname_nbytes, cd_pl);
	char_cnt -= fname_nbytes;
	smart_str_appendl(pretval, ": ", sizeof(": ") - 1);
	char_cnt -= 2;

	in_p = fval;
	in_left = fval_nbytes; 

	do {
		size_t prev_in_left;
		size_t out_size;

		if (char_cnt < (out_charset_len + 12)) {
			/* lfchars must be encoded in ASCII here*/
			smart_str_appendl(pretval, lfchars, lfchars_len);
			smart_str_appendc(pretval, ' ');
			char_cnt = max_line_len - 1;
		} 
 
		smart_str_appendl(pretval, "=?", sizeof("=?") - 1);
		char_cnt -= 2;
		smart_str_appendl(pretval, out_charset, out_charset_len);
		char_cnt -= out_charset_len;
		smart_str_appendc(pretval, '?');
		char_cnt --;

		switch (enc_scheme) {
			case PHP_ICONV_ENC_SCHEME_BASE64: {
				size_t ini_in_left;
				const char *ini_in_p;
				size_t out_reserved = 4;

				smart_str_appendc(pretval, 'B');
				char_cnt--;
				smart_str_appendc(pretval, '?');
				char_cnt--;

				prev_in_left = ini_in_left = in_left;
				ini_in_p = in_p;

				out_size = (char_cnt - 2) / 4 * 3;

				for (;;) {
					out_p = buf;

					if (out_size <= out_reserved) {
						err = PHP_ICONV_ERR_TOO_BIG;
						goto out;
					}

					out_left = out_size - out_reserved;

					if (icv(cd, &in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						switch (errno) {
							case EINVAL:
								err = PHP_ICONV_ERR_ILLEGAL_CHAR;
								goto out;

							case EILSEQ:
								err = PHP_ICONV_ERR_ILLEGAL_SEQ;
								goto out;

							case E2BIG:
								if (prev_in_left == in_left) {
									err = PHP_ICONV_ERR_TOO_BIG;
									goto out;
								}
								break;

							default:
								err = PHP_ICONV_ERR_UNKNOWN;
								goto out;
						}
#else
						if (prev_in_left == in_left) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					}

					out_left += out_reserved;

					if (icv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						if (errno != E2BIG) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#else
						if (out_left != 0) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					} else {
						break;
					}

					if (icv(cd, NULL, NULL, NULL, NULL) == (size_t)-1) {
						err = PHP_ICONV_ERR_UNKNOWN;
						goto out;
					}

					out_reserved += 4;
					in_left = ini_in_left;
					in_p = ini_in_p;
				}

				prev_in_left = in_left;

				encoded = php_base64_encode(buf, (int)(out_size - out_left), &encoded_len);
				if (char_cnt < encoded_len) {
					/* something went wrong! */
					err = PHP_ICONV_ERR_UNKNOWN;
					goto out;
				}

				smart_str_appendl(pretval, encoded, encoded_len);
				char_cnt -= encoded_len;
				smart_str_appendl(pretval, "?=", sizeof("?=") - 1);
				char_cnt -= 2;

				efree(encoded);
				encoded = NULL;
			} break; /* case PHP_ICONV_ENC_SCHEME_BASE64: */

			case PHP_ICONV_ENC_SCHEME_QPRINT: {
				smart_str_appendc(pretval, 'Q');
				char_cnt--;
				smart_str_appendc(pretval, '?');
				char_cnt--;

				prev_in_left = in_left;

				while (in_left > 0) {
					out_p = buf;
					out_left = out_size = 1;

					if (icv(cd, &in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						switch (errno) {
							case EINVAL:
								err = PHP_ICONV_ERR_ILLEGAL_CHAR;
								goto out;

							case EILSEQ:
								err = PHP_ICONV_ERR_ILLEGAL_SEQ;
								goto out;

							case E2BIG:
								break;
	
							default:
								err = PHP_ICONV_ERR_UNKNOWN;
								goto out;
						}
#else
						if (prev_in_left == in_left) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					}

					if (out_size > out_left) {
						if ((buf[0] >= 33 && buf[0] <= 60) ||
							(buf[0] >= 62 && buf[0] <= 126)) {

							if (char_cnt >= 1 + 2) {
								smart_str_appendc(pretval, buf[0]);
								char_cnt--;
							} else {
								in_p -= (prev_in_left - in_left);
								in_left = prev_in_left;

								break;
							}
						} else {
							if (char_cnt >= 3 + 2) { 
								static char qp_digits[] = "0123456789ABCDEF";
								smart_str_appendc(pretval, '=');
								smart_str_appendc(pretval, qp_digits[(buf[0] >> 4) & 0x0f]);
								smart_str_appendc(pretval, qp_digits[(buf[0] & 0x0f)]);
								char_cnt -= 3;
							} else {
								in_p -= (prev_in_left - in_left);
								in_left = prev_in_left;

								break;
							}
						}
					}
					prev_in_left = in_left;
				}

				smart_str_appendl(pretval, "?=", sizeof("?=") - 1);
				char_cnt -= 2;

				if (icv(cd, NULL, NULL, NULL, NULL) == (size_t)-1) {
					err = PHP_ICONV_ERR_UNKNOWN;
					goto out;
				}

			} break; /* case PHP_ICONV_ENC_SCHEME_BASE64: */
		}
	} while (in_left > 0);

	smart_str_0(pretval);

out:
	if (cd != (iconv_t)(-1)) {
		icv_close(cd);
	}
	if (cd_pl != (iconv_t)(-1)) {
		icv_close(cd_pl);
	}
	if (encoded != NULL) {
		efree(encoded);
	}	
	if (buf != NULL) {
		efree(buf);
	}
	return err;
}
/* }}} */

/* {{{ _php_iconv_mime_decode() */
static php_iconv_err_t _php_iconv_mime_decode(smart_str *pretval, const char *str, size_t str_nbytes, const char *enc)
{
	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd = (iconv_t)(-1), cd_pl = (iconv_t)(-1);

	const char *p1;
	size_t str_left;
	unsigned int scan_stat = 0;
	const char *csname = NULL;
	size_t csname_len; 
	const char *encoded_text = NULL;
	size_t encoded_text_len;

	php_iconv_enc_scheme_t enc_scheme;

	cd_pl = icv_open(enc, "ASCII");

	if (cd_pl == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			err = PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			err = PHP_ICONV_ERR_CONVERTER;
		}
#else
		err = PHP_ICONV_ERR_UNKNOWN;
#endif
		goto out;
	}

	p1 = str;
	for (str_left = str_nbytes; str_left > 0; str_left--, p1++) {

		switch (scan_stat) {
			case 0:
				if (*p1 == '\r') {
					scan_stat = 7;
				} else if (*p1 == '\n') {
					scan_stat = 8;	
				} else if (*p1 == '=') {
					scan_stat = 1;
				} else {
					_php_iconv_appendc(pretval, *p1, cd_pl);
				}
				break;

			case 1:
				if (*p1 != '?') {
					err = PHP_ICONV_ERR_MALFORMED;
					goto out;
				}
				csname = p1 + 1;
				scan_stat = 2;
				break;
			
			case 2: /* charset name */
				if (*p1 == '?') {
					char tmpbuf[80];

					if (csname == NULL) {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}

					csname_len = (size_t)(p1 - csname);

					if (csname_len > sizeof(tmpbuf) - 1) {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}

					memcpy(tmpbuf, csname, csname_len);
					tmpbuf[csname_len] = '\0';

					if (cd != (iconv_t)(-1)) {
						icv_close(cd);
					}

					cd = icv_open(enc, tmpbuf);

					if (cd == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
						if (errno == EINVAL) {
							err = PHP_ICONV_ERR_WRONG_CHARSET;
						} else {
							err = PHP_ICONV_ERR_CONVERTER;
						}
#else
						err = PHP_ICONV_ERR_UNKNOWN;
#endif
						goto out;
					}

					scan_stat = 3;
				}
				break;

			case 3:
				switch (*p1) {
					case 'B':
						enc_scheme = PHP_ICONV_ENC_SCHEME_BASE64;
						break;

					case 'Q':
						enc_scheme = PHP_ICONV_ENC_SCHEME_QPRINT;
						break;

					default:
						err = PHP_ICONV_ERR_MALFORMED;							
						goto out;
				}
				scan_stat = 4;
				break;
		
			case 4:
				if (*p1 != '?') {
					err = PHP_ICONV_ERR_MALFORMED;
					goto out;
				}
				encoded_text = p1 + 1;
				scan_stat = 5;
				break;

			case 5:
				if (*p1 == '?') {
					char *decoded_text;
					size_t decoded_text_len;

					if (encoded_text == NULL) {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}

					encoded_text_len = (size_t)(p1 - encoded_text);
					switch (enc_scheme) {
						case PHP_ICONV_ENC_SCHEME_BASE64:
							decoded_text = (char *)php_base64_decode((unsigned char*)encoded_text, (int)encoded_text_len, &decoded_text_len);
							break;

						case PHP_ICONV_ENC_SCHEME_QPRINT:
							decoded_text = (char *)php_quot_print_decode((unsigned char*)encoded_text, (int)encoded_text_len, &decoded_text_len);
							break;
					}

					if (decoded_text == NULL) {
						err = PHP_ICONV_ERR_UNKNOWN;
						goto out;
					}

					err = _php_iconv_appendl(pretval, decoded_text, decoded_text_len, cd);
					efree(decoded_text);
					if (err != PHP_ICONV_ERR_SUCCESS) {
						goto out;
					}
					scan_stat = 6;
				}
				break;

			case 6:
				if (*p1 != '=') {
					err = PHP_ICONV_ERR_MALFORMED;
					goto out;
				}
				scan_stat = 0;
				break;

			case 7:
				if (*p1 == '\n') {
					scan_stat = 8;
				} else {
					/* bare CR */
					_php_iconv_appendc(pretval, '\r', cd_pl);
					_php_iconv_appendc(pretval, *p1, cd_pl);
				}
				break;

			case 8:
				if (*p1 != ' ' && *p1 != '\t') {
					err = PHP_ICONV_ERR_MALFORMED;
					goto out;
				}
				scan_stat = 9;
				break;

			case 9:
				if (*p1 == '=') {
					scan_stat = 1;
				} else if (*p1 != ' ' && *p1 != '\t') {
					err = PHP_ICONV_ERR_MALFORMED;
					goto out;
				}
				break;
		}
	}

	if (cd != (iconv_t)(-1)) {
		if ((err = _php_iconv_appendl(pretval, NULL, 0, cd)) != PHP_ICONV_ERR_SUCCESS) {
			goto out;
		}
	}
	smart_str_0(pretval);
out:
	if (cd != (iconv_t)(-1)) {
		icv_close(cd);
	}
	if (cd_pl != (iconv_t)(-1)) {
		icv_close(cd_pl);
	}
	return err;
}
/* }}} */

/* {{{ php_iconv_show_error() */
static void _php_iconv_show_error(php_iconv_err_t err, const char *out_charset, const char *in_charset TSRMLS_DC)
{
	switch (err) {
		case PHP_ICONV_ERR_SUCCESS:
			break;

		case PHP_ICONV_ERR_CONVERTER:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Cannot open converter");
			break;

		case PHP_ICONV_ERR_WRONG_CHARSET:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong charset, cannot convert from `%s' to `%s'",
			          in_charset, out_charset);
			break;

		case PHP_ICONV_ERR_ILLEGAL_CHAR:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected incomplete character in input string");
			break;

		case PHP_ICONV_ERR_ILLEGAL_SEQ:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected illegal character in input string");
			break;

		case PHP_ICONV_ERR_TOO_BIG:
			/* should not happen */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Run out buffer");
			break;

		default:
			/* other error */
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown error (%d)", errno);
			break;
	}
}
/* }}} */

/* {{{ proto int iconv_strlen(string str [, string charset])
   Returns the character count of str */
PHP_FUNCTION(iconv_strlen)
{
	char *charset;
	long charset_len;
	char *str;
	long str_len; 

	php_iconv_err_t err;

	unsigned int retval;

	charset = ICONVG(internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
		&str, &str_len, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	err = _php_iconv_strlen(&retval, str, str_len, charset); 
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);
	if (err == PHP_ICONV_ERR_SUCCESS) {
		RETVAL_LONG(retval);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv_substr(string str, int offset, [int length, string charset])
   Returns specified part of a string */
PHP_FUNCTION(iconv_substr)
{
	char *charset;
	long charset_len;
	char *str;
	long str_len; 
	long offset, length;
	zval *len_z = NULL;

	php_iconv_err_t err;

	smart_str retval = {0};

	charset = ICONVG(internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|zs",
		&str, &str_len, &offset, &len_z,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (len_z == NULL) {
		length = str_len; 
	} else {
		convert_to_long_ex(&len_z);
		length = Z_LVAL_P(len_z);
	}

	err = _php_iconv_substr(&retval, str, str_len, offset, length, charset); 
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS && str != NULL) {
		if (retval.c != NULL) {
			RETVAL_STRINGL(retval.c, retval.len, 0);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv_strpos(string haystack, string needle, int offset [, string charset])
   Finds position of first occurrence of needle within part of haystack beginning with offset */
PHP_FUNCTION(iconv_strpos)
{
	char *charset;
	long charset_len;
	char *haystk;
	long haystk_len; 
	char *ndl;
	long ndl_len;
	long offset;

	php_iconv_err_t err;

	unsigned int retval;

	offset = 0;
	charset = ICONVG(internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ls",
		&haystk, &haystk_len, &ndl, &ndl_len,
		&offset, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (offset < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string.");
		RETURN_FALSE;
	}

	err = _php_iconv_strpos(&retval, haystk, haystk_len, ndl, ndl_len,
	                        offset, charset); 
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS && retval != (unsigned int)-1) {
		RETVAL_LONG((long)retval);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv_strrpos(string haystack, string needle [, string charset])
   Finds position of last occurrence of needle within part of haystack beginning with offset */
PHP_FUNCTION(iconv_strrpos)
{
	char *charset;
	long charset_len;
	char *haystk;
	long haystk_len; 
	char *ndl;
	long ndl_len;

	php_iconv_err_t err;

	unsigned int retval;

	charset = ICONVG(internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s",
		&haystk, &haystk_len, &ndl, &ndl_len,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	err = _php_iconv_strpos(&retval, haystk, haystk_len, ndl, ndl_len,
	                        -1, charset); 
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS && retval != (unsigned int)-1) {
		RETVAL_LONG((long)retval);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv_mime_encode(string field_name, string field_value, string scheme, string out_charset [, string in_charset, int line_len, string lfchars])
   Composes a mime header field with field_name and field_value in a specified scheme */
PHP_FUNCTION(iconv_mime_encode)
{
	char *field_name;
	long field_name_len;
	char *field_value;
	long field_value_len;
	char *scheme;
	long scheme_len;	
	char *in_charset;
	long in_charset_len;
	char *out_charset;
	long out_charset_len;
	long line_len = 76;
	char *lfchars = "\r\n";
	long lfchars_len = sizeof("\r\n")-1; 
	
	php_iconv_enc_scheme_t scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;

	smart_str retval = {0};

	php_iconv_err_t err;

	in_charset = ICONVG(internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssss|sls",
		&field_name, &field_name_len, &field_value, &field_value_len,
		&scheme, &scheme_len, &out_charset, &out_charset_len,
		&in_charset, &in_charset_len, &line_len, &lfchars, &lfchars_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (scheme_len > 0) {
		switch (scheme[0]) {
			case 'B': case 'b':
				scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;
				break;

			case 'Q': case 'q':
				scheme_id = PHP_ICONV_ENC_SCHEME_QPRINT;
				break;
		}
	}

	err = _php_iconv_mime_encode(&retval, field_name, field_name_len,
		field_value, field_value_len, line_len, lfchars, scheme_id,
		out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.c != NULL) {
			RETVAL_STRINGL(retval.c, retval.len, 0);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv_mime_decode(string encoded_string [, string charset])
   Decodes a mime header field */
PHP_FUNCTION(iconv_mime_decode)
{
	char *encoded_str;
	long encoded_str_len;
	char *charset;
	long charset_len;
	
	smart_str retval = {0};

	php_iconv_err_t err;

	charset = ICONVG(internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
		&encoded_str, &encoded_str_len, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	err = _php_iconv_mime_decode(&retval, encoded_str, encoded_str_len, charset);
	_php_iconv_show_error(err, charset, "???" TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.c != NULL) {
			RETVAL_STRINGL(retval.c, retval.len, 0);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv(string in_charset, string out_charset, string str)
   Returns str converted to the out_charset character set */
PHP_NAMED_FUNCTION(php_if_iconv)
{
	char *in_charset, *out_charset, *in_buffer, *out_buffer;
	size_t out_len;
	int in_charset_len, out_charset_len, in_buffer_len;
	php_iconv_err_t err;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
		&in_charset, &in_charset_len, &out_charset, &out_charset_len, &in_buffer, &in_buffer_len) == FAILURE)
		return;

	err = php_iconv_string(in_buffer, (size_t)in_buffer_len,
		&out_buffer, &out_len, out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset TSRMLS_CC); 
	if (out_buffer != NULL) {
		RETVAL_STRINGL(out_buffer, out_len, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string ob_iconv_handler(string contents, int status)
   Returns str in output buffer converted to the iconv.output_encoding character set */
PHP_FUNCTION(ob_iconv_handler)
{
	char *out_buffer, *content_type, *mimetype = NULL, *s;
	zval *zv_string;
	unsigned int out_len;
	int status, mimetype_alloced  = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &zv_string, &status) == FAILURE)
		return;

	convert_to_string_ex(&zv_string);

	if (SG(sapi_headers).mimetype && 
		strncasecmp(SG(sapi_headers).mimetype, "text/", 5) == 0) {
		if ((s = strchr(SG(sapi_headers).mimetype,';')) == NULL){
			mimetype = SG(sapi_headers).mimetype;
		} else {
			mimetype = estrndup(SG(sapi_headers).mimetype, s-SG(sapi_headers).mimetype);
			mimetype_alloced = 1;
		}
	} else if (SG(sapi_headers).send_default_content_type) {
		mimetype =(SG(default_mimetype) ? SG(default_mimetype) : SAPI_DEFAULT_MIMETYPE);
	}
	if (mimetype != NULL) {
		php_iconv_err_t err = php_iconv_string(Z_STRVAL_P(zv_string),
				Z_STRLEN_P(zv_string), &out_buffer, &out_len,
				ICONVG(output_encoding), ICONVG(internal_encoding));
		_php_iconv_show_error(err, ICONVG(output_encoding), ICONVG(internal_encoding) TSRMLS_CC);
		if (out_buffer != NULL) {
			spprintf(&content_type, 0, "Content-Type:%s; charset=%s", mimetype, ICONVG(output_encoding));
			if (content_type && sapi_add_header(content_type, strlen(content_type), 0) != FAILURE) {
				SG(sapi_headers).send_default_content_type = 0;
			}
			RETURN_STRINGL(out_buffer, out_len, 0);
		}
		if (mimetype_alloced) {
			efree(mimetype);
		}
	}

	zval_dtor(return_value);
	*return_value = *zv_string;
	zval_copy_ctor(return_value);
}
/* }}} */

/* {{{ proto bool iconv_set_encoding(string type, string charset)
   Sets internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_set_encoding)
{
	char *type, *charset;
	int type_len, charset_len, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &type, &type_len, &charset, &charset_len) == FAILURE)
		return;

	if(!strcasecmp("input_encoding", type)) {
		retval = zend_alter_ini_entry("iconv.input_encoding", sizeof("iconv.input_encoding"), charset, charset_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	} else if(!strcasecmp("output_encoding", type)) {
		retval = zend_alter_ini_entry("iconv.output_encoding", sizeof("iconv.output_encoding"), charset, charset_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	} else if(!strcasecmp("internal_encoding", type)) {
		retval = zend_alter_ini_entry("iconv.internal_encoding", sizeof("iconv.internal_encoding"), charset, charset_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	} else {
		RETURN_FALSE;
	}

	if (retval == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array iconv_get_encoding([string type])
   Get internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_get_encoding)
{
	char *type = "all";
	int type_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &type, &type_len) == FAILURE)
		return;

	if (!strcasecmp("all", type)) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_string(return_value, "input_encoding",    ICONVG(input_encoding), 1);
		add_assoc_string(return_value, "output_encoding",   ICONVG(output_encoding), 1);
		add_assoc_string(return_value, "internal_encoding", ICONVG(internal_encoding), 1);
	} else if (!strcasecmp("input_encoding", type)) {
		RETVAL_STRING(ICONVG(input_encoding), 1);
	} else if (!strcasecmp("output_encoding", type)) {
		RETVAL_STRING(ICONVG(output_encoding), 1);
	} else if (!strcasecmp("internal_encoding", type)) {
		RETVAL_STRING(ICONVG(internal_encoding), 1);
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
