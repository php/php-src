/*
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

#include <unicode/ustring.h>
#include <unicode/ucal.h>

#include "../intl_convert.h"
#include "../common/common_date.h"
#include "dateformat.h"
#include "dateformat_class.h"
#include "dateformat_data.h"

/* {{{ Internal function which calls the udat_format */
static void internal_format(IntlDateFormatter_object *dfo, UDate timestamp, zval *return_value)
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


/* {{{ Internal function which fetches an element from the passed array for the key_name passed */
static int32_t internal_get_arr_ele(IntlDateFormatter_object *dfo,
		HashTable* hash_arr, char* key_name, intl_error *err)
{
	zval	*ele_value	= NULL;
	int32_t	result		= 0;
	char	*message;

	if (U_FAILURE(err->code)) {
		return result;
	}

	if ((ele_value = zend_hash_str_find(hash_arr, key_name, strlen(key_name))) != NULL) {
		if(Z_TYPE_P(ele_value) != IS_LONG) {
			spprintf(&message, 0, "datefmt_format: parameter array contains "
					"a non-integer element for key '%s'", key_name);
			intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
			efree(message);
		} else {
			if (Z_LVAL_P(ele_value) > INT32_MAX ||
					Z_LVAL_P(ele_value) < INT32_MIN) {
				spprintf(&message, 0, "datefmt_format: value " ZEND_LONG_FMT " is out of "
						"bounds for a 32-bit integer in key '%s'",
						Z_LVAL_P(ele_value), key_name);
				intl_errors_set(err, U_ILLEGAL_ARGUMENT_ERROR, message, 1);
				efree(message);
			} else {
				result = Z_LVAL_P(ele_value);
			}
		}
	}

	return result;
}
/* }}} */

/* {{{ Internal function which sets UCalendar  from the passed array and retrieves timestamp */
static UDate internal_get_timestamp(IntlDateFormatter_object *dfo,
		HashTable *hash_arr)
{
	int32_t		year,
				month,
				hour,
				minute,
				second,
				mday;
	UCalendar	*pcal;
	UDate		result;
	intl_error	*err = &dfo->datef_data.error;

#define INTL_GET_ELEM(elem) \
	internal_get_arr_ele(dfo, hash_arr, (elem), err)

	/* Fetch  values from the incoming array */
	year	= INTL_GET_ELEM(CALENDAR_YEAR) + 1900; /* tm_year is years since 1900 */
	/* Month in ICU and PHP starts from January =0 */
	month	= INTL_GET_ELEM(CALENDAR_MON);
	hour	= INTL_GET_ELEM(CALENDAR_HOUR);
	minute	= INTL_GET_ELEM(CALENDAR_MIN);
	second	= INTL_GET_ELEM(CALENDAR_SEC);
	/* For the ucal_setDateTime() function, this is the 'date'  value */
	mday	= INTL_GET_ELEM(CALENDAR_MDAY);

#undef INTL_GET_ELEM

	pcal = ucal_clone(udat_getCalendar(DATE_FORMAT_OBJECT(dfo)),
			&INTL_DATA_ERROR_CODE(dfo));

	if (INTL_DATA_ERROR_CODE(dfo) != U_ZERO_ERROR) {
		intl_errors_set(err, INTL_DATA_ERROR_CODE(dfo), "datefmt_format: "
				"error cloning calendar", 0);
		return 0;
	}

	/* set the incoming values for the calendar */
	ucal_setDateTime(pcal, year, month, mday, hour, minute, second, &INTL_DATA_ERROR_CODE(dfo));
	/* actually, ucal_setDateTime cannot fail */

	/* Fetch the timestamp from the UCalendar */
	result = ucal_getMillis(pcal, &INTL_DATA_ERROR_CODE(dfo));
	ucal_close(pcal);
	return result;
}


/* {{{ Format the time value as a string. */
PHP_FUNCTION(datefmt_format)
{
	UDate 		timestamp	= 0;
	HashTable	*hash_arr	= NULL;
	zval		*zarg		= NULL;

	DATE_FORMAT_METHOD_INIT_VARS;

	/* Parse parameters. */
	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oz",
			&object, IntlDateFormatter_ce_ptr, &zarg) == FAILURE) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "datefmt_format: unable "
				"to parse input params", 0 );
		RETURN_THROWS();
	}

	DATE_FORMAT_METHOD_FETCH_OBJECT;

	if (Z_TYPE_P(zarg) == IS_ARRAY) {
		hash_arr = Z_ARRVAL_P(zarg);
		if (!hash_arr || zend_hash_num_elements(hash_arr) == 0) {
			RETURN_FALSE;
		}

		timestamp = internal_get_timestamp(dfo, hash_arr);
		INTL_METHOD_CHECK_STATUS(dfo, "datefmt_format: date formatting failed")
	} else {
		timestamp = intl_zval_to_millis(zarg, INTL_DATA_ERROR_P(dfo),
				"datefmt_format");
		if (U_FAILURE(INTL_DATA_ERROR_CODE(dfo))) {
			RETURN_FALSE;
		}
	}

	internal_format( dfo, timestamp, return_value);
}

/* }}} */
