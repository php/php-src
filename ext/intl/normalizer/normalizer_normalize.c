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
#include "unicode/unorm.h"
#include "normalizer.h"
#include "normalizer_class.h"
#include "normalizer_normalize.h"
#include "intl_convert.h"

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
	size_needed = unorm_normalize( uinput, uinput_len, form, (int32_t) 0 /* options */, uret_buf, uret_len, &status);

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
		size_needed = unorm_normalize( uinput, uinput_len, form, (int32_t) 0 /* options */, uret_buf, uret_len, &status);

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
/* {{{ proto bool normalizer_is_normalize( string $input [, string $form = FORM_C] )
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
	uret = unorm_isNormalizedWithOptions( uinput, uinput_len, form, (int32_t) 0 /* options */, &status);

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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
