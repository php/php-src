/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_streams.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_versioning.h"
#include "php_date.h"
#include "lib/timelib.h"
#include <time.h>
#include <unicode/udat.h>

/* {{{ arginfo */
static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmdate, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_idate, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strtotime, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, now)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_mktime, 0, 0, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, mon)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmmktime, 0, 0, 0)
	ZEND_ARG_INFO(0, hour)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, sec)
	ZEND_ARG_INFO(0, mon)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_checkdate, 0)
	ZEND_ARG_INFO(0, month)
	ZEND_ARG_INFO(0, day)
	ZEND_ARG_INFO(0, year)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_strftime, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_gmstrftime, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_time, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_localtime, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp)
	ZEND_ARG_INFO(0, associative_array)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_getdate, 0, 0, 0)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_default_timezone_set, 0)
	ZEND_ARG_INFO(0, timezone_identifier)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_default_timezone_get, 0)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sunrise, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
	ZEND_ARG_INFO(0, zenith)
	ZEND_ARG_INFO(0, gmt_offset)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO_EX(arginfo_date_sunset, 0, 0, 1)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
	ZEND_ARG_INFO(0, zenith)
	ZEND_ARG_INFO(0, gmt_offset)
ZEND_END_ARG_INFO()

static
ZEND_BEGIN_ARG_INFO(arginfo_date_sun_info, 0)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, latitude)
	ZEND_ARG_INFO(0, longitude)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ Function table */
zend_function_entry date_functions[] = {
	PHP_FE(strtotime, arginfo_strtotime)
	PHP_FE(date, arginfo_date)
	PHP_FE(idate, arginfo_idate)
	PHP_FE(gmdate, arginfo_gmdate)
	PHP_FE(mktime, arginfo_mktime)
	PHP_FE(gmmktime, arginfo_gmmktime)
	PHP_FE(checkdate, arginfo_checkdate)

#ifdef HAVE_STRFTIME
	PHP_FE(strftime, arginfo_strftime)
	PHP_FE(gmstrftime, arginfo_gmstrftime)
#endif

	PHP_FE(time, arginfo_time)
	PHP_FE(localtime, arginfo_localtime)
	PHP_FE(getdate, arginfo_getdate)

	/* Advanced Interface */
	PHP_FE(date_create, NULL)
	PHP_FE(date_parse, NULL)
	PHP_FE(date_format, NULL)
	PHP_FE(date_format_locale, NULL)
	PHP_FE(date_modify, NULL)
	PHP_FE(date_timezone_get, NULL)
	PHP_FE(date_timezone_set, NULL)
	PHP_FE(date_offset_get, NULL)

	PHP_FE(date_time_set, NULL)
	PHP_FE(date_date_set, NULL)
	PHP_FE(date_isodate_set, NULL)

	PHP_FE(timezone_open, NULL)
	PHP_FE(timezone_name_get, NULL)
	PHP_FE(timezone_name_from_abbr, NULL)
	PHP_FE(timezone_offset_get, NULL)
	PHP_FE(timezone_transistions_get, NULL)
	PHP_FE(timezone_identifiers_list, NULL)
	PHP_FE(timezone_abbreviations_list, NULL)

	/* Options and Configuration */
	PHP_FE(date_default_timezone_set, arginfo_date_default_timezone_set)
	PHP_FE(date_default_timezone_get, arginfo_date_default_timezone_get)

	/* Astronomical functions */
	PHP_FE(date_sunrise, arginfo_date_sunrise)
	PHP_FE(date_sunset, arginfo_date_sunset)
	PHP_FE(date_sun_info, arginfo_date_sun_info)
	{NULL, NULL, NULL}
};


zend_function_entry date_funcs_date[] = {
	ZEND_NAMED_FE(format, ZEND_FN(date_format), NULL)
	ZEND_NAMED_FE(modify, ZEND_FN(date_modify), NULL)
	ZEND_NAMED_FE(getTimezone, ZEND_FN(date_timezone_get), NULL)
	ZEND_NAMED_FE(setTimezone, ZEND_FN(date_timezone_set), NULL)
	ZEND_NAMED_FE(getOffset, ZEND_FN(date_offset_get), NULL)
	ZEND_NAMED_FE(setTime, ZEND_FN(date_time_set), NULL)
	ZEND_NAMED_FE(setDate, ZEND_FN(date_date_set), NULL)
	ZEND_NAMED_FE(setISODate, ZEND_FN(date_isodate_set), NULL)
	{NULL, NULL, NULL}
};

zend_function_entry date_funcs_timezone[] = {
	ZEND_NAMED_FE(getName, ZEND_FN(timezone_name_get), NULL)
	ZEND_NAMED_FE(getOffset, ZEND_FN(timezone_offset_get), NULL)
	ZEND_NAMED_FE(getTransistions, ZEND_FN(timezone_transistions_get), NULL)
	ZEND_FENTRY(listAbbreviations, zif_timezone_abbreviations_list, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FENTRY(listIdentifiers, zif_timezone_identifiers_list, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};

static char* guess_timezone(const timelib_tzdb *tzdb TSRMLS_DC);
static void date_register_classes(TSRMLS_D);
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(date)
static PHP_GINIT_FUNCTION(date);

/* True global */
timelib_tzdb *php_date_global_timezone_db;
int php_date_global_timezone_db_enabled;

#define DATE_DEFAULT_LATITUDE "31.7667"
#define DATE_DEFAULT_LONGITUDE "35.2333"

/* on 90'35; common sunset declaration (start of sun body appear) */
#define DATE_SUNSET_ZENITH "90.583333"

/* on 90'35; common sunrise declaration (sun body disappeared) */
#define DATE_SUNRISE_ZENITH "90.583333"

/* {{{ INI Settings */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("date.timezone", "", PHP_INI_ALL, OnUpdateString, default_timezone, zend_date_globals, date_globals)
	PHP_INI_ENTRY("date.default_latitude",           DATE_DEFAULT_LATITUDE,        PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.default_longitude",          DATE_DEFAULT_LONGITUDE,       PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunset_zenith",              DATE_SUNSET_ZENITH,           PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("date.sunrise_zenith",             DATE_SUNRISE_ZENITH,          PHP_INI_ALL, NULL)
PHP_INI_END()
/* }}} */

zend_class_entry *date_ce_date, *date_ce_timezone;

static zend_object_handlers date_object_handlers_date;
static zend_object_handlers date_object_handlers_timezone;

typedef struct _php_date_obj php_date_obj;
typedef struct _php_timezone_obj php_timezone_obj;

struct _php_date_obj {
	zend_object   std;
	timelib_time *time;
};

struct _php_timezone_obj {
	zend_object     std;
	timelib_tzinfo *tz;
};

#define DATE_SET_CONTEXT \
	zval *object; \
	object = getThis(); \
   
#define DATE_FETCH_OBJECT	\
	php_date_obj *obj;	\
	DATE_SET_CONTEXT; \
	if (object) {	\
		if (ZEND_NUM_ARGS()) {	\
			WRONG_PARAM_COUNT;	\
		}	\
	} else {	\
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "O", &object, date_ce_date) == FAILURE) {	\
			RETURN_FALSE;	\
		}	\
	}	\
	obj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);	\

static void date_object_free_storage_date(void *object TSRMLS_DC);
static void date_object_free_storage_timezone(void *object TSRMLS_DC);
static zend_object_value date_object_new_date(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value date_object_new_timezone(zend_class_entry *class_type TSRMLS_DC);

/* {{{ Module struct */
zend_module_entry date_module_entry = {
	STANDARD_MODULE_HEADER,
	"date",                     /* extension name */
	date_functions,             /* function list */
	PHP_MINIT(date),            /* process startup */
	PHP_MSHUTDOWN(date),        /* process shutdown */
	PHP_RINIT(date),            /* request startup */
	PHP_RSHUTDOWN(date),        /* request shutdown */
	PHP_MINFO(date),            /* extension info */
	PHP_VERSION,                /* extension version */
	PHP_MODULE_GLOBALS(date),   /* globals descriptor */
	PHP_GINIT(date),            /* globals ctor */
	NULL,                       /* globals dtor */
	NULL,                       /* post deactivate */
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */


/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(date)
{
	date_globals->default_timezone = NULL;
	date_globals->timezone = NULL;
}
/* }}} */


static void _php_date_tzinfo_dtor(void *tzinfo)
{
	timelib_tzinfo **tzi = (timelib_tzinfo **)tzinfo;

	timelib_tzinfo_dtor(*tzi);
}

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(date)
{
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
	}
	DATEG(timezone) = NULL;
	zend_hash_init(&DATEG(tzcache), 4, NULL, _php_date_tzinfo_dtor, 0);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(date)
{
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
	}
	DATEG(timezone) = NULL;
	zend_hash_destroy(&DATEG(tzcache));

	return SUCCESS;
}
/* }}} */

#define DATE_TIMEZONEDB      php_date_global_timezone_db ? php_date_global_timezone_db : timelib_builtin_db()

/*
 * RFC822, Section 5.1: http://www.ietf.org/rfc/rfc822.txt
 *  date-time   =  [ day "," ] date time        ; dd mm yy hh:mm:ss zzz
 *  day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"  /  "Fri"  / "Sat" /  "Sun"
 *  date        =  1*2DIGIT month 2DIGIT        ; day month year e.g. 20 Jun 82
 *  month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"  /  "May"  /  "Jun" /  "Jul"  /  "Aug"  /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
 *  time        =  hour zone                    ; ANSI and Military
 *  hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT] ; 00:00:00 - 23:59:59
 *  zone        =  "UT"  / "GMT"  /  "EST" / "EDT"  /  "CST" / "CDT"  /  "MST" / "MDT"  /  "PST" / "PDT"  /  1ALPHA  / ( ("+" / "-") 4DIGIT )
 */
#define DATE_FORMAT_RFC822   "D, d M y H:i:s O"

/*
 * RFC850, Section 2.1.4: http://www.ietf.org/rfc/rfc850.txt
 *  Format must be acceptable both to the ARPANET and to the getdate routine.
 *  One format that is acceptable to both is Weekday, DD-Mon-YY HH:MM:SS TIMEZONE
 *  TIMEZONE can be any timezone name (3 or more letters)
 */
#define DATE_FORMAT_RFC850   "l, d-M-y H:i:s T"

/*
 * RFC1036, Section 2.1.2: http://www.ietf.org/rfc/rfc1036.txt
 *  Its format must be acceptable both in RFC-822 and to the getdate(3)
 *  Wdy, DD Mon YY HH:MM:SS TIMEZONE
 *  There is no hope of having a complete list of timezones.  Universal
 *  Time (GMT), the North American timezones (PST, PDT, MST, MDT, CST,
 *  CDT, EST, EDT) and the +/-hhmm offset specifed in RFC-822 should be supported.
 */
#define DATE_FORMAT_RFC1036  "D, d M y H:i:s O"

/*
 * RFC1123, Section 5.2.14: http://www.ietf.org/rfc/rfc1123.txt
 *  RFC-822 Date and Time Specification: RFC-822 Section 5
 *  The syntax for the date is hereby changed to: date = 1*2DIGIT month 2*4DIGIT
 */
#define DATE_FORMAT_RFC1123  "D, d M Y H:i:s O"

/*
 * RFC2822, Section 3.3: http://www.ietf.org/rfc/rfc2822.txt
 *  FWS             =       ([*WSP CRLF] 1*WSP) /   ; Folding white space
 *  CFWS            =       *([FWS] comment) (([FWS] comment) / FWS)
 *  
 *  date-time       =       [ day-of-week "," ] date FWS time [CFWS]
 *  day-of-week     =       ([FWS] day-name)
 *  day-name        =       "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"
 *  date            =       day month year
 *  year            =       4*DIGIT
 *  month           =       (FWS month-name FWS)
 *  month-name      =       "Jan" / "Feb" / "Mar" / "Apr" / "May" / "Jun" / "Jul" / "Aug" / "Sep" / "Oct" / "Nov" / "Dec"
 *  day             =       ([FWS] 1*2DIGIT)
 *  time            =       time-of-day FWS zone
 *  time-of-day     =       hour ":" minute [ ":" second ]
 *  hour            =       2DIGIT
 *  minute          =       2DIGIT
 *  second          =       2DIGIT
 *  zone            =       (( "+" / "-" ) 4DIGIT)
 */
#define DATE_FORMAT_RFC2822  "D, d M Y H:i:s O"
/*
 * RFC3339, Section 5.6: http://www.ietf.org/rfc/rfc3339.txt
 *  date-fullyear   = 4DIGIT
 *  date-month      = 2DIGIT  ; 01-12
 *  date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on month/year
 *  
 *  time-hour       = 2DIGIT  ; 00-23
 *  time-minute     = 2DIGIT  ; 00-59
 *  time-second     = 2DIGIT  ; 00-58, 00-59, 00-60 based on leap second rules
 *  
 *  time-secfrac    = "." 1*DIGIT
 *  time-numoffset  = ("+" / "-") time-hour ":" time-minute
 *  time-offset     = "Z" / time-numoffset
 *  
 *  partial-time    = time-hour ":" time-minute ":" time-second [time-secfrac]
 *  full-date       = date-fullyear "-" date-month "-" date-mday
 *  full-time       = partial-time time-offset
 *  
 *  date-time       = full-date "T" full-time
 */
#define DATE_FORMAT_RFC3339  "Y-m-d\\TH:i:sP"

#define DATE_FORMAT_ISO8601  "Y-m-d\\TH:i:sO"

#define DATE_TZ_ERRMSG \
	"It is not safe to rely on the system's timezone settings. Please use " \
	"the date.timezone setting, the TZ environment variable or the " \
	"date_default_timezone_set() function. In case you used any of those " \
	"methods and you are still getting this warning, you most likely " \
	"misspelled the timezone identifier. "

#define SUNFUNCS_RET_TIMESTAMP 0
#define SUNFUNCS_RET_STRING    1
#define SUNFUNCS_RET_DOUBLE    2


/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(date)
{
	REGISTER_INI_ENTRIES();

	date_register_classes(TSRMLS_C);

/*
 * RFC4287, Section 3.3: http://www.ietf.org/rfc/rfc4287.txt
 *   A Date construct is an element whose content MUST conform to the
 *   "date-time" production in [RFC3339].  In addition, an uppercase "T"
 *   character MUST be used to separate date and time, and an uppercase
 *   "Z" character MUST be present in the absence of a numeric time zone offset.
 */
	REGISTER_STRING_CONSTANT("DATE_ATOM",    DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);
/*
 * Preliminary specification: http://wp.netscape.com/newsref/std/cookie_spec.html
 *   "This is based on RFC 822, RFC 850,  RFC 1036, and  RFC 1123,
 *   with the variations that the only legal time zone is GMT
 *   and the separators between the elements of the date must be dashes."
 */
	REGISTER_STRING_CONSTANT("DATE_COOKIE",  DATE_FORMAT_RFC850,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_ISO8601", DATE_FORMAT_ISO8601, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC822",  DATE_FORMAT_RFC822,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC850",  DATE_FORMAT_RFC850,  CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC1036", DATE_FORMAT_RFC1036, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC1123", DATE_FORMAT_RFC1123, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_RFC2822", DATE_FORMAT_RFC2822, CONST_CS | CONST_PERSISTENT);
 	REGISTER_STRING_CONSTANT("DATE_RFC3339", DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);
/*
 * RSS 2.0 Specification: http://blogs.law.harvard.edu/tech/rss
 *   "All date-times in RSS conform to the Date and Time Specification of RFC 822,
 *   with the exception that the year may be expressed with two characters or four characters (four preferred)"
 */
	REGISTER_STRING_CONSTANT("DATE_RSS",     DATE_FORMAT_RFC1123, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("DATE_W3C",     DATE_FORMAT_RFC3339, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_TIMESTAMP", SUNFUNCS_RET_TIMESTAMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_STRING", SUNFUNCS_RET_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SUNFUNCS_RET_DOUBLE", SUNFUNCS_RET_DOUBLE, CONST_CS | CONST_PERSISTENT);

	php_date_global_timezone_db = NULL;
	php_date_global_timezone_db_enabled = 0;
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(date)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(date)
{
	const timelib_tzdb *tzdb = DATE_TIMEZONEDB;
	
	php_info_print_table_start();
	php_info_print_table_row(2, "date/time support", "enabled");
	php_info_print_table_row(2, "Timezone Database Version", tzdb->version);
	php_info_print_table_row(2, "Timezone Database", php_date_global_timezone_db_enabled ? "external" : "internal");
	php_info_print_table_row(2, "Default timezone", guess_timezone(tzdb TSRMLS_CC));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Timezone Cache functions */
static timelib_tzinfo *php_date_parse_tzfile(char *formal_tzname, const timelib_tzdb *tzdb TSRMLS_DC)
{
	timelib_tzinfo *tzi, **ptzi;

	if (zend_hash_find(&DATEG(tzcache), formal_tzname, strlen(formal_tzname) + 1, (void **) &ptzi) == SUCCESS) {
		return *ptzi;
	}

	tzi = timelib_parse_tzfile(formal_tzname, tzdb);
	if (tzi) {
		zend_hash_add(&DATEG(tzcache), formal_tzname, strlen(formal_tzname) + 1, (void *) &tzi, sizeof(timelib_tzinfo*), NULL);
	}
	return tzi;
}
/* }}} */

/* {{{ Helper functions */
static char* guess_timezone(const timelib_tzdb *tzdb TSRMLS_DC)
{
	char *env;

	/* Checking configure timezone */
	if (DATEG(timezone) && (strlen(DATEG(timezone)) > 0)) {
		return DATEG(timezone);
	}
	/* Check environment variable */
	env = getenv("TZ");
	if (env && *env && timelib_timezone_id_is_valid(env, tzdb)) {
		return env;
	}
	/* Check config setting for default timezone */
	if (DATEG(default_timezone) && (strlen(DATEG(default_timezone)) > 0) && timelib_timezone_id_is_valid(DATEG(default_timezone), tzdb)) {
		return DATEG(default_timezone);
	}
#if HAVE_TM_ZONE
	/* Try to guess timezone from system information */
	{
		struct tm *ta, tmbuf;
		time_t     the_time;
		char      *tzid;
		
		the_time = time(NULL);
		ta = php_localtime_r(&the_time, &tmbuf);
		tzid = timelib_timezone_id_from_abbr(ta->tm_zone, ta->tm_gmtoff, ta->tm_isdst);
		if (! tzid) {
			tzid = "UTC";
		}
		
		php_error_docref(NULL TSRMLS_CC, E_STRICT, DATE_TZ_ERRMSG "We selected '%s' for '%s/%.1f/%s' instead", tzid, ta->tm_zone, (float) (ta->tm_gmtoff / 3600), ta->tm_isdst ? "DST" : "no DST");
		return tzid;
	}
#endif
#ifdef PHP_WIN32
	{
		char *tzid;
		TIME_ZONE_INFORMATION tzi;

		switch (GetTimeZoneInformation(&tzi))
		{
			/* no DST or not in effect */
			case TIME_ZONE_ID_UNKNOWN:
			case TIME_ZONE_ID_STANDARD:
php_win_std_time:
				tzid = timelib_timezone_id_from_abbr("", (tzi.Bias + tzi.StandardBias) * -60, 0);
				if (! tzid) {
					tzid = "UTC";
				}
				php_error_docref(NULL TSRMLS_CC, E_STRICT, DATE_TZ_ERRMSG "We selected '%s' for '%.1f/no DST' instead", tzid, ((tzi.Bias + tzi.StandardBias) / -60.0));
				break;

			/* DST in effect */
			case TIME_ZONE_ID_DAYLIGHT:
				/* If user has disabled DST in the control panel, Windows returns 0 here */
				if (tzi.DaylightBias == 0) {
					goto php_win_std_time;
				}
				
				tzid = timelib_timezone_id_from_abbr("", (tzi.Bias + tzi.DaylightBias) * -60, 1);
				if (! tzid) {
					tzid = "UTC";
				}
				php_error_docref(NULL TSRMLS_CC, E_STRICT, DATE_TZ_ERRMSG "We selected '%s' for '%.1f/DST' instead", tzid, ((tzi.Bias + tzi.DaylightBias) / -60.0));
				break;
		}
		return tzid;
	}
#endif
	/* Fallback to UTC */
	php_error_docref(NULL TSRMLS_CC, E_WARNING, DATE_TZ_ERRMSG "We had to select 'UTC' because your platform doesn't provide functionality for the guessing algorithm");
	return "UTC";
}

PHPAPI timelib_tzinfo *get_timezone_info(TSRMLS_D)
{
	char *tz;
	timelib_tzinfo *tzi;
	
	tz = guess_timezone(DATE_TIMEZONEDB TSRMLS_CC);
	tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB TSRMLS_CC);
	if (! tzi) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Timezone database is corrupt - this should *never* happen!");
	}
	return tzi;
}
/* }}} */


/* {{{ date() and gmdate() data */
#include "ext/standard/php_smart_str.h"

static char *mon_full_names[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};

static char *mon_short_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *day_full_names[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

static char *day_short_names[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static char *am_pm_lower_names[] = { "am", "pm" };
static char *am_pm_upper_names[] = { "AM", "PM" };

static char *english_suffix(timelib_sll number)
{
	if (number >= 10 && number <= 19) {
		return "th";
	} else {
		switch (number % 10) {
			case 1: return "st";
			case 2: return "nd";
			case 3: return "rd";
		}
	}
	return "th";
}
/* }}} */

/* {{{ day of week helpers */
char *php_date_full_day_name(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll day_of_week = timelib_day_of_week(y, m, d);
	if (day_of_week < 0) {
		return "Unknown";
	} 
	return day_full_names[day_of_week];	
}

char *php_date_short_day_name(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll day_of_week = timelib_day_of_week(y, m, d);
	if (day_of_week < 0) {
		return "Unknown";
	} 
	return day_short_names[day_of_week];	
}
/* }}} */

/* {{{ date_format - (gm)date helper */

typedef struct {
	int day_shortname_lengths[7];
	int day_fullname_lengths[7];
	int month_shortname_lengths[12];
	int month_fullname_lengths[12];
	int am_pm_lenghts[2];

	char* day_shortname[7];
	char* day_fullname[7];
	char* month_shortname[12];
	char* month_fullname[12];
	char* am_pm_name[2];
} php_locale_data;

static const UChar sLongPat [] = { 0x004D, 0x004D, 0x004D, 0x004D, 0x0020,
	    0x0079, 0x0079, 0x0079, 0x0079 };


#define DATE_LOC_READ(type, var, cor)                                          \
	count = udat_countSymbols(fmt, (type));                                    \
	for (i = 0 - (cor); i < count; i++) {                                      \
		array[i] = (UChar *) malloc(sizeof(UChar) * 15);                       \
		                                                                       \
		status = U_ZERO_ERROR;                                                 \
		j = udat_getSymbols(fmt, (type), i, array[i], 15, &status);            \
		                                                                       \
		tmp->var[i + (cor)] = array[i];                                        \
	}

static php_locale_data* date_get_locale_data(char *locale)
{
	php_locale_data *tmp = malloc(sizeof(php_locale_data));
	UDateFormat *fmt;
	UErrorCode status = 0;
	int32_t count, i, j;
	UChar *array[15];
	UChar *pat = sLongPat;
	int32_t len = 9;

	fmt = udat_open(UDAT_IGNORE,UDAT_IGNORE, locale, NULL, 0, pat, len, &status);

	DATE_LOC_READ(UDAT_WEEKDAYS, day_fullname, -1);
	DATE_LOC_READ(UDAT_SHORT_WEEKDAYS, day_shortname, -1);
	DATE_LOC_READ(UDAT_MONTHS, month_fullname, 0);
	DATE_LOC_READ(UDAT_SHORT_MONTHS, month_shortname, 0);
	DATE_LOC_READ(UDAT_AM_PMS, am_pm_name, 0);

	udat_close(fmt);

	return tmp;
}

static void date_free_locale_data(php_locale_data *data)
{
	int i;
	for (i = 0; i < 7; ++i) {
		free(data->day_shortname[i]);
		free(data->day_fullname[i]);
	}
	for (i = 0; i < 12; ++i) {
		free(data->month_shortname[i]);
		free(data->month_fullname[i]);
	}
	free(data->am_pm_name[0]);
	free(data->am_pm_name[1]);
	free(data);
}

static inline int date_spprintf(char **str, size_t size TSRMLS_DC, const char *format, ...)
{
	int      c;
	va_list  ap;

	va_start(ap, format);

	if (UG(unicode)) {
		c = vuspprintf(str, size, format, ap);
	} else {
		c = vspprintf(str, size, format, ap);
	}
	va_end(ap);
	return c;
}

#define dayname_short(s,l) s < 0 ? "Unknown" : (l ? loc_dat->day_shortname[s] : day_short_names[s])
#define dayname_full(s,l) s < 0 ? "Unknown" : (l ? loc_dat->day_fullname[s] : day_full_names[s])
#define monthname_short(s,l) l ? loc_dat->month_shortname[s] : mon_short_names[s]
#define monthname_full(s,l) l ? loc_dat->month_fullname[s] : mon_full_names[s]
#define am_pm_lower_full(s,l) l ? loc_dat->am_pm_name[s] : am_pm_lower_names[s]
#define am_pm_upper_full(s,l) l ? loc_dat->am_pm_name[s] : am_pm_upper_names[s]

static char *date_format(char *format, int format_len, int *return_len, timelib_time *t, int localtime, int localized TSRMLS_DC)
{
	smart_str            string = {0};
	int                  i, no_free, length;
	char                *buffer;
	timelib_time_offset *offset = NULL;
	timelib_sll          isoweek, isoyear;
	php_locale_data *loc_dat;
	int                  rfc_colon = 0;

	if (!format_len) {
		if (UG(unicode)) {
			return eustrdup(EMPTY_STR);
		} else {		
			return estrdup("");
		}
	}

	loc_dat = date_get_locale_data(UG(default_locale));

	if (localtime) {
		if (t->zone_type == TIMELIB_ZONETYPE_ABBR) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = strdup(t->tz_abbr);
		} else if (t->zone_type == TIMELIB_ZONETYPE_OFFSET) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = malloc(9); /* GMT±xxxx\0 */
			snprintf(offset->abbr, 9, "GMT%c%02d%02d", 
			                          localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
			                          localtime ? abs(offset->offset / 3600) : 0,
			                          localtime ? abs((offset->offset % 3600) / 60) : 0 );
		} else {
			offset = timelib_get_time_zone_info(t->sse, t->tz_info);
		}
	}
	timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear);

	for (i = 0; i < format_len; i++) {
		no_free = 0;
		switch (format[i]) {
			/* day */
			case 'd': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->d); break;
			case 'D': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, dayname_short(timelib_day_of_week(t->y, t->m, t->d), localized)); break;
			case 'j': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) t->d); break;
			case 'l': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, dayname_full(timelib_day_of_week(t->y, t->m, t->d), localized)); break;
			case 'S': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", english_suffix(t->d)); break;
			case 'w': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_day_of_week(t->y, t->m, t->d)); break;
			case 'N': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_iso_day_of_week(t->y, t->m, t->d)); break;
			case 'z': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_day_of_year(t->y, t->m, t->d)); break;

			/* week */
			case 'W': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) isoweek); break; /* iso weeknr */
			case 'o': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) isoyear); break; /* iso year */

			/* month */
			case 'F': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, monthname_full(t->m - 1, localized)); break;
			case 'm': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->m); break;
			case 'M': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, monthname_short(t->m - 1, localized)); break;
			case 'n': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) t->m); break;
			case 't': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) timelib_days_in_month(t->y, t->m)); break;

			/* year */
			case 'L': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", timelib_is_leap((int) t->y)); break;
			case 'y': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->y % 100); break;
			case 'Y': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%04d", (int) t->y); break;

			/* time */
			case 'a': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, am_pm_lower_full(t->h >= 12 ? 1 : 0, localized)); break;
			case 'A': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%R", localized ? IS_UNICODE : IS_STRING, am_pm_upper_full(t->h >= 12 ? 1 : 0, localized)); break;
			case 'B': {
				int retval = (((((long)t->sse)-(((long)t->sse) - ((((long)t->sse) % 86400) + 3600))) * 10) / 864);
				while (retval < 0) {
					retval += 1000;
				}
				retval = retval % 1000;
				length = date_spprintf(&buffer, 32 TSRMLS_CC, "%03d", retval);
				break;
			}
			case 'g': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'G': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", (int) t->h); break;
			case 'h': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'H': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->h); break;
			case 'i': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->i); break;
			case 's': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%02d", (int) t->s); break;

			/* timezone */
			case 'I': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", localtime ? offset->is_dst : 0); break;
 			case 'P': rfc_colon = 1; /* break intentionally missing */
			case 'O': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c%02d%s%02d",
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
 											rfc_colon ? ":" : "",
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'T': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", localtime ? offset->abbr : "GMT"); break;
			case 'e': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%s", localtime ? t->tz_info->name : "UTC"); break;
			case 'Z': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%d", localtime ? offset->offset : 0); break;

			/* full date/time */
			case 'c': length = date_spprintf(&buffer, 96 TSRMLS_CC, "%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
							                (int) t->y, (int) t->m, (int) t->d,
											(int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'r': length = date_spprintf(&buffer, 96 TSRMLS_CC, "%3s, %02d %3s %04d %02d:%02d:%02d %c%02d%02d",
							                php_date_short_day_name(t->y, t->m, t->d),
											(int) t->d, mon_short_names[t->m - 1],
											(int) t->y, (int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'U': length = date_spprintf(&buffer, 32 TSRMLS_CC, "%lld", (timelib_sll) t->sse); break;

			case '\\': if (i < format_len) i++; length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c", format[i]); break;

			default: length = date_spprintf(&buffer, 32 TSRMLS_CC, "%c", format[i]);
					 break;
		}
		smart_str_appendl(&string, buffer, length);
		if (!no_free) {
			efree(buffer);
		}
	}

	smart_str_0(&string);
	date_free_locale_data(loc_dat);

	if (localtime) {
		timelib_time_offset_dtor(offset);
	}

	if (UG(unicode)) {
		*return_len = string.len / 2;
	} else {
		*return_len = string.len;
	}
	return string.c;
}

static void php_date(INTERNAL_FUNCTION_PARAMETERS, int localtime)
{
	char   *format;
	int     format_len;
	long    ts;
	char   *string;

	if (ZEND_NUM_ARGS() == 1) {
		ts = time(NULL);
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}

	string = php_format_date(format, format_len, ts, localtime TSRMLS_CC);

	if (UG(unicode)) {
		RETVAL_UNICODE(string, 0);
	} else {
		RETVAL_STRING(string, 0);
	}
}
/* }}} */

PHPAPI char *php_format_date(char *format, int format_len, time_t ts, int localtime TSRMLS_DC) /* {{{ */
{
	timelib_time   *t;
	timelib_tzinfo *tzi;
	char           *string;
	int             return_len;

	t = timelib_time_ctor();

	if (localtime) {
		tzi = get_timezone_info(TSRMLS_C);
		t->tz_info = tzi;
		t->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(t, ts);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}

	string = date_format(format, format_len, &return_len, t, localtime, 0 TSRMLS_CC);
	
	timelib_time_dtor(t);
	return string;
}
/* }}} */

/* {{{ php_idate
 */
PHPAPI int php_idate(char format, time_t ts, int localtime)
{
	timelib_time   *t;
	timelib_tzinfo *tzi;
	int retval = -1;
	timelib_time_offset *offset = NULL;
	timelib_sll isoweek, isoyear;

	t = timelib_time_ctor();

	if (!localtime) {
		TSRMLS_FETCH();
		tzi = get_timezone_info(TSRMLS_C);
		t->tz_info = tzi;
		t->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(t, ts);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}

	if (!localtime) {
		if (t->zone_type == TIMELIB_ZONETYPE_ABBR) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = strdup(t->tz_abbr);
		} else if (t->zone_type == TIMELIB_ZONETYPE_OFFSET) {
			offset = timelib_time_offset_ctor();
			offset->offset = (t->z - (t->dst * 60)) * -60;
			offset->leap_secs = 0;
			offset->is_dst = t->dst;
			offset->abbr = malloc(9); /* GMT±xxxx\0 */
			snprintf(offset->abbr, 9, "GMT%c%02d%02d", 
			                          !localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
			                          !localtime ? abs(offset->offset / 3600) : 0,
			                          !localtime ? abs((offset->offset % 3600) / 60) : 0 );
		} else {
			offset = timelib_get_time_zone_info(t->sse, t->tz_info);
		}
	}

	timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear);

	switch (format) {
		/* day */
		case 'd': case 'j': retval = (int) t->d; break;

		case 'w': retval = (int) timelib_day_of_week(t->y, t->m, t->d); break;
		case 'z': retval = (int) timelib_day_of_year(t->y, t->m, t->d); break;

		/* week */
		case 'W': retval = (int) isoweek; break; /* iso weeknr */

		/* month */
		case 'm': case 'n': retval = (int) t->m; break;
		case 't': retval = (int) timelib_days_in_month(t->y, t->m); break;

		/* year */
		case 'L': retval = (int) timelib_is_leap((int) t->y); break;
		case 'y': retval = (int) (t->y % 100); break;
		case 'Y': retval = (int) t->y; break;

		/* Swatch Beat a.k.a. Internet Time */
		case 'B':
			retval = (((((long)t->sse)-(((long)t->sse) - ((((long)t->sse) % 86400) + 3600))) * 10) / 864);			
			while (retval < 0) {
				retval += 1000;
			}
			retval = retval % 1000;
			break;

		/* time */
		case 'g': case 'h': retval = (int) ((t->h % 12) ? (int) t->h % 12 : 12); break;
		case 'H': case 'G': retval = (int) t->h; break;
		case 'i': retval = (int) t->i; break;
		case 's': retval = (int) t->s; break;

		/* timezone */
		case 'I': retval = (int) (!localtime ? offset->is_dst : 0); break;
		case 'Z': retval = (int) (!localtime ? offset->offset : 0); break;

		case 'U': retval = (int) t->sse; break;
	}

	if (!localtime) {
		timelib_time_offset_dtor(offset);
	}
	timelib_time_dtor(t);

	return retval;
}
/* }}} */

/* {{{ proto string date(string format [, long timestamp])
   Format a local date/time */
PHP_FUNCTION(date)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto string gmdate(string format [, long timestamp])
   Format a GMT date/time */
PHP_FUNCTION(gmdate)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int idate(string format [, int timestamp])
   Format a local time/date as integer */
PHP_FUNCTION(idate)
{
	char   *format;
	int     format_len;
	long    ts;
	int ret; 

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}

	if (format_len != 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "idate format is one char");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 1) {
		ts = time(NULL);
	}

	ret = php_idate(format[0], ts, 0);
	if (ret == -1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unrecognized date format token.");
		RETURN_FALSE;
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ php_date_set_tzdb - NOT THREADSAFE */
PHPAPI void php_date_set_tzdb(timelib_tzdb *tzdb)
{
	const timelib_tzdb *builtin = timelib_builtin_db();
	
	if (php_version_compare(tzdb->version, builtin->version) > 0) {
		php_date_global_timezone_db = tzdb;
		php_date_global_timezone_db_enabled = 1;
	}
}
/* }}} */

/* {{{ php_parse_date: Backwards compability function */
PHPAPI signed long php_parse_date(char *string, signed long *now)
{
	timelib_time *parsed_time;
	int           error2;
	signed long   retval;

	parsed_time = timelib_strtotime(string, strlen(string), NULL, DATE_TIMEZONEDB);
	timelib_update_ts(parsed_time, NULL);
	retval = timelib_date_to_int(parsed_time, &error2);
	timelib_time_dtor(parsed_time);
	if (error2) {
		return -1;
	}
	return retval;
}
/* }}} */


/* {{{ proto int strtotime(string time [, int now ])
   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	char *times, *initial_ts;
	int   time_len, error1, error2;
	struct timelib_error_container *error;
	long  preset_ts, ts;

	timelib_time *t, *now;
	timelib_tzinfo *tzi;

	tzi = get_timezone_info(TSRMLS_C);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sl", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have an initial timestamp */
		now = timelib_time_ctor();

		initial_ts = emalloc(25);
		snprintf(initial_ts, 24, "@%ld", preset_ts);
		t = timelib_strtotime(initial_ts, strlen(initial_ts), NULL, DATE_TIMEZONEDB); /* we ignore the error here, as this should never fail */
		timelib_update_ts(t, tzi);
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, t->sse);
		timelib_time_dtor(t);
		efree(initial_ts);
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &times, &time_len) != FAILURE) {
		/* We have no initial timestamp */
		now = timelib_time_ctor();
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, (timelib_sll) time(NULL));
	} else {
		RETURN_FALSE;
	}

	if (!time_len) {
		timelib_time_dtor(now);	
		RETURN_FALSE;
	}

	t = timelib_strtotime(times, time_len, &error, DATE_TIMEZONEDB);
	error1 = error->error_count;
	timelib_error_container_dtor(error);
	timelib_fill_holes(t, now, 0);
	timelib_update_ts(t, tzi);
	ts = timelib_date_to_int(t, &error2);

	/* if tz_info is not a copy, avoid double free */
	if (now->tz_info != tzi && now->tz_info) {
		timelib_tzinfo_dtor(now->tz_info);
	}
	if (t->tz_info != tzi) {
		timelib_tzinfo_dtor(t->tz_info);
	}

	timelib_time_dtor(now);	
	timelib_time_dtor(t);

	if (error1 || error2) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ts);
	}
}
/* }}} */


/* {{{ php_mktime - (gm)mktime helper */
PHPAPI void php_mktime(INTERNAL_FUNCTION_PARAMETERS, int gmt)
{
	long hou, min, sec, mon, day, yea, dst = -1;
	timelib_time *now;
	timelib_tzinfo *tzi = NULL;
	long ts, adjust_seconds = 0;
	int error;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lllllll", &hou, &min, &sec, &mon, &day, &yea, &dst) == FAILURE) {
		RETURN_FALSE;
	}
	/* Initialize structure with current time */
	now = timelib_time_ctor();
	if (gmt) {
		timelib_unixtime2gmt(now, (timelib_sll) time(NULL));
	} else {
		tzi = get_timezone_info(TSRMLS_C);
		now->tz_info = tzi;
		now->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(now, (timelib_sll) time(NULL));
	}
	/* Fill in the new data */
	switch (ZEND_NUM_ARGS()) {
		case 7:
			/* break intentionally missing */
		case 6:
			if (yea >= 0 && yea < 70) {
				yea += 2000;
			} else if (yea >= 70 && yea <= 110) {
				yea += 1900;
			}
			now->y = yea;
			/* break intentionally missing again */
		case 5:
			now->d = day;
			/* break missing intentionally here too */
		case 4:
			now->m = mon;
			/* and here */
		case 3:
			now->s = sec;
			/* yup, this break isn't here on purpose too */
		case 2:
			now->i = min;
			/* last intentionally missing break */
		case 1:
			now->h = hou;
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_STRICT, "You should be using the time() function instead");
	}
	/* Update the timestamp */
	if (gmt) {
		timelib_update_ts(now, NULL);
	} else {
		timelib_update_ts(now, tzi);
	}
	/* Support for the deprecated is_dst parameter */
	if (dst != -1) {
		php_error_docref(NULL TSRMLS_CC, E_STRICT, "The is_dst parameter is deprecated");
		if (gmt) {
			/* GMT never uses DST */
			if (dst == 1) {
				adjust_seconds = -3600;
			}
		} else {
			/* Figure out is_dst for current TS */
			timelib_time_offset *tmp_offset;
			tmp_offset = timelib_get_time_zone_info(now->sse, tzi);
			if (dst == 1 && tmp_offset->is_dst == 0) {
				adjust_seconds = -3600;
			}
			if (dst == 0 && tmp_offset->is_dst == 1) {
				adjust_seconds = +3600;
			}
			timelib_time_offset_dtor(tmp_offset);
		}
	}
	/* Clean up and return */
	ts = timelib_date_to_int(now, &error);
	ts += adjust_seconds;
	timelib_time_dtor(now);

	if (error) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ts);
	}
}
/* }}} */

/* {{{ proto int mktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
   Get UNIX timestamp for a date */
PHP_FUNCTION(mktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
   Get UNIX timestamp for a GMT date */
PHP_FUNCTION(gmmktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto bool checkdate(int month, int day, int year)
   Returns true(1) if it is a valid date in gregorian calendar */
PHP_FUNCTION(checkdate)
{
	long m, d, y;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &m, &d, &y) == FAILURE) {
		RETURN_FALSE;
	}

	if (y < 1 || y > 32767 || m < 1 || m > 12 || d < 1 || d > timelib_days_in_month(y, m)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;	/* True : This month, day, year arguments are valid */
}
/* }}} */

#ifdef HAVE_STRFTIME
/* {{{ php_strftime - (gm)strftime helper */
PHPAPI void php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gmt)
{
	char                *format, *buf;
	int                  format_len;
	long                 timestamp;
	struct tm            ta;
	int                  max_reallocs = 5;
	size_t               buf_len = 64, real_len;
	timelib_time        *ts;
	timelib_tzinfo      *tzi;
	timelib_time_offset *offset = NULL;

	timestamp = (long) time(NULL);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	if (format_len == 0) {
		RETURN_FALSE;
	}

	ts = timelib_time_ctor();
	if (gmt) {
		tzi = NULL;
		timelib_unixtime2gmt(ts, (timelib_sll) timestamp);
	} else {
		tzi = get_timezone_info(TSRMLS_C);
		ts->tz_info = tzi;
		ts->zone_type = TIMELIB_ZONETYPE_ID;
		timelib_unixtime2local(ts, (timelib_sll) timestamp);
	}
	ta.tm_sec   = ts->s;
	ta.tm_min   = ts->i;
	ta.tm_hour  = ts->h;
	ta.tm_mday  = ts->d;
	ta.tm_mon   = ts->m - 1;
	ta.tm_year  = ts->y - 1900;
	ta.tm_wday  = timelib_day_of_week(ts->y, ts->m, ts->d);
	ta.tm_yday  = timelib_day_of_year(ts->y, ts->m, ts->d);
	if (gmt) {
		ta.tm_isdst = 0;
#if HAVE_TM_GMTOFF
		ta.tm_gmtoff = 0;
#endif
#if HAVE_TM_ZONE
		ta.tm_zone = "GMT";
#endif
	} else {
		offset = timelib_get_time_zone_info(timestamp, tzi);

		ta.tm_isdst = offset->is_dst;
#if HAVE_TM_GMTOFF
		ta.tm_gmtoff = offset->offset;
#endif
#if HAVE_TM_ZONE
		ta.tm_zone = offset->abbr;
#endif
	}

	buf = (char *) emalloc(buf_len);
	while ((real_len=strftime(buf, buf_len, format, &ta))==buf_len || real_len==0) {
		buf_len *= 2;
		buf = (char *) erealloc(buf, buf_len);
		if (!--max_reallocs) {
			break;
		}
	}

	timelib_time_dtor(ts);
	if (!gmt) {
		timelib_time_offset_dtor(offset);
	}

	if (real_len && real_len != buf_len) {
		buf = (char *) erealloc(buf, real_len + 1);
		if (UG(unicode)) {
			RETVAL_RT_STRINGL(buf, real_len, 1);
			efree(buf);
			return;
		} else {
			RETURN_STRINGL(buf, real_len, 0);
		}

	}
	efree(buf);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string strftime(string format [, int timestamp])
   Format a local time/date according to locale settings */
PHP_FUNCTION(strftime)
{
	php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string gmstrftime(string format [, int timestamp])
   Format a GMT/UCT time/date according to locale settings */
PHP_FUNCTION(gmstrftime)
{
	php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */
#endif

/* {{{ proto int time(void)
   Return current UNIX timestamp */
PHP_FUNCTION(time)
{
	RETURN_LONG((long)time(NULL));
}
/* }}} */

/* {{{ proto array localtime([int timestamp [, bool associative_array]])
   Returns the results of the C system call localtime as an associative array if the associative_array argument is set to 1 other wise it is a regular array */
PHP_FUNCTION(localtime)
{
	long timestamp = (long)time(NULL);
	int associative = 0;
	timelib_tzinfo *tzi;
	timelib_time   *ts;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lb", &timestamp, &associative) == FAILURE) {
		RETURN_FALSE;
	}

	tzi = get_timezone_info(TSRMLS_C);
	ts = timelib_time_ctor();
	ts->tz_info = tzi;
	ts->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(ts, (timelib_sll) timestamp);

	array_init(return_value);

	if (associative) {
		add_assoc_long(return_value, "tm_sec",   ts->s);
		add_assoc_long(return_value, "tm_min",   ts->i);
		add_assoc_long(return_value, "tm_hour",  ts->h);
		add_assoc_long(return_value, "tm_mday",  ts->d);
		add_assoc_long(return_value, "tm_mon",   ts->m - 1);
		add_assoc_long(return_value, "tm_year",  ts->y - 1900);
		add_assoc_long(return_value, "tm_wday",  timelib_day_of_week(ts->y, ts->m, ts->d));
		add_assoc_long(return_value, "tm_yday",  timelib_day_of_year(ts->y, ts->m, ts->d));
		add_assoc_long(return_value, "tm_isdst", ts->dst);
	} else {
		add_next_index_long(return_value, ts->s);
		add_next_index_long(return_value, ts->i);
		add_next_index_long(return_value, ts->h);
		add_next_index_long(return_value, ts->d);
		add_next_index_long(return_value, ts->m - 1);
		add_next_index_long(return_value, ts->y- 1900);
		add_next_index_long(return_value, timelib_day_of_week(ts->y, ts->m, ts->d));
		add_next_index_long(return_value, timelib_day_of_year(ts->y, ts->m, ts->d));
		add_next_index_long(return_value, ts->dst);
	}

	timelib_time_dtor(ts);
}
/* }}} */

/* {{{ proto array getdate([int timestamp])
   Get date/time information */
PHP_FUNCTION(getdate)
{
	long timestamp = (long)time(NULL);
	timelib_tzinfo *tzi;
	timelib_time   *ts;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	tzi = get_timezone_info(TSRMLS_C);
	ts = timelib_time_ctor();
	ts->tz_info = tzi;
	ts->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(ts, (timelib_sll) timestamp);

	array_init(return_value);

	add_assoc_long(return_value, "seconds", ts->s);
	add_assoc_long(return_value, "minutes", ts->i);
	add_assoc_long(return_value, "hours", ts->h);
	add_assoc_long(return_value, "mday", ts->d);
	add_assoc_long(return_value, "wday", timelib_day_of_week(ts->y, ts->m, ts->d));
	add_assoc_long(return_value, "mon", ts->m);
	add_assoc_long(return_value, "year", ts->y);
	add_assoc_long(return_value, "yday", timelib_day_of_year(ts->y, ts->m, ts->d));
	add_assoc_string(return_value, "weekday", php_date_full_day_name(ts->y, ts->m, ts->d), 1);
	add_assoc_string(return_value, "month", mon_full_names[ts->m - 1], 1);
	add_index_long(return_value, 0, timestamp);

	timelib_time_dtor(ts);
}
/* }}} */

static void date_register_classes(TSRMLS_D)
{
	zend_class_entry ce_date, ce_timezone;

	INIT_CLASS_ENTRY(ce_date, "date", date_funcs_date);
	ce_date.create_object = date_object_new_date;
	date_ce_date = zend_register_internal_class_ex(&ce_date, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_date, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_date.clone_obj = NULL;

#define REGISTER_DATE_CLASS_CONST_STRING(const_name, value) \
	zend_declare_class_constant_stringl(date_ce_date, const_name, sizeof(const_name)-1, value, sizeof(value)-1 TSRMLS_CC);

	REGISTER_DATE_CLASS_CONST_STRING("ATOM",    DATE_FORMAT_RFC3339);
	REGISTER_DATE_CLASS_CONST_STRING("COOKIE",  DATE_FORMAT_RFC850);
	REGISTER_DATE_CLASS_CONST_STRING("ISO8601", DATE_FORMAT_ISO8601);
	REGISTER_DATE_CLASS_CONST_STRING("RFC822",  DATE_FORMAT_RFC822);
	REGISTER_DATE_CLASS_CONST_STRING("RFC850",  DATE_FORMAT_RFC850);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1036", DATE_FORMAT_RFC1036);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1123", DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("RFC2822", DATE_FORMAT_RFC2822);
	REGISTER_DATE_CLASS_CONST_STRING("RFC3339", DATE_FORMAT_RFC3339);
	REGISTER_DATE_CLASS_CONST_STRING("RSS",     DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("W3C",     DATE_FORMAT_RFC3339);


	INIT_CLASS_ENTRY(ce_timezone, "timezone", date_funcs_timezone);
	ce_timezone.create_object = date_object_new_timezone;
	date_ce_timezone = zend_register_internal_class_ex(&ce_timezone, NULL, NULL TSRMLS_CC);
	memcpy(&date_object_handlers_timezone, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	date_object_handlers_timezone.clone_obj = NULL;
}

static zend_object_value date_object_new_date(zend_class_entry *class_type TSRMLS_DC)
{
	php_date_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_date_obj));
	memset(intern, 0, sizeof(php_date_obj));
	
	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_date, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_date;
	
	return retval;
}

static zend_object_value date_object_new_timezone(zend_class_entry *class_type TSRMLS_DC)
{
	php_timezone_obj *intern;
	zend_object_value retval;
	zval *tmp;

	intern = emalloc(sizeof(php_timezone_obj));
	memset(intern, 0, sizeof(php_timezone_obj));

	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_timezone, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_timezone;
	
	return retval;
}

static void date_object_free_storage_date(void *object TSRMLS_DC)
{
	php_date_obj *intern = (php_date_obj *)object;

	if (intern->time) {
		if (intern->time->tz_info) {
			timelib_tzinfo_dtor(intern->time->tz_info);
		}
		timelib_time_dtor(intern->time);
	}

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}

static void date_object_free_storage_timezone(void *object TSRMLS_DC)
{
	php_timezone_obj *intern = (php_timezone_obj *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);
	efree(object);
}

/* Advanced Interface */
static zval * date_instantiate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}

	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	object->refcount = 1;
	object->is_ref = 1;
	return object;
}

PHP_FUNCTION(date_create)
{
	php_date_obj   *dateobj;
	zval           *timezone_object = NULL;
	timelib_time   *now;
	timelib_tzinfo *tzi;
	char           *time_str;
	int             time_str_len = 0, free_tzi = 0;;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sO", &time_str, &time_str_len, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	date_instantiate(date_ce_date, return_value TSRMLS_CC);
	dateobj = (php_date_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	dateobj->time = timelib_strtotime(time_str_len ? time_str : "now", time_str_len ? time_str_len : sizeof("now") -1, NULL, DATE_TIMEZONEDB);

	if (timezone_object) {
		php_timezone_obj *tzobj;

		tzobj = (php_timezone_obj *) zend_object_store_get_object(timezone_object TSRMLS_CC);
		tzi = timelib_tzinfo_clone(tzobj->tz);
		free_tzi = 1;
	} else if (dateobj->time->tz_info) {
		tzi = timelib_tzinfo_clone(dateobj->time->tz_info);
		free_tzi = 1;
	} else {
		tzi = get_timezone_info(TSRMLS_C);
	}

	now = timelib_time_ctor();
	now->tz_info = tzi;
	now->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(now, (timelib_sll) time(NULL));

	timelib_fill_holes(dateobj->time, now, 0);
	timelib_update_ts(dateobj->time, tzi);

	if (now->tz_info != tzi) {
		timelib_tzinfo_dtor(now->tz_info);
	}
	if (free_tzi) {
		timelib_tzinfo_dtor(tzi);
	}
	timelib_time_dtor(now);	
}

PHP_FUNCTION(date_parse)
{
	char                           *date;
	int                             date_len, i;
	struct timelib_error_container *error;
	timelib_time                   *parsed_time;
	zval                           *element;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &date, &date_len) == FAILURE) {
		RETURN_FALSE;
	}

	parsed_time = timelib_strtotime(date, date_len, &error, DATE_TIMEZONEDB);
	array_init(return_value);
#define PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(name, elem) \
	if (parsed_time->elem == -1) {               \
		add_assoc_bool(return_value, #name, 0); \
	} else {                                       \
		add_assoc_long(return_value, #name, parsed_time->elem); \
	}
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(year,      y);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(month,     m);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(day,       d);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(hour,      h);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(minute,    i);
	PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(second,    s);
	
	if (parsed_time->f == -1) {
		add_assoc_bool(return_value, "fraction", 0);
	} else {
		add_assoc_double(return_value, "fraction", parsed_time->f);
	}

	add_assoc_long(return_value, "warning_count", error->warning_count);
	MAKE_STD_ZVAL(element);
	array_init(element);
	for (i = 0; i < error->warning_count; i++) {
		add_index_string(element, error->warning_messages[i].position, error->warning_messages[i].message, 1);
	}
	add_assoc_zval(return_value, "warnings", element);

	add_assoc_long(return_value, "error_count", error->error_count);
	MAKE_STD_ZVAL(element);
	array_init(element);
	for (i = 0; i < error->error_count; i++) {
		add_index_string(element, error->error_messages[i].position, error->error_messages[i].message, 1);
	}
	add_assoc_zval(return_value, "errors", element);
	timelib_error_container_dtor(error);

	add_assoc_bool(return_value, "is_localtime", parsed_time->is_localtime);

	if (parsed_time->is_localtime) {
		PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone_type, zone_type);
		switch (parsed_time->zone_type) {
			case TIMELIB_ZONETYPE_OFFSET:
				PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone, z);
				add_assoc_bool(return_value, "is_dst", parsed_time->dst);
				break;
			case TIMELIB_ZONETYPE_ID:
				if (parsed_time->tz_abbr) {
					add_assoc_string(return_value, "tz_abbr", parsed_time->tz_abbr, 1);
				}
				if (parsed_time->tz_info) {
					add_assoc_string(return_value, "tz_id", parsed_time->tz_info->name, 1);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				PHP_DATE_PARSE_DATE_SET_TIME_ELEMENT(zone, z);
				add_assoc_bool(return_value, "is_dst", parsed_time->dst);
				add_assoc_string(return_value, "tz_abbr", parsed_time->tz_abbr, 1);
				break;
		}
	}
	if (parsed_time->have_relative || parsed_time->have_weekday_relative) {
		MAKE_STD_ZVAL(element);
		array_init(element);
	}
	if (parsed_time->have_relative) {
		add_assoc_long(element, "year",   parsed_time->relative.y);
		add_assoc_long(element, "month",  parsed_time->relative.m);
		add_assoc_long(element, "day",    parsed_time->relative.d);
		add_assoc_long(element, "hour",   parsed_time->relative.h);
		add_assoc_long(element, "minute", parsed_time->relative.i);
		add_assoc_long(element, "second", parsed_time->relative.s);
	}
	if (parsed_time->have_weekday_relative) {
		add_assoc_long(element, "weekday", parsed_time->relative.weekday);
	}
	if (parsed_time->have_relative || parsed_time->have_weekday_relative) {
		add_assoc_zval(return_value, "relative", element);
	}
	timelib_time_dtor(parsed_time);
}
/* }}} */

PHP_FUNCTION(date_format)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format, *str;
	int           format_len, length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	str = date_format(format, format_len, &length, dateobj->time, dateobj->time->is_localtime, 0 TSRMLS_CC);
	if (UG(unicode)) {
		RETURN_UNICODEL((UChar*) str, length, 0);
	} else {
		RETURN_STRINGL(str, length, 0);
	}
}

PHP_FUNCTION(date_format_locale)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format, *str;
	int           format_len, length;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	str = date_format(format, format_len, &length, dateobj->time, dateobj->time->is_localtime, 1 TSRMLS_CC);
	if (UG(unicode)) {
		RETURN_UNICODEL((UChar*)str, length, 0);
	} else {
		RETURN_STRINGL(str, length, 0);
	}
}

PHP_FUNCTION(date_modify)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *modify;
	int           modify_len;
	timelib_time *tmp_time;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	tmp_time = timelib_strtotime(modify, modify_len, NULL, DATE_TIMEZONEDB);
	dateobj->time->relative.y = tmp_time->relative.y;
	dateobj->time->relative.m = tmp_time->relative.m;
	dateobj->time->relative.d = tmp_time->relative.d;
	dateobj->time->relative.h = tmp_time->relative.h;
	dateobj->time->relative.i = tmp_time->relative.i;
	dateobj->time->relative.s = tmp_time->relative.s;
	dateobj->time->relative.weekday = tmp_time->relative.weekday;
	dateobj->time->have_relative = tmp_time->have_relative;
	dateobj->time->have_weekday_relative = tmp_time->have_weekday_relative;
	dateobj->time->sse_uptodate = 0;
	timelib_time_dtor(tmp_time);

	timelib_update_ts(dateobj->time, NULL);
	timelib_update_from_sse(dateobj->time);
}

PHP_FUNCTION(date_timezone_get)
{
	zval             *object;
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	if (dateobj->time->is_localtime && dateobj->time->tz_info) {
		date_instantiate(date_ce_timezone, return_value TSRMLS_CC);
		tzobj = (php_timezone_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
		tzobj->tz = timelib_tzinfo_clone(dateobj->time->tz_info);
	} else {
		RETURN_FALSE;
	}
}

PHP_FUNCTION(date_timezone_set)
{
	zval             *object;
	zval             *timezone_object;
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|O", &object, date_ce_date, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	tzobj = (php_timezone_obj *) zend_object_store_get_object(timezone_object TSRMLS_CC);
	if (dateobj->time->tz_info) {
		timelib_tzinfo_dtor(dateobj->time->tz_info);
	}
	timelib_set_timezone(dateobj->time, timelib_tzinfo_clone(tzobj->tz));
	timelib_unixtime2local(dateobj->time, dateobj->time->sse);
}

PHP_FUNCTION(date_offset_get)
{
	zval                *object;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	if (dateobj->time->is_localtime && dateobj->time->tz_info) {
		offset = timelib_get_time_zone_info(dateobj->time->sse, dateobj->time->tz_info);
		RETVAL_LONG(offset->offset);
		timelib_time_offset_dtor(offset);
		return;
	} else {
		RETURN_LONG(0);
	}
}

PHP_FUNCTION(date_time_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          h, i, s = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_date, &h, &i, &s) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	dateobj->time->h = h;
	dateobj->time->i = i;
	dateobj->time->s = s;
	timelib_update_ts(dateobj->time, NULL);
}

PHP_FUNCTION(date_date_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          y, m, d;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olll", &object, date_ce_date, &y, &m, &d) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	dateobj->time->y = y;
	dateobj->time->m = m;
	dateobj->time->d = d;
	timelib_update_ts(dateobj->time, NULL);
}

PHP_FUNCTION(date_isodate_set)
{
	zval         *object;
	php_date_obj *dateobj;
	long          y, w, d = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_date, &y, &w, &d) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	dateobj->time->y = y;
	dateobj->time->m = 1;
	dateobj->time->d = 1;
	dateobj->time->relative.d = timelib_daynr_from_weeknr(y, w, d);
	dateobj->time->have_relative = 1;
	
	timelib_update_ts(dateobj->time, NULL);
}


PHP_FUNCTION(timezone_open)
{
	php_timezone_obj *tzobj;
	char *tz;
	int   tz_len;
	timelib_tzinfo *tzi = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len) == FAILURE) {
		RETURN_FALSE;
	}
	/* Try finding the tz information as "Timezone Abbreviation" */
	if (!tzi) {
		char *tzid;
		
		tzid = timelib_timezone_id_from_abbr(tz, -1, 0);
		if (tzid) {
			tzi = php_date_parse_tzfile(tzid, DATE_TIMEZONEDB TSRMLS_CC);
		}
	}
	/* Try finding the tz information as "Timezone Identifier" */
	if (!tzi) {
		tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB TSRMLS_CC);
	}
	/* If we find it we instantiate the object otherwise, well, we don't and return false */
	if (tzi) {
		date_instantiate(date_ce_timezone, return_value TSRMLS_CC);
		tzobj = (php_timezone_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
		tzobj->tz = tzi;
	} else {
		RETURN_FALSE;
	}
}

PHP_FUNCTION(timezone_name_get)
{
	zval             *object;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);

	RETURN_STRING(tzobj->tz->name, 1);
}

PHP_FUNCTION(timezone_name_from_abbr)
{
	char    *abbr;
	char    *tzname;
	int      abbr_len;
	long     gmtoffset = -1;
	long     isdst = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ll", &abbr, &abbr_len, &gmtoffset, &isdst) == FAILURE) {
		RETURN_FALSE;
	}
	tzname = timelib_timezone_id_from_abbr(abbr, gmtoffset, isdst);

	if (tzname) {
		RETURN_STRING(tzname, 1);
	} else {
		RETURN_FALSE;
	}
}

PHP_FUNCTION(timezone_offset_get)
{
	zval                *object, *dateobject;
	php_timezone_obj    *tzobj;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_timezone, &dateobject, date_ce_date) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	dateobj = (php_date_obj *) zend_object_store_get_object(dateobject TSRMLS_CC);

	offset = timelib_get_time_zone_info(dateobj->time->sse, tzobj->tz);
	RETVAL_LONG(offset->offset);
	timelib_time_offset_dtor(offset);
}

PHP_FUNCTION(timezone_transistions_get)
{
	zval                *object, *element;
	php_timezone_obj    *tzobj;
	int                  i;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);

	array_init(return_value);
	for (i = 0; i < tzobj->tz->timecnt; ++i) {
		MAKE_STD_ZVAL(element);
		array_init(element);
		add_assoc_long(element, "ts",     tzobj->tz->trans[i]);
		add_assoc_string(element, "time", php_format_date(DATE_FORMAT_ISO8601, 13, tzobj->tz->trans[i], 0 TSRMLS_CC), 0);
		add_assoc_long(element, "offset", tzobj->tz->type[tzobj->tz->trans_idx[i]].offset);
		add_assoc_bool(element, "isdst",  tzobj->tz->type[tzobj->tz->trans_idx[i]].isdst);
		add_assoc_string(element, "abbr", &tzobj->tz->timezone_abbr[tzobj->tz->type[tzobj->tz->trans_idx[i]].abbr_idx], 1);

		add_next_index_zval(return_value, element);
	}
}

PHP_FUNCTION(timezone_identifiers_list)
{
	const timelib_tzdb             *tzdb;
	const timelib_tzdb_index_entry *table;
	int                             i, item_count;

	tzdb = DATE_TIMEZONEDB;
	item_count = tzdb->index_size;
	table = tzdb->index;
	
	array_init(return_value);

	for (i = 0; i < item_count; ++i) {
		add_next_index_string(return_value, table[i].id, 1);
	};
}

PHP_FUNCTION(timezone_abbreviations_list)
{
	const timelib_tz_lookup_table *table, *entry;
	zval                          *element, **abbr_array_pp, *abbr_array;
	
	table = timelib_timezone_abbreviations_list();
	array_init(return_value);
	entry = table;

	do {
		MAKE_STD_ZVAL(element);
		array_init(element);
		add_assoc_bool(element, "dst", entry->type);
		add_assoc_long(element, "offset", entry->gmtoffset);
		if (entry->full_tz_name) {
			add_assoc_string(element, "timezone_id", entry->full_tz_name, 1);
		} else {
			add_assoc_null(element, "timezone_id");
		}

		if (zend_hash_find(HASH_OF(return_value), entry->name, strlen(entry->name) + 1, (void **) &abbr_array_pp) == FAILURE) {
			MAKE_STD_ZVAL(abbr_array);
			array_init(abbr_array);
			add_assoc_zval(return_value, entry->name, abbr_array);
		} else {
			abbr_array = *abbr_array_pp;
		}
		add_next_index_zval(abbr_array, element);
		entry++;
	} while (entry->name);
}


/* {{{ proto bool date_default_timezone_set(string timezone_identifier)
   Sets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_set)
{
	char *zone;
	int   zone_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zone, &zone_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (!timelib_timezone_id_is_valid(zone, DATE_TIMEZONEDB)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Timezone ID '%s' is invalid", zone);
		RETURN_FALSE;
	}
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
		DATEG(timezone) = NULL;
	}
	DATEG(timezone) = estrndup(zone, zone_len);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string date_default_timezone_get()
   Gets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_get)
{
	timelib_tzinfo *default_tz;

	default_tz = get_timezone_info(TSRMLS_C);
	RETVAL_ASCII_STRING(default_tz->name, 1);
}
/* }}} */

/* {{{ php_do_date_sunrise_sunset
 *  Common for date_sunrise() and date_sunset() functions
 */
static void php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAMETERS, int calc_sunset)
{
	double latitude, longitude, zenith, gmt_offset = 0, altitude;
	double h_rise, h_set, N;
	timelib_sll rise, set, transit;
	long time, retformat;
	int             rs;
	timelib_time   *t;
	timelib_tzinfo *tzi;
	char            retstr[6];
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ldddd", &time, &retformat, &latitude, &longitude, &zenith, &gmt_offset) == FAILURE) {
		RETURN_FALSE;
	}
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			retformat = SUNFUNCS_RET_STRING;
		case 2:
			latitude = INI_FLT("date.default_latitude");
		case 3:
			longitude = INI_FLT("date.default_longitude");
		case 4:
			if (calc_sunset) {
				zenith = INI_FLT("date.sunset_zenith");
			} else {
				zenith = INI_FLT("date.sunrise_zenith");
			}
		case 5:
		case 6:
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid format");
			RETURN_FALSE;
			break;
	}
	if (retformat != SUNFUNCS_RET_TIMESTAMP &&
		retformat != SUNFUNCS_RET_STRING &&
		retformat != SUNFUNCS_RET_DOUBLE)
	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong return format given, pick one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING or SUNFUNCS_RET_DOUBLE");
		RETURN_FALSE;
	}
	altitude = 90 - zenith;

	/* Initialize time struct */
	t = timelib_time_ctor();
	tzi = get_timezone_info(TSRMLS_C);
	t->tz_info = tzi;
	t->zone_type = TIMELIB_ZONETYPE_ID;

	if (ZEND_NUM_ARGS() <= 5) {
		gmt_offset = timelib_get_current_offset(t) / 3600;
	}

	timelib_unixtime2local(t, time);
	rs = timelib_astro_rise_set_altitude(t, longitude, latitude, altitude, altitude > -1 ? 1 : 0, &h_rise, &h_set, &rise, &set, &transit);
	timelib_time_dtor(t);
	
	if (rs != 0) {
		RETURN_FALSE;
	}

	if (retformat == SUNFUNCS_RET_TIMESTAMP) {
		RETURN_LONG(calc_sunset ? set : rise);
	}
	N = (calc_sunset ? h_set : h_rise) + gmt_offset;
	while (N > 24) {
		N -= 24;
	}
	while (N < 0) {
		N += 24;
	}
	switch (retformat) {
		case SUNFUNCS_RET_STRING:
			sprintf(retstr, "%02d:%02d", (int) N, (int) (60 * (N - (int) N)));
			RETURN_RT_STRINGL(retstr, 5, 1);
			break;
		case SUNFUNCS_RET_DOUBLE:
			RETURN_DOUBLE(N);
			break;
	}
}
/* }}} */

/* {{{ proto mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
   Returns time of sunrise for a given day and location */
PHP_FUNCTION(date_sunrise)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
   Returns time of sunset for a given day and location */
PHP_FUNCTION(date_sunset)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array date_sun_info(long time, float latitude, float longitude)
   Returns an array with information about sun set/rise and twilight begin/end */
PHP_FUNCTION(date_sun_info)
{
	long            time;
	double          latitude, longitude;
	timelib_time   *t, *t2;
	timelib_tzinfo *tzi;
	int             rs;
	timelib_sll     rise, set, transit;
	int             dummy;
	double          ddummy;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ldd", &time, &latitude, &longitude) == FAILURE) {
		RETURN_FALSE;
	}
	/* Initialize time struct */
	t = timelib_time_ctor();
	tzi = get_timezone_info(TSRMLS_C);
	t->tz_info = tzi;
	t->zone_type = TIMELIB_ZONETYPE_ID;
	timelib_unixtime2local(t, time);

	/* Setup */
	t2 = timelib_time_ctor();
	array_init(return_value);
	
	/* Get sun up/down and transit */
	rs = timelib_astro_rise_set_altitude(t, latitude, longitude, -35.0/60, 1, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "sunrise", 0);
			add_assoc_bool(return_value, "sunset", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "sunrise", 1);
			add_assoc_bool(return_value, "sunset", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "sunrise", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "sunset", timelib_date_to_int(t2, &dummy));
	}
	t2->sse = transit;
	add_assoc_long(return_value, "transit", timelib_date_to_int(t2, &dummy));

	/* Get civil twilight */
	rs = timelib_astro_rise_set_altitude(t, latitude, longitude, -6.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "civil_twilight_begin", 0);
			add_assoc_bool(return_value, "civil_twilight_end", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "civil_twilight_begin", 1);
			add_assoc_bool(return_value, "civil_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "civil_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "civil_twilight_end", timelib_date_to_int(t2, &dummy));
	}

	/* Get nautical twilight */
	rs = timelib_astro_rise_set_altitude(t, latitude, longitude, -12.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "nautical_twilight_begin", 0);
			add_assoc_bool(return_value, "nautical_twilight_end", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "nautical_twilight_begin", 1);
			add_assoc_bool(return_value, "nautical_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "nautical_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "nautical_twilight_end", timelib_date_to_int(t2, &dummy));
	}

	/* Get astronomical twilight */
	rs = timelib_astro_rise_set_altitude(t, latitude, longitude, -18.0, 0, &ddummy, &ddummy, &rise, &set, &transit);
	switch (rs) {
		case -1: /* always below */
			add_assoc_bool(return_value, "astronomical_twilight_begin", 0);
			add_assoc_bool(return_value, "astronomical_twilight_end", 0);
			break;
		case 1: /* always above */
			add_assoc_bool(return_value, "astronomical_twilight_begin", 1);
			add_assoc_bool(return_value, "astronomical_twilight_end", 1);
			break;
		default:
			t2->sse = rise;
			add_assoc_long(return_value, "astronomical_twilight_begin", timelib_date_to_int(t2, &dummy));
			t2->sse = set;
			add_assoc_long(return_value, "astronomical_twilight_end", timelib_date_to_int(t2, &dummy));
	}
	timelib_time_dtor(t);
	timelib_time_dtor(t2);
}
/* }}} */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
