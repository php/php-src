/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2023 Derick Rethans
 * Copyright (c) 2018 MongoDB, Inc.
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
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

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
#define TIME     258
#define DATE     259

#define TIMELIB_XMLRPC_SOAP    260
#define TIMELIB_TIME12         261
#define TIMELIB_TIME24         262
#define TIMELIB_GNU_NOCOLON    263
#define TIMELIB_GNU_NOCOLON_TZ 264
#define TIMELIB_ISO_NOCOLON    265

#define TIMELIB_AMERICAN       266
#define TIMELIB_ISO_DATE       267
#define TIMELIB_DATE_FULL      268
#define TIMELIB_DATE_TEXT      269
#define TIMELIB_DATE_NOCOLON   270
#define TIMELIB_PG_YEARDAY     271
#define TIMELIB_PG_TEXT        272
#define TIMELIB_PG_REVERSE     273
#define TIMELIB_CLF            274
#define TIMELIB_DATE_NO_DAY    275
#define TIMELIB_SHORTDATE_WITH_TIME 276
#define TIMELIB_DATE_FULL_POINTED 277
#define TIMELIB_TIME24_WITH_ZONE 278
#define TIMELIB_ISO_WEEK       279
#define TIMELIB_LF_DAY_OF_MONTH 280
#define TIMELIB_WEEK_DAY_OF_MONTH 281

#define TIMELIB_TIMEZONE       300
#define TIMELIB_AGO            301

#define TIMELIB_RELATIVE       310

#define TIMELIB_ERROR          999

/* Some compilers like AIX, defines uchar in sys/types.h */
#undef uchar
typedef unsigned char uchar;

#define   BSIZE	   8192

#define   YYCTYPE      uchar
#define   YYCURSOR     cursor
#define   YYLIMIT      s->lim
#define   YYMARKER     s->ptr
#define   YYFILL(n)    return EOI;

#define   RET(i)       {s->cur = cursor; return i;}

#define timelib_string_free timelib_free

#define TIMELIB_HAVE_TIME() { if (s->time->have_time) { add_error(s, TIMELIB_ERR_DOUBLE_TIME, "Double time specification"); timelib_string_free(str); return TIMELIB_ERROR; } else { s->time->have_time = 1; s->time->h = 0; s->time->i = 0; s->time->s = 0; s->time->us = 0; } }
#define TIMELIB_UNHAVE_TIME() { s->time->have_time = 0; s->time->h = 0; s->time->i = 0; s->time->s = 0; s->time->us = 0; }
#define TIMELIB_HAVE_DATE() { if (s->time->have_date) { add_error(s, TIMELIB_ERR_DOUBLE_DATE, "Double date specification"); timelib_string_free(str); return TIMELIB_ERROR; } else { s->time->have_date = 1; } }
#define TIMELIB_UNHAVE_DATE() { s->time->have_date = 0; s->time->d = 0; s->time->m = 0; s->time->y = 0; }
#define TIMELIB_HAVE_RELATIVE() { s->time->have_relative = 1; }
#define TIMELIB_HAVE_WEEKDAY_RELATIVE() { s->time->have_relative = 1; s->time->relative.have_weekday_relative = 1; }
#define TIMELIB_HAVE_SPECIAL_RELATIVE() { s->time->have_relative = 1; s->time->relative.have_special_relative = 1; }
#define TIMELIB_HAVE_TZ() { s->cur = cursor; if (s->time->have_zone) { s->time->have_zone > 1 ? add_error(s, TIMELIB_ERR_DOUBLE_TZ, "Double timezone specification") : add_warning(s, TIMELIB_WARN_DOUBLE_TZ, "Double timezone specification"); timelib_string_free(str); s->time->have_zone++; return TIMELIB_ERROR; } else { s->time->have_zone++; } }

#define TIMELIB_INIT  s->cur = cursor; str = timelib_string(s); ptr = str
#define TIMELIB_DEINIT timelib_string_free(str)
#define TIMELIB_ADJUST_RELATIVE_WEEKDAY() if (in->time.have_weekday_relative && (in.rel.d > 0)) { in.rel.d -= 7; }

#define TIMELIB_PROCESS_YEAR(x, l) { \
	if (((x) == TIMELIB_UNSET) || ((l) >= 4)) { \
	/*	(x) = 0; */          \
	} else if ((x) < 100) {  \
		if ((x) < 70) {      \
			(x) += 2000;     \
		} else {             \
			(x) += 1900;     \
		}                    \
	}                        \
}

#ifdef DEBUG_PARSER
#define DEBUG_OUTPUT(s) printf("%s\n", s);
#define YYDEBUG(s,c) { if (s != -1) { printf("state: %d ", s); printf("[%c]\n", c); } }
#else
#define DEBUG_OUTPUT(s)
#define YYDEBUG(s,c)
#endif

typedef struct _timelib_elems {
	unsigned int   c; /* Number of elements */
	char         **v; /* Values */
} timelib_elems;

typedef struct _Scanner {
	int           fd;
	uchar        *lim, *str, *ptr, *cur, *tok, *pos;
	unsigned int  line, len;
	timelib_error_container *errors;

	timelib_time        *time;
	const timelib_tzdb  *tzdb;
} Scanner;

typedef struct _timelib_lookup_table {
	const char *name;
	int         type;
	int         value;
} timelib_lookup_table;

typedef struct _timelib_relunit {
	const char *name;
	int         unit;
	int         multiplier;
} timelib_relunit;

/* The timezone table. */
static const timelib_tz_lookup_table timelib_timezone_lookup[] = {
#include "timezonemap.h"
	{ NULL, 0, 0, NULL },
};

static const timelib_tz_lookup_table timelib_timezone_fallbackmap[] = {
#include "fallbackmap.h"
	{ NULL, 0, 0, NULL },
};

static const timelib_tz_lookup_table timelib_timezone_utc[] = {
	{ "utc", 0, 0, "UTC" },
};

#if defined(_POSIX_TZNAME_MAX)
# define MAX_ABBR_LEN _POSIX_TZNAME_MAX
#elif defined(TZNAME_MAX)
# define MAX_ABBR_LEN TZNAME_MAX
#else
# define MAX_ABBR_LEN 6
#endif

static timelib_relunit const timelib_relunit_lookup[] = {
	{ "ms",           TIMELIB_MICROSEC, 1000 },
	{ "msec",         TIMELIB_MICROSEC, 1000 },
	{ "msecs",        TIMELIB_MICROSEC, 1000 },
	{ "millisecond",  TIMELIB_MICROSEC, 1000 },
	{ "milliseconds", TIMELIB_MICROSEC, 1000 },
	{ "µs",           TIMELIB_MICROSEC,    1 },
	{ "usec",         TIMELIB_MICROSEC,    1 },
	{ "usecs",        TIMELIB_MICROSEC,    1 },
	{ "µsec",         TIMELIB_MICROSEC,    1 },
	{ "µsecs",        TIMELIB_MICROSEC,    1 },
	{ "microsecond",  TIMELIB_MICROSEC,    1 },
	{ "microseconds", TIMELIB_MICROSEC,    1 },
	{ "sec",         TIMELIB_SECOND,  1 },
	{ "secs",        TIMELIB_SECOND,  1 },
	{ "second",      TIMELIB_SECOND,  1 },
	{ "seconds",     TIMELIB_SECOND,  1 },
	{ "min",         TIMELIB_MINUTE,  1 },
	{ "mins",        TIMELIB_MINUTE,  1 },
	{ "minute",      TIMELIB_MINUTE,  1 },
	{ "minutes",     TIMELIB_MINUTE,  1 },
	{ "hour",        TIMELIB_HOUR,    1 },
	{ "hours",       TIMELIB_HOUR,    1 },
	{ "day",         TIMELIB_DAY,     1 },
	{ "days",        TIMELIB_DAY,     1 },
	{ "week",        TIMELIB_DAY,     7 },
	{ "weeks",       TIMELIB_DAY,     7 },
	{ "fortnight",   TIMELIB_DAY,    14 },
	{ "fortnights",  TIMELIB_DAY,    14 },
	{ "forthnight",  TIMELIB_DAY,    14 },
	{ "forthnights", TIMELIB_DAY,    14 },
	{ "month",       TIMELIB_MONTH,   1 },
	{ "months",      TIMELIB_MONTH,   1 },
	{ "year",        TIMELIB_YEAR,    1 },
	{ "years",       TIMELIB_YEAR,    1 },

	{ "mondays",     TIMELIB_WEEKDAY, 1 },
	{ "monday",      TIMELIB_WEEKDAY, 1 },
	{ "mon",         TIMELIB_WEEKDAY, 1 },
	{ "tuesdays",    TIMELIB_WEEKDAY, 2 },
	{ "tuesday",     TIMELIB_WEEKDAY, 2 },
	{ "tue",         TIMELIB_WEEKDAY, 2 },
	{ "wednesdays",  TIMELIB_WEEKDAY, 3 },
	{ "wednesday",   TIMELIB_WEEKDAY, 3 },
	{ "wed",         TIMELIB_WEEKDAY, 3 },
	{ "thursdays",   TIMELIB_WEEKDAY, 4 },
	{ "thursday",    TIMELIB_WEEKDAY, 4 },
	{ "thu",         TIMELIB_WEEKDAY, 4 },
	{ "fridays",     TIMELIB_WEEKDAY, 5 },
	{ "friday",      TIMELIB_WEEKDAY, 5 },
	{ "fri",         TIMELIB_WEEKDAY, 5 },
	{ "saturdays",   TIMELIB_WEEKDAY, 6 },
	{ "saturday",    TIMELIB_WEEKDAY, 6 },
	{ "sat",         TIMELIB_WEEKDAY, 6 },
	{ "sundays",     TIMELIB_WEEKDAY, 0 },
	{ "sunday",      TIMELIB_WEEKDAY, 0 },
	{ "sun",         TIMELIB_WEEKDAY, 0 },

	{ "weekday",     TIMELIB_SPECIAL, TIMELIB_SPECIAL_WEEKDAY },
	{ "weekdays",    TIMELIB_SPECIAL, TIMELIB_SPECIAL_WEEKDAY },
	{ NULL,          0,          0 }
};

/* The relative text table. */
static timelib_lookup_table const timelib_reltext_lookup[] = {
	{ "first",    0,  1 },
	{ "next",     0,  1 },
	{ "second",   0,  2 },
	{ "third",    0,  3 },
	{ "fourth",   0,  4 },
	{ "fifth",    0,  5 },
	{ "sixth",    0,  6 },
	{ "seventh",  0,  7 },
	{ "eight",    0,  8 },
	{ "eighth",   0,  8 },
	{ "ninth",    0,  9 },
	{ "tenth",    0, 10 },
	{ "eleventh", 0, 11 },
	{ "twelfth",  0, 12 },
	{ "last",     0, -1 },
	{ "previous", 0, -1 },
	{ "this",     1,  0 },
	{ NULL,       1,  0 }
};

/* The month table. */
static timelib_lookup_table const timelib_month_lookup[] = {
	{ "jan",  0,  1 },
	{ "feb",  0,  2 },
	{ "mar",  0,  3 },
	{ "apr",  0,  4 },
	{ "may",  0,  5 },
	{ "jun",  0,  6 },
	{ "jul",  0,  7 },
	{ "aug",  0,  8 },
	{ "sep",  0,  9 },
	{ "sept", 0,  9 },
	{ "oct",  0, 10 },
	{ "nov",  0, 11 },
	{ "dec",  0, 12 },
	{ "i",    0,  1 },
	{ "ii",   0,  2 },
	{ "iii",  0,  3 },
	{ "iv",   0,  4 },
	{ "v",    0,  5 },
	{ "vi",   0,  6 },
	{ "vii",  0,  7 },
	{ "viii", 0,  8 },
	{ "ix",   0,  9 },
	{ "x",    0, 10 },
	{ "xi",   0, 11 },
	{ "xii",  0, 12 },

	{ "january",   0,  1 },
	{ "february",  0,  2 },
	{ "march",     0,  3 },
	{ "april",     0,  4 },
	{ "may",       0,  5 },
	{ "june",      0,  6 },
	{ "july",      0,  7 },
	{ "august",    0,  8 },
	{ "september", 0,  9 },
	{ "october",   0, 10 },
	{ "november",  0, 11 },
	{ "december",  0, 12 },
	{  NULL,       0,  0 }
};

#if 0
static char* timelib_ltrim(char *s)
{
	char *ptr = s;
	while (ptr[0] == ' ' || ptr[0] == '\t') {
		ptr++;
	}
	return ptr;
}
#endif

#if 0
uchar *fill(Scanner *s, uchar *cursor){
	if(!s->eof){
		unsigned int cnt = s->tok - s->bot;
		if(cnt){
			memcpy(s->bot, s->tok, s->lim - s->tok);
			s->tok = s->bot;
			s->ptr -= cnt;
			cursor -= cnt;
			s->pos -= cnt;
			s->lim -= cnt;
		}
		if((s->top - s->lim) < BSIZE){
			uchar *buf = (uchar*) timelib_malloc(((s->lim - s->bot) + BSIZE)*sizeof(uchar));
			memcpy(buf, s->tok, s->lim - s->tok);
			s->tok = buf;
			s->ptr = &buf[s->ptr - s->bot];
			cursor = &buf[cursor - s->bot];
			s->pos = &buf[s->pos - s->bot];
			s->lim = &buf[s->lim - s->bot];
			s->top = &s->lim[BSIZE];
			timelib_free(s->bot);
			s->bot = buf;
		}
		if((cnt = read(s->fd, (char*) s->lim, BSIZE)) != BSIZE){
			s->eof = &s->lim[cnt]; *(s->eof)++ = '\n';
		}
		s->lim += cnt;
	}
	return cursor;
}
#endif

static timelib_error_message *alloc_error_message(timelib_error_message **messages, int *count)
{
	/* Realloc in power of two increments */
	int is_pow2 = (*count & (*count - 1)) == 0;

	if (is_pow2) {
		size_t alloc_size = *count ? (*count * 2) : 1;

		*messages = timelib_realloc(*messages, alloc_size * sizeof(timelib_error_message));
	}
	return *messages + (*count)++;
}

static void add_warning(Scanner *s, int error_code, const char *error)
{
	timelib_error_message *message = alloc_error_message(&s->errors->warning_messages, &s->errors->warning_count);

	message->error_code = error_code;
	message->position = s->tok ? s->tok - s->str : 0;
	message->character = s->tok ? *s->tok : 0;
	message->message = timelib_strdup(error);
}

static void add_error(Scanner *s, int error_code, const char *error)
{
	timelib_error_message *message = alloc_error_message(&s->errors->error_messages, &s->errors->error_count);

	message->error_code = error_code;
	message->position = s->tok ? s->tok - s->str : 0;
	message->character = s->tok ? *s->tok : 0;
	message->message = timelib_strdup(error);
}

static void add_pbf_warning(Scanner *s, int error_code, const char *error, const char *sptr, const char *cptr)
{
	timelib_error_message *message = alloc_error_message(&s->errors->warning_messages, &s->errors->warning_count);

	message->error_code = error_code;
	message->position = cptr - sptr;
	message->character = *cptr;
	message->message = timelib_strdup(error);
}

static void add_pbf_error(Scanner *s, int error_code, const char *error, const char *sptr, const char *cptr)
{
	timelib_error_message *message = alloc_error_message(&s->errors->error_messages, &s->errors->error_count);

	message->error_code = error_code;
	message->position = cptr - sptr;
	message->character = *cptr;
	message->message = timelib_strdup(error);
}

static timelib_sll timelib_meridian(const char **ptr, timelib_sll h)
{
	timelib_sll retval = 0;

	while (!strchr("AaPp", **ptr)) {
		++*ptr;
	}
	if (**ptr == 'a' || **ptr == 'A') {
		if (h == 12) {
			retval = -12;
		}
	} else if (h != 12) {
		retval = 12;
	}
	++*ptr;
	if (**ptr == '.') {
		++*ptr;
	}
	if (**ptr == 'M' || **ptr == 'm') {
		++*ptr;
	}
	if (**ptr == '.') {
		++*ptr;
	}
	return retval;
}

static timelib_sll timelib_meridian_with_check(const char **ptr, timelib_sll h)
{
	timelib_sll retval = 0;

	while (**ptr && !strchr("AaPp", **ptr)) {
		++*ptr;
	}
	if(!**ptr) {
		return TIMELIB_UNSET;
	}
	if (**ptr == 'a' || **ptr == 'A') {
		if (h == 12) {
			retval = -12;
		}
	} else if (h != 12) {
		retval = 12;
	}
	++*ptr;
	if (**ptr == '.') {
		++*ptr;
		if (**ptr != 'm' && **ptr != 'M') {
			return TIMELIB_UNSET;
		}
		++*ptr;
		if (**ptr != '.' ) {
			return TIMELIB_UNSET;
		}
		++*ptr;
	} else if (**ptr == 'm' || **ptr == 'M') {
		++*ptr;
	} else {
		return TIMELIB_UNSET;
	}
	return retval;
}

static char *timelib_string(Scanner *s)
{
	char *tmp = timelib_calloc(1, s->cur - s->tok + 1);
	memcpy(tmp, s->tok, s->cur - s->tok);

	return tmp;
}

static timelib_sll timelib_get_nr_ex(const char **ptr, int max_length, int *scanned_length)
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
	if (scanned_length) {
		*scanned_length = end - begin;
	}
	str = timelib_calloc(1, end - begin + 1);
	memcpy(str, begin, end - begin);
	tmp_nr = strtoll(str, NULL, 10);
	timelib_free(str);
	return tmp_nr;
}

static timelib_sll timelib_get_nr(const char **ptr, int max_length)
{
	return timelib_get_nr_ex(ptr, max_length, NULL);
}

static void timelib_skip_day_suffix(const char **ptr)
{
	if (isspace(**ptr)) {
		return;
	}
	if (!timelib_strncasecmp(*ptr, "nd", 2) || !timelib_strncasecmp(*ptr, "rd", 2) ||!timelib_strncasecmp(*ptr, "st", 2) || !timelib_strncasecmp(*ptr, "th", 2)) {
		*ptr += 2;
	}
}

static timelib_sll timelib_get_frac_nr(const char **ptr)
{
	const char *begin, *end;
	char *str;
	double tmp_nr = TIMELIB_UNSET;

	while ((**ptr != '.') && (**ptr != ':') && ((**ptr < '0') || (**ptr > '9'))) {
		if (**ptr == '\0') {
			return TIMELIB_UNSET;
		}
		++*ptr;
	}
	begin = *ptr;
	while ((**ptr == '.') || (**ptr == ':') || ((**ptr >= '0') && (**ptr <= '9'))) {
		++*ptr;
	}
	end = *ptr;
	str = timelib_calloc(1, end - begin);
	memcpy(str, begin + 1, end - begin - 1);
	tmp_nr = strtod(str, NULL) * pow(10, 7 - (end - begin));
	timelib_free(str);
	return tmp_nr;
}

static timelib_ull timelib_get_signed_nr(Scanner *s, const char **ptr, int max_length)
{
	char *str, *str_ptr;
	timelib_sll tmp_nr = 0;
	int len = 0;

	str = timelib_calloc(1, max_length + 2); // for sign and \0
	str_ptr = str;

	while (((**ptr < '0') || (**ptr > '9')) && (**ptr != '+') && (**ptr != '-')) {
		if (**ptr == '\0') {
			add_error(s, TIMELIB_ERR_UNEXPECTED_DATA, "Found unexpected data");
			timelib_free(str);
			return 0;
		}
		++*ptr;
	}

	if ((**ptr == '+') || (**ptr == '-')) {
		*str_ptr = **ptr;
		++*ptr;
		++str_ptr;
	}

	while (((**ptr < '0') || (**ptr > '9'))) {
		if (**ptr == '\0') {
			timelib_free(str);
			add_error(s, TIMELIB_ERR_UNEXPECTED_DATA, "Found unexpected data");
			return 0;
		}
		++*ptr;
	}

	while ((**ptr >= '0') && (**ptr <= '9') && len < max_length) {
		*str_ptr = **ptr;
		++*ptr;
		++str_ptr;
		++len;
	}

	errno = 0;
	tmp_nr = strtoll(str, NULL, 10);
	if (errno == ERANGE) {
		timelib_free(str);
		add_error(s, TIMELIB_ERR_NUMBER_OUT_OF_RANGE, "Number out of range");
		return 0;
	}

	timelib_free(str);

	return tmp_nr;
}

static timelib_sll timelib_lookup_relative_text(const char **ptr, int *behavior)
{
	char *word;
	const char *begin = *ptr, *end;
	timelib_sll  value = 0;
	const timelib_lookup_table *tp;

	while ((**ptr >= 'A' && **ptr <= 'Z') || (**ptr >= 'a' && **ptr <= 'z')) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	for (tp = timelib_reltext_lookup; tp->name; tp++) {
		if (timelib_strcasecmp(word, tp->name) == 0) {
			value = tp->value;
			*behavior = tp->type;
		}
	}

	timelib_free(word);
	return value;
}

static timelib_sll timelib_get_relative_text(const char **ptr, int *behavior)
{
	while (**ptr == ' ' || **ptr == '\t' || **ptr == '-' || **ptr == '/') {
		++*ptr;
	}
	return timelib_lookup_relative_text(ptr, behavior);
}

static timelib_long timelib_lookup_month(const char **ptr)
{
	char *word;
	const char *begin = *ptr, *end;
	timelib_long  value = 0;
	const timelib_lookup_table *tp;

	while ((**ptr >= 'A' && **ptr <= 'Z') || (**ptr >= 'a' && **ptr <= 'z')) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	for (tp = timelib_month_lookup; tp->name; tp++) {
		if (timelib_strcasecmp(word, tp->name) == 0) {
			value = tp->value;
		}
	}

	timelib_free(word);
	return value;
}

static timelib_long timelib_get_month(const char **ptr)
{
	while (**ptr == ' ' || **ptr == '\t' || **ptr == '-' || **ptr == '.' || **ptr == '/') {
		++*ptr;
	}
	return timelib_lookup_month(ptr);
}

static void timelib_eat_spaces(const char **ptr)
{
	do {
		if (**ptr == ' ' || **ptr == '\t') {
			++*ptr;
			continue;
		}
		if ((*ptr)[0] == '\xe2' && (*ptr)[1] == '\x80' && (*ptr)[2] == '\xaf') { // NNBSP
			*ptr += 3;
			continue;
		}
		if ((*ptr)[0] == '\xc2' && (*ptr)[1] == '\xa0') { // NBSP
			*ptr += 2;
			continue;
		}
		break;
	} while (true);
}

static void timelib_eat_until_separator(const char **ptr)
{
	++*ptr;
	while (strchr(" \t.,:;/-0123456789", **ptr) == NULL) {
		++*ptr;
	}
}

static const timelib_relunit* timelib_lookup_relunit(const char **ptr)
{
	char *word;
	const char *begin = *ptr, *end;
	const timelib_relunit *tp, *value = NULL;

	while (**ptr != '\0' && **ptr != ' ' && **ptr != ',' && **ptr != '\t' && **ptr != ';' && **ptr != ':' &&
		**ptr != '/' && **ptr != '.' && **ptr != '-' && **ptr != '(' && **ptr != ')' ) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	for (tp = timelib_relunit_lookup; tp->name; tp++) {
		if (timelib_strcasecmp(word, tp->name) == 0) {
			value = tp;
			break;
		}
	}

	timelib_free(word);
	return value;
}

static void add_with_overflow(Scanner *s, timelib_sll *e, timelib_sll amount, int multiplier)
{
#if defined(__has_builtin) && __has_builtin(__builtin_saddll_overflow)
	if (__builtin_saddll_overflow(*e, amount * multiplier, e)) {
		add_error(s, TIMELIB_ERR_NUMBER_OUT_OF_RANGE, "Number out of range");
	}
#else
	*e += (amount * multiplier);
#endif
}

/**
 * The time_part parameter is a flag. It can be TIMELIB_TIME_PART_KEEP in case
 * the time portion should not be reset to midnight, or
 * TIMELIB_TIME_PART_DONT_KEEP in case it does need to be reset. This is used
 * for not overwriting the time portion for 'X weekday'.
 */
static void timelib_set_relative(const char **ptr, timelib_sll amount, int behavior, Scanner *s, int time_part)
{
	const timelib_relunit* relunit;

	if (!(relunit = timelib_lookup_relunit(ptr))) {
		return;
	}

	switch (relunit->unit) {
		case TIMELIB_MICROSEC: add_with_overflow(s, &s->time->relative.us, amount, relunit->multiplier); break;
		case TIMELIB_SECOND:   add_with_overflow(s, &s->time->relative.s, amount, relunit->multiplier); break;
		case TIMELIB_MINUTE:   add_with_overflow(s, &s->time->relative.i, amount, relunit->multiplier); break;
		case TIMELIB_HOUR:     add_with_overflow(s, &s->time->relative.h, amount, relunit->multiplier); break;
		case TIMELIB_DAY:      add_with_overflow(s, &s->time->relative.d, amount, relunit->multiplier); break;
		case TIMELIB_MONTH:    add_with_overflow(s, &s->time->relative.m, amount, relunit->multiplier); break;
		case TIMELIB_YEAR:     add_with_overflow(s, &s->time->relative.y, amount, relunit->multiplier); break;

		case TIMELIB_WEEKDAY:
			TIMELIB_HAVE_WEEKDAY_RELATIVE();
			if (time_part != TIMELIB_TIME_PART_KEEP) {
				TIMELIB_UNHAVE_TIME();
			}
			s->time->relative.d += (amount > 0 ? amount - 1 : amount) * 7;
			s->time->relative.weekday = relunit->multiplier;
			s->time->relative.weekday_behavior = behavior;
			break;

		case TIMELIB_SPECIAL:
			TIMELIB_HAVE_SPECIAL_RELATIVE();
			if (time_part != TIMELIB_TIME_PART_KEEP) {
				TIMELIB_UNHAVE_TIME();
			}
			s->time->relative.special.type = relunit->multiplier;
			s->time->relative.special.amount = amount;
	}
}

static const timelib_tz_lookup_table* abbr_search(const char *word, timelib_long gmtoffset, int isdst)
{
	int first_found = 0;
	const timelib_tz_lookup_table  *tp, *first_found_elem = NULL;
	const timelib_tz_lookup_table  *fmp;

	if (timelib_strcasecmp("utc", word) == 0 || timelib_strcasecmp("gmt", word) == 0) {
		return timelib_timezone_utc;
	}

	for (tp = timelib_timezone_lookup; tp->name; tp++) {
		if (timelib_strcasecmp(word, tp->name) == 0) {
			if (!first_found) {
				first_found = 1;
				first_found_elem = tp;
				if (gmtoffset == -1) {
					return tp;
				}
			}
			if (tp->gmtoffset == gmtoffset) {
				return tp;
			}
		}
	}
	if (first_found) {
		return first_found_elem;
	}

	/* Still didn't find anything, let's find the zone solely based on
	 * offset/isdst then */
	for (fmp = timelib_timezone_fallbackmap; fmp->name; fmp++) {
		if (fmp->gmtoffset == gmtoffset && fmp->type == isdst) {
			return fmp;
		}
	}
	return NULL;
}

static timelib_long timelib_lookup_abbr(const char **ptr, int *dst, char **tz_abbr, int *found)
{
	char *word;
	const char *begin = *ptr, *end;
	timelib_long  value = 0;
	const timelib_tz_lookup_table *tp;

	/* Only include A-Z, a-z, 0-9, /, _, and - in abbreviations/TZ IDs */
	while (
		(**ptr >= 'A' && **ptr <= 'Z') ||
		(**ptr >= 'a' && **ptr <= 'z') ||
		(**ptr >= '0' && **ptr <= '9') ||
		**ptr == '/' || **ptr == '_' || **ptr == '-' || **ptr == '+'
	) {
		++*ptr;
	}
	end = *ptr;
	word = timelib_calloc(1, end - begin + 1);
	memcpy(word, begin, end - begin);

	if (end - begin < MAX_ABBR_LEN && (tp = abbr_search(word, -1, 0))) {
		value = tp->gmtoffset;
		*dst = tp->type;
		value -= tp->type * 3600;
		*found = 1;
	} else {
		*found = 0;
	}

	*tz_abbr = word;
	return value;
}

#define sHOUR(a) (int)(a * 3600)
#define sMIN(a) (int)(a * 60)

static timelib_long timelib_parse_tz_cor(const char **ptr, int *tz_not_found)
{
	const char *begin = *ptr, *end;
	timelib_long  tmp;

	*tz_not_found = 1;

	while (isdigit(**ptr) || **ptr == ':') {
		++*ptr;
	}
	end = *ptr;
	switch (end - begin) {
		case 1: /* H */
		case 2: /* HH */
			*tz_not_found = 0;
			return sHOUR(strtol(begin, NULL, 10));

		case 3: /* H:M */
		case 4: /* H:MM, HH:M, HHMM */
			if (begin[1] == ':') {
				*tz_not_found = 0;
				tmp = sHOUR(strtol(begin, NULL, 10)) + sMIN(strtol(begin + 2, NULL, 10));
				return tmp;
			} else if (begin[2] == ':') {
				*tz_not_found = 0;
				tmp = sHOUR(strtol(begin, NULL, 10)) + sMIN(strtol(begin + 3, NULL, 10));
				return tmp;
			} else {
				*tz_not_found = 0;
				tmp = strtol(begin, NULL, 10);
				return sHOUR(tmp / 100) + sMIN(tmp % 100);
			}

		case 5: /* HH:MM */
			if (begin[2] != ':') {
				break;
			}

			*tz_not_found = 0;
			tmp = sHOUR(strtol(begin, NULL, 10)) + sMIN(strtol(begin + 3, NULL, 10));
			return tmp;

		case 6: /* HHMMSS */
			*tz_not_found = 0;
			tmp = strtol(begin, NULL, 10);
			tmp = sHOUR(tmp / 10000) + sMIN((tmp / 100) % 100) + (tmp % 100);
			return tmp;

		case 8: /* HH:MM:SS */
			if (begin[2] != ':' || begin[5] != ':') {
				break;
			}

			*tz_not_found = 0;
			tmp = sHOUR(strtol(begin, NULL, 10)) + sMIN(strtol(begin + 3, NULL, 10)) + strtol(begin + 6, NULL, 10);
			return tmp;

	}
	return 0;
}

static timelib_long timelib_parse_tz_minutes(const char **ptr, timelib_time *t)
{
	timelib_long retval = TIMELIB_UNSET;
	const char *begin = *ptr;

	/* First character must be +/- */
	if (**ptr != '+' && **ptr != '-') {
		return retval;
	}

	++*ptr;
	while (isdigit(**ptr)) {
		++*ptr;
	}

	if (*begin == '+') {
		t->is_localtime = 1;
		t->zone_type = TIMELIB_ZONETYPE_OFFSET;
		t->dst = 0;

		retval = sMIN(strtol(begin + 1, NULL, 10));
	} else if (*begin == '-') {
		t->is_localtime = 1;
		t->zone_type = TIMELIB_ZONETYPE_OFFSET;
		t->dst = 0;

		retval = -1 * sMIN(strtol(begin + 1, NULL, 10));
	}
	return retval;
}

timelib_long timelib_parse_zone(const char **ptr, int *dst, timelib_time *t, int *tz_not_found, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_wrapper)
{
	timelib_tzinfo *res;
	timelib_long            retval = 0;

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
		t->dst = 0;

		retval = timelib_parse_tz_cor(ptr, tz_not_found);
	} else if (**ptr == '-') {
		++*ptr;
		t->is_localtime = 1;
		t->zone_type = TIMELIB_ZONETYPE_OFFSET;
		t->dst = 0;

		retval = -1 * timelib_parse_tz_cor(ptr, tz_not_found);
	} else {
		int found = 0;
		timelib_long offset = 0;
		char *tz_abbr;

		t->is_localtime = 1;

		/* First, we lookup by abbreviation only */
		offset = timelib_lookup_abbr(ptr, dst, &tz_abbr, &found);
		if (found) {
			t->zone_type = TIMELIB_ZONETYPE_ABBR;
			t->dst = *dst;
			timelib_time_tz_abbr_update(t, tz_abbr);
		}

		/* Otherwise, we look if we have a TimeZone identifier */
		if (!found || strcmp("UTC", tz_abbr) == 0) {
			int dummy_error_code;

			if ((res = tz_wrapper(tz_abbr, tzdb, &dummy_error_code)) != NULL) {
				t->tz_info = res;
				t->zone_type = TIMELIB_ZONETYPE_ID;
				found++;
			}
		}
		timelib_free(tz_abbr);
		*tz_not_found = (found == 0);
		retval = offset;
	}
	while (**ptr == ')') {
		++*ptr;
	}
	return retval;
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

static int scan(Scanner *s, timelib_tz_get_wrapper tz_get_wrapper)
{
	uchar *cursor = s->cur;
	char *str;
	const char *ptr = NULL;

std:
	s->tok = cursor;
	s->len = 0;
/*!re2c
any = [\000-\377];

nbsp = [\302][\240];
nnbsp = [\342][\200][\257];
space = [ \t]+ | nbsp+ | nnbsp+;
frac = "."[0-9]+;

ago = 'ago';

hour24 = [01]?[0-9] | "2"[0-4];
hour24lz = [01][0-9] | "2"[0-4];
hour12 = "0"?[1-9] | "1"[0-2];
minute = [0-5]?[0-9];
minutelz = [0-5][0-9];
second = minute | "60";
secondlz = minutelz | "60";
meridian = ([AaPp] "."? [Mm] "."?) [\000\t ];
tz = "("? [A-Za-z]{1,6} ")"? | [A-Z][a-z]+([_/-][A-Za-z]+)+;
tzcorrection = "GMT"? [+-] ((hour24 (":"? minute)?) | (hour24lz minutelz secondlz) | (hour24lz ":" minutelz ":" secondlz));

daysuf = "st" | "nd" | "rd" | "th";

month = "0"? [0-9] | "1"[0-2];
day   = (([0-2]?[0-9]) | ("3"[01])) daysuf?;
year  = [0-9]{1,4};
year2 = [0-9]{2};
year4 = [0-9]{4};
year4withsign = [+-]? [0-9]{4};
yearx = [+-] [0-9]{5,19};

dayofyear = "00"[1-9] | "0"[1-9][0-9] | [1-2][0-9][0-9] | "3"[0-5][0-9] | "36"[0-6];
weekofyear = "0"[1-9] | [1-4][0-9] | "5"[0-3];

monthlz = "0" [0-9] | "1" [0-2];
daylz   = "0" [0-9] | [1-2][0-9] | "3" [01];

dayfulls = 'sundays' | 'mondays' | 'tuesdays' | 'wednesdays' | 'thursdays' | 'fridays' | 'saturdays';
dayfull = 'sunday' | 'monday' | 'tuesday' | 'wednesday' | 'thursday' | 'friday' | 'saturday';
dayabbr = 'sun' | 'mon' | 'tue' | 'wed' | 'thu' | 'fri' | 'sat' | 'sun';
dayspecial = 'weekday' | 'weekdays';
daytext = dayfulls | dayfull | dayabbr | dayspecial;

monthfull = 'january' | 'february' | 'march' | 'april' | 'may' | 'june' | 'july' | 'august' | 'september' | 'october' | 'november' | 'december';
monthabbr = 'jan' | 'feb' | 'mar' | 'apr' | 'may' | 'jun' | 'jul' | 'aug' | 'sep' | 'sept' | 'oct' | 'nov' | 'dec';
monthroman = "I" | "II" | "III" | "IV" | "V" | "VI" | "VII" | "VIII" | "IX" | "X" | "XI" | "XII";
monthtext = monthfull | monthabbr | monthroman;

/* Time formats */
timetiny12 = hour12 space? meridian;
timeshort12 = hour12[:.]minutelz space? meridian;
timelong12 = hour12[:.]minute[:.]secondlz space? meridian;

timetiny24 = 't' hour24;
timeshort24 = 't'? hour24[:.]minute;
timelong24 =  't'? hour24[:.]minute[:.]second;
iso8601long =  't'? hour24 [:.] minute [:.] second frac;

/* iso8601shorttz = hour24 [:] minutelz space? (tzcorrection | tz); */
iso8601normtz =  't'? hour24 [:.] minute [:.] secondlz space? (tzcorrection | tz);
/* iso8601longtz =  hour24 [:] minute [:] secondlz frac space? (tzcorrection | tz); */

gnunocolon       = 't'? hour24lz minutelz;
/* gnunocolontz     = hour24lz minutelz space? (tzcorrection | tz); */
iso8601nocolon   = 't'? hour24lz minutelz secondlz;
/* iso8601nocolontz = hour24lz minutelz secondlz space? (tzcorrection | tz); */

/* Date formats */
americanshort    = month "/" day;
american         = month "/" day "/" year;
iso8601dateslash = year4 "/" monthlz "/" daylz "/"?;
dateslash        = year4 "/" month "/" day;
iso8601date4     = year4withsign "-" monthlz "-" daylz;
iso8601date2     = year2 "-" monthlz "-" daylz;
iso8601datex     = yearx "-" monthlz "-" daylz;
gnudateshorter   = year4 "-" month;
gnudateshort     = year "-" month "-" day;
pointeddate4     = day [.\t-] month [.-] year4;
pointeddate2     = day [.\t] month "." year2;
datefull         = day ([ \t.-])* monthtext ([ \t.-])* year;
datenoday        = monthtext ([ .\t-])* year4;
datenodayrev     = year4 ([ .\t-])* monthtext;
datetextual      = monthtext ([ .\t-])* day [,.stndrh\t ]+ year;
datenoyear       = monthtext ([ .\t-])* day ([,.stndrh\t ]+|[\000]);
datenoyearrev    = day ([ .\t-])* monthtext;
datenocolon      = year4 monthlz daylz;

/* Special formats */
soap             = year4 "-" monthlz "-" daylz "T" hour24lz ":" minutelz ":" secondlz frac tzcorrection?;
xmlrpc           = year4 monthlz daylz "T" hour24 ":" minutelz ":" secondlz;
xmlrpcnocolon    = year4 monthlz daylz 't' hour24 minutelz secondlz;
wddx             = year4 "-" month "-" day "T" hour24 ":" minute ":" second;
pgydotd          = year4 [.-]? dayofyear;
pgtextshort      = monthabbr "-" daylz "-" year;
pgtextreverse    = year "-" monthabbr "-" daylz;
mssqltime        = hour12 ":" minutelz ":" secondlz [:.] [0-9]+ meridian;
isoweekday       = year4 "-"? "W" weekofyear "-"? [0-7];
isoweek          = year4 "-"? "W" weekofyear;
exif             = year4 ":" monthlz ":" daylz " " hour24lz ":" minutelz ":" secondlz;
firstdayof       = 'first day of';
lastdayof        = 'last day of';
backof           = 'back of ' hour24 (space? meridian)?;
frontof          = 'front of ' hour24 (space? meridian)?;

/* Common Log Format: 10/Oct/2000:13:55:36 -0700 */
clf              = day "/" monthabbr "/" year4 ":" hour24lz ":" minutelz ":" secondlz space tzcorrection;

/* Timestamp format: @1126396800 */
timestamp        = "@" "-"? [0-9]+;
timestampms      = "@" "-"? [0-9]+ "." [0-9]{0,6};

/* To fix some ambiguities */
dateshortwithtimeshort12  = datenoyear timeshort12;
dateshortwithtimelong12   = datenoyear timelong12;
dateshortwithtimeshort  = datenoyear timeshort24;
dateshortwithtimelong   = datenoyear timelong24;
dateshortwithtimelongtz = datenoyear iso8601normtz;

/*
 * Relative regexps
 */
reltextnumber = 'first'|'second'|'third'|'fourth'|'fifth'|'sixth'|'seventh'|'eight'|'eighth'|'ninth'|'tenth'|'eleventh'|'twelfth';
reltexttext = 'next'|'last'|'previous'|'this';
reltextunit = 'ms' | 'µs' | (('msec'|'millisecond'|'µsec'|'microsecond'|'usec'|'sec'|'second'|'min'|'minute'|'hour'|'day'|'fortnight'|'forthnight'|'month'|'year') 's'?) | 'weeks' | daytext;

relnumber = ([+-]*[ \t]*[0-9]{1,13});
relative = relnumber space? (reltextunit | 'week' );
relativetext = (reltextnumber|reltexttext) space reltextunit;
relativetextweek = reltexttext space 'week';

weekdayof        = (reltextnumber|reltexttext) space (dayfulls|dayfull|dayabbr) space 'of';

*/

/*!re2c
	/* so that vim highlights correctly */
	'yesterday'
	{
		DEBUG_OUTPUT("yesterday");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();
		TIMELIB_UNHAVE_TIME();

		s->time->relative.d = -1;
		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	'now'
	{
		DEBUG_OUTPUT("now");
		TIMELIB_INIT;

		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	'noon'
	{
		DEBUG_OUTPUT("noon");
		TIMELIB_INIT;
		TIMELIB_UNHAVE_TIME();
		TIMELIB_HAVE_TIME();
		s->time->h = 12;

		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	'midnight' | 'today'
	{
		DEBUG_OUTPUT("midnight | today");
		TIMELIB_INIT;
		TIMELIB_UNHAVE_TIME();

		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	'tomorrow'
	{
		DEBUG_OUTPUT("tomorrow");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();
		TIMELIB_UNHAVE_TIME();

		s->time->relative.d = 1;
		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	timestamp
	{
		timelib_ull i;

		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();
		TIMELIB_UNHAVE_DATE();
		TIMELIB_UNHAVE_TIME();
		TIMELIB_HAVE_TZ();

		i = timelib_get_signed_nr(s, &ptr, 24);
		s->time->y = 1970;
		s->time->m = 1;
		s->time->d = 1;
		s->time->h = s->time->i = s->time->s = 0;
		s->time->us = 0;
		s->time->relative.s += i;
		s->time->is_localtime = 1;
		s->time->zone_type = TIMELIB_ZONETYPE_OFFSET;
		s->time->z = 0;
		s->time->dst = 0;

		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	timestampms
	{
		timelib_sll i;
		timelib_ull us;
		const char *ptr_before;
		bool is_negative;

		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();
		TIMELIB_UNHAVE_DATE();
		TIMELIB_UNHAVE_TIME();
		TIMELIB_HAVE_TZ();

		is_negative = *(ptr + 1) == '-';

		i = timelib_get_signed_nr(s, &ptr, 24);

		ptr_before = ptr;
		us = timelib_get_signed_nr(s, &ptr, 6);
		us = us * pow(10, 7 - (ptr - ptr_before));
		if (is_negative) {
			us *= -1;
		}

		s->time->y = 1970;
		s->time->m = 1;
		s->time->d = 1;
		s->time->h = s->time->i = s->time->s = 0;
		s->time->us = 0;
		s->time->relative.s += i;
		s->time->relative.us = us;
		s->time->is_localtime = 1;
		s->time->zone_type = TIMELIB_ZONETYPE_OFFSET;
		s->time->z = 0;
		s->time->dst = 0;

		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	firstdayof | lastdayof
	{
		DEBUG_OUTPUT("firstdayof | lastdayof");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();

		/* skip "last day of" or "first day of" */
		if (*ptr == 'l' || *ptr == 'L') {
			s->time->relative.first_last_day_of = TIMELIB_SPECIAL_LAST_DAY_OF_MONTH;
		} else {
			s->time->relative.first_last_day_of = TIMELIB_SPECIAL_FIRST_DAY_OF_MONTH;
		}

		TIMELIB_DEINIT;
		return TIMELIB_LF_DAY_OF_MONTH;
	}

	backof | frontof
	{
		DEBUG_OUTPUT("backof | frontof");
		TIMELIB_INIT;
		TIMELIB_UNHAVE_TIME();
		TIMELIB_HAVE_TIME();

		if (*ptr == 'b') {
			s->time->h = timelib_get_nr(&ptr, 2);
			s->time->i = 15;
		} else {
			s->time->h = timelib_get_nr(&ptr, 2) - 1;
			s->time->i = 45;
		}
		if (*ptr != '\0' ) {
			timelib_eat_spaces(&ptr);
			s->time->h += timelib_meridian(&ptr, s->time->h);
		}

		TIMELIB_DEINIT;
		return TIMELIB_LF_DAY_OF_MONTH;
	}

	weekdayof
	{
		timelib_sll i;
		int         behavior = 0;
		DEBUG_OUTPUT("weekdayof");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();
		TIMELIB_HAVE_SPECIAL_RELATIVE();

		i = timelib_get_relative_text(&ptr, &behavior);
		timelib_eat_spaces(&ptr);
		if (i > 0) { /* first, second... etc */
			s->time->relative.special.type = TIMELIB_SPECIAL_DAY_OF_WEEK_IN_MONTH;
			timelib_set_relative(&ptr, i, 1, s, TIMELIB_TIME_PART_DONT_KEEP);
		} else { /* last */
			s->time->relative.special.type = TIMELIB_SPECIAL_LAST_DAY_OF_WEEK_IN_MONTH;
			timelib_set_relative(&ptr, i, behavior, s, TIMELIB_TIME_PART_DONT_KEEP);
		}
		TIMELIB_DEINIT;
		return TIMELIB_WEEK_DAY_OF_MONTH;
	}

	timetiny12 | timeshort12 | timelong12
	{
		DEBUG_OUTPUT("timetiny12 | timeshort12 | timelong12");
		TIMELIB_INIT;
		TIMELIB_HAVE_TIME();
		s->time->h = timelib_get_nr(&ptr, 2);
		if (*ptr == ':' || *ptr == '.') {
			s->time->i = timelib_get_nr(&ptr, 2);
			if (*ptr == ':' || *ptr == '.') {
				s->time->s = timelib_get_nr(&ptr, 2);
			}
		}
		timelib_eat_spaces(&ptr);
		s->time->h += timelib_meridian(&ptr, s->time->h);
		TIMELIB_DEINIT;
		return TIMELIB_TIME12;
	}

	mssqltime
	{
		DEBUG_OUTPUT("mssqltime");
		TIMELIB_INIT;
		TIMELIB_HAVE_TIME();
		s->time->h = timelib_get_nr(&ptr, 2);
		s->time->i = timelib_get_nr(&ptr, 2);
		if (*ptr == ':' || *ptr == '.') {
			s->time->s = timelib_get_nr(&ptr, 2);

			if (*ptr == ':' || *ptr == '.') {
				s->time->us = timelib_get_frac_nr(&ptr);
			}
		}
		timelib_eat_spaces(&ptr);
		s->time->h += timelib_meridian(&ptr, s->time->h);
		TIMELIB_DEINIT;
		return TIMELIB_TIME24_WITH_ZONE;
	}

	timetiny24 | timeshort24 | timelong24 /* | iso8601short | iso8601norm */ | iso8601long /*| iso8601shorttz | iso8601normtz | iso8601longtz*/
	{
		int tz_not_found;
		DEBUG_OUTPUT("timetiny24 | timeshort24 | timelong24 | iso8601long");
		TIMELIB_INIT;
		TIMELIB_HAVE_TIME();
		s->time->h = timelib_get_nr(&ptr, 2);
		if (*ptr == ':' || *ptr == '.') {
			s->time->i = timelib_get_nr(&ptr, 2);
			if (*ptr == ':' || *ptr == '.') {
				s->time->s = timelib_get_nr(&ptr, 2);

				if (*ptr == '.') {
					s->time->us = timelib_get_frac_nr(&ptr);
				}
			}
		}

		if (*ptr != '\0') {
			s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
			if (tz_not_found) {
				add_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database");
			}
		}
		TIMELIB_DEINIT;
		return TIMELIB_TIME24_WITH_ZONE;
	}

	gnunocolon
	{
		DEBUG_OUTPUT("gnunocolon");
		TIMELIB_INIT;
		switch (s->time->have_time) {
			case 0:
				s->time->h = timelib_get_nr(&ptr, 2);
				s->time->i = timelib_get_nr(&ptr, 2);
				s->time->s = 0;
				break;
			case 1:
				s->time->y = timelib_get_nr(&ptr, 4);
				break;
			default:
				TIMELIB_DEINIT;
				add_error(s, TIMELIB_ERR_DOUBLE_TIME, "Double time specification");
				return TIMELIB_ERROR;
		}
		s->time->have_time++;
		TIMELIB_DEINIT;
		return TIMELIB_GNU_NOCOLON;
	}
/*
	gnunocolontz
	{
		DEBUG_OUTPUT("gnunocolontz");
		TIMELIB_INIT;
		switch (s->time->have_time) {
			case 0:
				s->time->h = timelib_get_nr(&ptr, 2);
				s->time->i = timelib_get_nr(&ptr, 2);
				s->time->s = 0;
				s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, s->tzdb, tz_get_wrapper);
				break;
			case 1:
				s->time->y = timelib_get_nr(&ptr, 4);
				break;
			default:
				TIMELIB_DEINIT;
				return TIMELIB_ERROR;
		}
		s->time->have_time++;
		TIMELIB_DEINIT;
		return TIMELIB_GNU_NOCOLON_TZ;
	}
*/
	iso8601nocolon /*| iso8601nocolontz*/
	{
		int tz_not_found;
		DEBUG_OUTPUT("iso8601nocolon");
		TIMELIB_INIT;
		TIMELIB_HAVE_TIME();
		s->time->h = timelib_get_nr(&ptr, 2);
		s->time->i = timelib_get_nr(&ptr, 2);
		s->time->s = timelib_get_nr(&ptr, 2);

		if (*ptr != '\0') {
			s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
			if (tz_not_found) {
				add_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database");
			}
		}
		TIMELIB_DEINIT;
		return TIMELIB_ISO_NOCOLON;
	}

	americanshort | american
	{
		int length = 0;
		DEBUG_OUTPUT("americanshort | american");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		if (*ptr == '/') {
			s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
			TIMELIB_PROCESS_YEAR(s->time->y, length);
		}
		TIMELIB_DEINIT;
		return TIMELIB_AMERICAN;
	}

	iso8601date4 | iso8601dateslash | dateslash
	{
		DEBUG_OUTPUT("iso8601date4 | iso8601date2 | iso8601dateslash | dateslash");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_signed_nr(s, &ptr, 4);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		TIMELIB_DEINIT;
		return TIMELIB_ISO_DATE;
	}

	iso8601date2
	{
		int length = 0;
		DEBUG_OUTPUT("iso8601date2");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_ISO_DATE;
	}

	iso8601datex
	{
		DEBUG_OUTPUT("iso8601datex");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_signed_nr(s, &ptr, 19);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		TIMELIB_DEINIT;
		return TIMELIB_ISO_DATE;
	}

	gnudateshorter
	{
		int length = 0;
		DEBUG_OUTPUT("gnudateshorter");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = 1;
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_ISO_DATE;
	}

	gnudateshort
	{
		int length = 0;
		DEBUG_OUTPUT("gnudateshort");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_ISO_DATE;
	}

	datefull
	{
		int length = 0;
		DEBUG_OUTPUT("datefull");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->d = timelib_get_nr(&ptr, 2);
		timelib_skip_day_suffix(&ptr);
		s->time->m = timelib_get_month(&ptr);
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_FULL;
	}

	pointeddate4
	{
		DEBUG_OUTPUT("pointed date YYYY");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->d = timelib_get_nr(&ptr, 2);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->y = timelib_get_nr(&ptr, 4);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_FULL_POINTED;
	}

	pointeddate2
	{
		int length = 0;
		DEBUG_OUTPUT("pointed date YY");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->d = timelib_get_nr(&ptr, 2);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->y = timelib_get_nr_ex(&ptr, 2, &length);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_FULL_POINTED;
	}

	datenoday
	{
		int length = 0;
		DEBUG_OUTPUT("datenoday");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_get_month(&ptr);
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->d = 1;
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_NO_DAY;
	}

	datenodayrev
	{
		int length = 0;
		DEBUG_OUTPUT("datenodayrev");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->m = timelib_get_month(&ptr);
		s->time->d = 1;
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_NO_DAY;
	}

	datetextual | datenoyear
	{
		int length = 0;
		DEBUG_OUTPUT("datetextual | datenoyear");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_get_month(&ptr);
		s->time->d = timelib_get_nr(&ptr, 2);
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_TEXT;
	}

	datenoyearrev
	{
		DEBUG_OUTPUT("datenoyearrev");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->d = timelib_get_nr(&ptr, 2);
		timelib_skip_day_suffix(&ptr);
		s->time->m = timelib_get_month(&ptr);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_TEXT;
	}

	datenocolon
	{
		DEBUG_OUTPUT("datenocolon");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr(&ptr, 4);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_NOCOLON;
	}

	xmlrpc | xmlrpcnocolon | soap | wddx | exif
	{
		int tz_not_found;
		DEBUG_OUTPUT("xmlrpc | xmlrpcnocolon | soap | wddx | exif");
		TIMELIB_INIT;
		TIMELIB_HAVE_TIME();
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr(&ptr, 4);
		s->time->m = timelib_get_nr(&ptr, 2);
		s->time->d = timelib_get_nr(&ptr, 2);
		s->time->h = timelib_get_nr(&ptr, 2);
		s->time->i = timelib_get_nr(&ptr, 2);
		s->time->s = timelib_get_nr(&ptr, 2);
		if (*ptr == '.') {
			s->time->us = timelib_get_frac_nr(&ptr);
			if (*ptr) { /* timezone is optional */
				s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
				if (tz_not_found) {
					add_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database");
				}
			}
		}
		TIMELIB_DEINIT;
		return TIMELIB_XMLRPC_SOAP;
	}

	pgydotd
	{
		int length = 0;
		DEBUG_OUTPUT("pgydotd");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->d = timelib_get_nr(&ptr, 3);
		s->time->m = 1;
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_PG_YEARDAY;
	}

	isoweekday
	{
		timelib_sll w, d;
		DEBUG_OUTPUT("isoweekday");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		TIMELIB_HAVE_RELATIVE();

		s->time->y = timelib_get_nr(&ptr, 4);
		w = timelib_get_nr(&ptr, 2);
		d = timelib_get_nr(&ptr, 1);
		s->time->m = 1;
		s->time->d = 1;
		s->time->relative.d = timelib_daynr_from_weeknr(s->time->y, w, d);

		TIMELIB_DEINIT;
		return TIMELIB_ISO_WEEK;
	}

	isoweek
	{
		timelib_sll w, d;
		DEBUG_OUTPUT("isoweek");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		TIMELIB_HAVE_RELATIVE();

		s->time->y = timelib_get_nr(&ptr, 4);
		w = timelib_get_nr(&ptr, 2);
		d = 1;
		s->time->m = 1;
		s->time->d = 1;
		s->time->relative.d = timelib_daynr_from_weeknr(s->time->y, w, d);

		TIMELIB_DEINIT;
		return TIMELIB_ISO_WEEK;
	}

	pgtextshort
	{
		int length = 0;
		DEBUG_OUTPUT("pgtextshort");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_get_month(&ptr);
		s->time->d = timelib_get_nr(&ptr, 2);
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_PG_TEXT;
	}

	pgtextreverse
	{
		int length = 0;
		DEBUG_OUTPUT("pgtextreverse");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->y = timelib_get_nr_ex(&ptr, 4, &length);
		s->time->m = timelib_get_month(&ptr);
		s->time->d = timelib_get_nr(&ptr, 2);
		TIMELIB_PROCESS_YEAR(s->time->y, length);
		TIMELIB_DEINIT;
		return TIMELIB_PG_TEXT;
	}

	clf
	{
		int tz_not_found;
		DEBUG_OUTPUT("clf");
		TIMELIB_INIT;
		TIMELIB_HAVE_TIME();
		TIMELIB_HAVE_DATE();
		s->time->d = timelib_get_nr(&ptr, 2);
		s->time->m = timelib_get_month(&ptr);
		s->time->y = timelib_get_nr(&ptr, 4);
		s->time->h = timelib_get_nr(&ptr, 2);
		s->time->i = timelib_get_nr(&ptr, 2);
		s->time->s = timelib_get_nr(&ptr, 2);

		timelib_eat_spaces(&ptr);

		s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
		if (tz_not_found) {
			add_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database");
		}
		TIMELIB_DEINIT;
		return TIMELIB_CLF;
	}

	year4
	{
		DEBUG_OUTPUT("year4");
		TIMELIB_INIT;
		s->time->y = timelib_get_nr(&ptr, 4);
		TIMELIB_DEINIT;
		return TIMELIB_CLF;
	}

	ago
	{
		DEBUG_OUTPUT("ago");
		TIMELIB_INIT;
		s->time->relative.y = 0 - s->time->relative.y;
		s->time->relative.m = 0 - s->time->relative.m;
		s->time->relative.d = 0 - s->time->relative.d;
		s->time->relative.h = 0 - s->time->relative.h;
		s->time->relative.i = 0 - s->time->relative.i;
		s->time->relative.s = 0 - s->time->relative.s;
		s->time->relative.weekday = 0 - s->time->relative.weekday;
		if (s->time->relative.weekday == 0) {
			s->time->relative.weekday = -7;
		}
		if (s->time->relative.have_special_relative && s->time->relative.special.type == TIMELIB_SPECIAL_WEEKDAY) {
			s->time->relative.special.amount = 0 - s->time->relative.special.amount;
		}
		TIMELIB_DEINIT;
		return TIMELIB_AGO;
	}

	daytext
	{
		const timelib_relunit* relunit;
		DEBUG_OUTPUT("daytext");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();
		TIMELIB_HAVE_WEEKDAY_RELATIVE();
		TIMELIB_UNHAVE_TIME();
		relunit = timelib_lookup_relunit(&ptr);
		s->time->relative.weekday = relunit->multiplier;
		if (s->time->relative.weekday_behavior != 2) {
			s->time->relative.weekday_behavior = 1;
		}

		TIMELIB_DEINIT;
		return TIMELIB_WEEKDAY;
	}

	relativetextweek
	{
		timelib_sll i;
		int         behavior = 0;
		DEBUG_OUTPUT("relativetextweek");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();

		while(*ptr) {
			i = timelib_get_relative_text(&ptr, &behavior);
			timelib_eat_spaces(&ptr);
			timelib_set_relative(&ptr, i, behavior, s, TIMELIB_TIME_PART_DONT_KEEP);
			s->time->relative.weekday_behavior = 2;

			/* to handle the format weekday + last/this/next week */
			if (s->time->relative.have_weekday_relative == 0) {
				TIMELIB_HAVE_WEEKDAY_RELATIVE();
				s->time->relative.weekday = 1;
			}
		}
		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	relativetext
	{
		timelib_sll i;
		int         behavior = 0;
		DEBUG_OUTPUT("relativetext");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();

		while(*ptr) {
			i = timelib_get_relative_text(&ptr, &behavior);
			timelib_eat_spaces(&ptr);
			timelib_set_relative(&ptr, i, behavior, s, TIMELIB_TIME_PART_DONT_KEEP);
		}
		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	monthfull | monthabbr
	{
		DEBUG_OUTPUT("monthtext");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_lookup_month(&ptr);
		TIMELIB_DEINIT;
		return TIMELIB_DATE_TEXT;
	}

	tzcorrection | tz
	{
		int tz_not_found;
		DEBUG_OUTPUT("tzcorrection | tz");
		TIMELIB_INIT;
		TIMELIB_HAVE_TZ();
		timelib_eat_spaces(&ptr);
		s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
		if (tz_not_found) {
			add_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database");
		}
		TIMELIB_DEINIT;
		return TIMELIB_TIMEZONE;
	}

	dateshortwithtimeshort12 | dateshortwithtimelong12
	{
		DEBUG_OUTPUT("dateshortwithtimeshort12 | dateshortwithtimelong12");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_get_month(&ptr);
		s->time->d = timelib_get_nr(&ptr, 2);

		TIMELIB_HAVE_TIME();
		s->time->h = timelib_get_nr(&ptr, 2);
		s->time->i = timelib_get_nr(&ptr, 2);
		if (*ptr == ':' || *ptr == '.') {
			s->time->s = timelib_get_nr(&ptr, 2);

			if (*ptr == '.') {
				s->time->us = timelib_get_frac_nr(&ptr);
			}
		}

		s->time->h += timelib_meridian(&ptr, s->time->h);
		TIMELIB_DEINIT;
		return TIMELIB_SHORTDATE_WITH_TIME;
	}

	dateshortwithtimeshort | dateshortwithtimelong | dateshortwithtimelongtz
	{
		int tz_not_found;
		DEBUG_OUTPUT("dateshortwithtimeshort | dateshortwithtimelong | dateshortwithtimelongtz");
		TIMELIB_INIT;
		TIMELIB_HAVE_DATE();
		s->time->m = timelib_get_month(&ptr);
		s->time->d = timelib_get_nr(&ptr, 2);

		TIMELIB_HAVE_TIME();
		s->time->h = timelib_get_nr(&ptr, 2);
		s->time->i = timelib_get_nr(&ptr, 2);
		if (*ptr == ':') {
			s->time->s = timelib_get_nr(&ptr, 2);

			if (*ptr == '.') {
				s->time->us = timelib_get_frac_nr(&ptr);
			}
		}

		if (*ptr != '\0') {
			s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
			if (tz_not_found) {
				add_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database");
			}
		}
		TIMELIB_DEINIT;
		return TIMELIB_SHORTDATE_WITH_TIME;
	}

	relative
	{
		timelib_ull i;
		DEBUG_OUTPUT("relative");
		TIMELIB_INIT;
		TIMELIB_HAVE_RELATIVE();

		while(*ptr) {
			i = timelib_get_signed_nr(s, &ptr, 24);
			timelib_eat_spaces(&ptr);
			timelib_set_relative(&ptr, i, 1, s, TIMELIB_TIME_PART_KEEP);
		}
		TIMELIB_DEINIT;
		return TIMELIB_RELATIVE;
	}

	[.,]
	{
		goto std;
	}

	space
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
		add_error(s, TIMELIB_ERR_UNEXPECTED_CHARACTER, "Unexpected character");
		goto std;
	}
*/
}

/*!max:re2c */

timelib_time *timelib_strtotime(const char *s, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper)
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
		in.time = timelib_time_ctor();
		add_error(&in, TIMELIB_ERR_EMPTY_STRING, "Empty string");
		if (errors) {
			*errors = in.errors;
		} else {
			timelib_error_container_dtor(in.errors);
		}
		in.time->y = in.time->d = in.time->m = in.time->h = in.time->i = in.time->s = in.time->us = in.time->dst = in.time->z = TIMELIB_UNSET;
		in.time->is_localtime = in.time->zone_type = 0;
		return in.time;
	}
	e++;

	in.str = timelib_malloc((e - s) + YYMAXFILL);
	memset(in.str, 0, (e - s) + YYMAXFILL);
	memcpy(in.str, s, (e - s));
	in.lim = in.str + (e - s) + YYMAXFILL;
	in.cur = in.str;
	in.time = timelib_time_ctor();
	in.time->y = TIMELIB_UNSET;
	in.time->d = TIMELIB_UNSET;
	in.time->m = TIMELIB_UNSET;
	in.time->h = TIMELIB_UNSET;
	in.time->i = TIMELIB_UNSET;
	in.time->s = TIMELIB_UNSET;
	in.time->us = TIMELIB_UNSET;
	in.time->z = TIMELIB_UNSET;
	in.time->dst = TIMELIB_UNSET;
	in.tzdb = tzdb;
	in.time->is_localtime = 0;
	in.time->zone_type = 0;
	in.time->relative.days = TIMELIB_UNSET;

	do {
		t = scan(&in, tz_get_wrapper);
#ifdef DEBUG_PARSER
		printf("%d\n", t);
#endif
	} while(t != EOI);

	/* do funky checking whether the parsed time was valid time */
	if (in.time->have_time && !timelib_valid_time( in.time->h, in.time->i, in.time->s)) {
		add_warning(&in, TIMELIB_WARN_INVALID_TIME, "The parsed time was invalid");
	}
	/* do funky checking whether the parsed date was valid date */
	if (in.time->have_date && !timelib_valid_date( in.time->y, in.time->m, in.time->d)) {
		add_warning(&in, TIMELIB_WARN_INVALID_DATE, "The parsed date was invalid");
	}

	timelib_free(in.str);
	if (errors) {
		*errors = in.errors;
	} else {
		timelib_error_container_dtor(in.errors);
	}
	return in.time;
}

#define TIMELIB_CHECK_NUMBER                                           \
		if (strchr("0123456789", *ptr) == NULL)                        \
		{                                                              \
			add_pbf_error(s, TIMELIB_ERR_UNEXPECTED_DATA, "Unexpected data found.", string, begin); \
		}
#define TIMELIB_CHECK_SIGNED_NUMBER                                    \
		if (strchr("+-0123456789", *ptr) == NULL)                      \
		{                                                              \
			add_pbf_error(s, TIMELIB_ERR_UNEXPECTED_DATA, "Unexpected data found.", string, begin); \
		}

static void timelib_time_reset_fields(timelib_time *time)
{
	assert(time != NULL);

	time->y = 1970;
	time->m = 1;
	time->d = 1;
	time->h = time->i = time->s = 0;
	time->us = 0;
	time->tz_info = NULL;
}

static void timelib_time_reset_unset_fields(timelib_time *time)
{
	assert(time != NULL);

	if (time->y == TIMELIB_UNSET ) time->y = 1970;
	if (time->m == TIMELIB_UNSET ) time->m = 1;
	if (time->d == TIMELIB_UNSET ) time->d = 1;
	if (time->h == TIMELIB_UNSET ) time->h = 0;
	if (time->i == TIMELIB_UNSET ) time->i = 0;
	if (time->s == TIMELIB_UNSET ) time->s = 0;
	if (time->us == TIMELIB_UNSET ) time->us = 0;
}

static const timelib_format_specifier default_format_map[] = {
	{'+', TIMELIB_FORMAT_ALLOW_EXTRA_CHARACTERS},
	{'#', TIMELIB_FORMAT_ANY_SEPARATOR},
	{'j', TIMELIB_FORMAT_DAY_TWO_DIGIT},
	{'d', TIMELIB_FORMAT_DAY_TWO_DIGIT_PADDED},
	{'z', TIMELIB_FORMAT_DAY_OF_YEAR},
	{'S', TIMELIB_FORMAT_DAY_SUFFIX},
	{'U', TIMELIB_FORMAT_EPOCH_SECONDS},
	{'\\', TIMELIB_FORMAT_ESCAPE},
	{'h', TIMELIB_FORMAT_HOUR_TWO_DIGIT_12_MAX},
	{'g', TIMELIB_FORMAT_HOUR_TWO_DIGIT_12_MAX_PADDED},
	{'H', TIMELIB_FORMAT_HOUR_TWO_DIGIT_24_MAX},
	{'G', TIMELIB_FORMAT_HOUR_TWO_DIGIT_24_MAX_PADDED},
	{'a', TIMELIB_FORMAT_MERIDIAN},
	{'A', TIMELIB_FORMAT_MERIDIAN},
	{'u', TIMELIB_FORMAT_MICROSECOND_SIX_DIGIT},
	{'v', TIMELIB_FORMAT_MILLISECOND_THREE_DIGIT},
	{'i', TIMELIB_FORMAT_MINUTE_TWO_DIGIT},
	{'n', TIMELIB_FORMAT_MONTH_TWO_DIGIT},
	{'m', TIMELIB_FORMAT_MONTH_TWO_DIGIT_PADDED},
	{'?', TIMELIB_FORMAT_RANDOM_CHAR},
	{'!', TIMELIB_FORMAT_RESET_ALL},
	{'|', TIMELIB_FORMAT_RESET_ALL_WHEN_NOT_SET},
	{'s', TIMELIB_FORMAT_SECOND_TWO_DIGIT},
	{';', TIMELIB_FORMAT_SEPARATOR},
	{':', TIMELIB_FORMAT_SEPARATOR},
	{'/', TIMELIB_FORMAT_SEPARATOR},
	{'.', TIMELIB_FORMAT_SEPARATOR},
	{',', TIMELIB_FORMAT_SEPARATOR},
	{'-', TIMELIB_FORMAT_SEPARATOR},
	{'(', TIMELIB_FORMAT_SEPARATOR},
	{')', TIMELIB_FORMAT_SEPARATOR},
	{'*', TIMELIB_FORMAT_SKIP_TO_SEPARATOR},
	{'D', TIMELIB_FORMAT_TEXTUAL_DAY_3_LETTER},
	{'l', TIMELIB_FORMAT_TEXTUAL_DAY_FULL},
	{'M', TIMELIB_FORMAT_TEXTUAL_MONTH_3_LETTER},
	{'F', TIMELIB_FORMAT_TEXTUAL_MONTH_FULL},
	{'e', TIMELIB_FORMAT_TIMEZONE_OFFSET},
	{'P', TIMELIB_FORMAT_TIMEZONE_OFFSET},
	{'p', TIMELIB_FORMAT_TIMEZONE_OFFSET},
	{'T', TIMELIB_FORMAT_TIMEZONE_OFFSET},
	{'O', TIMELIB_FORMAT_TIMEZONE_OFFSET},
	{' ', TIMELIB_FORMAT_WHITESPACE},
	{'y', TIMELIB_FORMAT_YEAR_TWO_DIGIT},
	{'Y', TIMELIB_FORMAT_YEAR_FOUR_DIGIT},
	{'x', TIMELIB_FORMAT_YEAR_EXPANDED},
	{'X', TIMELIB_FORMAT_YEAR_EXPANDED},
	{'\0', TIMELIB_FORMAT_END}
};

static const timelib_format_config default_format_config = {
	default_format_map,
	// No prefix required by default.
	'\0'
};

static timelib_format_specifier_code timelib_lookup_format(char input, const timelib_format_specifier* format_map)
{
	while (format_map && format_map->specifier != '\0') {
		if (format_map->specifier == input) {
			return format_map->code;
		}
		format_map++;
	}
	return TIMELIB_FORMAT_LITERAL;
}

timelib_time *timelib_parse_from_format(const char *format, const char *string, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper)
{
	return timelib_parse_from_format_with_map(format, string, len, errors, tzdb, tz_get_wrapper, &default_format_config);
}

timelib_time *timelib_parse_from_format_with_map(const char *format, const char *string, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper, const timelib_format_config* format_config)
{
	const char  *fptr = format;
	const char  *ptr = string;
	const char  *begin;
	timelib_sll  tmp;
	Scanner      in;
	Scanner     *s = &in;
	bool         allow_extra = false;
	bool         prefix_found = false;
	int          iso_year = TIMELIB_UNSET;
	int          iso_week_of_year = TIMELIB_UNSET;
	int          iso_day_of_week = TIMELIB_UNSET;
	char         prefix_char = format_config->prefix_char;
	const timelib_format_specifier *format_map = format_config->format_map;

	memset(&in, 0, sizeof(in));
	in.errors = timelib_malloc(sizeof(timelib_error_container));
	in.errors->warning_count = 0;
	in.errors->warning_messages = NULL;
	in.errors->error_count = 0;
	in.errors->error_messages = NULL;

	in.time = timelib_time_ctor();
	in.time->y = TIMELIB_UNSET;
	in.time->d = TIMELIB_UNSET;
	in.time->m = TIMELIB_UNSET;
	in.time->h = TIMELIB_UNSET;
	in.time->i = TIMELIB_UNSET;
	in.time->s = TIMELIB_UNSET;
	in.time->us = TIMELIB_UNSET;
	in.time->z = TIMELIB_UNSET;
	in.time->dst = TIMELIB_UNSET;
	in.tzdb = tzdb;
	in.time->is_localtime = 0;
	in.time->zone_type = 0;

	/* Loop over the format string */
	while (*fptr && *ptr) {
		begin = ptr;

		if (prefix_char) {
			/* There are 2 cases where the input string and format string
			 * should match the next literal:
			 *
			 * 1. No prefix has been specified yet in the format, so expect 1:1
			 *    match.
			 * 2. Sequential prefix characters indicating that the second
			 *    prefix is escaped. (e.g. "%%" is expecting literal "%")
			 */
			if ((!prefix_found && *fptr != prefix_char) ||
				(prefix_found && *fptr == prefix_char)) {
				if (*fptr != *ptr) {
					add_pbf_error(s, TIMELIB_ERR_FORMAT_LITERAL_MISMATCH, "Format literal not found", string, begin);
				}
				ptr++;
				fptr++;
				prefix_found = false;
				continue;
			}

			if (*fptr == prefix_char) {
				fptr++;
				prefix_found = true;
				continue;
			}

			/* Fall through case is that the prefix has been found and the next
			 * character is the format specifier. */
			prefix_found = false;
		}

		switch (timelib_lookup_format(*fptr, format_map)) {
			case TIMELIB_FORMAT_TEXTUAL_DAY_3_LETTER: /* three letter day */
			case TIMELIB_FORMAT_TEXTUAL_DAY_FULL: /* full day */
				{
					const timelib_relunit* tmprel = 0;

					tmprel = timelib_lookup_relunit(&ptr);
					if (!tmprel) {
						add_pbf_error(s, TIMELIB_ERR_NO_TEXTUAL_DAY, "A textual day could not be found", string, begin);
						break;
					} else {
						in.time->have_relative = 1;
						in.time->relative.have_weekday_relative = 1;
						in.time->relative.weekday = tmprel->multiplier;
						in.time->relative.weekday_behavior = 1;
					}
				}
				break;
			case TIMELIB_FORMAT_DAY_TWO_DIGIT: /* two digit day, without leading zero */
			case TIMELIB_FORMAT_DAY_TWO_DIGIT_PADDED: /* two digit day, with leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->d = timelib_get_nr(&ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_DAY, "A two digit day could not be found", string, begin);
					break;
				}

				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_DAY_SUFFIX: /* day suffix, ignored, nor checked */
				timelib_skip_day_suffix(&ptr);
				break;
			case TIMELIB_FORMAT_DAY_OF_YEAR: /* day of year - resets month (0 based) - also initializes everything else to !TIMELIB_UNSET */
				TIMELIB_CHECK_NUMBER;
				if (s->time->y == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_MERIDIAN_BEFORE_HOUR, "A 'day of year' can only come after a year has been found", string, begin);
				}
				if ((tmp = timelib_get_nr(&ptr, 3)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_THREE_DIGIT_DAY_OF_YEAR, "A three digit day-of-year could not be found", string, begin);
					break;
				}

				if (s->time->y != TIMELIB_UNSET) {
					s->time->have_date = 1;
					s->time->m = 1;
					s->time->d = tmp + 1;
					timelib_do_normalize(s->time);
				}
				break;

			case TIMELIB_FORMAT_MONTH_TWO_DIGIT: /* two digit month, without leading zero */
			case TIMELIB_FORMAT_MONTH_TWO_DIGIT_PADDED: /* two digit month, with leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->m = timelib_get_nr(&ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_MONTH, "A two digit month could not be found", string, begin);
					break;
				}
				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_TEXTUAL_MONTH_3_LETTER: /* three letter month */
			case TIMELIB_FORMAT_TEXTUAL_MONTH_FULL: /* full month */
				tmp = timelib_lookup_month(&ptr);
				if (!tmp) {
					add_pbf_error(s, TIMELIB_ERR_NO_TEXTUAL_MONTH, "A textual month could not be found", string, begin);
					break;
				}

				s->time->have_date = 1;
				s->time->m = tmp;
				break;
			case TIMELIB_FORMAT_YEAR_TWO_DIGIT: /* two digit year */
				{
					int length = 0;
					TIMELIB_CHECK_NUMBER;
					if ((s->time->y = timelib_get_nr_ex(&ptr, 2, &length)) == TIMELIB_UNSET) {
						add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_YEAR, "A two digit year could not be found", string, begin);
						break;
					}
					
					s->time->have_date = 1;
					TIMELIB_PROCESS_YEAR(s->time->y, length);
				}
				break;
			case TIMELIB_FORMAT_YEAR_FOUR_DIGIT: /* four digit year */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->y = timelib_get_nr(&ptr, 4)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_FOUR_DIGIT_YEAR, "A four digit year could not be found", string, begin);
					break;
				}

				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_YEAR_EXPANDED: /* optional symbol, followed by up to 19 digits */
				TIMELIB_CHECK_SIGNED_NUMBER;
				if ((s->time->y = timelib_get_signed_nr(s, &ptr, 19)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_FOUR_DIGIT_YEAR, "An expanded digit year could not be found", string, begin);
					break;
				}

				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_HOUR_TWO_DIGIT_12_MAX: /* two digit hour, without leading zero */
			case TIMELIB_FORMAT_HOUR_TWO_DIGIT_12_MAX_PADDED: /* two digit hour, with leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->h = timelib_get_nr(&ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_HOUR, "A two digit hour could not be found", string, begin);
					break;
				}
				if (s->time->h > 12) {
					add_pbf_error(s, TIMELIB_ERR_HOUR_LARGER_THAN_12, "Hour cannot be higher than 12", string, begin);
					break;
				}

				s->time->have_time = 1;
				break;
			case TIMELIB_FORMAT_HOUR_TWO_DIGIT_24_MAX_PADDED: /* two digit hour, with leading zero */
			case TIMELIB_FORMAT_HOUR_TWO_DIGIT_24_MAX: /* two digit hour, without leading zero */
				TIMELIB_CHECK_NUMBER;
				if ((s->time->h = timelib_get_nr(&ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_HOUR, "A two digit hour could not be found", string, begin);
					break;
				}

				s->time->have_time = 1;
				break;
			case TIMELIB_FORMAT_MERIDIAN: /* am/pm/a.m./p.m. AM/PM/A.M./P.M. */
				if (s->time->h == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_MERIDIAN_BEFORE_HOUR, "Meridian can only come after an hour has been found", string, begin);
				}
				if ((tmp = timelib_meridian_with_check(&ptr, s->time->h)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_MERIDIAN, "A meridian could not be found", string, begin);
					break;
				}

				s->time->have_time = 1;
				if (s->time->h != TIMELIB_UNSET) {
					s->time->h += tmp;
				}
				break;
			case TIMELIB_FORMAT_MINUTE_TWO_DIGIT: /* two digit minute, with leading zero */
				{
					int length;
					timelib_sll min;

					TIMELIB_CHECK_NUMBER;
					min = timelib_get_nr_ex(&ptr, 2, &length);
					if (min == TIMELIB_UNSET || length != 2) {
						add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_MINUTE, "A two digit minute could not be found", string, begin);
						break;
					}

					s->time->have_time = 1;
					s->time->i = min;
				}
				break;
			case TIMELIB_FORMAT_SECOND_TWO_DIGIT: /* two digit second, with leading zero */
				{
					int length;
					timelib_sll sec;

					TIMELIB_CHECK_NUMBER;
					sec = timelib_get_nr_ex(&ptr, 2, &length);
					if (sec == TIMELIB_UNSET || length != 2) {
						add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_SECOND, "A two digit second could not be found", string, begin);
						break;
					}

					s->time->have_time = 1;
					s->time->s = sec;
				}
				break;
			case TIMELIB_FORMAT_MICROSECOND_SIX_DIGIT: /* up to six digit microsecond */
				{
					double f;
					const char *tptr;

					TIMELIB_CHECK_NUMBER;
					tptr = ptr;
					if ((f = timelib_get_nr(&ptr, 6)) == TIMELIB_UNSET || (ptr - tptr < 1)) {
						add_pbf_error(s, TIMELIB_ERR_NO_SIX_DIGIT_MICROSECOND, "A six digit microsecond could not be found", string, begin);
						break;
					}

					s->time->us = (f * pow(10, 6 - (ptr - tptr)));
				}
				break;
			case TIMELIB_FORMAT_MILLISECOND_THREE_DIGIT: /* up to three digit millisecond */
				{
					double f;
					const char *tptr;

					TIMELIB_CHECK_NUMBER;
					tptr = ptr;
					if ((f = timelib_get_nr(&ptr, 3)) == TIMELIB_UNSET || (ptr - tptr < 1)) {
						add_pbf_error(s, TIMELIB_ERR_NO_THREE_DIGIT_MILLISECOND, "A three digit millisecond could not be found", string, begin);
						break;
					}
					
					s->time->us = (f * pow(10, 3 - (ptr - tptr)) * 1000);
				}
				break;
			case TIMELIB_FORMAT_WHITESPACE: /* any sort of whitespace (' ' and \t) */
				timelib_eat_spaces(&ptr);
				break;
			case TIMELIB_FORMAT_EPOCH_SECONDS: /* epoch seconds */
				TIMELIB_CHECK_SIGNED_NUMBER;
				tmp = timelib_get_signed_nr(s, &ptr, 24);
				s->time->have_zone = 1;
				s->time->sse = tmp;
				s->time->is_localtime = 1;
				s->time->zone_type = TIMELIB_ZONETYPE_OFFSET;
				s->time->z = 0;
				s->time->dst = 0;
				timelib_update_from_sse(s->time);
				break;

			case TIMELIB_FORMAT_ANY_SEPARATOR: /* separation symbol */
				if (timelib_lookup_format(*ptr, format_map) != TIMELIB_FORMAT_SEPARATOR) {
					add_pbf_error(s, TIMELIB_ERR_NO_SEP_SYMBOL, "The separation symbol ([;:/.,-]) could not be found", string, begin);
					break;
				}
				
				++ptr;
				break;

			case TIMELIB_FORMAT_SEPARATOR:
				if (*ptr != *fptr) {
					add_pbf_error(s, TIMELIB_ERR_NO_SEP_SYMBOL, "The separation symbol could not be found", string, begin);
					break;
				}

				++ptr;
				break;

			case TIMELIB_FORMAT_RESET_ALL: /* reset all fields to default */
				timelib_time_reset_fields(s->time);
				break; /* break intentionally not missing */

			case TIMELIB_FORMAT_RESET_ALL_WHEN_NOT_SET: /* reset all fields to default when not set */
				timelib_time_reset_unset_fields(s->time);
				break; /* break intentionally not missing */

			case TIMELIB_FORMAT_RANDOM_CHAR: /* random char */
				++ptr;
				break;

			case TIMELIB_FORMAT_ESCAPE: /* escaped char */
				if (!fptr[1]) {
					add_pbf_error(s, TIMELIB_ERR_EXPECTED_ESCAPE_CHAR, "Escaped character expected", string, begin);
					break;
				}
				fptr++;
				if (*ptr != *fptr) {
					add_pbf_error(s, TIMELIB_ERR_NO_ESCAPED_CHAR, "The escaped character could not be found", string, begin);
					break;
				}

				++ptr;
				break;

			case TIMELIB_FORMAT_SKIP_TO_SEPARATOR: /* random chars until a separator or number ([ \t.,:;/-0123456789]) */
				timelib_eat_until_separator(&ptr);
				break;

			case TIMELIB_FORMAT_ALLOW_EXTRA_CHARACTERS: /* allow extra chars in the format */
				allow_extra = true;
				break;
			case TIMELIB_FORMAT_YEAR_ISO:
				if ((iso_year = timelib_get_nr(&ptr, 4)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_FOUR_DIGIT_YEAR_ISO, "A four digit ISO year could not be found", string, begin);
					break;
				}

				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_WEEK_OF_YEAR_ISO:
				if ((iso_week_of_year = timelib_get_nr(&ptr, 2)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_TWO_DIGIT_WEEK, "A two digit ISO week could not be found", string, begin);
					break;
				}
				/* Range is 1 - 53  for ISO week of year */
				if (iso_week_of_year < 1 || iso_week_of_year > 53) {
					add_pbf_error(s, TIMELIB_ERR_INVALID_WEEK, "ISO Week must be between 1 and 53", string, begin);
					break;
				}

				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_DAY_OF_WEEK_ISO:
				if ((iso_day_of_week = timelib_get_nr(&ptr, 1)) == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_NO_DAY_OF_WEEK, "A single digit day of week could not be found", string, begin);
					break;
				}
				if (iso_day_of_week < 1 || iso_day_of_week > 7) {
					add_pbf_error(s, TIMELIB_ERR_INVALID_DAY_OF_WEEK, "Day of week must be between 1 and 7", string, begin);
					break;
				}

				s->time->have_date = 1;
				break;
			case TIMELIB_FORMAT_TIMEZONE_OFFSET: /* timezone */
				{
					int tz_not_found;

					s->time->z = timelib_parse_zone(&ptr, &s->time->dst, s->time, &tz_not_found, s->tzdb, tz_get_wrapper);
					if (tz_not_found) {
						add_pbf_error(s, TIMELIB_ERR_TZID_NOT_FOUND, "The timezone could not be found in the database", string, begin);
						break;
					}

					s->time->have_zone = 1;
				}
				break;
			case TIMELIB_FORMAT_TIMEZONE_OFFSET_MINUTES: /* timezone format +/-mmm */
				s->time->z = timelib_parse_tz_minutes(&ptr, s->time);
				if (s->time->z == TIMELIB_UNSET) {
					add_pbf_error(s, TIMELIB_ERR_INVALID_TZ_OFFSET, "Invalid timezone offset in minutes", string, begin);
					break;
				}

				s->time->have_zone = 1;
				break;
			case TIMELIB_FORMAT_LITERAL:
			default:
				if (*fptr != *ptr) {
					add_pbf_error(s, TIMELIB_ERR_WRONG_FORMAT_SEP, "The format separator does not match", string, begin);
				}
				ptr++;
		}
		fptr++;
	}
	if (*ptr) {
		if (allow_extra) {
			add_pbf_warning(s, TIMELIB_WARN_TRAILING_DATA, "Trailing data", string, ptr);
		} else {
			add_pbf_error(s, TIMELIB_ERR_TRAILING_DATA, "Trailing data", string, ptr);
		}
	}

	if (*fptr) {
		/* Trailing reset specifiers are valid. */
		int done = 0;
		while (*fptr && !done) {
			switch (timelib_lookup_format(*fptr, format_map)) {
				case TIMELIB_FORMAT_RESET_ALL: /* reset all fields to default */
					timelib_time_reset_fields(s->time);
					break;

				case TIMELIB_FORMAT_RESET_ALL_WHEN_NOT_SET: /* reset all fields to default when not set */
					timelib_time_reset_unset_fields(s->time);
					break;
				case TIMELIB_FORMAT_ALLOW_EXTRA_CHARACTERS:
					break;

				default:
					add_pbf_error(s, TIMELIB_ERR_DATA_MISSING, "Not enough data available to satisfy format", string, ptr);
					done = 1;
			}
			fptr++;
		}
	}

	/* clean up a bit */
	if (s->time->h != TIMELIB_UNSET || s->time->i != TIMELIB_UNSET || s->time->s != TIMELIB_UNSET || s->time->us != TIMELIB_UNSET) {
		if (s->time->h == TIMELIB_UNSET ) {
			s->time->h = 0;
		}
		if (s->time->i == TIMELIB_UNSET ) {
			s->time->i = 0;
		}
		if (s->time->s == TIMELIB_UNSET ) {
			s->time->s = 0;
		}
		if (s->time->us == TIMELIB_UNSET ) {
			s->time->us = 0;
		}
	}

	/* Check for mixing of ISO dates with natural dates. */
	if (s->time->y != TIMELIB_UNSET && (iso_week_of_year != TIMELIB_UNSET || iso_year != TIMELIB_UNSET || iso_day_of_week != TIMELIB_UNSET)) {
		add_pbf_error(s, TIMELIB_ERR_MIX_ISO_WITH_NATURAL, "Mixing of ISO dates with natural dates is not allowed", string, ptr);
	}
	if (iso_year != TIMELIB_UNSET && (s->time->y != TIMELIB_UNSET || s->time->m != TIMELIB_UNSET || s->time->d != TIMELIB_UNSET)) {
		add_pbf_error(s, TIMELIB_ERR_MIX_ISO_WITH_NATURAL, "Mixing of ISO dates with natural dates is not allowed", string, ptr);
	}

	/* Convert ISO values */
	if (iso_year != TIMELIB_UNSET) {
		/* Default week of year and day of week to 1. */
		if (iso_week_of_year == TIMELIB_UNSET) {
			iso_week_of_year = 1;
		}
		if (iso_day_of_week == TIMELIB_UNSET) {
			iso_day_of_week = 1;
		}
		timelib_date_from_isodate(iso_year, iso_week_of_year, iso_day_of_week, &s->time->y, &s->time->m, &s->time->d);
	} else if (iso_week_of_year != TIMELIB_UNSET || iso_day_of_week != TIMELIB_UNSET) {
		add_pbf_warning(s, TIMELIB_WARN_INVALID_DATE, "The parsed date was invalid", string, ptr);
	}

	/* do funky checking whether the parsed time was valid time */
	if (s->time->h != TIMELIB_UNSET && s->time->i != TIMELIB_UNSET &&
		s->time->s != TIMELIB_UNSET &&
		!timelib_valid_time( s->time->h, s->time->i, s->time->s)) {
		add_pbf_warning(s, TIMELIB_WARN_INVALID_TIME, "The parsed time was invalid", string, ptr);
	}
	/* do funky checking whether the parsed date was valid date */
	if (s->time->y != TIMELIB_UNSET && s->time->m != TIMELIB_UNSET &&
		s->time->d != TIMELIB_UNSET &&
		!timelib_valid_date( s->time->y, s->time->m, s->time->d)) {
		add_pbf_warning(s, TIMELIB_WARN_INVALID_DATE, "The parsed date was invalid", string, ptr);
	}

	if (errors) {
		*errors = in.errors;
	} else {
		timelib_error_container_dtor(in.errors);
	}
	return in.time;
}

void timelib_fill_holes(timelib_time *parsed, timelib_time *now, int options)
{
	if (!(options & TIMELIB_OVERRIDE_TIME) && parsed->have_date && !parsed->have_time) {
		parsed->h = 0;
		parsed->i = 0;
		parsed->s = 0;
		parsed->us = 0;
	}
	if (
		parsed->y != TIMELIB_UNSET || parsed->m != TIMELIB_UNSET || parsed->d != TIMELIB_UNSET ||
		parsed->h != TIMELIB_UNSET || parsed->i != TIMELIB_UNSET || parsed->s != TIMELIB_UNSET
	) {
		if (parsed->us == TIMELIB_UNSET) parsed->us = 0;
	} else {
		if (parsed->us == TIMELIB_UNSET) parsed->us = now->us != TIMELIB_UNSET ? now->us : 0;
	}
	if (parsed->y == TIMELIB_UNSET) parsed->y = now->y != TIMELIB_UNSET ? now->y : 0;
	if (parsed->m == TIMELIB_UNSET) parsed->m = now->m != TIMELIB_UNSET ? now->m : 0;
	if (parsed->d == TIMELIB_UNSET) parsed->d = now->d != TIMELIB_UNSET ? now->d : 0;
	if (parsed->h == TIMELIB_UNSET) parsed->h = now->h != TIMELIB_UNSET ? now->h : 0;
	if (parsed->i == TIMELIB_UNSET) parsed->i = now->i != TIMELIB_UNSET ? now->i : 0;
	if (parsed->s == TIMELIB_UNSET) parsed->s = now->s != TIMELIB_UNSET ? now->s : 0;

	if (!parsed->tz_info) {
		parsed->tz_info = now->tz_info ? (!(options & TIMELIB_NO_CLONE) ? timelib_tzinfo_clone(now->tz_info) : now->tz_info) : NULL;

		if (parsed->z == TIMELIB_UNSET) parsed->z = now->z != TIMELIB_UNSET ? now->z : 0;
		if (parsed->dst == TIMELIB_UNSET) parsed->dst = now->dst != TIMELIB_UNSET ? now->dst : 0;

		if (!parsed->tz_abbr) {
			parsed->tz_abbr = now->tz_abbr ? timelib_strdup(now->tz_abbr) : NULL;
		}
	}

	if (parsed->zone_type == 0 && now->zone_type != 0) {
		parsed->zone_type = now->zone_type;
/*		parsed->tz_abbr = now->tz_abbr ? timelib_strdup(now->tz_abbr) : NULL;
		parsed->tz_info = now->tz_info ? timelib_tzinfo_clone(now->tz_info) : NULL;
*/		parsed->is_localtime = 1;
	}
/*	timelib_dump_date(parsed, 2);
	timelib_dump_date(now, 2);
*/
}

const char *timelib_timezone_id_from_abbr(const char *abbr, timelib_long gmtoffset, int isdst)
{
	const timelib_tz_lookup_table *tp;

	tp = abbr_search(abbr, gmtoffset, isdst);
	if (tp) {
		return (tp->full_tz_name);
	} else {
		return NULL;
	}
}

const timelib_tz_lookup_table *timelib_timezone_abbreviations_list(void)
{
	return timelib_timezone_lookup;
}

#ifdef DEBUG_PARSER_STUB
int main(void)
{
	timelib_time time = timelib_strtotime("May 12");

	printf ("%04d-%02d-%02d %02d:%02d:%02d.%-5d %+04d %1d",
		time.y, time.m, time.d, time.h, time.i, time.s, time.f, time.z, time.dst);
	if (time.have_relative) {
		printf ("%3dY %3dM %3dD / %3dH %3dM %3dS",
			time.relative.y, time.relative.m, time.relative.d, time.relative.h, time.relative.i, time.relative.s);
	}
	if (time.have_weekday_relative) {
		printf (" / %d", time.relative.weekday);
	}
	if (time.have_weeknr_day) {
		printf(" / %dW%d", time.relative.weeknr_day.weeknr, time.relative.weeknr_day.dayofweek);
	}
	return 0;
}
#endif

/*
 * vim: syntax=c
 */
