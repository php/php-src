/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
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

	PHP_FE(date_default_timezone_set, NULL)
	PHP_FE(date_default_timezone_get, NULL)
	{NULL, NULL, NULL}
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(date)

/* {{{ INI Settings */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("date.timezone", "", PHP_INI_ALL, OnUpdateString, default_timezone, zend_date_globals, date_globals)
PHP_INI_END()
/* }}} */

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

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(date)
{
	if (DATEG(timezone)) {
		efree(DATEG(timezone));
	}
	DATEG(timezone) = NULL;

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

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(date)
{
	ZEND_INIT_MODULE_GLOBALS(date, php_date_init_globals, NULL);
	REGISTER_INI_ENTRIES();

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
	php_info_print_table_start();
	php_info_print_table_row(2, "date/time support", "enabled");
	php_info_print_table_end();
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
		tzid = timelib_timezone_id_from_abbr(ta->tm_zone);
		if (! tzid) {
			tzid = "UTC";
		}
		
		php_error_docref(NULL TSRMLS_CC, E_STRICT, "It is not safe to rely on the systems timezone settings, please use the date.timezone setting, the TZ environment variable or the date_timezone_set() function. We now use '%s' for '%s'", tzid, ta->tm_zone);
		return tzid;
	}
#endif
	/* Fallback to UTC */
	return "UTC";
}

static timelib_tzinfo *get_timezone_info(TSRMLS_D)
{
	char *tz;
	timelib_tzinfo *tzi;
	
	tz = guess_timezone(TSRMLS_C);
	tzi = timelib_parse_tzfile(tz);
	if (! tzi) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Timezone setting (date.timezone) or TZ environment variable contain an unknown timezone.");
		tzi = timelib_parse_tzfile("UTC");

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

static char *english_suffix(int number)
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

/* {{{ php_format_date - (gm)date helper */
static char *php_format_date(char *format, int format_len, timelib_time *t, int localtime)
{
	smart_str            string = {0};
	int                  i;
	char                 buffer[33];
	timelib_time_offset *offset;
	timelib_sll          isoweek, isoyear;

	if (localtime) {
		offset = timelib_get_time_zone_info(t->sse, t->tz_info);
	}
	buffer[32] = '\0';
	smart_str_appends(&string, "");
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
			case 'z': snprintf(buffer, 32, "%d", (int) timelib_day_of_year(t->y, t->m, t->d)); break;

			/* week */
			case 'W': snprintf(buffer, 32, "%d", (int) isoweek); break; /* iso weeknr */
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
	char *format;
	int   format_len;
	long  ts = time(NULL);
	timelib_time   *t;
	char           *string;
	timelib_tzinfo *tzi;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &format, &format_len, &ts) == FAILURE) {
		RETURN_FALSE;
	}

	t = timelib_time_ctor();

	if (localtime) {
		tzi = get_timezone_info(TSRMLS_C);
		timelib_unixtime2local(t, ts, tzi);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}
	string = php_format_date(format, format_len, t, localtime);
	
	RETVAL_STRING(string, 0);
	if (localtime) {
		timelib_tzinfo_dtor(tzi);
	}
	timelib_time_dtor(t);
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


/* {{{ php_parse_date: Backwards compability function */
signed long php_parse_date(char *string, signed long *now)
{
	timelib_time *parsed_time;
	int           error1, error2;
	signed long   retval;

	parsed_time = timelib_strtotime(string, &error1);
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
		t = timelib_strtotime(initial_ts, &error1); /* we ignore the error here, as this should never fail */
		timelib_update_ts(t, tzi);
		timelib_unixtime2local(now, t->sse, tzi);
		timelib_time_dtor(t);
		efree(initial_ts);
	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &times, &time_len) != FAILURE) {
		/* We have no initial timestamp */
		now = timelib_time_ctor();
		timelib_unixtime2local(now, (timelib_sll) time(NULL), tzi);
	} else {
		timelib_tzinfo_dtor(tzi);
		RETURN_FALSE;
	}

	t = timelib_strtotime(times, &error1);
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

	timelib_tzinfo_dtor(tzi);
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
			php_error_docref(NULL TSRMLS_CC, E_STRICT, "You should be using the time() function instead.");
	}
	/* Update the timestamp */
	if (gmt) {
		timelib_update_ts(now, NULL);
	} else {
		timelib_update_ts(now, tzi);
	}
	/* Support for the deprecated is_dst parameter */
	if (dst != -1) {
		php_error_docref(NULL TSRMLS_CC, E_STRICT, "The is_dst parameter is deprecated.");
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
	if (!gmt) {
		timelib_tzinfo_dtor(tzi);
	}

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

	if (!gmt) {
		timelib_tzinfo_dtor(tzi);
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

	timelib_tzinfo_dtor(tzi);
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

	timelib_tzinfo_dtor(tzi);
	timelib_time_dtor(ts);
}
/* }}} */


/* {{{ proto void date_default_timezone_set(string timezone_identifier)
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

/* {{{ proto void date_default_timezone_get(string timezone_identifier)
   Gets the default timezone used by all date/time functions in a script */
PHP_FUNCTION(date_default_timezone_get)
{
	RETURN_STRING(DATEG(timezone), 0);
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
