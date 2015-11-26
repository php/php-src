/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Derick Rethans
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "timelib.h"

#include <stdio.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#define TIMELIB_SUPPORTS_V2DATA
#include "timezonedb.h"

#if (defined(__APPLE__) || defined(__APPLE_CC__)) && (defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__))
# if defined(__LITTLE_ENDIAN__)
#  undef WORDS_BIGENDIAN
# else
#  if defined(__BIG_ENDIAN__)
#   define WORDS_BIGENDIAN
#  endif
# endif
#endif

#ifdef WORDS_BIGENDIAN
#define timelib_conv_int(l) (l)
#else
#define timelib_conv_int(l) ((l & 0x000000ff) << 24) + ((l & 0x0000ff00) << 8) + ((l & 0x00ff0000) >> 8) + ((l & 0xff000000) >> 24)
#endif

static int read_preamble(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t version;

	/* read ID */
	version = (*tzf)[3] - '0';
	*tzf += 4;

	/* read BC flag */
	tz->bc = (**tzf == '\1');
	*tzf += 1;

	/* read country code */
	memcpy(tz->location.country_code, *tzf, 2);
	tz->location.country_code[2] = '\0';
	*tzf += 2;

	/* skip rest of preamble */
	*tzf += 13;

	return version;
}

static void read_header(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[6];

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->bit32.ttisgmtcnt = timelib_conv_int(buffer[0]);
	tz->bit32.ttisstdcnt = timelib_conv_int(buffer[1]);
	tz->bit32.leapcnt    = timelib_conv_int(buffer[2]);
	tz->bit32.timecnt    = timelib_conv_int(buffer[3]);
	tz->bit32.typecnt    = timelib_conv_int(buffer[4]);
	tz->bit32.charcnt    = timelib_conv_int(buffer[5]);
	*tzf += sizeof(buffer);
}

static void skip_64bit_transistions(const unsigned char **tzf, timelib_tzinfo *tz)
{
	if (tz->bit64.timecnt) {
		*tzf += (sizeof(int64_t) * tz->bit64.timecnt);
		*tzf += (sizeof(unsigned char) * tz->bit64.timecnt);
	}
}

static void read_transistions(const unsigned char **tzf, timelib_tzinfo *tz)
{
	int32_t *buffer = NULL;
	uint32_t i;
	unsigned char *cbuffer = NULL;

	if (tz->bit32.timecnt) {
		buffer = (int32_t*) timelib_malloc(tz->bit32.timecnt * sizeof(int32_t));
		if (!buffer) {
			return;
		}
		memcpy(buffer, *tzf, sizeof(int32_t) * tz->bit32.timecnt);
		*tzf += (sizeof(int32_t) * tz->bit32.timecnt);
		for (i = 0; i < tz->bit32.timecnt; i++) {
			buffer[i] = timelib_conv_int(buffer[i]);
		}

		cbuffer = (unsigned char*) timelib_malloc(tz->bit32.timecnt * sizeof(unsigned char));
		if (!cbuffer) {
			timelib_free(buffer);
			return;
		}
		memcpy(cbuffer, *tzf, sizeof(unsigned char) * tz->bit32.timecnt);
		*tzf += sizeof(unsigned char) * tz->bit32.timecnt;
	}

	tz->trans = buffer;
	tz->trans_idx = cbuffer;
}

static void skip_64bit_types(const unsigned char **tzf, timelib_tzinfo *tz)
{
	*tzf += sizeof(unsigned char) * 6 * tz->bit64.typecnt;
	*tzf += sizeof(char) * tz->bit64.charcnt;
	if (tz->bit64.leapcnt) {
		*tzf += sizeof(int64_t) * tz->bit64.leapcnt * 2;
	}
	if (tz->bit64.ttisstdcnt) {
		*tzf += sizeof(unsigned char) * tz->bit64.ttisstdcnt;
	}
	if (tz->bit64.ttisgmtcnt) {
		*tzf += sizeof(unsigned char) * tz->bit64.ttisgmtcnt;
	}
}

static void read_types(const unsigned char **tzf, timelib_tzinfo *tz)
{
	unsigned char *buffer;
	int32_t *leap_buffer;
	unsigned int i, j;

	buffer = (unsigned char*) timelib_malloc(tz->bit32.typecnt * sizeof(unsigned char) * 6);
	if (!buffer) {
		return;
	}
	memcpy(buffer, *tzf, sizeof(unsigned char) * 6 * tz->bit32.typecnt);
	*tzf += sizeof(unsigned char) * 6 * tz->bit32.typecnt;

	tz->type = (ttinfo*) timelib_malloc(tz->bit32.typecnt * sizeof(struct ttinfo));
	if (!tz->type) {
		timelib_free(buffer);
		return;
	}

	for (i = 0; i < tz->bit32.typecnt; i++) {
		j = i * 6;
		tz->type[i].offset = (buffer[j] * 16777216) + (buffer[j + 1] * 65536) + (buffer[j + 2] * 256) + buffer[j + 3];
		tz->type[i].isdst = buffer[j + 4];
		tz->type[i].abbr_idx = buffer[j + 5];
	}
	timelib_free(buffer);

	tz->timezone_abbr = (char*) timelib_malloc(tz->bit32.charcnt);
	if (!tz->timezone_abbr) {
		return;
	}
	memcpy(tz->timezone_abbr, *tzf, sizeof(char) * tz->bit32.charcnt);
	*tzf += sizeof(char) * tz->bit32.charcnt;

	if (tz->bit32.leapcnt) {
		leap_buffer = (int32_t *) timelib_malloc(tz->bit32.leapcnt * 2 * sizeof(int32_t));
		if (!leap_buffer) {
			return;
		}
		memcpy(leap_buffer, *tzf, sizeof(int32_t) * tz->bit32.leapcnt * 2);
		*tzf += sizeof(int32_t) * tz->bit32.leapcnt * 2;

		tz->leap_times = (tlinfo*) timelib_malloc(tz->bit32.leapcnt * sizeof(tlinfo));
		if (!tz->leap_times) {
			timelib_free(leap_buffer);
			return;
		}
		for (i = 0; i < tz->bit32.leapcnt; i++) {
			tz->leap_times[i].trans = timelib_conv_int(leap_buffer[i * 2]);
			tz->leap_times[i].offset = timelib_conv_int(leap_buffer[i * 2 + 1]);
		}
		timelib_free(leap_buffer);
	}

	if (tz->bit32.ttisstdcnt) {
		buffer = (unsigned char*) timelib_malloc(tz->bit32.ttisstdcnt * sizeof(unsigned char));
		if (!buffer) {
			return;
		}
		memcpy(buffer, *tzf, sizeof(unsigned char) * tz->bit32.ttisstdcnt);
		*tzf += sizeof(unsigned char) * tz->bit32.ttisstdcnt;

		for (i = 0; i < tz->bit32.ttisstdcnt; i++) {
			tz->type[i].isstdcnt = buffer[i];
		}
		timelib_free(buffer);
	}

	if (tz->bit32.ttisgmtcnt) {
		buffer = (unsigned char*) timelib_malloc(tz->bit32.ttisgmtcnt * sizeof(unsigned char));
		if (!buffer) {
			return;
		}
		memcpy(buffer, *tzf, sizeof(unsigned char) * tz->bit32.ttisgmtcnt);
		*tzf += sizeof(unsigned char) * tz->bit32.ttisgmtcnt;

		for (i = 0; i < tz->bit32.ttisgmtcnt; i++) {
			tz->type[i].isgmtcnt = buffer[i];
		}
		timelib_free(buffer);
	}
}

static void skip_posix_string(const unsigned char **tzf, timelib_tzinfo *tz)
{
	int n_count = 0;

	do {
		if (*tzf[0] == '\n') {
			n_count++;
		}
		(*tzf)++;
	} while (n_count < 2);
}

static void read_location(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[3];
	uint32_t comments_len;

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->location.latitude = timelib_conv_int(buffer[0]);
	tz->location.latitude = (tz->location.latitude / 100000) - 90;
	tz->location.longitude = timelib_conv_int(buffer[1]);
	tz->location.longitude = (tz->location.longitude / 100000) - 180;
	comments_len = timelib_conv_int(buffer[2]);
	*tzf += sizeof(buffer);

	tz->location.comments = timelib_malloc(comments_len + 1);
	memcpy(tz->location.comments, *tzf, comments_len);
	tz->location.comments[comments_len] = '\0';
	*tzf += comments_len;
}

void timelib_dump_tzinfo(timelib_tzinfo *tz)
{
	uint32_t i;

	printf("Country Code:      %s\n", tz->location.country_code);
	printf("Geo Location:      %f,%f\n", tz->location.latitude, tz->location.longitude);
	printf("Comments:\n%s\n",          tz->location.comments);
	printf("BC:                %s\n",  tz->bc ? "" : "yes");
	printf("UTC/Local count:   " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit32.ttisgmtcnt);
	printf("Std/Wall count:    " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit32.ttisstdcnt);
	printf("Leap.sec. count:   " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit32.leapcnt);
	printf("Trans. count:      " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit32.timecnt);
	printf("Local types count: " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit32.typecnt);
	printf("Zone Abbr. count:  " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit32.charcnt);

	printf ("%8s (%12s) = %3d [%5ld %1d %3d '%s' (%d,%d)]\n",
		"", "", 0,
		(long int) tz->type[0].offset,
		tz->type[0].isdst,
		tz->type[0].abbr_idx,
		&tz->timezone_abbr[tz->type[0].abbr_idx],
		tz->type[0].isstdcnt,
		tz->type[0].isgmtcnt
		);
	for (i = 0; i < tz->bit32.timecnt; i++) {
		printf ("%08X (%12d) = %3d [%5ld %1d %3d '%s' (%d,%d)]\n",
			tz->trans[i], tz->trans[i], tz->trans_idx[i],
			(long int) tz->type[tz->trans_idx[i]].offset,
			tz->type[tz->trans_idx[i]].isdst,
			tz->type[tz->trans_idx[i]].abbr_idx,
			&tz->timezone_abbr[tz->type[tz->trans_idx[i]].abbr_idx],
			tz->type[tz->trans_idx[i]].isstdcnt,
			tz->type[tz->trans_idx[i]].isgmtcnt
			);
	}
	for (i = 0; i < tz->bit32.leapcnt; i++) {
		printf ("%08X (%12ld) = %d\n",
			tz->leap_times[i].trans,
			(long) tz->leap_times[i].trans,
			tz->leap_times[i].offset);
	}
}

static int seek_to_tz_position(const unsigned char **tzf, char *timezone, const timelib_tzdb *tzdb)
{
	int left = 0, right = tzdb->index_size - 1;
#ifdef HAVE_SETLOCALE
	char *cur_locale = NULL, *tmp;

	tmp = setlocale(LC_CTYPE, NULL);
	if (tmp) {
		cur_locale = timelib_strdup(tmp);
	}
	setlocale(LC_CTYPE, "C");
#endif

	do {
		int mid = ((unsigned)left + right) >> 1;
		int cmp = strcasecmp(timezone, tzdb->index[mid].id);

		if (cmp < 0) {
			right = mid - 1;
		} else if (cmp > 0) {
			left = mid + 1;
		} else { /* (cmp == 0) */
			(*tzf) = &(tzdb->data[tzdb->index[mid].pos]);
#ifdef HAVE_SETLOCALE
			setlocale(LC_CTYPE, cur_locale);
			if (cur_locale) timelib_free(cur_locale);
#endif
			return 1;
		}

	} while (left <= right);

#ifdef HAVE_SETLOCALE
	setlocale(LC_CTYPE, cur_locale);
	if (cur_locale) timelib_free(cur_locale);
#endif
	return 0;
}

const timelib_tzdb *timelib_builtin_db(void)
{
	return &timezonedb_builtin;
}

const timelib_tzdb_index_entry *timelib_timezone_builtin_identifiers_list(int *count)
{
	*count = sizeof(timezonedb_idx_builtin) / sizeof(*timezonedb_idx_builtin);
	return timezonedb_idx_builtin;
}

int timelib_timezone_id_is_valid(char *timezone, const timelib_tzdb *tzdb)
{
	const unsigned char *tzf;
	return (seek_to_tz_position(&tzf, timezone, tzdb));
}

static void skip_64bit_preamble(const unsigned char **tzf, timelib_tzinfo *tz)
{
	*tzf += 20;
}

static void read_64bit_header(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[6];

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->bit64.ttisgmtcnt = timelib_conv_int(buffer[0]);
	tz->bit64.ttisstdcnt = timelib_conv_int(buffer[1]);
	tz->bit64.leapcnt    = timelib_conv_int(buffer[2]);
	tz->bit64.timecnt    = timelib_conv_int(buffer[3]);
	tz->bit64.typecnt    = timelib_conv_int(buffer[4]);
	tz->bit64.charcnt    = timelib_conv_int(buffer[5]);
	*tzf += sizeof(buffer);
}

timelib_tzinfo *timelib_parse_tzfile(char *timezone, const timelib_tzdb *tzdb)
{
	const unsigned char *tzf;
	timelib_tzinfo *tmp;
	int version;

	if (seek_to_tz_position(&tzf, timezone, tzdb)) {
		tmp = timelib_tzinfo_ctor(timezone);

		version = read_preamble(&tzf, tmp);
		read_header(&tzf, tmp);
		read_transistions(&tzf, tmp);
		read_types(&tzf, tmp);
		if (version == 2) {
			skip_64bit_preamble(&tzf, tmp);
			read_64bit_header(&tzf, tmp);
			skip_64bit_transistions(&tzf, tmp);
			skip_64bit_types(&tzf, tmp);
			skip_posix_string(&tzf, tmp);
		}
		read_location(&tzf, tmp);
	} else {
		tmp = NULL;
	}

	return tmp;
}

static ttinfo* fetch_timezone_offset(timelib_tzinfo *tz, timelib_sll ts, timelib_sll *transition_time)
{
	uint32_t i;

	/* If there is no transition time, we pick the first one, if that doesn't
	 * exist we return NULL */
	if (!tz->bit32.timecnt || !tz->trans) {
		*transition_time = 0;
		if (tz->bit32.typecnt == 1) {
			return &(tz->type[0]);
		}
		return NULL;
	}

	/* If the TS is lower than the first transition time, then we scan over
	 * all the transition times to find the first non-DST one, or the first
	 * one in case there are only DST entries. Not sure which smartass came up
	 * with this idea in the first though :) */
	if (ts < tz->trans[0]) {
		uint32_t j;

		*transition_time = 0;
		j = 0;
		while (j < tz->bit32.timecnt && tz->type[tz->trans_idx[j]].isdst) {
			++j;
		}
		if (j == tz->bit32.timecnt) {
			j = 0;
		}
		return &(tz->type[tz->trans_idx[j]]);
	}

	/* In all other cases we loop through the available transtion times to find
	 * the correct entry */
	for (i = 0; i < tz->bit32.timecnt; i++) {
		if (ts < tz->trans[i]) {
			*transition_time = tz->trans[i - 1];
			return &(tz->type[tz->trans_idx[i - 1]]);
		}
	}
	*transition_time = tz->trans[tz->bit32.timecnt - 1];
	return &(tz->type[tz->trans_idx[tz->bit32.timecnt - 1]]);
}

static tlinfo* fetch_leaptime_offset(timelib_tzinfo *tz, timelib_sll ts)
{
	int i;

	if (!tz->bit32.leapcnt || !tz->leap_times) {
		return NULL;
	}

	for (i = tz->bit32.leapcnt - 1; i > 0; i--) {
		if (ts > tz->leap_times[i].trans) {
			return &(tz->leap_times[i]);
		}
	}
	return NULL;
}

int timelib_timestamp_is_in_dst(timelib_sll ts, timelib_tzinfo *tz)
{
	ttinfo *to;
	timelib_sll dummy;

	if ((to = fetch_timezone_offset(tz, ts, &dummy))) {
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
	timelib_sll                transistion_time;

	if ((to = fetch_timezone_offset(tz, ts, &transistion_time))) {
		offset = to->offset;
		abbr = &(tz->timezone_abbr[to->abbr_idx]);
		tmp->is_dst = to->isdst;
		tmp->transistion_time = transistion_time;
	} else {
		offset = 0;
		abbr = tz->timezone_abbr;
		tmp->is_dst = 0;
		tmp->transistion_time = 0;
	}

	if ((tl = fetch_leaptime_offset(tz, ts))) {
		leap_secs = -tl->offset;
	}

	tmp->offset = offset;
	tmp->leap_secs = leap_secs;
	tmp->abbr = abbr ? timelib_strdup(abbr) : timelib_strdup("GMT");

	return tmp;
}

timelib_sll timelib_get_current_offset(timelib_time *t)
{
	timelib_time_offset *gmt_offset;
	timelib_sll retval;

	switch (t->zone_type) {
		case TIMELIB_ZONETYPE_ABBR:
		case TIMELIB_ZONETYPE_OFFSET:
			return (t->z + t->dst) * -60;

		case TIMELIB_ZONETYPE_ID:
			gmt_offset = timelib_get_time_zone_info(t->sse, t->tz_info);
			retval = gmt_offset->offset;
			timelib_time_offset_dtor(gmt_offset);
			return retval;

		default:
			return 0;
	}
}
