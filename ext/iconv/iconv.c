/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rui Hirokawa <rui_hirokawa@ybb.ne.jp>                       |
   |          Stig Bakken <ssb@php.net>                                   |
   |          Moriyoshi Koizumi <moriyoshi@php.net>                       |
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

#ifdef PHP_ICONV_H_PATH
#include PHP_ICONV_H_PATH
#else
#include <iconv.h>
#endif

#ifdef HAVE_GLIBC_ICONV
#include <gnu/libc-version.h>
#endif

#ifdef HAVE_LIBICONV
#undef iconv
#endif

#include "ext/standard/php_smart_str.h"
#include "ext/standard/base64.h"
#include "ext/standard/quot_print.h"

#define _php_iconv_memequal(a, b, c) \
  ((c) == sizeof(unsigned long) ? *((unsigned long *)(a)) == *((unsigned long *)(b)) : ((c) == sizeof(unsigned int) ? *((unsigned int *)(a)) == *((unsigned int *)(b)) : memcmp(a, b, c) == 0))

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_strlen, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_substr, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_strpos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_strrpos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_mime_encode, 0, 0, 2)
	ZEND_ARG_INFO(0, field_name)
	ZEND_ARG_INFO(0, field_value)
	ZEND_ARG_INFO(0, preference) /* ZEND_ARG_ARRAY_INFO(0, preference, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_mime_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, encoded_string)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_mime_decode_headers, 0, 0, 1)
	ZEND_ARG_INFO(0, headers)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_iconv, 0)
	ZEND_ARG_INFO(0, in_charset)
	ZEND_ARG_INFO(0, out_charset)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_iconv_set_encoding, 0)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_iconv_get_encoding, 0, 0, 0)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ iconv_functions[]
 */
const zend_function_entry iconv_functions[] = {
	PHP_RAW_NAMED_FE(iconv,php_if_iconv,				arginfo_iconv)
	PHP_FE(iconv_get_encoding,						arginfo_iconv_get_encoding)
	PHP_FE(iconv_set_encoding,						arginfo_iconv_set_encoding)
	PHP_FE(iconv_strlen,							arginfo_iconv_strlen)
	PHP_FE(iconv_substr,							arginfo_iconv_substr)
	PHP_FE(iconv_strpos,							arginfo_iconv_strpos)
	PHP_FE(iconv_strrpos,							arginfo_iconv_strrpos)
	PHP_FE(iconv_mime_encode,						arginfo_iconv_mime_encode)
	PHP_FE(iconv_mime_decode,						arginfo_iconv_mime_decode)
	PHP_FE(iconv_mime_decode_headers,				arginfo_iconv_mime_decode_headers)
	PHP_FE_END
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(iconv)
static PHP_GINIT_FUNCTION(iconv);

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
	PHP_MODULE_GLOBALS(iconv),
	PHP_GINIT(iconv),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ICONV
ZEND_GET_MODULE(iconv)
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(iconv)
{
	iconv_globals->input_encoding = NULL;
	iconv_globals->output_encoding = NULL;
	iconv_globals->internal_encoding = NULL;
}
/* }}} */

#if defined(HAVE_LIBICONV) && defined(ICONV_ALIASED_LIBICONV)
#define iconv libiconv
#endif

/* {{{ typedef enum php_iconv_enc_scheme_t */
typedef enum _php_iconv_enc_scheme_t {
	PHP_ICONV_ENC_SCHEME_BASE64,
	PHP_ICONV_ENC_SCHEME_QPRINT
} php_iconv_enc_scheme_t;
/* }}} */

#define PHP_ICONV_MIME_DECODE_STRICT            (1<<0)
#define PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR (1<<1)

/* {{{ prototypes */
static php_iconv_err_t _php_iconv_appendl(smart_str *d, const char *s, size_t l, iconv_t cd);
static php_iconv_err_t _php_iconv_appendc(smart_str *d, const char c, iconv_t cd);

static void _php_iconv_show_error(php_iconv_err_t err, const char *out_charset, const char *in_charset TSRMLS_DC);

static php_iconv_err_t _php_iconv_strlen(unsigned int *pretval, const char *str, size_t nbytes, const char *enc);

static php_iconv_err_t _php_iconv_substr(smart_str *pretval, const char *str, size_t nbytes, int offset, int len, const char *enc);

static php_iconv_err_t _php_iconv_strpos(unsigned int *pretval, const char *haystk, size_t haystk_nbytes, const char *ndl, size_t ndl_nbytes, int offset, const char *enc);

static php_iconv_err_t _php_iconv_mime_encode(smart_str *pretval, const char *fname, size_t fname_nbytes, const char *fval, size_t fval_nbytes, unsigned int max_line_len, const char *lfchars, php_iconv_enc_scheme_t enc_scheme, const char *out_charset, const char *enc);

static php_iconv_err_t _php_iconv_mime_decode(smart_str *pretval, const char *str, size_t str_nbytes, const char *enc, const char **next_pos, int mode);

static php_iconv_err_t php_iconv_stream_filter_register_factory(TSRMLS_D);
static php_iconv_err_t php_iconv_stream_filter_unregister_factory(TSRMLS_D);

static int php_iconv_output_conflict(const char *handler_name, size_t handler_name_len TSRMLS_DC);
static php_output_handler *php_iconv_output_handler_init(const char *name, size_t name_len, size_t chunk_size, int flags TSRMLS_DC);
static int php_iconv_output_handler(void **nothing, php_output_context *output_context);
/* }}} */

/* {{{ static globals */
static char _generic_superset_name[] = ICONV_UCS4_ENCODING;
#define GENERIC_SUPERSET_NAME _generic_superset_name
#define GENERIC_SUPERSET_NBYTES 4
/* }}} */

static PHP_INI_MH(OnUpdateStringIconvCharset)
{
	if(new_value_length >= ICONV_CSNMAXLEN) {
		return FAILURE;
	}
	OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
	return SUCCESS;
}

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("iconv.input_encoding",    ICONV_INPUT_ENCODING,    PHP_INI_ALL, OnUpdateStringIconvCharset, input_encoding,    zend_iconv_globals, iconv_globals)
	STD_PHP_INI_ENTRY("iconv.output_encoding",   ICONV_OUTPUT_ENCODING,   PHP_INI_ALL, OnUpdateStringIconvCharset, output_encoding,   zend_iconv_globals, iconv_globals)
	STD_PHP_INI_ENTRY("iconv.internal_encoding", ICONV_INTERNAL_ENCODING, PHP_INI_ALL, OnUpdateStringIconvCharset, internal_encoding, zend_iconv_globals, iconv_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(miconv)
{
	char *version = "unknown";

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
#elif defined(NETWARE)
	version = "OS built-in";
#endif

#ifdef PHP_ICONV_IMPL
	REGISTER_STRING_CONSTANT("ICONV_IMPL", PHP_ICONV_IMPL, CONST_CS | CONST_PERSISTENT);
#elif HAVE_LIBICONV
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "libiconv", CONST_CS | CONST_PERSISTENT);
#elif defined(NETWARE)
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "Novell", CONST_CS | CONST_PERSISTENT);
#else
	REGISTER_STRING_CONSTANT("ICONV_IMPL", "unknown", CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_STRING_CONSTANT("ICONV_VERSION", version, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ICONV_MIME_DECODE_STRICT", PHP_ICONV_MIME_DECODE_STRICT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ICONV_MIME_DECODE_CONTINUE_ON_ERROR", PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR, CONST_CS | CONST_PERSISTENT);

	if (php_iconv_stream_filter_register_factory(TSRMLS_C) != PHP_ICONV_ERR_SUCCESS) {
		return FAILURE;
	}

	php_output_handler_alias_register(ZEND_STRL("ob_iconv_handler"), php_iconv_output_handler_init TSRMLS_CC);
	php_output_handler_conflict_register(ZEND_STRL("ob_iconv_handler"), php_iconv_output_conflict TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(miconv)
{
	php_iconv_stream_filter_unregister_factory(TSRMLS_C);
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

static int php_iconv_output_conflict(const char *handler_name, size_t handler_name_len TSRMLS_DC)
{
	if (php_output_get_level(TSRMLS_C)) {
		if (php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("ob_iconv_handler") TSRMLS_CC)
		||	php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("mb_output_handler") TSRMLS_CC)) {
			return FAILURE;
		}
	}
	return SUCCESS;
}

static php_output_handler *php_iconv_output_handler_init(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags TSRMLS_DC)
{
	return php_output_handler_create_internal(handler_name, handler_name_len, php_iconv_output_handler, chunk_size, flags TSRMLS_CC);
}

static int php_iconv_output_handler(void **nothing, php_output_context *output_context)
{
	char *s, *content_type, *mimetype = NULL;
	int output_status, mimetype_len = 0;
	PHP_OUTPUT_TSRMLS(output_context);

	if (output_context->op & PHP_OUTPUT_HANDLER_START) {
		output_status = php_output_get_status(TSRMLS_C);
		if (output_status & PHP_OUTPUT_SENT) {
			return FAILURE;
		}

		if (SG(sapi_headers).mimetype && !strncasecmp(SG(sapi_headers).mimetype, "text/", 5)) {
			if ((s = strchr(SG(sapi_headers).mimetype,';')) == NULL){
				mimetype = SG(sapi_headers).mimetype;
			} else {
				mimetype = SG(sapi_headers).mimetype;
				mimetype_len = s - SG(sapi_headers).mimetype;
			}
		} else if (SG(sapi_headers).send_default_content_type) {
			mimetype = SG(default_mimetype) ? SG(default_mimetype) : SAPI_DEFAULT_MIMETYPE;
		}

		if (mimetype != NULL && !(output_context->op & PHP_OUTPUT_HANDLER_CLEAN)) {
			int len;
			char *p = strstr(ICONVG(output_encoding), "//");

			if (p) {
				len = spprintf(&content_type, 0, "Content-Type:%.*s; charset=%.*s", mimetype_len ? mimetype_len : (int) strlen(mimetype), mimetype, (int)(p - ICONVG(output_encoding)), ICONVG(output_encoding));
			} else {
				len = spprintf(&content_type, 0, "Content-Type:%.*s; charset=%s", mimetype_len ? mimetype_len : (int) strlen(mimetype), mimetype, ICONVG(output_encoding));
			}
			if (content_type && SUCCESS == sapi_add_header(content_type, len, 0)) {
				SG(sapi_headers).send_default_content_type = 0;
				php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE, NULL TSRMLS_CC);
			}
		}
	}

	if (output_context->in.used) {
		output_context->out.free = 1;
		_php_iconv_show_error(php_iconv_string(output_context->in.data, output_context->in.used, &output_context->out.data, &output_context->out.used, ICONVG(output_encoding), ICONVG(internal_encoding)), ICONVG(output_encoding), ICONVG(internal_encoding) TSRMLS_CC);
	}

	return SUCCESS;
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

			if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
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

			if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)0) {
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
PHP_ICONV_API php_iconv_err_t php_iconv_string(const char *in_p, size_t in_len,
							char **out, size_t *out_len,
							const char *out_charset, const char *in_charset)
{
#if !ICONV_SUPPORTS_ERRNO
	size_t in_size, out_size, out_left;
	char *out_buffer, *out_p;
	iconv_t cd;
	size_t result;

	*out = NULL;
	*out_len = 0;

	/*
	  This is not the right way to get output size...
	  This is not space efficient for large text.
	  This is also problem for encoding like UTF-7/UTF-8/ISO-2022 which
	  a single char can be more than 4 bytes.
	  I added 15 extra bytes for safety. <yohgaki@php.net>
	*/
	out_size = in_len * sizeof(int) + 15;
	out_left = out_size;

	in_size = in_len;

	cd = iconv_open(out_charset, in_charset);

	if (cd == (iconv_t)(-1)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}

	out_buffer = (char *) emalloc(out_size + 1);
	out_p = out_buffer;

#ifdef NETWARE
	result = iconv(cd, (char **) &in_p, &in_size, (char **)
#else
	result = iconv(cd, (const char **) &in_p, &in_size, (char **)
#endif
				&out_p, &out_left);

	if (result == (size_t)(-1)) {
		efree(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	if (out_left < 8) {
		size_t pos = out_p - out_buffer;
		out_buffer = (char *) safe_erealloc(out_buffer, out_size, 1, 8);
		out_p = out_buffer+pos;
		out_size += 7;
		out_left += 7;
	}

	/* flush the shift-out sequences */
	result = iconv(cd, NULL, NULL, &out_p, &out_left);

	if (result == (size_t)(-1)) {
		efree(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	*out_len = out_size - out_left;
	out_buffer[*out_len] = '\0';
	*out = out_buffer;

	iconv_close(cd);

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

	cd = iconv_open(out_charset, in_charset);

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
		result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
		out_size = bsz - out_left;
		if (result == (size_t)(-1)) {
			if (errno == E2BIG && in_left > 0) {
				/* converted string is longer than out buffer */
				bsz += in_len;

				tmp_buf = (char*) erealloc(out_buf, bsz+1);
				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
				continue;
			}
		}
		break;
	}

	if (result != (size_t)(-1)) {
		/* flush the shift-out sequences */
		for (;;) {
		   	result = iconv(cd, NULL, NULL, (char **) &out_p, &out_left);
			out_size = bsz - out_left;

			if (result != (size_t)(-1)) {
				break;
			}

			if (errno == E2BIG) {
				bsz += 16;
				tmp_buf = (char *) erealloc(out_buf, bsz);

				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
			} else {
				break;
			}
		}
	}

	iconv_close(cd);

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

	cd = iconv_open(GENERIC_SUPERSET_NAME, enc);

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

	errno = out_left = 0;

	for (in_p = str, in_left = nbytes, cnt = 0; in_left > 0; cnt+=2) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
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
		case 0:
			*pretval = cnt;
			break;

		default:
			err = PHP_ICONV_ERR_UNKNOWN;
			break;
	}
#else
	*pretval = cnt;
#endif

	iconv_close(cd);

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
	int total_len;

	err = _php_iconv_strlen(&total_len, str, nbytes, enc);
	if (err != PHP_ICONV_ERR_SUCCESS) {
		return err;
	}

	if (len < 0) {
		if ((len += (total_len - offset)) < 0) {
			return PHP_ICONV_ERR_SUCCESS;
		}
	}

	if (offset < 0) {
		if ((offset += total_len) < 0) {
			return PHP_ICONV_ERR_SUCCESS;
		}
	}

	if(len > total_len) {
		len = total_len;
	}


	if (offset >= total_len) {
		return PHP_ICONV_ERR_SUCCESS;
	}

	if ((offset + len) > total_len ) {
		/* trying to compute the length */
		len = total_len - offset;
	}

	if (len == 0) {
		smart_str_appendl(pretval, "", 0);
		smart_str_0(pretval);
		return PHP_ICONV_ERR_SUCCESS;
	}

	cd1 = iconv_open(GENERIC_SUPERSET_NAME, enc);

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

	cd2 = (iconv_t)NULL;
	errno = 0;

	for (in_p = str, in_left = nbytes, cnt = 0; in_left > 0 && len > 0; ++cnt) {
		size_t prev_in_left;
		out_p = buf;
		out_left = sizeof(buf);

		prev_in_left = in_left;

		if (iconv(cd1, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
			if (prev_in_left == in_left) {
				break;
			}
		}

		if (cnt >= (unsigned int)offset) {
			if (cd2 == (iconv_t)NULL) {
				cd2 = iconv_open(enc, GENERIC_SUPERSET_NAME);

				if (cd2 == (iconv_t)(-1)) {
					cd2 = (iconv_t)NULL;
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

			if (_php_iconv_appendl(pretval, buf, sizeof(buf), cd2) != PHP_ICONV_ERR_SUCCESS) {
				break;
			}
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
	}
#endif
	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (cd2 != (iconv_t)NULL) {
			_php_iconv_appendl(pretval, NULL, 0, cd2);
		}
		smart_str_0(pretval);
	}

	if (cd1 != (iconv_t)NULL) {
		iconv_close(cd1);
	}

	if (cd2 != (iconv_t)NULL) {
		iconv_close(cd2);
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

	cd = iconv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd == (iconv_t)(-1)) {
		if (ndl_buf != NULL) {
			efree(ndl_buf);
		}
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

		if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
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
			if (cnt >= (unsigned int)offset) {
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

					if (_php_iconv_memequal(buf, &ndl_buf[i], sizeof(buf))) {
						match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
						i += GENERIC_SUPERSET_NBYTES;
						ndl_buf_p = &ndl_buf[i];
						ndl_buf_left = ndl_buf_len - i;
					} else {
						match_ofs = (unsigned int)-1;
						ndl_buf_p = ndl_buf;
						ndl_buf_left = ndl_buf_len;
					}
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

				if (_php_iconv_memequal(buf, &ndl_buf[i], sizeof(buf))) {
					match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
					i += GENERIC_SUPERSET_NBYTES;
					ndl_buf_p = &ndl_buf[i];
					ndl_buf_left = ndl_buf_len - i;
				} else {
					match_ofs = (unsigned int)-1;
					ndl_buf_p = ndl_buf;
					ndl_buf_left = ndl_buf_len;
				}
			}
		}
	}

	if (ndl_buf) {
		efree(ndl_buf);
	}

	iconv_close(cd);

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
	static int qp_table[256] = {
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x00 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x10 */
		3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x20 */
		1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 3, 1, 3, /* 0x30 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x40 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, /* 0x50 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x60 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, /* 0x70 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x80 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0x90 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xA0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xB0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xC0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xD0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xE0 */
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3  /* 0xF0 */
	};

	out_charset_len = strlen(out_charset);
	lfchars_len = strlen(lfchars);

	if ((fname_nbytes + 2) >= max_line_len
		|| (out_charset_len + 12) >= max_line_len) {
		/* field name is too long */
		err = PHP_ICONV_ERR_TOO_BIG;
		goto out;
	}

	cd_pl = iconv_open(ICONV_ASCII_ENCODING, enc);
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

	cd = iconv_open(out_charset, enc);
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

	buf = safe_emalloc(1, max_line_len, 5);

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
				int dummy;

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

					if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
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

					if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)-1) {
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

					if (iconv(cd, NULL, NULL, NULL, NULL) == (size_t)-1) {
						err = PHP_ICONV_ERR_UNKNOWN;
						goto out;
					}

					out_reserved += 4;
					in_left = ini_in_left;
					in_p = ini_in_p;
				}

				prev_in_left = in_left;

				encoded = (char *) php_base64_encode((unsigned char *) buf, (int)(out_size - out_left), &dummy);
				encoded_len = (size_t)dummy;

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
				size_t ini_in_left;
				const char *ini_in_p;
				const unsigned char *p;
				size_t nbytes_required;

				smart_str_appendc(pretval, 'Q');
				char_cnt--;
				smart_str_appendc(pretval, '?');
				char_cnt--;

				prev_in_left = ini_in_left = in_left;
				ini_in_p = in_p;

				for (out_size = (char_cnt - 2) / 3; out_size > 0;) {
					size_t prev_out_left;

					nbytes_required = 0;

					out_p = buf;
					out_left = out_size;

					if (iconv(cd, (char **)&in_p, &in_left, (char **) &out_p, &out_left) == (size_t)-1) {
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
									err = PHP_ICONV_ERR_UNKNOWN;
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

					prev_out_left = out_left;
					if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
						if (errno != E2BIG) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#else
						if (out_left == prev_out_left) {
							err = PHP_ICONV_ERR_UNKNOWN;
							goto out;
						}
#endif
					}

					for (p = (unsigned char *)buf; p < (unsigned char *)out_p; p++) {
						nbytes_required += qp_table[*p];
					}

					if (nbytes_required <= char_cnt - 2) {
						break;
					}

					out_size -= ((nbytes_required - (char_cnt - 2)) + 1) / 3;
					in_left = ini_in_left;
					in_p = ini_in_p;
				}

				for (p = (unsigned char *)buf; p < (unsigned char *)out_p; p++) {
					if (qp_table[*p] == 1) {
						smart_str_appendc(pretval, *(char *)p);
						char_cnt--;
					} else {
						static char qp_digits[] = "0123456789ABCDEF";
						smart_str_appendc(pretval, '=');
						smart_str_appendc(pretval, qp_digits[(*p >> 4) & 0x0f]);
						smart_str_appendc(pretval, qp_digits[(*p & 0x0f)]);
						char_cnt -= 3;
					}
				}

				smart_str_appendl(pretval, "?=", sizeof("?=") - 1);
				char_cnt -= 2;

				if (iconv(cd, NULL, NULL, NULL, NULL) == (size_t)-1) {
					err = PHP_ICONV_ERR_UNKNOWN;
					goto out;
				}

			} break; /* case PHP_ICONV_ENC_SCHEME_QPRINT: */
		}
	} while (in_left > 0);

	smart_str_0(pretval);

out:
	if (cd != (iconv_t)(-1)) {
		iconv_close(cd);
	}
	if (cd_pl != (iconv_t)(-1)) {
		iconv_close(cd_pl);
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
static php_iconv_err_t _php_iconv_mime_decode(smart_str *pretval, const char *str, size_t str_nbytes, const char *enc, const char **next_pos, int mode)
{
	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd = (iconv_t)(-1), cd_pl = (iconv_t)(-1);

	const char *p1;
	size_t str_left;
	unsigned int scan_stat = 0;
	const char *csname = NULL;
	size_t csname_len;
	const char *encoded_text = NULL;
	size_t encoded_text_len = 0;
	const char *encoded_word = NULL;
	const char *spaces = NULL;

	php_iconv_enc_scheme_t enc_scheme = PHP_ICONV_ENC_SCHEME_BASE64;

	if (next_pos != NULL) {
		*next_pos = NULL;
	}

	cd_pl = iconv_open(enc, ICONV_ASCII_ENCODING);

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
		int eos = 0;

		switch (scan_stat) {
			case 0: /* expecting any character */
				switch (*p1) {
					case '\r': /* part of an EOL sequence? */
						scan_stat = 7;
						break;

					case '\n':
						scan_stat = 8;
						break;

					case '=': /* first letter of an encoded chunk */
						encoded_word = p1;
						scan_stat = 1;
						break;

					case ' ': case '\t': /* a chunk of whitespaces */
						spaces = p1;
						scan_stat = 11;
						break;

					default: /* first letter of a non-encoded word */
						_php_iconv_appendc(pretval, *p1, cd_pl);
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						}
						break;
				}
				break;

			case 1: /* expecting a delimiter */
				if (*p1 != '?') {
					err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
					if (err != PHP_ICONV_ERR_SUCCESS) {
						goto out;
					}
					encoded_word = NULL;
					if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
						scan_stat = 12;
					} else {
						scan_stat = 0;
					}
					break;
				}
				csname = p1 + 1;
				scan_stat = 2;
				break;

			case 2: /* expecting a charset name */
				switch (*p1) {
					case '?': /* normal delimiter: encoding scheme follows */
						scan_stat = 3;
						break;

					case '*': /* new style delimiter: locale id follows */
						scan_stat = 10;
						break;
				}
				if (scan_stat != 2) {
					char tmpbuf[80];

					if (csname == NULL) {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}

					csname_len = (size_t)(p1 - csname);

					if (csname_len > sizeof(tmpbuf) - 1) {
						if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
							err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
							if (err != PHP_ICONV_ERR_SUCCESS) {
								goto out;
							}
							encoded_word = NULL;
							if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
								scan_stat = 12;
							} else {
								scan_stat = 0;
							}
							break;
						} else {
							err = PHP_ICONV_ERR_MALFORMED;
							goto out;
						}
					}

					memcpy(tmpbuf, csname, csname_len);
					tmpbuf[csname_len] = '\0';

					if (cd != (iconv_t)(-1)) {
						iconv_close(cd);
					}

					cd = iconv_open(enc, tmpbuf);

					if (cd == (iconv_t)(-1)) {
						if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
							/* Bad character set, but the user wants us to
							 * press on. In this case, we'll just insert the
							 * undecoded encoded word, since there isn't really
							 * a more sensible behaviour available; the only
							 * other options are to swallow the encoded word
							 * entirely or decode it with an arbitrarily chosen
							 * single byte encoding, both of which seem to have
							 * a higher WTF factor than leaving it undecoded.
							 *
							 * Given this approach, we need to skip ahead to
							 * the end of the encoded word. */
							int qmarks = 2;
							while (qmarks > 0 && str_left > 1) {
								if (*(++p1) == '?') {
									--qmarks;
								}
								--str_left;
							}

							/* Look ahead to check for the terminating = that
							 * should be there as well; if it's there, we'll
							 * also include that. If it's not, there isn't much
							 * we can do at this point. */
							if (*(p1 + 1) == '=') {
								++p1;
								--str_left;
							}

							err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
							if (err != PHP_ICONV_ERR_SUCCESS) {
								goto out;
							}

							/* Let's go back and see if there are further
							 * encoded words or bare content, and hope they
							 * might actually have a valid character set. */
							scan_stat = 12;
							break;
						} else {
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
					}
				}
				break;

			case 3: /* expecting a encoding scheme specifier */
				switch (*p1) {
					case 'b':
					case 'B':
						enc_scheme = PHP_ICONV_ENC_SCHEME_BASE64;
						scan_stat = 4;
						break;

					case 'q':
					case 'Q':
						enc_scheme = PHP_ICONV_ENC_SCHEME_QPRINT;
						scan_stat = 4;
						break;

					default:
						if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
							err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
							if (err != PHP_ICONV_ERR_SUCCESS) {
								goto out;
							}
							encoded_word = NULL;
							if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
								scan_stat = 12;
							} else {
								scan_stat = 0;
							}
							break;
						} else {
							err = PHP_ICONV_ERR_MALFORMED;
							goto out;
						}
				}
				break;

			case 4: /* expecting a delimiter */
				if (*p1 != '?') {
					if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
						/* pass the entire chunk through the converter */
						err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
						if (err != PHP_ICONV_ERR_SUCCESS) {
							goto out;
						}
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						} else {
							scan_stat = 0;
						}
						break;
					} else {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}
				}
				encoded_text = p1 + 1;
				scan_stat = 5;
				break;

			case 5: /* expecting an encoded portion */
				if (*p1 == '?') {
					encoded_text_len = (size_t)(p1 - encoded_text);
					scan_stat = 6;
				}
				break;

			case 7: /* expecting a "\n" character */
				if (*p1 == '\n') {
					scan_stat = 8;
				} else {
					/* bare CR */
					_php_iconv_appendc(pretval, '\r', cd_pl);
					_php_iconv_appendc(pretval, *p1, cd_pl);
					scan_stat = 0;
				}
				break;

			case 8: /* checking whether the following line is part of a
					   folded header */
				if (*p1 != ' ' && *p1 != '\t') {
					--p1;
					str_left = 1; /* quit_loop */
					break;
				}
				if (encoded_word == NULL) {
					_php_iconv_appendc(pretval, ' ', cd_pl);
				}
				spaces = NULL;
				scan_stat = 11;
				break;

			case 6: /* expecting a End-Of-Chunk character "=" */
				if (*p1 != '=') {
					if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
						/* pass the entire chunk through the converter */
						err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
						if (err != PHP_ICONV_ERR_SUCCESS) {
							goto out;
						}
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						} else {
							scan_stat = 0;
						}
						break;
					} else {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}
				}
				scan_stat = 9;
				if (str_left == 1) {
					eos = 1;
				} else {
					break;
				}

			case 9: /* choice point, seeing what to do next.*/
				switch (*p1) {
					default:
						/* Handle non-RFC-compliant formats
						 *
						 * RFC2047 requires the character that comes right
						 * after an encoded word (chunk) to be a whitespace,
						 * while there are lots of broken implementations that
						 * generate such malformed headers that don't fulfill
						 * that requirement.
						 */
						if (!eos) {
							if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
								/* pass the entire chunk through the converter */
								err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
								if (err != PHP_ICONV_ERR_SUCCESS) {
									goto out;
								}
								scan_stat = 12;
								break;
							}
						}
						/* break is omitted intentionally */

					case '\r': case '\n': case ' ': case '\t': {
						char *decoded_text;
						size_t decoded_text_len;
						int dummy;

						switch (enc_scheme) {
							case PHP_ICONV_ENC_SCHEME_BASE64:
								decoded_text = (char *)php_base64_decode((unsigned char*)encoded_text, (int)encoded_text_len, &dummy);
								decoded_text_len = (size_t)dummy;
								break;

							case PHP_ICONV_ENC_SCHEME_QPRINT:
								decoded_text = (char *)php_quot_print_decode((unsigned char*)encoded_text, (int)encoded_text_len, &decoded_text_len, 1);
								break;
							default:
								decoded_text = NULL;
								break;
						}

						if (decoded_text == NULL) {
							if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
								/* pass the entire chunk through the converter */
								err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
								if (err != PHP_ICONV_ERR_SUCCESS) {
									goto out;
								}
								encoded_word = NULL;
								if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
									scan_stat = 12;
								} else {
									scan_stat = 0;
								}
								break;
							} else {
								err = PHP_ICONV_ERR_UNKNOWN;
								goto out;
							}
						}

						err = _php_iconv_appendl(pretval, decoded_text, decoded_text_len, cd);
						efree(decoded_text);

						if (err != PHP_ICONV_ERR_SUCCESS) {
							if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
								/* pass the entire chunk through the converter */
								err = _php_iconv_appendl(pretval, encoded_word, (size_t)(p1 - encoded_word), cd_pl);
								encoded_word = NULL;
								if (err != PHP_ICONV_ERR_SUCCESS) {
									break;
								}
							} else {
								goto out;
							}
						}

						if (eos) { /* reached end-of-string. done. */
							scan_stat = 0;
							break;
						}

						switch (*p1) {
							case '\r': /* part of an EOL sequence? */
								scan_stat = 7;
								break;

							case '\n':
								scan_stat = 8;
								break;

							case '=': /* first letter of an encoded chunk */
								scan_stat = 1;
								break;

							case ' ': case '\t': /* medial whitespaces */
								spaces = p1;
								scan_stat = 11;
								break;

							default: /* first letter of a non-encoded word */
								_php_iconv_appendc(pretval, *p1, cd_pl);
								scan_stat = 12;
								break;
						}
					} break;
				}
				break;

			case 10: /* expects a language specifier. dismiss it for now */
				if (*p1 == '?') {
					scan_stat = 3;
				}
				break;

			case 11: /* expecting a chunk of whitespaces */
				switch (*p1) {
					case '\r': /* part of an EOL sequence? */
						scan_stat = 7;
						break;

					case '\n':
						scan_stat = 8;
						break;

					case '=': /* first letter of an encoded chunk */
						if (spaces != NULL && encoded_word == NULL) {
							_php_iconv_appendl(pretval, spaces, (size_t)(p1 - spaces), cd_pl);
							spaces = NULL;
						}
						encoded_word = p1;
						scan_stat = 1;
						break;

					case ' ': case '\t':
						break;

					default: /* first letter of a non-encoded word */
						if (spaces != NULL) {
							_php_iconv_appendl(pretval, spaces, (size_t)(p1 - spaces), cd_pl);
							spaces = NULL;
						}
						_php_iconv_appendc(pretval, *p1, cd_pl);
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						} else {
							scan_stat = 0;
						}
						break;
				}
				break;

			case 12: /* expecting a non-encoded word */
				switch (*p1) {
					case '\r': /* part of an EOL sequence? */
						scan_stat = 7;
						break;

					case '\n':
						scan_stat = 8;
						break;

					case ' ': case '\t':
						spaces = p1;
						scan_stat = 11;
						break;

					case '=': /* first letter of an encoded chunk */
						if (!(mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							encoded_word = p1;
							scan_stat = 1;
							break;
						}
						/* break is omitted intentionally */

					default:
						_php_iconv_appendc(pretval, *p1, cd_pl);
						break;
				}
				break;
		}
	}
	switch (scan_stat) {
		case 0: case 8: case 11: case 12:
			break;
		default:
			if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
				if (scan_stat == 1) {
					_php_iconv_appendc(pretval, '=', cd_pl);
				}
				err = PHP_ICONV_ERR_SUCCESS;
			} else {
				err = PHP_ICONV_ERR_MALFORMED;
				goto out;
			}
	}

	if (next_pos != NULL) {
		*next_pos = p1;
	}

	smart_str_0(pretval);
out:
	if (cd != (iconv_t)(-1)) {
		iconv_close(cd);
	}
	if (cd_pl != (iconv_t)(-1)) {
		iconv_close(cd_pl);
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
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Wrong charset, conversion from `%s' to `%s' is not allowed",
			          in_charset, out_charset);
			break;

		case PHP_ICONV_ERR_ILLEGAL_CHAR:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected an incomplete multibyte character in input string");
			break;

		case PHP_ICONV_ERR_ILLEGAL_SEQ:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Detected an illegal character in input string");
			break;

		case PHP_ICONV_ERR_TOO_BIG:
			/* should not happen */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Buffer length exceeded");
			break;

		case PHP_ICONV_ERR_MALFORMED:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Malformed string");
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
	char *charset = ICONVG(internal_encoding);
	int charset_len = 0;
	char *str;
	int str_len;

	php_iconv_err_t err;

	unsigned int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
		&str, &str_len, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
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
	char *charset = ICONVG(internal_encoding);
	int charset_len = 0;
	char *str;
	int str_len;
	long offset, length = 0;

	php_iconv_err_t err;

	smart_str retval = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|ls",
		&str, &str_len, &offset, &length,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() < 3) {
		length = str_len;
	}

	err = _php_iconv_substr(&retval, str, str_len, offset, length, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS && str != NULL && retval.c != NULL) {
		RETURN_STRINGL(retval.c, retval.len, 0);
	}
	smart_str_free(&retval);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int iconv_strpos(string haystack, string needle [, int offset [, string charset]])
   Finds position of first occurrence of needle within part of haystack beginning with offset */
PHP_FUNCTION(iconv_strpos)
{
	char *charset = ICONVG(internal_encoding);
	int charset_len = 0;
	char *haystk;
	int haystk_len;
	char *ndl;
	int ndl_len;
	long offset = 0;

	php_iconv_err_t err;

	unsigned int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ls",
		&haystk, &haystk_len, &ndl, &ndl_len,
		&offset, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (offset < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string.");
		RETURN_FALSE;
	}

	if (ndl_len < 1) {
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

/* {{{ proto int iconv_strrpos(string haystack, string needle [, string charset])
   Finds position of last occurrence of needle within part of haystack beginning with offset */
PHP_FUNCTION(iconv_strrpos)
{
	char *charset = ICONVG(internal_encoding);
	int charset_len = 0;
	char *haystk;
	int haystk_len;
	char *ndl;
	int ndl_len;

	php_iconv_err_t err;

	unsigned int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s",
		&haystk, &haystk_len, &ndl, &ndl_len,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (ndl_len < 1) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
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

/* {{{ proto string iconv_mime_encode(string field_name, string field_value [, array preference])
   Composes a mime header field with field_name and field_value in a specified scheme */
PHP_FUNCTION(iconv_mime_encode)
{
	const char *field_name = NULL;
	int field_name_len;
	const char *field_value = NULL;
	int field_value_len;
	zval *pref = NULL;
	zval tmp_zv, *tmp_zv_p = NULL;
	smart_str retval = {0};
	php_iconv_err_t err;

	const char *in_charset = ICONVG(internal_encoding);
	const char *out_charset = in_charset;
	long line_len = 76;
	const char *lfchars = "\r\n";
	php_iconv_enc_scheme_t scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|a",
		&field_name, &field_name_len, &field_value, &field_value_len,
		&pref) == FAILURE) {

		RETURN_FALSE;
	}

	if (pref != NULL) {
		zval **ppval;

		if (zend_hash_find(Z_ARRVAL_P(pref), "scheme", sizeof("scheme"), (void **)&ppval) == SUCCESS) {
			if (Z_TYPE_PP(ppval) == IS_STRING && Z_STRLEN_PP(ppval) > 0) {
				switch (Z_STRVAL_PP(ppval)[0]) {
					case 'B': case 'b':
						scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;
						break;

					case 'Q': case 'q':
						scheme_id = PHP_ICONV_ENC_SCHEME_QPRINT;
						break;
				}
			}
		}

		if (zend_hash_find(Z_ARRVAL_P(pref), "input-charset", sizeof("input-charset"), (void **)&ppval) == SUCCESS) {
			if (Z_STRLEN_PP(ppval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(ppval) == IS_STRING && Z_STRLEN_PP(ppval) > 0) {
				in_charset = Z_STRVAL_PP(ppval);
			}
		}


		if (zend_hash_find(Z_ARRVAL_P(pref), "output-charset", sizeof("output-charset"), (void **)&ppval) == SUCCESS) {
			if (Z_STRLEN_PP(ppval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(ppval) == IS_STRING && Z_STRLEN_PP(ppval) > 0) {
				out_charset = Z_STRVAL_PP(ppval);
			}
		}

		if (zend_hash_find(Z_ARRVAL_P(pref), "line-length", sizeof("line-length"), (void **)&ppval) == SUCCESS) {
			zval val, *pval = *ppval;

			if (Z_TYPE_P(pval) != IS_LONG) {
				val = *pval;
				zval_copy_ctor(&val);
				convert_to_long(&val);
				pval = &val;
			}

			line_len = Z_LVAL_P(pval);

			if (pval == &val) {
				zval_dtor(&val);
			}
		}

		if (zend_hash_find(Z_ARRVAL_P(pref), "line-break-chars", sizeof("line-break-chars"), (void **)&ppval) == SUCCESS) {
			if (Z_TYPE_PP(ppval) != IS_STRING) {
				tmp_zv = **ppval;
				zval_copy_ctor(&tmp_zv);
				convert_to_string(&tmp_zv);

				lfchars = Z_STRVAL(tmp_zv);

				tmp_zv_p = &tmp_zv;
			} else {
				lfchars = Z_STRVAL_PP(ppval);
			}
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

	if (tmp_zv_p != NULL) {
		zval_dtor(tmp_zv_p);
	}
}
/* }}} */

/* {{{ proto string iconv_mime_decode(string encoded_string [, int mode, string charset])
   Decodes a mime header field */
PHP_FUNCTION(iconv_mime_decode)
{
	char *encoded_str;
	int encoded_str_len;
	char *charset = ICONVG(internal_encoding);
	int charset_len = 0;
	long mode = 0;

	smart_str retval = {0};

	php_iconv_err_t err;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls",
		&encoded_str, &encoded_str_len, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_mime_decode(&retval, encoded_str, encoded_str_len, charset, NULL, mode);
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

/* {{{ proto array iconv_mime_decode_headers(string headers [, int mode, string charset])
   Decodes multiple mime header fields */
PHP_FUNCTION(iconv_mime_decode_headers)
{
	const char *encoded_str;
	int encoded_str_len;
	char *charset = ICONVG(internal_encoding);
	int charset_len = 0;
	long mode = 0;

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls",
		&encoded_str, &encoded_str_len, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	array_init(return_value);

	while (encoded_str_len > 0) {
		smart_str decoded_header = {0};
		char *header_name = NULL;
		size_t header_name_len = 0;
		char *header_value = NULL;
		size_t header_value_len = 0;
		char *p, *limit;
		const char *next_pos;

		if (PHP_ICONV_ERR_SUCCESS != (err = _php_iconv_mime_decode(&decoded_header, encoded_str, encoded_str_len, charset, &next_pos, mode))) {
			smart_str_free(&decoded_header);
			break;
		}

		if (decoded_header.c == NULL) {
			break;
		}

		limit = decoded_header.c + decoded_header.len;
		for (p = decoded_header.c; p < limit; p++) {
			if (*p == ':') {
				*p = '\0';
				header_name = decoded_header.c;
				header_name_len = (p - decoded_header.c) + 1;

				while (++p < limit) {
					if (*p != ' ' && *p != '\t') {
						break;
					}
				}

				header_value = p;
				header_value_len = limit - p;

				break;
			}
		}

		if (header_name != NULL) {
			zval **elem, *new_elem;

			if (zend_hash_find(Z_ARRVAL_P(return_value), header_name, header_name_len, (void **)&elem) == SUCCESS) {
				if (Z_TYPE_PP(elem) != IS_ARRAY) {
					MAKE_STD_ZVAL(new_elem);
					array_init(new_elem);

					Z_ADDREF_PP(elem);
					add_next_index_zval(new_elem, *elem);

					zend_hash_update(Z_ARRVAL_P(return_value), header_name, header_name_len, (void *)&new_elem, sizeof(new_elem), NULL);

					elem = &new_elem;
				}
				add_next_index_stringl(*elem, header_value, header_value_len, 1);
			} else {
				add_assoc_stringl_ex(return_value, header_name, header_name_len, header_value, header_value_len, 1);
			}
		}
		encoded_str_len -= next_pos - encoded_str;
		encoded_str = next_pos;

		smart_str_free(&decoded_header);
	}

	if (err != PHP_ICONV_ERR_SUCCESS) {
		_php_iconv_show_error(err, charset, "???" TSRMLS_CC);
		zval_dtor(return_value);
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
	int in_charset_len = 0, out_charset_len = 0, in_buffer_len;
	php_iconv_err_t err;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
		&in_charset, &in_charset_len, &out_charset, &out_charset_len, &in_buffer, &in_buffer_len) == FAILURE)
		return;

	if (in_charset_len >= ICONV_CSNMAXLEN || out_charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = php_iconv_string(in_buffer, (size_t)in_buffer_len,
		&out_buffer, &out_len, out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset TSRMLS_CC);
	if (err == PHP_ICONV_ERR_SUCCESS && out_buffer != NULL) {
		RETVAL_STRINGL(out_buffer, out_len, 0);
	} else {
		if (out_buffer != NULL) {
			efree(out_buffer);
		}
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool iconv_set_encoding(string type, string charset)
   Sets internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_set_encoding)
{
	char *type, *charset;
	int type_len, charset_len =0, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &type, &type_len, &charset, &charset_len) == FAILURE)
		return;

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

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

/* {{{ proto mixed iconv_get_encoding([string type])
   Get internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_get_encoding)
{
	char *type = "all";
	int type_len = sizeof("all")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &type, &type_len) == FAILURE)
		return;

	if (!strcasecmp("all", type)) {
		array_init(return_value);
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

/* {{{ iconv stream filter */
typedef struct _php_iconv_stream_filter {
	iconv_t cd;
	int persistent;
	char *to_charset;
	size_t to_charset_len;
	char *from_charset;
	size_t from_charset_len;
	char stub[128];
	size_t stub_len;
} php_iconv_stream_filter;
/* }}} iconv stream filter */

/* {{{ php_iconv_stream_filter_dtor */
static void php_iconv_stream_filter_dtor(php_iconv_stream_filter *self)
{
	iconv_close(self->cd);
	pefree(self->to_charset, self->persistent);
	pefree(self->from_charset, self->persistent);
}
/* }}} */

/* {{{ php_iconv_stream_filter_ctor() */
static php_iconv_err_t php_iconv_stream_filter_ctor(php_iconv_stream_filter *self,
		const char *to_charset, size_t to_charset_len,
		const char *from_charset, size_t from_charset_len, int persistent)
{
	if (NULL == (self->to_charset = pemalloc(to_charset_len + 1, persistent))) {
		return PHP_ICONV_ERR_ALLOC;
	}
	self->to_charset_len = to_charset_len;
	if (NULL == (self->from_charset = pemalloc(from_charset_len + 1, persistent))) {
		pefree(self->to_charset, persistent);
		return PHP_ICONV_ERR_ALLOC;
	}
	self->from_charset_len = from_charset_len;

	memcpy(self->to_charset, to_charset, to_charset_len);
	self->to_charset[to_charset_len] = '\0';
	memcpy(self->from_charset, from_charset, from_charset_len);
	self->from_charset[from_charset_len] = '\0';

	if ((iconv_t)-1 == (self->cd = iconv_open(self->to_charset, self->from_charset))) {
		pefree(self->from_charset, persistent);
		pefree(self->to_charset, persistent);
		return PHP_ICONV_ERR_UNKNOWN;
	}
	self->persistent = persistent;
	self->stub_len = 0;
	return PHP_ICONV_ERR_SUCCESS;
}
/* }}} */

/* {{{ php_iconv_stream_filter_append_bucket */
static int php_iconv_stream_filter_append_bucket(
		php_iconv_stream_filter *self,
		php_stream *stream, php_stream_filter *filter,
		php_stream_bucket_brigade *buckets_out,
		const char *ps, size_t buf_len, size_t *consumed,
		int persistent TSRMLS_DC)
{
	php_stream_bucket *new_bucket;
	char *out_buf = NULL;
	size_t out_buf_size;
	char *pd, *pt;
	size_t ocnt, prev_ocnt, icnt, tcnt;
	size_t initial_out_buf_size;

	if (ps == NULL) {
		initial_out_buf_size = 64;
		icnt = 1;
	} else {
		initial_out_buf_size = buf_len;
		icnt = buf_len;
	}

	out_buf_size = ocnt = prev_ocnt = initial_out_buf_size;
	if (NULL == (out_buf = pemalloc(out_buf_size, persistent))) {
		return FAILURE;
	}

	pd = out_buf;

	if (self->stub_len > 0) {
		pt = self->stub;
		tcnt = self->stub_len;

		while (tcnt > 0) {
			if (iconv(self->cd, &pt, &tcnt, &pd, &ocnt) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
				switch (errno) {
					case EILSEQ:
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): invalid multibyte sequence", self->from_charset, self->to_charset);
						goto out_failure;

					case EINVAL:
						if (ps != NULL) {
							if (icnt > 0) {
								if (self->stub_len >= sizeof(self->stub)) {
									php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): insufficient buffer", self->from_charset, self->to_charset);
									goto out_failure;
								}
								self->stub[self->stub_len++] = *(ps++);
								icnt--;
								pt = self->stub;
								tcnt = self->stub_len;
							} else {
								tcnt = 0;
								break;
							}
						}
						break;

					case E2BIG: {
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

					default:
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
						goto out_failure;
				}
#else
				if (ocnt == prev_ocnt) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
					goto out_failure;
				}
#endif
			}
			prev_ocnt = ocnt;
		}
		memmove(self->stub, pt, tcnt);
		self->stub_len = tcnt;
	}

	while (icnt > 0) {
		if ((ps == NULL ? iconv(self->cd, NULL, NULL, &pd, &ocnt):
					iconv(self->cd, (char **)&ps, &icnt, &pd, &ocnt)) == (size_t)-1) {
#if ICONV_SUPPORTS_ERRNO
			switch (errno) {
				case EILSEQ:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): invalid multibyte sequence", self->from_charset, self->to_charset);
					goto out_failure;

				case EINVAL:
					if (ps != NULL) {
						if (icnt > sizeof(self->stub)) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): insufficient buffer", self->from_charset, self->to_charset);
							goto out_failure;
						}
						memcpy(self->stub, ps, icnt);
						self->stub_len = icnt;
						ps += icnt;
						icnt = 0;
					} else {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unexpected octet values", self->from_charset, self->to_charset);
						goto out_failure;
					}
					break;

				case E2BIG: {
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

				default:
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
					goto out_failure;
			}
#else
			if (ocnt == prev_ocnt) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
				goto out_failure;
			}
#endif
		} else {
			if (ps == NULL) {
				break;
			}
		}
		prev_ocnt = ocnt;
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
/* }}} php_iconv_stream_filter_append_bucket */

/* {{{ php_iconv_stream_filter_do_filter */
static php_stream_filter_status_t php_iconv_stream_filter_do_filter(
		php_stream *stream, php_stream_filter *filter,
		php_stream_bucket_brigade *buckets_in,
		php_stream_bucket_brigade *buckets_out,
		size_t *bytes_consumed, int flags TSRMLS_DC)
{
	php_stream_bucket *bucket = NULL;
	size_t consumed = 0;
	php_iconv_stream_filter *self = (php_iconv_stream_filter *)filter->abstract;

	while (buckets_in->head != NULL) {
		bucket = buckets_in->head;

		php_stream_bucket_unlink(bucket TSRMLS_CC);

		if (php_iconv_stream_filter_append_bucket(self, stream, filter,
				buckets_out, bucket->buf, bucket->buflen, &consumed,
				php_stream_is_persistent(stream) TSRMLS_CC) != SUCCESS) {
			goto out_failure;
		}

		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags != PSFS_FLAG_NORMAL) {
		if (php_iconv_stream_filter_append_bucket(self, stream, filter,
				buckets_out, NULL, 0, &consumed,
				php_stream_is_persistent(stream) TSRMLS_CC) != SUCCESS) {
			goto out_failure;
		}
	}

	if (bytes_consumed != NULL) {
		*bytes_consumed = consumed;
	}

	return PSFS_PASS_ON;

out_failure:
	if (bucket != NULL) {
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}
	return PSFS_ERR_FATAL;
}
/* }}} */

/* {{{ php_iconv_stream_filter_cleanup */
static void php_iconv_stream_filter_cleanup(php_stream_filter *filter TSRMLS_DC)
{
	php_iconv_stream_filter_dtor((php_iconv_stream_filter *)filter->abstract);
	pefree(filter->abstract, ((php_iconv_stream_filter *)filter->abstract)->persistent);
}
/* }}} */

static php_stream_filter_ops php_iconv_stream_filter_ops = {
	php_iconv_stream_filter_do_filter,
	php_iconv_stream_filter_cleanup,
	"convert.iconv.*"
};

/* {{{ php_iconv_stream_filter_create */
static php_stream_filter *php_iconv_stream_filter_factory_create(const char *name, zval *params, int persistent TSRMLS_DC)
{
	php_stream_filter *retval = NULL;
	php_iconv_stream_filter *inst;
	char *from_charset = NULL, *to_charset = NULL;
	size_t from_charset_len, to_charset_len;

	if ((from_charset = strchr(name, '.')) == NULL) {
		return NULL;
	}
	++from_charset;
	if ((from_charset = strchr(from_charset, '.')) == NULL) {
		return NULL;
	}
	++from_charset;
	if ((to_charset = strpbrk(from_charset, "/.")) == NULL) {
		return NULL;
	}
	from_charset_len = to_charset - from_charset;
	++to_charset;
	to_charset_len = strlen(to_charset);

	if (from_charset_len >= ICONV_CSNMAXLEN || to_charset_len >= ICONV_CSNMAXLEN) {
		return NULL;
	}

	if (NULL == (inst = pemalloc(sizeof(php_iconv_stream_filter), persistent))) {
		return NULL;
	}

	if (php_iconv_stream_filter_ctor(inst, to_charset, to_charset_len, from_charset, from_charset_len, persistent) != PHP_ICONV_ERR_SUCCESS) {
		pefree(inst, persistent);
		return NULL;
	}

	if (NULL == (retval = php_stream_filter_alloc(&php_iconv_stream_filter_ops, inst, persistent))) {
		php_iconv_stream_filter_dtor(inst);
		pefree(inst, persistent);
	}

	return retval;
}
/* }}} */

/* {{{ php_iconv_stream_register_factory */
static php_iconv_err_t php_iconv_stream_filter_register_factory(TSRMLS_D)
{
	static php_stream_filter_factory filter_factory = {
		php_iconv_stream_filter_factory_create
	};

	if (FAILURE == php_stream_filter_register_factory(
				php_iconv_stream_filter_ops.label,
				&filter_factory TSRMLS_CC)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}
	return PHP_ICONV_ERR_SUCCESS;
}
/* }}} */

/* {{{ php_iconv_stream_unregister_factory */
static php_iconv_err_t php_iconv_stream_filter_unregister_factory(TSRMLS_D)
{
	if (FAILURE == php_stream_filter_unregister_factory(
				php_iconv_stream_filter_ops.label TSRMLS_CC)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}
	return PHP_ICONV_ERR_SUCCESS;
}
/* }}} */
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
