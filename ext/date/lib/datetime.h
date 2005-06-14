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
   | Authors: Derick Rethans <dr@ez.no>                                   |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "timelib_structs.h"

#define TIMELIB_NONE             0x00
#define TIMELIB_OVERRIDE_TIME    0x01

/* From dow.c */
int timelib_day_of_week(int y, int m, int d);
int timelib_daynr_from_weeknr(int y, int w, int d);

/* From parse_date.re */
timelib_time *timelib_strtotime(char *s);
void timelib_fill_holes(timelib_time *parsed, timelib_time *now, int options);

/* From tm2unixtime.c */
void timelib_update_ts(timelib_time* time, timelib_tzinfo* tzi);

/* From unixtime2tm.c */
int timelib_apply_localtime(timelib_time *t, uint localtime);
void timelib_unixtime2gmt(timelib_time* tm, timelib_sll ts);
void timelib_unixtime2local(timelib_time *tm, timelib_sll ts, timelib_tzinfo* tz);
void timelib_set_timezone(timelib_time *t, timelib_tzinfo *tz);

/* From parse_tz.c */
timelib_tzinfo *timelib_parse_tzfile(char *timezone);
int timelib_timestamp_is_in_dst(timelib_sll ts, timelib_tzinfo *tz);
timelib_time_offset *timelib_get_time_zone_info(timelib_sll ts, timelib_tzinfo *tz);

/* From datetime.c */
timelib_tzinfo* timelib_tzinfo_ctor();
void timelib_time_tz_abbr_update(timelib_time* tm, char* tz_abbr);
void timelib_time_tz_name_update(timelib_time* tm, char* tz_name);
void timelib_tzinfo_dtor(timelib_tzinfo *tz);
timelib_tzinfo* timelib_tzinfo_clone(timelib_tzinfo *tz);

timelib_time* timelib_time_ctor();
void timelib_time_dtor(timelib_time* t);

timelib_time_offset* timelib_time_offset_ctor();
void timelib_time_offset_dtor(timelib_time_offset* t);

signed long timelib_date_to_int(timelib_time *d, int *error);
void timelib_dump_date(timelib_time *d, int options);

