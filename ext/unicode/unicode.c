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
#if HAVE_UNICODE
#include "zend_unicode.h"

void php_register_unicode_iterators(TSRMLS_D);

/* {{{ proto unicode unicode_decode(string input, string encoding) U
   Takes a string in the source encoding and converts it to a UTF-16 unicode string, returning the result */
static PHP_FUNCTION(unicode_decode)
{
	union {
		void *vptr;
		char *bin;
	} input;
	zend_uchar type;
	int len;
	char *encoding;
	int enclen;
	UErrorCode status;
	UConverter *conv = NULL;
	UChar *target;
	int targetlen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ts", &input.vptr, &len, &type, &encoding, &enclen)) {
		return;
	}

	if (type == IS_UNICODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "input string is already unicode");
		RETURN_FALSE;
	}

	status = U_ZERO_ERROR;
	conv = ucnv_open(encoding, &status);
	if (!conv) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not locate converter for %s", encoding);
		RETURN_FALSE;
	}

	status = U_ZERO_ERROR;
	zend_convert_to_unicode(conv, &target, &targetlen, input.bin, len, &status);
	if (U_FAILURE(status)) {
		/* TODO: error handling semantics ? */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "conversion was not entirely successful: %d", status);
	}
	RETVAL_UNICODEL(target, targetlen, 0);

	ucnv_close(conv);	
}
/* }}} */

/* {{{ proto bool unicode_semantics() U
   Check whether unicode semantics are enabled */
static PHP_FUNCTION(unicode_semantics)
{
	RETURN_BOOL(UG(unicode));
}
/* }}} */

/* {{{ proto string unicode_encode(unicode input, string encoding) U
   Takes a unicode string and converts it to a string in the specified encoding */
static PHP_FUNCTION(unicode_encode)
{
	UChar *uni;
	int len;
	char *encoding;
	int enclen;
	UErrorCode status;
	UConverter *conv = NULL;
	char *target;
	int targetlen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "us", &uni, &len, &encoding, &enclen)) {
		return;
	}

	status = U_ZERO_ERROR;
	conv = ucnv_open(encoding, &status);
	if (!conv) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not locate converter for %s", encoding);
		RETURN_FALSE;
	}

	status = U_ZERO_ERROR;
	zend_convert_from_unicode(conv, &target, &targetlen, uni, len, &status);
	if (U_FAILURE(status)) {
		/* TODO: error handling semantics ? */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "conversion was not entirely successful: %d", status);
	}
	RETVAL_STRINGL(target, targetlen, 0);

	ucnv_close(conv);	
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
	}

	zend_update_converters_error_behavior(TSRMLS_C);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool unicode_set_subst_char(int direction, string character) U
   Sets global substitution character for the specified conversion direction */
PHP_FUNCTION(unicode_set_subst_char)
{
	zend_conv_direction direction;
	UChar *subst_char;
	UChar32 cp;
	int subst_char_len;
	long tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lu", &tmp, &subst_char, &subst_char_len) == FAILURE) {
		return;
	}
	direction = (zend_conv_direction) tmp;

	if (direction != ZEND_FROM_UNICODE && direction != ZEND_TO_UNICODE) {
		php_error(E_WARNING, "Invalid conversion direction value");
		RETURN_FALSE;
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

	if (direction == ZEND_FROM_UNICODE) {
		int len;
		len = zend_codepoint_to_uchar(cp, UG(from_subst_char));
		UG(from_subst_char)[len] = 0;
	}

	zend_update_converters_error_behavior(TSRMLS_C);
	RETURN_TRUE;
}

/* {{{ unicode_functions[] */
zend_function_entry unicode_functions[] = {
	PHP_FE(i18n_loc_get_default, NULL)
	PHP_FE(i18n_loc_set_default, NULL)
	PHP_FE(unicode_decode, NULL)
	PHP_FE(unicode_semantics, NULL)
	PHP_FE(unicode_encode, NULL)
	PHP_FE(unicode_set_error_mode, NULL)
	PHP_FE(unicode_set_subst_char, NULL)
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
	if (php_stream_filter_register_factory("unicode.*", &php_unicode_filter_factory TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	php_register_unicode_iterators(TSRMLS_C);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(unicode)
{
	if (php_stream_filter_unregister_factory("unicode.*" TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}
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


#endif /* HAVE_UNICODE */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
