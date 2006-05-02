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
  | Authors: Andrei Zmievski <andrei@php.net>                            |
  |          Wez Furlong <wez@php.net>                                   |
  +----------------------------------------------------------------------+
*/

/* $Id$ */ 

#include "php_unicode.h"
#include "zend_unicode.h"
#include "php_property.h"

void php_register_unicode_iterators(TSRMLS_D);

/* {{{ proto unicode unicode_decode(binary input, string encoding [, int flags]) U
   Takes a binary string converts it to a Unicode string using the specifed encoding */
static PHP_FUNCTION(unicode_decode)
{
	char *str, *enc;
	int str_len, enc_len;
	long flags;
	UChar *dest;
	int dest_len;
	UErrorCode status;
	UConverter *conv = NULL;
	int num_conv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Ss|l", &str, &str_len, &enc, &enc_len, &flags)) {
		return;
	}

	if (ZEND_NUM_ARGS() > 2) {
		if ((flags & 0xff) > ZEND_CONV_ERROR_LAST_ENUM) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "illegal value for conversion error mode");
			RETURN_FALSE;
		}
	} else {
		flags = UG(to_error_mode);
	}

	status = U_ZERO_ERROR;
	conv = ucnv_open(enc, &status);
	if (U_FAILURE(status)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not create converter for '%s' encoding", enc);
		RETURN_FALSE;
	}

	zend_set_converter_error_mode(conv, ZEND_TO_UNICODE, flags);

	status = U_ZERO_ERROR;
	num_conv = zend_convert_to_unicode(conv, &dest, &dest_len, str, str_len, &status);
	if (U_FAILURE(status)) {
		zend_raise_conversion_error_ex("could not decode binary string", conv, ZEND_TO_UNICODE, num_conv, (flags & ZEND_CONV_ERROR_EXCEPTION) TSRMLS_CC);
		efree(dest);
		ucnv_close(conv);	
		RETURN_FALSE;
	}
	ucnv_close(conv);	

	RETVAL_UNICODEL(dest, dest_len, 0);
}
/* }}} */

/* {{{ proto binary unicode_encode(unicode input, string encoding [, int flags]) U
   Takes a Unicode string and converts it to a binary string using the specified encoding */
static PHP_FUNCTION(unicode_encode)
{
	UChar *uni;
	char *enc;
	int uni_len, enc_len;
	long flags;
	char *dest;
	int dest_len;
	UErrorCode status;
	UConverter *conv = NULL;
	int num_conv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Us|l", &uni, &uni_len, &enc, &enc_len, &flags) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 2) {
		if ((flags & 0xff) > ZEND_CONV_ERROR_LAST_ENUM) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "illegal value for conversion error mode");
			RETURN_FALSE;
		}
	} else {
		flags = UG(from_error_mode);
	}

	status = U_ZERO_ERROR;
	conv = ucnv_open(enc, &status);
	if (U_FAILURE(status)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not create converter for '%s' encoding", enc);
		RETURN_FALSE;
	}

	zend_set_converter_error_mode(conv, ZEND_FROM_UNICODE, flags);
	zend_set_converter_subst_char(conv, UG(from_subst_char));

	status = U_ZERO_ERROR;
	num_conv = zend_convert_from_unicode(conv, &dest, &dest_len, uni, uni_len, &status);
	if (U_FAILURE(status)) {
		int32_t offset = u_countChar32(uni, num_conv);
		zend_raise_conversion_error_ex("could not encode Unicode string", conv, ZEND_FROM_UNICODE, offset, (flags & ZEND_CONV_ERROR_EXCEPTION) TSRMLS_CC);
		efree(dest);
		ucnv_close(conv);	
		RETURN_FALSE;
	}
	ucnv_close(conv);	

	RETVAL_STRINGL(dest, dest_len, 0);
}
/* }}} */

/* {{{ proto bool unicode_semantics() U
   Check whether unicode semantics are enabled */
static PHP_FUNCTION(unicode_semantics)
{
	RETURN_BOOL(UG(unicode));
}
/* }}} */

/* {{{ proto bool unicode_set_error_mode(int direction, int mode) U
   Sets global conversion error mode for the specified conversion direction */
PHP_FUNCTION(unicode_set_error_mode)
{
	zend_conv_direction direction;
	long tmp, mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &tmp, &mode) == FAILURE) {
		return;
	}
	direction = (zend_conv_direction) tmp;

	if (direction != ZEND_FROM_UNICODE && direction != ZEND_TO_UNICODE) {
		php_error(E_WARNING, "Invalid conversion direction value");
		RETURN_FALSE;
	}

	if ((mode & 0xff) > ZEND_CONV_ERROR_LAST_ENUM) {
		php_error(E_WARNING, "Illegal value for conversion error mode");
		RETURN_FALSE;
	}

	if (direction == ZEND_FROM_UNICODE) {
		UG(from_error_mode) = mode;
	} else {
		UG(to_error_mode)   = mode;
	}

	zend_update_converters_error_behavior(TSRMLS_C);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool unicode_set_subst_char(string character) U
   Sets global substitution character for conversion from Unicode to codepage */
PHP_FUNCTION(unicode_set_subst_char)
{
	UChar *subst_char;
	UChar32 cp;
	int subst_char_len, len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &subst_char, &subst_char_len) == FAILURE) {
		return;
	}

	if (subst_char_len < 1 ) {
		php_error(E_WARNING, "Empty substitution character");
		RETURN_FALSE;
	}

	cp = zend_get_codepoint_at(subst_char, subst_char_len, 0);

	if (cp < 0 || cp >= UCHAR_MAX_VALUE) {
		zend_error(E_WARNING, "Substitution character value U+%06x is out of range (0 - 0x10FFFF)", cp);
		RETURN_FALSE;
	}

	len = zend_codepoint_to_uchar(cp, UG(from_subst_char));
	UG(from_subst_char)[len] = 0;
	zend_update_converters_error_behavior(TSRMLS_C);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int unicode_get_error_mode(int direction) U
   Returns global conversion error mode for the specified conversion direction */
PHP_FUNCTION(unicode_get_error_mode)
{
	zend_conv_direction direction;
	long tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &tmp) == FAILURE) {
		return;
	}
	direction = (zend_conv_direction) tmp;

	if (direction == ZEND_FROM_UNICODE) {
		RETURN_LONG(UG(from_error_mode));
	} else if (direction == ZEND_TO_UNICODE) {
		RETURN_LONG(UG(to_error_mode));
	} else {
		php_error(E_WARNING, "Invalid conversion direction value");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string unicode_get_subst_char() U
   Returns global substitution character for conversion from Unicode to codepage */
PHP_FUNCTION(unicode_get_subst_char)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	RETURN_UNICODE(UG(from_subst_char), 1);
}
/* }}} */


/* {{{ unicode_functions[] */
zend_function_entry unicode_functions[] = {
	PHP_FE(locale_get_default, NULL)
	PHP_FE(locale_set_default, NULL)
	PHP_FE(unicode_decode, NULL)
	PHP_FE(unicode_semantics, NULL)
	PHP_FE(unicode_encode, NULL)
	PHP_FE(unicode_set_error_mode, NULL)
	PHP_FE(unicode_set_subst_char, NULL)
	PHP_FE(unicode_get_error_mode, NULL)
	PHP_FE(unicode_get_subst_char, NULL)
	PHP_FE(collator_create, NULL)
	PHP_FE(collator_compare, NULL)
	PHP_FE(collator_get_default, NULL)
	PHP_FE(collator_set_default, NULL)

	/* character property functions */
	PHP_FE(unicode_is_lower,  NULL)
	PHP_FE(unicode_is_upper,  NULL)
	PHP_FE(unicode_is_digit,  NULL)
	PHP_FE(unicode_is_alpha,  NULL)
	PHP_FE(unicode_is_alnum,  NULL)
	PHP_FE(unicode_is_xdigit, NULL)
	PHP_FE(unicode_is_punct,  NULL)
	PHP_FE(unicode_is_graph,  NULL)
	PHP_FE(unicode_is_blank,  NULL)
	PHP_FE(unicode_is_space,  NULL)
	PHP_FE(unicode_is_cntrl,  NULL)
	PHP_FE(unicode_is_print,  NULL)

	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ unicode_module_entry
 */
zend_module_entry unicode_module_entry = {
	STANDARD_MODULE_HEADER,
	"unicode",
	unicode_functions,
	PHP_MINIT(unicode),     /* Replace with NULL if there is nothing to do at php startup   */ 
	PHP_MSHUTDOWN(unicode), /* Replace with NULL if there is nothing to do at php shutdown  */
	PHP_RINIT(unicode),         /* Replace with NULL if there is nothing to do at request start */
	PHP_RSHUTDOWN(unicode),   /* Replace with NULL if there is nothing to do at request end   */
	PHP_MINFO(unicode),
	"1.0", 
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_UNICODE
ZEND_GET_MODULE(unicode)
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(unicode)
{
	php_register_unicode_iterators(TSRMLS_C);
	php_init_collation(TSRMLS_C);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(unicode)
{
	/* add your stuff here */

  
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(unicode)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(unicode)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(unicode)
{
	php_info_print_box_start(0);
	php_printf("ICU API extension\n");
	php_printf("Based on ICU library %s\n", U_COPYRIGHT_STRING);
	php_printf("ICU Version %s\n", U_ICU_VERSION);
	php_info_print_box_end();
	/* add your stuff here */

}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
