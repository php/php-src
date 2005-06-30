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

function_entry date_functions[] = {
	PHP_FE(date, NULL)
	PHP_FE(gmdate, NULL)
	PHP_FE(strtotime, NULL)
	{NULL, NULL, NULL}
};

ZEND_DECLARE_MODULE_GLOBALS(date)

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("date.timezone", "GMT", PHP_INI_ALL, OnUpdateString, default_timezone, zend_date_globals, date_globals)
PHP_INI_END()


zend_module_entry date_module_entry = {
	STANDARD_MODULE_HEADER,
	"date",                     /* extension name */
	date_functions,             /* function list */
	PHP_MINIT(date),            /* process startup */
	PHP_MSHUTDOWN(date),        /* process shutdown */
	NULL,                       /* request startup */
	NULL,                       /* request shutdown */
	PHP_MINFO(date),            /* extension info */
	PHP_VERSION,                /* extension version */
	STANDARD_MODULE_PROPERTIES
};

/* {{{ php_date_init_globals */
static void php_date_init_globals(zend_date_globals *date_globals)
{
	date_globals->default_timezone = NULL;
}
/* }}} */


PHP_MINIT_FUNCTION(date)
{
	ZEND_INIT_MODULE_GLOBALS(date, php_date_init_globals, NULL);
	REGISTER_INI_ENTRIES();

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(date)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}


PHP_MINFO_FUNCTION(date)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "date/time support", "enabled");
	php_info_print_table_end();
}

/* =[ Helper functions ] ================================================== */

static char* guess_timezone(TSRMLS_D)
{
	char *env;

	env = getenv("TZ");
	if (env) {
		return env;
	}
	/* Check config setting */
	if (DATEG(default_timezone)) {
		return DATEG(default_timezone);
	}
	return "GMT";
}

/* =[ date() and gmdate() ]================================================ */
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
	timelib_isoweek_from_date(t->y, t->m, t->d, &isoweek, &isoyear);

	for (i = 0; i < format_len; i++) {
		switch (format[i]) {
			/* day */
			case 'd': snprintf(&buffer, 32, "%02d", (int) t->d); break;
			case 'D': snprintf(&buffer, 32, "%s", day_short_names[timelib_day_of_week(t->y, t->m, t->d)]); break;
			case 'j': snprintf(&buffer, 32, "%d", (int) t->d); break;
			case 'l': snprintf(&buffer, 32, "%s", day_full_names[timelib_day_of_week(t->y, t->m, t->d)]); break;
			case 'S': snprintf(&buffer, 32, "%s", english_suffix(t->d)); break;
			case 'w': snprintf(&buffer, 32, "%d", (int) timelib_day_of_week(t->y, t->m, t->d)); break;
			case 'z': snprintf(&buffer, 32, "%d", (int) timelib_day_of_year(t->y, t->m, t->d)); break;

			/* week */
			case 'W': snprintf(&buffer, 32, "%d", (int) isoweek); break; /* iso weeknr */
			case 'o': snprintf(&buffer, 32, "%d", (int) isoyear); break; /* iso year */

			/* month */
			case 'F': snprintf(&buffer, 32, "%s", mon_full_names[t->m - 1]); break;
			case 'm': snprintf(&buffer, 32, "%02d", (int) t->m); break;
			case 'M': snprintf(&buffer, 32, "%s", mon_short_names[t->m - 1]); break;
			case 'n': snprintf(&buffer, 32, "%d", (int) t->m); break;
			case 't': snprintf(&buffer, 32, "%d", (int) timelib_days_in_month(t->y, t->m)); break;

			/* year */
			case 'L': snprintf(&buffer, 32, "%d", timelib_is_leap((int) t->y)); break;
			case 'y': snprintf(&buffer, 32, "%02d", (int) t->y % 100); break;
			case 'Y': snprintf(&buffer, 32, "%04d", (int) t->y); break;

			/* time */
			case 'a': snprintf(&buffer, 32, "%s", t->h >= 12 ? "pm" : "am"); break;
			case 'A': snprintf(&buffer, 32, "%s", t->h >= 12 ? "PM" : "AM"); break;
			case 'B': snprintf(&buffer, 32, "[B unimplemented]"); break;
			case 'g': snprintf(&buffer, 32, "%d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'G': snprintf(&buffer, 32, "%d", (int) t->h); break;
			case 'h': snprintf(&buffer, 32, "%02d", (t->h % 12) ? (int) t->h % 12 : 12); break;
			case 'H': snprintf(&buffer, 32, "%02d", (int) t->h); break;
			case 'i': snprintf(&buffer, 32, "%02d", (int) t->i); break;
			case 's': snprintf(&buffer, 32, "%02d", (int) t->s); break;

			/* timezone */
			case 'I': snprintf(&buffer, 32, "%d", localtime ? offset->is_dst : 0); break;
			case 'O': snprintf(&buffer, 32, "%c%02d%02d",
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'T': snprintf(&buffer, 32, "%s", localtime ? offset->abbr : "GMT"); break;
			case 'e': snprintf(&buffer, 32, "%s", localtime ? t->tz_info->name : "UTC"); break;
			case 'Z': snprintf(&buffer, 32, "%d", localtime ? offset->offset : 0); break;

			/* full date/time */
			case 'c': snprintf(&buffer, 32, "%04d-%02d-%02dT%02d:%02d:%02d%c%02d:%02d",
							                (int) t->y, (int) t->m, (int) t->d,
											(int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'r': snprintf(&buffer, 32, "%3s, %02d %3s %04d %02d:%02d:%02d %c%02d%02d",
							                day_short_names[timelib_day_of_week(t->y, t->m, t->d)],
											(int) t->d, mon_short_names[t->m - 1],
											(int) t->y, (int) t->h, (int) t->i, (int) t->s,
											localtime ? ((offset->offset < 0) ? '-' : '+') : '+',
											localtime ? abs(offset->offset / 3600) : 0,
											localtime ? abs((offset->offset % 3600) / 60) : 0
							  );
					  break;
			case 'U': snprintf(&buffer, 32, "%lld", (long long) t->sse); break;

			case '\\': if (i < format_len) i++; buffer[0] = format[i]; buffer[1] = '\0'; break;

			default: buffer[0] = format[i]; buffer[1] = '\0';
		}
		smart_str_appends(&string, buffer);
		buffer[0] = '\0';
	}

	smart_str_0(&string);

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
		tzi = timelib_parse_tzfile(guess_timezone());
		if (! tzi) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find any timezone setting");
			timelib_time_dtor(t);
			RETURN_FALSE;
		}
		timelib_unixtime2local(t, ts, tzi);
	} else {
		tzi = NULL;
		timelib_unixtime2gmt(t, ts);
	}
	string = php_format_date(format, format_len, t, localtime);
	
	RETVAL_STRING(string, 0);
	timelib_time_dtor(t);
}

PHP_FUNCTION(date)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(gmdate)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/* Backwards compability function */
signed long php_parse_date(char *string, signed long *now)
{
	timelib_time *parsed_time;
	int           error;
	signed long   retval;

	parsed_time = timelib_strtotime(string);
	timelib_update_ts(parsed_time, NULL);
	retval = timelib_date_to_int(parsed_time, &error);
	timelib_time_dtor(parsed_time);
	if (error) {
		return -1;
	}
	return retval;
}


/* {{{ proto int strtotime(string time, int now)
   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	char *times, *initial_ts;
	int   time_len, error;
	long  preset_ts, ts;

	timelib_time *t, *now;
	timelib_tzinfo *tzi;

	tzi = timelib_parse_tzfile(guess_timezone(TSRMLS_C));
	if (! tzi) {
		tzi = timelib_parse_tzfile("GMT");
	}
	if (! tzi) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find any timezone setting");
		RETURN_FALSE;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "sl", &times, &time_len, &preset_ts) != FAILURE) {
		/* We have an initial timestamp */
		now = timelib_time_ctor();

		initial_ts = emalloc(25);
		snprintf(initial_ts, 24, "@%lu", preset_ts);
		t = timelib_strtotime(initial_ts);
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

	t = timelib_strtotime(times);
	timelib_fill_holes(t, now, 0);
	timelib_update_ts(t, tzi);
	ts = timelib_date_to_int(t, &error);

	/* if tz_info is not a copy, avoid double free */
	if (now->tz_info == tzi) {
		now->tz_info = NULL;
	}
	if (t->tz_info == tzi) {
		t->tz_info = NULL;
	}

	timelib_time_dtor(now);	
	timelib_time_dtor(t);
	timelib_tzinfo_dtor(tzi);

	if (error) {
		RETURN_FALSE;
	} else {
		RETURN_LONG(ts);
	}
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
