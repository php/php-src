/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Derick Rethans
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

#ifndef __TIMELIB_STRUCTS_H__
#define __TIMELIB_STRUCTS_H__

#ifdef HAVE_TIMELIB_CONFIG_H
# include "timelib_config.h"
#endif

#ifndef TIMELIB_OMIT_STDINT

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

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

#endif /* TIMELIB_OMIT_STDINT */

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if defined(__x86_64__) || defined(__LP64__) || defined(_LP64) || defined(_WIN64)
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

typedef struct ttinfo
{
	int32_t      offset;
	int          isdst;
	unsigned int abbr_idx;

	unsigned int isstdcnt;
	unsigned int isgmtcnt;
} ttinfo;

typedef struct tlinfo
{
	int32_t  trans;
	int32_t  offset;
} tlinfo;

typedef struct tlocinfo
{
	char country_code[3];
	double latitude;
	double longitude;
	char *comments;
} tlocinfo;

typedef struct timelib_tzinfo
{
	char    *name;
	struct {
		uint32_t ttisgmtcnt;
		uint32_t ttisstdcnt;
		uint32_t leapcnt;
		uint32_t timecnt;
		uint32_t typecnt;
		uint32_t charcnt;
	} bit32;
	struct {
		uint64_t ttisgmtcnt;
		uint64_t ttisstdcnt;
		uint64_t leapcnt;
		uint64_t timecnt;
		uint64_t typecnt;
		uint64_t charcnt;
	} bit64;

	int32_t *trans;
	unsigned char *trans_idx;

	ttinfo  *type;
	char    *timezone_abbr;

	tlinfo  *leap_times;
	unsigned char bc;
	tlocinfo location;
} timelib_tzinfo;

typedef struct timelib_special {
	unsigned int type;
	timelib_sll amount;
} timelib_special;

typedef struct timelib_rel_time {
	timelib_sll y, m, d; /* Years, Months and Days */
	timelib_sll h, i, s; /* Hours, mInutes and Seconds */

	int weekday; /* Stores the day in 'next monday' */
	int weekday_behavior; /* 0: the current day should *not* be counted when advancing forwards; 1: the current day *should* be counted */

	int first_last_day_of;
	int invert; /* Whether the difference should be inverted */
	timelib_sll days; /* Contains the number of *days*, instead of Y-M-D differences */

	timelib_special  special;
	unsigned int   have_weekday_relative, have_special_relative;
} timelib_rel_time;

typedef struct timelib_time_offset {
	int32_t      offset;
	unsigned int leap_secs;
	unsigned int is_dst;
	char        *abbr;
	timelib_sll  transistion_time;
} timelib_time_offset;

typedef struct timelib_time {
	timelib_sll      y, m, d;     /* Year, Month, Day */
	timelib_sll      h, i, s;     /* Hour, mInute, Second */
	double           f;           /* Fraction */
	int              z;           /* GMT offset in minutes */
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

typedef struct timelib_abbr_info {
	timelib_sll  utc_offset;
	char        *abbr;
	int          dst;
} timelib_abbr_info;

typedef struct timelib_error_message {
	int         position;
	char        character;
	char       *message;
} timelib_error_message;

typedef struct timelib_error_container {
	struct timelib_error_message *error_messages;
	struct timelib_error_message *warning_messages;
	int                           error_count;
	int                           warning_count;
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

#define TIMELIB_ZONETYPE_OFFSET 1
#define TIMELIB_ZONETYPE_ABBR   2
#define TIMELIB_ZONETYPE_ID     3

#define SECS_PER_ERA   TIMELIB_LL_CONST(12622780800)
#define SECS_PER_DAY   86400
#define DAYS_PER_YEAR    365
#define DAYS_PER_LYEAR   366
/* 400*365 days + 97 leap days */
#define DAYS_PER_LYEAR_PERIOD 146097
#define YEARS_PER_LYEAR_PERIOD 400

#define timelib_is_leap(y) ((y) % 4 == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

#define TIMELIB_DEBUG(s)  if (0) { s }

#endif
