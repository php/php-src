/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   +----------------------------------------------------------------------+
 */


/* $Id$ */


#include "php.h"
#include "zend_operators.h"
#include "datetime.h"
#include "php_globals.h"

#include <time.h>
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#include <stdio.h>

#include "php_parsedate.h"

char *mon_full_names[] =
{
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};
char *mon_short_names[] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
char *day_full_names[] =
{
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
char *day_short_names[] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

#if !defined(HAVE_TM_ZONE) && !defined(_TIMEZONE) && !defined(HAVE_DECLARED_TIMEZONE)
#if defined(NETWARE) && defined(NEW_LIBC)
#define timezone    _timezone   /* timezone is called '_timezone' in new version of LibC */
#endif
extern time_t timezone;
extern int daylight;
#endif

static int phpday_tab[2][12] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

#define isleap(year) (((year%4) == 0 && (year%100)!=0) || (year%400)==0)
#define YEAR_BASE 1900

/* {{{ proto int time(void)
   Return current UNIX timestamp */
PHP_FUNCTION(time)
{
	RETURN_LONG((long)time(NULL));
}
/* }}} */

/* {{{ php_mktime
 */
void php_mktime(INTERNAL_FUNCTION_PARAMETERS, int gm)
{
	pval **arguments[7];
	struct tm *ta, tmbuf;
	time_t t, seconds;
	int i, gmadjust, arg_count = ZEND_NUM_ARGS();
	int is_dst = -1, val, chgsecs = 0;

	if (arg_count > 7 || zend_get_parameters_array_ex(arg_count, arguments) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	/* convert supplied arguments to longs */
	for (i = 0; i < arg_count; i++) {
		convert_to_long_ex(arguments[i]);
	}
	t = time(NULL);
#ifdef HAVE_TZSET
	tzset();
#endif
	/*
	** Set default time parameters with local time values,
	** EVEN when some GMT time parameters are specified!
	** This may give strange result, with PHP gmmktime(0, 0, 0),
	** which is assumed to return GMT midnight time
	** for today (in localtime), so that the result time may be
	** AFTER or BEFORE the current time.
	** May be we should initialize tn using gmtime(), so that
	** default parameters for PHP gmmktime would be the current
	** GMT time values...
	*/
	ta = php_localtime_r(&t, &tmbuf);

	/* Let DST be unknown. mktime() should compute the right value
	** and behave correctly. Unless the user overrides this.
	*/
	ta->tm_isdst = -1;

	/*
	** Now change date values with supplied parameters.
	*/
	switch(arg_count) {
	case 7: /* daylight saving time flag */
#ifdef PHP_WIN32
		if (daylight > 0) {
			ta->tm_isdst = is_dst = Z_LVAL_PP(arguments[6]);
		} else {
			ta->tm_isdst = is_dst = 0;
		}
#else
		ta->tm_isdst = is_dst = Z_LVAL_PP(arguments[6]);
#endif
		/* fall-through */
	case 6: /* year */
		/* special case: 
		   a zero in year, month and day is considered illegal
		   as it would be interpreted as 30.11.1999 otherwise
		*/
		if (  (  Z_LVAL_PP(arguments[5])==0)
			  &&(Z_LVAL_PP(arguments[4])==0)
			  &&(Z_LVAL_PP(arguments[3])==0)
			  ) {
			RETURN_LONG(-1);
		}

		/*
		** Accept parameter in range 0..1000 interpreted as 1900..2900
		** (if 100 is given, it means year 2000)
		** or in range 1001..9999 interpreted as is (this will store
		** negative tm_year for years in range 1001..1899)
		** This function is then Y2K ready, and accepts a wide range of
		** dates including the whole gregorian calendar.
		** But it cannot represent ancestral dates prior to year 1001.
		** Additionally, input parameters of 0..70 are mapped to 100..170
		*/
		if (Z_LVAL_PP(arguments[5]) < 70)
			ta->tm_year = Z_LVAL_PP(arguments[5]) + 100;
		else
			ta->tm_year = Z_LVAL_PP(arguments[5])
			  - ((Z_LVAL_PP(arguments[5]) > 1000) ? 1900 : 0);
		/* fall-through */
	case 5: /* day in month (1-baesd) */
 		val = (*arguments[4])->value.lval; 
		if (val < 1) { 
			chgsecs += (1-val) * 60*60*24; 
			val = 1; 			
		} 
		ta->tm_mday = val; 
		/* fall-through */ 
	case 4: /* month (zero-based) */
		val = (*arguments[3])->value.lval - 1; 
		while (val < 0) { 
			val += 12; ta->tm_year--; 
		} 
		ta->tm_mon = val; 
		/* fall-through */ 
	case 3: /* second */
		val = (*arguments[2])->value.lval; 
		if (val < 1) { 
			chgsecs += (1-val); val = 1; 
		} 
		ta->tm_sec = val; 
		/* fall-through */ 
	case 2: /* minute */
		val = (*arguments[1])->value.lval; 
		if (val < 1) { 
			chgsecs += (1-val) * 60; val = 1; 
		} 
		ta->tm_min = val; 
		/* fall-through */ 
	case 1: /* hour */
		val = (*arguments[0])->value.lval; 
		if (val < 1) { 
			chgsecs += (1-val) * 60*60; val = 1; 
		} 
		ta->tm_hour = val; 
		/* fall-through */ 
	case 0: 
		break; 
	} 
	
	t = mktime(ta); 

#ifdef PHP_WIN32
	if (t - chgsecs < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Windows does not support negative values for this function");
		RETURN_LONG(-1);
	}
#endif

	seconds = t - chgsecs;

	if (is_dst == -1) {
		struct tm t1, t2;
		t1 = *localtime(&t);
		t2 = *localtime(&seconds);

		if(t1.tm_isdst != t2.tm_isdst) {
			seconds += (t1.tm_isdst == 1) ? 3600 : -3600;
			ta = localtime(&seconds);
		}

		is_dst = ta->tm_isdst; 
	}
	
	if (gm) {
#if HAVE_TM_GMTOFF
	    /*
		** mktime(ta) very nicely just filled ta->tm_gmtoff with
		** the exactly right value for adjustment if we want GMT.
	    */
	    gmadjust = ta->tm_gmtoff;
#else
	    /*
	    ** If correcting for daylight savings time, we set the adjustment to
		** the value of timezone - 3600 seconds.
	    */
#ifdef __CYGWIN__
	    gmadjust = -(is_dst ? _timezone - 3600 : _timezone);
#else
	    gmadjust = -(is_dst ? timezone - 3600 : timezone);
#endif
#endif
		seconds += gmadjust;
	}

	RETURN_LONG(seconds);
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

/* {{{ php_date
 */
static void
php_date(INTERNAL_FUNCTION_PARAMETERS, int gm)
{
	pval **format, **timestamp;
	time_t the_time;
	struct tm *ta, tmbuf;
	int i, size = 0, length, h, beat, fd, wd, yd, wk;
	char tmp_buff[32];
#if !HAVE_TM_GMTOFF
	long tzone;
	char *tname[2]= {"GMT Standard Time", "BST"};
#endif

	switch(ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &format) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		the_time = time(NULL);
		break;
	case 2:
		if (zend_get_parameters_ex(2, &format, &timestamp) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(timestamp);
		the_time = Z_LVAL_PP(timestamp);
#ifdef PHP_WIN32
		if (the_time < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Windows does not support dates prior to midnight (00:00:00), January 1, 1970");
			RETURN_FALSE;
		}
#endif
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(format);

	if (gm) {
		ta = php_gmtime_r(&the_time, &tmbuf);
#if !HAVE_TM_GMTOFF
		tzone = 0;
#endif
	} else {
		ta = php_localtime_r(&the_time, &tmbuf);
#if !HAVE_TM_GMTOFF
#ifdef __CYGWIN__
		tzone = _timezone;
#else
		tzone = timezone;
#endif
		tname[0] = tzname[0];
#endif
	}

	if (!ta) {			/* that really shouldn't happen... */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unexpected error");
		RETURN_FALSE;
	}
	for (i = 0; i < Z_STRLEN_PP(format); i++) {
		switch (Z_STRVAL_PP(format)[i]) {
			case 'r':		/* rfc822 format */
				size += 31;
				break;
			case 'U':		/* seconds since the epoch */
				size += 10;
				break;
			case 'F':		/* month, textual, full */
			case 'l':		/* day (of the week), textual */
				size += 28;
				break;
			case 'T':		/* timezone name */
#if HAVE_TM_ZONE
				size += strlen(ta->tm_zone);
#elif HAVE_TZNAME
				if (ta->tm_isdst > 0 ) {
					size += strlen(tname[1]);
				} else {
					size += strlen(tname[0]);
				}
#endif
				break;
			case 'Z':		/* timezone offset in seconds */
				size += 6;
				break;
			case 'O':		/* GMT offset in [+-]HHMM format */
				size += 5;
				break;
			case 'Y':		/* year, numeric, 4 digits */
				size += 4;
				break;
			case 'M':		/* month, textual, 3 letters */
			case 'D':		/* day, textual, 3 letters */
			case 'z':		/* day of the year, 1 to 366 */
			case 'B':		/* Swatch Beat, 3 digits */
				size += 3;
				break;
			case 'y':		/* year, numeric, 2 digits */
			case 'm':		/* month, numeric */
			case 'n':		/* month, numeric, no leading zeroes */
			case 'd':		/* day of the month, numeric */
			case 'j':		/* day of the month, numeric, no leading zeros */
			case 'H':		/* hour, numeric, 24 hour format */
			case 'h':		/* hour, numeric, 12 hour format */
			case 'G':		/* hour, numeric, 24 hour format, no leading zeroes */
			case 'g':		/* hour, numeric, 12 hour format, no leading zeroes */
			case 'i':		/* minutes, numeric */
			case 's':		/* seconds, numeric */
			case 'A':		/* AM/PM */
			case 'a':		/* am/pm */
			case 'S':		/* standard english suffix for the day of the month (e.g. 3rd, 2nd, etc) */
			case 't':		/* days in current month */
			case 'W':		/* ISO-8601 week number of year, weeks starting on Monday */
				size += 2;
				break;
			case '\\':
				if(i < Z_STRLEN_PP(format)-1) {
					i++;
				}
				size ++;
				break;
			case 'L':		/* boolean for leap year */
			case 'w':		/* day of the week, numeric */
			case 'I':		/* DST? */
			default:
				size++;
				break;
		}
	}

	Z_STRVAL_P(return_value) = (char *) emalloc(size + 1);
	Z_STRVAL_P(return_value)[0] = '\0';

	for (i = 0; i < Z_STRLEN_PP(format); i++) {
		switch (Z_STRVAL_PP(format)[i]) {
			case '\\':
				if(i < Z_STRLEN_PP(format)-1) {
					char ch[2];
					ch[0]=Z_STRVAL_PP(format)[i+1];
					ch[1]='\0';
					strcat(Z_STRVAL_P(return_value), ch);
					i++;
				}
				break;
			case 'U':		/* seconds since the epoch */
				sprintf(tmp_buff, "%ld", (long)the_time); /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'F':		/* month, textual, full */
				strcat(Z_STRVAL_P(return_value), mon_full_names[ta->tm_mon]);
				break;
			case 'l':		/* day (of the week), textual, full */
				strcat(Z_STRVAL_P(return_value), day_full_names[ta->tm_wday]);
				break;
			case 'Y':		/* year, numeric, 4 digits */
				sprintf(tmp_buff, "%d", ta->tm_year + YEAR_BASE);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'M':		/* month, textual, 3 letters */
				strcat(Z_STRVAL_P(return_value), mon_short_names[ta->tm_mon]);
				break;
			case 'D':		/* day (of the week), textual, 3 letters */
				strcat(Z_STRVAL_P(return_value), day_short_names[ta->tm_wday]);
				break;
			case 'z':		/* day (of the year) */
				sprintf(tmp_buff, "%d", ta->tm_yday);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'y':		/* year, numeric, 2 digits */
				sprintf(tmp_buff, "%02d", ((ta->tm_year)%100));  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'm':		/* month, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_mon + 1);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'n':      /* month, numeric, no leading zeros */
				sprintf(tmp_buff, "%d", ta->tm_mon + 1);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'd':		/* day of the month, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_mday);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'j':
				sprintf(tmp_buff, "%d", ta->tm_mday); /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'H':		/* hour, numeric, 24 hour format */
				sprintf(tmp_buff, "%02d", ta->tm_hour);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'h':		/* hour, numeric, 12 hour format */
				h = ta->tm_hour % 12; if (h==0) h = 12;
				sprintf(tmp_buff, "%02d", h);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'G':      /* hour, numeric, 24 hour format, no leading zeros */
				sprintf(tmp_buff, "%d", ta->tm_hour);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'g':      /* hour, numeric, 12 hour format, no leading zeros */
				h = ta->tm_hour % 12; if (h==0) h = 12;
				sprintf(tmp_buff, "%d", h);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'i':		/* minutes, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_min);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 's':		/* seconds, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_sec);  /* SAFE */ 
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'A':		/* AM/PM */
				strcat(Z_STRVAL_P(return_value), (ta->tm_hour >= 12 ? "PM" : "AM"));
				break;
			case 'a':		/* am/pm */
				strcat(Z_STRVAL_P(return_value), (ta->tm_hour >= 12 ? "pm" : "am"));
				break;
			case 'S':		/* standard english suffix, e.g. 2nd/3rd for the day of the month */
				if (ta->tm_mday >= 10 && ta->tm_mday <= 19) {
					strcat(Z_STRVAL_P(return_value), "th");
				} else {
					switch (ta->tm_mday % 10) {
						case 1:
							strcat(Z_STRVAL_P(return_value), "st");
							break;
						case 2:
							strcat(Z_STRVAL_P(return_value), "nd");
							break;
						case 3:
							strcat(Z_STRVAL_P(return_value), "rd");
							break;
						default:
							strcat(Z_STRVAL_P(return_value), "th");
							break;
					}
				}
				break;
			case 't':		/* days in current month */
				sprintf(tmp_buff, "%2d", phpday_tab[isleap((ta->tm_year+YEAR_BASE))][ta->tm_mon] );
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'w':		/* day of the week, numeric EXTENSION */
				sprintf(tmp_buff, "%01d", ta->tm_wday);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'O':		/* GMT offset in [+-]HHMM format */
#if HAVE_TM_GMTOFF				
				sprintf(tmp_buff, "%c%02d%02d", (ta->tm_gmtoff < 0) ? '-' : '+', abs(ta->tm_gmtoff / 3600), abs( (ta->tm_gmtoff % 3600) / 60 ));
#else
				sprintf(tmp_buff, "%c%02d%02d", ((ta->tm_isdst ? tzone - 3600:tzone)>0)?'-':'+', abs((ta->tm_isdst ? tzone - 3600 : tzone) / 3600), abs(((ta->tm_isdst ? tzone - 3600 : tzone) % 3600) / 60));
#endif
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'Z':		/* timezone offset in seconds */
#if HAVE_TM_GMTOFF
				sprintf(tmp_buff, "%ld", ta->tm_gmtoff);
#else
				sprintf(tmp_buff, "%ld", ta->tm_isdst ? -(tzone- 3600) : -tzone);
#endif
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'L':		/* boolean for leapyear */
				sprintf(tmp_buff, "%d", (isleap((ta->tm_year+YEAR_BASE)) ? 1 : 0 ) );
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'T':		/* timezone name */
#if HAVE_TM_ZONE
				strcat(Z_STRVAL_P(return_value), ta->tm_zone);
#elif HAVE_TZNAME
				strcat(Z_STRVAL_P(return_value), ta->tm_isdst ? tname[1] : tname[0]);
#endif
				break;
			case 'B':	/* Swatch Beat a.k.a. Internet Time */
				beat =  (((((long)the_time)-(((long)the_time) -
					((((long)the_time) % 86400) + 3600))) * 10) / 864);
				while (beat < 0) {
					beat += 1000;
				}
				beat = beat % 1000;
				sprintf(tmp_buff, "%03d", beat); /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'I':
				sprintf(tmp_buff, "%d", ta->tm_isdst);
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'r':
#if HAVE_TM_GMTOFF				
				sprintf(tmp_buff, "%3s, %2d %3s %04d %02d:%02d:%02d %c%02d%02d", 
					day_short_names[ta->tm_wday],
					ta->tm_mday,
					mon_short_names[ta->tm_mon],
					ta->tm_year + YEAR_BASE,
					ta->tm_hour,
					ta->tm_min,
					ta->tm_sec,
					(ta->tm_gmtoff < 0) ? '-' : '+',
					abs(ta->tm_gmtoff / 3600),
					abs( (ta->tm_gmtoff % 3600) / 60 )
				);
#else
				sprintf(tmp_buff, "%3s, %2d %3s %04d %02d:%02d:%02d %c%02d%02d", 
					day_short_names[ta->tm_wday],
					ta->tm_mday,
					mon_short_names[ta->tm_mon],
					ta->tm_year + YEAR_BASE,
					ta->tm_hour,
					ta->tm_min,
					ta->tm_sec,
					((ta->tm_isdst ? tzone - 3600 : tzone) > 0) ? '-' : '+',
					abs((ta->tm_isdst ? tzone - 3600 : tzone) / 3600),
					abs( ((ta->tm_isdst ? tzone - 3600 : tzone) % 3600) / 60 )
				);
#endif
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;
			case 'W':		/* ISO-8601 week number of year, weeks starting on Monday */
				wd = ta->tm_wday==0 ? 6 : ta->tm_wday-1;/* weekday */
				yd = ta->tm_yday + 1;					/* days since January 1st */

				fd = (7 + wd - yd % 7+ 1) % 7;			/* weekday (1st January) */	

				/* week is a last year week (52 or 53) */
				if ((yd <= 7 - fd) && fd > 3){			
					wk = (fd == 4 || (fd == 5 && isleap((ta->tm_year + YEAR_BASE - 1)))) ? 53 : 52;
				}
				/* week is a next year week (1) */
				else if (isleap((ta->tm_year+YEAR_BASE)) + 365 - yd < 3 - wd){
					wk = 1;
				}
				/* normal week */
				else {
					wk = (yd + 6 - wd + fd) / 7 - (fd > 3);
				}

				sprintf(tmp_buff, "%d", wk);  /* SAFE */
				strcat(Z_STRVAL_P(return_value), tmp_buff);
				break;

			default:
				length = strlen(Z_STRVAL_P(return_value));
				Z_STRVAL_P(return_value)[length] = Z_STRVAL_PP(format)[i];
				Z_STRVAL_P(return_value)[length + 1] = '\0';
				break;
		}
	}
	Z_STRLEN_P(return_value) = strlen(Z_STRVAL_P(return_value));
	Z_TYPE_P(return_value) = IS_STRING;
}
/* }}} */

/* {{{ proto string date(string format [, int timestamp])
   Format a local time/date */
PHP_FUNCTION(date)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string gmdate(string format [, int timestamp])
   Format a GMT/UTC date/time */
PHP_FUNCTION(gmdate)
{
	php_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array localtime([int timestamp [, bool associative_array]])
   Returns the results of the C system call localtime as an associative array if the associative_array argument is set to 1 other wise it is a regular array */
PHP_FUNCTION(localtime)
{
	zval **timestamp_arg, **assoc_array_arg;
	struct tm *ta, tmbuf;
	time_t timestamp;
	int assoc_array = 0;
	int arg_count = ZEND_NUM_ARGS();

	if (arg_count < 0 || arg_count > 2 ||
		zend_get_parameters_ex(arg_count, &timestamp_arg, &assoc_array_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch (arg_count) {
		case 0:
			timestamp = (long)time(NULL);
			break;
		case 1:
			convert_to_long_ex(timestamp_arg);
			timestamp = Z_LVAL_PP(timestamp_arg);
			break;
		case 2:
			convert_to_long_ex(timestamp_arg);
			convert_to_long_ex(assoc_array_arg);
			timestamp = Z_LVAL_PP(timestamp_arg);
			assoc_array = Z_LVAL_PP(assoc_array_arg);
			break;
	}
	
#ifdef PHP_WIN32
	if (timestamp < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Windows does not support negative values for this function");
		RETURN_FALSE
	}
#endif
	
	if (NULL == (ta = php_localtime_r(&timestamp, &tmbuf))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid local time");
		RETURN_FALSE;
	}
	array_init(return_value);

	if (assoc_array) {
		add_assoc_long(return_value, "tm_sec",   ta->tm_sec);
		add_assoc_long(return_value, "tm_min",   ta->tm_min);
		add_assoc_long(return_value, "tm_hour",  ta->tm_hour);
		add_assoc_long(return_value, "tm_mday",  ta->tm_mday);
		add_assoc_long(return_value, "tm_mon",   ta->tm_mon);
		add_assoc_long(return_value, "tm_year",  ta->tm_year);
		add_assoc_long(return_value, "tm_wday",  ta->tm_wday);
		add_assoc_long(return_value, "tm_yday",  ta->tm_yday);
		add_assoc_long(return_value, "tm_isdst", ta->tm_isdst);
	} else {
		add_next_index_long(return_value, ta->tm_sec);
		add_next_index_long(return_value, ta->tm_min);
		add_next_index_long(return_value, ta->tm_hour);
		add_next_index_long(return_value, ta->tm_mday);
		add_next_index_long(return_value, ta->tm_mon);
		add_next_index_long(return_value, ta->tm_year);
		add_next_index_long(return_value, ta->tm_wday);
		add_next_index_long(return_value, ta->tm_yday);
		add_next_index_long(return_value, ta->tm_isdst);
	}
}
/* }}} */

/* {{{ proto array getdate([int timestamp])
   Get date/time information */
PHP_FUNCTION(getdate)
{
	pval **timestamp_arg;
	struct tm *ta, tmbuf;
	time_t timestamp;

	if (ZEND_NUM_ARGS() == 0) {
		timestamp = time(NULL);
	} else if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &timestamp_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long_ex(timestamp_arg);
		timestamp = Z_LVAL_PP(timestamp_arg);
	}

	ta = php_localtime_r(&timestamp, &tmbuf);
	if (!ta) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot perform date calculation");
		return;
	}
	array_init(return_value);
	add_assoc_long(return_value, "seconds", ta->tm_sec);
	add_assoc_long(return_value, "minutes", ta->tm_min);
	add_assoc_long(return_value, "hours", ta->tm_hour);
	add_assoc_long(return_value, "mday", ta->tm_mday);
	add_assoc_long(return_value, "wday", ta->tm_wday);
	add_assoc_long(return_value, "mon", ta->tm_mon + 1);
	add_assoc_long(return_value, "year", ta->tm_year + 1900);
	add_assoc_long(return_value, "yday", ta->tm_yday);
	add_assoc_string(return_value, "weekday", day_full_names[ta->tm_wday], 1);
	add_assoc_string(return_value, "month", mon_full_names[ta->tm_mon], 1);
	add_index_long(return_value, 0, timestamp);
}
/* }}} */

/* {{{ php_std_date
   Return date string in standard format for http headers */
char *php_std_date(time_t t)
{
	struct tm *tm1, tmbuf;
	char *str;
	TSRMLS_FETCH();

	tm1 = php_gmtime_r(&t, &tmbuf);
	str = emalloc(81);
	if (PG(y2k_compliance)) {
		snprintf(str, 80, "%s, %02d-%s-%04d %02d:%02d:%02d GMT",
				day_short_names[tm1->tm_wday],
				tm1->tm_mday,
				mon_short_names[tm1->tm_mon],
				tm1->tm_year+1900,
				tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	} else {
		snprintf(str, 80, "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
				day_short_names[tm1->tm_wday],
				tm1->tm_mday,
				mon_short_names[tm1->tm_mon],
				((tm1->tm_year)%100),
				tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	}
	
	str[79]=0;
	return (str);
}
/* }}} */

/* {{{ proto bool checkdate(int month, int day, int year)
   Returns true(1) if it is a valid date in gregorian calendar */
PHP_FUNCTION(checkdate)
{
	pval **month, **day, **year;
	int m, d, y, res=0;
	
	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3, &month, &day, &year) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(Z_TYPE_PP(year) == IS_STRING) {
		res = is_numeric_string(Z_STRVAL_PP(year), Z_STRLEN_PP(year), NULL, NULL, 0);
		if(res!=IS_LONG && res !=IS_DOUBLE) {
			RETURN_FALSE;	
		}
	}
	convert_to_long_ex(day);
	convert_to_long_ex(month);
	convert_to_long_ex(year);
	y = Z_LVAL_PP(year);
	m = Z_LVAL_PP(month);
	d = Z_LVAL_PP(day);

	if (y < 1 || y > 32767) {
		RETURN_FALSE;
	}
	if (m < 1 || m > 12) {
		RETURN_FALSE;
	}
	if (d < 1 || d > phpday_tab[isleap(y)][m - 1]) {
		RETURN_FALSE;
	}
	RETURN_TRUE;				/* True : This month, day, year arguments are valid */
}
/* }}} */

#if HAVE_STRFTIME
/* {{{ _php_strftime
 */
void _php_strftime(INTERNAL_FUNCTION_PARAMETERS, int gm)
{
	pval **format_arg, **timestamp_arg;
	char *format, *buf;
	time_t timestamp;
	struct tm *ta, tmbuf;
	int max_reallocs = 5;
	size_t buf_len=64, real_len;

	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &format_arg)==FAILURE) {
				RETURN_FALSE;
			}
			time(&timestamp);
			break;
		case 2:
			if (zend_get_parameters_ex(2, &format_arg, &timestamp_arg)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(timestamp_arg);
			timestamp = Z_LVAL_PP(timestamp_arg);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string_ex(format_arg);
	if (Z_STRLEN_PP(format_arg)==0) {
		RETURN_FALSE;
	}
	if (timestamp < 0) {
		RETURN_FALSE;
	}
	format = Z_STRVAL_PP(format_arg);
	if (gm) {
		ta = php_gmtime_r(&timestamp, &tmbuf);
	} else {
		ta = php_localtime_r(&timestamp, &tmbuf);
	}

	buf = (char *) emalloc(buf_len);
	while ((real_len=strftime(buf, buf_len, format, ta))==buf_len || real_len==0) {
		buf_len *= 2;
		buf = (char *) erealloc(buf, buf_len);
		if(!--max_reallocs) break;
	}
	
	if(real_len && real_len != buf_len) {
		buf = (char *) erealloc(buf, real_len+1);
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
	_php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string gmstrftime(string format [, int timestamp])
   Format a GMT/UCT time/date according to locale settings */
PHP_FUNCTION(gmstrftime)
{
	_php_strftime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

#endif

/* {{{ proto int strtotime(string time, int now)
   Convert string representation of date and time to a timestamp */
PHP_FUNCTION(strtotime)
{
	zval	**z_time, **z_now;
	int 	 argc;
	time_t now;

	argc = ZEND_NUM_ARGS();

	if (argc < 1 || argc > 2 || zend_get_parameters_ex(argc, &z_time, &z_now)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(z_time);
	if (Z_STRLEN_PP(z_time) == 0)
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Called with empty time parameter");
	if (argc == 2) {
		convert_to_long_ex(z_now);
		now = Z_LVAL_PP(z_now);
		RETURN_LONG(php_parse_date(Z_STRVAL_PP(z_time), &now));
	} else {
		RETURN_LONG(php_parse_date(Z_STRVAL_PP(z_time), NULL));
	}
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
