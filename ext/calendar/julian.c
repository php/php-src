/* $selId: julian.c,v 2.0 1995/10/24 01:13:06 lees Exp $
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
 *     SdnToJulian(
 *         zend_long  sdn,
 *         int      *pYear,
 *         int      *pMonth,
 *         int      *pDay);
 *
 * Convert a SDN to a Julian calendar date.  If the input SDN is less than
 * 1, the three output values will all be set to zero, otherwise *pYear
 * will be >= -4713 and != 0; *pMonth will be in the range 1 to 12
 * inclusive; *pDay will be in the range 1 to 31 inclusive.
 *
 *     zend_long
 *     JulianToSdn(
 *         int inputYear,
 *         int inputMonth,
 *         int inputDay);
 *
 * Convert a Julian calendar date to a SDN.  Zero is returned when the
 * input date is detected as invalid or out of the supported range.  The
 * return value will be > 0 for all valid, supported dates, but there are
 * some invalid dates that will return a positive value.  To verify that a
 * date is valid, convert it to SDN and then back and compare with the
 * original.
 *
 * VALID RANGE
 *
 *     4713 B.C. to at least 10000 A.D.
 *
 *     Although this software can handle dates all the way back to 4713
 *     B.C., such use may not be meaningful.  The calendar was created in
 *     46 B.C., but the details did not stabilize until at least 8 A.D.,
 *     and perhaps as late at the 4th century.  Also, the beginning of a
 *     year varied from one culture to another - not all accepted January
 *     as the first month.
 *
 * CALENDAR OVERVIEW
 *
 *     Julius Caesar created the calendar in 46 B.C. as a modified form of
 *     the old Roman republican calendar which was based on lunar cycles.
 *     The new Julian calendar set fixed lengths for the months, abandoning
 *     the lunar cycle.  It also specified that there would be exactly 12
 *     months per year and 365.25 days per year with every 4th year being a
 *     leap year.
 *
 *     Note that the current accepted value for the tropical year is
 *     365.242199 days, not 365.25.  This lead to an 11 day shift in the
 *     calendar with respect to the seasons by the 16th century when the
 *     Gregorian calendar was created to replace the Julian calendar.
 *
 *     The difference between the Julian and today's Gregorian calendar is
 *     that the Gregorian does not make centennial years leap years unless
 *     they are a multiple of 400, which leads to a year of 365.2425 days.
 *     In other words, in the Gregorian calendar, 1700, 1800 and 1900 are
 *     not leap years, but 2000 is.  All centennial years are leap years in
 *     the Julian calendar.
 *
 *     The details are unknown, but the lengths of the months were adjusted
 *     until they finally stablized in 8 A.D. with their current lengths:
 *
 *         January          31
 *         February         28/29
 *         March            31
 *         April            30
 *         May              31
 *         June             30
 *         Quintilis/July   31
 *         Sextilis/August  31
 *         September        30
 *         October          31
 *         November         30
 *         December         31
 *
 *     In the early days of the calendar, the days of the month were not
 *     numbered as we do today.  The numbers ran backwards (decreasing) and
 *     were counted from the Ides (15th of the month - which in the old
 *     Roman republican lunar calendar would have been the full moon) or
 *     from the Nonae (9th day before the Ides) or from the beginning of
 *     the next month.
 *
 *     In the early years, the beginning of the year varied, sometimes
 *     based on the ascension of rulers.  It was not always the first of
 *     January.
 *
 *     Also, today's epoch, 1 A.D. or the birth of Jesus Christ, did not
 *     come into use until several centuries later when Christianity became
 *     a dominant religion.
 *
 * ALGORITHMS
 *
 *     The calculations are based on two different cycles: a 4 year cycle
 *     of leap years and a 5 month cycle of month lengths.
 *
 *     The 5 month cycle is used to account for the varying lengths of
 *     months.  You will notice that the lengths alternate between 30 and
 *     31 days, except for three anomalies: both July and August have 31
 *     days, both December and January have 31, and February is less than
 *     30.  Starting with March, the lengths are in a cycle of 5 months
 *     (31, 30, 31, 30, 31):
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
 *     For this reason the calculations (internally) assume that the year
 *     starts with March 1.
 *
 * TESTING
 *
 *     This algorithm has been tested from the year 4713 B.C. to 10000 A.D.
 *     The source code of the verification program is included in this
 *     package.
 *
 * REFERENCES
 *
 *     Conversions Between Calendar Date and Julian Day Number by Robert J.
 *     Tantzen, Communications of the Association for Computing Machinery
 *     August 1963.  (Also published in Collected Algorithms from CACM,
 *     algorithm number 199).  [Note: the published algorithm is for the
 *     Gregorian calendar, but was adjusted to use the Julian calendar's
 *     simpler leap year rule.]
 *
 **************************************************************************/

#include "sdncal.h"
#include <limits.h>

#define JULIAN_SDN_OFFSET         32083
#define DAYS_PER_5_MONTHS  153
#define DAYS_PER_4_YEARS   1461

void SdnToJulian(
					zend_long sdn,
					int *pYear,
					int *pMonth,
					int *pDay)
{
	int year;
	int month;
	int day;
	zend_long temp;
	int dayOfYear;

	if (sdn <= 0) {
		goto fail;
	}
	/* Check for overflow */
	if (sdn > (LONG_MAX - JULIAN_SDN_OFFSET * 4 + 1) / 4 || sdn < LONG_MIN / 4) {
		goto fail;
	}
	temp = sdn * 4 + (JULIAN_SDN_OFFSET * 4 - 1);

	/* Calculate the year and day of year (1 <= dayOfYear <= 366). */
	{
		long yearl = temp / DAYS_PER_4_YEARS;
		if (yearl > INT_MAX || yearl < INT_MIN) {
			goto fail;
		}
		year = (int) yearl;
	}
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

zend_long JulianToSdn(
						int inputYear,
						int inputMonth,
						int inputDay)
{
	int year;
	int month;

	/* check for invalid dates */
	if (inputYear == 0 || inputYear < -4713 ||
		inputMonth <= 0 || inputMonth > 12 ||
		inputDay <= 0 || inputDay > 31) {
		return (0);
	}
	/* check for dates before SDN 1 (Jan 2, 4713 B.C.) */
	if (inputYear == -4713) {
		if (inputMonth == 1 && inputDay == 1) {
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

	return ((year * DAYS_PER_4_YEARS) / 4
			+ (month * DAYS_PER_5_MONTHS + 2) / 5
			+ inputDay
			- JULIAN_SDN_OFFSET);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
