/* $selId: jewish.c,v 2.0 1995/10/24 01:13:06 lees Exp $
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
 *     SdnToJewish(
 *         long int sdn,
 *         int *pYear,
 *         int *pMonth,
 *         int *pDay);
 *
 * Convert a SDN to a Jewish calendar date.  If the input SDN is before the
 * first day of year 1, the three output values will all be set to zero,
 * otherwise *pYear will be > 0; *pMonth will be in the range 1 to 13
 * inclusive; *pDay will be in the range 1 to 30 inclusive.  Note that Adar
 * II is assigned the month number 7 and Elul is always 13.
 *
 *     long int
 *     JewishToSdn(
 *         int year,
 *         int month,
 *         int day);
 *
 * Convert a Jewish calendar date to a SDN.  Zero is returned when the
 * input date is detected as invalid or out of the supported range.  The
 * return value will be > 0 for all valid, supported dates, but there are
 * some invalid dates that will return a positive value.  To verify that a
 * date is valid, convert it to SDN and then back and compare with the
 * original.
 *
 *     char *JewishMonthName[14];
 *
 * Convert a Jewish month number (1 to 13) to the name of the Jewish month
 * (null terminated).  An index of zero will return a zero length string.
 *
 * VALID RANGE
 *
 *     Although this software can handle dates all the way back to the year
 *     1 (3761 B.C.), such use may not be meaningful.
 *
 *     The Jewish calendar has been in use for several thousand years, but
 *     in the early days there was no formula to determine the start of a
 *     month.  A new month was started when the new moon was first
 *     observed.
 *
 *     It is not clear when the current rule based calendar replaced the
 *     observation based calendar.  According to the book "Jewish Calendar
 *     Mystery Dispelled" by George Zinberg, the patriarch Hillel II
 *     published these rules in 358 A.D.  But, according to The
 *     Encyclopedia Judaica, Hillel II may have only published the 19 year
 *     rule for determining the occurrence of leap years.
 *
 *     I have yet to find a specific date when the current set of rules
 *     were known to be in use.
 *
 * CALENDAR OVERVIEW
 *
 *     The Jewish calendar is based on lunar as well as solar cycles.  A
 *     month always starts on or near a new moon and has either 29 or 30
 *     days (a lunar cycle is about 29 1/2 days).  Twelve of these
 *     alternating 29-30 day months gives a year of 354 days, which is
 *     about 11 1/4 days short of a solar year.
 *
 *     Since a month is defined to be a lunar cycle (new moon to new moon),
 *     this 11 1/4 day difference cannot be overcome by adding days to a
 *     month as with the Gregorian calendar, so an entire month is
 *     periodically added to the year, making some years 13 months long.
 *
 *     For astronomical as well as ceremonial reasons, the start of a new
 *     year may be delayed until a day or two after the new moon causing
 *     years to vary in length.  Leap years can be from 383 to 385 days and
 *     common years can be from 353 to 355 days.  These are the months of
 *     the year and their possible lengths:
 *
 *                       COMMON YEAR          LEAP YEAR
 *          1 Tishri    30   30   30         30   30   30
 *          2 Heshvan   29   29   30         29   29   30 (variable)
 *          3 Kislev    29   30   30         29   30   30 (variable)
 *          4 Tevet     29   29   29         29   29   29
 *          5 Shevat    30   30   30         30   30   30
 *          6 Adar I    --   --   --         30   30   30 (optional)
 *          7 Adar (II) 29   29   29         29   29   29
 *          8 Nisan     30   30   30         30   30   30
 *          9 Iyyar     29   29   29         29   29   29
 *         10 Sivan     30   30   30         30   30   30
 *         11 Tammuz    29   29   29         29   29   29
 *         12 Av        30   30   30         30   30   30
 *         13 Elul      29   29   29         29   29   29
 *                     ---  ---  ---        ---  ---  ---
 *                     353  354  355        383  384  385
 *
 *     Note that the month names and other words that appear in this file
 *     have multiple possible spellings in the Roman character set.  I have
 *     chosen to use the spellings found in the Encyclopedia Judaica.
 *
 *     Adar I, the month added for leap years, is sometimes referred to as
 *     the 13th month, but I have chosen to assign it the number 6 to keep
 *     the months in chronological order.  This may not be consistent with
 *     other numbering schemes.
 *
 *     Leap years occur in a fixed pattern of 19 years called the metonic
 *     cycle.  The 3rd, 6th, 8th, 11th, 14th, 17th and 19th years of this
 *     cycle are leap years.  The first metonic cycle starts with Jewish
 *     year 1, or 3761/60 B.C.  This is believed to be the year of
 *     creation.
 *
 *     To construct the calendar for a year, you must first find the length
 *     of the year by determining the first day of the year (Tishri 1, or
 *     Rosh Ha-Shanah) and the first day of the following year.  This
 *     selects one of the six possible month length configurations listed
 *     above.
 *
 *     Finding the first day of the year is the most difficult part.
 *     Finding the date and time of the new moon (or molad) is the first
 *     step.  For this purpose, the lunar cycle is assumed to be 29 days 12
 *     hours and 793 halakim.  A halakim is 1/1080th of an hour or 3 1/3
 *     seconds.  (This assumed value is only about 1/2 second less than the
 *     value used by modern astronomers -- not bad for a number that was
 *     determined so long ago.)  The first molad of year 1 occurred on
 *     Sunday at 11:20:11 P.M.  This would actually be Monday, because the
 *     Jewish day is considered to begin at sunset.
 *
 *     Since sunset varies, the day is assumed to begin at 6:00 P.M.  for
 *     calendar calculation purposes.  So, the first molad was 5 hours 793
 *     halakim after the start of Tishri 1, 0001 (which was Monday
 *     September 7, 4761 B.C. by the Gregorian calendar).  All subsequent
 *     molads can be calculated from this starting point by adding the
 *     length of a lunar cycle.
 *
 *     Once the molad that starts a year is determined the actual start of
 *     the year (Tishri 1) can be determined.  Tishri 1 will be the day of
 *     the molad unless it is delayed by one of the following four rules
 *     (called dehiyyot).  Each rule can delay the start of the year by one
 *     day, and since rule #1 can combine with one of the other rules, it
 *     can be delayed as much as two days.
 *
 *         1.  Tishri 1 must never be Sunday, Wednesday or Friday.  (This
 *             is largely to prevent certain holidays from occurring on the
 *             day before or after the Sabbath.)
 *
 *         2.  If the molad occurs on or after noon, Tishri 1 must be
 *             delayed.
 *
 *         3.  If it is a common (not leap) year and the molad occurs on
 *             Tuesday at or after 3:11:20 A.M., Tishri 1 must be delayed.
 *
 *         4.  If it is the year following a leap year and the molad occurs
 *             on Monday at or after 9:32:43 and 1/3 sec, Tishri 1 must be
 *             delayed.
 *
 * GLOSSARY
 *
 *     dehiyyot         The set of 4 rules that determine when the new year
 *                      starts relative to the molad.
 *
 *     halakim          1/1080th of an hour or 3 1/3 seconds.
 *
 *     lunar cycle      The period of time between mean conjunctions of the
 *                      sun and moon (new moon to new moon).  This is
 *                      assumed to be 29 days 12 hours and 793 halakim for
 *                      calendar purposes.
 *
 *     metonic cycle    A 19 year cycle which determines which years are
 *                      leap years and which are common years.  The 3rd,
 *                      6th, 8th, 11th, 14th, 17th and 19th years of this
 *                      cycle are leap years.
 *
 *     molad            The date and time of the mean conjunction of the
 *                      sun and moon (new moon).  This is the approximate
 *                      beginning of a month.
 *
 *     Rosh Ha-Shanah   The first day of the Jewish year (Tishri 1).
 *
 *     Tishri           The first month of the Jewish year.
 *
 * ALGORITHMS
 *
 *     SERIAL DAY NUMBER TO JEWISH DATE
 *
 *     The simplest approach would be to use the rules stated above to find
 *     the molad of Tishri before and after the given day number.  Then use
 *     the molads to find Tishri 1 of the current and following years.
 *     From this the length of the year can be determined and thus the
 *     length of each month.  But this method is used as a last resort.
 *
 *     The first 59 days of the year are the same regardless of the length
 *     of the year.  As a result, only the day number of the start of the
 *     year is required.
 *
 *     Similarly, the last 6 months do not change from year to year.  And
 *     since it can be determined whether the year is a leap year by simple
 *     division, the lengths of Adar I and II can be easily calculated.  In
 *     fact, all dates after the 3rd month are consistent from year to year
 *     (once it is known whether it is a leap year).
 *
 *     This means that if the given day number falls in the 3rd month or on
 *     the 30th day of the 2nd month the length of the year must be found,
 *     but in no other case.
 *
 *     So, the approach used is to take the given day number and round it
 *     to the closest molad of Tishri (first new moon of the year).  The
 *     rounding is not really to the *closest* molad, but is such that if
 *     the day number is before the middle of the 3rd month the molad at
 *     the start of the year is found, otherwise the molad at the end of
 *     the year is found.
 *
 *     Only if the day number is actually found to be in the ambiguous
 *     period of 29 to 31 days is the other molad calculated.
 *
 *     JEWISH DATE TO SERIAL DAY NUMBER
 *
 *     The year number is used to find which 19 year metonic cycle contains
 *     the date and which year within the cycle (this is a division and
 *     modulus).  This also determines whether it is a leap year.
 *
 *     If the month is 1 or 2, the calculation is simple addition to the
 *     first of the year.
 *
 *     If the month is 8 (Nisan) or greater, the calculation is simple
 *     subtraction from beginning of the following year.
 *
 *     If the month is 4 to 7, it is considered whether it is a leap year
 *     and then simple subtraction from the beginning of the following year
 *     is used.
 *
 *     Only if it is the 3rd month is both the start and end of the year
 *     required.
 *
 * TESTING
 *
 *     This algorithm has been tested in two ways.  First, 510 dates from a
 *     table in "Jewish Calendar Mystery Dispelled" were calculated and
 *     compared to the table.  Second, the calculation algorithm described
 *     in "Jewish Calendar Mystery Dispelled" was coded and used to verify
 *     all dates from the year 1 (3761 B.C.) to the year 13760 (10000
 *     A.D.).
 *
 *     The source code of the verification program is included in this
 *     package.
 *
 * REFERENCES
 *
 *     The Encyclopedia Judaica, the entry for "Calendar"
 *
 *     The Jewish Encyclopedia
 *
 *     Jewish Calendar Mystery Dispelled by George Zinberg, Vantage Press,
 *     1963
 *
 *     The Comprehensive Hebrew Calendar by Arthur Spier, Behrman House
 *
 *     The Book of Calendars [note that this work contains many typos]
 *
 **************************************************************************/

#if defined(PHP_WIN32)
#pragma setlocale("english")
#endif

#include "sdncal.h"

#define HALAKIM_PER_HOUR 1080
#define HALAKIM_PER_DAY 25920
#define HALAKIM_PER_LUNAR_CYCLE ((29 * HALAKIM_PER_DAY) + 13753)
#define HALAKIM_PER_METONIC_CYCLE (HALAKIM_PER_LUNAR_CYCLE * (12 * 19 + 7))

#define JEWISH_SDN_OFFSET 347997
#define JEWISH_SDN_MAX 324542846L /* 12/13/887605, greater value raises interger overflow */
#define NEW_MOON_OF_CREATION 31524

#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

#define NOON (18 * HALAKIM_PER_HOUR)
#define AM3_11_20 ((9 * HALAKIM_PER_HOUR) + 204)
#define AM9_32_43 ((15 * HALAKIM_PER_HOUR) + 589)

int monthsPerYear[19] =
{
12, 12, 13, 12, 12, 13, 12, 13, 12, 12, 13, 12, 12, 13, 12, 12, 13, 12, 13
};

static int yearOffset[19] =
{
	0, 12, 24, 37, 49, 61, 74, 86, 99, 111, 123,
	136, 148, 160, 173, 185, 197, 210, 222
};

/* names for leap (13-month) year */
char *JewishMonthNameLeap[14] =
{
	"", 
	"Tishri",
	"Heshvan",
	"Kislev",
	"Tevet",
	"Shevat",
	"Adar I",
	"Adar II",
	"Nisan",
	"Iyyar",
	"Sivan",
	"Tammuz",
	"Av",
	"Elul"
};

/* names for regular year */
char *JewishMonthName[14] =
{
	"", 
	"Tishri",
	"Heshvan",
	"Kislev",
	"Tevet",
	"Shevat",
	"",
	"Adar",
	"Nisan",
	"Iyyar",
	"Sivan",
	"Tammuz",
	"Av",
	"Elul"
};

/* names for leap (13-month) year */
char *JewishMonthHebNameLeap[14] =
{
	"", 
	"תשרי",
	"חשון",
	"כסלו",
	"טבת",
	"שבט",
	"אדר א'",
	"אדר ב'",
	"ניסן",
	"אייר",
	"סיון",
	"תמוז",
	"אב",
	"אלול"
};

/* names for regular year */
char *JewishMonthHebName[14] =
{
	"", 
	"תשרי",
	"חשון",
	"כסלו",
	"טבת",
	"שבט",
	"",
	"אדר",
	"ניסן",
	"אייר",
	"סיון",
	"תמוז",
	"אב",
	"אלול"
};

/************************************************************************
 * Given the year within the 19 year metonic cycle and the time of a molad
 * (new moon) which starts that year, this routine will calculate what day
 * will be the actual start of the year (Tishri 1 or Rosh Ha-Shanah).  This
 * first day of the year will be the day of the molad unless one of 4 rules
 * (called dehiyyot) delays it.  These 4 rules can delay the start of the
 * year by as much as 2 days.
 */
static zend_long Tishri1(
						   int metonicYear,
						   zend_long moladDay,
						   zend_long moladHalakim)
{
	zend_long tishri1;
	int dow;
	int leapYear;
	int lastWasLeapYear;

	tishri1 = moladDay;
	dow = tishri1 % 7;
	leapYear = metonicYear == 2 || metonicYear == 5 || metonicYear == 7
		|| metonicYear == 10 || metonicYear == 13 || metonicYear == 16
		|| metonicYear == 18;
	lastWasLeapYear = metonicYear == 3 || metonicYear == 6
		|| metonicYear == 8 || metonicYear == 11 || metonicYear == 14
		|| metonicYear == 17 || metonicYear == 0;

	/* Apply rules 2, 3 and 4. */
	if ((moladHalakim >= NOON) ||
		((!leapYear) && dow == TUESDAY && moladHalakim >= AM3_11_20) ||
		(lastWasLeapYear && dow == MONDAY && moladHalakim >= AM9_32_43)) {
		tishri1++;
		dow++;
		if (dow == 7) {
			dow = 0;
		}
	}
	/* Apply rule 1 after the others because it can cause an additional
	 * delay of one day. */
	if (dow == WEDNESDAY || dow == FRIDAY || dow == SUNDAY) {
		tishri1++;
	}
	return (tishri1);
}

/************************************************************************
 * Given a metonic cycle number, calculate the date and time of the molad
 * (new moon) that starts that cycle.  Since the length of a metonic cycle
 * is a constant, this is a simple calculation, except that it requires an
 * intermediate value which is bigger that 32 bits.  Because this
 * intermediate value only needs 36 to 37 bits and the other numbers are
 * constants, the process has been reduced to just a few steps.
 */
static void MoladOfMetonicCycle(
								   int metonicCycle,
								   zend_long *pMoladDay,
								   zend_long *pMoladHalakim)
{
	register zend_ulong r1, r2, d1, d2;

	/* Start with the time of the first molad after creation. */
	r1 = NEW_MOON_OF_CREATION;

	/* Calculate metonicCycle * HALAKIM_PER_METONIC_CYCLE.  The upper 32
	 * bits of the result will be in r2 and the lower 16 bits will be
	 * in r1. */
	r1 += metonicCycle * (HALAKIM_PER_METONIC_CYCLE & 0xFFFF);
	r2 = r1 >> 16;
	r2 += metonicCycle * ((HALAKIM_PER_METONIC_CYCLE >> 16) & 0xFFFF);

	/* Calculate r2r1 / HALAKIM_PER_DAY.  The remainder will be in r1, the
	 * upper 16 bits of the quotient will be in d2 and the lower 16 bits
	 * will be in d1. */
	d2 = r2 / HALAKIM_PER_DAY;
	r2 -= d2 * HALAKIM_PER_DAY;
	r1 = (r2 << 16) | (r1 & 0xFFFF);
	d1 = r1 / HALAKIM_PER_DAY;
	r1 -= d1 * HALAKIM_PER_DAY;

	*pMoladDay = (d2 << 16) | d1;
	*pMoladHalakim = r1;
}

/************************************************************************
 * Given a day number, find the molad of Tishri (the new moon at the start
 * of a year) which is closest to that day number.  It's not really the
 * *closest* molad that we want here.  If the input day is in the first two
 * months, we want the molad at the start of the year.  If the input day is
 * in the fourth to last months, we want the molad at the end of the year.
 * If the input day is in the third month, it doesn't matter which molad is
 * returned, because both will be required.  This type of "rounding" allows
 * us to avoid calculating the length of the year in most cases.
 */
static void FindTishriMolad(
							   zend_long inputDay,
							   int *pMetonicCycle,
							   int *pMetonicYear,
							   zend_long *pMoladDay,
							   zend_long *pMoladHalakim)
{
	zend_long moladDay;
	zend_long moladHalakim;
	int metonicCycle;
	int metonicYear;

	/* Estimate the metonic cycle number.  Note that this may be an under
	 * estimate because there are 6939.6896 days in a metonic cycle not
	 * 6940, but it will never be an over estimate.  The loop below will
	 * correct for any error in this estimate. */
	metonicCycle = (inputDay + 310) / 6940;

	/* Calculate the time of the starting molad for this metonic cycle. */
	MoladOfMetonicCycle(metonicCycle, &moladDay, &moladHalakim);

	/* If the above was an under estimate, increment the cycle number until
	 * the correct one is found.  For modern dates this loop is about 98.6%
	 * likely to not execute, even once, because the above estimate is
	 * really quite close. */
	while (moladDay < inputDay - 6940 + 310) {
		metonicCycle++;
		moladHalakim += HALAKIM_PER_METONIC_CYCLE;
		moladDay += moladHalakim / HALAKIM_PER_DAY;
		moladHalakim = moladHalakim % HALAKIM_PER_DAY;
	}

	/* Find the molad of Tishri closest to this date. */
	for (metonicYear = 0; metonicYear < 18; metonicYear++) {
		if (moladDay > inputDay - 74) {
			break;
		}
		moladHalakim += HALAKIM_PER_LUNAR_CYCLE * monthsPerYear[metonicYear];
		moladDay += moladHalakim / HALAKIM_PER_DAY;
		moladHalakim = moladHalakim % HALAKIM_PER_DAY;
	}

	*pMetonicCycle = metonicCycle;
	*pMetonicYear = metonicYear;
	*pMoladDay = moladDay;
	*pMoladHalakim = moladHalakim;
}

/************************************************************************
 * Given a year, find the number of the first day of that year and the date
 * and time of the starting molad.
 */
static void FindStartOfYear(
							   int year,
							   int *pMetonicCycle,
							   int *pMetonicYear,
							   zend_long *pMoladDay,
							   zend_long *pMoladHalakim,
							   int *pTishri1)
{
	*pMetonicCycle = (year - 1) / 19;
	*pMetonicYear = (year - 1) % 19;
	MoladOfMetonicCycle(*pMetonicCycle, pMoladDay, pMoladHalakim);

	*pMoladHalakim += HALAKIM_PER_LUNAR_CYCLE * yearOffset[*pMetonicYear];
	*pMoladDay += *pMoladHalakim / HALAKIM_PER_DAY;
	*pMoladHalakim = *pMoladHalakim % HALAKIM_PER_DAY;

	*pTishri1 = Tishri1(*pMetonicYear, *pMoladDay, *pMoladHalakim);
}

/************************************************************************
 * Given a serial day number (SDN), find the corresponding year, month and
 * day in the Jewish calendar.  The three output values will always be
 * modified.  If the input SDN is before the first day of year 1, they will
 * all be set to zero, otherwise *pYear will be > 0; *pMonth will be in the
 * range 1 to 13 inclusive; *pDay will be in the range 1 to 30 inclusive.
 */
void SdnToJewish(
					zend_long sdn,
					int *pYear,
					int *pMonth,
					int *pDay)
{
	zend_long inputDay;
	zend_long day;
	zend_long halakim;
	int metonicCycle;
	int metonicYear;
	int tishri1;
	int tishri1After;
	int yearLength;

	if (sdn <= JEWISH_SDN_OFFSET || sdn > JEWISH_SDN_MAX) {
		*pYear = 0;
		*pMonth = 0;
		*pDay = 0;
		return;
	}
	inputDay = sdn - JEWISH_SDN_OFFSET;

	FindTishriMolad(inputDay, &metonicCycle, &metonicYear, &day, &halakim);
	tishri1 = Tishri1(metonicYear, day, halakim);

	if (inputDay >= tishri1) {
		/* It found Tishri 1 at the start of the year. */
		*pYear = metonicCycle * 19 + metonicYear + 1;
		if (inputDay < tishri1 + 59) {
			if (inputDay < tishri1 + 30) {
				*pMonth = 1;
				*pDay = inputDay - tishri1 + 1;
			} else {
				*pMonth = 2;
				*pDay = inputDay - tishri1 - 29;
			}
			return;
		}
		/* We need the length of the year to figure this out, so find
		 * Tishri 1 of the next year. */
		halakim += HALAKIM_PER_LUNAR_CYCLE * monthsPerYear[metonicYear];
		day += halakim / HALAKIM_PER_DAY;
		halakim = halakim % HALAKIM_PER_DAY;
		tishri1After = Tishri1((metonicYear + 1) % 19, day, halakim);
	} else {
		/* It found Tishri 1 at the end of the year. */
		*pYear = metonicCycle * 19 + metonicYear;
		if (inputDay >= tishri1 - 177) {
			/* It is one of the last 6 months of the year. */
			if (inputDay > tishri1 - 30) {
				*pMonth = 13;
				*pDay = inputDay - tishri1 + 30;
			} else if (inputDay > tishri1 - 60) {
				*pMonth = 12;
				*pDay = inputDay - tishri1 + 60;
			} else if (inputDay > tishri1 - 89) {
				*pMonth = 11;
				*pDay = inputDay - tishri1 + 89;
			} else if (inputDay > tishri1 - 119) {
				*pMonth = 10;
				*pDay = inputDay - tishri1 + 119;
			} else if (inputDay > tishri1 - 148) {
				*pMonth = 9;
				*pDay = inputDay - tishri1 + 148;
			} else {
				*pMonth = 8;
				*pDay = inputDay - tishri1 + 178;
			}
			return;
		} else {
			if (monthsPerYear[(*pYear - 1) % 19] == 13) {
				*pMonth = 7;
				*pDay = inputDay - tishri1 + 207;
				if (*pDay > 0)
					return;
				(*pMonth)--;
				(*pDay) += 30;
				if (*pDay > 0)
					return;
				(*pMonth)--;
				(*pDay) += 30;
			} else {
				*pMonth = 7;
				*pDay = inputDay - tishri1 + 207;
				if (*pDay > 0)
					return;
				(*pMonth) -= 2;
				(*pDay) += 30;
			}
			if (*pDay > 0)
				return;
			(*pMonth)--;
			(*pDay) += 29;
			if (*pDay > 0)
				return;

			/* We need the length of the year to figure this out, so find
			 * Tishri 1 of this year. */
			tishri1After = tishri1;
			FindTishriMolad(day - 365,
							&metonicCycle, &metonicYear, &day, &halakim);
			tishri1 = Tishri1(metonicYear, day, halakim);
		}
	}

	yearLength = tishri1After - tishri1;
	day = inputDay - tishri1 - 29;
	if (yearLength == 355 || yearLength == 385) {
		/* Heshvan has 30 days */
		if (day <= 30) {
			*pMonth = 2;
			*pDay = day;
			return;
		}
		day -= 30;
	} else {
		/* Heshvan has 29 days */
		if (day <= 29) {
			*pMonth = 2;
			*pDay = day;
			return;
		}
		day -= 29;
	}

	/* It has to be Kislev. */
	*pMonth = 3;
	*pDay = day;
}

/************************************************************************
 * Given a year, month and day in the Jewish calendar, find the
 * corresponding serial day number (SDN).  Zero is returned when the input
 * date is detected as invalid.  The return value will be > 0 for all valid
 * dates, but there are some invalid dates that will return a positive
 * value.  To verify that a date is valid, convert it to SDN and then back
 * and compare with the original.
 */
zend_long JewishToSdn(
						int year,
						int month,
						int day)
{
	zend_long sdn;
	int metonicCycle;
	int metonicYear;
	int tishri1;
	int tishri1After;
	zend_long moladDay;
	zend_long moladHalakim;
	int yearLength;
	int lengthOfAdarIAndII;

	if (year <= 0 || day <= 0 || day > 30) {
		return (0);
	}
	switch (month) {
		case 1:
		case 2:
			/* It is Tishri or Heshvan - don't need the year length. */
			FindStartOfYear(year, &metonicCycle, &metonicYear,
							&moladDay, &moladHalakim, &tishri1);
			if (month == 1) {
				sdn = tishri1 + day - 1;
			} else {
				sdn = tishri1 + day + 29;
			}
			break;

		case 3:
			/* It is Kislev - must find the year length. */

			/* Find the start of the year. */
			FindStartOfYear(year, &metonicCycle, &metonicYear,
							&moladDay, &moladHalakim, &tishri1);

			/* Find the end of the year. */
			moladHalakim += HALAKIM_PER_LUNAR_CYCLE * monthsPerYear[metonicYear];
			moladDay += moladHalakim / HALAKIM_PER_DAY;
			moladHalakim = moladHalakim % HALAKIM_PER_DAY;
			tishri1After = Tishri1((metonicYear + 1) % 19, moladDay, moladHalakim);

			yearLength = tishri1After - tishri1;

			if (yearLength == 355 || yearLength == 385) {
				sdn = tishri1 + day + 59;
			} else {
				sdn = tishri1 + day + 58;
			}
			break;

		case 4:
		case 5:
		case 6:
			/* It is Tevet, Shevat or Adar I - don't need the year length. */

			FindStartOfYear(year + 1, &metonicCycle, &metonicYear,
							&moladDay, &moladHalakim, &tishri1After);

			if (monthsPerYear[(year - 1) % 19] == 12) {
				lengthOfAdarIAndII = 29;
			} else {
				lengthOfAdarIAndII = 59;
			}

			if (month == 4) {
				sdn = tishri1After + day - lengthOfAdarIAndII - 237;
			} else if (month == 5) {
				sdn = tishri1After + day - lengthOfAdarIAndII - 208;
			} else {
				sdn = tishri1After + day - lengthOfAdarIAndII - 178;
			}
			break;

		default:
			/* It is Adar II or later - don't need the year length. */
			FindStartOfYear(year + 1, &metonicCycle, &metonicYear,
							&moladDay, &moladHalakim, &tishri1After);

			switch (month) {
				case 7:
					sdn = tishri1After + day - 207;
					break;
				case 8:
					sdn = tishri1After + day - 178;
					break;
				case 9:
					sdn = tishri1After + day - 148;
					break;
				case 10:
					sdn = tishri1After + day - 119;
					break;
				case 11:
					sdn = tishri1After + day - 89;
					break;
				case 12:
					sdn = tishri1After + day - 60;
					break;
				case 13:
					sdn = tishri1After + day - 30;
					break;
				default:
					return (0);
			}
	}
	return (sdn + JEWISH_SDN_OFFSET);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
