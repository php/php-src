/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
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

#include "php.h"

#if HAVE_ICONV

#include "php_ini.h"
#include "php_iconv.h"


ZEND_DECLARE_MODULE_GLOBALS(iconv)

/* True global resources - no need for thread safety here */
static int le_iconv;

/* Every user visible function must have an entry in iconv_functions[].
*/
function_entry iconv_functions[] = {
    PHP_FE(iconv,									NULL)
    PHP_FE(ob_iconv_handler,						NULL)
    PHP_FE(iconv_set_encoding,						NULL)
	{NULL, NULL, NULL}	
};

zend_module_entry iconv_module_entry = {
	"iconv",
	iconv_functions,
	PHP_MINIT(iconv),
	PHP_MSHUTDOWN(iconv),
	NULL,
	NULL,
	PHP_MINFO(iconv),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ICONV
ZEND_GET_MODULE(iconv)
#endif

int php_iconv_string(char *, char **, char *, char *);


static PHP_INI_MH(OnUpdateIconvOutputEncoding)
{
	ICONVLS_FETCH();

	if (ICONVG(iconv_output_encoding)) {
		free(ICONVG(iconv_output_encoding));
	}
	ICONVG(iconv_output_encoding) = zend_strndup(new_value, new_value_length);
	return SUCCESS;
}

static PHP_INI_MH(OnUpdateIconvInternalEncoding)
{
	ICONVLS_FETCH();

	if (ICONVG(iconv_internal_encoding)) {
		free(ICONVG(iconv_internal_encoding));
	}
	ICONVG(iconv_internal_encoding) = zend_strndup(new_value, new_value_length);
	return SUCCESS;
}


PHP_INI_BEGIN()
	PHP_INI_ENTRY_EX("iconv.output_encoding",		ICONV_OUTPUT_ENCODING,		PHP_INI_SYSTEM,		OnUpdateIconvOutputEncoding,			NULL)
	PHP_INI_ENTRY_EX("iconv.internal_encoding",		ICONV_INTERNAL_ENCODING,		PHP_INI_SYSTEM,		OnUpdateIconvInternalEncoding,			NULL)
PHP_INI_END()


PHP_MINIT_FUNCTION(iconv)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(iconv)
{
/* Remove comments if you have entries in php.ini
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MINFO_FUNCTION(iconv)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "iconv support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

int php_iconv_string(char *in_p, char **out, char *in_charset, char *out_charset) {
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
  
    cd = iconv_open(out_charset, in_charset);
  
	if (cd == (iconv_t)(-1)) {
		php_error(E_WARNING, "iconv: cannot convert from `%s' to `%s'",
				  in_charset, out_charset);
		efree(out_buffer);
		return -1;
	}
	
	result = iconv(cd, (const char **) &in_p, &in_size, (char **)
				   &out_p, &out_size);

    if (result == (size_t)(-1)) {
        sprintf(out_buffer, "???") ;
		return -1;
    }

    iconv_close(cd);

    return SUCCESS;
}

/* {{{ proto string iconv(string in_charset, string out_charset, string str)
   Returns str converted to the out_charset character set */
PHP_FUNCTION(iconv)
{
    zval **in_charset, **out_charset, **in_buffer;
    unsigned int in_size, out_size;
    char *out_buffer, *in_p, *out_p;
    size_t result;
    typedef unsigned int ucs4_t;

    if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &in_charset, &out_charset, &in_buffer) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(in_charset);
    convert_to_string_ex(out_charset);
    convert_to_string_ex(in_buffer);

	if (php_iconv_string(Z_STRVAL_PP(in_buffer), &out_buffer, 
						 Z_STRVAL_PP(in_charset), Z_STRVAL_PP(out_charset)) == SUCCESS) {
		RETVAL_STRING(out_buffer, 0);
	} else {
		RETURN_FALSE;
	}
}

/* {{{ proto string ob_iconv_handler(string contents)
   Returns str in output buffer converted to the iconv.output_encoding character set */
PHP_FUNCTION(ob_iconv_handler)
{
	int coding;
	char *out_buffer;
	zval **zv_string;
	ICONVLS_FETCH();

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &zv_string)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (php_iconv_string(Z_STRVAL_PP(zv_string), &out_buffer,
						 ICONVG(iconv_internal_encoding), 
						 ICONVG(iconv_output_encoding))==SUCCESS) {
		RETVAL_STRING(out_buffer, 0);
	} else {
		zval_dtor(return_value);
		*return_value = **zv_string;
		zval_copy_ctor(return_value);		
	}
	
}

/* {{{ proto bool iconv_set_encoding(string int_charset, string out_charset)
   Sets internal encoding and output encoding for ob_iconv_handler() */
PHP_FUNCTION(iconv_set_encoding)
{
	zval **int_charset, **out_charset;
	ICONVLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &int_charset, &out_charset) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(int_charset);
	convert_to_string_ex(out_charset);

	if (ICONVG(iconv_internal_encoding)) {
		free(ICONVG(iconv_internal_encoding));
	}
	ICONVG(iconv_internal_encoding) = estrndup(Z_STRVAL_PP(int_charset), Z_STRLEN_PP(int_charset));

	if (ICONVG(iconv_output_encoding)) {
		free(ICONVG(iconv_output_encoding));
	}
	ICONVG(iconv_output_encoding) = estrndup(Z_STRVAL_PP(out_charset),Z_STRLEN_PP(out_charset));

	RETURN_TRUE;
}

/* }}} */
#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
