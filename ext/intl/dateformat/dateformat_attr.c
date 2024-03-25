/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kirti Velankar <kirtig@yahoo-inc.com>                       |
   +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../php_intl.h"
#include "dateformat_class.h"
#include "../intl_convert.h"
#include "dateformat_class.h"

#include <unicode/ustring.h>
#include <unicode/udat.h>

/* {{{ Get formatter datetype. */
PHP_FUNCTION( datefmt_get_datetype )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	INTL_METHOD_CHECK_STATUS(dfo, "Error getting formatter datetype." );

	RETURN_LONG(dfo->date_type );
}
/* }}} */

/* {{{ Get formatter timetype. */
PHP_FUNCTION( datefmt_get_timetype )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	INTL_METHOD_CHECK_STATUS(dfo, "Error getting formatter timetype." );

	RETURN_LONG(dfo->time_type );
}
/* }}} */

/* {{{ Get formatter pattern. */
PHP_FUNCTION( datefmt_get_pattern )
{
	UChar  value_buf[64];
	uint32_t    length = USIZE( value_buf );
	UChar* value  = value_buf;
	bool   is_pattern_localized = false;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
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

/* {{{ Set formatter pattern. */
PHP_FUNCTION( datefmt_set_pattern )
{
	char*       value = NULL;
	size_t      value_len = 0;
	int32_t     slength = 0;
	UChar*	    svalue  = NULL;
	bool   is_pattern_localized = false;


	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Os",
		&object, IntlDateFormatter_ce_ptr,  &value, &value_len ) == FAILURE )
	{
		RETURN_THROWS();
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

/* {{{ Get formatter locale. */
PHP_FUNCTION( datefmt_get_locale )
{
	char *loc;
	zend_long  loc_type =ULOC_ACTUAL_LOCALE;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O|l",
		&object, IntlDateFormatter_ce_ptr,&loc_type) == FAILURE )
	{

		RETURN_THROWS();
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	loc = (char *)udat_getLocaleByType(DATE_FORMAT_OBJECT(dfo), loc_type,&INTL_DATA_ERROR_CODE(dfo));
	INTL_METHOD_CHECK_STATUS(dfo, "Error getting locale");
	RETURN_STRING(loc);
}
/* }}} */

/* {{{ Get formatter isLenient. */
PHP_FUNCTION( datefmt_is_lenient )
{

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{

		RETURN_THROWS();
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	RETVAL_BOOL(udat_isLenient(DATE_FORMAT_OBJECT(dfo)));
}
/* }}} */

/* {{{ Set formatter lenient. */
PHP_FUNCTION( datefmt_set_lenient )
{
	bool isLenient = false;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "Ob",
	&object, IntlDateFormatter_ce_ptr,&isLenient ) == FAILURE )
	{
		RETURN_THROWS();
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	udat_setLenient(DATE_FORMAT_OBJECT(dfo), (UBool)isLenient );
}
/* }}} */
