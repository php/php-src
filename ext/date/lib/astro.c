/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Algorithms are taken from a public domain source by Paul             |
   | Schlyter, who wrote this in December 1992                            |
   +----------------------------------------------------------------------+
   | Authors: Derick Rethans <derick@derickrethans.nl>                    |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include <math.h>
#include "timelib.h"

#define days_since_2000_Jan_0(y,m,d) \
	(367L*(y)-((7*((y)+(((m)+9)/12)))/4)+((275*(m))/9)+(d)-730530L)

#ifndef PI
 #define PI        3.1415926535897932384
#endif

#define RADEG     ( 180.0 / PI )
#define DEGRAD    ( PI / 180.0 )

/* The trigonometric functions in degrees */

#define sind(x)  sin((x)*DEGRAD)
#define cosd(x)  cos((x)*DEGRAD)
#define tand(x)  tan((x)*DEGRAD)

#define atand(x)    (RADEG*atan(x))
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atan2d(y,x) (RADEG*atan2(y,x))


/* Following are some macros around the "workhorse" function __daylen__ */
/* They mainly fill in the desired values for the reference altitude    */
/* below the horizon, and also selects whether this altitude should     */
/* refer to the Sun's center or its upper limb.                         */


#include "astro.h"

/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and < 360.0                                   */
/******************************************************************/

#define INV360    (1.0 / 360.0)

/*****************************************/
/* Reduce angle to within 0..360 degrees */
/*****************************************/
static double astro_revolution(double x)
{
	return (x - 360.0 * floor(x * INV360));
}

/*********************************************/
/* Reduce angle to within +180..+180 degrees */
/*********************************************/
static double astro_rev180( double x )
{
	return (x - 360.0 * floor(x * INV360 + 0.5));
}

/*******************************************************************/
/* This function computes GMST0, the Greenwich Mean Sidereal Time  */
/* at 0h UT (i.e. the sidereal time at the Greenwhich meridian at  */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generalized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/

static double astro_GMST0(double d)
{
	double sidtim0;
	/* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
	/* L = M + w, as defined in sunpos().  Since I'm too lazy to */
	/* add these numbers, I'll let the C compiler do it for me.  */
	/* Any decent C compiler will add the constants at compile   */
	/* time, imposing no runtime or code overhead.               */
	sidtim0 = astro_revolution((180.0 + 356.0470 + 282.9404) + (0.9856002585 + 4.70935E-5) * d);
	return sidtim0;
} 

/* This function computes the Sun's position at any instant */

/******************************************************/
/* Computes the Sun's ecliptic longitude and distance */
/* at an instant given in d, number of days since     */
/* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
/* computed, since it's always very near 0.           */
/******************************************************/
static void astro_sunpos(double d, double *lon, double *r)
{
	double M,         /* Mean anomaly of the Sun */
	       w,         /* Mean longitude of perihelion */
	                  /* Note: Sun's mean longitude = M + w */
	       e,         /* Eccentricity of Earth's orbit */
	       E,         /* Eccentric anomaly */
	       x, y,      /* x, y coordinates in orbit */
	       v;         /* True anomaly */

	/* Compute mean elements */
	M = astro_revolution(356.0470 + 0.9856002585 * d);
	w = 282.9404 + 4.70935E-5 * d;
	e = 0.016709 - 1.151E-9 * d;

	/* Compute true longitude and radius vector */
	E = M + e * RADEG * sind(M) * (1.0 + e * cosd(M));
	x = cosd(E) - e;
	y = sqrt(1.0 - e*e) * sind(E);
	*r = sqrt(x*x + y*y);              /* Solar distance */
	v = atan2d(y, x);                  /* True anomaly */
	*lon = v + w;                        /* True solar longitude */
	if (*lon >= 360.0) {
		*lon -= 360.0;                   /* Make it 0..360 degrees */
	}
}

static void astro_sun_RA_dec(double d, double *RA, double *dec, double *r)
{
	double lon, obl_ecl, x, y, z;

	/* Compute Sun's ecliptical coordinates */
	astro_sunpos(d, &lon, r);

	/* Compute ecliptic rectangular coordinates (z=0) */
	x = *r * cosd(lon);
	y = *r * sind(lon);

	/* Compute obliquity of ecliptic (inclination of Earth's axis) */
	obl_ecl = 23.4393 - 3.563E-7 * d;

	/* Convert to equatorial rectangular coordinates - x is unchanged */
	z = y * sind(obl_ecl);
	y = y * cosd(obl_ecl);

	/* Convert to spherical coordinates */
	*RA = atan2d(y, x);
	*dec = atan2d(z, sqrt(x*x + y*y));
}

/**
 * Note: timestamp = unixtimestamp (NEEDS to be 00:00:00 UT)
 *       Eastern longitude positive, Western longitude negative       
 *       Northern latitude positive, Southern latitude negative       
 *       The longitude value IS critical in this function!            
 *       altit = the altitude which the Sun should cross              
 *               Set to -35/60 degrees for rise/set, -6 degrees       
 *               for civil, -12 degrees for nautical and -18          
 *               degrees for astronomical twilight.                   
 *         upper_limb: non-zero -> upper limb, zero -> center         
 *               Set to non-zero (e.g. 1) when computing rise/set     
 *               times, and to zero when computing start/end of       
 *               twilight.                                            
 *        *rise = where to store the rise time                        
 *        *set  = where to store the set  time                        
 *                Both times are relative to the specified altitude,  
 *                and thus this function can be used to compute       
 *                various twilight times, as well as rise/set times   
 * Return value:  0 = sun rises/sets this day, times stored at        
 *                    *trise and *tset.                               
 *               +1 = sun above the specified "horizon" 24 hours.     
 *                    *trise set to time when the sun is at south,    
 *                    minus 12 hours while *tset is set to the south  
 *                    time plus 12 hours. "Day" length = 24 hours     
 *               -1 = sun is below the specified "horizon" 24 hours   
 *                    "Day" length = 0 hours, *trise and *tset are    
 *                    both set to the time when the sun is at south.  
 *                                                                    
 */
int timelib_astro_rise_set_altitude(timelib_time *t_loc, double lon, double lat, double altit, int upper_limb, double *h_rise, double *h_set, timelib_sll *ts_rise, timelib_sll *ts_set, timelib_sll *ts_transit)
{
	double  d,  /* Days since 2000 Jan 0.0 (negative before) */
	sr,         /* Solar distance, astronomical units */
	sRA,        /* Sun's Right Ascension */
	sdec,       /* Sun's declination */
	sradius,    /* Sun's apparent radius */
	t,          /* Diurnal arc */
	tsouth,     /* Time when Sun is at south */
	sidtime;    /* Local sidereal time */
	timelib_time *t_utc;
	timelib_sll   timestamp, old_sse;

	int rc = 0; /* Return cde from function - usually 0 */

	/* Normalize time */
	old_sse = t_loc->sse;
	t_loc->h = 12;
	t_loc->i = t_loc->s = 0;
	timelib_update_ts(t_loc, NULL);

	/* Calculate TS belonging to UTC 00:00 of the current day */
	t_utc = timelib_time_ctor();
	t_utc->y = t_loc->y;
	t_utc->m = t_loc->m;
	t_utc->d = t_loc->d;
	t_utc->h = t_utc->i = t_utc->s = 0;
	timelib_update_ts(t_utc, NULL);

	/* Compute d of 12h local mean solar time */
	timestamp = t_loc->sse;
	d = timelib_ts_to_juliandate(timestamp) - lon/360.0;

	/* Compute local sidereal time of this moment */
	sidtime = astro_revolution(astro_GMST0(d) + 180.0 + lon);

	/* Compute Sun's RA + Decl at this moment */
	astro_sun_RA_dec( d, &sRA, &sdec, &sr );

	/* Compute time when Sun is at south - in hours UT */
	tsouth = 12.0 - astro_rev180(sidtime - sRA) / 15.0;

	/* Compute the Sun's apparent radius, degrees */
	sradius = 0.2666 / sr;

	/* Do correction to upper limb, if necessary */
	if (upper_limb) {
		altit -= sradius;
	}

	/* Compute the diurnal arc that the Sun traverses to reach */
	/* the specified altitude altit: */
	{
		double cost;
		cost = (sind(altit) - sind(lat) * sind(sdec)) / (cosd(lat) * cosd(sdec));
		*ts_transit = t_utc->sse + (tsouth * 3600);
		if (cost >= 1.0) {
			rc = -1;
			t = 0.0;       /* Sun always below altit */

			*ts_rise = *ts_set = t_utc->sse + (tsouth * 3600);
		} else if (cost <= -1.0) {
			rc = +1;
			t = 12.0;      /* Sun always above altit */

			*ts_rise = t_loc->sse - (12 * 3600);
			*ts_set  = t_loc->sse + (12 * 3600);
		} else {
			t = acosd(cost) / 15.0;   /* The diurnal arc, hours */

			/* Store rise and set times - as Unix Timestamp */
			*ts_rise = ((tsouth - t) * 3600) + t_utc->sse;
			*ts_set  = ((tsouth + t) * 3600) + t_utc->sse;

			*h_rise = (tsouth - t);
			*h_set  = (tsouth + t);
		}
	}

	/* Kill temporary time and restore original sse */
	timelib_time_dtor(t_utc);
	t_loc->sse = old_sse;

	return rc;
}

double timelib_ts_to_juliandate(timelib_sll ts)
{
	double tmp;

	tmp = ts;
	tmp /= 86400;
	tmp += 2440587.5;
	tmp -= 2451543;

	return tmp;
}
