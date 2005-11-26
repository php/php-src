/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2005 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
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

/* {{{ Function table */
function_entry date_functions[] = {
	PHP_FE(strtotime, NULL)
	PHP_FE(date, NULL)
	PHP_FE(gmdate, NULL)
	PHP_FE(mktime, NULL)
	PHP_FE(gmmktime, NULL)
	PHP_FE(checkdate, NULL)

#ifdef HAVE_STRFTIME
	PHP_FE(strftime, NULL)
	PHP_FE(gmstrftime, NULL)
#endif

	PHP_FE(time, NULL)
	PHP_FE(localtime, NULL)
	PHP_FE(getdate, NULL)

#ifdef EXPERIMENTAL_DATE_SUPPORT
	/* Advanced Interface */
	PHP_FE(date_create, NULL)
	PHP_FE(date_format, NULL)
	PHP_FE(date_modify, NULL)
	PHP_FE(date_timezone_get, NULL)
	PHP_FE(date_timezone_set, NULL)
	PHP_FE(date_offset_get, NULL)

	PHP_FE(date_time_set, NULL)
	PHP_FE(date_date_set, NULL)
	PHP_FE(date_isodate_set, NULL)

	PHP_FE(timezone_open, NULL)
	PHP_FE(timezone_name_get, NULL)
	PHP_FE(timezone_offset_get, NULL)
	PHP_FE(timezone_transistions_get, NULL)
	PHP_FE(timezone_identifiers_list, NULL)
	PHP_FE(timezone_abbreviations_list, NULL)
#endif

	/* Options and Configuration */
	PHP_FE(date_default_timezone_set, NULL)
	PHP_FE(date_default_timezone_get, NULL)
	{NULL, NULL, NULL}
};


function_entry date_funcs_date[] = {
#ifdef EXPERIMENTAL_DATE_SUPPORT
	ZEND_NAMED_FE(format, ZEND_FN(date_format), NULL)
	ZEND_NAMED_FE(modify, ZEND_FN(date_modify), NULL)
	ZEND_NAMED_FE(getTimezone, ZEND_FN(date_timezone_get), NULL)
	ZEND_NAMED_FE(setTimezone, ZEND_FN(date_timezone_set), NULL)
	ZEND_NAMED_FE(getOffset, ZEND_FN(date_offset_get), NULL)
	ZEND_NAMED_FE(setTime, ZEND_FN(date_time_set), NULL)
	ZEND_NAMED_FE(setDate, ZEND_FN(date_date_set), NULL)
	ZEND_NAMED_FE(setISODate, ZEND_FN(date_isodate_set), NULL)
#endif	
	{NULL, NULL, NULL}
};

function_entry date_funcs_timezone[] = {
#ifdef EXPERIMENTAL_DATE_SUPPORT
	ZEND_NAMED_FE(getName, ZEND_FN(timezone_name_get), NULL)
	ZEND_NAMED_FE(getOffset, ZEND_FN(timezone_offset_get), NULL)
	ZEND_NAMED_FE(getTransistions, ZEND_FN(timezone_transistions_get), NULL)
	ZEND_MALIAS(timezone, listAbbreviations, abbreviations_list, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_MALIAS(timezone, listIdentifiers, identifiers_list, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
#endif
	{NULL, NULL, NULL}
};

static char* guess_timezone(TSRMLS_D);
static void date_register_classes(TSRMLS_D);
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(date)

/* True global */
timelib_tzdb *php_date_global_timezone_db;
int php_date_global_timezone_db_enabled;

/* {{{ INI Settings */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("date.timezone", "", PHP_INI_ALL, OnUpdateString, default_timezone, zend_date_globals, date_globals)
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

#ifdef EXPERIMENTAL_DATE_SUPPORT
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

#endif

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
	STANDARD_MODULE_PROPERTIES
};
/* }}} */


/* {{{ php_date_init_globals */
static void php_date_init_globals(zend_date_globals *date_globals)
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

#define DATE_FORMAT_ISO8601  "Y-m-d\\TH:i:sO"
#define DATE_FORMAT_RFC1036  "l, d-M-y H:i:s T"
#define DATE_FORMAT_RFC1123  "D, d M Y H:i:s T"
#define DATE_FORMAT_RFC2822  "D, d M Y H:i:s O"

#define DATE_TZ_ERRMSG \
	"It is not safe to rely on the system's timezone settings. Please use " \
	"the date.timezone setting, the TZ environment variable or the " \
	"date_default_timezone_set() function. "

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(date)
{
	ZEND_INIT_MODULE_GLOBALS(date, php_date_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	date_register_classes(TSRMLS_C);

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
	timelib_tzdb *tzdb = DATE_TIMEZONEDB;
	
	php_info_print_table_start();
	php_info_print_table_row(2, "date/time support", "enabled");
	php_info_print_table_row(2, "Timezone Database Version", tzdb->version);
	php_info_print_table_row(2, "Timezone Database", php_date_global_timezone_db_enabled ? "external" : "internal");
	php_info_print_table_row(2, "Default timezone", guess_timezone(TSRMLS_C));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Timezone Cache functions */
static timelib_tzinfo *php_date_parse_tzfile(char *formal_tzname, timelib_tzdb *tzdb TSRMLS_DC)
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
static char* guess_timezone(TSRMLS_D)
{
	char *env;

	/* Checking configure timezone */
	if (DATEG(timezone) && (strlen(DATEG(timezone)) > 0)) {
		return DATEG(timezone);
	}
	/* Check environment variable */
	env = getenv("TZ");
	if (env && *env) {
		return env;
	}
	/* Check config setting for default timezone */
	if (DATEG(default_timezone) && (strlen(DATEG(default_timezone)) > 0)) {
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
	
	tz = guess_timezone(TSRMLS_C);
	tzi = php_date_parse_tzfile(tz, DATE_TIMEZONEDB TSRMLS_CC);
	if (! tzi) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Timezone setting (date.timezone) or TZ environment variable contains an unknown timezone");
		tzi = php_date_parse_tzfile("UTC", DATE_TIMEZONEDB TSRMLS_CC);

		if (! tzi) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Timezone database is corrupt - this should *never* happen!");
		}
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

/* {{{ date_format - (gm)date helper */
static char *date_format(char *format, int format_len, timelib_time *t, int localtime)
{
	smart_str            string = {0};
	int                  i;
	char                 buffer[33];
	timelib_time_offset *offset;
	timelib_sll          isoweek, isoyear;

	if (!format_len) {
		return estrdup("");
	}

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
	buffer[32] = '\0';
	timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear);

	for (i = 0; i < format_len; i++) {
		switch (format[i]) {
			/* day */
			case 'd': snprintf(buffer, 32, "%02d", (int) t->d); break;
			case 'D': snprintf(buffer, 32, "%s", day_short_names[timelib_day_of_week(t->y, t->m, t->d)]); break;
			case 'j': snprintf(buffer, 32, "%d", (int) t->d); break;
			case 'l': snprintf(buffer, 32, "%s", day_full_names[timelib_day_of_week(t->y, t->m, t->d)]); break;
			case 'S': snprintf(buffer, 32, "%s", english_suffix(t->d)); break;
			case 'w': snprintf(buffer, 32, "%d", (int) timelib_day_of_week(t->y, t->m, t->d)); break;
			case 'N': snprintf(buffer, 32, "%d", (int) timelib_iso_day_of_week(t->y, t->m, t->d)); break;
			case 'z': snprintf(buffer, 32, "%d", (int) timelib_day_of_year(t->y, t->m, t->d)); break;

			/* week */
			case 'W': snprintf(buffer, 32, "%02d", (int) isoweek); break; /* iso weeknr */
			case 'o': snprintf(buffer, 32, "%d", (int) isoyear); break; /* iso year */

			/* month */
			case 'F': snprintf(buffer, 32, "%s", mon_full_names[t->m - 1]); break;
			case 'm': snprintf(buffer, 32, "%02d", (int) t->m); break;
			case 'M': snprintf(buffer, 32, "%s", mon_short_names[t->m - 1]); break;
			case 'n': snprintf(buffer, 32, "%d", (int) t->m); break;
			case 't': snprintf(buffer, 32, "%d", (int) timelib_days_in_month(t->y, t->m)); break;

			/* year */
			case 'L': snprintf(buffer, 32, "%d", timelib_is_leap((int) t->y)); break;
			case 'y': snprintf(buffer, 32, "%02d", (int) t->y % 100); break;
			case 'Y': snprintf(buffer, 32, "%04d", (int) t->y); break;

			/* time */
			case 'a': snprintf(buffer, 32, "%s", t->h >= 12 ? "pm" : "am"); break;
			case 'A': snprintf(buffer, 32, "%s", t->h >= 12 ? "PM" : "AM"); break;
			case 'B': snprintf(buffer, 32, "[B unimplemented]"); break;
			case 'g': snprintf(buffer, 32, "%d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'G': snprintf(buffer, 32, "%d", (int) t->h); break;
			case 'h': snprintf(buffer, 32, "%02d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'H': snprintf(buffer, 32, "%02d", (int) t->h); break;
			case 'i': snprintf(buffer, 32, "%02d", (int) t->i); break;
			case 's': snprintf(buffer, 32, "%02d", (int) t->s); break;

			/* timezone */
			case 'I': snprintf(buffer, 32, "%d", localtime ? offset->is_dst : 0); break;
			case 'O': snprintf(buffer, 32, "%c%02d%02d",
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'T': snprintf(buffer, 32, "%s", localtime ? offset->abbr : "GMT"); break;
			case 'e': snprintf(buffer, 32, "%s", localtime ? t->tz_info->name : "UTC"); break;
			case 'Z': snprintf(buffer, 32, "%d", localtime ? offset->offset : 0); break;

			/* full date/time */
			case 'c': snprintf(buffer, 32, "%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
							                (int) t->y, (int) t->m, (int) t->d,
											(int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'r': snprintf(buffer, 32, "%3s, %02d %3s %04d %02d:%02d:%02d %c%02d%02d",
							                day_short_names[timelib_day_of_week(t->y, t->m, t->d)],
											(int) t->d, mon_short_names[t->m - 1],
											(int) t->y, (int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'U': snprintf(buffer, 32, "%lld", (timelib_sll) t->sse); break;

			case '\\': if (i < format_len) i++; buffer[0] = format[i]; buffer[1] = '\0'; break;

			default: buffer[0] = format[i]; buffer[1] = '\0';
		}
		smart_str_appends(&string, buffer);
		buffer[0] = '\0';
	}

	smart_str_0(&string);

	if (localtime) {
		timelib_time_offset_dtor(offset);
	}

	return string.c;
}

static void php_date(INTERNAL_FUNCTION_PARAMETERS, int localtime)
{
	char   *format;
	int     format_len;
	time_t  ts;
	char   *string;

	if (ZEND_NUM_ARGS() == 1) {
		ts = time(NULL);
	}
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}

	string = php_format_date(format, format_len, ts, localtime TSRMLS_CC);
	
	RETVAL_STRING(string, 0);
}
/* }}} */

PHPAPI char *php_format_date(char *format, int format_len, time_t ts, int localtime TSRMLS_DC) /* {{{ */
{
	timelib_time   *t;
	timelib_tzinfo *tzi;
	char *string;

	t = timelib_time_ctor();

	if (localtime) {
		tzi = get_timezone_info(TSRMLS_C);
		timelib_unixtime2local(t, ts, tzi);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}

	string = date_format(format, format_len, t, localtime);
	
	timelib_time_dtor(t);
	return string;
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

/* {{{ php_date_set_tzdb - NOT THREADSAFE */
PHPAPI void php_date_set_tzdb(timelib_tzdb *tzdb)
{
	timelib_tzdb *builtin = timelib_builtin_db();
	
	if (php_version_compare(tzdb->version, builtin->version) > 0) {
		php_date_global_timezone_db = tzdb;
		php_date_global_timezone_db_enabled = 1;
	}
}
/* }}} */

/* {{{ php_parse_date: Backwards compability function */
signed long php_parse_date(char *string, signed long *now)
{
	timelib_time *parsed_time;
	int           error1, error2;
	signed long   retval;

	parsed_time = timelib_strtotime(string, &error1, DATE_TIMEZONEDB);
	timelib_update_ts(parsed_time, NULL);
	retval = timelib_date_to_int(parsed_time, &error2);
	timelib_time_dtor(parsed_time);
	if (error1 || error2) {
		return -1;
	}
	return retval;
}
/* }}} */


/* {{{ proto int strtotime(string time, int now)
   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	char *times, *initial_ts;
	int   time_len, error1, error2;
	long  preset_ts, ts;

	timelib_time *t, *now;
	timelib_tzinfo *tzi;

	tzi = get_timezone_info(TSRMLS_C);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sl", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have an initial timestamp */
		now = timelib_time_ctor();

		initial_ts = emalloc(25);
		snprintf(initial_ts, 24, "@%lu", preset_ts);
		t = timelib_strtotime(initial_ts, &error1, DATE_TIMEZONEDB); /* we ignore the error here, as this should never fail */
		timelib_update_ts(t, tzi);
		timelib_unixtime2local(now, t->sse, tzi);
		timelib_time_dtor(t);
		efree(initial_ts);
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &times, &time_len) != FAILURE) {
		/* We have no initial timestamp */
		now = timelib_time_ctor();
		timelib_unixtime2local(now, (timelib_sll) time(NULL), tzi);
	} else {
		RETURN_FALSE;
	}

	t = timelib_strtotime(times, &error1, DATE_TIMEZONEDB);
	timelib_fill_holes(t, now, 0);
	timelib_update_ts(t, tzi);
	ts = timelib_date_to_int(t, &error2);

	/* if tz_info is not a copy, avoid double free */
	if (now->tz_info != tzi) {
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
	timelib_tzinfo *tzi;
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
		timelib_unixtime2local(now, (timelib_sll) time(NULL), tzi);
	}
	/* Fill in the new data */
	switch (ZEND_NUM_ARGS()) {
		case 7:
			/* break intentionally missing */
		case 6:
			if (yea >= 0 && yea < 70) {
				yea += 2000;
			} else if (yea >= 70 && yea <= 100) {
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

/* {{{ proto int mktime(int hour, int min, int sec, int mon, int day, int year)
   Get UNIX timestamp for a date */
PHP_FUNCTION(mktime)
{
	php_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int gmmktime(int hour, int min, int sec, int mon, int day, int year)
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
	timelib_time_offset *offset;

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
		timelib_unixtime2local(ts, (timelib_sll) timestamp, tzi);
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
		RETURN_STRINGL(buf, real_len, 0);
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
	timelib_unixtime2local(ts, (timelib_sll) timestamp, tzi);

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
	timelib_unixtime2local(ts, (timelib_sll) timestamp, tzi);

	array_init(return_value);

	add_assoc_long(return_value, "seconds", ts->s);
	add_assoc_long(return_value, "minutes", ts->i);
	add_assoc_long(return_value, "hours", ts->h);
	add_assoc_long(return_value, "mday", ts->d);
	add_assoc_long(return_value, "wday", timelib_day_of_week(ts->y, ts->m, ts->d));
	add_assoc_long(return_value, "mon", ts->m);
	add_assoc_long(return_value, "year", ts->y);
	add_assoc_long(return_value, "yday", timelib_day_of_year(ts->y, ts->m, ts->d));
	add_assoc_string(return_value, "weekday", day_full_names[timelib_day_of_week(ts->y, ts->m, ts->d)], 1);
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

	REGISTER_DATE_CLASS_CONST_STRING("ATOM",    DATE_FORMAT_ISO8601);
	REGISTER_DATE_CLASS_CONST_STRING("COOKIE",  DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("ISO8601", DATE_FORMAT_ISO8601);
	REGISTER_DATE_CLASS_CONST_STRING("RFC822",  DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("RFC850",  DATE_FORMAT_RFC1036);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1036", DATE_FORMAT_RFC1036);
	REGISTER_DATE_CLASS_CONST_STRING("RFC1123", DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("RFC2822", DATE_FORMAT_RFC2822);
	REGISTER_DATE_CLASS_CONST_STRING("RSS",     DATE_FORMAT_RFC1123);
	REGISTER_DATE_CLASS_CONST_STRING("W3C",     DATE_FORMAT_ISO8601);


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
	intern->std.ce = class_type;
	
	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	
	retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) date_object_free_storage_date, NULL TSRMLS_CC);
	retval.handlers = &date_object_handlers_date;
	
	return retval;
}

static zend_object_value date_object_new_timezone(zend_class_entry *class_type TSRMLS_DC)
{
	php_timezone_obj *intern;
	zend_object_value retval;

	intern = emalloc(sizeof(php_timezone_obj));
	memset(intern, 0, sizeof(php_timezone_obj));
	intern->std.ce = class_type;

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

	if (intern->std.properties) {
		zend_hash_destroy(intern->std.properties);
		efree(intern->std.properties);
		intern->std.properties = NULL;
	}
	
	efree(object);
}

static void date_object_free_storage_timezone(void *object TSRMLS_DC)
{
	efree(object);
}

#ifdef EXPERIMENTAL_DATE_SUPPORT
/* Advanced Interface */
static zval * date_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
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
	int             error;
	timelib_time   *now;
	timelib_tzinfo *tzi;
	char           *time_str;
	int             time_str_len = 0, free_tzi = 0;;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sO", &time_str, &time_str_len, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	date_instanciate(date_ce_date, return_value TSRMLS_CC);
	dateobj = (php_date_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	dateobj->time = timelib_strtotime(time_str_len ? time_str : "now", &error, DATE_TIMEZONEDB);

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
	timelib_unixtime2local(now, (timelib_sll) time(NULL), tzi);

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

PHP_FUNCTION(date_format)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format;
	int           format_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	RETURN_STRING(date_format(format, format_len, dateobj->time, dateobj->time->is_localtime), 0);
}

PHP_FUNCTION(date_modify)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *modify;
	int           modify_len;
	int           error;
	timelib_time *tmp_time;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);

	tmp_time = timelib_strtotime(modify, &error, DATE_TIMEZONEDB);
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
		date_instanciate(date_ce_timezone, return_value TSRMLS_CC);
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
	timelib_unixtime2local(dateobj->time, dateobj->time->sse, dateobj->time->tz_info);
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
		date_instanciate(date_ce_timezone, return_value TSRMLS_CC);
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
	timelib_tzdb             *tzdb;
	timelib_tzdb_index_entry *table;
	int                       i, item_count;

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
	timelib_tz_lookup_table *table, *entry;
	zval                    *element, **abbr_array_pp, *abbr_array;
	
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
#endif


/* {{{ proto bool date_default_timezone_set(string timezone_identifier)
   Sets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_set)
{
	char *zone;
	int   zone_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &zone, &zone_len) == FAILURE) {
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
	RETVAL_STRING(default_tz->name, 1);
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
