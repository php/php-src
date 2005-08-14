/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
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

/* {{{ proto unicode unicode_decode(string input, string encoding)
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
	int32_t targetlen;

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

/* {{{ proto string unicode_encode(unicode input, string encoding)
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
	int32_t targetlen;

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
/* {{{ unicode_functions[] */
function_entry unicode_functions[] = {
	PHP_FE(icu_loc_get_default, NULL)
	PHP_FE(icu_loc_set_default, NULL)
	PHP_FE(unicode_decode, NULL)
	PHP_FE(unicode_encode, NULL)
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
	/* add your stuff here */

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
