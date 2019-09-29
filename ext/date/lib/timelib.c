/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2019 Derick Rethans
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
 *
 * Portions copyright (c) 1998-2017 Zend Technologies Ltd.
 *
 * The timelib_strcasecmp and timelib_strncasecmp are taken from PHP's
 * Zend/zend_operators.[hc] source files.
 *
 */

#include "timelib.h"
#include "timelib_private.h"
#include <ctype.h>
#include <math.h>

#define TIMELIB_LLABS(y) (y < 0 ? (y * -1) : y)

const char *timelib_error_messages[8] = {
	"No error",
	"Can not allocate buffer for parsing",
	"Corrupt tzfile: The transitions in the file don't always increase",
	"Corrupt tzfile: The expected 64-bit preamble is missing",
	"Corrupt tzfile: No abbreviation could be found for a transition",
	"The version used in this timezone identifier is unsupported",
	"No timezone with this name could be found",
};

const char *timelib_get_error_message(int error_code)
{
	int entries = sizeof(timelib_error_messages) / sizeof(char*);

	if (error_code >= 0 && error_code < entries) {
		return timelib_error_messages[error_code];
	}
	return "Unknown error code";
}

timelib_time* timelib_time_ctor(void)
{
	timelib_time *t;
	t = timelib_calloc(1, sizeof(timelib_time));

	return t;
}

void timelib_time_dtor(timelib_time* t)
{
	TIMELIB_TIME_FREE(t->tz_abbr);
	TIMELIB_TIME_FREE(t);
}

int timelib_time_compare(timelib_time *t1, timelib_time *t2)
{
	if (t1->sse == t2->sse) {
		if (t1->us == t2->us) {
			return 0;
		}

		return (t1->us < t2->us) ? -1 : 1;
	}

	return (t1->sse < t2->sse) ? -1 : 1;
}

timelib_time* timelib_time_clone(timelib_time *orig)
{
	timelib_time *tmp = timelib_time_ctor();
	memcpy(tmp, orig, sizeof(timelib_time));
	if (orig->tz_abbr) {
		tmp->tz_abbr = timelib_strdup(orig->tz_abbr);
	}
	if (orig->tz_info) {
		tmp->tz_info = orig->tz_info;
	}
	return tmp;
}

timelib_rel_time* timelib_rel_time_ctor(void)
{
	timelib_rel_time *t;
	t = timelib_calloc(1, sizeof(timelib_rel_time));

	return t;
}

void timelib_rel_time_dtor(timelib_rel_time* t)
{
	TIMELIB_TIME_FREE(t);
}

timelib_rel_time* timelib_rel_time_clone(timelib_rel_time *rel)
{
	timelib_rel_time *tmp = timelib_rel_time_ctor();
	memcpy(tmp, rel, sizeof(timelib_rel_time));
	return tmp;
}

void timelib_time_tz_abbr_update(timelib_time* tm, char* tz_abbr)
{
	unsigned int i;
	size_t tz_abbr_len = strlen(tz_abbr);

	TIMELIB_TIME_FREE(tm->tz_abbr);
	tm->tz_abbr = timelib_strdup(tz_abbr);
	for (i = 0; i < tz_abbr_len; i++) {
		tm->tz_abbr[i] = toupper(tz_abbr[i]);
	}
}

timelib_time_offset* timelib_time_offset_ctor(void)
{
	timelib_time_offset *t;
	t = timelib_calloc(1, sizeof(timelib_time_offset));

	return t;
}

void timelib_time_offset_dtor(timelib_time_offset* t)
{
	TIMELIB_TIME_FREE(t->abbr);
	TIMELIB_TIME_FREE(t);
}

char *timelib_get_tz_abbr_ptr(timelib_time *t)
{
	if (!t->sse_uptodate) {
		timelib_update_ts(t, NULL);
	};
	return t->tz_abbr;
}

void timelib_error_container_dtor(timelib_error_container *errors)
{
	int i;

	for (i = 0; i < errors->warning_count; i++) {
		timelib_free(errors->warning_messages[i].message);
	}
	timelib_free(errors->warning_messages);
	for (i = 0; i < errors->error_count; i++) {
		timelib_free(errors->error_messages[i].message);
	}
	timelib_free(errors->error_messages);
	timelib_free(errors);
}

timelib_long timelib_date_to_int(timelib_time *d, int *error)
{
	timelib_sll ts;

	ts = d->sse;

	if (ts < TIMELIB_LONG_MIN || ts > TIMELIB_LONG_MAX) {
		if (error) {
			*error = 1;
		}
		return 0;
	}
	if (error) {
		*error = 0;
	}
	return (timelib_long) d->sse;
}

void timelib_decimal_hour_to_hms(double h, int *hour, int *min, int *sec)
{
	if (h > 0) {
		*hour = floor(h);
		*min = floor((h - *hour) * 60);
		*sec = (h - *hour - ((float) *min / 60)) * 3600;
	} else {
		*hour = ceil(h);
		*min = 0 - ceil((h - *hour) * 60);
		*sec = 0 - (h - *hour - ((float) *min / -60)) * 3600;
	}
}

void timelib_hms_to_decimal_hour(int hour, int min, int sec, double *h)
{
	if (hour > 0) {
		*h = ((double)hour + (double)min / 60 + (double)sec / 3600);
	} else {
		*h = ((double)hour - (double)min / 60 - (double)sec / 3600);
	}
}

static const unsigned char timelib_tolower_map[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

#define timelib_tolower(c) (timelib_tolower_map[(unsigned char)(c)])
#undef MIN
#undef MAX
#define MAX(a, b)  (((a)>(b))?(a):(b))
#define MIN(a, b)  (((a)<(b))?(a):(b))

int timelib_strcasecmp(const char *s1, const char *s2)
{
	size_t len;
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	int c1, c2;

	if (s1 == s2) {
		return 0;
	}

	len = MIN(len1, len2);
	while (len--) {
		c1 = timelib_tolower(*(unsigned char *)s1++);
		c2 = timelib_tolower(*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return (int)(len1 - len2);
}

int timelib_strncasecmp(const char *s1, const char *s2, size_t length)
{
	size_t len;
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	int c1, c2;

	if (s1 == s2) {
		return 0;
	}
	len = MIN(length, MIN(len1, len2));
	while (len--) {
		c1 = timelib_tolower(*(unsigned char *)s1++);
		c2 = timelib_tolower(*(unsigned char *)s2++);
		if (c1 != c2) {
			return c1 - c2;
		}
	}

	return (int)(MIN(length, len1) - MIN(length, len2));
}

#undef MIN
#undef MAX

void timelib_dump_date(timelib_time *d, int options)
{
	if ((options & 2) == 2) {
		printf("TYPE: %d ", d->zone_type);
	}
	printf("TS: %lld | %s%04lld-%02lld-%02lld %02lld:%02lld:%02lld",
		d->sse, d->y < 0 ? "-" : "", TIMELIB_LLABS(d->y), d->m, d->d, d->h, d->i, d->s);
	if (d->us > 0) {
		printf(" 0.%06lld", d->us);
	}

	if (d->is_localtime) {
		switch (d->zone_type) {
			case TIMELIB_ZONETYPE_OFFSET: /* Only offset */
				printf(" GMT %05d%s", d->z, d->dst == 1 ? " (DST)" : "");
				break;
			case TIMELIB_ZONETYPE_ID: /* Timezone struct */
				/* Show abbreviation if wanted */
				if (d->tz_abbr) {
					printf(" %s", d->tz_abbr);
				}
				/* Do we have a TimeZone struct? */
				if (d->tz_info) {
					printf(" %s", d->tz_info->name);
				}
				break;
			case TIMELIB_ZONETYPE_ABBR:
				printf(" %s", d->tz_abbr);
				printf(" %05d%s", d->z, d->dst == 1 ? " (DST)" : "");
				break;
		}
	}

	if ((options & 1) == 1) {
		if (d->have_relative) {
			printf("%3lldY %3lldM %3lldD / %3lldH %3lldM %3lldS",
				d->relative.y, d->relative.m, d->relative.d, d->relative.h, d->relative.i, d->relative.s);
			if (d->relative.us) {
				printf(" 0.%06lld", d->relative.us);
			}
			if (d->relative.first_last_day_of != 0) {
				switch (d->relative.first_last_day_of) {
					case 1:
						printf(" / first day of");
						break;
					case 2:
						printf(" / last day of");
						break;
				}
			}
			if (d->relative.have_weekday_relative) {
				printf(" / %d.%d", d->relative.weekday, d->relative.weekday_behavior);
			}
			if (d->relative.have_special_relative) {
				switch (d->relative.special.type) {
					case TIMELIB_SPECIAL_WEEKDAY:
						printf(" / %lld weekday", d->relative.special.amount);
						break;
					case TIMELIB_SPECIAL_DAY_OF_WEEK_IN_MONTH:
						printf(" / x y of z month");
						break;
					case TIMELIB_SPECIAL_LAST_DAY_OF_WEEK_IN_MONTH:
						printf(" / last y of z month");
						break;
				}
			}
		}
	}
	printf("\n");
}

void timelib_dump_rel_time(timelib_rel_time *d)
{
	printf("%3lldY %3lldM %3lldD / %3lldH %3lldM %3lldS (days: %lld)%s",
		d->y, d->m, d->d, d->h, d->i, d->s, d->days, d->invert ? " inverted" : "");
	if (d->first_last_day_of != 0) {
		switch (d->first_last_day_of) {
			case 1:
				printf(" / first day of");
				break;
			case 2:
				printf(" / last day of");
				break;
		}
	}
	printf("\n");
}
