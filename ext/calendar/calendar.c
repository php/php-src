/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo             <shane@caraveo.com>               | 
   |          Colin Viebrock            <colin@easydns.com>               |
   |          Hartmut Holzgraefe        <hartmut@six.de>                  |
   |          Wez Furlong               <wez@thebrainroom.com>            |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef PHP_WIN32
#define _WINNLS_
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_calendar.h"
#include "sdncal.h"

function_entry calendar_functions[] = {
	PHP_FE(jdtogregorian, NULL)
	PHP_FE(gregoriantojd, NULL)
	PHP_FE(jdtojulian, NULL)
	PHP_FE(juliantojd, NULL)
	PHP_FE(jdtojewish, NULL)
	PHP_FE(jewishtojd, NULL)
	PHP_FE(jdtofrench, NULL)
	PHP_FE(frenchtojd, NULL)
	PHP_FE(jddayofweek, NULL)
	PHP_FE(jdmonthname, NULL)
	PHP_FE(easter_date, NULL)
	PHP_FE(easter_days, NULL)
	PHP_FE(unixtojd,    NULL)
	PHP_FE(jdtounix,    NULL)
	PHP_FE(cal_to_jd,	NULL)
	PHP_FE(cal_from_jd,	NULL)
	PHP_FE(cal_days_in_month, NULL)
	PHP_FE(cal_info, NULL)
	{NULL, NULL, NULL}
};


zend_module_entry calendar_module_entry = {
	STANDARD_MODULE_HEADER,
	"calendar", 
	calendar_functions, 
	PHP_MINIT(calendar),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(calendar),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES,
};

#ifdef COMPILE_DL_CALENDAR
ZEND_GET_MODULE(calendar)
#endif

/* this order must match the conversion table below */
enum cal_name_type_t {
	CAL_GREGORIAN = 0,
	CAL_JULIAN,
	CAL_JEWISH,
	CAL_FRENCH,
	CAL_NUM_CALS
};
typedef long int (*cal_to_jd_func_t)(int month, int day, int year);
typedef void (*cal_from_jd_func_t)(long int jd, int* year, int* month, int* day);
typedef char* (*cal_as_string_func_t)(int year, int month, int day);

struct cal_entry_t	{
	char * name;
	char * symbol;
	cal_to_jd_func_t			to_jd;
	cal_from_jd_func_t			from_jd;
	int num_months;
	int max_days_in_month;
	char **					month_name_short;
	char **					month_name_long;
};
static struct cal_entry_t cal_conversion_table[CAL_NUM_CALS] = {
	{ "Gregorian", "CAL_GREGORIAN", GregorianToSdn, SdnToGregorian, 12, 31, MonthNameShort, MonthNameLong },
	{ "Julian", "CAL_JULIAN", JulianToSdn, SdnToJulian, 12, 31, MonthNameShort, MonthNameLong },
	{ "Jewish", "CAL_JEWISH", JewishToSdn, SdnToJewish, 13, 30, JewishMonthName, JewishMonthName },
	{ "French", "CAL_FRENCH", FrenchToSdn, SdnToFrench, 13, 30, FrenchMonthName, FrenchMonthName }
};

/* For jddayofweek */
enum	{ CAL_DOW_DAYNO, CAL_DOW_SHORT, CAL_DOW_LONG };
/* For jdmonthname */
enum	{ CAL_MONTH_GREGORIAN_SHORT, CAL_MONTH_GREGORIAN_LONG,
	CAL_MONTH_JULIAN_SHORT, CAL_MONTH_JULIAN_LONG, CAL_MONTH_JEWISH,
	CAL_MONTH_FRENCH };
	
PHP_MINIT_FUNCTION(calendar)
{
	REGISTER_LONG_CONSTANT("CAL_GREGORIAN", CAL_GREGORIAN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_JULIAN", CAL_JULIAN, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_JEWISH", CAL_JEWISH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_FRENCH", CAL_FRENCH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_NUM_CALS", CAL_NUM_CALS, CONST_CS|CONST_PERSISTENT);
	/* constants for jddayofweek */
	REGISTER_LONG_CONSTANT("CAL_DOW_DAYNO", CAL_DOW_DAYNO, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_DOW_SHORT", CAL_DOW_SHORT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_DOW_LONG",	CAL_DOW_LONG, CONST_CS|CONST_PERSISTENT);
	/* constants for jdmonthname */
	REGISTER_LONG_CONSTANT("CAL_MONTH_GREGORIAN_SHORT", CAL_MONTH_GREGORIAN_SHORT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_MONTH_GREGORIAN_LONG",	CAL_MONTH_GREGORIAN_LONG, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_MONTH_JULIAN_SHORT",	CAL_MONTH_JULIAN_SHORT, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_MONTH_JULIAN_LONG",		CAL_MONTH_JULIAN_LONG, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_MONTH_JEWISH",			CAL_MONTH_JEWISH, CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_MONTH_FRENCH",			CAL_MONTH_FRENCH, CONST_CS|CONST_PERSISTENT);
	/* constants for easter calculation */
	REGISTER_LONG_CONSTANT("CAL_EASTER_DEFAULT",			CAL_EASTER_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_EASTER_ROMAN",				CAL_EASTER_ROMAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_EASTER_ALWAYS_GREGORIAN",	CAL_EASTER_ALWAYS_GREGORIAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CAL_EASTER_ALWAYS_JULIAN",		CAL_EASTER_ALWAYS_JULIAN, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(calendar)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Calendar support", "enabled");
  php_info_print_table_end();
}

/* {{{ proto array cal_info(int calendar)
   Returns information about a particular calendar */
PHP_FUNCTION(cal_info)
{
	zval ** cal;
	zval * months, *smonths;
	int i;
	struct cal_entry_t * calendar;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &cal) != SUCCESS)	{
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(cal);
	if (Z_LVAL_PP(cal) < 0 || Z_LVAL_PP(cal) >= CAL_NUM_CALS)	{
		zend_error(E_WARNING, "%s(): invalid calendar ID %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(cal));
		RETURN_FALSE;
	}

	calendar = &cal_conversion_table[Z_LVAL_PP(cal)];
	array_init(return_value);

	MAKE_STD_ZVAL(months);
	MAKE_STD_ZVAL(smonths);
	array_init(months);
	array_init(smonths);
	
	for (i=1; i<= calendar->num_months; i++)	{
		add_index_string(months, i, calendar->month_name_long[i], 1);
		add_index_string(smonths, i, calendar->month_name_short[i], 1);
	}
	add_assoc_zval(return_value, "months", months);
	add_assoc_zval(return_value, "abbrevmonths", smonths);
	add_assoc_long(return_value, "maxdaysinmonth", calendar->max_days_in_month);
	add_assoc_string(return_value, "calname", calendar->name, 1);
	add_assoc_string(return_value, "calsymbol", calendar->symbol, 1);
	
}
/* }}} */

/* {{{ proto int cal_days_in_month(int calendar, int month, int year)
   Returns the number of days in a month for a given year and calendar */
PHP_FUNCTION(cal_days_in_month)
{
	zval ** cal, **month, **year;
	struct cal_entry_t * calendar;
	long sdn_start, sdn_next;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &cal, &month, &year) != SUCCESS)	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cal);
	convert_to_long_ex(month);
	convert_to_long_ex(year);

	if (Z_LVAL_PP(cal) < 0 || Z_LVAL_PP(cal) >= CAL_NUM_CALS)	{
		zend_error(E_WARNING, "%s(): invalid calendar ID %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(cal));
		RETURN_FALSE;
	}

	calendar = &cal_conversion_table[Z_LVAL_PP(cal)];
	
	sdn_start = calendar->to_jd(Z_LVAL_PP(year), Z_LVAL_PP(month), 1);
	
	sdn_next = calendar->to_jd(Z_LVAL_PP(year), 1 + Z_LVAL_PP(month), 1);

	if (sdn_next == 0)	{
		/* if invalid, try first month of the next year... */
		sdn_next = calendar->to_jd(Z_LVAL_PP(year) + 1, 1, 1);
	}
	
	RETURN_LONG(sdn_next - sdn_start);
}
/* }}} */

/* {{{ proto int cal_to_jd(int calendar, int month, int day, int year)
   Converts from a supported calendar to Julian Day Count */
PHP_FUNCTION(cal_to_jd)
{
	zval ** cal, **month, **day, **year;
	long jdate;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &cal, &month, &day, &year) != SUCCESS)	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(cal);
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(year);

	if (Z_LVAL_PP(cal) < 0 || Z_LVAL_PP(cal) >= CAL_NUM_CALS)	{
		zend_error(E_WARNING, "%s(): invalid calendar ID %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(cal));
		RETURN_FALSE;
	}

	jdate = cal_conversion_table[Z_LVAL_PP(cal)].to_jd(
			Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));
	RETURN_LONG(jdate);
}
/* }}} */

/* {{{ proto array cal_from_jd(int jd, int calendar)
   Converts from Julian Day Count to a supported calendar and return extended information */
PHP_FUNCTION(cal_from_jd)
{
	zval ** jd, ** cal;
	int month, day, year, dow;
	char date[16];
	struct cal_entry_t * calendar;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &jd, &cal) != SUCCESS)	{
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(jd);
	convert_to_long_ex(cal);

	if (Z_LVAL_PP(cal) < 0 || Z_LVAL_PP(cal) >= CAL_NUM_CALS)	{
		zend_error(E_WARNING, "%s(): invalid calendar ID %d", get_active_function_name(TSRMLS_C), Z_LVAL_PP(cal));
		RETURN_FALSE;
	}
	calendar = &cal_conversion_table[Z_LVAL_PP(cal)];

	array_init(return_value);

	calendar->from_jd(
			Z_LVAL_PP(jd),
			&year, &month, &day);

	sprintf(date, "%i/%i/%i", month, day, year);
	add_assoc_string(return_value, "date", date, 1);
	
	add_assoc_long(return_value, "month", month);
	add_assoc_long(return_value, "day", day);
	add_assoc_long(return_value, "year", year);

	/* day of week */
	dow = DayOfWeek(Z_LVAL_PP(jd));
	add_assoc_long(return_value, "dow", dow);
	add_assoc_string(return_value, "abbrevdayname", DayNameShort[dow], 1);
	add_assoc_string(return_value, "dayname", DayNameLong[dow], 1);
	/* month name */
	add_assoc_string(return_value, "abbrevmonth", calendar->month_name_short[month], 1);
	add_assoc_string(return_value, "monthname", calendar->month_name_long[month], 1);
}
/* }}} */

/* {{{ proto string jdtogregorian(int juliandaycount)
   Converts a julian day count to a gregorian calendar date */
PHP_FUNCTION(jdtogregorian)
{
	pval **julday;
	int year, month, day;
	char date[10];

	if (zend_get_parameters_ex(1, &julday) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(julday);
	SdnToGregorian(Z_LVAL_PP(julday), &year, &month, &day);
	sprintf(date, "%i/%i/%i", month, day, year);

	RETURN_STRING(date, 1);
}
/* }}} */

/* {{{ proto int gregoriantojd(int month, int day, int year)
   Converts a gregorian calendar date to julian day count */
 PHP_FUNCTION(gregoriantojd)
{
	pval **year, **month, **day;
	int jdate;

	if (zend_get_parameters_ex(3, &month, &day, &year) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(year);

	jdate = GregorianToSdn(Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));

	RETURN_LONG(jdate);
}
/* }}} */

/* {{{ proto string jdtojulian(int juliandaycount)
   Convert a julian day count to a julian calendar date */
 PHP_FUNCTION(jdtojulian) 
{
	pval **julday;
	int year, month, day;
	char date[10];

	if (zend_get_parameters_ex(1, &julday) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(julday);
	SdnToJulian(Z_LVAL_PP(julday), &year, &month, &day);
	sprintf(date, "%i/%i/%i", month, day, year);

	RETURN_STRING(date, 1);
}
/* }}} */

/* {{{ proto int juliantojd(int month, int day, int year)
   Converts a julian calendar date to julian day count */
 PHP_FUNCTION(juliantojd)
{
	pval **year, **month, **day;
	int jdate;

	if (zend_get_parameters_ex(3, &month, &day, &year) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(year);

	jdate = JulianToSdn(Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));

	RETURN_LONG(jdate);
}
/* }}} */

/* {{{ proto string jdtojewish(int juliandaycount)
   Converts a julian day count to a jewish calendar date */
 PHP_FUNCTION(jdtojewish) 
{
	pval **julday;
	int year, month, day;
	char date[10];

	if (zend_get_parameters_ex(1, &julday) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(julday);
	
	SdnToJewish(Z_LVAL_PP(julday), &year, &month, &day);
	sprintf(date, "%i/%i/%i", month, day, year);

	RETURN_STRING(date, 1);
}
/* }}} */

/* {{{ proto int jewishtojd(int month, int day, int year)
   Converts a jewish calendar date to a julian day count */
 PHP_FUNCTION(jewishtojd)
{
	pval **year, **month, **day;
	int jdate;

	if (zend_get_parameters_ex(3, &month, &day, &year) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(year);

	jdate = JewishToSdn(Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));

	RETURN_LONG(jdate);
}
/* }}} */

/* {{{ proto string jdtofrench(int juliandaycount)
   Converts a julian day count to a french republic calendar date */
 PHP_FUNCTION(jdtofrench)
{
	pval **julday;
	int year, month, day;
	char date[10];

	if (zend_get_parameters_ex(1, &julday) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(julday);
	
	SdnToFrench(Z_LVAL_PP(julday), &year, &month, &day);
	sprintf(date, "%i/%i/%i", month, day, year);

	RETURN_STRING(date, 1);
}
/* }}} */

/* {{{ proto int frenchtojd(int month, int day, int year)
   Converts a french republic calendar date to julian day count */
 PHP_FUNCTION(frenchtojd)
{
	pval **year, **month, **day;
	int jdate;

	if (zend_get_parameters_ex(3, &month, &day, &year) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(month);
	convert_to_long_ex(day);
	convert_to_long_ex(year);

	jdate = FrenchToSdn(Z_LVAL_PP(year), Z_LVAL_PP(month), Z_LVAL_PP(day));

	RETURN_LONG(jdate);
}
/* }}} */

/* {{{ proto mixed jddayofweek(int juliandaycount [, int mode])
   Returns name or number of day of week from julian day count */
 PHP_FUNCTION(jddayofweek)
{
	pval *julday, *mode;
	int day;
	char *daynamel, *daynames;
	int myargc=ZEND_NUM_ARGS(), mymode=0;
	
	if ((myargc < 1) || (myargc > 2) || (zend_get_parameters(ht, myargc, &julday, &mode) != SUCCESS)) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(julday);
	if(myargc==2) {
	  convert_to_long(mode);
	  mymode = Z_LVAL_P(mode);
	} 

	day = DayOfWeek(Z_LVAL_P(julday));
	daynamel = DayNameLong[day];
	daynames = DayNameShort[day];

		switch (mymode) {
			case CAL_DOW_SHORT:
				RETURN_STRING(daynamel, 1);
				break;
			case CAL_DOW_LONG:
				RETURN_STRING(daynames, 1);
				break;
			case CAL_DOW_DAYNO:
			default:
				RETURN_LONG(day);
				break;
		}
}
/* }}} */

/* {{{ proto string jdmonthname(int juliandaycount, int mode)
   Returns name of month for julian day count */
 PHP_FUNCTION(jdmonthname)
{
	pval **julday, **mode;
	char *monthname = NULL;
	int month, day, year;

	if (zend_get_parameters_ex(2, &julday, &mode) != SUCCESS) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(julday);
	convert_to_long_ex(mode);

	switch(Z_LVAL_PP(mode)) {
		case CAL_MONTH_GREGORIAN_LONG:			/* gregorian or julian month */
			SdnToGregorian(Z_LVAL_PP(julday), &year, &month, &day);
			monthname = MonthNameLong[month];
			break;
		case CAL_MONTH_JULIAN_SHORT:			/* gregorian or julian month */
			SdnToJulian(Z_LVAL_PP(julday), &year, &month, &day);
			monthname = MonthNameShort[month];
			break;
		case CAL_MONTH_JULIAN_LONG:			/* gregorian or julian month */
			SdnToJulian(Z_LVAL_PP(julday), &year, &month, &day);
			monthname = MonthNameLong[month];
			break;
		case CAL_MONTH_JEWISH:			/* jewish month */
			SdnToJewish(Z_LVAL_PP(julday), &year, &month, &day);
			monthname = JewishMonthName[month];
			break;
		case CAL_MONTH_FRENCH:			/* french month */
			SdnToFrench(Z_LVAL_PP(julday), &year, &month, &day);
			monthname = FrenchMonthName[month];
			break;
		default:			/* default gregorian */
		case CAL_MONTH_GREGORIAN_SHORT:			/* gregorian or julian month */
			SdnToGregorian(Z_LVAL_PP(julday), &year, &month, &day);
			monthname = MonthNameShort[month];
			break;
	}

	RETURN_STRING(monthname, 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
