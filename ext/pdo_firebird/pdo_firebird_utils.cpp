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

#include "pdo_firebird_utils.h"
#include <firebird/Interface.h>
#include <cstring>

/* Returns the client version. 0 bytes are minor version, 1 bytes are major version. */
extern "C" unsigned fb_get_client_version(void)
{
	Firebird::IMaster* master = Firebird::fb_get_master_interface();
	Firebird::IUtil* util = master->getUtilInterface();
	return util->getClientVersion();
}

extern "C" ISC_TIME fb_encode_time(unsigned hours, unsigned minutes, unsigned seconds, unsigned fractions)
{
	Firebird::IMaster* master = Firebird::fb_get_master_interface();
	Firebird::IUtil* util = master->getUtilInterface();
	return util->encodeTime(hours, minutes, seconds, fractions);
}

extern "C" ISC_DATE fb_encode_date(unsigned year, unsigned month, unsigned day)
{
	Firebird::IMaster* master = Firebird::fb_get_master_interface();
	Firebird::IUtil* util = master->getUtilInterface();
	return util->encodeDate(year, month, day);
}

#if FB_API_VER >= 40
static void fb_copy_status(const ISC_STATUS* from, ISC_STATUS* to, size_t maxLength)
{
	for(size_t i=0; i < maxLength; ++i) {
		memcpy(to + i, from + i, sizeof(ISC_STATUS));
		if (from[i] == isc_arg_end) {
			break;
		}
	}
}

/* Decodes a time with time zone into its time components. */
extern "C" ISC_STATUS fb_decode_time_tz(ISC_STATUS* isc_status, const ISC_TIME_TZ* timeTz, unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions,
   unsigned timeZoneBufferLength, char* timeZoneBuffer)
{
	Firebird::IMaster* master = Firebird::fb_get_master_interface();
	Firebird::IUtil* util = master->getUtilInterface();
	Firebird::IStatus* status = master->getStatus();
	Firebird::CheckStatusWrapper st(status);
	util->decodeTimeTz(&st, timeTz, hours, minutes, seconds, fractions,
						timeZoneBufferLength, timeZoneBuffer);
	if (st.hasData())  {
		fb_copy_status((const ISC_STATUS*)st.getErrors(), isc_status, 20);
	}
	status->dispose();
	return isc_status[1];
}

/* Decodes a timestamp with time zone into its date and time components */
extern "C" ISC_STATUS fb_decode_timestamp_tz(ISC_STATUS* isc_status, const ISC_TIMESTAMP_TZ* timestampTz,
	unsigned* year, unsigned* month, unsigned* day,
	unsigned* hours, unsigned* minutes, unsigned* seconds, unsigned* fractions,
	unsigned timeZoneBufferLength, char* timeZoneBuffer)
{
	Firebird::IMaster* master = Firebird::fb_get_master_interface();
	Firebird::IUtil* util = master->getUtilInterface();
	Firebird::IStatus* status = master->getStatus();
	Firebird::CheckStatusWrapper st(status);
	util->decodeTimeStampTz(&st, timestampTz, year, month, day,
							hours, minutes, seconds, fractions,
							timeZoneBufferLength, timeZoneBuffer);
	if (st.hasData())  {
		fb_copy_status((const ISC_STATUS*)st.getErrors(), isc_status, 20);
	}
	status->dispose();
	return isc_status[1];
}

#endif
