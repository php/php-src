/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Derick Rethans
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

#ifndef __TIMELIB_H__
#define __TIMELIB_H__

#ifdef HAVE_TIMELIB_CONFIG_H
# include "timelib_config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <inttypes.h>

# ifndef HAVE_INT32_T
#  if SIZEOF_INT == 4
typedef int int32_t;
#  elif SIZEOF_LONG == 4
typedef long int int32_t;
#  endif
# endif

# ifndef HAVE_UINT32_T
#  if SIZEOF_INT == 4
typedef unsigned int uint32_t;
#  elif SIZEOF_LONG == 4
typedef unsigned long int uint32_t;
#  endif
# endif

#ifdef _WIN32
# if _MSC_VER >= 1600
# include <stdint.h>
# endif
# ifndef SIZEOF_INT
#  define SIZEOF_INT 4
# endif
# ifndef SIZEOF_LONG
#  define SIZEOF_LONG 4
# endif
# ifndef int32_t
typedef __int32           int32_t;
# endif
# ifndef uint32_t
typedef unsigned __int32  uint32_t;
# endif
# ifndef int64_t
typedef __int64           int64_t;
# endif
# ifndef uint64_t
typedef unsigned __int64  uint64_t;
# endif
# ifndef PRId32
#  define PRId32       "I32d"
# endif
# ifndef PRIu32
#  define PRIu32       "I32u"
# endif
# ifndef PRId64
#  define PRId64       "I64d"
# endif
# ifndef PRIu64
#  define PRIu64       "I64u"
# endif
# ifndef INT32_MAX
#define INT32_MAX    _I32_MAX
# endif
# ifndef INT32_MIN
#define INT32_MIN    ((int32_t)_I32_MIN)
# endif
# ifndef UINT32_MAX
#define UINT32_MAX   _UI32_MAX
# endif
# ifndef INT64_MIN
#define INT64_MIN    ((int64_t)_I64_MIN)
# endif
# ifndef INT64_MAX
#define INT64_MAX    _I64_MAX
# endif
# ifndef UINT64_MAX
#define UINT64_MAX   _UI64_MAX
# endif
#endif

#if (defined(__x86_64__) || defined(__LP64__) || defined(_LP64) || defined(_WIN64)) && !defined(TIMELIB_FORCE_LONG32)
typedef int64_t timelib_long;
typedef uint64_t timelib_ulong;
# define TIMELIB_LONG_MAX INT64_MAX
# define TIMELIB_LONG_MIN INT64_MIN
# define TIMELIB_ULONG_MAX UINT64_MAX
# define TIMELIB_LONG_FMT "%" PRId64
# define TIMELIB_ULONG_FMT "%" PRIu64
#else
typedef int32_t timelib_long;
typedef uint32_t timelib_ulong;
# define TIMELIB_LONG_MAX INT32_MAX
# define TIMELIB_LONG_MIN INT32_MIN
# define TIMELIB_ULONG_MAX UINT32_MAX
# define TIMELIB_LONG_FMT "%" PRId32
# define TIMELIB_ULONG_FMT "%" PRIu32
#endif

#if defined(_MSC_VER)
typedef uint64_t timelib_ull;
typedef int64_t timelib_sll;
# define TIMELIB_LL_CONST(n) n ## i64
#else
typedef unsigned long long timelib_ull;
typedef signed long long timelib_sll;
# define TIMELIB_LL_CONST(n) n ## ll
#endif

typedef struct _ttinfo ttinfo;
typedef struct _tlinfo tlinfo;

typedef struct _tlocinfo
{
	char country_code[3];
	double latitude;
	double longitude;
	char *comments;
} tlocinfo;

typedef struct _timelib_tzinfo
{
	char    *name;
	struct {
		uint32_t ttisgmtcnt;
		uint32_t ttisstdcnt;
		uint32_t leapcnt;
		uint32_t timecnt;
		uint32_t typecnt;
		uint32_t charcnt;
	} _bit32;
	struct {
		uint64_t ttisgmtcnt;
		uint64_t ttisstdcnt;
		uint64_t leapcnt;
		uint64_t timecnt;
		uint64_t typecnt;
		uint64_t charcnt;
	} bit64;

	int64_t *trans;
	unsigned char *trans_idx;

	ttinfo  *type;
	char    *timezone_abbr;

	tlinfo  *leap_times;
	unsigned char bc;
	tlocinfo location;
} timelib_tzinfo;

typedef struct _timelib_rel_time {
	timelib_sll y, m, d; /* Years, Months and Days */
	timelib_sll h, i, s; /* Hours, mInutes and Seconds */
	timelib_sll us;      /* Microseconds */

	int weekday; /* Stores the day in 'next monday' */
	int weekday_behavior; /* 0: the current day should *not* be counted when advancing forwards; 1: the current day *should* be counted */

	int first_last_day_of;
	int invert; /* Whether the difference should be inverted */
	timelib_sll days; /* Contains the number of *days*, instead of Y-M-D differences */

	struct {
		unsigned int type;
		timelib_sll amount;
	} special;

	unsigned int   have_weekday_relative, have_special_relative;
} timelib_rel_time;

typedef struct _timelib_time_offset {
	int32_t      offset;
	unsigned int leap_secs;
	unsigned int is_dst;
	char        *abbr;
	timelib_sll  transition_time;
} timelib_time_offset;

typedef struct _timelib_time {
	timelib_sll      y, m, d;     /* Year, Month, Day */
	timelib_sll      h, i, s;     /* Hour, mInute, Second */
	timelib_sll      us;          /* Microseconds */
	int              z;           /* UTC offset in seconds */
	char            *tz_abbr;     /* Timezone abbreviation (display only) */
	timelib_tzinfo  *tz_info;     /* Timezone structure */
	signed int       dst;         /* Flag if we were parsing a DST zone */
	timelib_rel_time relative;

	timelib_sll      sse;         /* Seconds since epoch */

	unsigned int   have_time, have_date, have_zone, have_relative, have_weeknr_day;

	unsigned int   sse_uptodate; /* !0 if the sse member is up to date with the date/time members */
	unsigned int   tim_uptodate; /* !0 if the date/time members are up to date with the sse member */
	unsigned int   is_localtime; /*  1 if the current struct represents localtime, 0 if it is in GMT */
	unsigned int   zone_type;    /*  1 time offset,
	                              *  3 TimeZone identifier,
	                              *  2 TimeZone abbreviation */
} timelib_time;

typedef struct _timelib_abbr_info {
	timelib_sll  utc_offset;
	char        *abbr;
	int          dst;
} timelib_abbr_info;

#define TIMELIB_WARN_MASK                      0x1ff
#define TIMELIB_ERR_MASK                       0x2ff

#define TIMELIB_WARN_DOUBLE_TZ                 0x101
#define TIMELIB_WARN_INVALID_TIME              0x102
#define TIMELIB_WARN_INVALID_DATE              0x103
#define TIMELIB_WARN_TRAILING_DATA             0x11a

#define TIMELIB_ERR_DOUBLE_TZ                  0x201
#define TIMELIB_ERR_TZID_NOT_FOUND             0x202
#define TIMELIB_ERR_DOUBLE_TIME                0x203
#define TIMELIB_ERR_DOUBLE_DATE                0x204
#define TIMELIB_ERR_UNEXPECTED_CHARACTER       0x205
#define TIMELIB_ERR_EMPTY_STRING               0x206
#define TIMELIB_ERR_UNEXPECTED_DATA            0x207
#define TIMELIB_ERR_NO_TEXTUAL_DAY             0x208
#define TIMELIB_ERR_NO_TWO_DIGIT_DAY           0x209
#define TIMELIB_ERR_NO_THREE_DIGIT_DAY_OF_YEAR 0x20a
#define TIMELIB_ERR_NO_TWO_DIGIT_MONTH         0x20b
#define TIMELIB_ERR_NO_TEXTUAL_MONTH           0x20c
#define TIMELIB_ERR_NO_TWO_DIGIT_YEAR          0x20d
#define TIMELIB_ERR_NO_FOUR_DIGIT_YEAR         0x20e
#define TIMELIB_ERR_NO_TWO_DIGIT_HOUR          0x20f
#define TIMELIB_ERR_HOUR_LARGER_THAN_12        0x210
#define TIMELIB_ERR_MERIDIAN_BEFORE_HOUR       0x211
#define TIMELIB_ERR_NO_MERIDIAN                0x212
#define TIMELIB_ERR_NO_TWO_DIGIT_MINUTE        0x213
#define TIMELIB_ERR_NO_TWO_DIGIT_SECOND        0x214
#define TIMELIB_ERR_NO_SIX_DIGIT_MICROSECOND   0x215
#define TIMELIB_ERR_NO_SEP_SYMBOL              0x216
#define TIMELIB_ERR_EXPECTED_ESCAPE_CHAR       0x217
#define TIMELIB_ERR_NO_ESCAPED_CHAR            0x218
#define TIMELIB_ERR_WRONG_FORMAT_SEP           0x219
#define TIMELIB_ERR_TRAILING_DATA              0x21a
#define TIMELIB_ERR_DATA_MISSING               0x21b
#define TIMELIB_ERR_NO_THREE_DIGIT_MILLISECOND 0x21c
#define TIMELIB_ERR_NO_FOUR_DIGIT_YEAR_ISO     0x21d
#define TIMELIB_ERR_NO_TWO_DIGIT_WEEK          0x21e
#define TIMELIB_ERR_INVALID_WEEK               0x21f
#define TIMELIB_ERR_NO_DAY_OF_WEEK             0x220
#define TIMELIB_ERR_INVALID_DAY_OF_WEEK        0x221
#define TIMELIB_ERR_INVALID_SPECIFIER          0x222
#define TIMELIB_ERR_INVALID_TZ_OFFSET          0x223
#define TIMELIB_ERR_FORMAT_LITERAL_MISMATCH    0x224
#define TIMELIB_ERR_MIX_ISO_WITH_NATURAL       0x225

#define TIMELIB_ZONETYPE_OFFSET 1
#define TIMELIB_ZONETYPE_ABBR   2
#define TIMELIB_ZONETYPE_ID     3

typedef struct _timelib_error_message {
	int         error_code;
	int         position;
	char        character;
	char       *message;
} timelib_error_message;

typedef struct _timelib_error_container {
	timelib_error_message *error_messages;
	timelib_error_message *warning_messages;
	int                    error_count;
	int                    warning_count;
} timelib_error_container;

typedef struct _timelib_tz_lookup_table {
	char       *name;
	int         type;
	float       gmtoffset;
	char       *full_tz_name;
} timelib_tz_lookup_table;

typedef struct _timelib_tzdb_index_entry {
	char *id;
	unsigned int pos;
} timelib_tzdb_index_entry;

typedef struct _timelib_tzdb {
	char                           *version;
	int                             index_size;
	const timelib_tzdb_index_entry *index;
	const unsigned char            *data;
} timelib_tzdb;

#ifndef timelib_malloc
# define timelib_malloc  malloc
# define timelib_realloc realloc
# define timelib_calloc  calloc
# define timelib_strdup  strdup
# define timelib_free    free
#endif

#define TIMELIB_VERSION 202003
#define TIMELIB_EXTENDED_VERSION 20203001
#define TIMELIB_ASCII_VERSION "2020.03"

#define TIMELIB_NONE             0x00
#define TIMELIB_OVERRIDE_TIME    0x01
#define TIMELIB_NO_CLONE         0x02

#define TIMELIB_UNSET   -99999

/* An entry for each of these error codes is also in the
 * timelib_error_messages array in timelib.c */
#define TIMELIB_ERROR_NO_ERROR                            0x00
#define TIMELIB_ERROR_CANNOT_ALLOCATE                     0x01
#define TIMELIB_ERROR_CORRUPT_TRANSITIONS_DONT_INCREASE   0x02
#define TIMELIB_ERROR_CORRUPT_NO_64BIT_PREAMBLE           0x03
#define TIMELIB_ERROR_CORRUPT_NO_ABBREVIATION             0x04
#define TIMELIB_ERROR_UNSUPPORTED_VERSION                 0x05
#define TIMELIB_ERROR_NO_SUCH_TIMEZONE                    0x06
#define TIMELIB_ERROR_SLIM_FILE                           0x07

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _timelib_format_specifier_code {
	TIMELIB_FORMAT_ALLOW_EXTRA_CHARACTERS = 0,
	TIMELIB_FORMAT_ANY_SEPARATOR,
	TIMELIB_FORMAT_DAY_TWO_DIGIT,
	TIMELIB_FORMAT_DAY_TWO_DIGIT_PADDED,
	TIMELIB_FORMAT_DAY_OF_WEEK_ISO,
	TIMELIB_FORMAT_DAY_OF_WEEK,
	TIMELIB_FORMAT_DAY_OF_YEAR,
	TIMELIB_FORMAT_DAY_SUFFIX,
	TIMELIB_FORMAT_END,
	TIMELIB_FORMAT_EPOCH_SECONDS,
	TIMELIB_FORMAT_ESCAPE,
	TIMELIB_FORMAT_HOUR_TWO_DIGIT_12_MAX,
	TIMELIB_FORMAT_HOUR_TWO_DIGIT_12_MAX_PADDED,
	TIMELIB_FORMAT_HOUR_TWO_DIGIT_24_MAX,
	TIMELIB_FORMAT_HOUR_TWO_DIGIT_24_MAX_PADDED,
	TIMELIB_FORMAT_LITERAL,
	TIMELIB_FORMAT_MERIDIAN,
	TIMELIB_FORMAT_MICROSECOND_SIX_DIGIT,
	TIMELIB_FORMAT_MILLISECOND_THREE_DIGIT,
	TIMELIB_FORMAT_MINUTE_TWO_DIGIT,
	TIMELIB_FORMAT_MONTH_TWO_DIGIT,
	TIMELIB_FORMAT_MONTH_TWO_DIGIT_PADDED,
	TIMELIB_FORMAT_RANDOM_CHAR,
	TIMELIB_FORMAT_RESET_ALL,
	TIMELIB_FORMAT_RESET_ALL_WHEN_NOT_SET,
	TIMELIB_FORMAT_SECOND_TWO_DIGIT,
	TIMELIB_FORMAT_SEPARATOR,
	TIMELIB_FORMAT_SKIP_TO_SEPARATOR,
	TIMELIB_FORMAT_TEXTUAL_DAY_3_LETTER,
	TIMELIB_FORMAT_TEXTUAL_DAY_FULL,
	TIMELIB_FORMAT_TEXTUAL_MONTH_3_LETTER,
	TIMELIB_FORMAT_TEXTUAL_MONTH_FULL,
	TIMELIB_FORMAT_TIMEZONE_OFFSET,
	TIMELIB_FORMAT_TIMEZONE_OFFSET_MINUTES,
	TIMELIB_FORMAT_WEEK_OF_YEAR_ISO,
	TIMELIB_FORMAT_WEEK_OF_YEAR,
	TIMELIB_FORMAT_WHITESPACE,
	TIMELIB_FORMAT_YEAR_TWO_DIGIT,
	TIMELIB_FORMAT_YEAR_FOUR_DIGIT,
	TIMELIB_FORMAT_YEAR_ISO
} timelib_format_specifier_code;

typedef struct _timelib_format_specifier {
	char                          specifier;
	timelib_format_specifier_code code;
} timelib_format_specifier;

typedef struct _timelib_format_config {
	const timelib_format_specifier *format_map;
	/* Format speciifiers must be preceded by 'prefix_char' if not '\0'. */
	char                            prefix_char;
} timelib_format_config;

/* Function pointers */
typedef timelib_tzinfo* (*timelib_tz_get_wrapper)(const char *tzname, const timelib_tzdb *tzdb, int *error_code);

/* From dow.c */
/* Calculates the day of the week from y, m, and d. 0=Sunday..6=Saturday */
timelib_sll timelib_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d);

/* Calculates the day of the ISO week from y, m, and d. 1=Monday, 7=Sunday */
timelib_sll timelib_iso_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d);

/* Calculates the day of the year according to y-m-d. 0=Jan 1st..364/365=Dec
 * 31st */
timelib_sll timelib_day_of_year(timelib_sll y, timelib_sll m, timelib_sll d);

/* Calculates the day of the year according to y-w-dow. 0..364/365 */
timelib_sll timelib_daynr_from_weeknr(timelib_sll iy, timelib_sll iw, timelib_sll id);

/* Calculates the number of days in month m for year y. 28..31 */
timelib_sll timelib_days_in_month(timelib_sll y, timelib_sll m);

/* Calculates the ISO year and week from y, m, and d, into iw and iy */
void timelib_isoweek_from_date(timelib_sll y, timelib_sll m, timelib_sll d, timelib_sll *iw, timelib_sll *iy);

/* Calculates the ISO year, week, and day of week from y, m, and d, into iy,
 * iw, and id */
void timelib_isodate_from_date(timelib_sll y, timelib_sll m, timelib_sll d, timelib_sll *iy, timelib_sll *iw, timelib_sll *id);

/* Calculates the year, month, and day from iy, iw, and iw, into y, m, and d */
void timelib_date_from_isodate(timelib_sll iy, timelib_sll iw, timelib_sll id, timelib_sll *y, timelib_sll *m, timelib_sll *d);

/* Returns true if h, i and s fit in the range 00:00:00..23:59:59, false
 * otherwise */
int timelib_valid_time(timelib_sll h, timelib_sll i, timelib_sll s);

/* Returns true if m fits in the range 1..12, and d fits in the range
 * 1..<days-in-month> for year y */
int timelib_valid_date(timelib_sll y, timelib_sll m, timelib_sll d);

/* From parse_date.re */

/* Parses the date/time string in 's' with length 'len' into the constituent
 * parts of timelib_time*.
 *
 * Depending on the contents of the string 's', not all elements might be
 * filled. You can check whether a specific element has been parsed by
 * comparing with the TIMELIB_UNSET define.
 *
 * If errors occur, this function keeps already parsed elements in the
 * returned timelib_time* value.
 *
 * If the **errors points to a timelib_error_container variable, warnings
 * and errors will be recorded. You are responsible for freeing the stored
 * information with timelib_error_container_dtor(). To see whether errors have
 * occurred, inspect errors->errors_count. To see whether warnings have occurred,
 * inspect errors->warnings_count.
 *
 * The returned timelib_time* value is dynamically allocated and should be
 * freed with timelib_time_dtor().
 */
timelib_time *timelib_strtotime(const char *s, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper);

/* Parses the date/time string in 's' with length 'len' into the constituent
 * parts of timelib_time* according to the format in 'format'.
 *
 * Depending on the contents of the string 's', not all elements might be
 * filled. You can check whether a specific element has been parsed by
 * comparing with the TIMELIB_UNSET define.
 *
 * If errors occur, this function keeps already parsed elements in the
 * returned timelib_time* value.
 *
 * If the **errors points to a timelib_error_container variable, warnings
 * and errors will be recorded. You are responsible for freeing the stored
 * information with timelib_error_container_dtor(). To see whether errors have
 * occurred, inspect errors->errors_count. To see whether warnings have occurred,
 * inspect errors->warnings_count.
 *
 * The returned timelib_time* value is dynamically allocated and should be
 * freed with timelib_time_dtor().
 */
timelib_time *timelib_parse_from_format(const char *format, const char *s, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper);

/* Parses the date/time string in 's' with length 'len' into the constituent
 * parts of timelib_time* according to the format in 'format' with format
 * specifier configuration 'format_config'.
 *
 * 'format_map' is an array of pairs, with the first element being the format
 * specifier as a character and the second element corresponds to the
 * representation of the specifier from the enum list
 * 'timelib_format_specifier_code'.
 *
 * Note: 'format_map' must be terminated with specifier '\0' to indicate to the
 * parser that there are no more format specifiers in the list.
 */
timelib_time *timelib_parse_from_format_with_map(const char *format, const char *s, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper, const timelib_format_config* format_config);

/* Fills the gaps in the parsed timelib_time with information from the reference date/time in 'now'
 *
 * If any of the 'parsed' y, m, d, h, i or s parameters is unset (TIMELIB_UNSET):
 * - if microtime (us) is unset, then the us of the parsed time is set to 0.
 * - else if microtime (us) is unset and 'now'->'us' is set, use it, otherwise use 0.
 *
 * For either of the 'parsed' y, m, d, h, i, s, z (utc offset in seconds) or
 * dst is unset, set it to the corresponding value in 'now' if set, otherwise
 * set it to 0.
 *
 * It duplicates tz_abbr if unset in 'parsed' but set in 'now'.
 *
 * It duplicates tz_info if unset in 'parsed', but set in 'now' unless
 * TIMELIB_NO_CLONE is passed, in which case only the pointer in 'parsed' is
 * set to 'now'.
 *
 * If the option TIMELIB_OVERRIDE_TIME is passed and the parsed date/time has
 * no time portion, the function will ignore the time aspect in 'now' and
 * instead fill it with zeros.
 */
void timelib_fill_holes(timelib_time *parsed, timelib_time *now, int options);

/* Tries to convert a time zone abbreviation, gmtoffset and/or isdst flag
 * combination to a time zone identifier.
 *
 * If 'abbr' is either 'utc' or 'gmt' (case insensitve) then "UTC" is
 * returned.
 *
 * It first uses the data in the timezonemap.h file to find a matching
 * abbreviation/GMT offset combination. If not found, it uses the data in
 * fallbackmap.h to match only the GMT offset/isdst flag to try to find a
 * match. If nothing is found, NULL is returned.
 *
 * The returned char* is not duplicated, and should not be freed.
 */
char *timelib_timezone_id_from_abbr(const char *abbr, timelib_long gmtoffset, int isdst);

/* Returns an array of known time zone abbreviations
 *
 * This file is generated from the time zone database through the
 * gettzmapping.php scripts, which requires that an up-to-date time zone
 * database is used with the PHP binary that runs the script.
 *
 * Each item in the returned list contains the abbreviation, a flag whether
 * it's an abbreviation used with DST, the UTC offset in seconds, and the name
 * of the time zone identifier that this abbreviation belongs to.
 *
 * The order for each specific abbreviation is controlled through the
 * preference list in the gettzmapping.php script. Time zones that match the
 * pattern ±\d{2,4} are excluded
 */
const timelib_tz_lookup_table *timelib_timezone_abbreviations_list(void);

/**
 * DEPRECATED, but still used by PHP.
 */
timelib_long timelib_parse_zone(const char **ptr, int *dst, timelib_time *t, int *tz_not_found, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_wrapper);

/* From parse_iso_intervals.re */

/**
 * Parses a subset of an ISO 8601 intervals specification string into its
 * constituent parts.
 *
 * If the **errors points to a timelib_error_container variable, warnings
 * and errors will be recorded. You are responsible for freeing the stored
 * information with timelib_error_container_dtor(). To see whether errors have
 * occurred, inspect errors->errors_count. To see whether warnings have occurred,
 * inspect errors->warnings_count.
 */
void timelib_strtointerval(const char *s, size_t len,
                           timelib_time **begin, timelib_time **end,
                           timelib_rel_time **period, int *recurrences,
                           timelib_error_container **errors);


/* From tm2unixtime.c */

/**
 * Uses the y/m/d/h/i/s fields to calculate and store the equivalent timestamp
 * in the sse field.
 *
 * It uses the time zone information associated with 'time' to account for the
 * right UTC offset and/or DST rules. You can associate time zone information
 * with the timelib_set_timezone_* functions (see below).
 *
 * If the type is 'TIMELIB_ZONETYPE_ID' and there is no associated tzinfo, it
 * will use the second argument 'tzi' to provide the rules necessary to
 * calculate the right timestamp.
 */
void timelib_update_ts(timelib_time* time, timelib_tzinfo* tzi);

/**
 * Takes the information from the y/m/d/h/i/s fields and makes sure their
 * values are in the right range.
 *
 * If a value under- or overflows it will adjust the larger measure up (or
 * down). It also takes into account leap days.
 */
void timelib_do_normalize(timelib_time *base);

/**
 * Takes the information from the y/m/d/h/i/s fields of 'rt' and makes sure
 * their values are in the right range.
 *
 * If a value under- or overflows it will adjust the larger measure up (or
 * down). As this function operates on a *relative date/time*, it also takes
 * into account leap days and correctly accounts for the difference depending
 * on the base date/time in 'base'.
 */
void timelib_do_rel_normalize(timelib_time *base, timelib_rel_time *rt);

/* From unixtime2tm.c */

/**
 * Takes the unix timestamp in seconds from 'ts' and populates the y/m/d/h/i/s
 * fields of 'tm' without taking time zones into account
 */
void timelib_unixtime2gmt(timelib_time* tm, timelib_sll ts);

/**
 * Takes the Unix timestamp from 'ts', and calculates the y/m/d/h/i/s fields
 * according to the time zone information attached to 'tm'.
 */
void timelib_unixtime2local(timelib_time *tm, timelib_sll ts);

/**
 * Takes the Unix timestamp stored in 'tm', and calculates the y/m/d/h/i/s
 * fields according to the time zone information attached to 'tm'.
 */
void timelib_update_from_sse(timelib_time *tm);

/**
 * Attaches the UTC offset as time zone information to 't'.
 *
 * 'utc_offset' is in seconds East of UTC.
 */
void timelib_set_timezone_from_offset(timelib_time *t, timelib_sll utc_offset);

/**
 * Attaches the information from 'abbr_info' as time zone information to 't'.
 *
 * The timelib_abbr_info struct contains an abbreviation ('abbr') which string
 * value is duplicated, as well as a 'utc_offset' and 'dst' flag. It only
 * supports a 'dst' change over of 1 hour.
 */
void timelib_set_timezone_from_abbr(timelib_time *t, timelib_abbr_info abbr_info);

/**
 * Attaches the time zone information in 'tz' to to 't'.
 *
 * It fetches the right UTC offset that is currently stored in the time
 * stamp field in 't' ('sse'), and assigns that to the 'z' field and 'dst'
 * field (whether DST is in effect at the time). It also sets the current
 * abbreviation to the 'tz_addr' field, making sure that if a value was already
 * set it was freed.
 *
 * The time zone information in 'tz' is *not* duplicated into the 't' field so
 * it should not be freed until all timelib_time* variables have been freed as
 * well.
 */
void timelib_set_timezone(timelib_time *t, timelib_tzinfo *tz);

/* From parse_tz.c */

/**
 * Returns whether the time zone ID 'timezone' is available in the time zone
 * database as pointed to be 'tzdb'.
 */
int timelib_timezone_id_is_valid(const char *timezone, const timelib_tzdb *tzdb);

/**
 * Converts the binary stored time zone information from 'tzdb' for the time
 * zone 'timezone' into a structure the library can use for calculations.
 *
 * The function can be used on both timelib_builtin_db as well as a time zone
 * db as opened by timelib_zoneinfo.
 *
 * 'error_code' must not be a null pointer, and will always be written to. If
 * the value is TIMELIB_ERROR_NO_ERROR then the file was parsed without
 * problems.
 *
 * The function will return null upon failure, and also set an error code
 * through 'error_code'.
 *
 * The error code is one of the TIMELIB_ERROR_* constants as listed above.
 * These error constants can be converted into a string by
 * timelib_get_error_message.
 *
 * If the function returns not-null, the 'error_code' might have a non-null
 * value that can be used to detect incompatibilities. The only one that is
 * currently detected is whether the file is a 'slim' file, in which case
 * 'error_code' will be set to TIMELIB_ERROR_SLIM_FILE.
 *
 * This function allocates memory for the new time zone structure, which must
 * be freed after use. Although it is recommended that a cache of each used
 * time zone is kept.
 */
timelib_tzinfo *timelib_parse_tzfile(const char *timezone, const timelib_tzdb *tzdb, int *error_code);

/**
 * Frees up the resources allocated by 'timelib_parse_tzfile'.
 */
void timelib_tzinfo_dtor(timelib_tzinfo *tz);

/**
 * Deep-clones a timelib_tzinfo structure.
 *
 * This allocates resources that need to be freed with 'timelib_tzinfo_dtor'
 */
timelib_tzinfo* timelib_tzinfo_clone(timelib_tzinfo *tz);

/**
 * Returns whether DST is active with time zone 'tz' for the time stamp 'ts'.
 *
 * Returns 0 if DST is not active, 1 if DST is active, or -1 if no transitions
 * were available through 'tz'.
 */
int timelib_timestamp_is_in_dst(timelib_sll ts, timelib_tzinfo *tz);

/**
 * Returns offset information with time zone 'tz' for the time stamp 'ts'.
 *
 * The returned information contains: the offset in seconds East of UTC (in
 * 'offset'), whether DST is active ('is_dst'), what the current time zone
 * abbreviation is ('abbr') and the transition time that got to this state (in
 * 'transition_time');
 */
timelib_time_offset *timelib_get_time_zone_info(timelib_sll ts, timelib_tzinfo *tz);

/**
 * Returns the UTC offset currently applicable for the information stored in 't'.
 *
 * The value returned is the UTC offset in seconds East.
 */
timelib_sll timelib_get_current_offset(timelib_time *t);

/**
 * Displays debugging information about the time zone information in 'tz'.
 */
void timelib_dump_tzinfo(timelib_tzinfo *tz);

/**
 * Returns a pointer to the built-in time zone database.
 *
 * You must *not* free the returned pointer as it is part of the text segment.
 */
const timelib_tzdb *timelib_builtin_db(void);

/**
 * Returns a pointer to the start of an array containing a list of timezone identifiers.
 *
 * The amount of entries in the array is returned through the 'count' OUT parameter.
 *
 * Each entry contains the time zone ID ('id' field), and the position within the time zone
 * information ('pos' field). The pos field should not be used.
 */
const timelib_tzdb_index_entry *timelib_timezone_identifiers_list(const timelib_tzdb *tzdb, int *count);

/* From parse_zoneinfo.c */

/**
 * Scans the directory and subdirectories of 'directory' for valid time zone files and builds
 * a time zone database out of these files.
 *
 * Typically, the directory should point to '/usr/share/zoneinfo'.
 *
 * Unlike 'timelib_builtin_db', the return value of this function must be freed
 * with the 'timelib_zoneinfo_dtor' function.
 */
timelib_tzdb *timelib_zoneinfo(const char *directory);

/**
 * Frees up the resources as created through 'timelib_zoneinfo'.
 *
 * This function must be used to free up all the resources that have been
 * allocated while calling 'timelib_zoneinfo'.
 */
void timelib_zoneinfo_dtor(timelib_tzdb *tzdb);

/* From timelib.c */

/**
 * Returns a static string containing an error message belonging to a specific
 * error code.
 */
const char *timelib_get_error_message(int error_code);

/**
 * Allocates resources for the relative time structure.
 *
 * Must be freed with 'timelib_rel_time_dtor'.
 */
timelib_rel_time* timelib_rel_time_ctor(void);

/**
 * Frees up the resources as allocated through 'timelib_rel_time_ctor'.
 */
void timelib_rel_time_dtor(timelib_rel_time* t);

/**
 * Creates a new timelib_rel_time resource and copies over the information
 * from 'tz'.
 *
 * Must be freed with 'timelib_rel_time_dtor'.
 */
timelib_rel_time* timelib_rel_time_clone(timelib_rel_time *tz);

/**
 * Allocates resources for the time structure.
 *
 * Must be freed with 'timelib_time_dtor'.
 */
timelib_time* timelib_time_ctor(void);

/**
 * Frees up the resources as allocated through 'timelib_time_ctor'.
 */
void timelib_time_dtor(timelib_time* t);

/**
 * Creates a new timelib_time resource and copies over the information
 * from 'orig'.
 *
 * Must be freed with 'timelib_time_dtor'.
 */
timelib_time* timelib_time_clone(timelib_time* orig);

/**
 * Compares two timelib_time structures and returns which one is earlier in
 * time.
 *
 * To decide which comes earlier it uses the 'sse' (Seconds Since Epoch) and
 * 'us' (microseconds) fields.
 *
 * Returns -1 if t1 < t2, 0 if t1 == t2, and -1 if t1 > t2.
 */
int timelib_time_compare(timelib_time *t1, timelib_time *t2);

/**
 * Allocates resources for the time offset structure.
 *
 * Must be freed with 'timelib_time_offset_dtor'.
 */
timelib_time_offset* timelib_time_offset_ctor(void);

/**
 * Frees up the resources as allocated through 'timelib_time_offset_ctor'.
 */
void timelib_time_offset_dtor(timelib_time_offset* t);

/**
 * Frees up the resources allocated while converting strings to timelib_time
 * structures with the timelib_strtotime and timelib_strtointerval functions.
 */
void timelib_error_container_dtor(timelib_error_container *errors);

/**
 * Converts the 'sse' value of 'd' to a timelib_long type.
 *
 * If the value fits in the TIMELIB_LONG_MIN and TIMELIB_LONG_MAX range, the
 * value is cast to (timelib_long) and returned. If *error is not a NULL
 * pointer, it will be set to 0.
 *
 * If the value does *not* fit in the range, the function returns 0 and if
 * *error is not a NULL pointer, it will be set to 1.
 *
 * timelib_long is a 32 bit signed long integer on 32 bit platforms, and a 64
 * bit signed long long integer on 64 bit platforms. In other words, it makes
 * sure that the value in 'sse' (which is always a signed long long 64 bit
 * integer) can be used safely outside of the library.
 */
timelib_long timelib_date_to_int(timelib_time *d, int *error);

/**
 * Displays debugging information about the date/time information stored in 'd'.
 *
 * 'options' is a bit field, where:
 * - 1 controls whether the relative time portion is shown.
 * - 2 controls whether the zone type is shown.
 */
void timelib_dump_date(timelib_time *d, int options);

/**
 * Displays debugging information about the relative time information stored
 * in 'd'.
 */
void timelib_dump_rel_time(timelib_rel_time *d);

/**
 * Converts a decimal hour into hour/min/sec components
 */
void timelib_decimal_hour_to_hms(double h, int *hour, int *min, int *sec);

/**
 * Converts hour/min/sec values into a decimal hour
 */
void timelib_hms_to_decimal_hour(int hour, int min, int sec, double *h);

/* from astro.c */

/**
 * Converts the Unix Epoch time stamp 'ts' to a Julian Day
 *
 * The value returned is the number of whole days since -4714-11-24T12:00:00 UTC
 * (in the proleptic Gregorian calendar):
 * https://en.wikipedia.org/wiki/Julian_day
 */
double timelib_ts_to_julianday(timelib_sll ts);

/**
 * Converts the Unix Epoch time stamp 'ts' to the J2000 epoch
 *
 * The value returned is the number of whole days since 2000-01-01T12:00:00
 * UTC: https://en.wikipedia.org/wiki/Epoch_(astronomy)#Julian_years_and_J2000
 */
double timelib_ts_to_j2000(timelib_sll ts);

/**
 * Calculates when the Sun is above a certain latitude.
 *
 * Parameters:
 * - time: A timelib_time time describing that needs to specific midnight for a
 *         specific day.
 * - lon: The longitude of the observer (East positive, West negative).
 * - lat: The latitude of the observer (North positive, South negative).
 * - altit: The altitude. Set to -35/60 for rise/set, -6 for civil twilight,
 *          -12 for nautical, and -18 for astronomical twilight.
 * - upper_limb: set to non-zero for rise/set calculations, and 0 for twilight
 *               calculations.
 *
 * Out Parameters:
 * - h_rise: The decimal hour when the Sun rises
 * - h_set: The decimal hour when the Sun sets
 * - ts_rise: The Unix timestamp of the Sun rising
 * - ts_set: The Unix timestamp of the Sun setting
 * - ts_transit: The Unix timestmap of the Sun transitting through South
 *
 * Return Values:
 * - 0: The Sun rises and sets.
 * - +1: The Sun is always above the horizon. (ts_rise is set to ts_transit -
 *       (12 * 3600); ts_set is set to ts_transit + (12 * 3600).
 * - -1: The Sun is awlays below the horizon. (ts_rise and ts_set are set
 *       to ts_transit)
 */
int timelib_astro_rise_set_altitude(timelib_time *time, double lon, double lat, double altit, int upper_limb, double *h_rise, double *h_set, timelib_sll *ts_rise, timelib_sll *ts_set, timelib_sll *ts_transit);

/* from interval.c */

/**
 * Calculates the difference between two times
 *
 * The result is a timelib_rel_time structure that describes how you can
 * convert from 'one' to 'two' with 'timelib_add'. This does *not* necessarily
 * mean that you can go from 'two' to 'one' by using 'timelib_sub' due to the
 * way months and days are calculated.
 */
timelib_rel_time *timelib_diff(timelib_time *one, timelib_time *two);

/**
 * Adds the relative time information 'interval' to the base time 't'.
 *
 * This can be a relative time as created by 'timelib_diff', but also by more
 * complex statements such as "next workday".
 */
timelib_time *timelib_add(timelib_time *t, timelib_rel_time *interval);

/**
 * Subtracts the relative time information 'interval' to the base time 't'.
 *
 * This can be a relative time as created by 'timelib_diff'. Unlike with
 * 'timelib_add', this does not support more complex statements such as "next
 * workday".
 */
timelib_time *timelib_sub(timelib_time *t, timelib_rel_time *interval);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
