
/* $selId: dow.c,v 2.0 1995/10/24 01:13:06 lees Exp $
 * Copyright 1993-1995, Scott E. Lee, all rights reserved.
 * Permission granted to use, copy, modify, distribute and sell so long as
 * the above copyright and this permission statement are retained in all
 * copies.  THERE IS NO WARRANTY - USE AT YOUR OWN RISK.
 */

/**************************************************************************
 *
 * These are the externally visible components of this file:
 *
 *     int
 *     DayOfWeek(
 *         long int sdn);
 *
 * Convert a SDN to a day-of-week number (0 to 6).  Where 0 stands for
 * Sunday, 1 for Monday, etc. and 6 stands for Saturday.
 *
 *     char *DayNameShort[7];
 *
 * Convert a day-of-week number (0 to 6), as returned from DayOfWeek(), to
 * the abbreviated (three character) name of the day.
 *
 *     char *DayNameLong[7];
 *
 * Convert a day-of-week number (0 to 6), as returned from DayOfWeek(), to
 * the name of the day.
 *
 **************************************************************************/

#include "sdncal.h"

int DayOfWeek(
				 zend_long sdn)
{
	int dow;

	dow = (sdn + 1) % 7;
	if (dow >= 0) {
		return (dow);
	} else {
		return (dow + 7);
	}
}

char *DayNameShort[7] =
{
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

char *DayNameLong[7] =
{
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
