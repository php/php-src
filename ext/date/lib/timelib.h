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

#ifndef __TIMELIB_H__
#define __TIMELIB_H__

#include "timelib_structs.h"
#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef timelib_malloc
# define timelib_malloc  malloc
# define timelib_realloc realloc
# define timelib_calloc  calloc
# define timelib_strdup  strdup
# define timelib_free    free
#endif

#define TIMELIB_VERSION 201502
#define TIMELIB_ASCII_VERSION "2015.02"

#define TIMELIB_NONE             0x00
#define TIMELIB_OVERRIDE_TIME    0x01
#define TIMELIB_NO_CLONE         0x02

#define TIMELIB_UNSET   -99999

#define TIMELIB_SPECIAL_WEEKDAY                   0x01
#define TIMELIB_SPECIAL_DAY_OF_WEEK_IN_MONTH      0x02
#define TIMELIB_SPECIAL_LAST_DAY_OF_WEEK_IN_MONTH 0x03

#define TIMELIB_SPECIAL_FIRST_DAY_OF_MONTH        0x01
#define TIMELIB_SPECIAL_LAST_DAY_OF_MONTH         0x02

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#if defined(_MSC_VER) && !defined(strcasecmp)
#define strcasecmp stricmp
#endif

#if defined(_MSC_VER) && !defined(strncasecmp)
#define strncasecmp strnicmp
#endif

/* Function pointers */
typedef timelib_tzinfo* (*timelib_tz_get_wrapper)(char *tzname, const timelib_tzdb *tzdb);

/* From dow.c */
timelib_sll timelib_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d);
timelib_sll timelib_iso_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d);
timelib_sll timelib_day_of_year(timelib_sll y, timelib_sll m, timelib_sll d);
timelib_sll timelib_daynr_from_weeknr(timelib_sll y, timelib_sll w, timelib_sll d);
timelib_sll timelib_days_in_month(timelib_sll y, timelib_sll m);
void timelib_isoweek_from_date(timelib_sll y, timelib_sll m, timelib_sll d, timelib_sll *iw, timelib_sll *iy);
int timelib_valid_time(timelib_sll h, timelib_sll i, timelib_sll s);
int timelib_valid_date(timelib_sll y, timelib_sll m, timelib_sll d);

/* From parse_date.re */
timelib_time *timelib_strtotime(char *s, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper);
timelib_time *timelib_parse_from_format(char *format, char *s, size_t len, timelib_error_container **errors, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_get_wrapper);
void timelib_fill_holes(timelib_time *parsed, timelib_time *now, int options);
char *timelib_timezone_id_from_abbr(const char *abbr, timelib_long gmtoffset, int isdst);
const timelib_tz_lookup_table *timelib_timezone_abbreviations_list(void);
timelib_long timelib_parse_tz_cor(char**);

/* From parse_iso_intervals.re */
void timelib_strtointerval(char *s, size_t len,
                           timelib_time **begin, timelib_time **end,
						   timelib_rel_time **period, int *recurrences,
						   struct timelib_error_container **errors);


/* From tm2unixtime.c */
void timelib_update_ts(timelib_time* time, timelib_tzinfo* tzi);
void timelib_do_normalize(timelib_time *base);
void timelib_do_rel_normalize(timelib_time *base, timelib_rel_time *rt);

/* From unixtime2tm.c */
int timelib_apply_localtime(timelib_time *t, unsigned int localtime);
void timelib_unixtime2gmt(timelib_time* tm, timelib_sll ts);
void timelib_unixtime2local(timelib_time *tm, timelib_sll ts);
void timelib_update_from_sse(timelib_time *tm);
void timelib_set_timezone_from_offset(timelib_time *t, timelib_sll utc_offset);
void timelib_set_timezone_from_abbr(timelib_time *t, timelib_abbr_info abbr_info);
void timelib_set_timezone(timelib_time *t, timelib_tzinfo *tz);

/* From parse_tz.c */
int timelib_timezone_id_is_valid(char *timezone, const timelib_tzdb *tzdb);
timelib_tzinfo *timelib_parse_tzfile(char *timezone, const timelib_tzdb *tzdb);
int timelib_timestamp_is_in_dst(timelib_sll ts, timelib_tzinfo *tz);
timelib_time_offset *timelib_get_time_zone_info(timelib_sll ts, timelib_tzinfo *tz);
timelib_sll timelib_get_current_offset(timelib_time *t);
void timelib_dump_tzinfo(timelib_tzinfo *tz);
const timelib_tzdb *timelib_builtin_db(void);
const timelib_tzdb_index_entry *timelib_timezone_builtin_identifiers_list(int *count);
timelib_long timelib_parse_zone(char **ptr, int *dst, timelib_time *t, int *tz_not_found, const timelib_tzdb *tzdb, timelib_tz_get_wrapper tz_wrapper);

/* From timelib.c */
timelib_tzinfo* timelib_tzinfo_ctor(char *name);
void timelib_time_tz_abbr_update(timelib_time* tm, char* tz_abbr);
void timelib_time_tz_name_update(timelib_time* tm, char* tz_name);
void timelib_tzinfo_dtor(timelib_tzinfo *tz);
timelib_tzinfo* timelib_tzinfo_clone(timelib_tzinfo *tz);

timelib_rel_time* timelib_rel_time_ctor(void);
void timelib_rel_time_dtor(timelib_rel_time* t);
timelib_rel_time* timelib_rel_time_clone(timelib_rel_time *tz);

timelib_time* timelib_time_ctor(void);
void timelib_time_set_option(timelib_time* tm, int option, void* option_value);
void timelib_time_dtor(timelib_time* t);
timelib_time* timelib_time_clone(timelib_time* orig);

timelib_time_offset* timelib_time_offset_ctor(void);
void timelib_time_offset_dtor(timelib_time_offset* t);

void timelib_error_container_dtor(timelib_error_container *errors);

timelib_long timelib_date_to_int(timelib_time *d, int *error);
void timelib_dump_date(timelib_time *d, int options);
void timelib_dump_rel_time(timelib_rel_time *d);

void timelib_decimal_hour_to_hms(double h, int *hour, int *min, int *sec);
timelib_long timelib_parse_tz_cor(char **ptr);

/* from astro.c */
double timelib_ts_to_juliandate(timelib_sll ts);
int timelib_astro_rise_set_altitude(timelib_time *time, double lon, double lat, double altit, int upper_limb, double *h_rise, double *h_set, timelib_sll *ts_rise, timelib_sll *ts_set, timelib_sll *ts_transit);

/* from interval.c */
timelib_rel_time *timelib_diff(timelib_time *one, timelib_time *two);
timelib_time *timelib_add(timelib_time *t, timelib_rel_time *interval);
timelib_time *timelib_sub(timelib_time *t, timelib_rel_time *interval);

#endif
