/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
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

/* {{{ iconv_functions[]
 */
function_entry iconv_functions[] = {
	PHP_NAMED_FE(iconv,php_if_iconv,				NULL)
	PHP_FE(ob_iconv_handler,						NULL)
	PHP_FE(iconv_get_encoding,						NULL)
	PHP_FE(iconv_set_encoding,						NULL)
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
	PHP_ICONV_ERR_UNKNOWN           = 6
} php_iconv_err_t;
/* }}} */

/* {{{ prototypes */ 
static php_iconv_err_t php_iconv_string(const char * in_p, size_t in_len, char **out, size_t *out_len, const char *in_charset, const char *out_charset);
static void _php_iconv_show_error(php_iconv_err_t err, const char *in_charset, const char *out_charset TSRMLS_DC);
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

/* {{{ php_iconv_string
 */
php_iconv_err_t php_iconv_string(const char *in_p, size_t in_len,
							char **out, size_t *out_len,
							const char *in_charset, const char *out_charset)
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

/* {{{ php_iconv_show_error() */
static void _php_iconv_show_error(php_iconv_err_t err, const char *in_charset, const char *out_charset TSRMLS_DC)
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

/* {{{ proto string iconv(string in_charset, string out_charset, string str)
   Returns str converted to the out_charset character set */
PHP_NAMED_FUNCTION(php_if_iconv)
{
	char *in_charset, *out_charset, *in_buffer, *out_buffer;
	unsigned int out_len;
	int in_charset_len, out_charset_len, in_buffer_len, err;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
		&in_charset, &in_charset_len, &out_charset, &out_charset_len, &in_buffer, &in_buffer_len) == FAILURE)
		return;

	err = php_iconv_string(in_buffer, in_buffer_len,
		&out_buffer,  &out_len, in_charset, out_charset);
	_php_iconv_show_error(err, in_charset, out_charset TSRMLS_CC); 
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
				ICONVG(internal_encoding), ICONVG(output_encoding));
		_php_iconv_show_error(err, ICONVG(internal_encoding), ICONVG(output_encoding) TSRMLS_CC);
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
