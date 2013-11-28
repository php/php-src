/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

#include "timelib.h"

#include <stdio.h>
#include <ctype.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if defined(_MSC_VER)
# define strtoll(s, f, b) _atoi64(s)
#elif !defined(HAVE_STRTOLL)
# if defined(HAVE_ATOLL)
#  define strtoll(s, f, b) atoll(s)
# else
#  define strtoll(s, f, b) strtol(s, f, b)
# endif
#endif

#define TIMELIB_UNSET   -99999

#define TIMELIB_SECOND  1
#define TIMELIB_MINUTE  2
#define TIMELIB_HOUR    3
#define TIMELIB_DAY     4
#define TIMELIB_MONTH   5
#define TIMELIB_YEAR    6

#define EOI      257

#define TIMELIB_PERIOD  260
#define TIMELIB_ISO_DATE 261
#define TIMELIB_ERROR   999

typedef unsigned char uchar;

#define   BSIZE	   8192

#define   YYCTYPE      uchar
#define   YYCURSOR     cursor
#define   YYLIMIT      s->lim
#define   YYMARKER     s->ptr
#define   YYFILL(n)    return EOI;

#define   RET(i)       {s->cur = cursor; return i;}

#define timelib_string_free free

#define TIMELIB_INIT  s->cur = cursor; str = timelib_string(s); ptr = str
#define TIMELIB_DEINIT timelib_string_free(str)

#ifdef DEBUG_PARSER
#define DEBUG_OUTPUT(s) printf("%s\n", s);
#define YYDEBUG(s,c) { if (s != -1) { printf("state: %d ", s); printf("[%c]\n", c); } }
#else
#define DEBUG_OUTPUT(s)
#define YYDEBUG(s,c)
#endif

#include "timelib_structs.h"

typedef struct Scanner {
	int           fd;
	uchar        *lim, *str, *ptr, *cur, *tok, *pos;
	unsigned int  line, len;
	struct timelib_error_container *errors;

	struct timelib_time     *begin;
	struct timelib_time     *end;
	struct timelib_rel_time *period;
	int                      recurrences;

	int have_period;
	int have_recurrences;
	int have_date;
	int have_begin_date;
	int have_end_date;
} Scanner;

static void add_warning(Scanner *s, char *error)
{
	s->errors->warning_count++;
	s->errors->warning_messages = realloc(s->errors->warning_messages, s->errors->warning_count * sizeof(timelib_error_message));
	s->errors->warning_messages[s->errors->warning_count - 1].position = s->tok ? s->tok - s->str : 0;
	s->errors->warning_messages[s->errors->warning_count - 1].character = s->tok ? *s->tok : 0;
	s->errors->warning_messages[s->errors->warning_count - 1].message = strdup(error);
}

static void add_error(Scanner *s, char *error)
{
	s->errors->error_count++;
	s->errors->error_messages = realloc(s->errors->error_messages, s->errors->error_count * sizeof(timelib_error_message));
	s->errors->error_messages[s->errors->error_count - 1].position = s->tok ? s->tok - s->str : 0;
	s->errors->error_messages[s->errors->error_count - 1].character = s->tok ? *s->tok : 0;
	s->errors->error_messages[s->errors->error_count - 1].message = strdup(error);
}

static char *timelib_string(Scanner *s)
{
	char *tmp = calloc(1, s->cur - s->tok + 1);
	memcpy(tmp, s->tok, s->cur - s->tok);

	return tmp;
}

static timelib_sll timelib_get_nr(char **ptr, int max_length)
{
	char *begin, *end, *str;
	timelib_sll tmp_nr = TIMELIB_UNSET;
	int len = 0;

	while ((**ptr < '0') || (**ptr > '9')) {
		if (**ptr == '\0') {
			return TIMELIB_UNSET;
		}
		++*ptr;
	}
	begin = *ptr;
	while ((**ptr >= '0') && (**ptr <= '9') && len < max_length) {
		++*ptr;
		++len;
	}
	end = *ptr;
	str = calloc(1, end - begin + 1);
	memcpy(str, begin, end - begin);
	tmp_nr = strtoll(str, NULL, 10);
	free(str);
	return tmp_nr;
}

static timelib_ull timelib_get_unsigned_nr(char **ptr, int max_length)
{
	timelib_ull dir = 1;

	while (((**ptr < '0') || (**ptr > '9')) && (**ptr != '+') && (**ptr != '-')) {
		if (**ptr == '\0') {
			return TIMELIB_UNSET;
		}
		++*ptr;
	}

	while (**ptr == '+' || **ptr == '-')
	{
		if (**ptr == '-') {
			dir *= -1;
		}
		++*ptr;
	}
	return dir * timelib_get_nr(ptr, max_length);
}

static void timelib_eat_spaces(char **ptr)
{
	while (**ptr == ' ' || **ptr == '\t') {
		++*ptr;
	}
}

static void timelib_eat_until_separator(char **ptr)
{
	while (strchr(" \t.,:;/-0123456789", **ptr) == NULL) {
		++*ptr;
	}
}

static long timelib_get_zone(char **ptr, int *dst, timelib_time *t, int *tz_not_found, const timelib_tzdb *tzdb)
{
	long retval = 0;

	*tz_not_found = 0;

	while (**ptr == ' ' || **ptr == '\t' || **ptr == '(') {
		++*ptr;
	}
	if ((*ptr)[0] == 'G' && (*ptr)[1] == 'M' && (*ptr)[2] == 'T' && ((*ptr)[3] == '+' || (*ptr)[3] == '-')) {
		*ptr += 3;
	}
	if (**ptr == '+') {
		++*ptr;
		t->is_localtime = 1;
		t->zone_type = TIMELIB_ZONETYPE_OFFSET;
		*tz_not_found = 0;
		t->dst = 0;

		retval = -1 * timelib_parse_tz_cor(ptr);
	} else if (**ptr == '-') {
		++*ptr;
		t->is_localtime = 1;
		t->zone_type = TIMELIB_ZONETYPE_OFFSET;
		*tz_not_found = 0;
		t->dst = 0;

		retval = timelib_parse_tz_cor(ptr);
	}
	while (**ptr == ')') {
		++*ptr;
	}
	return retval;
}

#define timelib_split_free(arg) {       \
	int i;                         \
	for (i = 0; i < arg.c; i++) {  \
		free(arg.v[i]);            \
	}                              \
	if (arg.v) {                   \
		free(arg.v);               \
	}                              \
}

/* date parser's scan function too large for VC6 - VC7.x
   drop the optimization solves the problem */
#ifdef PHP_WIN32
#pragma optimize( "", off )
#endif
static int scan(Scanner *s)
{
	uchar *cursor = s->cur;
	char *str, *ptr = NULL;
		
std:
	s->tok = cursor;
	s->len = 0;
/*!re2c

/* */
any = [\000-\377];
number = [0-9]+;

hour24lz = [01][0-9] | "2"[0-4];
minutelz = [0-5][0-9];
monthlz = "0" [1-9] | "1" [0-2];
monthlzz = "0" [0-9] | "1" [0-2];
daylz   = "0" [1-9] | [1-2][0-9] | "3" [01];
daylzz  = "0" [0-9] | [1-2][0-9] | "3" [01];
secondlz = minutelz;
year4 = [0-9]{4};
weekofyear = "0"[1-9] | [1-4][0-9] | "5"[0-3];

space = [ \t]+;
datetimebasic  = year4 monthlz daylz "T" hour24lz minutelz secondlz "Z";
datetimeextended  = year4 "-" monthlz "-" daylz "T" hour24lz ':' minutelz ':' secondlz "Z";
period   = "P" (number "Y")? (number "M")? (number "W")? (number "D")? ("T" (number "H")? (number "M")? (number "S")?)?;
combinedrep = "P" year4 "-" monthlzz "-" daylzz "T" hour24lz ':' minutelz ':' secondlz;

recurrences = "R" number;

isoweekday       = year4 "-"? "W" weekofyear "-"? [0-7];
isoweek          = year4 "-"? "W" weekofyear;

*/

/*!re2c
	/* so that vim highlights correctly */
	recurrences
	{
		DEBUG_OUTPUT("recurrences");
		TIMELIB_INIT;
		ptr++;
		s->recurrences = timelib_get_unsigned_nr((char **) &ptr, 9);
		TIMELIB_DEINIT;
		s->have_recurrences = 1;
		return TIMELIB_PERIOD;
	}

	datetimebasic| datetimeextended
	{
		timelib_time *current;

		if (s->have_date || s->have_period) {
			current = s->end;
			s->have_end_date = 1;
		} else {
			current = s->begin;
			s->have_begin_date = 1;
		}
		DEBUG_OUTPUT("datetimebasic | datetimeextended");
		TIMELIB_INIT;
		current->y = timelib_get_nr((char **) &ptr, 4);
		current->m = timelib_get_nr((char **) &ptr, 2);
		current->d = timelib_get_nr((char **) &ptr, 2);
		current->h = timelib_get_nr((char **) &ptr, 2);
		current->i = timelib_get_nr((char **) &ptr, 2);
		current->s = timelib_get_nr((char **) &ptr, 2);
		s->have_date = 1;
		TIMELIB_DEINIT;
		return TIMELIB_ISO_DATE;
	}

	period
	{
		timelib_sll nr;
		int         in_time = 0;
		DEBUG_OUTPUT("period");
		TIMELIB_INIT;
		ptr++;
		do {
			if ( *ptr == 'T' ) {
				in_time = 1;
				ptr++;
			}
			if ( *ptr == '\0' ) {
				add_error(s, "Missing expected time part");
				break;
			}

			nr = timelib_get_unsigned_nr((char **) &ptr, 12);
			switch (*ptr) {
				case 'Y': s->period->y = nr; break;
				case 'W': s->period->d = nr * 7; break;
				case 'D': s->period->d = nr; break;
				case 'H': s->period->h = nr; break;
				case 'S': s->period->s = nr; break;
				case 'M': 
					if (in_time) {
						s->period->i = nr;
					} else {
						s->period->m = nr; 
					}
					break;
				default:
					add_error(s, "Undefined period specifier");
					break;
			}
			ptr++;
		} while (!s->errors->error_count && *ptr);
		s->have_period = 1;
		TIMELIB_DEINIT;
		return TIMELIB_PERIOD;
	}

	combinedrep
	{
		DEBUG_OUTPUT("combinedrep");
		TIMELIB_INIT;
		s->period->y = timelib_get_unsigned_nr((char **) &ptr, 4);
		ptr++;
		s->period->m = timelib_get_unsigned_nr((char **) &ptr, 2);
		ptr++;
		s->period->d = timelib_get_unsigned_nr((char **) &ptr, 2);
		ptr++;
		s->period->h = timelib_get_unsigned_nr((char **) &ptr, 2);
		ptr++;
		s->period->i = timelib_get_unsigned_nr((char **) &ptr, 2);
		ptr++;
		s->period->s = timelib_get_unsigned_nr((char **) &ptr, 2);
		s->have_period = 1;
		TIMELIB_DEINIT;
		return TIMELIB_PERIOD;
	}

	[ .,\t/]
	{
		goto std;
	}

	"\000"|"\n"
	{
		s->pos = cursor; s->line++;
		goto std;
	}

	any
	{
		add_error(s, "Unexpected character");
		goto std;
	}
*/
}
#ifdef PHP_WIN32
#pragma optimize( "", on )
#endif

/*!max:re2c */

void timelib_strtointerval(char *s, int len, 
                           timelib_time **begin, timelib_time **end, 
						   timelib_rel_time **period, int *recurrences, 
						   struct timelib_error_container **errors)
{
	Scanner in;
	int t;
	char *e = s + len - 1;

	memset(&in, 0, sizeof(in));
	in.errors = malloc(sizeof(struct timelib_error_container));
	in.errors->warning_count = 0;
	in.errors->warning_messages = NULL;
	in.errors->error_count = 0;
	in.errors->error_messages = NULL;

	if (len > 0) {
		while (isspace(*s) && s < e) {
			s++;
		}
		while (isspace(*e) && e > s) {
			e--;
		}
	}
	if (e - s < 0) {
		add_error(&in, "Empty string");
		if (errors) {
			*errors = in.errors;
		} else {
			timelib_error_container_dtor(in.errors);
		}
		return;
	}
	e++;

	/* init cursor */
	in.str = malloc((e - s) + YYMAXFILL);
	memset(in.str, 0, (e - s) + YYMAXFILL);
	memcpy(in.str, s, (e - s));
	in.lim = in.str + (e - s) + YYMAXFILL;
	in.cur = in.str;

	/* init value containers */
	in.begin = timelib_time_ctor();
	in.begin->y = TIMELIB_UNSET;
	in.begin->d = TIMELIB_UNSET;
	in.begin->m = TIMELIB_UNSET;
	in.begin->h = TIMELIB_UNSET;
	in.begin->i = TIMELIB_UNSET;
	in.begin->s = TIMELIB_UNSET;
	in.begin->f = 0;
	in.begin->z = 0;
	in.begin->dst = 0;
	in.begin->is_localtime = 0;
	in.begin->zone_type = TIMELIB_ZONETYPE_OFFSET;

	in.end = timelib_time_ctor();
	in.end->y = TIMELIB_UNSET;
	in.end->d = TIMELIB_UNSET;
	in.end->m = TIMELIB_UNSET;
	in.end->h = TIMELIB_UNSET;
	in.end->i = TIMELIB_UNSET;
	in.end->s = TIMELIB_UNSET;
	in.end->f = 0;
	in.end->z = 0;
	in.end->dst = 0;
	in.end->is_localtime = 0;
	in.end->zone_type = TIMELIB_ZONETYPE_OFFSET;

	in.period = timelib_rel_time_ctor();
	in.period->y = 0;
	in.period->d = 0;
	in.period->m = 0;
	in.period->h = 0;
	in.period->i = 0;
	in.period->s = 0;
	in.period->weekday = 0;
	in.period->weekday_behavior = 0;
	in.period->first_last_day_of = 0;
	in.period->days = TIMELIB_UNSET;

	in.recurrences = 1;

	do {
		t = scan(&in);
#ifdef DEBUG_PARSER
		printf("%d\n", t);
#endif
	} while(t != EOI);

	free(in.str);
	if (errors) {
		*errors = in.errors;
	} else {
		timelib_error_container_dtor(in.errors);
	}
	if (in.have_begin_date) {
		*begin = in.begin;
	} else {
		timelib_time_dtor(in.begin);
	}
	if (in.have_end_date) {
		*end   = in.end;
	} else {
		timelib_time_dtor(in.end);
	}
	if (in.have_period) {
		*period = in.period;
	} else {
		timelib_rel_time_dtor(in.period);
	}
	if (in.have_recurrences) {
		*recurrences = in.recurrences;
	}
}


/*
 * vim: syntax=c
 */
