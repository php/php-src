/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
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
#include "snprintf.h"
#include "php_globals.h"

#include <time.h>
#include <stdio.h>

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

#if !defined(HAVE_TM_ZONE) && !defined(_TIMEZONE) && !(WIN32||WINNT)
extern time_t timezone;
#endif

static int phpday_tab[2][12] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

PHP_FUNCTION(time)
{
	return_value->value.lval = (long) time(NULL);
	return_value->type = IS_LONG;
}

void _php3_mktime(INTERNAL_FUNCTION_PARAMETERS, int gm)
{
	pval *arguments[7];
	struct tm ta, *tn;
	time_t t;
	int i, gmadjust=0,arg_count = ARG_COUNT(ht);

	if (arg_count > 7 || getParametersArray(ht, arg_count, arguments) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	/* convert supplied arguments to longs */
	for (i = 0; i < arg_count; i++) {
		convert_to_long(arguments[i]);
	}
	t=time(NULL);
#ifdef HAVE_TZSET
	tzset();
#endif
	tn = localtime(&t);
	memcpy(&ta,tn,sizeof(struct tm));
	ta.tm_isdst = arg_count > 6 ? arguments[6]->value.lval : gm ? 0 : -1;

	switch(arg_count) {
	case 7:
	case 6:
		ta.tm_year = arguments[5]->value.lval - ((arguments[5]->value.lval > 1000) ? 1900 : 0);
		/* fall-through */
	case 5:
		ta.tm_mday = arguments[4]->value.lval;
		/* fall-through */
	case 4:
		ta.tm_mon = arguments[3]->value.lval - 1;
		/* fall-through */
	case 3:
		ta.tm_sec = arguments[2]->value.lval;
		/* fall-through */
	case 2:
		ta.tm_min = arguments[1]->value.lval;
		/* fall-through */
	case 1:
		ta.tm_hour = arguments[0]->value.lval;
	case 0:
		break;
	}
	t=mktime(&ta); /* Need to do this because of Daylight savings */
	tn = localtime(&t);

	if (gm) {
#if HAVE_TM_GMTOFF
		gmadjust=tn->tm_gmtoff;
#else
		gmadjust=timezone;
#endif
		ta.tm_hour += gmadjust / 3600;
		ta.tm_min += gmadjust % 3600;
	}

	RETURN_LONG(mktime(&ta));
}

PHP_FUNCTION(mktime)
{
	_php3_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(gmmktime)
{
	_php3_mktime(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

static void
_php3_date(INTERNAL_FUNCTION_PARAMETERS, int gm)
{
	pval *format, *timestamp;
	time_t the_time;
	struct tm *ta;
	int i, size = 0, length, h;
	char tmp_buff[16];

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &format) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		the_time = time(NULL);
		break;
	case 2:
		if (getParameters(ht, 2, &format, &timestamp) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(timestamp);
		the_time = timestamp->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(format);

	if (gm) {
		ta = gmtime(&the_time);
	} else {
		ta = localtime(&the_time);
	}

	if (!ta) {			/* that really shouldn't happen... */
		php3_error(E_WARNING, "unexpected error in date()");
		RETURN_FALSE;
	}
	for (i = 0; i < format->value.str.len; i++) {
		switch (format->value.str.val[i]) {
			case 'U':		/* seconds since the epoch */
				size += 10;
				break;
			case 'F':		/* month, textual, full */
			case 'l':		/* day (of the week), textual */
				size += 9;
				break;
			case 'Y':		/* year, numeric, 4 digits */
				size += 4;
				break;
			case 'M':		/* month, textual, 3 letters */
			case 'D':		/* day, textual, 3 letters */
			case 'z':		/* day of the year */
				size += 3;
				break;
			case 'y':		/* year, numeric, 2 digits */
			case 'm':		/* month, numeric */
			case 'd':		/* day of the month, numeric */
			case 'j':		/* day of the month, numeric, no leading zeros */
			case 'H':		/* hour, numeric, 24 hour format */
			case 'h':		/* hour, numeric, 12 hour format */
			case 'i':		/* minutes, numeric */
			case 's':		/* seconds, numeric */
			case 'A':		/* AM/PM */
			case 'a':		/* am/pm */
			case 'S':		/* standard english suffix for the day of the month (e.g. 3rd, 2nd, etc) */
				size += 2;
				break;
			case '\\':
				if(i < format->value.str.len-1) {
					i++;
				}
			case 'w':		/* day of the week, numeric */
			default:
				size++;
				break;
		}
	}

	return_value->value.str.val = (char *) emalloc(size + 1);
	return_value->value.str.val[0] = '\0';

	for (i = 0; i < format->value.str.len; i++) {
		switch (format->value.str.val[i]) {
			case '\\':
				if(i < format->value.str.len-1) {
					char ch[2];
					ch[0]=format->value.str.val[i+1];
					ch[1]='\0';
					strcat(return_value->value.str.val, ch);
					i++;
				}
				break;
			case 'U':		/* seconds since the epoch */
				sprintf(tmp_buff, "%ld", (long)the_time); /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'F':		/* month, textual, full */
				strcat(return_value->value.str.val, mon_full_names[ta->tm_mon]);
				break;
			case 'l':		/* day (of the week), textual, full */
				strcat(return_value->value.str.val, day_full_names[ta->tm_wday]);
				break;
			case 'Y':		/* year, numeric, 4 digits */
				sprintf(tmp_buff, "%d", ta->tm_year + 1900);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'M':		/* month, textual, 3 letters */
				strcat(return_value->value.str.val, mon_short_names[ta->tm_mon]);
				break;
			case 'D':		/* day (of the week), textual, 3 letters */
				strcat(return_value->value.str.val, day_short_names[ta->tm_wday]);
				break;
			case 'z':		/* day (of the year) */
				sprintf(tmp_buff, "%d", ta->tm_yday);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'y':		/* year, numeric, 2 digits */
				sprintf(tmp_buff, "%02d", ((ta->tm_year)%100));  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'm':		/* month, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_mon + 1);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'd':		/* day of the month, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_mday);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'j':
				sprintf(tmp_buff, "%d", ta->tm_mday); /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'H':		/* hour, numeric, 24 hour format */
				sprintf(tmp_buff, "%02d", ta->tm_hour);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'h':		/* hour, numeric, 12 hour format */
				h = ta->tm_hour % 12; if (h==0) h = 12;
				sprintf(tmp_buff, "%02d", h);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'i':		/* minutes, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_min);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 's':		/* seconds, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_sec);  /* SAFE */ 
				strcat(return_value->value.str.val, tmp_buff);
				break;
			case 'A':		/* AM/PM */
				strcat(return_value->value.str.val, (ta->tm_hour >= 12 ? "PM" : "AM"));
				break;
			case 'a':		/* am/pm */
				strcat(return_value->value.str.val, (ta->tm_hour >= 12 ? "pm" : "am"));
				break;
			case 'S':		/* standard english suffix, e.g. 2nd/3rd for the day of the month */
				if (ta->tm_mday >= 10 && ta->tm_mday <= 19) {
					strcat(return_value->value.str.val, "th");
				} else {
					switch (ta->tm_mday % 10) {
						case 1:
							strcat(return_value->value.str.val, "st");
							break;
						case 2:
							strcat(return_value->value.str.val, "nd");
							break;
						case 3:
							strcat(return_value->value.str.val, "rd");
							break;
						default:
							strcat(return_value->value.str.val, "th");
							break;
					}
				}
				break;
			case 'w':		/* day of the week, numeric EXTENSION */
				sprintf(tmp_buff, "%01d", ta->tm_wday);  /* SAFE */
				strcat(return_value->value.str.val, tmp_buff);
				break;
			default:
				length = strlen(return_value->value.str.val);
				return_value->value.str.val[length] = format->value.str.val[i];
				return_value->value.str.val[length + 1] = '\0';
				break;
		}
	}
	return_value->value.str.len = strlen(return_value->value.str.val);
	return_value->type = IS_STRING;
}

PHP_FUNCTION(date)
{
	_php3_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(gmdate)
{
	_php3_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_FUNCTION(getdate)
{
	pval *timestamp_arg;
	struct tm *ta;
	time_t timestamp;

	if (ARG_COUNT(ht) == 0) {
		timestamp = time(NULL);
	} else if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &timestamp_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_long(timestamp_arg);
		timestamp = timestamp_arg->value.lval;
	}

	ta = localtime(&timestamp);
	if (!ta) {
		php3_error(E_WARNING, "Cannot perform date calculation");
		return;
	}
	if (array_init(return_value) == FAILURE) {
		php3_error(E_ERROR, "Unable to initialize array");
		return;
	}
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

/* Return date string in standard format for http headers */
char *php3_std_date(time_t t)
{
	struct tm *tm1;
	char *str;
	PLS_FETCH();

	tm1 = gmtime(&t);
	str = emalloc(81);
	if (PG(y2k_compliance)) {
		snprintf(str, 80, "%s, %02d-%s-%04d %02d:%02d:%02d GMT",
				day_full_names[tm1->tm_wday],
				tm1->tm_mday,
				mon_short_names[tm1->tm_mon],
				tm1->tm_year+1900,
				tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	} else {
		snprintf(str, 80, "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
				day_full_names[tm1->tm_wday],
				tm1->tm_mday,
				mon_short_names[tm1->tm_mon],
				((tm1->tm_year)%100),
				tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	}
	
	str[79]=0;
	return (str);
}

/* 
 * CheckDate(month, day, year);
 *  returns True(1) if it is valid date
 *
 */
#define isleap(year) (((year%4) == 0 && (year%100)!=0) || (year%400)==0)
PHP_FUNCTION(checkdate)
{
	pval *month, *day, *year;
	int m, d, y;
	
	if (ARG_COUNT(ht) != 3 ||
		getParameters(ht, 3, &month, &day, &year) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(day);
	convert_to_long(month);
	convert_to_long(year);
	y = year->value.lval;
	m = month->value.lval;
	d = day->value.lval;

	if (y < 100)
		y += 1900;

	if (y < 0 || y > 32767) {
		RETURN_FALSE;
	}
	if (m < 1 || m > 12) {
		RETURN_FALSE;
	}
	if (d < 1 || d > phpday_tab[isleap(y)][m - 1]) {
		RETURN_FALSE;
	}
	RETURN_TRUE;				/* True : This month,day,year arguments are valid */
}


#if HAVE_STRFTIME

PHP_FUNCTION(strftime)
{
	pval *format_arg, *timestamp_arg;
	char *format,*buf;
	time_t timestamp;
	struct tm *ta;
	int max_reallocs = 5;
	size_t buf_len=64, real_len;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &format_arg)==FAILURE) {
				RETURN_FALSE;
			}
			time(&timestamp);
			break;
		case 2:
			if (getParameters(ht, 2, &format_arg, &timestamp_arg)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(timestamp_arg);
			timestamp = timestamp_arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	convert_to_string(format_arg);
	if (format_arg->value.str.len==0) {
		RETURN_FALSE;
	}
	format = format_arg->value.str.val;
	ta = localtime(&timestamp);

	buf = (char *) emalloc(buf_len);
	while ((real_len=strftime(buf,buf_len,format,ta))==buf_len || real_len==0) {
		buf_len *= 2;
		buf = (char *) erealloc(buf, buf_len);
		if(!--max_reallocs) break;
	}
	
	if(real_len && real_len != buf_len) {
		buf = (char *) erealloc(buf,real_len+1);
		RETURN_STRINGL(buf, real_len, 0);
	}
	efree(buf);
	RETURN_FALSE;
}
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
