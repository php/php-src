/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Derick Rethans
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "timelib.h"
#include "timelib_private.h"

#include <ctype.h>

#if defined(_MSC_VER)
# define strtoll(s, f, b) _atoi64(s)
#elif !defined(HAVE_STRTOLL)
# if defined(HAVE_ATOLL)
#  define strtoll(s, f, b) atoll(s)
# else
#  define strtoll(s, f, b) strtol(s, f, b)
# endif
#endif

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

#define timelib_string_free timelib_free

#define TIMELIB_INIT  s->cur = cursor; str = timelib_string(s); ptr = str
#define TIMELIB_DEINIT timelib_string_free(str)

#ifdef DEBUG_PARSER
#define DEBUG_OUTPUT(s) printf("%s\n", s);
#define YYDEBUG(s,c) { if (s != -1) { printf("state: %d ", s); printf("[%c]\n", c); } }
#else
#define DEBUG_OUTPUT(s)
#define YYDEBUG(s,c)
#endif

typedef struct _Scanner {
	int           fd;
	uchar        *lim, *str, *ptr, *cur, *tok, *pos;
	unsigned int  line, len;
	timelib_error_container *errors;

	timelib_time     *begin;
	timelib_time     *end;
	timelib_rel_time *period;
	int               recurrences;

	int have_period;
	int have_recurrences;
	int have_date;
	int have_begin_date;
	int have_end_date;
} Scanner;

static void add_error(Scanner *s, const char *error)
{
	s->errors->error_count++;
	s->errors->error_messages = timelib_realloc(s->errors->error_messages, s->errors->error_count * sizeof(timelib_error_message));
	s->errors->error_messages[s->errors->error_count - 1].position = s->tok ? s->tok - s->str : 0;
	s->errors->error_messages[s->errors->error_count - 1].character = s->tok ? *s->tok : 0;
	s->errors->error_messages[s->errors->error_count - 1].message = timelib_strdup(error);
}

static char *timelib_string(Scanner *s)
{
	char *tmp = timelib_calloc(1, s->cur - s->tok + 1);
	memcpy(tmp, s->tok, s->cur - s->tok);

	return tmp;
}

static timelib_sll timelib_get_nr(const char **ptr, int max_length)
{
	const char *begin, *end;
	char *str;
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
	str = timelib_calloc(1, end - begin + 1);
	memcpy(str, begin, end - begin);
	tmp_nr = strtoll(str, NULL, 10);
	timelib_free(str);
	return tmp_nr;
}

static timelib_ull timelib_get_unsigned_nr(const char **ptr, int max_length)
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

#define timelib_split_free(arg) {       \
	int i;                         \
	for (i = 0; i < arg.c; i++) {  \
		timelib_free(arg.v[i]);    \
	}                              \
	if (arg.v) {                   \
		timelib_free(arg.v);       \
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
	char *str;
	const char *ptr = NULL;

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
		s->recurrences = timelib_get_unsigned_nr(&ptr, 9);
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
		current->y = timelib_get_nr(&ptr, 4);
		current->m = timelib_get_nr(&ptr, 2);
		current->d = timelib_get_nr(&ptr, 2);
		current->h = timelib_get_nr(&ptr, 2);
		current->i = timelib_get_nr(&ptr, 2);
		current->s = timelib_get_nr(&ptr, 2);
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

			nr = timelib_get_unsigned_nr(&ptr, 12);
			switch (*ptr) {
				case 'Y': s->period->y = nr; break;
				case 'W': s->period->d += nr * 7; break;
				case 'D': s->period->d += nr; break;
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
		s->period->y = timelib_get_unsigned_nr(&ptr, 4);
		ptr++;
		s->period->m = timelib_get_unsigned_nr(&ptr, 2);
		ptr++;
		s->period->d = timelib_get_unsigned_nr(&ptr, 2);
		ptr++;
		s->period->h = timelib_get_unsigned_nr(&ptr, 2);
		ptr++;
		s->period->i = timelib_get_unsigned_nr(&ptr, 2);
		ptr++;
		s->period->s = timelib_get_unsigned_nr(&ptr, 2);
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

void timelib_strtointerval(const char *s, size_t len,
                           timelib_time **begin, timelib_time **end,
						   timelib_rel_time **period, int *recurrences,
						   timelib_error_container **errors)
{
	Scanner in;
	int t;
	const char *e = s + len - 1;

	memset(&in, 0, sizeof(in));
	in.errors = timelib_malloc(sizeof(timelib_error_container));
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
	in.str = timelib_malloc((e - s) + YYMAXFILL);
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
	in.begin->us = 0;
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
	in.end->us = 0;
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

	timelib_free(in.str);
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
