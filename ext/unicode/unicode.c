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
#include "php_transform.h"

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
	num_conv = zend_string_to_unicode_ex(conv, &dest, &dest_len, str, str_len, &status);
	if (U_FAILURE(status)) {
		zend_raise_conversion_error_ex("could not decode binary string", conv, ZEND_TO_UNICODE, num_conv TSRMLS_CC);
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
	num_conv = zend_unicode_to_string_ex(conv, &dest, &dest_len, uni, uni_len, &status);
	if (U_FAILURE(status)) {
		int32_t offset = u_countChar32(uni, num_conv);
		zend_raise_conversion_error_ex("could not encode Unicode string", conv, ZEND_FROM_UNICODE, offset TSRMLS_CC);
		efree(dest);
		ucnv_close(conv);	
		RETURN_FALSE;
	}
	ucnv_close(conv);	

	RETVAL_STRINGL(dest, dest_len, 0);
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
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_UNICODE(UG(from_subst_char), 1);
}
/* }}} */

/* {{{ proto callback unicode_set_error_handler(callback new_callback) U
   Set (or clear) the custom Unicode conversion error handler */
PHP_FUNCTION(unicode_set_error_handler)
{
	zval	   *error_handler;
	zend_bool	had_orig_error_handler=0;
	zval		error_handler_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &error_handler) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(error_handler) != IS_NULL) { /* NULL == unset */
		if (!zend_is_callable(error_handler, 0, &error_handler_name TSRMLS_CC)) {
			zend_error(E_WARNING, "%v() expects the argument (%R) to be a valid callback",
					   get_active_function_name(TSRMLS_C), Z_TYPE(error_handler_name), Z_UNIVAL(error_handler_name));
			zval_dtor(&error_handler_name);
			return;
		}
		zval_dtor(&error_handler_name);
	}

	if (UG(conv_error_handler)) {
		had_orig_error_handler = 1;
		*return_value = *UG(conv_error_handler);
		zval_copy_ctor(return_value);
		zend_ptr_stack_push(&UG(conv_error_handlers), UG(conv_error_handler));
	}
	ALLOC_ZVAL(UG(conv_error_handler));

	if (Z_TYPE_P(error_handler) == IS_NULL) { /* unset user-defined handler */
		FREE_ZVAL(UG(conv_error_handler));
		UG(conv_error_handler) = NULL;
		zval_dtor(return_value);
		RETURN_TRUE;
	}

	*UG(conv_error_handler) = *error_handler;
	zval_copy_ctor(UG(conv_error_handler));

	if (!had_orig_error_handler) {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto bool unicode_restore_error_handler(void) U
   Restores the active error handler to the one which was previously active (before the last unicode_set_error_handler() call) */
PHP_FUNCTION(unicode_restore_error_handler)
{
	if (UG(conv_error_handler)) {
		zval *ceh = UG(conv_error_handler);

		UG(conv_error_handler) = NULL;
		zval_ptr_dtor(&ceh);
	}

	if (zend_ptr_stack_num_elements(&UG(conv_error_handlers))==0) {
		UG(conv_error_handler) = NULL;
	} else {
		UG(conv_error_handler) = zend_ptr_stack_pop(&UG(conv_error_handlers));
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ unicode_functions[] */
const zend_function_entry unicode_functions[] = {
	PHP_FE(locale_get_default, NULL)
	PHP_FE(locale_set_default, NULL)
	PHP_FE(unicode_decode, NULL)
	PHP_FE(unicode_encode, NULL)
	PHP_FE(unicode_set_error_handler, NULL)
	PHP_FE(unicode_restore_error_handler, NULL)
	PHP_FE(unicode_set_error_mode, NULL)
	PHP_FE(unicode_set_subst_char, NULL)
	PHP_FE(unicode_get_error_mode, NULL)
	PHP_FE(unicode_get_subst_char, NULL)
	PHP_FE(collator_create, NULL)
	PHP_FE(collator_compare, NULL)
	PHP_FE(collator_get_default, NULL)
	PHP_FE(collator_set_default, NULL)

	/* character property functions */
	PHP_FE(char_is_lower,  NULL)
	PHP_FE(char_is_upper,  NULL)
	PHP_FE(char_is_digit,  NULL)
	PHP_FE(char_is_alpha,  NULL)
	PHP_FE(char_is_alnum,  NULL)
	PHP_FE(char_is_xdigit, NULL)
	PHP_FE(char_is_punct,  NULL)
	PHP_FE(char_is_graph,  NULL)
	PHP_FE(char_is_blank,  NULL)
	PHP_FE(char_is_space,  NULL)
	PHP_FE(char_is_cntrl,  NULL)
	PHP_FE(char_is_print,  NULL)

	PHP_FE(char_is_defined, 		NULL)
	PHP_FE(char_is_id_start, 		NULL)
	PHP_FE(char_is_id_part, 		NULL)
	PHP_FE(char_is_id_ignorable,	NULL)
	PHP_FE(char_is_iso_control,		NULL)
	PHP_FE(char_is_mirrored, 		NULL)
	PHP_FE(char_is_base, 			NULL)
	PHP_FE(char_is_whitespace, 		NULL)
	PHP_FE(char_is_alphabetic, 		NULL)
	PHP_FE(char_is_uppercase, 		NULL)
	PHP_FE(char_is_lowercase, 		NULL)
	PHP_FE(char_is_titlecase,		NULL)

	PHP_FE(char_get_numeric_value, 		NULL)
	PHP_FE(char_get_digit_value, 		NULL)
	PHP_FE(char_get_combining_class,	NULL)
	PHP_FE(char_get_mirrored, 			NULL)
	PHP_FE(char_get_direction, 			NULL)
	PHP_FE(char_get_age,	 			NULL)
	PHP_FE(char_get_type,	 			NULL)
	PHP_FE(char_is_valid, 				NULL)

	PHP_FE(char_from_digit, 			NULL)
	PHP_FE(char_from_name, 				NULL)
	PHP_FE(char_get_name, 				NULL)
	PHP_FE(char_has_binary_property, 	NULL)
	PHP_FE(char_get_property_value, 	NULL)
	PHP_FE(char_get_property_min_value, NULL)
	PHP_FE(char_get_property_max_value, NULL)
	PHP_FE(char_get_property_name, 		NULL)
	PHP_FE(char_get_property_from_name,	NULL)
	PHP_FE(char_get_property_value_name, NULL)
	PHP_FE(char_get_property_value_from_name, NULL)

	PHP_FE(char_enum_names, NULL)
	PHP_FE(char_enum_types, NULL)

	/* text transformation functions */
	PHP_FE(str_transliterate, NULL)

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
	php_register_unicode_constants(TSRMLS_C);
	
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
	php_info_print_table_row(2, "ICU API extension", "enabled");
	php_info_print_table_row(2, "Based on ICU library", U_COPYRIGHT_STRING);
	php_info_print_table_row(2, "ICU Version", U_ICU_VERSION);
	php_info_print_box_end();
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
