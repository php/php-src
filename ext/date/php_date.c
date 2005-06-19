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

static char* guess_timezone(TSRMLS_D)
{
	char *env;

	env = getenv("TZ");
	if (env) {
		return env;
	}
	if (DATEG(default_timezone)) {
		return DATEG(default_timezone);
	}
	/* Check config setting */
	/*
	 */
	return "GMT";
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
