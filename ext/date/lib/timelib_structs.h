/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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

#ifndef __TIMELIB_STRUCTS_H__
#define __TIMELIB_STRUCTS_H__

#include "timelib_config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#ifdef PHP_WIN32
/* TODO: Remove these hacks/defs once we have the int definitions in main/ 
	 rathen than in each 2nd extension and win32/ */
# include "win32/php_stdint.h"
#else
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
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
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
	uint32_t ttisgmtcnt;
	uint32_t ttisstdcnt;
	uint32_t leapcnt;
	uint32_t timecnt;
	uint32_t typecnt;
	uint32_t charcnt;

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
	int                           warning_count;
	struct timelib_error_message *warning_messages;
	int                           error_count;
	struct timelib_error_message *error_messages;
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
