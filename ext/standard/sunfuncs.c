/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moshe Doron <mosdoron@netvision.net.il>                      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
	The sun position algorithm taken from the 'US Naval Observatory's
	Almanac for Computers', implemented by Ken Bloom <kekabloom@ucdavis.edu>
	for the zmanim project <http://sourceforge.net/projects/zmanim/>
	and finally converted to C by Moshe Doron <mosdoron@netvision.net.il>.
*/

#include "php.h"
#include "php_sunfuncs.h"
#include "datetime.h"
#include "php_ini.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

/* {{{ macros and constants
 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define to_rad(degrees) (degrees * M_PI / 180)
#define to_rad_with_min(degrees) (degrees + minutes / 60)
#define to_deg(rad) (rad * 180 / M_PI)
/* }}} */

/* {{{ php_sunrise_sunset
   returns time in UTC */
static double php_sunrise_sunset(long N, double latitude, double longitude, double zenith, int calc_sunset)
{
	double lngHour, t, M, L, Lx, RA, RAx, Lquadrant, RAquadrant, sinDec, cosDec, cosH, H, T, UT, UTx;

	/* step 1: First calculate the day of the year 
	int N = theday - date(1, 1, theday.year()) + 1;
	*/
	
	/* step 2: convert the longitude to hour value and calculate an approximate time */
	lngHour = longitude / 15;
	
	/* use 18 for sunset instead of 6 */
	if (calc_sunset) {
		t = (double) N + ((18 - lngHour) / 24);		/* Sunset */
	} else {
		t = (double) N + ((6 - lngHour) / 24);		/* Sunrise */
	}
	
	/* step 3: calculate the sun's mean anomaly */
	M = (0.9856 * t) - 3.289;

	/* step 4: calculate the sun's true longitude */
	L = M + (1.916 * sin(to_rad(M))) + (0.020 * sin (to_rad(2 * M))) + 282.634;

	while (L < 0) {
		Lx = L + 360;
		assert (Lx != L); /* askingtheguru: realy needed? */
		L = Lx;
	}

	while (L >= 360) {
		Lx = L - 360;
		assert (Lx != L); /* askingtheguru: realy needed? */
		L = Lx;
	}

	/* step 5a: calculate the sun's right ascension */
	RA = to_deg(atan(0.91764 * tan(to_rad(L))));

	while (RA < 0) {
		RAx = RA + 360;
		assert (RAx != RA);  /* askingtheguru: realy needed? */
		RA = RAx;
	}

	while (RA >= 360) {
		RAx = RA - 360;
		assert (RAx != RA);  /* askingtheguru: realy needed? */
		RA = RAx;
	}
	
	/* step 5b: right ascension value needs to be in the same quadrant as L */
	Lquadrant = floor(L / 90) * 90;
	RAquadrant = floor(RA / 90) * 90;
	RA = RA + (Lquadrant - RAquadrant);

	/* step 5c: right ascension value needs to be converted into hours */
	RA /= 15;

	/* step 6: calculate the sun's declination */
	sinDec = 0.39782 * sin(to_rad(L));
	cosDec = cos(asin(sinDec));

	/* step 7a: calculate the sun's local hour angle */
	cosH = (cos(to_rad(zenith)) - (sinDec * sin(to_rad(latitude)))) / (cosDec * cos(to_rad(latitude)));

	/* XXX: What's the use of this block.. ?
	 * if (!calc_sunset && cosH > 1 || calc_sunset && cosH < -1) {
	 *    throw doesnthappen(); 
	 * }
	 */
		
	/* step 7b: finish calculating H and convert into hours */
	if (calc_sunset) {
		H = to_deg(acos(cosH));			/* Sunset */
	} else {
		H = 360 - to_deg(acos(cosH));	/* Sunrise */
	}
	H = H / 15;

	/* step 8: calculate local mean time */
	T = H + RA - (0.06571 * t) - 6.622;

	/* step 9: convert to UTC */
	UT = T - lngHour;

	while (UT < 0) {
		UTx = UT + 24;
		assert (UTx != UT); /* askingtheguru: realy needed? */
		UT = UTx;
	}

	while (UT >= 24) {
		UTx = UT - 24;
		assert (UTx != UT); /* askingtheguru: realy needed? */
		UT = UTx;
	}
	
	return UT;
}
/* }}} */

/* {{{ php_do_date_sunrise_sunset
 *  Common for date_sunrise() and date_sunset() functions
 */
static void php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAMETERS, int calc_sunset)
{
	zval *date;
	double latitude, longitude, zenith, gmt_offset, ret;
	int time, N, retformat;
	char retstr[6];
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ldddd", &date, &retformat, &latitude, &longitude, &zenith, &gmt_offset) == FAILURE) {
		RETURN_FALSE;
	}
	
	switch (Z_TYPE_P(date)) {
		case IS_LONG:
			time = Z_LVAL_P(date);
			break;
		case IS_STRING:
			/* todo: more user friendly format */
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "date must be timestamp for now");
			RETURN_FALSE;
	}

	N = php_idate('z', time, 0) + 1;
	
	switch (ZEND_NUM_ARGS()) {
		case 1:
			retformat = SUNFUNCS_RET_STRING;
		case 2:
			latitude = INI_FLT("date.default_latitude");
		case 3:
			longitude = INI_FLT("date.default_longitude");
		case 4:
			if (calc_sunset) {
				zenith = INI_FLT("date.sunset_zenith");
			} else {
				zenith = INI_FLT("date.sunrise_zenith");
			}
		case 5:
			gmt_offset = php_idate('Z', time, 0) / 3600;
		default:
			break;
	}
	
	ret = php_sunrise_sunset(N, latitude, longitude, zenith, calc_sunset) + gmt_offset;

	switch (retformat) {
		case SUNFUNCS_RET_TIMESTAMP:
			RETURN_LONG((int) (time - (time % (24 * 3600))) + (int) (60 * ret));
			break;
		case SUNFUNCS_RET_STRING:
			N = (int) ret;
			sprintf(retstr, "%02d:%02d", N, (int) (60 * (ret - (double) N)));
			RETVAL_STRINGL(retstr, 5, 1);
			break;
		case SUNFUNCS_RET_DOUBLE:
			RETURN_DOUBLE(ret);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid format");
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto mixed date_sunrise(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
   Returns time of sunrise for a given day & location */
PHP_FUNCTION(date_sunrise)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto mixed date_sunset(mixed time [, int format [, float latitude [, float longitude [, float zenith [, float gmt_offset]]]]])
   Returns time of sunset for a given day & location */
PHP_FUNCTION(date_sunset)
{
	php_do_date_sunrise_sunset(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
