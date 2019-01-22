/*
   +----------------------------------------------------------------------+
   | PHP Version 7														  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is		  |
   | available through the world-wide-web at the following url:			  |
   | http://www.php.net/license/3_01.txt								  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to		  |
   | license@php.net so we can mail you a copy immediately.				  |
   +----------------------------------------------------------------------+
   | Authors: Ed Batutis <ed@batutis.com>								  |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#if U_ICU_VERSION_MAJOR_NUM < 56
#include "unicode/unorm.h"
#else
#include <unicode/unorm2.h>
#endif
#include "normalizer.h"
#include "normalizer_class.h"
#include "normalizer_normalize.h"
#include "intl_convert.h"
#include <unicode/utf8.h>


#if U_ICU_VERSION_MAJOR_NUM >= 56
static const UNormalizer2 *intl_get_normalizer(zend_long form, UErrorCode *err)
{/*{{{*/
	switch (form)
	{
		case NORMALIZER_FORM_C:
			return unorm2_getNFCInstance(err);
			break;
		case NORMALIZER_FORM_D:
			return unorm2_getNFDInstance(err);
			break;
		case NORMALIZER_FORM_KC:
			return unorm2_getNFKCInstance(err);
			break;
		case NORMALIZER_FORM_KD:
			return unorm2_getNFKDInstance(err);
			break;
		case NORMALIZER_FORM_KC_CF:
			return unorm2_getNFKCCasefoldInstance(err);
			break;
	}

	*err = U_ILLEGAL_ARGUMENT_ERROR;
	return NULL;
}/*}}}*/

static int32_t intl_normalize(zend_long form, const UChar *src, int32_t src_len, UChar *dst, int32_t dst_len, UErrorCode *err)
{/*{{{*/
	const UNormalizer2 *norm;

	/* Mimic the behavior of ICU < 56. */
	if (UNEXPECTED(NORMALIZER_NONE == form)) {
		/* FIXME This is a noop which should be removed somewhen after PHP 7.3.*/
		zend_error(E_DEPRECATED, "Normalizer::NONE is obsolete with ICU 56 and above and will be removed in later PHP versions");

		if (dst_len >= src_len) {
			memmove(dst, src, sizeof(UChar) * src_len);
			dst[src_len] = '\0';
			*err = U_ZERO_ERROR;
			return src_len;
		}

		*err = U_BUFFER_OVERFLOW_ERROR;
		return -1;
	}

	norm = intl_get_normalizer(form, err);
	if(U_FAILURE(*err)) {
		return -1;
	}

	return unorm2_normalize(norm, src, src_len, dst, dst_len, err);
}/*}}}*/

static UBool intl_is_normalized(zend_long form, const UChar *uinput, int32_t uinput_len, UErrorCode *err)
{/*{{{*/
	const UNormalizer2 *norm = intl_get_normalizer(form, err);

	if(U_FAILURE(*err)) {
		return FALSE;
	}

	return unorm2_isNormalized(norm, uinput, uinput_len, err);
}/*}}}*/
#endif

/* {{{ proto string Normalizer::normalize( string $input [, string $form = FORM_C] )
 * Normalize a string. }}} */
/* {{{ proto string normalizer_normalize( string $input [, string $form = FORM_C] )
 * Normalize a string.
 */
PHP_FUNCTION( normalizer_normalize )
{
	char*			input = NULL;
	/* form is optional, defaults to FORM_C */
	zend_long	    form = NORMALIZER_DEFAULT;
	size_t			input_len = 0;

	UChar*			uinput = NULL;
	int32_t		    uinput_len = 0;
	int			    expansion_factor = 1;
	UErrorCode		status = U_ZERO_ERROR;

	UChar*			uret_buf = NULL;
	int32_t			uret_len = 0;

	zend_string*    u8str;

	int32_t			size_needed;

	intl_error_reset( NULL );

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "s|l",
				&input, &input_len, &form ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
						 "normalizer_normalize: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	expansion_factor = 1;

	switch(form) {
		case NORMALIZER_NONE:
			break;
		case NORMALIZER_FORM_D:
			expansion_factor = 3;
			break;
		case NORMALIZER_FORM_KD:
			expansion_factor = 3;
			break;
		case NORMALIZER_FORM_C:
		case NORMALIZER_FORM_KC:
#if U_ICU_VERSION_MAJOR_NUM >= 56
		case NORMALIZER_FORM_KC_CF:
#endif
			break;
		default:
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
						"normalizer_normalize: illegal normalization form", 0 );
			RETURN_FALSE;
	}

	/*
	 * Normalize string (converting it to UTF-16 first).
	 */

	/* First convert the string to UTF-16. */
	intl_convert_utf8_to_utf16(&uinput, &uinput_len, input, input_len, &status );

	if( U_FAILURE( status ) )
	{
		/* Set global error code. */
		intl_error_set_code( NULL, status );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 );
		if (uinput) {
			efree( uinput );
		}
		RETURN_FALSE;
	}


	/* Allocate memory for the destination buffer for normalization */
	uret_len = uinput_len * expansion_factor;
	uret_buf = eumalloc( uret_len + 1 );

	/* normalize */
#if U_ICU_VERSION_MAJOR_NUM < 56
	size_needed = unorm_normalize( uinput, uinput_len, form, (int32_t) 0 /* options */, uret_buf, uret_len, &status);
#else
	size_needed = intl_normalize(form, uinput, uinput_len, uret_buf, uret_len, &status);
#endif

	/* Bail out if an unexpected error occurred.
	 * (U_BUFFER_OVERFLOW_ERROR means that *target buffer is not large enough).
	 * (U_STRING_NOT_TERMINATED_WARNING usually means that the input string is empty).
	 */
	if( U_FAILURE(status) && status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING ) {
		efree( uret_buf );
		efree( uinput );
		RETURN_NULL();
	}

	if ( size_needed > uret_len ) {
		/* realloc does not seem to work properly - memory is corrupted
		 * uret_buf =  eurealloc(uret_buf, size_needed + 1);
		 */
		efree( uret_buf );
		uret_buf = eumalloc( size_needed + 1 );
		uret_len = size_needed;

		status = U_ZERO_ERROR;

		/* try normalize again */
#if U_ICU_VERSION_MAJOR_NUM < 56
		size_needed = unorm_normalize( uinput, uinput_len, form, (int32_t) 0 /* options */, uret_buf, uret_len, &status);
#else
		size_needed = intl_normalize(form, uinput, uinput_len, uret_buf, uret_len, &status);
#endif

		/* Bail out if an unexpected error occurred. */
		if( U_FAILURE(status)  ) {
			/* Set error messages. */
			intl_error_set_custom_msg( NULL,"Error normalizing string", 0 );
			efree( uret_buf );
			efree( uinput );
			RETURN_FALSE;
		}
	}

	efree( uinput );

	/* the buffer we actually used */
	uret_len = size_needed;

	/* Convert normalized string from UTF-16 to UTF-8. */
	u8str = intl_convert_utf16_to_utf8(uret_buf, uret_len, &status );
	efree( uret_buf );
	if( !u8str )
	{
		intl_error_set( NULL, status,
				"normalizer_normalize: error converting normalized text UTF-8", 0 );
		RETURN_FALSE;
	}

	/* Return it. */
	RETVAL_NEW_STR( u8str );
}
/* }}} */

/* {{{ proto bool Normalizer::isNormalized( string $input [, string $form = FORM_C] )
 * Test if a string is in a given normalization form. }}} */
/* {{{ proto bool normalizer_is_normalized( string $input [, string $form = FORM_C] )
 * Test if a string is in a given normalization form.
 */
PHP_FUNCTION( normalizer_is_normalized )
{
	char*	 	input = NULL;
	/* form is optional, defaults to FORM_C */
	zend_long		form = NORMALIZER_DEFAULT;
	size_t		input_len = 0;

	UChar*	 	uinput = NULL;
	int		uinput_len = 0;
	UErrorCode	status = U_ZERO_ERROR;

	UBool		uret = FALSE;

	intl_error_reset( NULL );

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "s|l",
				&input, &input_len, &form) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"normalizer_is_normalized: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	switch(form) {
		/* case NORMALIZER_NONE: not allowed - doesn't make sense */

		case NORMALIZER_FORM_D:
		case NORMALIZER_FORM_KD:
		case NORMALIZER_FORM_C:
		case NORMALIZER_FORM_KC:
#if U_ICU_VERSION_MAJOR_NUM >= 56
		case NORMALIZER_FORM_KC_CF:
#endif
			break;
		default:
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
						"normalizer_normalize: illegal normalization form", 0 );
			RETURN_FALSE;
	}


	/*
	 * Test normalization of string (converting it to UTF-16 first).
	 */

	/* First convert the string to UTF-16. */
	intl_convert_utf8_to_utf16(&uinput, &uinput_len, input, input_len, &status );

	if( U_FAILURE( status ) )
	{
		/* Set global error code. */
		intl_error_set_code( NULL, status );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting string to UTF-16.", 0 );
		if (uinput) {
			efree( uinput );
		}
		RETURN_FALSE;
	}


	/* test string */
#if U_ICU_VERSION_MAJOR_NUM < 56
	uret = unorm_isNormalizedWithOptions( uinput, uinput_len, form, (int32_t) 0 /* options */, &status);
#else
	uret = intl_is_normalized(form, uinput, uinput_len, &status);
#endif

	efree( uinput );

	/* Bail out if an unexpected error occurred. */
	if( U_FAILURE(status)  ) {
		/* Set error messages. */
		intl_error_set_custom_msg( NULL,"Error testing if string is the given normalization form.", 0 );
		RETURN_FALSE;
	}

	if ( uret )
		RETURN_TRUE;

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string|null Normalizer::getRawDecomposition( string $input [, string $form = FORM_C] )
 * Returns the Decomposition_Mapping property for the given UTF-8 encoded code point. }}} */
/* {{{ proto string|null normalizer_get_raw_decomposition( string $input [, string $form = FORM_C] )
 * Returns the Decomposition_Mapping property for the given UTF-8 encoded code point.
 */
#if U_ICU_VERSION_MAJOR_NUM >= 56
PHP_FUNCTION( normalizer_get_raw_decomposition )
{
	char* input = NULL;
	size_t input_length = 0;

	UChar32 codepoint = -1;
	int32_t offset = 0;

    UErrorCode status = U_ZERO_ERROR;
    const UNormalizer2 *norm;
    UChar decomposition[32];
    int32_t decomposition_length;

	zend_long form = NORMALIZER_DEFAULT;

	intl_error_reset(NULL);

	if ((zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &input, &input_length, &form) == FAILURE)) {
		return;
	}

	norm = intl_get_normalizer(form, &status);

	U8_NEXT(input, offset, input_length, codepoint);
	if ((size_t)offset != input_length) {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Input string must be exactly one UTF-8 encoded code point long.", 0);
		return;
	}

	if ((codepoint < UCHAR_MIN_VALUE) || (codepoint > UCHAR_MAX_VALUE)) {
		intl_error_set_code(NULL, U_ILLEGAL_ARGUMENT_ERROR);
		intl_error_set_custom_msg(NULL, "Code point out of range", 0);
		return;
	}

	decomposition_length = unorm2_getRawDecomposition(norm, codepoint, decomposition, 32, &status);
	if (decomposition_length == -1) {
		RETURN_NULL();
	}

	RETVAL_NEW_STR(intl_convert_utf16_to_utf8(decomposition, decomposition_length, &status));
}
#endif
/* }}} */
