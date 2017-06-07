/* $selId: gregor.c,v 2.0 1995/10/24 01:13:06 lees Exp $
 * Copyright 1993-1995, Scott E. Lee, all rights reserved.
 * Permission granted to use, copy, modify, distribute and sell so long as
 * the above copyright and this permission statement are retained in all
 * copies.  THERE IS NO WARRANTY - USE AT YOUR OWN RISK.
 */

/**************************************************************************
 *
 * These are the externally visible components of this file:
 *
 *     void
 *     SdnToGregorian(
 *         long int  sdn,
 *         int      *pYear,
 *         int      *pMonth,
 *         int      *pDay);
 *
 * Convert a SDN to a Gregorian calendar date.  If the input SDN is less
 * than 1, the three output values will all be set to zero, otherwise
 * *pYear will be >= -4714 and != 0; *pMonth will be in the range 1 to 12
 * inclusive; *pDay will be in the range 1 to 31 inclusive.
 *
 *     long int
 *     GregorianToSdn(
 *         int inputYear,
 *         int inputMonth,
 *         int inputDay);
 *
 * Convert a Gregorian calendar date to a SDN.  Zero is returned when the
 * input date is detected as invalid or out of the supported range.  The
 * return value will be > 0 for all valid, supported dates, but there are
 * some invalid dates that will return a positive value.  To verify that a
 * date is valid, convert it to SDN and then back and compare with the
 * original.
 *
 *     char *MonthNameShort[13];
 *
 * Convert a Gregorian month number (1 to 12) to the abbreviated (three
 * character) name of the Gregorian month (null terminated).  An index of
 * zero will return a zero length string.
 *
 *     char *MonthNameLong[13];
 *
 * Convert a Gregorian month number (1 to 12) to the name of the Gregorian
 * month (null terminated).  An index of zero will return a zero length
 * string.
 *
 * VALID RANGE
 *
 *     4714 B.C. to at least 10000 A.D.
 *
 *     Although this software can handle dates all the way back to 4714
 *     B.C., such use may not be meaningful.  The Gregorian calendar was
 *     not instituted until October 15, 1582 (or October 5, 1582 in the
 *     Julian calendar).  Some countries did not accept it until much
 *     later.  For example, Britain converted in 1752, The USSR in 1918 and
 *     Greece in 1923.  Most European countries used the Julian calendar
 *     prior to the Gregorian.
 *
 * CALENDAR OVERVIEW
 *
 *     The Gregorian calendar is a modified version of the Julian calendar.
 *     The only difference being the specification of leap years.  The
 *     Julian calendar specifies that every year that is a multiple of 4
 *     will be a leap year.  This leads to a year that is 365.25 days long,
 *     but the current accepted value for the tropical year is 365.242199
 *     days.
 *
 *     To correct this error in the length of the year and to bring the
 *     vernal equinox back to March 21, Pope Gregory XIII issued a papal
 *     bull declaring that Thursday October 4, 1582 would be followed by
 *     Friday October 15, 1582 and that centennial years would only be a
 *     leap year if they were a multiple of 400.  This shortened the year
 *     by 3 days per 400 years, giving a year of 365.2425 days.
 *
 *     Another recently proposed change in the leap year rule is to make
 *     years that are multiples of 4000 not a leap year, but this has never
 *     been officially accepted and this rule is not implemented in these
 *     algorithms.
 *
 * ALGORITHMS
 *
 *     The calculations are based on three different cycles: a 400 year
 *     cycle of leap years, a 4 year cycle of leap years and a 5 month
 *     cycle of month lengths.
 *
 *     The 5 month cycle is used to account for the varying lengths of
 *     months.  You will notice that the lengths alternate between 30
 *     and 31 days, except for three anomalies: both July and August
 *     have 31 days, both December and January have 31, and February
 *     is less than 30.  Starting with March, the lengths are in a
 *     cycle of 5 months (31, 30, 31, 30, 31):
 *
 *         Mar   31 days  \
 *         Apr   30 days   |
 *         May   31 days    > First cycle
 *         Jun   30 days   |
 *         Jul   31 days  /
 *
 *         Aug   31 days  \
 *         Sep   30 days   |
 *         Oct   31 days    > Second cycle
 *         Nov   30 days   |
 *         Dec   31 days  /
 *
 *         Jan   31 days  \
 *         Feb 28/9 days   |
 *                          > Third cycle (incomplete)
 *
 *     For this reason the calculations (internally) assume that the
 *     year starts with March 1.
 *
 * TESTING
 *
 *     This algorithm has been tested from the year 4714 B.C. to 10000
 *     A.D.  The source code of the verification program is included in
 *     this package.
 *
 * REFERENCES
 *
 *     Conversions Between Calendar Date and Julian Day Number by Robert J.
 *     Tantzen, Communications of the Association for Computing Machinery
 *     August 1963.  (Also published in Collected Algorithms from CACM,
 *     algorithm number 199).
 *
 **************************************************************************/

#include "sdncal.h"
#include <limits.h>

#define GREGOR_SDN_OFFSET         32045
#define DAYS_PER_5_MONTHS  153
#define DAYS_PER_4_YEARS   1461
#define DAYS_PER_400_YEARS 146097

void SdnToGregorian(
					   zend_long sdn,
					   int *pYear,
					   int *pMonth,
					   int *pDay)
{
	int century;
	int year;
	int month;
	int day;
	zend_long temp;
	int dayOfYear;

	if (sdn <= 0 ||
			sdn > (LONG_MAX - 4 * GREGOR_SDN_OFFSET) / 4) {
		goto fail;
	}
	temp = (sdn + GREGOR_SDN_OFFSET) * 4 - 1;

	/* Calculate the century (year/100). */
	century = temp / DAYS_PER_400_YEARS;

	/* Calculate the year and day of year (1 <= dayOfYear <= 366). */
	temp = ((temp % DAYS_PER_400_YEARS) / 4) * 4 + 3;
	year = (century * 100) + (temp / DAYS_PER_4_YEARS);
	dayOfYear = (temp % DAYS_PER_4_YEARS) / 4 + 1;

	/* Calculate the month and day of month. */
	temp = dayOfYear * 5 - 3;
	month = temp / DAYS_PER_5_MONTHS;
	day = (temp % DAYS_PER_5_MONTHS) / 5 + 1;

	/* Convert to the normal beginning of the year. */
	if (month < 10) {
		month += 3;
	} else {
		year += 1;
		month -= 9;
	}

	/* Adjust to the B.C./A.D. type numbering. */
	year -= 4800;
	if (year <= 0)
		year--;

	*pYear = year;
	*pMonth = month;
	*pDay = day;
	return;

fail:
	*pYear = 0;
	*pMonth = 0;
	*pDay = 0;
}

zend_long GregorianToSdn(
						   int inputYear,
						   int inputMonth,
						   int inputDay)
{
	zend_long year;
	int month;

	/* check for invalid dates */
	if (inputYear == 0 || inputYear < -4714 ||
		inputMonth <= 0 || inputMonth > 12 ||
		inputDay <= 0 || inputDay > 31) {
		return (0);
	}
	/* check for dates before SDN 1 (Nov 25, 4714 B.C.) */
	if (inputYear == -4714) {
		if (inputMonth < 11) {
			return (0);
		}
		if (inputMonth == 11 && inputDay < 25) {
			return (0);
		}
	}
	/* Make year always a positive number. */
	if (inputYear < 0) {
		year = inputYear + 4801;
	} else {
		year = inputYear + 4800;
	}

	/* Adjust the start of the year. */
	if (inputMonth > 2) {
		month = inputMonth - 3;
	} else {
		month = inputMonth + 9;
		year--;
	}

	return (((year / 100) * DAYS_PER_400_YEARS) / 4
			+ ((year % 100) * DAYS_PER_4_YEARS) / 4
			+ (month * DAYS_PER_5_MONTHS + 2) / 5
			+ inputDay
			- GREGOR_SDN_OFFSET);
}

char *MonthNameShort[13] =
{
	"",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

char *MonthNameLong[13] =
{
	"",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};
