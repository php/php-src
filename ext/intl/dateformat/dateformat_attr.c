/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../php_intl.h"
#include "dateformat_class.h"
#include "../intl_convert.h"
#include "dateformat_class.h"
#include "dateformat_attr.h"

#include <unicode/ustring.h>
#include <unicode/udat.h>

/* {{{ proto unicode IntlDateFormatter::getDateType( )
 * Get formatter datetype. }}} */
/* {{{ proto string datefmt_get_datetype( IntlDateFormatter $mf )
 * Get formatter datetype.
 */
PHP_FUNCTION( datefmt_get_datetype )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_datetype: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	INTL_METHOD_CHECK_STATUS(dfo, "Error getting formatter datetype." );

	RETURN_LONG(dfo->date_type );
}
/* }}} */

/* {{{ proto unicode IntlDateFormatter::getTimeType( )
 * Get formatter timetype. }}} */
/* {{{ proto string datefmt_get_timetype( IntlDateFormatter $mf )
 * Get formatter timetype.
 */
PHP_FUNCTION( datefmt_get_timetype )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_timetype: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	INTL_METHOD_CHECK_STATUS(dfo, "Error getting formatter timetype." );

	RETURN_LONG(dfo->time_type );
}
/* }}} */

/* {{{ proto string IntlDateFormatter::getPattern( )
 * Get formatter pattern. }}} */
/* {{{ proto string datefmt_get_pattern( IntlDateFormatter $mf )
 * Get formatter pattern.
 */
PHP_FUNCTION( datefmt_get_pattern )
{
	UChar  value_buf[64];
	int    length = USIZE( value_buf );
	UChar* value  = value_buf;
	zend_bool   is_pattern_localized =FALSE;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_pattern: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	length = udat_toPattern(DATE_FORMAT_OBJECT(dfo), is_pattern_localized, value, length, &INTL_DATA_ERROR_CODE(dfo));
	if(INTL_DATA_ERROR_CODE(dfo) == U_BUFFER_OVERFLOW_ERROR && length >= USIZE( value_buf )) {
		++length; /* to avoid U_STRING_NOT_TERMINATED_WARNING */
		INTL_DATA_ERROR_CODE(dfo) = U_ZERO_ERROR;
		value = eumalloc(length);
		length = udat_toPattern(DATE_FORMAT_OBJECT(dfo), is_pattern_localized, value, length, &INTL_DATA_ERROR_CODE(dfo) );
		if(U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
			efree(value);
			value = value_buf;
		}
	}
	INTL_METHOD_CHECK_STATUS(dfo, "Error getting formatter pattern" );

	INTL_METHOD_RETVAL_UTF8( dfo, value, length, ( value != value_buf ) );
}
/* }}} */

/* {{{ proto bool IntlDateFormatter::setPattern( string $pattern )
 * Set formatter pattern. }}} */
/* {{{ proto bool datefmt_set_pattern( IntlDateFormatter $mf, string $pattern )
 * Set formatter pattern.
 */
PHP_FUNCTION( datefmt_set_pattern )
{
	char*       value = NULL;
	size_t      value_len = 0;
	int32_t     slength = 0;
	UChar*	    svalue  = NULL;
	zend_bool   is_pattern_localized =FALSE;


	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os",
		&object, IntlDateFormatter_ce_ptr,  &value, &value_len ) == FAILURE )
	{
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_set_pattern: unable to parse input params", 0);
		RETURN_FALSE;
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	/* Convert given pattern to UTF-16. */
	intl_convert_utf8_to_utf16(&svalue, &slength, value, value_len, &INTL_DATA_ERROR_CODE(dfo));
	INTL_METHOD_CHECK_STATUS(dfo, "Error converting pattern to UTF-16" );

	udat_applyPattern(DATE_FORMAT_OBJECT(dfo), (UBool)is_pattern_localized, svalue, slength);

	if (svalue) {
		efree(svalue);
	}
	INTL_METHOD_CHECK_STATUS(dfo, "Error setting symbol value");

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string IntlDateFormatter::getLocale()
 * Get formatter locale. }}} */
/* {{{ proto string datefmt_get_locale(IntlDateFormatter $mf)
 * Get formatter locale.
 */
PHP_FUNCTION( datefmt_get_locale )
{
	char *loc;
	zend_long  loc_type =ULOC_ACTUAL_LOCALE;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O|l",
		&object, IntlDateFormatter_ce_ptr,&loc_type) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_locale: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	loc = (char *)udat_getLocaleByType(DATE_FORMAT_OBJECT(dfo), loc_type,&INTL_DATA_ERROR_CODE(dfo));
	INTL_METHOD_CHECK_STATUS(dfo, "Error getting locale");
	RETURN_STRING(loc);
}
/* }}} */

/* {{{ proto string IntlDateFormatter::isLenient()
 * Get formatter isLenient. }}} */
/* {{{ proto string datefmt_isLenient(IntlDateFormatter $mf)
 * Get formatter locale.
 */
PHP_FUNCTION( datefmt_is_lenient )
{

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_is_lenient: unable to parse input params", 0 );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	RETVAL_BOOL(udat_isLenient(DATE_FORMAT_OBJECT(dfo)));
}
/* }}} */

/* {{{ proto string IntlDateFormatter::setLenient()
 * Set formatter lenient. }}} */
/* {{{ proto string datefmt_setLenient(IntlDateFormatter $mf)
 * Set formatter lenient.
 */
PHP_FUNCTION( datefmt_set_lenient )
{
	zend_bool isLenient  = FALSE;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ob",
	&object, IntlDateFormatter_ce_ptr,&isLenient ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_set_lenient: unable to parse input params", 0 );
		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	udat_setLenient(DATE_FORMAT_OBJECT(dfo), (UBool)isLenient );
}
/* }}} */
