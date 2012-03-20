/* -*- c-file-style: "linux" -*- */

/*
 * strftime.c
 *
 * Public-domain implementation of ANSI C library routine.
 *
 * It's written in old-style C for maximal portability.
 * However, since I'm used to prototypes, I've included them too.
 *
 * If you want stuff in the System V ascftime routine, add the SYSV_EXT define.
 * For extensions from SunOS, add SUNOS_EXT.
 * For stuff needed to implement the P1003.2 date command, add POSIX2_DATE.
 * For VMS dates, add VMS_EXT.
 * For a an RFC822 time format, add MAILHEADER_EXT.
 * For ISO week years, add ISO_DATE_EXT.
 * For complete POSIX semantics, add POSIX_SEMANTICS.
 *
 * The code for %c, %x, and %X now follows the 1003.2 specification for
 * the POSIX locale.
 * This version ignores LOCALE information.
 * It also doesn't worry about multi-byte characters.
 * So there.
 *
 * This file is also shipped with GAWK (GNU Awk), gawk specific bits of
 * code are included if GAWK is defined.
 *
 * Arnold Robbins
 * January, February, March, 1991
 * Updated March, April 1992
 * Updated April, 1993
 * Updated February, 1994
 * Updated May, 1994
 * Updated January, 1995
 * Updated September, 1995
 * Updated January, 1996
 *
 * Fixes from ado@elsie.nci.nih.gov
 * February 1991, May 1992
 * Fixes from Tor Lillqvist tml@tik.vtt.fi
 * May, 1993
 * Further fixes from ado@elsie.nci.nih.gov
 * February 1994
 * %z code from chip@chinacat.unicom.com
 * Applied September 1995
 * %V code fixed (again) and %G, %g added,
 * January 1996
 */

#include "ruby/ruby.h"
#include "date_tmx.h"

#ifndef GAWK
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#endif
#if defined(TM_IN_SYS_TIME) || !defined(GAWK)
#include <sys/types.h>
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#endif
#include <math.h>

/* defaults: season to taste */
#define SYSV_EXT	1	/* stuff in System V ascftime routine */
#define SUNOS_EXT	1	/* stuff in SunOS strftime routine */
#define POSIX2_DATE	1	/* stuff in Posix 1003.2 date command */
#define VMS_EXT		1	/* include %v for VMS date format */
#define MAILHEADER_EXT	1	/* add %z for HHMM format */
#define ISO_DATE_EXT	1	/* %G and %g for year of ISO week */

#if defined(ISO_DATE_EXT)
#if ! defined(POSIX2_DATE)
#define POSIX2_DATE	1
#endif
#endif

#if defined(POSIX2_DATE)
#if ! defined(SYSV_EXT)
#define SYSV_EXT	1
#endif
#if ! defined(SUNOS_EXT)
#define SUNOS_EXT	1
#endif
#endif

#if defined(POSIX2_DATE)
#define adddecl(stuff)	stuff
#else
#define adddecl(stuff)
#endif

#undef strchr	/* avoid AIX weirdness */

#if 0
#if !defined __STDC__ && !defined _WIN32
#define const	/**/
static int weeknumber();
adddecl(static int iso8601wknum();)
static int weeknumber_v();
adddecl(static int iso8601wknum_v();)
#else
static int weeknumber(const struct tm *timeptr, int firstweekday);
adddecl(static int iso8601wknum(const struct tm *timeptr);)
static int weeknumber_v(const struct tmx *tmx, int firstweekday);
adddecl(static int iso8601wknum_v(const struct tmx *tmx);)
#endif
#endif

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#else
extern void *malloc();
extern void *realloc();
extern char *getenv();
extern char *strchr();
#endif

#define range(low, item, hi)	max((low), min((item), (hi)))

#undef min	/* just in case */

/* min --- return minimum of two numbers */

#ifndef __STDC__
static inline int
min(a, b)
int a, b;
#else
static inline int
min(int a, int b)
#endif
{
	return (a < b ? a : b);
}

#undef max	/* also, just in case */

/* max --- return maximum of two numbers */

#ifndef __STDC__
static inline int
max(a, b)
int a, b;
#else
static inline int
max(int a, int b)
#endif
{
	return (a > b ? a : b);
}

#ifdef NO_STRING_LITERAL_CONCATENATION
#error No string literal concatenation
#endif

#define add(x,y) (rb_funcall((x), '+', 1, (y)))
#define sub(x,y) (rb_funcall((x), '-', 1, (y)))
#define mul(x,y) (rb_funcall((x), '*', 1, (y)))
#define quo(x,y) (rb_funcall((x), rb_intern("quo"), 1, (y)))
#define div(x,y) (rb_funcall((x), rb_intern("div"), 1, (y)))
#define mod(x,y) (rb_funcall((x), '%', 1, (y)))

/* strftime --- produce formatted time */

static size_t
date_strftime_with_tmx(char *s, size_t maxsize, const char *format,
		       const struct tmx *tmx)
{
	char *endp = s + maxsize;
	char *start = s;
	const char *sp, *tp;
	auto char tbuf[100];
	ptrdiff_t i;
	int v, w;
	int precision, flags, colons;
	char padding;
	enum {LEFT, CHCASE, LOWER, UPPER, LOCALE_O, LOCALE_E};
#define BIT_OF(n) (1U<<(n))

	/* various tables, useful in North America */
	static const char days_l[][10] = {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday",
	};
	static const char months_l[][10] = {
		"January", "February", "March", "April",
		"May", "June", "July", "August", "September",
		"October", "November", "December",
	};
	static const char ampm[][3] = { "AM", "PM", };

	if (s == NULL || format == NULL || tmx == NULL || maxsize == 0)
		return 0;

	/* quick check if we even need to bother */
	if (strchr(format, '%') == NULL && strlen(format) + 1 >= maxsize) {
	err:
		errno = ERANGE;
		return 0;
	}

	for (; *format && s < endp - 1; format++) {
#define FLAG_FOUND() do { \
			if (precision > 0 || flags & (BIT_OF(LOCALE_E)|BIT_OF(LOCALE_O))) \
				goto unknown; \
		} while (0)
#define NEEDS(n) do if (s >= endp || (n) >= endp - s - 1) goto err; while (0)
#define FILL_PADDING(i) do { \
	if (!(flags & BIT_OF(LEFT)) && precision > (i)) { \
		NEEDS(precision); \
		memset(s, padding ? padding : ' ', precision - (i)); \
		s += precision - (i); \
	} \
	else { \
		NEEDS(i); \
	} \
} while (0);
#define FMT(def_pad, def_prec, fmt, val) \
		do { \
			int l; \
			if (precision <= 0) precision = (def_prec); \
			if (flags & BIT_OF(LEFT)) precision = 1; \
			l = snprintf(s, endp - s, \
				     ((padding == '0' || (!padding && (def_pad) == '0')) ? "%0*"fmt : "%*"fmt), \
				     precision, (val)); \
			if (l < 0) goto err; \
			s += l; \
		} while (0)
#define STRFTIME(fmt) \
		do { \
			i = date_strftime_with_tmx(s, endp - s, (fmt), tmx); \
			if (!i) return 0; \
			if (precision > i) {\
				if (start + maxsize < s + precision) { \
					errno = ERANGE; \
					return 0; \
				} \
				memmove(s + precision - i, s, i);\
				memset(s, padding ? padding : ' ', precision - i); \
				s += precision;	\
	                }\
			else s += i; \
		} while (0)
#define FMTV(def_pad, def_prec, fmt, val) \
                do { \
                        VALUE tmp = (val); \
                        if (FIXNUM_P(tmp)) { \
                                FMT((def_pad), (def_prec), "l"fmt, FIX2LONG(tmp)); \
                        } \
                        else { \
                                VALUE args[2], result; \
                                size_t l; \
                                if (precision <= 0) precision = (def_prec); \
                                if (flags & BIT_OF(LEFT)) precision = 1; \
                                args[0] = INT2FIX(precision); \
                                args[1] = (val); \
                                if (padding == '0' || (!padding && (def_pad) == '0')) \
                                        result = rb_str_format(2, args, rb_str_new2("%0*"fmt)); \
                                else \
                                        result = rb_str_format(2, args, rb_str_new2("%*"fmt)); \
                                l = strlcpy(s, StringValueCStr(result), endp-s); \
                                if ((size_t)(endp-s) <= l) \
                                        goto err; \
                                s += l; \
                        } \
                } while (0)

		if (*format != '%') {
			*s++ = *format;
			continue;
		}
		tp = tbuf;
		sp = format;
		precision = -1;
		flags = 0;
		padding = 0;
                colons = 0;
	again:
		switch (*++format) {
		case '\0':
			format--;
			goto unknown;

		case '%':
			FILL_PADDING(1);
			*s++ = '%';
			continue;

		case 'a':	/* abbreviated weekday name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			{
				int wday = tmx_wday;
				if (wday < 0 || wday > 6)
					i = 1, tp = "?";
				else
					i = 3, tp = days_l[wday];
			}
			break;

		case 'A':	/* full weekday name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			{
				int wday = tmx_wday;
				if (wday < 0 || wday > 6)
					i = 1, tp = "?";
				else
					i = strlen(tp = days_l[wday]);
			}
			break;

#ifdef SYSV_EXT
		case 'h':	/* abbreviated month name */
#endif
		case 'b':	/* abbreviated month name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			{
				int mon = tmx_mon;
				if (mon < 1 || mon > 12)
					i = 1, tp = "?";
				else
					i = 3, tp = months_l[mon-1];
			}
			break;

		case 'B':	/* full month name */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(LOWER)|BIT_OF(CHCASE));
				flags |= BIT_OF(UPPER);
			}
			{
				int mon = tmx_mon;
				if (mon < 1 || mon > 12)
					i = 1, tp = "?";
				else
					i = strlen(tp = months_l[mon-1]);
			}
			break;

		case 'c':	/* appropriate date and time representation */
			STRFTIME("%a %b %e %H:%M:%S %Y");
			continue;

		case 'd':	/* day of the month, 01 - 31 */
			v = range(1, tmx_mday, 31);
			FMT('0', 2, "d", v);
			continue;

		case 'H':	/* hour, 24-hour clock, 00 - 23 */
			v = range(0, tmx_hour, 23);
			FMT('0', 2, "d", v);
			continue;

		case 'I':	/* hour, 12-hour clock, 01 - 12 */
			v = range(0, tmx_hour, 23);
			if (v == 0)
				v = 12;
			else if (v > 12)
				v -= 12;
			FMT('0', 2, "d", v);
			continue;

		case 'j':	/* day of the year, 001 - 366 */
			FMT('0', 3, "d", tmx_yday);
			continue;

		case 'm':	/* month, 01 - 12 */
			v = range(1, tmx_mon, 12);
			FMT('0', 2, "d", v);
			continue;

		case 'M':	/* minute, 00 - 59 */
			v = range(0, tmx_min, 59);
			FMT('0', 2, "d", v);
			continue;

		case 'p':	/* AM or PM based on 12-hour clock */
		case 'P':	/* am or pm based on 12-hour clock */
			if ((*format == 'p' && (flags & BIT_OF(CHCASE))) ||
			    (*format == 'P' && !(flags & (BIT_OF(CHCASE)|BIT_OF(UPPER))))) {
				flags &= ~(BIT_OF(UPPER)|BIT_OF(CHCASE));
				flags |= BIT_OF(LOWER);
			}
			v = range(0, tmx_hour, 23);
			if (v < 12)
				tp = ampm[0];
			else
				tp = ampm[1];
			i = 2;
			break;

		case 's':
			FMTV('0', 1, "d", tmx_secs);
                        continue;

		case 'Q':
			FMTV('0', 1, "d", tmx_msecs);
                        continue;

		case 'S':	/* second, 00 - 59 */
			v = range(0, tmx_sec, 59);
			FMT('0', 2, "d", v);
			continue;

		case 'U':	/* week of year, Sunday is first day of week */
			FMT('0', 2, "d", tmx_wnum0);
			continue;

		case 'w':	/* weekday, Sunday == 0, 0 - 6 */
			v = range(0, tmx_wday, 6);
			FMT('0', 1, "d", v);
			continue;

		case 'W':	/* week of year, Monday is first day of week */
			FMT('0', 2, "d", tmx_wnum1);
			continue;

		case 'x':	/* appropriate date representation */
			STRFTIME("%m/%d/%y");
			continue;

		case 'X':	/* appropriate time representation */
			STRFTIME("%H:%M:%S");
			continue;

		case 'y':	/* year without a century, 00 - 99 */
			v = NUM2INT(mod(tmx_year, INT2FIX(100)));
			FMT('0', 2, "d", v);
			continue;

		case 'Y':	/* year with century */
			{
				VALUE year = tmx_year;
				if (FIXNUM_P(year)) {
					long y = FIX2LONG(year);
					FMT('0', 0 <= y ? 4 : 5, "ld", y);
				}
				else {
					FMTV('0', 4, "d", year);
				}
			}
			continue;

#ifdef MAILHEADER_EXT
		case 'z':	/* time zone offset east of GMT e.g. -0600 */
			{
				long off, aoff;
				int hl, hw;

				off = NUM2LONG(rb_funcall(tmx_offset, rb_intern("round"), 0));

				aoff = off;
				if (aoff < 0)
					aoff = -off;

				if ((aoff / 3600) < 10)
					hl = 1;
				else
					hl = 2;
				hw = 2;
				if (flags & BIT_OF(LEFT) && hl == 1)
					hw = 1;

				switch (colons) {
				case 0: /* %z -> +hhmm */
					precision = precision <= (3 + hw) ? hw : precision-3;
					NEEDS(precision + 3);
					break;

				case 1: /* %:z -> +hh:mm */
					precision = precision <= (4 + hw) ? hw : precision-4;
					NEEDS(precision + 4);
					break;

				case 2: /* %::z -> +hh:mm:ss */
					precision = precision <= (7 + hw) ? hw : precision-7;
					NEEDS(precision + 7);
					break;

				case 3: /* %:::z -> +hh[:mm[:ss]] */
				{
					if (aoff % 3600 == 0) {
						precision = precision <= (1 + hw) ? hw : precision-1;
						NEEDS(precision + 3);
					}
					else if (aoff % 60 == 0) {
						precision = precision <= (4 + hw) ? hw : precision-4;
						NEEDS(precision + 4);
					}
					else {
						precision = precision <= (7 + hw) ? hw : precision-7;
						NEEDS(precision + 7);
					}
				}
				break;

				default:
					format--;
					goto unknown;
				}
				if (padding == ' ' && precision > hl) {
					i = snprintf(s, endp - s, "%*s", precision - hl, "");
					precision = hl;
					if (i < 0) goto err;
					s += i;
				}
				if (off < 0) {
					off = -off;
					*s++ = '-';
				} else {
					*s++ = '+';
				}
				i = snprintf(s, endp - s, "%.*ld", precision, off / 3600);
				if (i < 0) goto err;
				s += i;
				off = off % 3600;
				if (colons == 3 && off == 0)
					continue;
				if (1 <= colons)
					*s++ = ':';
				i = snprintf(s, endp - s, "%02d", (int)(off / 60));
				if (i < 0) goto err;
				s += i;
				off = off % 60;
				if (colons == 3 && off == 0)
					continue;
				if (2 <= colons) {
					*s++ = ':';
					i = snprintf(s, endp - s, "%02d", (int)off);
					if (i < 0) goto err;
					s += i;
				}
			}
			continue;
#endif /* MAILHEADER_EXT */

		case 'Z':	/* time zone name or abbreviation */
			if (flags & BIT_OF(CHCASE)) {
				flags &= ~(BIT_OF(UPPER)|BIT_OF(CHCASE));
				flags |= BIT_OF(LOWER);
			}
			{
				char *zone = tmx_zone;
				if (zone == NULL)
					tp = "";
				else
					tp = zone;
				i = strlen(tp);
			}
			break;

#ifdef SYSV_EXT
		case 'n':	/* same as \n */
			FILL_PADDING(1);
			*s++ = '\n';
			continue;

		case 't':	/* same as \t */
			FILL_PADDING(1);
			*s++ = '\t';
			continue;

		case 'D':	/* date as %m/%d/%y */
			STRFTIME("%m/%d/%y");
			continue;

		case 'e':	/* day of month, blank padded */
			FMT(' ', 2, "d", range(1, tmx_mday, 31));
			continue;

		case 'r':	/* time as %I:%M:%S %p */
			STRFTIME("%I:%M:%S %p");
			continue;

		case 'R':	/* time as %H:%M */
			STRFTIME("%H:%M");
			continue;

		case 'T':	/* time as %H:%M:%S */
			STRFTIME("%H:%M:%S");
			continue;
#endif

#ifdef SUNOS_EXT
		case 'k':	/* hour, 24-hour clock, blank pad */
			v = range(0, tmx_hour, 23);
			FMT(' ', 2, "d", v);
			continue;

		case 'l':	/* hour, 12-hour clock, 1 - 12, blank pad */
			v = range(0, tmx_hour, 23);
			if (v == 0)
				v = 12;
			else if (v > 12)
				v -= 12;
			FMT(' ', 2, "d", v);
			continue;
#endif

#ifdef VMS_EXT
		case 'v':	/* date as dd-bbb-YYYY */
			STRFTIME("%e-%b-%Y");
			continue;
#endif

#ifdef POSIX2_DATE
		case 'C':
                        FMTV('0', 2, "d", div(tmx_year, INT2FIX(100)));
			continue;

		case 'E':
			/* POSIX locale extensions, ignored for now */
			flags |= BIT_OF(LOCALE_E);
			if (*(format + 1) && strchr("cCxXyY", *(format + 1)))
				goto again;
			goto unknown;
		case 'O':
			/* POSIX locale extensions, ignored for now */
			flags |= BIT_OF(LOCALE_O);
			if (*(format + 1) && strchr("deHImMSuUVwWy",
						    *(format + 1)))
				goto again;
			goto unknown;
		case 'V':	/* week of year according ISO 8601 */
			FMT('0', 2, "d", tmx_cweek);
			continue;

		case 'u':
		/* ISO 8601: Weekday as a decimal number [1 (Monday) - 7] */
			FMT('0', 1, "d", tmx_cwday);
			continue;
#endif	/* POSIX2_DATE */

#ifdef ISO_DATE_EXT
		case 'g':	/* year of ISO week without a century */
			v = NUM2INT(mod(tmx_cwyear, INT2FIX(100)));
			FMT('0', 2, "d", v);
			continue;

		case 'G':	/* year of ISO week with century */
			{
				VALUE year = tmx_cwyear;
				if (FIXNUM_P(year)) {
					long y = FIX2LONG(year);
					FMT('0', 0 <= y ? 4 : 5, "ld", y);
				}
				else {
					FMTV('0', 4, "d", year);
				}
                                continue;
			}

#endif /* ISO_DATE_EXT */

		case 'L':
			w = 3;
			goto subsec;

		case 'N':
			/*
			 * fractional second digits. default is 9 digits
			 * (nanosecond).
			 *
			 * %3N  millisecond (3 digits)
			 * %6N  microsecond (6 digits)
			 * %9N  nanosecond (9 digits)
			 */
			w = 9;
		subsec:
                        if (precision <= 0) {
                            precision = w;
                        }
                        NEEDS(precision);

			{
                                VALUE subsec = tmx_sec_fraction;
                                int ww;
                                long n;

                                ww = precision;
                                while (9 <= ww) {
                                        subsec = mul(subsec, INT2FIX(1000000000));
                                        ww -= 9;
                                }
                                n = 1;
                                for (; 0 < ww; ww--)
                                        n *= 10;
                                if (n != 1)
                                        subsec = mul(subsec, INT2FIX(n));
                                subsec = div(subsec, INT2FIX(1));

                                if (FIXNUM_P(subsec)) {
                                        (void)snprintf(s, endp - s, "%0*ld", precision, FIX2LONG(subsec));
                                        s += precision;
                                }
                                else {
                                        VALUE args[2], result;
                                        args[0] = INT2FIX(precision);
                                        args[1] = subsec;
                                        result = rb_str_format(2, args, rb_str_new2("%0*d"));
                                        (void)strlcpy(s, StringValueCStr(result), endp-s);
                                        s += precision;
                                }
			}
			continue;

		case 'F':	/*  Equivalent to %Y-%m-%d */
			STRFTIME("%Y-%m-%d");
			continue;
		case '+':
			STRFTIME("%a %b %e %H:%M:%S %Z %Y");
			continue;

		case '-':
			FLAG_FOUND();
			flags |= BIT_OF(LEFT);
			padding = precision = 0;
			goto again;

		case '^':
			FLAG_FOUND();
			flags |= BIT_OF(UPPER);
			goto again;

		case '#':
			FLAG_FOUND();
			flags |= BIT_OF(CHCASE);
			goto again;

		case '_':
			FLAG_FOUND();
			padding = ' ';
			goto again;

		case ':':
                        colons++;
			goto again;

		case '0':
			padding = '0';
		case '1':  case '2': case '3': case '4':
		case '5': case '6':  case '7': case '8': case '9':
			{
				char *e;
				precision = (int)strtoul(format, &e, 10);
				format = e - 1;
				goto again;
			}

		default:
		unknown:
			i = format - sp + 1;
			tp = sp;
			precision = -1;
			flags = 0;
			padding = 0;
                        colons = 0;
			break;
		}
		if (i) {
			FILL_PADDING(i);
			memcpy(s, tp, i);
			switch (flags & (BIT_OF(UPPER)|BIT_OF(LOWER))) {
			case BIT_OF(UPPER):
				do {
					if (ISLOWER(*s)) *s = TOUPPER(*s);
				} while (s++, --i);
				break;
			case BIT_OF(LOWER):
				do {
					if (ISUPPER(*s)) *s = TOLOWER(*s);
				} while (s++, --i);
				break;
			default:
				s += i;
				break;
			}
		}
	}
	if (s >= endp) {
		goto err;
	}
	if (*format == '\0') {
		*s = '\0';
		return (s - start);
	} else
		return 0;
}

size_t
date_strftime(char *s, size_t maxsize, const char *format,
	      const struct tmx *tmx)
{
	return date_strftime_with_tmx(s, maxsize, format, tmx);
}

#if 0
/* isleap --- is a year a leap year? */

#ifndef __STDC__
static int
isleap(year)
long year;
#else
static int
isleap(long year)
#endif
{
	return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

static void
tmx2tm_noyear(const struct tmx *tmx, struct tm *result)
{
    struct tm tm;

    /* for isleap() in iso8601wknum.  +100 is -1900 (mod 400). */
    tm.tm_year = FIX2INT(mod(tmx_year, INT2FIX(400))) + 100;

    tm.tm_mon = tmx_mon-1;
    tm.tm_mday = tmx_mday;
    tm.tm_hour = tmx_hour;
    tm.tm_min = tmx_min;
    tm.tm_sec = tmx_sec;
    tm.tm_wday = tmx_wday;
    tm.tm_yday = tmx_yday-1;
    tm.tm_isdst = 0;
#if defined(HAVE_STRUCT_TM_TM_GMTOFF)
    tm.tm_gmtoff = NUM2LONG(tmx_offset);
#endif
#if defined(HAVE_TM_ZONE)
    tm.tm_zone = (char *)tmx_zone;
#endif
    *result = tm;
}

#ifdef POSIX2_DATE
/* iso8601wknum --- compute week number according to ISO 8601 */

#ifndef __STDC__
static int
iso8601wknum(timeptr)
const struct tm *timeptr;
#else
static int
iso8601wknum(const struct tm *timeptr)
#endif
{
	/*
	 * From 1003.2:
	 *	If the week (Monday to Sunday) containing January 1
	 *	has four or more days in the new year, then it is week 1;
	 *	otherwise it is the highest numbered week of the previous
	 *	year (52 or 53), and the next week is week 1.
	 *
	 * ADR: This means if Jan 1 was Monday through Thursday,
	 *	it was week 1, otherwise week 52 or 53.
	 *
	 * XPG4 erroneously included POSIX.2 rationale text in the
	 * main body of the standard. Thus it requires week 53.
	 */

	int weeknum, jan1day;

	/* get week number, Monday as first day of the week */
	weeknum = weeknumber(timeptr, 1);

	/*
	 * With thanks and tip of the hatlo to tml@tik.vtt.fi
	 *
	 * What day of the week does January 1 fall on?
	 * We know that
	 *	(timeptr->tm_yday - jan1.tm_yday) MOD 7 ==
	 *		(timeptr->tm_wday - jan1.tm_wday) MOD 7
	 * and that
	 * 	jan1.tm_yday == 0
	 * and that
	 * 	timeptr->tm_wday MOD 7 == timeptr->tm_wday
	 * from which it follows that. . .
	 */
	jan1day = timeptr->tm_wday - (timeptr->tm_yday % 7);
	if (jan1day < 0)
		jan1day += 7;

	/*
	 * If Jan 1 was a Monday through Thursday, it was in
	 * week 1.  Otherwise it was last year's highest week, which is
	 * this year's week 0.
	 *
	 * What does that mean?
	 * If Jan 1 was Monday, the week number is exactly right, it can
	 *	never be 0.
	 * If it was Tuesday through Thursday, the weeknumber is one
	 *	less than it should be, so we add one.
	 * Otherwise, Friday, Saturday or Sunday, the week number is
	 * OK, but if it is 0, it needs to be 52 or 53.
	 */
	switch (jan1day) {
	case 1:		/* Monday */
		break;
	case 2:		/* Tuesday */
	case 3:		/* Wednesday */
	case 4:		/* Thursday */
		weeknum++;
		break;
	case 5:		/* Friday */
	case 6:		/* Saturday */
	case 0:		/* Sunday */
		if (weeknum == 0) {
#ifdef USE_BROKEN_XPG4
			/* XPG4 (as of March 1994) says 53 unconditionally */
			weeknum = 53;
#else
			/* get week number of last week of last year */
			struct tm dec31ly;	/* 12/31 last year */
			dec31ly = *timeptr;
			dec31ly.tm_year--;
			dec31ly.tm_mon = 11;
			dec31ly.tm_mday = 31;
			dec31ly.tm_wday = (jan1day == 0) ? 6 : jan1day - 1;
			dec31ly.tm_yday = 364 + isleap(dec31ly.tm_year + 1900L);
			weeknum = iso8601wknum(& dec31ly);
#endif
		}
		break;
	}

	if (timeptr->tm_mon == 11) {
		/*
		 * The last week of the year
		 * can be in week 1 of next year.
		 * Sigh.
		 *
		 * This can only happen if
		 *	M   T  W
		 *	29  30 31
		 *	30  31
		 *	31
		 */
		int wday, mday;

		wday = timeptr->tm_wday;
		mday = timeptr->tm_mday;
		if (   (wday == 1 && (mday >= 29 && mday <= 31))
		    || (wday == 2 && (mday == 30 || mday == 31))
		    || (wday == 3 &&  mday == 31))
			weeknum = 1;
	}

	return weeknum;
}

static int
iso8601wknum_v(const struct tmx *tmx)
{
        struct tm tm;
        tmx2tm_noyear(tmx, &tm);
        return iso8601wknum(&tm);
}

#endif

/* weeknumber --- figure how many weeks into the year */

/* With thanks and tip of the hatlo to ado@elsie.nci.nih.gov */

#ifndef __STDC__
static int
weeknumber(timeptr, firstweekday)
const struct tm *timeptr;
int firstweekday;
#else
static int
weeknumber(const struct tm *timeptr, int firstweekday)
#endif
{
	int wday = timeptr->tm_wday;
	int ret;

	if (firstweekday == 1) {
		if (wday == 0)	/* sunday */
			wday = 6;
		else
			wday--;
	}
	ret = ((timeptr->tm_yday + 7 - wday) / 7);
	if (ret < 0)
		ret = 0;
	return ret;
}

static int
weeknumber_v(const struct tmx *tmx, int firstweekday)
{
        struct tm tm;
        tmx2tm_noyear(tmx, &tm);
        return weeknumber(&tm, firstweekday);
}
#endif

#if 0
/* ADR --- I'm loathe to mess with ado's code ... */

Date:         Wed, 24 Apr 91 20:54:08 MDT
From: Michal Jaegermann <audfax!emory!vm.ucs.UAlberta.CA!NTOMCZAK>
To: arnold@audiofax.com

Hi Arnold,
in a process of fixing of strftime() in libraries on Atari ST I grabbed
some pieces of code from your own strftime.  When doing that it came
to mind that your weeknumber() function compiles a little bit nicer
in the following form:
/*
 * firstweekday is 0 if starting in Sunday, non-zero if in Monday
 */
{
    return (timeptr->tm_yday - timeptr->tm_wday +
	    (firstweekday ? (timeptr->tm_wday ? 8 : 1) : 7)) / 7;
}
How nicer it depends on a compiler, of course, but always a tiny bit.

   Cheers,
   Michal
   ntomczak@vm.ucs.ualberta.ca
#endif

#ifdef	TEST_STRFTIME

/*
 * NAME:
 *	tst
 *
 * SYNOPSIS:
 *	tst
 *
 * DESCRIPTION:
 *	"tst" is a test driver for the function "strftime".
 *
 * OPTIONS:
 *	None.
 *
 * AUTHOR:
 *	Karl Vogel
 *	Control Data Systems, Inc.
 *	vogelke@c-17igp.wpafb.af.mil
 *
 * BUGS:
 *	None noticed yet.
 *
 * COMPILE:
 *	cc -o tst -DTEST_STRFTIME strftime.c
 */

/* ADR: I reformatted this to my liking, and deleted some unneeded code. */

#ifndef NULL
#include	<stdio.h>
#endif
#include	<time.h>
#include	<sys/time.h>
#include	<string.h>

#define		MAXTIME		132

/*
 * Array of time formats.
 */

static char *array[] =
{
	"(%%A)      full weekday name, var length (Sunday..Saturday)  %A",
	"(%%B)       full month name, var length (January..December)  %B",
	"(%%C)                                               Century  %C",
	"(%%D)                                       date (%%m/%%d/%%y)  %D",
	"(%%E)                           Locale extensions (ignored)  %E",
	"(%%H)                          hour (24-hour clock, 00..23)  %H",
	"(%%I)                          hour (12-hour clock, 01..12)  %I",
	"(%%M)                                       minute (00..59)  %M",
	"(%%O)                           Locale extensions (ignored)  %O",
	"(%%R)                                 time, 24-hour (%%H:%%M)  %R",
	"(%%S)                                       second (00..60)  %S",
	"(%%T)                              time, 24-hour (%%H:%%M:%%S)  %T",
	"(%%U)    week of year, Sunday as first day of week (00..53)  %U",
	"(%%V)                    week of year according to ISO 8601  %V",
	"(%%W)    week of year, Monday as first day of week (00..53)  %W",
	"(%%X)     appropriate locale time representation (%H:%M:%S)  %X",
	"(%%Y)                           year with century (1970...)  %Y",
	"(%%Z) timezone (EDT), or blank if timezone not determinable  %Z",
	"(%%a)          locale's abbreviated weekday name (Sun..Sat)  %a",
	"(%%b)            locale's abbreviated month name (Jan..Dec)  %b",
	"(%%c)           full date (Sat Nov  4 12:02:33 1989)%n%t%t%t  %c",
	"(%%d)                             day of the month (01..31)  %d",
	"(%%e)               day of the month, blank-padded ( 1..31)  %e",
	"(%%h)                                should be same as (%%b)  %h",
	"(%%j)                            day of the year (001..366)  %j",
	"(%%k)               hour, 24-hour clock, blank pad ( 0..23)  %k",
	"(%%l)               hour, 12-hour clock, blank pad ( 1..12)  %l",
	"(%%m)                                        month (01..12)  %m",
	"(%%p)              locale's AM or PM based on 12-hour clock  %p",
	"(%%r)                   time, 12-hour (same as %%I:%%M:%%S %%p)  %r",
	"(%%u) ISO 8601: Weekday as decimal number [1 (Monday) - 7]   %u",
	"(%%v)                                VMS date (dd-bbb-YYYY)  %v",
	"(%%w)                       day of week (0..6, Sunday == 0)  %w",
	"(%%x)                appropriate locale date representation  %x",
	"(%%y)                      last two digits of year (00..99)  %y",
	"(%%z)      timezone offset east of GMT as HHMM (e.g. -0500)  %z",
	(char *) NULL
};

/* main routine. */

int
main(argc, argv)
int argc;
char **argv;
{
	char *next;
	char string[MAXTIME];

	int k;
	int length;

	struct tm *tm;

	time_t clock;

	/* Call the function. */

	clock = time(NULL);
	tm = localtime(&clock);

	for (k = 0; next = array[k]; k++) {
		length = strftime(string, MAXTIME, next, tm);
		printf("%s\n", string);
	}

	exit(0);
}
#endif	/* TEST_STRFTIME */
