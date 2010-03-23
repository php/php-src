/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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

#include <unicode/ustring.h>
#include <unicode/udat.h>
#include <unicode/ucal.h>

#include "php_intl.h"
#include "intl_convert.h"
#include "dateformat_class.h"
#include "dateformat.h"

/* {{{ dateformat_register_constants
 * Register constants common for the both (OO and procedural)
 * APIs.
 */
void dateformat_register_constants( INIT_FUNC_ARGS )
{
	if( IntlDateFormatter_ce_ptr == NULL) {
		zend_error(E_ERROR, "DateFormat class not defined");
		return;
	}

	#define DATEFORMATTER_EXPOSE_CONST(x) REGISTER_LONG_CONSTANT(#x, x, CONST_CS)
	#define DATEFORMATTER_EXPOSE_CLASS_CONST(x) zend_declare_class_constant_long( IntlDateFormatter_ce_ptr, ZEND_STRS( #x ) - 1, UDAT_##x TSRMLS_CC );
	#define DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST(name, value) zend_declare_class_constant_long( IntlDateFormatter_ce_ptr, ZEND_STRS( name ) - 1, value TSRMLS_CC );

	#define DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST(x) zend_declare_class_constant_long( IntlDateFormatter_ce_ptr, ZEND_STRS( #x ) - 1, UCAL_##x TSRMLS_CC );

	/* UDateFormatStyle constants */
	DATEFORMATTER_EXPOSE_CLASS_CONST( FULL );
	DATEFORMATTER_EXPOSE_CLASS_CONST( LONG );
	DATEFORMATTER_EXPOSE_CLASS_CONST( MEDIUM );
	DATEFORMATTER_EXPOSE_CLASS_CONST( SHORT );
	DATEFORMATTER_EXPOSE_CLASS_CONST( NONE );

/*
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "GREGORIAN", DATEF_GREGORIAN );
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "CUSTOMARY", DATEF_CUSTOMARY );
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "BUDDHIST", DATEF_BUDDHIST );
	DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST( "JAPANESE_IMPERIAL", DATEF_JAPANESE_IMPERIAL );
*/

	DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST( GREGORIAN );
	DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST( TRADITIONAL );

	#undef DATEFORMATTER_EXPOSE_UCAL_CLASS_CONST
	#undef DATEFORMATTER_EXPOSE_CUSTOM_CLASS_CONST
	#undef DATEFORMATTER_EXPOSE_CLASS_CONST
	#undef DATEFORMATTER_EXPOSE_CONST
}
/* }}} */

/* {{{ */
static void datefmt_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
    char*       locale;
	int         locale_len = 0;
	zval*       object;
    long        date_type = 0;
    long        time_type = 0;
    long        calendar = UCAL_GREGORIAN;
    char*       timezone_str = NULL;
    int         timezone_str_len = 0;
    char*       pattern_str = NULL;
    int         pattern_str_len = 0;
    UChar*      svalue = NULL;		/* UTF-16 pattern_str */
    int         slength = 0;
    UChar*      timezone_utf16 = NULL;		/* UTF-16 timezone_str */
    int         timezone_utf16_len = 0;
	UCalendar   ucal_obj = NULL;
	IntlDateFormatter_object* dfo;
	
	intl_error_reset( NULL TSRMLS_CC );
	object = return_value;
	/* Parse parameters. */
    if( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "sll|sls",
		&locale, &locale_len, &date_type, &time_type, &timezone_str, &timezone_str_len, &calendar,&pattern_str, &pattern_str_len ) == FAILURE )
    {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,	"datefmt_create: unable to parse input parameters", 0 TSRMLS_CC );
		zval_dtor(return_value);
		RETURN_NULL();
    }

	INTL_CHECK_LOCALE_LEN_OBJ(locale_len, return_value);
	DATE_FORMAT_METHOD_FETCH_OBJECT;
	/* Convert pattern (if specified) to UTF-16. */
	if( pattern_str && pattern_str_len>0 ){
		intl_convert_utf8_to_utf16(&svalue, &slength, pattern_str, pattern_str_len, &INTL_DATA_ERROR_CODE(dfo));
		INTL_CTOR_CHECK_STATUS(dfo, "datefmt_create: error converting pattern to UTF-16");
	}

	/* Convert pattern (if specified) to UTF-16. */
	if( timezone_str && timezone_str_len >0 ){
		intl_convert_utf8_to_utf16(&timezone_utf16, &timezone_utf16_len, timezone_str, timezone_str_len, &INTL_DATA_ERROR_CODE(dfo));
		INTL_CTOR_CHECK_STATUS(dfo, "datefmt_create: error converting timezone_str to UTF-16" );
	}

	if(locale_len == 0) {
		locale = INTL_G(default_locale);
	}

	if( pattern_str && pattern_str_len>0 ){
		DATE_FORMAT_OBJECT(dfo) = udat_open(UDAT_IGNORE, UDAT_IGNORE, locale, timezone_utf16, timezone_utf16_len, svalue, slength, &INTL_DATA_ERROR_CODE(dfo));
	} else {
		DATE_FORMAT_OBJECT(dfo) = udat_open(time_type, date_type, locale, timezone_utf16, timezone_utf16_len, svalue, slength, &INTL_DATA_ERROR_CODE(dfo));
	}

    /* Set the calendar if passed */
    if(!U_FAILURE(INTL_DATA_ERROR_CODE(dfo)) && calendar) {
		ucal_obj = ucal_open( timezone_utf16, timezone_utf16_len, locale, calendar, &INTL_DATA_ERROR_CODE(dfo) );
		if(!U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
			udat_setCalendar( DATE_FORMAT_OBJECT(dfo), ucal_obj );
		}
    }

	if(svalue)
	{
		efree(svalue);
	}
	if(timezone_utf16)
	{
		efree(timezone_utf16);
	}

	INTL_CTOR_CHECK_STATUS(dfo, "datefmt_create: date formatter creation failed");

	/* Set the class variables */
	dfo->date_type = date_type;
	dfo->time_type = time_type;
	dfo->calendar  = calendar;
	if( timezone_str && timezone_str_len > 0){
		dfo->timezone_id = estrndup( timezone_str, timezone_str_len);
	}
}
/* }}} */

/* {{{ proto IntlDateFormatter IntlDateFormatter::create(string $locale, long date_type, long time_type[, string $timezone_str, long $calendar, string $pattern] )
 * Create formatter. }}} */
/* {{{ proto IntlDateFormatter datefmt_create(string $locale, long date_type, long time_type[, string $timezone_str, long $calendar, string $pattern] )
 
 * Create formatter.
 */
PHP_FUNCTION( datefmt_create )
{
    object_init_ex( return_value, IntlDateFormatter_ce_ptr );
	datefmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void IntlDateFormatter::__construct(string $locale, long date_type, long time_type[, string $timezone_str, long $calendar, string $pattern])
 * IntlDateFormatter object constructor.
 */
PHP_METHOD( IntlDateFormatter, __construct )
{
	return_value = getThis();
	datefmt_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto int IntlDateFormatter::getErrorCode()
 * Get formatter's last error code. }}} */
/* {{{ proto int datefmt_get_error_code( IntlDateFormatter $nf )
 * Get formatter's last error code.
 */
PHP_FUNCTION( datefmt_get_error_code )
{
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_error_code: unable to parse input params", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	dfo = (IntlDateFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	/* Return formatter's last error code. */
	RETURN_LONG( INTL_DATA_ERROR_CODE(dfo) );
}
/* }}} */

/* {{{ proto string IntlDateFormatter::getErrorMessage( )
 * Get text description for formatter's last error code. }}} */
/* {{{ proto string datefmt_get_error_message( IntlDateFormatter $coll )
 * Get text description for formatter's last error code.
 */
PHP_FUNCTION( datefmt_get_error_message )
{
	char*                    message = NULL;
	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O",
		&object, IntlDateFormatter_ce_ptr ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"datefmt_get_error_message: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	dfo = (IntlDateFormatter_object *) zend_object_store_get_object( object TSRMLS_CC );

	/* Return last error message. */
	message = intl_error_get_message( INTL_DATA_ERROR_P(dfo) TSRMLS_CC );
	RETURN_STRING( message, 0);
}
/* }}} */
