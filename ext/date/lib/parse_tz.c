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

#include <stdio.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <string.h>

#include "datetime.h"
#include "timezonedb.h"

static void read_header(char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[6];

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->ttisgmtcnt = htonl(buffer[0]);
	tz->ttisstdcnt = htonl(buffer[1]);
	tz->leapcnt    = htonl(buffer[2]);
	tz->timecnt    = htonl(buffer[3]);
	tz->typecnt    = htonl(buffer[4]);
	tz->charcnt    = htonl(buffer[5]);
	*tzf += sizeof(buffer);
}

static void read_transistions(char **tzf, timelib_tzinfo *tz)
{
	int32_t *buffer = NULL;
	uint32_t i;
	unsigned char *cbuffer = NULL;

	if (tz->timecnt) {
		buffer = (int32_t*) malloc(tz->timecnt * sizeof(int32_t));
		if (!buffer) {
			return;
		}
		memcpy(buffer, *tzf, sizeof(int32_t) * tz->timecnt);
		*tzf += (sizeof(int32_t) * tz->timecnt);
		for (i = 0; i < tz->timecnt; i++) {
			buffer[i] = htonl(buffer[i]);
		}

		cbuffer = (unsigned char*) malloc(tz->timecnt * sizeof(unsigned char));
		if (!cbuffer) {
			return;
		}
		memcpy(cbuffer, *tzf, sizeof(unsigned char) * tz->timecnt);
		*tzf += sizeof(unsigned char) * tz->timecnt;
	}
	
	tz->trans = buffer;
	tz->trans_idx = cbuffer;
}

static void read_types(char **tzf, timelib_tzinfo *tz)
{
	unsigned char *buffer;
	int32_t *leap_buffer;
	unsigned int i, j;

	buffer = (unsigned char*) malloc(tz->typecnt * sizeof(unsigned char) * 6);
	if (!buffer) {
		return;
	}
	memcpy(buffer, *tzf, sizeof(unsigned char) * 6 * tz->typecnt);
	*tzf += sizeof(unsigned char) * 6 * tz->typecnt;

	tz->type = (ttinfo*) malloc(tz->typecnt * sizeof(struct ttinfo));

	for (i = 0; i < tz->typecnt; i++) {
		j = i * 6;
		tz->type[i].offset = (buffer[j] * 16777216) + (buffer[j + 1] * 65536) + (buffer[j + 2] * 256) + buffer[j + 3];
		tz->type[i].isdst = buffer[j + 4];
		tz->type[i].abbr_idx = buffer[j + 5];
	}
	free(buffer);

	tz->timezone_abbr = (char*) malloc(tz->charcnt);
	if (!tz->timezone_abbr) {
		return;
	}
	memcpy(tz->timezone_abbr, *tzf, sizeof(char) * tz->charcnt);
	*tzf += sizeof(char) * tz->charcnt;

	leap_buffer = (int32_t *) malloc(tz->leapcnt * 2 * sizeof(int32_t));
	if (!leap_buffer) {
		return;
	}
	memcpy(leap_buffer, *tzf, sizeof(int32_t) * tz->leapcnt * 2);
	*tzf += sizeof(int32_t) * tz->leapcnt * 2;

	tz->leap_times = (tlinfo*) malloc(tz->leapcnt * sizeof(tlinfo));
	if (!tz->leap_times) {
		return;
	}
	for (i = 0; i < tz->leapcnt; i++) {
		tz->leap_times[i].trans = htonl(leap_buffer[i * 2]);
		tz->leap_times[i].offset = htonl(leap_buffer[i * 2 + 1]);
	}
	free(leap_buffer);
	
	buffer = (unsigned char*) malloc(tz->ttisstdcnt * sizeof(unsigned char));
	if (!buffer) {
		return;
	}
	memcpy(buffer, *tzf, sizeof(unsigned char) * tz->ttisstdcnt);
	*tzf += sizeof(unsigned char) * tz->ttisstdcnt;

	for (i = 0; i < tz->ttisstdcnt; i++) {
		tz->type[i].isstdcnt = buffer[i];
	}
	free(buffer);

	buffer = (unsigned char*) malloc(tz->ttisgmtcnt * sizeof(unsigned char));
	if (!buffer) {
		return;
	}
	memcpy(buffer, *tzf, sizeof(unsigned char) * tz->ttisgmtcnt);
	*tzf += sizeof(unsigned char) * tz->ttisgmtcnt;

	for (i = 0; i < tz->ttisgmtcnt; i++) {
		tz->type[i].isgmtcnt = buffer[i];
	}
	free(buffer);
}

#if 0
static void dumpinfo(timelib_tzinfo *tz)
{
	uint32_t i;

	printf("UTC/Local count:   %lu\n", (unsigned long) tz->ttisgmtcnt);
	printf("Std/Wall count:    %lu\n", (unsigned long) tz->ttisstdcnt);
	printf("Leap.sec. count:   %lu\n", (unsigned long) tz->leapcnt);
	printf("Trans. count:      %lu\n", (unsigned long) tz->timecnt);
	printf("Local types count: %lu\n", (unsigned long) tz->typecnt);
	printf("Zone Abbr. count:  %lu\n", (unsigned long) tz->charcnt);

	for (i = 0; i < tz->timecnt; i++) {
		printf ("%08X (%12d) = %3d [%5ld %1d %3d '%s' (%d,%d)]\n",
			tz->trans[i], tz->trans[i], tz->trans_idx[i],
			tz->type[tz->trans_idx[i]].offset,
			tz->type[tz->trans_idx[i]].isdst,
			tz->type[tz->trans_idx[i]].abbr_idx,
			&tz->timezone_abbr[tz->type[tz->trans_idx[i]].abbr_idx],
			tz->type[tz->trans_idx[i]].isstdcnt,
			tz->type[tz->trans_idx[i]].isgmtcnt
			);
	}
	for (i = 0; i < tz->leapcnt; i++) {
		printf ("%08X (%12ld) = %d\n",
			tz->leap_times[i].trans,
			(long) tz->leap_times[i].trans,
			tz->leap_times[i].offset);
	}
}
#endif

static int seek_to_tz_position(char **tzf, char *timezone)
{
	int       found;
	tzdb_idx *ptr;

	/* Reset Index Position */
	ptr = timezonedb_idx;

	/* Start scanning the index file for the timezone */
	found = 0;
	do {
		if (strcmp(timezone, ptr->id) == 0) {
			found = 1;
			(*tzf) = &php_timezone_db_index[ptr->pos + 20];
			break;
		}
		ptr++;
	} while (ptr->id != NULL);
	if (!found) {
		return 0;
	}
	return 1;
}

timelib_tzinfo *timelib_parse_tzfile(char *timezone)
{
	char *tzf;
	timelib_tzinfo *tmp;

	if (seek_to_tz_position((char**) &tzf, timezone)) {
		tmp = timelib_tzinfo_ctor(timezone);

		read_header((char**) &tzf, tmp);
		read_transistions((char**) &tzf, tmp);
		read_types((char**) &tzf, tmp);
	} else {
		tmp = NULL;
	}

	return tmp;
}

static ttinfo* fetch_timezone_offset(timelib_tzinfo *tz, timelib_sll ts)
{
	uint32_t i;

	if (!tz->timecnt || !tz->trans) {
		return NULL;
	}

	for (i = 0; i < tz->timecnt; i++) {
		if (ts < tz->trans[i]) {
			return &(tz->type[tz->trans_idx[i - 1]]);
		}
	}
	return NULL;
}

static tlinfo* fetch_leaptime_offset(timelib_tzinfo *tz, timelib_sll ts)
{
	int i;

	if (!tz->leapcnt || !tz->leap_times) {
		return NULL;
	}

	for (i = tz->leapcnt - 1; i > 0; i--) {
		if (ts > tz->leap_times[i].trans) {
			return &(tz->leap_times[i]);
		}
	}
	return NULL;
}

int timelib_timestamp_is_in_dst(timelib_sll ts, timelib_tzinfo *tz)
{
	ttinfo *to;
	if ((to = fetch_timezone_offset(tz, ts))) {
		return to->isdst;
	}
	return -1;
}

timelib_time_offset *timelib_get_time_zone_info(timelib_sll ts, timelib_tzinfo *tz)
{
	ttinfo *to;
	tlinfo *tl;
	int32_t offset = 0, leap_secs = 0;
	char *abbr;
	timelib_time_offset *tmp = timelib_time_offset_ctor();

	if ((to = fetch_timezone_offset(tz, ts))) {
		offset = to->offset;
		abbr = &(tz->timezone_abbr[to->abbr_idx]);
		tmp->is_dst = to->isdst;
	} else {
		abbr = tz->timezone_abbr;
		tmp->is_dst = 0;
	}

	if ((tl = fetch_leaptime_offset(tz, ts))) {
		leap_secs = -tl->offset;
	}

	tmp->offset = offset;
	tmp->leap_secs = leap_secs;
	tmp->abbr = abbr ? strdup(abbr) : strdup("GMT");

	return tmp;
}
