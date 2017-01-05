/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
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

#include "zend_smart_str.h"
#include "ext/standard/base64.h"
#include "ext/standard/quot_print.h"

#define _php_iconv_memequal(a, b, c) \
  ((c) == sizeof(zend_ulong) ? *((zend_ulong *)(a)) == *((zend_ulong *)(b)) : ((c) == sizeof(unsigned int) ? *((unsigned int *)(a)) == *((unsigned int *)(b)) : memcmp(a, b, c) == 0))

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
	PHP_ICONV_VERSION,
	PHP_MODULE_GLOBALS(iconv),
	PHP_GINIT(iconv),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ICONV
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(iconv)
#endif

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(iconv)
{
#if defined(COMPILE_DL_ICONV) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
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

static void _php_iconv_show_error(php_iconv_err_t err, const char *out_charset, const char *in_charset);

static php_iconv_err_t _php_iconv_strlen(size_t *pretval, const char *str, size_t nbytes, const char *enc);

static php_iconv_err_t _php_iconv_substr(smart_str *pretval, const char *str, size_t nbytes, zend_long offset, zend_long len, const char *enc);

static php_iconv_err_t _php_iconv_strpos(size_t *pretval, const char *haystk, size_t haystk_nbytes, const char *ndl, size_t ndl_nbytes, zend_long offset, const char *enc);

static php_iconv_err_t _php_iconv_mime_encode(smart_str *pretval, const char *fname, size_t fname_nbytes, const char *fval, size_t fval_nbytes, size_t max_line_len, const char *lfchars, php_iconv_enc_scheme_t enc_scheme, const char *out_charset, const char *enc);

static php_iconv_err_t _php_iconv_mime_decode(smart_str *pretval, const char *str, size_t str_nbytes, const char *enc, const char **next_pos, int mode);

static php_iconv_err_t php_iconv_stream_filter_register_factory(void);
static php_iconv_err_t php_iconv_stream_filter_unregister_factory(void);

static int php_iconv_output_conflict(const char *handler_name, size_t handler_name_len);
static php_output_handler *php_iconv_output_handler_init(const char *name, size_t name_len, size_t chunk_size, int flags);
static int php_iconv_output_handler(void **nothing, php_output_context *output_context);
/* }}} */

/* {{{ static globals */
static char _generic_superset_name[] = ICONV_UCS4_ENCODING;
#define GENERIC_SUPERSET_NAME _generic_superset_name
#define GENERIC_SUPERSET_NBYTES 4
/* }}} */


static PHP_INI_MH(OnUpdateInputEncoding)
{
	if (ZSTR_LEN(new_value) >= ICONV_CSNMAXLEN) {
		return FAILURE;
	}
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.iconv", E_DEPRECATED, "Use of iconv.input_encoding is deprecated");
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}


static PHP_INI_MH(OnUpdateOutputEncoding)
{
	if (ZSTR_LEN(new_value) >= ICONV_CSNMAXLEN) {
		return FAILURE;
	}
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.iconv", E_DEPRECATED, "Use of iconv.output_encoding is deprecated");
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}


static PHP_INI_MH(OnUpdateInternalEncoding)
{
	if (ZSTR_LEN(new_value) >= ICONV_CSNMAXLEN) {
		return FAILURE;
	}
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.iconv", E_DEPRECATED, "Use of iconv.internal_encoding is deprecated");
	}
	OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	return SUCCESS;
}


/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("iconv.input_encoding",    "", PHP_INI_ALL, OnUpdateInputEncoding,    input_encoding,    zend_iconv_globals, iconv_globals)
	STD_PHP_INI_ENTRY("iconv.output_encoding",   "", PHP_INI_ALL, OnUpdateOutputEncoding,   output_encoding,   zend_iconv_globals, iconv_globals)
	STD_PHP_INI_ENTRY("iconv.internal_encoding", "", PHP_INI_ALL, OnUpdateInternalEncoding, internal_encoding, zend_iconv_globals, iconv_globals)
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

	if (php_iconv_stream_filter_register_factory() != PHP_ICONV_ERR_SUCCESS) {
		return FAILURE;
	}

	php_output_handler_alias_register(ZEND_STRL("ob_iconv_handler"), php_iconv_output_handler_init);
	php_output_handler_conflict_register(ZEND_STRL("ob_iconv_handler"), php_iconv_output_conflict);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(miconv)
{
	php_iconv_stream_filter_unregister_factory();
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(miconv)
{
	zval *iconv_impl, *iconv_ver;

	iconv_impl = zend_get_constant_str("ICONV_IMPL", sizeof("ICONV_IMPL")-1);
	iconv_ver = zend_get_constant_str("ICONV_VERSION", sizeof("ICONV_VERSION")-1);

	php_info_print_table_start();
	php_info_print_table_row(2, "iconv support", "enabled");
	php_info_print_table_row(2, "iconv implementation", Z_STRVAL_P(iconv_impl));
	php_info_print_table_row(2, "iconv library version", Z_STRVAL_P(iconv_ver));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static char *get_internal_encoding(void) {
	if (ICONVG(internal_encoding) && ICONVG(internal_encoding)[0]) {
		return ICONVG(internal_encoding);
	} else if (PG(internal_encoding) && PG(internal_encoding)[0]) {
		return PG(internal_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}

static char *get_input_encoding(void) {
	if (ICONVG(input_encoding) && ICONVG(input_encoding)[0]) {
		return ICONVG(input_encoding);
	} else if (PG(input_encoding) && PG(input_encoding)[0]) {
		return PG(input_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}

static char *get_output_encoding(void) {
	if (ICONVG(output_encoding) && ICONVG(output_encoding)[0]) {
		return ICONVG(output_encoding);
	} else if (PG(output_encoding) && PG(output_encoding)[0]) {
		return PG(output_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}


static int php_iconv_output_conflict(const char *handler_name, size_t handler_name_len)
{
	if (php_output_get_level()) {
		if (php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("ob_iconv_handler"))
		||	php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("mb_output_handler"))) {
			return FAILURE;
		}
	}
	return SUCCESS;
}

static php_output_handler *php_iconv_output_handler_init(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags)
{
	return php_output_handler_create_internal(handler_name, handler_name_len, php_iconv_output_handler, chunk_size, flags);
}

static int php_iconv_output_handler(void **nothing, php_output_context *output_context)
{
	char *s, *content_type, *mimetype = NULL;
	int output_status, mimetype_len = 0;

	if (output_context->op & PHP_OUTPUT_HANDLER_START) {
		output_status = php_output_get_status();
		if (output_status & PHP_OUTPUT_SENT) {
			return FAILURE;
		}

		if (SG(sapi_headers).mimetype && !strncasecmp(SG(sapi_headers).mimetype, "text/", 5)) {
			if ((s = strchr(SG(sapi_headers).mimetype,';')) == NULL){
				mimetype = SG(sapi_headers).mimetype;
			} else {
				mimetype = SG(sapi_headers).mimetype;
				mimetype_len = (int)(s - SG(sapi_headers).mimetype);
			}
		} else if (SG(sapi_headers).send_default_content_type) {
			mimetype = SG(default_mimetype) ? SG(default_mimetype) : SAPI_DEFAULT_MIMETYPE;
		}

		if (mimetype != NULL && !(output_context->op & PHP_OUTPUT_HANDLER_CLEAN)) {
			size_t len;
			char *p = strstr(get_output_encoding(), "//");

			if (p) {
				len = spprintf(&content_type, 0, "Content-Type:%.*s; charset=%.*s", mimetype_len ? mimetype_len : (int) strlen(mimetype), mimetype, (int) (p - get_output_encoding()), get_output_encoding());
			} else {
				len = spprintf(&content_type, 0, "Content-Type:%.*s; charset=%s", mimetype_len ? mimetype_len : (int) strlen(mimetype), mimetype, get_output_encoding());
			}
			if (content_type && SUCCESS == sapi_add_header(content_type, (uint)len, 0)) {
				SG(sapi_headers).send_default_content_type = 0;
				php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE, NULL);
			}
		}
	}

	if (output_context->in.used) {
		zend_string *out;
		output_context->out.free = 1;
		_php_iconv_show_error(php_iconv_string(output_context->in.data, output_context->in.used, &out, get_output_encoding(), get_internal_encoding()), get_output_encoding(), get_internal_encoding());
		if (out) {
			output_context->out.data = estrndup(ZSTR_VAL(out), ZSTR_LEN(out));
			output_context->out.used = ZSTR_LEN(out);
			zend_string_free(out);
		} else {
			output_context->out.data = NULL;
			output_context->out.used = 0;
		}
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
			smart_str_alloc(d, out_left, 0);

			out_p = ZSTR_VAL((d)->s) + ZSTR_LEN((d)->s);

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
			ZSTR_LEN((d)->s) += (buf_growth - out_left);
			buf_growth <<= 1;
		}
	} else {
		for (;;) {
			out_left = buf_growth - out_left;
			smart_str_alloc(d, out_left, 0);

			out_p = ZSTR_VAL((d)->s) + ZSTR_LEN((d)->s);

			if (iconv(cd, NULL, NULL, (char **) &out_p, &out_left) == (size_t)0) {
				ZSTR_LEN((d)->s) += (buf_growth - out_left);
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
			ZSTR_LEN((d)->s) += (buf_growth - out_left);
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

/* {{{ */
#if ICONV_BROKEN_IGNORE
static int _php_check_ignore(const char *charset)
{
  size_t clen = strlen(charset);
  if (clen >= 9 && strcmp("//IGNORE", charset+clen-8) == 0) {
    return 1;
  }
  if (clen >= 19 && strcmp("//IGNORE//TRANSLIT", charset+clen-18) == 0) {
    return 1;
  }
  return 0;
}
#else
#define _php_check_ignore(x) (0)
#endif
/* }}} */

/* {{{ php_iconv_string()
 */
PHP_ICONV_API php_iconv_err_t php_iconv_string(const char *in_p, size_t in_len, zend_string **out, const char *out_charset, const char *in_charset)
{
#if !ICONV_SUPPORTS_ERRNO
	size_t in_size, out_size, out_left;
	char *out_p;
	iconv_t cd;
	size_t result;
	zend_string *ret, *out_buffer;

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

	out_buffer = zend_string_alloc(out_size, 0);
	out_p = ZSTR_VAL(out_buffer);

#ifdef NETWARE
	result = iconv(cd, (char **) &in_p, &in_size, (char **)
#else
	result = iconv(cd, (const char **) &in_p, &in_size, (char **)
#endif
				&out_p, &out_left);

	if (result == (size_t)(-1)) {
		zend_string_free(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	if (out_left < 8) {
		size_t pos = out_p - ZSTR_VAL(out_buffer);
		out_buffer = zend_string_extend(out_buffer, out_size + 8, 0);
		out_p = ZSTR_VAL(out_buffer) + pos;
		out_size += 7;
		out_left += 7;
	}

	/* flush the shift-out sequences */
	result = iconv(cd, NULL, NULL, &out_p, &out_left);

	if (result == (size_t)(-1)) {
		zend_string_free(out_buffer);
		return PHP_ICONV_ERR_UNKNOWN;
	}

	ZSTR_VAL(out_buffer)[out_size - out_left] = '\0';
	ZSTR_LEN(out_buffer) = out_size - out_left;

	iconv_close(cd);

	*out = out_buffer;
	return PHP_ICONV_ERR_SUCCESS;

#else
	/*
	  iconv supports errno. Handle it better way.
	*/
	iconv_t cd;
	size_t in_left, out_size, out_left;
	char *out_p;
	size_t bsz, result = 0;
	php_iconv_err_t retval = PHP_ICONV_ERR_SUCCESS;
	zend_string *out_buf;
	int ignore_ilseq = _php_check_ignore(out_charset);

	*out = NULL;

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
	out_buf = zend_string_alloc(bsz, 0);
	out_p = ZSTR_VAL(out_buf);

	while (in_left > 0) {
		result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
		out_size = bsz - out_left;
		if (result == (size_t)(-1)) {
			if (ignore_ilseq && errno == EILSEQ) {
				if (in_left <= 1) {
					result = 0;
				} else {
					errno = 0;
					in_p++;
					in_left--;
					continue;
				}
			}

			if (errno == E2BIG && in_left > 0) {
				/* converted string is longer than out buffer */
				bsz += in_len;

				out_buf = zend_string_extend(out_buf, bsz, 0);
				out_p = ZSTR_VAL(out_buf);
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
				out_buf = zend_string_extend(out_buf, bsz, 0);
				out_p = ZSTR_VAL(out_buf);
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
				zend_string_free(out_buf);
				return PHP_ICONV_ERR_UNKNOWN;
		}
	}
	*out_p = '\0';
	ZSTR_LEN(out_buf) = out_size;
	*out = out_buf;
	return retval;
#endif
}
/* }}} */

/* {{{ _php_iconv_strlen() */
static php_iconv_err_t _php_iconv_strlen(size_t *pretval, const char *str, size_t nbytes, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES*2];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	size_t cnt;

	*pretval = (size_t)-1;

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

	errno = 0;
	out_left = 0;

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
	const char *str, size_t nbytes, zend_long offset, zend_long len, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd1, cd2;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	size_t cnt;
	size_t total_len;

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

	if((size_t)len > total_len) {
		len = total_len;
	}


	if ((size_t)offset > total_len) {
		return PHP_ICONV_ERR_SUCCESS;
	}

	if ((size_t)(offset + len) > total_len ) {
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

		if ((zend_long)cnt >= offset) {
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
static php_iconv_err_t _php_iconv_strpos(size_t *pretval,
	const char *haystk, size_t haystk_nbytes,
	const char *ndl, size_t ndl_nbytes,
	zend_long offset, const char *enc)
{
	char buf[GENERIC_SUPERSET_NBYTES];

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd;

	const char *in_p;
	size_t in_left;

	char *out_p;
	size_t out_left;

	size_t cnt;

	zend_string *ndl_buf;
	const char *ndl_buf_p;
	size_t ndl_buf_left;

	size_t match_ofs;

	*pretval = (size_t)-1;

	err = php_iconv_string(ndl, ndl_nbytes, &ndl_buf, GENERIC_SUPERSET_NAME, enc);

	if (err != PHP_ICONV_ERR_SUCCESS) {
		if (ndl_buf != NULL) {
			zend_string_free(ndl_buf);
		}
		return err;
	}

	cd = iconv_open(GENERIC_SUPERSET_NAME, enc);

	if (cd == (iconv_t)(-1)) {
		if (ndl_buf != NULL) {
			zend_string_free(ndl_buf);
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

	ndl_buf_p = ZSTR_VAL(ndl_buf);
	ndl_buf_left = ZSTR_LEN(ndl_buf);
	match_ofs = (size_t)-1;

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
			if (cnt >= (size_t)offset) {
				if (_php_iconv_memequal(buf, ndl_buf_p, sizeof(buf))) {
					if (match_ofs == (size_t)-1) {
						match_ofs = cnt;
					}
					ndl_buf_p += GENERIC_SUPERSET_NBYTES;
					ndl_buf_left -= GENERIC_SUPERSET_NBYTES;
					if (ndl_buf_left == 0) {
						*pretval = match_ofs;
						break;
					}
				} else {
					size_t i, j, lim;

					i = 0;
					j = GENERIC_SUPERSET_NBYTES;
					lim = (size_t)(ndl_buf_p - ZSTR_VAL(ndl_buf));

					while (j < lim) {
						if (_php_iconv_memequal(&ZSTR_VAL(ndl_buf)[j], &ZSTR_VAL(ndl_buf)[i],
						           GENERIC_SUPERSET_NBYTES)) {
							i += GENERIC_SUPERSET_NBYTES;
						} else {
							j -= i;
							i = 0;
						}
						j += GENERIC_SUPERSET_NBYTES;
					}

					if (_php_iconv_memequal(buf, &ZSTR_VAL(ndl_buf)[i], sizeof(buf))) {
						match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
						i += GENERIC_SUPERSET_NBYTES;
						ndl_buf_p = &ZSTR_VAL(ndl_buf)[i];
						ndl_buf_left = ZSTR_LEN(ndl_buf) - i;
					} else {
						match_ofs = (size_t)-1;
						ndl_buf_p = ZSTR_VAL(ndl_buf);
						ndl_buf_left = ZSTR_LEN(ndl_buf);
					}
				}
			}
		} else {
			if (_php_iconv_memequal(buf, ndl_buf_p, sizeof(buf))) {
				if (match_ofs == (size_t)-1) {
					match_ofs = cnt;
				}
				ndl_buf_p += GENERIC_SUPERSET_NBYTES;
				ndl_buf_left -= GENERIC_SUPERSET_NBYTES;
				if (ndl_buf_left == 0) {
					*pretval = match_ofs;
					ndl_buf_p = ZSTR_VAL(ndl_buf);
					ndl_buf_left = ZSTR_LEN(ndl_buf);
					match_ofs = -1;
				}
			} else {
				size_t i, j, lim;

				i = 0;
				j = GENERIC_SUPERSET_NBYTES;
				lim = (size_t)(ndl_buf_p - ZSTR_VAL(ndl_buf));

				while (j < lim) {
					if (_php_iconv_memequal(&ZSTR_VAL(ndl_buf)[j], &ZSTR_VAL(ndl_buf)[i],
							   GENERIC_SUPERSET_NBYTES)) {
						i += GENERIC_SUPERSET_NBYTES;
					} else {
						j -= i;
						i = 0;
					}
					j += GENERIC_SUPERSET_NBYTES;
				}

				if (_php_iconv_memequal(buf, &ZSTR_VAL(ndl_buf)[i], sizeof(buf))) {
					match_ofs += (lim - i) / GENERIC_SUPERSET_NBYTES;
					i += GENERIC_SUPERSET_NBYTES;
					ndl_buf_p = &ZSTR_VAL(ndl_buf)[i];
					ndl_buf_left = ZSTR_LEN(ndl_buf) - i;
				} else {
					match_ofs = (size_t)-1;
					ndl_buf_p = ZSTR_VAL(ndl_buf);
					ndl_buf_left = ZSTR_LEN(ndl_buf);
				}
			}
		}
	}

	if (ndl_buf) {
		zend_string_free(ndl_buf);
	}

	iconv_close(cd);

	return err;
}
/* }}} */

/* {{{ _php_iconv_mime_encode() */
static php_iconv_err_t _php_iconv_mime_encode(smart_str *pretval, const char *fname, size_t fname_nbytes, const char *fval, size_t fval_nbytes, size_t max_line_len, const char *lfchars, php_iconv_enc_scheme_t enc_scheme, const char *out_charset, const char *enc)
{
	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;
	iconv_t cd = (iconv_t)(-1), cd_pl = (iconv_t)(-1);
	size_t char_cnt = 0;
	size_t out_charset_len;
	size_t lfchars_len;
	char *buf = NULL;
	const char *in_p;
	size_t in_left;
	char *out_p;
	size_t out_left;
	zend_string *encoded = NULL;
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

				encoded = php_base64_encode((unsigned char *) buf, (out_size - out_left));

				if (char_cnt < ZSTR_LEN(encoded)) {
					/* something went wrong! */
					err = PHP_ICONV_ERR_UNKNOWN;
					goto out;
				}

				smart_str_appendl(pretval, ZSTR_VAL(encoded), ZSTR_LEN(encoded));
				char_cnt -= ZSTR_LEN(encoded);
				smart_str_appendl(pretval, "?=", sizeof("?=") - 1);
				char_cnt -= 2;

				zend_string_release(encoded);
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
#if !ICONV_SUPPORTS_ERRNO
					size_t prev_out_left;
#endif

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
#if !ICONV_SUPPORTS_ERRNO
					prev_out_left = out_left;
#endif
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
		zend_string_release(encoded);
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
						zend_string *decoded_text;

						switch (enc_scheme) {
							case PHP_ICONV_ENC_SCHEME_BASE64:
								decoded_text = php_base64_decode((unsigned char*)encoded_text, encoded_text_len);
								break;

							case PHP_ICONV_ENC_SCHEME_QPRINT:
								decoded_text = php_quot_print_decode((unsigned char*)encoded_text, encoded_text_len, 1);
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

						err = _php_iconv_appendl(pretval, ZSTR_VAL(decoded_text), ZSTR_LEN(decoded_text), cd);
						zend_string_release(decoded_text);

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
static void _php_iconv_show_error(php_iconv_err_t err, const char *out_charset, const char *in_charset)
{
	switch (err) {
		case PHP_ICONV_ERR_SUCCESS:
			break;

		case PHP_ICONV_ERR_CONVERTER:
			php_error_docref(NULL, E_NOTICE, "Cannot open converter");
			break;

		case PHP_ICONV_ERR_WRONG_CHARSET:
			php_error_docref(NULL, E_NOTICE, "Wrong charset, conversion from `%s' to `%s' is not allowed",
			          in_charset, out_charset);
			break;

		case PHP_ICONV_ERR_ILLEGAL_CHAR:
			php_error_docref(NULL, E_NOTICE, "Detected an incomplete multibyte character in input string");
			break;

		case PHP_ICONV_ERR_ILLEGAL_SEQ:
			php_error_docref(NULL, E_NOTICE, "Detected an illegal character in input string");
			break;

		case PHP_ICONV_ERR_TOO_BIG:
			/* should not happen */
			php_error_docref(NULL, E_WARNING, "Buffer length exceeded");
			break;

		case PHP_ICONV_ERR_MALFORMED:
			php_error_docref(NULL, E_WARNING, "Malformed string");
			break;

		default:
			/* other error */
			php_error_docref(NULL, E_NOTICE, "Unknown error (%d)", errno);
			break;
	}
}
/* }}} */

/* {{{ proto int iconv_strlen(string str [, string charset])
   Returns the character count of str */
PHP_FUNCTION(iconv_strlen)
{
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *str;

	php_iconv_err_t err;

	size_t retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|s",
		&str, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_strlen(&retval, ZSTR_VAL(str), ZSTR_LEN(str), charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);
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
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *str;
	zend_long offset, length = 0;

	php_iconv_err_t err;

	smart_str retval = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl|ls",
		&str, &offset, &length,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() < 3) {
		length = ZSTR_LEN(str);
	}

	err = _php_iconv_substr(&retval, ZSTR_VAL(str), ZSTR_LEN(str), offset, length, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);

	if (err == PHP_ICONV_ERR_SUCCESS && ZSTR_LEN(str) >= 0 && retval.s != NULL) {
		RETURN_NEW_STR(retval.s);
	}
	smart_str_free(&retval);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int iconv_strpos(string haystack, string needle [, int offset [, string charset]])
   Finds position of first occurrence of needle within part of haystack beginning with offset */
PHP_FUNCTION(iconv_strpos)
{
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *haystk;
	zend_string *ndl;
	zend_long offset = 0;

	php_iconv_err_t err;

	size_t retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|ls",
		&haystk, &ndl,
		&offset, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (offset < 0) {
		php_error_docref(NULL, E_WARNING, "Offset not contained in string.");
		RETURN_FALSE;
	}

	if (ZSTR_LEN(ndl) < 1) {
		RETURN_FALSE;
	}

	err = _php_iconv_strpos(&retval, ZSTR_VAL(haystk), ZSTR_LEN(haystk), ZSTR_VAL(ndl), ZSTR_LEN(ndl),
	                        offset, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);

	if (err == PHP_ICONV_ERR_SUCCESS && retval != (size_t)-1) {
		RETVAL_LONG((zend_long)retval);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int iconv_strrpos(string haystack, string needle [, string charset])
   Finds position of last occurrence of needle within part of haystack beginning with offset */
PHP_FUNCTION(iconv_strrpos)
{
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *haystk;
	zend_string *ndl;

	php_iconv_err_t err;

	size_t retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|s",
		&haystk, &ndl,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZSTR_LEN(ndl) < 1) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_strpos(&retval, ZSTR_VAL(haystk), ZSTR_LEN(haystk), ZSTR_VAL(ndl), ZSTR_LEN(ndl),
	                        -1, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);

	if (err == PHP_ICONV_ERR_SUCCESS && retval != (size_t)-1) {
		RETVAL_LONG((zend_long)retval);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv_mime_encode(string field_name, string field_value [, array preference])
   Composes a mime header field with field_name and field_value in a specified scheme */
PHP_FUNCTION(iconv_mime_encode)
{
	zend_string *field_name = NULL;
	zend_string *field_value = NULL;
	zend_string *tmp_str = NULL;
	zval *pref = NULL;
	smart_str retval = {0};
	php_iconv_err_t err;

	const char *in_charset = get_internal_encoding();
	const char *out_charset = in_charset;
	zend_long line_len = 76;
	const char *lfchars = "\r\n";
	php_iconv_enc_scheme_t scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|a",
		&field_name, &field_value,
		&pref) == FAILURE) {

		RETURN_FALSE;
	}

	if (pref != NULL) {
		zval *pzval;

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "scheme", sizeof("scheme") - 1)) != NULL) {
			if (Z_TYPE_P(pzval) == IS_STRING && Z_STRLEN_P(pzval) > 0) {
				switch (Z_STRVAL_P(pzval)[0]) {
					case 'B': case 'b':
						scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;
						break;

					case 'Q': case 'q':
						scheme_id = PHP_ICONV_ENC_SCHEME_QPRINT;
						break;
				}
			}
		}

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "input-charset", sizeof("input-charset") - 1)) != NULL && Z_TYPE_P(pzval) == IS_STRING) {
			if (Z_STRLEN_P(pzval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_STRLEN_P(pzval) > 0) {
				in_charset = Z_STRVAL_P(pzval);
			}
		}


		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "output-charset", sizeof("output-charset") - 1)) != NULL && Z_TYPE_P(pzval) == IS_STRING) {
			if (Z_STRLEN_P(pzval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_STRLEN_P(pzval) > 0) {
				out_charset = Z_STRVAL_P(pzval);
			}
		}

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "line-length", sizeof("line-length") - 1)) != NULL) {
			line_len = zval_get_long(pzval);
		}

		if ((pzval = zend_hash_str_find(Z_ARRVAL_P(pref), "line-break-chars", sizeof("line-break-chars") - 1)) != NULL) {
			if (Z_TYPE_P(pzval) != IS_STRING) {
				tmp_str = zval_get_string(pzval);
				lfchars = ZSTR_VAL(tmp_str);
			} else {
				lfchars = Z_STRVAL_P(pzval);
			}
		}
	}

	err = _php_iconv_mime_encode(&retval, ZSTR_VAL(field_name), ZSTR_LEN(field_name),
		ZSTR_VAL(field_value), ZSTR_LEN(field_value), line_len, lfchars, scheme_id,
		out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset);

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.s != NULL) {
			RETVAL_STR(retval.s);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}

	if (tmp_str) {
		zend_string_release(tmp_str);
	}
}
/* }}} */

/* {{{ proto string iconv_mime_decode(string encoded_string [, int mode, string charset])
   Decodes a mime header field */
PHP_FUNCTION(iconv_mime_decode)
{
	zend_string *encoded_str;
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_long mode = 0;

	smart_str retval = {0};

	php_iconv_err_t err;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ls",
		&encoded_str, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_mime_decode(&retval, ZSTR_VAL(encoded_str), ZSTR_LEN(encoded_str), charset, NULL, (int)mode);
	_php_iconv_show_error(err, charset, "???");

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.s != NULL) {
			RETVAL_STR(retval.s);
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
	zend_string *encoded_str;
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_long mode = 0;
	char *enc_str_tmp;
	size_t enc_str_len_tmp;

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ls",
		&encoded_str, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	array_init(return_value);

	enc_str_tmp = ZSTR_VAL(encoded_str);
	enc_str_len_tmp = ZSTR_LEN(encoded_str);
	while (enc_str_len_tmp > 0) {
		smart_str decoded_header = {0};
		char *header_name = NULL;
		size_t header_name_len = 0;
		char *header_value = NULL;
		size_t header_value_len = 0;
		char *p, *limit;
		const char *next_pos;

		if (PHP_ICONV_ERR_SUCCESS != (err = _php_iconv_mime_decode(&decoded_header, enc_str_tmp, enc_str_len_tmp, charset, &next_pos, (int)mode))) {
			smart_str_free(&decoded_header);
			break;
		}

		if (decoded_header.s == NULL) {
			break;
		}

		limit = ZSTR_VAL(decoded_header.s) + ZSTR_LEN(decoded_header.s);
		for (p = ZSTR_VAL(decoded_header.s); p < limit; p++) {
			if (*p == ':') {
				*p = '\0';
				header_name = ZSTR_VAL(decoded_header.s);
				header_name_len = p - ZSTR_VAL(decoded_header.s);

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
			zval *elem;

			if ((elem = zend_hash_str_find(Z_ARRVAL_P(return_value), header_name, header_name_len)) != NULL) {
				if (Z_TYPE_P(elem) != IS_ARRAY) {
					zval new_elem;

					array_init(&new_elem);
					Z_ADDREF_P(elem);
					add_next_index_zval(&new_elem, elem);

					elem = zend_hash_str_update(Z_ARRVAL_P(return_value), header_name, header_name_len, &new_elem);
				}
				add_next_index_stringl(elem, header_value, header_value_len);
			} else {
				add_assoc_stringl_ex(return_value, header_name, header_name_len, header_value, header_value_len);
			}
		}
		enc_str_len_tmp -= next_pos - enc_str_tmp;
		enc_str_tmp = (char *)next_pos;

		smart_str_free(&decoded_header);
	}

	if (err != PHP_ICONV_ERR_SUCCESS) {
		_php_iconv_show_error(err, charset, "???");
		zval_dtor(return_value);
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string iconv(string in_charset, string out_charset, string str)
   Returns str converted to the out_charset character set */
PHP_NAMED_FUNCTION(php_if_iconv)
{
	char *in_charset, *out_charset;
	zend_string *in_buffer;
	size_t in_charset_len = 0, out_charset_len = 0;
	php_iconv_err_t err;
	zend_string *out_buffer;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssS",
		&in_charset, &in_charset_len, &out_charset, &out_charset_len, &in_buffer) == FAILURE)
		return;

	if (in_charset_len >= ICONV_CSNMAXLEN || out_charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = php_iconv_string(ZSTR_VAL(in_buffer), (size_t)ZSTR_LEN(in_buffer), &out_buffer, out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset);
	if (err == PHP_ICONV_ERR_SUCCESS && out_buffer != NULL) {
		RETVAL_STR(out_buffer);
	} else {
		if (out_buffer != NULL) {
			zend_string_free(out_buffer);
		}
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool iconv_set_encoding(string type, string charset)
   Sets internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_set_encoding)
{
	char *type;
	zend_string *charset;
	size_t type_len, retval;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sS", &type, &type_len, &charset) == FAILURE)
		return;

	if (ZSTR_LEN(charset) >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if(!strcasecmp("input_encoding", type)) {
		name = zend_string_init("iconv.input_encoding", sizeof("iconv.input_encoding") - 1, 0);
	} else if(!strcasecmp("output_encoding", type)) {
		name = zend_string_init("iconv.output_encoding", sizeof("iconv.output_encoding") - 1, 0);
	} else if(!strcasecmp("internal_encoding", type)) {
		name = zend_string_init("iconv.internal_encoding", sizeof("iconv.internal_encoding") - 1, 0);
	} else {
		RETURN_FALSE;
	}

	retval = zend_alter_ini_entry(name, charset, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	zend_string_release(name);

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
	size_t type_len = sizeof("all")-1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &type, &type_len) == FAILURE)
		return;

	if (!strcasecmp("all", type)) {
		array_init(return_value);
		add_assoc_string(return_value, "input_encoding",    get_input_encoding());
		add_assoc_string(return_value, "output_encoding",   get_output_encoding());
		add_assoc_string(return_value, "internal_encoding", get_internal_encoding());
	} else if (!strcasecmp("input_encoding", type)) {
		RETVAL_STRING(get_input_encoding());
	} else if (!strcasecmp("output_encoding", type)) {
		RETVAL_STRING(get_output_encoding());
	} else if (!strcasecmp("internal_encoding", type)) {
		RETVAL_STRING(get_internal_encoding());
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
		int persistent)
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
						php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): invalid multibyte sequence", self->from_charset, self->to_charset);
						goto out_failure;

					case EINVAL:
						if (ps != NULL) {
							if (icnt > 0) {
								if (self->stub_len >= sizeof(self->stub)) {
									php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): insufficient buffer", self->from_charset, self->to_charset);
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
							if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent))) {
								goto out_failure;
							}

							php_stream_bucket_append(buckets_out, new_bucket);

							out_buf_size = ocnt = initial_out_buf_size;
							if (NULL == (out_buf = pemalloc(out_buf_size, persistent))) {
								return FAILURE;
							}
							pd = out_buf;
						} else {
							if (NULL == (new_out_buf = perealloc(out_buf, new_out_buf_size, persistent))) {
								if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent))) {
									goto out_failure;
								}

								php_stream_bucket_append(buckets_out, new_bucket);
								return FAILURE;
							}
							pd = new_out_buf + (pd - out_buf);
							ocnt += (new_out_buf_size - out_buf_size);
							out_buf = new_out_buf;
							out_buf_size = new_out_buf_size;
						}
					} break;

					default:
						php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
						goto out_failure;
				}
#else
				if (ocnt == prev_ocnt) {
					php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
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
					php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): invalid multibyte sequence", self->from_charset, self->to_charset);
					goto out_failure;

				case EINVAL:
					if (ps != NULL) {
						if (icnt > sizeof(self->stub)) {
							php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): insufficient buffer", self->from_charset, self->to_charset);
							goto out_failure;
						}
						memcpy(self->stub, ps, icnt);
						self->stub_len = icnt;
						ps += icnt;
						icnt = 0;
					} else {
						php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unexpected octet values", self->from_charset, self->to_charset);
						goto out_failure;
					}
					break;

				case E2BIG: {
					char *new_out_buf;
					size_t new_out_buf_size;

					new_out_buf_size = out_buf_size << 1;

					if (new_out_buf_size < out_buf_size) {
						/* whoa! no bigger buckets are sold anywhere... */
						if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent))) {
							goto out_failure;
						}

						php_stream_bucket_append(buckets_out, new_bucket);

						out_buf_size = ocnt = initial_out_buf_size;
						if (NULL == (out_buf = pemalloc(out_buf_size, persistent))) {
							return FAILURE;
						}
						pd = out_buf;
					} else {
						if (NULL == (new_out_buf = perealloc(out_buf, new_out_buf_size, persistent))) {
							if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent))) {
								goto out_failure;
							}

							php_stream_bucket_append(buckets_out, new_bucket);
							return FAILURE;
						}
						pd = new_out_buf + (pd - out_buf);
						ocnt += (new_out_buf_size - out_buf_size);
						out_buf = new_out_buf;
						out_buf_size = new_out_buf_size;
					}
				} break;

				default:
					php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
					goto out_failure;
			}
#else
			if (ocnt == prev_ocnt) {
				php_error_docref(NULL, E_WARNING, "iconv stream filter (\"%s\"=>\"%s\"): unknown error", self->from_charset, self->to_charset);
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

	if (out_buf_size > ocnt) {
		if (NULL == (new_bucket = php_stream_bucket_new(stream, out_buf, (out_buf_size - ocnt), 1, persistent))) {
			goto out_failure;
		}
		php_stream_bucket_append(buckets_out, new_bucket);
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
		size_t *bytes_consumed, int flags)
{
	php_stream_bucket *bucket = NULL;
	size_t consumed = 0;
	php_iconv_stream_filter *self = (php_iconv_stream_filter *)Z_PTR(filter->abstract);

	while (buckets_in->head != NULL) {
		bucket = buckets_in->head;

		php_stream_bucket_unlink(bucket);

		if (php_iconv_stream_filter_append_bucket(self, stream, filter,
				buckets_out, bucket->buf, bucket->buflen, &consumed,
				php_stream_is_persistent(stream)) != SUCCESS) {
			goto out_failure;
		}

		php_stream_bucket_delref(bucket);
	}

	if (flags != PSFS_FLAG_NORMAL) {
		if (php_iconv_stream_filter_append_bucket(self, stream, filter,
				buckets_out, NULL, 0, &consumed,
				php_stream_is_persistent(stream)) != SUCCESS) {
			goto out_failure;
		}
	}

	if (bytes_consumed != NULL) {
		*bytes_consumed = consumed;
	}

	return PSFS_PASS_ON;

out_failure:
	if (bucket != NULL) {
		php_stream_bucket_delref(bucket);
	}
	return PSFS_ERR_FATAL;
}
/* }}} */

/* {{{ php_iconv_stream_filter_cleanup */
static void php_iconv_stream_filter_cleanup(php_stream_filter *filter)
{
	php_iconv_stream_filter_dtor((php_iconv_stream_filter *)Z_PTR(filter->abstract));
	pefree(Z_PTR(filter->abstract), ((php_iconv_stream_filter *)Z_PTR(filter->abstract))->persistent);
}
/* }}} */

static php_stream_filter_ops php_iconv_stream_filter_ops = {
	php_iconv_stream_filter_do_filter,
	php_iconv_stream_filter_cleanup,
	"convert.iconv.*"
};

/* {{{ php_iconv_stream_filter_create */
static php_stream_filter *php_iconv_stream_filter_factory_create(const char *name, zval *params, int persistent)
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
static php_iconv_err_t php_iconv_stream_filter_register_factory(void)
{
	static php_stream_filter_factory filter_factory = {
		php_iconv_stream_filter_factory_create
	};

	if (FAILURE == php_stream_filter_register_factory(
				php_iconv_stream_filter_ops.label,
				&filter_factory)) {
		return PHP_ICONV_ERR_UNKNOWN;
	}
	return PHP_ICONV_ERR_SUCCESS;
}
/* }}} */

/* {{{ php_iconv_stream_unregister_factory */
static php_iconv_err_t php_iconv_stream_filter_unregister_factory(void)
{
	if (FAILURE == php_stream_filter_unregister_factory(
				php_iconv_stream_filter_ops.label)) {
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
