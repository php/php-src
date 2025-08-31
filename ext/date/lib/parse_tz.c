/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Derick Rethans
 * Copyright (c) 2018 MongoDB, Inc.
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
#include "timelib_private.h"

#define TIMELIB_SUPPORTS_V2DATA
#define TIMELIB_SUPPORT_SLIM_FILE
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

#if (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN))
# if __BYTE_ORDER == __BIG_ENDIAN
#  define WORDS_BIGENDIAN
# endif
#endif

#if defined(__s390__)
# if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define WORDS_BIGENDIAN
# else
#  undef WORDS_BIGENDIAN
# endif
#endif

#ifdef WORDS_BIGENDIAN
static inline uint32_t timelib_conv_int_unsigned(uint32_t value)
{
	return value;
}

static inline uint64_t timelib_conv_int64_unsigned(uint64_t value)
{
	return value;
}
#else
static inline uint32_t timelib_conv_int_unsigned(uint32_t value)
{
	return
		((value & 0x000000ff) << 24) +
		((value & 0x0000ff00) <<  8) +
		((value & 0x00ff0000) >>  8) +
		((value & 0xff000000) >> 24);
}

static inline uint64_t timelib_conv_int64_unsigned(uint64_t value)
{
	return
		((value & 0x00000000000000ff) << 56) +
		((value & 0x000000000000ff00) << 40) +
		((value & 0x0000000000ff0000) << 24) +
		((value & 0x00000000ff000000) <<  8) +
		((value & 0x000000ff00000000) >>  8) +
		((value & 0x0000ff0000000000) >> 24) +
		((value & 0x00ff000000000000) >> 40) +
		((value & 0xff00000000000000) >> 56);
}
#endif

#define timelib_conv_int_signed(value) ((int32_t) timelib_conv_int_unsigned((int32_t) value))
#define timelib_conv_int64_signed(value) ((int64_t) timelib_conv_int64_unsigned((int64_t) value))

static int read_php_preamble(const unsigned char **tzf, timelib_tzinfo *tz)
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

static int read_tzif_preamble(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t version;

	/* read ID */
	switch ((*tzf)[4]) {
		case '\0':
			version = 0;
			break;
		case '2':
			version = 2;
			break;
		case '3':
			version = 3;
			break;
		case '4':
			version = 4;
			break;
		default:
			return -1;
	}
	*tzf += 5;

	/* set BC flag and country code to default */
	tz->bc = 0;
	tz->location.country_code[0] = '?';
	tz->location.country_code[1] = '?';
	tz->location.country_code[2] = '\0';

	/* skip rest of preamble */
	*tzf += 15;

	return version;
}

static int read_preamble(const unsigned char **tzf, timelib_tzinfo *tz, unsigned int *type)
{
	/* read marker (TZif) or (PHP) */
	if (memcmp(*tzf, "PHP", 3) == 0) {
		*type = TIMELIB_TZINFO_PHP;
		return read_php_preamble(tzf, tz);
	} else if (memcmp(*tzf, "TZif", 4) == 0) {
		*type = TIMELIB_TZINFO_ZONEINFO;
		return read_tzif_preamble(tzf, tz);
	} else {
		return -1;
	}
}

static void read_32bit_header(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[6];

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->_bit32.ttisgmtcnt = timelib_conv_int_unsigned(buffer[0]);
	tz->_bit32.ttisstdcnt = timelib_conv_int_unsigned(buffer[1]);
	tz->_bit32.leapcnt    = timelib_conv_int_unsigned(buffer[2]);
	tz->_bit32.timecnt    = timelib_conv_int_unsigned(buffer[3]);
	tz->_bit32.typecnt    = timelib_conv_int_unsigned(buffer[4]);
	tz->_bit32.charcnt    = timelib_conv_int_unsigned(buffer[5]);

	*tzf += sizeof(buffer);
}

static int detect_slim_file(timelib_tzinfo *tz)
{
	if (
		(tz->_bit32.ttisgmtcnt == 0) &&
		(tz->_bit32.ttisstdcnt == 0) &&
		(tz->_bit32.leapcnt    == 0) &&
		(tz->_bit32.timecnt    == 0) &&
		(tz->_bit32.typecnt    == 1) &&
		(tz->_bit32.charcnt    == 1)
	) {
		return 1;
	}

	return 0;
}

static int read_64bit_transitions(const unsigned char **tzf, timelib_tzinfo *tz)
{
	int64_t *buffer = NULL;
	uint32_t i;
	unsigned char *cbuffer = NULL;

	if (tz->bit64.timecnt) {
		buffer = (int64_t*) timelib_malloc(tz->bit64.timecnt * sizeof(int64_t));
		if (!buffer) {
			return TIMELIB_ERROR_CANNOT_ALLOCATE;
		}
		memcpy(buffer, *tzf, sizeof(int64_t) * tz->bit64.timecnt);
		*tzf += (sizeof(int64_t) * tz->bit64.timecnt);
		for (i = 0; i < tz->bit64.timecnt; i++) {
			buffer[i] = timelib_conv_int64_signed(buffer[i]);
			/* Sanity check to see whether TS is just increasing */
			if (i > 0 && !(buffer[i] > buffer[i - 1])) {
				return TIMELIB_ERROR_CORRUPT_TRANSITIONS_DONT_INCREASE;
			}
		}

		cbuffer = (unsigned char*) timelib_malloc(tz->bit64.timecnt * sizeof(unsigned char));
		if (!cbuffer) {
			timelib_free(buffer);
			return TIMELIB_ERROR_CANNOT_ALLOCATE;
		}
		memcpy(cbuffer, *tzf, sizeof(unsigned char) * tz->bit64.timecnt);
		*tzf += sizeof(unsigned char) * tz->bit64.timecnt;
	}

	tz->trans = buffer;
	tz->trans_idx = cbuffer;

	return 0;
}

static void skip_32bit_transitions(const unsigned char **tzf, timelib_tzinfo *tz)
{
	if (tz->_bit32.timecnt) {
		*tzf += (sizeof(int32_t) * tz->_bit32.timecnt);
		*tzf += sizeof(unsigned char) * tz->_bit32.timecnt;
	}
}

static int read_64bit_types(const unsigned char **tzf, timelib_tzinfo *tz)
{
	unsigned char *buffer;
	int32_t *leap_buffer;
	unsigned int i, j;

	/* Offset Types */
	buffer = (unsigned char*) timelib_malloc(tz->bit64.typecnt * sizeof(unsigned char) * 6);
	if (!buffer) {
		return TIMELIB_ERROR_CANNOT_ALLOCATE;
	}
	memcpy(buffer, *tzf, sizeof(unsigned char) * 6 * tz->bit64.typecnt);
	*tzf += sizeof(unsigned char) * 6 * tz->bit64.typecnt;

	// We add two extra to have space for potential new ttinfo entries due to new types defined in the
	// POSIX string
	tz->type = (ttinfo*) timelib_calloc(1, (tz->bit64.typecnt + 2) * sizeof(ttinfo));
	if (!tz->type) {
		timelib_free(buffer);
		return TIMELIB_ERROR_CANNOT_ALLOCATE;
	}

	for (i = 0; i < tz->bit64.typecnt; i++) {
		j = i * 6;
		tz->type[i].offset = 0;
		tz->type[i].offset += (int32_t) (((uint32_t) buffer[j]) << 24) + (buffer[j + 1] << 16) + (buffer[j + 2] << 8) + tz->type[i].offset + buffer[j + 3];
		tz->type[i].isdst = buffer[j + 4];
		tz->type[i].abbr_idx = buffer[j + 5];
	}
	timelib_free(buffer);

	/* Abbreviations */
	tz->timezone_abbr = (char*) timelib_malloc(tz->bit64.charcnt);
	if (!tz->timezone_abbr) {
		return TIMELIB_ERROR_CORRUPT_NO_ABBREVIATION;
	}
	memcpy(tz->timezone_abbr, *tzf, sizeof(char) * tz->bit64.charcnt);
	*tzf += sizeof(char) * tz->bit64.charcnt;

	/* Leap seconds (only use in 'right/') format */
	if (tz->bit64.leapcnt) {
		leap_buffer = (int32_t *) timelib_malloc(tz->bit64.leapcnt * (sizeof(int64_t) + sizeof(int32_t)));
		if (!leap_buffer) {
			return TIMELIB_ERROR_CANNOT_ALLOCATE;
		}
		memcpy(leap_buffer, *tzf, tz->bit64.leapcnt * (sizeof(int64_t) + sizeof(int32_t)));
		*tzf += tz->bit64.leapcnt * (sizeof(int64_t) + sizeof(int32_t));

		tz->leap_times = (tlinfo*) timelib_malloc(tz->bit64.leapcnt * sizeof(tlinfo));
		if (!tz->leap_times) {
			timelib_free(leap_buffer);
			return TIMELIB_ERROR_CANNOT_ALLOCATE;
		}
		for (i = 0; i < tz->bit64.leapcnt; i++) {
			tz->leap_times[i].trans = timelib_conv_int64_signed(leap_buffer[i * 3 + 1] * 4294967296 + leap_buffer[i * 3]);
			tz->leap_times[i].offset = timelib_conv_int_signed(leap_buffer[i * 3 + 2]);
		}
		timelib_free(leap_buffer);
	}

	/* Standard/Wall Indicators (unused) */
	if (tz->bit64.ttisstdcnt) {
		buffer = (unsigned char*) timelib_malloc(tz->bit64.ttisstdcnt * sizeof(unsigned char));
		if (!buffer) {
			return TIMELIB_ERROR_CANNOT_ALLOCATE;
		}
		memcpy(buffer, *tzf, sizeof(unsigned char) * tz->bit64.ttisstdcnt);
		*tzf += sizeof(unsigned char) * tz->bit64.ttisstdcnt;

		for (i = 0; i < tz->bit64.ttisstdcnt; i++) {
			tz->type[i].isstdcnt = buffer[i];
		}
		timelib_free(buffer);
	}

	/* UT/Local Time Indicators (unused) */
	if (tz->bit64.ttisgmtcnt) {
		buffer = (unsigned char*) timelib_malloc(tz->bit64.ttisgmtcnt * sizeof(unsigned char));
		if (!buffer) {
			return TIMELIB_ERROR_CANNOT_ALLOCATE;
		}
		memcpy(buffer, *tzf, sizeof(unsigned char) * tz->bit64.ttisgmtcnt);
		*tzf += sizeof(unsigned char) * tz->bit64.ttisgmtcnt;

		for (i = 0; i < tz->bit64.ttisgmtcnt; i++) {
			tz->type[i].isgmtcnt = buffer[i];
		}
		timelib_free(buffer);
	}

	return 0;
}

static void skip_32bit_types(const unsigned char **tzf, timelib_tzinfo *tz)
{
	/* Offset Types */
	*tzf += sizeof(unsigned char) * 6 * tz->_bit32.typecnt;

	/* Abbreviations */
	*tzf += sizeof(char) * tz->_bit32.charcnt;

	/* Leap seconds (only use in 'right/') format */
	if (tz->_bit32.leapcnt) {
		*tzf += sizeof(int32_t) * tz->_bit32.leapcnt * 2;
	}

	/* Standard/Wall Indicators (unused) */
	if (tz->_bit32.ttisstdcnt) {
		*tzf += sizeof(unsigned char) * tz->_bit32.ttisstdcnt;
	}

	/* UT/Local Time Indicators (unused) */
	if (tz->_bit32.ttisgmtcnt) {
		*tzf += sizeof(unsigned char) * tz->_bit32.ttisgmtcnt;
	}
}

static void read_posix_string(const unsigned char **tzf, timelib_tzinfo *tz)
{
	const unsigned char *begin;

	// POSIX string is delimited by \n
	(*tzf)++;
	begin = *tzf;

	while (*tzf[0] != '\n') {
		(*tzf)++;
	}

	tz->posix_string = timelib_calloc(1, *tzf - begin + 1);
	memcpy(tz->posix_string, begin, *tzf - begin);

	// skip over closing \n
	(*tzf)++;
}

static signed int find_ttinfo_index(timelib_tzinfo *tz, int32_t offset, int isdst, char *abbr)
{
	uint64_t i;

	for (i = 0; i < tz->bit64.typecnt; i++) {
		if (
			(offset == tz->type[i].offset) &&
			(isdst == tz->type[i].isdst) &&
			(strcmp(abbr, &tz->timezone_abbr[tz->type[i].abbr_idx]) == 0)
		) {
			return i;
		}
	}

	return TIMELIB_UNSET;
}

static unsigned int add_abbr(timelib_tzinfo *tz, char *abbr)
{
	size_t old_length = tz->bit64.charcnt;
	size_t new_length = old_length + strlen(abbr) + 1;
	tz->timezone_abbr = (char*) timelib_realloc(tz->timezone_abbr, new_length);
	memcpy(tz->timezone_abbr + old_length, abbr, strlen(abbr));
	tz->bit64.charcnt = new_length;
	tz->timezone_abbr[new_length - 1] = '\0';

	return old_length;
}

static signed int add_new_ttinfo_index(timelib_tzinfo *tz, int32_t offset, int isdst, char *abbr)
{
	tz->type[tz->bit64.typecnt].offset = offset;
	tz->type[tz->bit64.typecnt].isdst = isdst;
	tz->type[tz->bit64.typecnt].abbr_idx = add_abbr(tz, abbr);
	tz->type[tz->bit64.typecnt].isstdcnt = 0;
	tz->type[tz->bit64.typecnt].isgmtcnt = 0;

	++tz->bit64.typecnt;

	return tz->bit64.typecnt - 1;
}

static int integrate_posix_string(timelib_tzinfo *tz)
{
	tz->posix_info = timelib_parse_posix_str(tz->posix_string);
	if (!tz->posix_info) {
		return 0;
	}

	tz->posix_info->type_index_std_type = find_ttinfo_index(tz, tz->posix_info->std_offset, 0, tz->posix_info->std);
	if (tz->posix_info->type_index_std_type == TIMELIB_UNSET) {
		tz->posix_info->type_index_std_type = add_new_ttinfo_index(tz, tz->posix_info->std_offset, 0, tz->posix_info->std);
		return 1;
	}

	/* If there is no DST set for this zone, return */
	if (!tz->posix_info->dst) {
		return 1;
	}

	tz->posix_info->type_index_dst_type = find_ttinfo_index(tz, tz->posix_info->dst_offset, 1, tz->posix_info->dst);
	if (tz->posix_info->type_index_dst_type == TIMELIB_UNSET) {
		tz->posix_info->type_index_dst_type = add_new_ttinfo_index(tz, tz->posix_info->dst_offset, 1, tz->posix_info->dst);
		return 1;
	}

	return 1;
}

static void read_location(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[3];
	uint32_t comments_len;

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->location.latitude = timelib_conv_int_unsigned(buffer[0]);
	tz->location.latitude = (tz->location.latitude / 100000) - 90;
	tz->location.longitude = timelib_conv_int_unsigned(buffer[1]);
	tz->location.longitude = (tz->location.longitude / 100000) - 180;
	comments_len = timelib_conv_int_unsigned(buffer[2]);
	*tzf += sizeof(buffer);

	tz->location.comments = timelib_malloc(comments_len + 1);
	memcpy(tz->location.comments, *tzf, comments_len);
	tz->location.comments[comments_len] = '\0';
	*tzf += comments_len;
}

static void set_default_location_and_comments(const unsigned char **tzf, timelib_tzinfo *tz)
{
	tz->location.latitude = 0;
	tz->location.longitude = 0;
	tz->location.comments = timelib_malloc(2);
	tz->location.comments[0] = '?';
	tz->location.comments[1] = '\0';
}

static char *format_ut_time(timelib_sll ts, timelib_tzinfo *tz)
{
	char *tmp = timelib_calloc(1, 64);
	timelib_time *t = timelib_time_ctor();

	timelib_unixtime2gmt(t, ts);
	snprintf(
		tmp, 64,
		"%04lld-%02lld-%02lld %02lld:%02lld:%02lld UT",
		t->y, t->m, t->d,
		t->h, t->i, t->s
	);

	timelib_time_dtor(t);
	return tmp;
}

static char *format_offset_type(timelib_tzinfo *tz, int i)
{
	char *tmp = timelib_calloc(1, 64);

	snprintf(
		tmp, 64,
		"%3d [%6ld %1d %3d '%s' (%d,%d)]",
		i,
		(long int) tz->type[i].offset,
		tz->type[i].isdst,
		tz->type[i].abbr_idx,
		&tz->timezone_abbr[tz->type[i].abbr_idx],
		tz->type[i].isstdcnt,
		tz->type[i].isgmtcnt
	);

	return tmp;
}

void timelib_dump_tzinfo(timelib_tzinfo *tz)
{
	uint32_t  i;
	char     *date_str, *trans_str;

	printf("Country Code:      %s\n", tz->location.country_code);
	printf("Geo Location:      %f,%f\n", tz->location.latitude, tz->location.longitude);
	printf("Comments:\n%s\n",          tz->location.comments);
	printf("BC:                %s\n",  tz->bc ? "no" : "yes");
	printf("Slim File:         %s\n",  detect_slim_file(tz) ? "yes" : "no");

	printf("\n64-bit:\n");
	printf("UTC/Local count:   " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit64.ttisgmtcnt);
	printf("Std/Wall count:    " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit64.ttisstdcnt);
	printf("Leap.sec. count:   " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit64.leapcnt);
	printf("Trans. count:      " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit64.timecnt);
	printf("Local types count: " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit64.typecnt);
	printf("Zone Abbr. count:  " TIMELIB_ULONG_FMT "\n", (timelib_ulong) tz->bit64.charcnt);

	trans_str = format_offset_type(tz, 0);
	printf("%22s (%20s) = %s\n", "", "", trans_str);
	timelib_free(trans_str);

	for (i = 0; i < tz->bit64.timecnt; i++) {
		date_str = format_ut_time(tz->trans[i], tz);
		trans_str = format_offset_type(tz, tz->trans_idx[i]);
		printf(
			"%s (%20" PRId64 ") = %s\n",
			date_str,
			tz->trans[i],
			trans_str
		);
		timelib_free(date_str);
		timelib_free(trans_str);
	}
	for (i = 0; i < tz->bit64.leapcnt; i++) {
		date_str = format_ut_time(tz->trans[i], tz);
		printf (
			"%s (%20ld) = %d\n",
			date_str,
			(long) tz->leap_times[i].trans,
			tz->leap_times[i].offset
		);
		timelib_free(date_str);
	}

	if (!tz->posix_string) {
		printf("\n%43sNo POSIX string\n", "");
		return;
	}

	if (strcmp("", tz->posix_string) == 0) {
		printf("\n%43sEmpty POSIX string\n", "");
		return;
	}

	printf("\n%43sPOSIX string: %s\n", "", tz->posix_string);
	if (tz->posix_info && tz->posix_info->std) {
		trans_str = format_offset_type(tz, tz->posix_info->type_index_std_type);
		printf("%43sstd: %s\n", "", trans_str);
		timelib_free(trans_str);

		if (tz->posix_info->dst) {
			trans_str = format_offset_type(tz, tz->posix_info->type_index_dst_type);
			printf("%43sdst: %s\n", "", trans_str);
			timelib_free(trans_str);
		}
	}
}

static int seek_to_tz_position(const unsigned char **tzf, const char *timezone, const timelib_tzdb *tzdb)
{
	int left = 0, right = tzdb->index_size - 1;

	if (tzdb->index_size == 0) {
		return 0;
	}

	do {
		int mid = ((unsigned)left + right) >> 1;
		int cmp = timelib_strcasecmp(timezone, tzdb->index[mid].id);

		if (cmp < 0) {
			right = mid - 1;
		} else if (cmp > 0) {
			left = mid + 1;
		} else { /* (cmp == 0) */
			(*tzf) = &(tzdb->data[tzdb->index[mid].pos]);
			return 1;
		}

	} while (left <= right);

	return 0;
}

const timelib_tzdb *timelib_builtin_db(void)
{
	return &timezonedb_builtin;
}

const timelib_tzdb_index_entry *timelib_timezone_identifiers_list(const timelib_tzdb *tzdb, int *count)
{
	*count = tzdb->index_size;
	return tzdb->index;
}

int timelib_timezone_id_is_valid(const char *timezone, const timelib_tzdb *tzdb)
{
	const unsigned char *tzf;
	return (seek_to_tz_position(&tzf, timezone, tzdb));
}

static int skip_64bit_preamble(const unsigned char **tzf, timelib_tzinfo *tz)
{
	if (memcmp(*tzf, "TZif2", 5) == 0) {
		*tzf += 20;
		return 1;
	} else if (memcmp(*tzf, "TZif3", 5) == 0) {
		*tzf += 20;
		return 1;
	} else if (memcmp(*tzf, "TZif4", 5) == 0) {
		*tzf += 20;
		return 1;
	} else {
		return 0;
	}
}

static void read_64bit_header(const unsigned char **tzf, timelib_tzinfo *tz)
{
	uint32_t buffer[6];

	memcpy(&buffer, *tzf, sizeof(buffer));
	tz->bit64.ttisgmtcnt = timelib_conv_int_unsigned(buffer[0]);
	tz->bit64.ttisstdcnt = timelib_conv_int_unsigned(buffer[1]);
	tz->bit64.leapcnt    = timelib_conv_int_unsigned(buffer[2]);
	tz->bit64.timecnt    = timelib_conv_int_unsigned(buffer[3]);
	tz->bit64.typecnt    = timelib_conv_int_unsigned(buffer[4]);
	tz->bit64.charcnt    = timelib_conv_int_unsigned(buffer[5]);
	*tzf += sizeof(buffer);
}

static timelib_tzinfo* timelib_tzinfo_ctor(const char *name)
{
	timelib_tzinfo *t;
	t = timelib_calloc(1, sizeof(timelib_tzinfo));
	t->name = timelib_strdup(name);

	return t;
}

timelib_tzinfo *timelib_parse_tzfile(const char *timezone, const timelib_tzdb *tzdb, int *error_code)
{
	const unsigned char *tzf;
	timelib_tzinfo *tmp;
	int version;
	int transitions_result, types_result;
	unsigned int type = TIMELIB_TZINFO_ZONEINFO; /* TIMELIB_TZINFO_PHP or TIMELIB_TZINFO_ZONEINFO */

	*error_code = TIMELIB_ERROR_NO_ERROR;

	if (seek_to_tz_position(&tzf, timezone, tzdb)) {
		tmp = timelib_tzinfo_ctor(timezone);

		version = read_preamble(&tzf, tmp, &type);
		if (version < 2 || version > 4) {
			*error_code = TIMELIB_ERROR_UNSUPPORTED_VERSION;
			timelib_tzinfo_dtor(tmp);
			return NULL;
		}
//printf("- timezone: %s, version: %0d\n", timezone, version);

		read_32bit_header(&tzf, tmp);
		skip_32bit_transitions(&tzf, tmp);
		skip_32bit_types(&tzf, tmp);

		if (!skip_64bit_preamble(&tzf, tmp)) {
			/* 64 bit preamble is not in place */
			*error_code = TIMELIB_ERROR_CORRUPT_NO_64BIT_PREAMBLE;
			timelib_tzinfo_dtor(tmp);
			return NULL;
		}
		read_64bit_header(&tzf, tmp);
		if ((transitions_result = read_64bit_transitions(&tzf, tmp)) != 0) {
			/* Corrupt file as transitions do not increase */
			*error_code = transitions_result;
			timelib_tzinfo_dtor(tmp);
			return NULL;
		}
		if ((types_result = read_64bit_types(&tzf, tmp)) != 0) {
			*error_code = types_result;
			timelib_tzinfo_dtor(tmp);
			return NULL;
		}

		read_posix_string(&tzf, tmp);
		if (strcmp("", tmp->posix_string) == 0) {
			*error_code = TIMELIB_ERROR_EMPTY_POSIX_STRING;
		} else if (!integrate_posix_string(tmp)) {
			*error_code = TIMELIB_ERROR_CORRUPT_POSIX_STRING;
			timelib_tzinfo_dtor(tmp);
			return NULL;
		}

		if (type == TIMELIB_TZINFO_PHP) {
			read_location(&tzf, tmp);
		} else {
			set_default_location_and_comments(&tzf, tmp);
		}
	} else {
		*error_code = TIMELIB_ERROR_NO_SUCH_TIMEZONE;
		tmp = NULL;
	}

	return tmp;
}

void timelib_tzinfo_dtor(timelib_tzinfo *tz)
{
	TIMELIB_TIME_FREE(tz->name);
	TIMELIB_TIME_FREE(tz->trans);
	TIMELIB_TIME_FREE(tz->trans_idx);
	TIMELIB_TIME_FREE(tz->type);
	TIMELIB_TIME_FREE(tz->timezone_abbr);
	TIMELIB_TIME_FREE(tz->leap_times);
	TIMELIB_TIME_FREE(tz->location.comments);
	TIMELIB_TIME_FREE(tz->posix_string);
	if (tz->posix_info) {
		timelib_posix_str_dtor(tz->posix_info);
	}
	TIMELIB_TIME_FREE(tz);
	tz = NULL;
}

timelib_tzinfo *timelib_tzinfo_clone(timelib_tzinfo *tz)
{
	timelib_tzinfo *tmp = timelib_tzinfo_ctor(tz->name);
	tmp->_bit32.ttisgmtcnt = tz->_bit32.ttisgmtcnt;
	tmp->_bit32.ttisstdcnt = tz->_bit32.ttisstdcnt;
	tmp->_bit32.leapcnt = tz->_bit32.leapcnt;
	tmp->_bit32.timecnt = tz->_bit32.timecnt;
	tmp->_bit32.typecnt = tz->_bit32.typecnt;
	tmp->_bit32.charcnt = tz->_bit32.charcnt;
	tmp->bit64.ttisgmtcnt = tz->bit64.ttisgmtcnt;
	tmp->bit64.ttisstdcnt = tz->bit64.ttisstdcnt;
	tmp->bit64.leapcnt = tz->bit64.leapcnt;
	tmp->bit64.timecnt = tz->bit64.timecnt;
	tmp->bit64.typecnt = tz->bit64.typecnt;
	tmp->bit64.charcnt = tz->bit64.charcnt;

	if (tz->bit64.timecnt) {
		tmp->trans = (int64_t *) timelib_malloc(tz->bit64.timecnt * sizeof(int64_t));
		tmp->trans_idx = (unsigned char*) timelib_malloc(tz->bit64.timecnt * sizeof(unsigned char));
		memcpy(tmp->trans, tz->trans, tz->bit64.timecnt * sizeof(int64_t));
		memcpy(tmp->trans_idx, tz->trans_idx, tz->bit64.timecnt * sizeof(unsigned char));
	}

	tmp->type = (ttinfo*) timelib_malloc(tz->bit64.typecnt * sizeof(ttinfo));
	memcpy(tmp->type, tz->type, tz->bit64.typecnt * sizeof(ttinfo));

	tmp->timezone_abbr = (char*) timelib_malloc(tz->bit64.charcnt);
	memcpy(tmp->timezone_abbr, tz->timezone_abbr, tz->bit64.charcnt);

	if (tz->bit64.leapcnt) {
		tmp->leap_times = (tlinfo*) timelib_malloc(tz->bit64.leapcnt * sizeof(tlinfo));
		memcpy(tmp->leap_times, tz->leap_times, tz->bit64.leapcnt * sizeof(tlinfo));
	}

	if (tz->posix_string) {
		tmp->posix_string = timelib_strdup(tz->posix_string);
	}

	return tmp;
}

/**
 * Algorithm From RFC 8536, Section 3.2
 * https://tools.ietf.org/html/rfc8536#section-3.2
 */
ttinfo* timelib_fetch_timezone_offset(timelib_tzinfo *tz, timelib_sll ts, timelib_sll *transition_time)
{
	uint32_t left, right;

	/* RFC 8536: If there are no transitions, local time for all timestamps is specified
	 * by the TZ string in the footer if present and nonempty; otherwise, it is specified
	 * by time type 0.
	 *
	 * timelib: If there is also no time type 0, return NULL.
	 */
	if (!tz->bit64.timecnt || !tz->trans) {
		if (tz->posix_info) {
			*transition_time = INT64_MIN;
			return timelib_fetch_posix_timezone_offset(tz, ts, NULL);
		}

		if (tz->bit64.typecnt == 1) {
			*transition_time = INT64_MIN;
			return &(tz->type[0]);
		}
		return NULL;
	}

	/* RFC 8536: Local time for timestamps before the first transition is specified by
	 * the first time type (time type 0). */
	if (ts < tz->trans[0]) {
		*transition_time = INT64_MIN;
		return &(tz->type[0]);
	}

	/* RFC 8536: Local time for timestamps on or after the last transition is specified
	 * by the TZ string in the footer (Section 3.3) if present and nonempty; otherwise,
	 * it is unspecified.
	 *
	 * timelib: For 'unspecified', timelib assumes the last transition
	 */
	if (ts >= tz->trans[tz->bit64.timecnt - 1]) {
		if (tz->posix_info) {
			return timelib_fetch_posix_timezone_offset(tz, ts, transition_time);
		}

		*transition_time = tz->trans[tz->bit64.timecnt - 1];
		return &(tz->type[tz->trans_idx[tz->bit64.timecnt - 1]]);
	}

	/* RFC 8536: The type corresponding to a transition time specifies local time for
	 * timestamps starting at the given transition time and continuing up to, but not
	 * including, the next transition time. */
	left = 0;
	right = tz->bit64.timecnt - 1;

	while (right - left > 1) {
		uint32_t mid = (left + right) >> 1;

		if (ts < tz->trans[mid]) {
			right = mid;
		} else {
			left = mid;
		}
	}
	*transition_time = tz->trans[left];
	return &(tz->type[tz->trans_idx[left]]);
}

static tlinfo* fetch_leaptime_offset(timelib_tzinfo *tz, timelib_sll ts)
{
	int i;

	if (!tz->bit64.leapcnt || !tz->leap_times) {
		return NULL;
	}

	for (i = tz->bit64.leapcnt - 1; i > 0; i--) {
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

	if ((to = timelib_fetch_timezone_offset(tz, ts, &dummy))) {
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
	timelib_sll                transition_time;

	if ((to = timelib_fetch_timezone_offset(tz, ts, &transition_time))) {
		offset = to->offset;
		abbr = &(tz->timezone_abbr[to->abbr_idx]);
		tmp->is_dst = to->isdst;
		tmp->transition_time = transition_time;
	} else {
		offset = 0;
		abbr = tz->timezone_abbr;
		tmp->is_dst = 0;
		tmp->transition_time = 0;
	}

	if ((tl = fetch_leaptime_offset(tz, ts))) {
		leap_secs = -tl->offset;
	}

	tmp->offset = offset;
	tmp->leap_secs = leap_secs;
	tmp->abbr = abbr ? timelib_strdup(abbr) : timelib_strdup("GMT");

	return tmp;
}

int timelib_get_time_zone_offset_info(timelib_sll ts, timelib_tzinfo *tz, int32_t* offset, timelib_sll* transition_time, unsigned int* is_dst)
{
	ttinfo *to;
	timelib_sll tmp_transition_time;

	if (tz == NULL) {
		return 0;
	}

	if ((to = timelib_fetch_timezone_offset(tz, ts, &tmp_transition_time))) {
		if (offset) {
			*offset = to->offset;
		}
		if (is_dst) {
			*is_dst = to->isdst;
		}
		if (transition_time) {
			*transition_time = tmp_transition_time;
		}
		return 1;
	}
	return 0;
}

timelib_sll timelib_get_current_offset(timelib_time *t)
{
	switch (t->zone_type) {
		case TIMELIB_ZONETYPE_ABBR:
		case TIMELIB_ZONETYPE_OFFSET:
			return t->z + (t->dst * 3600);

		case TIMELIB_ZONETYPE_ID: {
			int32_t      offset = 0;
			timelib_get_time_zone_offset_info(t->sse, t->tz_info, &offset, NULL, NULL);
			return offset;
		}

		default:
			return 0;
	}
}

int timelib_same_timezone(timelib_time *one, timelib_time *two)
{
    if (one->zone_type != two->zone_type) {
        return 0;
    }

    if (one->zone_type == TIMELIB_ZONETYPE_ABBR || one->zone_type == TIMELIB_ZONETYPE_OFFSET) {
        if ((one->z + (one->dst * 3600)) == (two->z + (two->dst * 3600))) {
            return 1;
        }
        return 0;
    }

    if (one->zone_type == TIMELIB_ZONETYPE_ID && strcmp(one->tz_info->name, two->tz_info->name) == 0) {
        return 1;
    }

    return 0;
}
