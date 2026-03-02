/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Simonov Denis <sim-mail@list.ru>                             |
  +----------------------------------------------------------------------+
*/

#ifndef PDO_FIREBIRD_UTILS_H
#define PDO_FIREBIRD_UTILS_H

#include <ibase.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned fb_get_client_version(void);

ISC_TIME fb_encode_time(unsigned hours, unsigned minutes, unsigned seconds, unsigned fractions);

ISC_DATE fb_encode_date(unsigned year, unsigned month, unsigned day);

#if FB_API_VER >= 40

ISC_STATUS fb_decode_time_tz(ISC_STATUS* isc_status, const ISC_TIME_TZ* timeTz, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions,
	unsigned timeZoneBufferLength, char* timeZoneBuffer);

ISC_STATUS fb_decode_timestamp_tz(ISC_STATUS* isc_status, const ISC_TIMESTAMP_TZ* timestampTz,
	unsigned* year, unsigned* month, unsigned* day,
	unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions,
	unsigned timeZoneBufferLength, char* timeZoneBuffer);

#endif

#ifdef __cplusplus
}
#endif

#endif	/* PDO_FIREBIRD_UTILS_H */
