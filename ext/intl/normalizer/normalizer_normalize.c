/*
   +----------------------------------------------------------------------+
   | PHP Version 5														  |
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
	/* form is optional, defaults to FORM_C */
	long			form = NORMALIZER_DEFAULT;
		
	UChar*			uinput = NULL;
	int			uinput_len = 0;
	int			expansion_factor = 1;
	UErrorCode		status = U_ZERO_ERROR;
		
	UChar*			uret_buf = NULL;
	int			uret_len = 0;
		
	int32_t			size_needed;
		
	NORMALIZER_METHOD_INIT_VARS

	intl_error_reset( NULL TSRMLS_CC );

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "u|l",
				&uinput, &uinput_len, &form ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
						 "normalizer_normalize: unable to parse input params", 1 TSRMLS_CC );

		RETURN_NULL();
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
						"normalizer_normalize: illegal normalization form", 1 TSRMLS_CC );
			RETURN_NULL();
	}

	/*
	 * Normalize string
	 */

	/* Allocate memory for the destination buffer for normalization */
	uret_len = uinput_len * expansion_factor;
	uret_buf = eumalloc( uret_len + 1 );

	/* normalize */
	size_needed = unorm_normalize( uinput, uinput_len, form, (int32_t) 0 /* options */, uret_buf, uret_len, &status);
	
	/* Bail out if an unexpected error occured.
	 * (U_BUFFER_OVERFLOW_ERROR means that *target buffer is not large enough).
	 * (U_STRING_NOT_TERMINATED_WARNING usually means that the input string is empty).
	 */
	if( U_FAILURE(status) && status != U_BUFFER_OVERFLOW_ERROR && status != U_STRING_NOT_TERMINATED_WARNING ) {
		efree( uret_buf );
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

		/* Bail out if an unexpected error occured. */
		if( U_FAILURE(status)  ) {
			/* Set error messages. */
			intl_error_set_custom_msg( NULL,"Error normalizing string", 1 TSRMLS_CC );
			efree( uret_buf );
			RETURN_NULL();
		}
	}

	/* the buffer we actually used */
	uret_len = size_needed;
	uret_buf[uret_len] = 0;
	RETURN_UNICODEL(uret_buf, uret_len, 0);
}
/* }}} */

/* {{{ proto bool Normalizer::isNormalized( string $input [, string $form = FORM_C] )
 * Test if a string is in a given normalization form. }}} */
/* {{{ proto bool normalizer_is_normalize( string $input [, string $form = FORM_C] )
 * Test if a string is in a given normalization form.
 */
PHP_FUNCTION( normalizer_is_normalized )
{
	/* form is optional, defaults to FORM_C */
	long		form = NORMALIZER_DEFAULT;

	UChar*	 	uinput = NULL;
	int		uinput_len = 0;
	UErrorCode	status = U_ZERO_ERROR;
		
	UBool		uret = FALSE;
		
	NORMALIZER_METHOD_INIT_VARS

	intl_error_reset( NULL TSRMLS_CC );

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "u|l",
				&uinput, &uinput_len, &form) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"normalizer_is_normalized: unable to parse input params", 1 TSRMLS_CC );

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
						"normalizer_normalize: illegal normalization form", 1 TSRMLS_CC );
			RETURN_NULL();
	}


	/*
	 * Test normalization of string 
	 */

	/* test string */
	uret = unorm_isNormalizedWithOptions( uinput, uinput_len, form, (int32_t) 0 /* options */, &status);
	
	/* Bail out if an unexpected error occured. */
	if( U_FAILURE(status)  ) {
		/* Set error messages. */
		intl_error_set_custom_msg( NULL,"Error testing if string is the given normalization form.", 1 TSRMLS_CC );
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
