#ifndef RUBY_TIMEV_H
#define RUBY_TIMEV_H

struct vtm {
    VALUE year; /* 2000 for example.  Integer. */
    int mon; /* 1..12 */
    int mday; /* 1..31 */
    int hour; /* 0..23 */
    int min; /* 0..59 */
    int sec; /* 0..60 */
    VALUE subsecx; /* 0 <= subsecx < TIME_SCALE.  possibly Rational. */
    VALUE utc_offset; /* -3600 as -01:00 for example.  possibly Rational. */
    int wday; /* 0:Sunday, 1:Monday, ..., 6:Saturday */
    int yday; /* 1..366 */
    int isdst; /* 0:StandardTime 1:DayLightSavingTime */
    const char *zone; /* "JST", "EST", "EDT", etc. */
};

#define TIME_SCALE 1000000000

#endif
