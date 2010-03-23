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
#include <unicode/ucal.h>

#include "php_intl.h"
#include "intl_convert.h"
#include "dateformat.h"
#include "dateformat_class.h"
#include "dateformat_format.h"
#include "dateformat_data.h"

/* {{{ 
 * Internal function which calls the udat_format
*/
static void internal_format(IntlDateFormatter_object *dfo, UDate timestamp, zval *return_value TSRMLS_DC)
{
	UChar* 	formatted =  NULL;
	int32_t	resultlengthneeded =0 ;
	
	resultlengthneeded=udat_format( DATE_FORMAT_OBJECT(dfo), timestamp, NULL, resultlengthneeded, NULL, &INTL_DATA_ERROR_CODE(dfo));
	if(INTL_DATA_ERROR_CODE(dfo)==U_BUFFER_OVERFLOW_ERROR)
	{
		INTL_DATA_ERROR_CODE(dfo)=U_ZERO_ERROR;
		formatted=(UChar*)emalloc(sizeof(UChar) * resultlengthneeded); 
		udat_format( DATE_FORMAT_OBJECT(dfo), timestamp, formatted, resultlengthneeded, NULL, &INTL_DATA_ERROR_CODE(dfo));
	}

	if (formatted && U_FAILURE( INTL_DATA_ERROR_CODE(dfo) ) ) {
			efree(formatted);
	}

	INTL_METHOD_CHECK_STATUS( dfo, "Date formatting failed" );
	INTL_METHOD_RETVAL_UTF8( dfo, formatted, resultlengthneeded, 1 );

}
/* }}} */


/* {{{ 
 * Internal function which fetches an element from the passed array for the key_name passed 
*/
static double internal_get_arr_ele(IntlDateFormatter_object *dfo, HashTable* hash_arr, char* key_name TSRMLS_DC)
{
	zval**  ele_value       = NULL;
	UDate result = -1;

        if( zend_hash_find( hash_arr, key_name, strlen(key_name) + 1, (void **)&ele_value ) == SUCCESS ){
                if( Z_TYPE_PP(ele_value)!= IS_LONG ){
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"datefmt_format: parameter array does not contain a long element.", 0 TSRMLS_CC );
                }else{
			result =  Z_LVAL_PP(ele_value);
		}
	}
	/* printf("\n Inside internal_get_arr_ele key_name= %s, result = %g \n", key_name, result); */
	return result;
}
/* }}} */

/* {{{ 
 * Internal function which sets UCalendar  from the passed array and retrieves timestamp
*/
static UDate internal_get_timestamp(IntlDateFormatter_object *dfo, HashTable* hash_arr  TSRMLS_DC)
{
	long year =0;
	long month =0;
	long hour =0;
	long minute =0;
	long second =0;
	long wday =0;
	long yday =0;
	long mday =0;
	UBool isInDST = FALSE;
	UCalendar *pcal;

	/* Fetch  values from the incoming array */
	year = internal_get_arr_ele( dfo, hash_arr, CALENDAR_YEAR TSRMLS_CC) + 1900; /* tm_year is years since 1900 */
	/* Month in ICU and PHP starts from January =0 */
	month = internal_get_arr_ele( dfo, hash_arr, CALENDAR_MON TSRMLS_CC);
	hour = internal_get_arr_ele( dfo, hash_arr, CALENDAR_HOUR TSRMLS_CC);
	minute = internal_get_arr_ele( dfo, hash_arr, CALENDAR_MIN TSRMLS_CC);
	second = internal_get_arr_ele( dfo, hash_arr, CALENDAR_SEC TSRMLS_CC);
	wday = internal_get_arr_ele( dfo, hash_arr, CALENDAR_WDAY TSRMLS_CC);
	yday = internal_get_arr_ele( dfo, hash_arr, CALENDAR_YDAY TSRMLS_CC);
	isInDST = internal_get_arr_ele( dfo, hash_arr, CALENDAR_ISDST TSRMLS_CC);
	/* For the ucal_setDateTime() function, this is the 'date'  value */
	mday = internal_get_arr_ele( dfo, hash_arr, CALENDAR_MDAY TSRMLS_CC);

	pcal = udat_getCalendar(DATE_FORMAT_OBJECT(dfo));
	/* set the incoming values for the calendar */
	ucal_setDateTime( pcal, year, month, mday, hour, minute, second, &INTL_DATA_ERROR_CODE(dfo));
	if( INTL_DATA_ERROR_CODE(dfo) != U_ZERO_ERROR){
		return 0;
	}
	
	/* Fetch the timestamp from the UCalendar */
	return ucal_getMillis(pcal, &INTL_DATA_ERROR_CODE(dfo) );
}


/* {{{ proto string IntlDateFormatter::format( [mixed]int $args or array $args )
 * Format the time value as a string. }}}*/
/* {{{ proto string datefmt_format( [mixed]int $args or array $args )
 * Format the time value as a string. }}}*/
PHP_FUNCTION(datefmt_format) 
{
	UDate 		timestamp =0;
	UDate 		p_timestamp =0;
	HashTable*      hash_arr        = NULL;
	zval*		zarg	= NULL;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oz", &object, IntlDateFormatter_ce_ptr,&zarg ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "datefmt_format: unable to parse input params", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	/* Fetch the object. */
	DATE_FORMAT_METHOD_FETCH_OBJECT;

	switch(Z_TYPE_P(zarg) ){
		case IS_LONG:
			p_timestamp = Z_LVAL_P(zarg) ;
			timestamp = p_timestamp * 1000;
			break;
		case IS_DOUBLE:
			/* timestamp*1000 since ICU expects it in milliseconds */
			p_timestamp = Z_DVAL_P(zarg) ;
			timestamp = p_timestamp * 1000;
			break;
		case IS_ARRAY:
			hash_arr = Z_ARRVAL_P(zarg);
			if( !hash_arr || zend_hash_num_elements( hash_arr ) == 0 )
				RETURN_FALSE;

			timestamp = internal_get_timestamp(dfo, hash_arr TSRMLS_CC);
			INTL_METHOD_CHECK_STATUS( dfo, "datefmt_format: Date formatting failed" )
			break;
		default:
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				"datefmt_format: takes either an array  or an integer timestamp value ", 0 TSRMLS_CC );
			RETURN_FALSE;
	}

	internal_format( dfo, timestamp, return_value TSRMLS_CC);
	
}

/* }}} */

