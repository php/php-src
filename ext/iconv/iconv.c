/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rui Hirokawa <louis@cityfujisawa.ne.jp>                     |
   |          Stig Bakken <ssb@fast.no>                                   |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_ICONV

#include <iconv.h>

#include "php_globals.h"
#include "php_iconv.h"
#include "ext/standard/info.h"
#include "ext/standard/php_output.h"
#include "SAPI.h"
#include "php_ini.h"


#if HAVE_LIBICONV
#define icv_open(a,b) libiconv_open(a,b)
#define icv_close(a) libiconv_close(a)
#define icv(a,b,c,d,e) libiconv(a,b,c,d,e)
#else
#define icv_open(a,b) iconv_open(a,b)
#define icv_close(a) iconv_close(a)
#define icv(a,b,c,d,e) iconv(a,b,c,d,e)
#endif


/* Every user visible function must have an entry in iconv_functions[].
*/
function_entry iconv_functions[] = {
    PHP_FE(iconv,									NULL)
    PHP_FE(ob_iconv_handler,						NULL)
    PHP_FE(iconv_get_encoding,						NULL)
    PHP_FE(iconv_set_encoding,						NULL)
	{NULL, NULL, NULL}	
};

zend_module_entry iconv_module_entry = {
	"iconv",
	iconv_functions,
	PHP_MINIT(miconv),
	PHP_MSHUTDOWN(miconv),
	NULL,
	NULL,
	PHP_MINFO(miconv),
	STANDARD_MODULE_PROPERTIES
};

ZEND_DECLARE_MODULE_GLOBALS(iconv)

#ifdef COMPILE_DL_ICONV
ZEND_GET_MODULE(iconv)
#endif

int php_iconv_string(char *, char **, char *, char *);

PHP_INI_BEGIN()
	 STD_PHP_INI_ENTRY("iconv.input_encoding",	  ICONV_INPUT_ENCODING,    PHP_INI_ALL, OnUpdateString,  input_encoding,	zend_iconv_globals,  iconv_globals)
	 STD_PHP_INI_ENTRY("iconv.output_encoding",	  ICONV_OUTPUT_ENCODING,   PHP_INI_ALL, OnUpdateString,  output_encoding,	zend_iconv_globals,  iconv_globals)
	 STD_PHP_INI_ENTRY("iconv.internal_encoding", ICONV_INTERNAL_ENCODING, PHP_INI_ALL, OnUpdateString,  internal_encoding,	zend_iconv_globals,  iconv_globals)
PHP_INI_END()

static void 
php_iconv_init_globals(zend_iconv_globals *iconv_globals)
{
	iconv_globals->input_encoding = NULL;
	iconv_globals->output_encoding = NULL;
	iconv_globals->internal_encoding = NULL;
}

PHP_MINIT_FUNCTION(miconv)
{
	ZEND_INIT_MODULE_GLOBALS(iconv, php_iconv_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(miconv)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

PHP_MINFO_FUNCTION(miconv)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "iconv support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

int php_iconv_string(char *in_p, char **out, char *in_charset, char *out_charset)
{
    unsigned int in_size, out_size;
    char *out_buffer, *out_p;
    iconv_t cd;
    size_t result;
    typedef unsigned int ucs4_t;

    in_size  = strlen(in_p) * sizeof(char) + 1;
    out_size = strlen(in_p) * sizeof(ucs4_t) + 1;

    out_buffer = (char *) emalloc(out_size);
	*out = out_buffer;
    out_p = out_buffer;
  
    cd = icv_open(out_charset, in_charset);
  
	if (cd == (iconv_t)(-1)) {
		php_error(E_WARNING, "iconv: cannot convert from `%s' to `%s'",
				  in_charset, out_charset);
		efree(out_buffer);
		return FAILURE;
	}
	
	result = icv(cd, (const char **) &in_p, &in_size, (char **)
				   &out_p, &out_size);

    if (result == (size_t)(-1)) {
		 efree(out_buffer);
		return FAILURE;
    }

    icv_close(cd);

    return SUCCESS;
}


/* {{{ proto string iconv(string in_charset, string out_charset, string str)
   Returns str converted to the out_charset character set */
PHP_FUNCTION(iconv)
{
    zval **in_charset, **out_charset, **in_buffer;
	char *out_buffer;
	
    if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &in_charset, &out_charset, &in_buffer) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(in_charset);
    convert_to_string_ex(out_charset);
    convert_to_string_ex(in_buffer);

	if (php_iconv_string(Z_STRVAL_PP(in_buffer), &out_buffer, Z_STRVAL_PP(in_charset), Z_STRVAL_PP(out_charset)) == SUCCESS) {
		RETVAL_STRING(out_buffer, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string ob_iconv_handler(string contents, int status)
   Returns str in output buffer converted to the iconv.output_encoding character set */
PHP_FUNCTION(ob_iconv_handler)
{
	char *out_buffer;
	zval **zv_string, **zv_status;
	SLS_FETCH();
	ICONVLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &zv_string, &zv_status)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(zv_string);
	convert_to_long_ex(zv_status);

	if (SG(sapi_headers).send_default_content_type &&
		php_iconv_string(Z_STRVAL_PP(zv_string), &out_buffer,
						 ICONVG(internal_encoding), 
						 ICONVG(output_encoding))==SUCCESS) {
		RETVAL_STRING(out_buffer, 0);
	} else {
		zval_dtor(return_value);
		*return_value = **zv_string;
		zval_copy_ctor(return_value);		
	}
	
}
/* }}} */

/* {{{ proto bool iconv_set_encoding(string type, string charset)
   Sets internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_set_encoding)
{
	zval **type, **charset;
	int argc = ZEND_NUM_ARGS();
	ICONVLS_FETCH();

	if (argc != 2 || zend_get_parameters_ex(2, &type, &charset) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(type);
	convert_to_string_ex(charset);

	if(!strcasecmp("input_encoding",Z_STRVAL_PP(type))) {
		if (ICONVG(input_encoding)) {
			free(ICONVG(input_encoding));
		}
		ICONVG(input_encoding) = estrndup(Z_STRVAL_PP(charset), Z_STRLEN_PP(charset));
	} else if(!strcasecmp("output_encoding",Z_STRVAL_PP(type))) {
		if (ICONVG(output_encoding)) {
			free(ICONVG(output_encoding));
		}
		ICONVG(output_encoding) = estrndup(Z_STRVAL_PP(charset), Z_STRLEN_PP(charset));
	} else if(!strcasecmp("internal_encoding",Z_STRVAL_PP(type))) {
		if (ICONVG(internal_encoding)) {
			free(ICONVG(internal_encoding));
		}
		ICONVG(internal_encoding) = estrndup(Z_STRVAL_PP(charset), Z_STRLEN_PP(charset));
	} else {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array iconv_get_encoding([string type])
   Get internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_get_encoding)
{
	zval **type;
	int argc = ZEND_NUM_ARGS();
	ICONVLS_FETCH();

	if (argc < 0 || argc > 1 || zend_get_parameters_ex(1, &type) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(type);

	if (argc == 0 || !strcasecmp("all",Z_STRVAL_PP(type))) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_string(return_value, "input_encoding", 
						 ICONVG(input_encoding), 1);
		add_assoc_string(return_value, "output_encoding", 
						 ICONVG(output_encoding), 1);
		add_assoc_string(return_value, "internal_encoding", 
						 ICONVG(internal_encoding), 1);
	} else if (!strcasecmp("input_encoding",Z_STRVAL_PP(type))) {
		RETVAL_STRING(ICONVG(input_encoding), 1);
	} else if (!strcasecmp("output_encoding",Z_STRVAL_PP(type))) {
		RETVAL_STRING(ICONVG(output_encoding), 1);
	} else if (!strcasecmp("internal_encoding",Z_STRVAL_PP(type))) {
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
 */
