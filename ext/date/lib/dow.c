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

static int m_table_common[13] = { -1, 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; /* 1 = jan */
static int m_table_leap[13] =   { -1, 6, 2, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; /* 1 = jan */

static timelib_sll century_value(timelib_sll j)
{
	timelib_sll i = j - 17;
	timelib_sll c = (4 - i * 2 + (i + 1) / 4) % 7;

	return c < 0 ? c + 7 : c;
}

timelib_sll timelib_day_of_week(timelib_sll y, timelib_sll m, timelib_sll d)
{
	timelib_sll c1, y1, m1;

	/* Only valid for Gregorian calendar */
	if (y < 1753) {
		return -1;
	}
	c1 = century_value(y / 100);
	y1 = (y % 100);
	m1 = is_leap(y) ? m_table_leap[m] : m_table_common[m];
	return (c1 + y1 + m1 + (y1 / 4) + d) % 7;
}

timelib_sll timelib_daynr_from_weeknr(timelib_sll y, timelib_sll w, timelib_sll d)
{
	timelib_sll dow, day;
	
	/* Figure out the dayofweek for y-1-1 */
	dow = timelib_day_of_week(y, 1, 1);
	/* then use that to figure out the offset for day 1 of week 1 */
	day = 0 - (dow > 4 ? dow - 7 : dow);

	/* Add weeks and days */
	return day + ((w - 1) * 7) + d;
}

#if 0
int main(void)
{
	printf("dow = %d\n", timelib_day_of_week(1978, 12, 22)); /* 5 */
	printf("dow = %d\n", timelib_day_of_week(2005,  2, 19)); /* 6 */
}
#endif
