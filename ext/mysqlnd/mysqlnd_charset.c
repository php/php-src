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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_charset.h"

#define ENUMERATE_ENCODINGS_CHARLEN(ENUMERATOR) \
	ENUMERATOR(mysqlnd_mbcharlen_null) \
	ENUMERATOR(mysqlnd_mbcharlen_big5) \
	ENUMERATOR(mysqlnd_mbcharlen_ujis) \
	ENUMERATOR(mysqlnd_mbcharlen_sjis) \
	ENUMERATOR(mysqlnd_mbcharlen_euckr) \
	ENUMERATOR(mysqlnd_mbcharlen_gb2312) \
	ENUMERATOR(mysqlnd_mbcharlen_gbk) \
	ENUMERATOR(mysqlnd_mbcharlen_utf8mb3) \
	ENUMERATOR(mysqlnd_mbcharlen_ucs2) \
	ENUMERATOR(mysqlnd_mbcharlen_eucjpms) \
	ENUMERATOR(mysqlnd_mbcharlen_utf8) \
	ENUMERATOR(mysqlnd_mbcharlen_utf16) \
	ENUMERATOR(mysqlnd_mbcharlen_utf32) \
	ENUMERATOR(mysqlnd_mbcharlen_cp932) \
	ENUMERATOR(mysqlnd_mbcharlen_gb18030)

#define ENUMERATE_ENCODINGS_VALID(ENUMERATOR) \
	ENUMERATOR(check_null) \
	ENUMERATOR(check_mb_big5) \
	ENUMERATOR(check_mb_ujis) \
	ENUMERATOR(check_mb_sjis) \
	ENUMERATOR(check_mb_euckr) \
	ENUMERATOR(check_mb_gb2312) \
	ENUMERATOR(check_mb_gbk) \
	ENUMERATOR(check_mb_utf8mb3_valid) \
	ENUMERATOR(check_mb_ucs2) \
	ENUMERATOR(check_mb_eucjpms) \
	ENUMERATOR(check_mb_utf8_valid) \
	ENUMERATOR(check_mb_utf16) \
	ENUMERATOR(check_mb_utf32) \
	ENUMERATOR(check_mb_cp932) \
	ENUMERATOR(my_ismbchar_gb18030)

#define LOWEST_MB_BIG5			0xA1
#define LOWEST_MB_UJIS			0x80
#define LOWEST_MB_SJIS			0x80
#define LOWEST_MB_EUCKR			0x80
#define LOWEST_MB_GB2312		0xA1
#define LOWEST_MB_GBK			0x80
#define LOWEST_MB_UTF8MB3		0x80
#define LOWEST_MB_UCS2			0x00
#define LOWEST_MB_EUCJPMS		0x80
#define LOWEST_MB_UTF8			0x80
#define LOWEST_MB_UTF16			0x00
#define LOWEST_MB_UTF32			0x00
#define LOWEST_MB_CP932			0x80
#define LOWEST_MB_GB18030		0x00

#define ENUMERATOR_ENUM(x) x##_id,
enum mysqlnd_encoding_charlen {
	ENUMERATE_ENCODINGS_CHARLEN(ENUMERATOR_ENUM)
};
enum mysqlnd_encoding_valid {
	ENUMERATE_ENCODINGS_VALID(ENUMERATOR_ENUM)
};
#undef ENUMERATOR_ENUM

static unsigned int mysqlnd_mbcharlen_dispatch(enum mysqlnd_encoding_charlen encoding, const unsigned int c);
static unsigned int mysqlnd_mbvalid_dispatch(enum mysqlnd_encoding_valid encoding, const char * const start, const char * const end);

/* {{{ utf8 functions */
static unsigned int check_mb_utf8mb3_sequence(const char * const start, const char * const end)
{
	zend_uchar	c;

	if (UNEXPECTED(start >= end)) {
		return 0;
	}

	c = (zend_uchar) start[0];

	if (c < 0x80) {
		return 1;		/* single byte character */
	}
	if (c < 0xC2) {
		return 0;		/* invalid mb character */
	}
	if (c < 0xE0) {
		if (start + 2 > end) {
			return 0;	/* too small */
		}
		if (!(((zend_uchar)start[1] ^ 0x80) < 0x40)) {
			return 0;
		}
		return 2;
	}
	if (c < 0xF0) {
		if (start + 3 > end) {
			return 0;	/* too small */
		}
		if (!(((zend_uchar)start[1] ^ 0x80) < 0x40 && ((zend_uchar)start[2] ^ 0x80) < 0x40 &&
			(c >= 0xE1 || (zend_uchar)start[1] >= 0xA0))) {
			return 0;	/* invalid utf8 character */
		}
		return 3;
	}
	return 0;
}


static unsigned int check_mb_utf8_sequence(const char * const start, const char * const end)
{
	zend_uchar	c;

	if (UNEXPECTED(start >= end)) {
		return 0;
	}

	c = (zend_uchar) start[0];

	if (c < 0x80) {
		return 1;		/* single byte character */
	}
	if (c < 0xC2) {
		return 0;		/* invalid mb character */
	}
	if (c < 0xE0) {
		if (start + 2 > end) {
			return 0;	/* too small */
		}
		if (!(((zend_uchar)start[1] ^ 0x80) < 0x40)) {
			return 0;
		}
		return 2;
	}
	if (c < 0xF0) {
		if (start + 3 > end) {
			return 0;	/* too small */
		}
		if (!(((zend_uchar)start[1] ^ 0x80) < 0x40 && ((zend_uchar)start[2] ^ 0x80) < 0x40 &&
			(c >= 0xE1 || (zend_uchar)start[1] >= 0xA0))) {
			return 0;	/* invalid utf8 character */
		}
		return 3;
	}
	if (c < 0xF5) {
		if (start + 4 > end) { /* We need 4 characters */
			return 0;	/* too small */
		}

		/*
		  UTF-8 quick four-byte mask:
		  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		  Encoding allows to encode U+00010000..U+001FFFFF

		  The maximum character defined in the Unicode standard is U+0010FFFF.
		  Higher characters U+00110000..U+001FFFFF are not used.

		  11110000.10010000.10xxxxxx.10xxxxxx == F0.90.80.80 == U+00010000 (min)
		  11110100.10001111.10111111.10111111 == F4.8F.BF.BF == U+0010FFFF (max)

		  Valid codes:
		  [F0][90..BF][80..BF][80..BF]
		  [F1][80..BF][80..BF][80..BF]
		  [F2][80..BF][80..BF][80..BF]
		  [F3][80..BF][80..BF][80..BF]
		  [F4][80..8F][80..BF][80..BF]
		*/

		if (!(((zend_uchar)start[1] ^ 0x80) < 0x40 &&
			((zend_uchar)start[2] ^ 0x80) < 0x40 &&
			((zend_uchar)start[3] ^ 0x80) < 0x40 &&
				(c >= 0xf1 || (zend_uchar)start[1] >= 0x90) &&
				(c <= 0xf3 || (zend_uchar)start[1] <= 0x8F)))
		{
			return 0;	/* invalid utf8 character */
		}
		return 4;
	}
	return 0;
}

static unsigned int check_mb_utf8mb3_valid(const char * const start, const char * const end)
{
	unsigned int len = check_mb_utf8mb3_sequence(start, end);
	return (len > 1)? len:0;
}

static unsigned int check_mb_utf8_valid(const char * const start, const char * const end)
{
	unsigned int len = check_mb_utf8_sequence(start, end);
	return (len > 1)? len:0;
}


static unsigned int mysqlnd_mbcharlen_utf8mb3(const unsigned int utf8)
{
	if (utf8 < 0x80) {
		return 1;		/* single byte character */
	}
	if (utf8 < 0xC2) {
		return 0;		/* invalid multibyte header */
	}
	if (utf8 < 0xE0) {
		return 2;		/* double byte character */
	}
	if (utf8 < 0xF0) {
		return 3;		/* triple byte character */
	}
	return 0;
}


static unsigned int mysqlnd_mbcharlen_utf8(const unsigned int utf8)
{
	if (utf8 < 0x80) {
		return 1;		/* single byte character */
	}
	if (utf8 < 0xC2) {
		return 0;		/* invalid multibyte header */
	}
	if (utf8 < 0xE0) {
		return 2;		/* double byte character */
	}
	if (utf8 < 0xF0) {
		return 3;		/* triple byte character */
	}
	if (utf8 < 0xF8) {
		return 4;		/* four byte character */
	}
	return 0;
}
/* }}} */


/* {{{ big5 functions */
#define valid_big5head(c)	(0xA1 <= (zend_uchar)(c) && (zend_uchar)(c) <= 0xF9)
#define valid_big5tail(c)	((0x40 <= (zend_uchar)(c) && (zend_uchar)(c) <= 0x7E) || \
							(0xA1 <= (zend_uchar)(c) && (zend_uchar)(c) <= 0xFE))

#define isbig5code(c,d) (isbig5head(c) && isbig5tail(d))

static unsigned int check_mb_big5(const char * const start, const char * const end)
{
	return (valid_big5head(*(start)) && (end - start) > 1 && valid_big5tail(*(start + 1)) ? 2 : 0);
}


static unsigned int mysqlnd_mbcharlen_big5(const unsigned int big5)
{
	return (valid_big5head(big5)) ? 2 : 1;
}
/* }}} */


/* {{{ cp932 functions */
#define valid_cp932head(c) ((0x81 <= (c) && (c) <= 0x9F) || (0xE0 <= (c) && c <= 0xFC))
#define valid_cp932tail(c) ((0x40 <= (c) && (c) <= 0x7E) || (0x80 <= (c) && c <= 0xFC))


static unsigned int check_mb_cp932(const char * const start, const char * const end)
{
	return (valid_cp932head((zend_uchar)start[0]) && (end - start >  1) &&
			valid_cp932tail((zend_uchar)start[1])) ? 2 : 0;
}


static unsigned int mysqlnd_mbcharlen_cp932(const unsigned int cp932)
{
	return (valid_cp932head((zend_uchar)cp932)) ? 2 : 1;
}
/* }}} */


/* {{{ euckr functions */
#define valid_euckr(c)	((0xA1 <= (zend_uchar)(c) && (zend_uchar)(c) <= 0xFE))

static unsigned int check_mb_euckr(const char * const start, const char * const end)
{
	if (end - start <= 1) {
		return 0;	/* invalid length */
	}
	if (*(zend_uchar *)start < 0x80) {
		return 0;	/* invalid euckr character */
	}
	if (valid_euckr(start[1])) {
		return 2;
	}
	return 0;
}


static unsigned int mysqlnd_mbcharlen_euckr(const unsigned int kr)
{
	return (valid_euckr(kr)) ? 2 : 1;
}
/* }}} */


/* {{{ eucjpms functions */
#define valid_eucjpms(c) 		(((c) & 0xFF) >= 0xA1 && ((c) & 0xFF) <= 0xFE)
#define valid_eucjpms_kata(c)	(((c) & 0xFF) >= 0xA1 && ((c) & 0xFF) <= 0xDF)
#define valid_eucjpms_ss2(c)	(((c) & 0xFF) == 0x8E)
#define valid_eucjpms_ss3(c)	(((c) & 0xFF) == 0x8F)

static unsigned int check_mb_eucjpms(const char * const start, const char * const end)
{
	if (*((zend_uchar *)start) < 0x80) {
		return 0;	/* invalid eucjpms character */
	}
	if (valid_eucjpms(start[0]) && (end - start) > 1 && valid_eucjpms(start[1])) {
		return 2;
	}
	if (valid_eucjpms_ss2(start[0]) && (end - start) > 1 && valid_eucjpms_kata(start[1])) {
		return 2;
	}
	if (valid_eucjpms_ss3(start[0]) && (end - start) > 2 && valid_eucjpms(start[1]) &&
		valid_eucjpms(start[2])) {
		return 3;
	}
	return 0;
}


static unsigned int mysqlnd_mbcharlen_eucjpms(const unsigned int jpms)
{
	if (valid_eucjpms(jpms) || valid_eucjpms_ss2(jpms)) {
		return 2;
	}
	if (valid_eucjpms_ss3(jpms)) {
		return 3;
	}
	return 1;
}
/* }}} */


/* {{{ gb2312 functions */
#define valid_gb2312_head(c)	(0xA1 <= (zend_uchar)(c) && (zend_uchar)(c) <= 0xF7)
#define valid_gb2312_tail(c)	(0xA1 <= (zend_uchar)(c) && (zend_uchar)(c) <= 0xFE)


static unsigned int check_mb_gb2312(const char * const start, const char * const end)
{
	return (valid_gb2312_head((unsigned int)start[0]) && end - start > 1 &&
			valid_gb2312_tail((unsigned int)start[1])) ? 2 : 0;
}


static unsigned int mysqlnd_mbcharlen_gb2312(const unsigned int gb)
{
	return (valid_gb2312_head(gb)) ? 2 : 1;
}
/* }}} */


/* {{{ gbk functions */
#define valid_gbk_head(c)	(0x81<=(zend_uchar)(c) && (zend_uchar)(c)<=0xFE)
#define valid_gbk_tail(c)	((0x40<=(zend_uchar)(c) && (zend_uchar)(c)<=0x7E) || (0x80<=(zend_uchar)(c) && (zend_uchar)(c)<=0xFE))

static unsigned int check_mb_gbk(const char * const start, const char * const end)
{
	return (valid_gbk_head(start[0]) && (end) - (start) > 1 && valid_gbk_tail(start[1])) ? 2 : 0;
}

static unsigned int mysqlnd_mbcharlen_gbk(const unsigned int gbk)
{
	return (valid_gbk_head(gbk) ? 2 : 1);
}
/* }}} */


/* {{{ sjis functions */
#define valid_sjis_head(c)	((0x81 <= (c) && (c) <= 0x9F) || (0xE0 <= (c) && (c) <= 0xFC))
#define valid_sjis_tail(c)	((0x40 <= (c) && (c) <= 0x7E) || (0x80 <= (c) && (c) <= 0xFC))


static unsigned int check_mb_sjis(const char * const start, const char * const end)
{
	return (valid_sjis_head((zend_uchar)start[0]) && (end - start) > 1 && valid_sjis_tail((zend_uchar)start[1])) ? 2 : 0;
}


static unsigned int mysqlnd_mbcharlen_sjis(const unsigned int sjis)
{
	return (valid_sjis_head((zend_uchar)sjis)) ? 2 : 1;
}
/* }}} */


/* {{{ ucs2 functions */
static unsigned int check_mb_ucs2(const char * const start __attribute((unused)), const char * const end __attribute((unused)))
{
	return 2; /* always 2 */
}

static unsigned int mysqlnd_mbcharlen_ucs2(const unsigned int ucs2 __attribute((unused)))
{
	return 2; /* always 2 */
}
/* }}} */


/* {{{ ujis functions */
#define valid_ujis(c)     	((0xA1 <= ((c)&0xFF) && ((c)&0xFF) <= 0xFE))
#define valid_ujis_kata(c)  ((0xA1 <= ((c)&0xFF) && ((c)&0xFF) <= 0xDF))
#define valid_ujis_ss2(c) 	(((c)&0xFF) == 0x8E)
#define valid_ujis_ss3(c) 	(((c)&0xFF) == 0x8F)

static unsigned int check_mb_ujis(const char * const start, const char * const end)
{
	if (*(zend_uchar*)start < 0x80) {
		return 0;	/* invalid ujis character */
	}
	if (valid_ujis(*(start)) && valid_ujis(*((start)+1))) {
		return 2;
	}
	if (valid_ujis_ss2(*(start)) && valid_ujis_kata(*((start)+1))) {
		return 2;
	}
	if (valid_ujis_ss3(*(start)) && (end-start) > 2 && valid_ujis(*((start)+1)) && valid_ujis(*((start)+2))) {
		return 3;
	}
	return 0;
}


static unsigned int mysqlnd_mbcharlen_ujis(const unsigned int ujis)
{
	return (valid_ujis(ujis)? 2: valid_ujis_ss2(ujis)? 2: valid_ujis_ss3(ujis)? 3: 1);
}
/* }}} */



/* {{{ utf16 functions */
#define UTF16_HIGH_HEAD(x)  ((((zend_uchar) (x)) & 0xFC) == 0xD8)
#define UTF16_LOW_HEAD(x)   ((((zend_uchar) (x)) & 0xFC) == 0xDC)

static unsigned int check_mb_utf16(const char * const start, const char * const end)
{
	if (start + 2 > end) {
		return 0;
	}

	if (UTF16_HIGH_HEAD(*start)) {
		return (start + 4 <= end) && UTF16_LOW_HEAD(start[2]) ? 4 : 0;
	}

	if (UTF16_LOW_HEAD(*start)) {
		return 0;
	}
	return 2;
}


static uint32_t mysqlnd_mbcharlen_utf16(const unsigned int utf16)
{
	return UTF16_HIGH_HEAD(utf16) ? 4 : 2;
}
/* }}} */


/* {{{ utf32 functions */
static unsigned int check_mb_utf32(const char * const start __attribute((unused)), const char * const end __attribute((unused)))
{
	return 4;
}


static unsigned int mysqlnd_mbcharlen_utf32(const unsigned int utf32 __attribute((unused)))
{
	return 4;
}
/* }}} */


/* {{{ gb18030 functions */
#define is_gb18030_odd(c)          (0x81 <= (zend_uchar) (c) && (zend_uchar) (c) <= 0xFE)
#define is_gb18030_even_2(c)       ((0x40 <= (zend_uchar) (c) && (zend_uchar) (c) <= 0x7E) || (0x80 <= (zend_uchar) (c) && (zend_uchar) (c) <= 0xFE))
#define is_gb18030_even_4(c)       (0x30 <= (zend_uchar) (c) && (zend_uchar) (c) <= 0x39)


static unsigned int mysqlnd_mbcharlen_gb18030(const unsigned int c)
{
	if (c <= 0xFF) {
		return !is_gb18030_odd(c);
	}
	if (c > 0xFFFF || !is_gb18030_odd((c >> 8) & 0xFF)) {
		return 0;
	}
	if (is_gb18030_even_2((c & 0xFF))) {
	    return 2;
	}
	if (is_gb18030_even_4((c & 0xFF))) {
		return 4;
	}

	return 0;
}


static unsigned int my_ismbchar_gb18030(const char * start, const char * end)
{
	if (end - start <= 1 || !is_gb18030_odd(start[0])) {
		return 0;
	}

	if (is_gb18030_even_2(start[1])) {
		return 2;
	} else if (end - start > 3 && is_gb18030_even_4(start[1]) && is_gb18030_odd(start[2]) && is_gb18030_even_4(start[3])) {
		return 4;
	}

	return 0;
}
/* }}} */

/*
  The server compiles sometimes the full utf-8 (the mb4) as utf8m4, and the old as utf8,
  for BC reasons. Sometimes, utf8mb4 is just utf8 but the old charsets are utf8mb3.
  Change easily now, with a macro, could be made compilastion dependable.
*/

#define UTF8_MB4 "utf8mb4"
#define UTF8_MB3 "utf8"

static zend_always_inline unsigned int mysqlnd_mbcharlen_null(const unsigned int c)
{
	ZEND_UNREACHABLE();
	return 0;
}

static zend_always_inline unsigned int check_null(const char * const start, const char * const end)
{
	ZEND_UNREACHABLE();
	return 0;
}

static unsigned int mysqlnd_mbcharlen_dispatch(enum mysqlnd_encoding_charlen encoding, const unsigned int c)
{
	switch (encoding) {
#define ENUMERATOR_DISPATCH(x) case x##_id: return x(c);
		ENUMERATE_ENCODINGS_CHARLEN(ENUMERATOR_DISPATCH)
#undef ENUMERATOR_DISPATCH
		default: return mysqlnd_mbcharlen_null(c);
	}
}

static unsigned int mysqlnd_mbvalid_dispatch(enum mysqlnd_encoding_valid encoding, const char * const start, const char * const end)
{
	switch (encoding) {
#define ENUMERATOR_DISPATCH(x) case x##_id: return x(start, end);
		ENUMERATE_ENCODINGS_VALID(ENUMERATOR_DISPATCH)
#undef ENUMERATOR_DISPATCH
		default: return check_null(start, end);
	}
}

/* {{{ mysqlnd_charsets */
const MYSQLND_CHARSET mysqlnd_charsets[] =
{
	{   1, "big5","big5_chinese_ci", 1, 2, "", mysqlnd_mbcharlen_big5_id, check_mb_big5_id, LOWEST_MB_BIG5},
	{   3, "dec8", "dec8_swedish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{   4, "cp850", "cp850_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{   6, "hp8", "hp8_english_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{   7, "koi8r", "koi8r_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{   8, "latin1", "latin1_swedish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{   5, "latin1", "latin1_german1_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100}, /* should be after 0x8 because swedish_ci is the default collation */
	{   9, "latin2", "latin2_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{   2, "latin2", "latin2_czech_cs", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100}, /* should be after 0x9 because general_ci is the default collation */
	{  10, "swe7", "swe7_swedish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  11, "ascii", "ascii_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  12, "ujis", "ujis_japanese_ci", 1, 3, "", mysqlnd_mbcharlen_ujis_id, check_mb_ujis_id, LOWEST_MB_UJIS},
	{  13, "sjis", "sjis_japanese_ci", 1, 2, "", mysqlnd_mbcharlen_sjis_id, check_mb_sjis_id, LOWEST_MB_SJIS},
	{  16, "hebrew", "hebrew_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  17, "filename", "filename", 1, 5, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  18, "tis620", "tis620_thai_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  19, "euckr", "euckr_korean_ci", 1, 2, "", mysqlnd_mbcharlen_euckr_id, check_mb_euckr_id, LOWEST_MB_EUCKR},
	{  21, "latin2", "latin2_hungarian_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  27, "latin2", "latin2_croatian_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  22, "koi8u", "koi8u_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  24, "gb2312", "gb2312_chinese_ci", 1, 2, "", mysqlnd_mbcharlen_gb2312_id, check_mb_gb2312_id, LOWEST_MB_GB2312},
	{  25, "greek", "greek_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  26, "cp1250", "cp1250_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  28, "gbk", "gbk_chinese_ci", 1, 2, "", mysqlnd_mbcharlen_gbk_id, check_mb_gbk_id, LOWEST_MB_GBK},
	{  30, "latin5", "latin5_turkish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  31, "latin1", "latin1_german2_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  15, "latin1", "latin1_danish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  32, "armscii8", "armscii8_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  33, UTF8_MB3, UTF8_MB3"_general_ci", 1, 3, "UTF-8 Unicode", mysqlnd_mbcharlen_utf8mb3_id,  check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{  35, "ucs2", "ucs2_general_ci", 2, 2, "UCS-2 Unicode", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{  36, "cp866", "cp866_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  37, "keybcs2", "keybcs2_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  38, "macce", "macce_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  39, "macroman", "macroman_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  40, "cp852", "cp852_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  41, "latin7", "latin7_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  20, "latin7", "latin7_estonian_cs", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  57, "cp1256", "cp1256_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  59, "cp1257", "cp1257_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  63, "binary", "binary", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  97, "eucjpms", "eucjpms_japanese_ci", 1, 3, "", mysqlnd_mbcharlen_eucjpms_id, check_mb_eucjpms_id, LOWEST_MB_EUCJPMS},
	{  29, "cp1257", "cp1257_lithuanian_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  31, "latin1", "latin1_german2_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  34, "cp1250", "cp1250_czech_cs", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  42, "latin7", "latin7_general_cs", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  43, "macce", "macce_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  44, "cp1250", "cp1250_croatian_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  45, UTF8_MB4, UTF8_MB4"_general_ci", 1, 4, "UTF-8 Unicode", mysqlnd_mbcharlen_utf8_id,  check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{  46, UTF8_MB4, UTF8_MB4"_bin", 1, 4, "UTF-8 Unicode", mysqlnd_mbcharlen_utf8_id,  check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{  47, "latin1", "latin1_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  48, "latin1", "latin1_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  49, "latin1", "latin1_general_cs", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  51, "cp1251", "cp1251_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  14, "cp1251", "cp1251_bulgarian_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  23, "cp1251", "cp1251_ukrainian_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  50, "cp1251", "cp1251_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  52, "cp1251", "cp1251_general_cs", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  53, "macroman", "macroman_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  54, "utf16", "utf16_general_ci", 2, 4, "UTF-16 Unicode", mysqlnd_mbcharlen_utf16_id, check_mb_utf16_id, LOWEST_MB_UTF16},
	{  55, "utf16", "utf16_bin", 2, 4, "UTF-16 Unicode", mysqlnd_mbcharlen_utf16_id, check_mb_utf16_id, LOWEST_MB_UTF16},
	{  56, "utf16le", "utf16le_general_ci", 2, 4, "UTF-16LE Unicode", mysqlnd_mbcharlen_utf16_id, check_mb_utf16_id, LOWEST_MB_UTF16},
	{  58, "cp1257", "cp1257_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
/*55*/{  60, "utf32", "utf32_general_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*55*/{  61, "utf32", "utf32_bin", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
	{  62, "utf16le", "utf16le_bin", 2, 4, "UTF-16LE Unicode", mysqlnd_mbcharlen_utf16_id, check_mb_utf16_id, LOWEST_MB_UTF16},
	{  64, "armscii8", "armscii8_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  65, "ascii", "ascii_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  66, "cp1250", "cp1250_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  67, "cp1256", "cp1256_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  68, "cp866", "cp866_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  69, "dec8", "dec8_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  70, "greek", "greek_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  71, "hebrew", "hebrew_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  72, "hp8", "hp8_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  73, "keybcs2", "keybcs2_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  74, "koi8r", "koi8r_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  75, "koi8u", "koi8u_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  77, "latin2", "latin2_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  78, "latin5", "latin5_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  79, "latin7", "latin7_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  80, "cp850", "cp850_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  81, "cp852", "cp852_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  82, "swe7", "swe7_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  83, UTF8_MB3, UTF8_MB3"_bin", 1, 3, "UTF-8 Unicode", mysqlnd_mbcharlen_utf8mb3_id,  check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{  84, "big5", "big5_bin", 1, 2, "", mysqlnd_mbcharlen_big5_id, check_mb_big5_id, LOWEST_MB_BIG5},
	{  85, "euckr", "euckr_bin", 1, 2, "", mysqlnd_mbcharlen_euckr_id, check_mb_euckr_id, LOWEST_MB_EUCKR},
	{  86, "gb2312", "gb2312_bin", 1, 2, "", mysqlnd_mbcharlen_gb2312_id, check_mb_gb2312_id, LOWEST_MB_GB2312},
	{  87, "gbk", "gbk_bin", 1, 2, "", mysqlnd_mbcharlen_gbk_id, check_mb_gbk_id, LOWEST_MB_GBK},
	{  88, "sjis", "sjis_bin", 1, 2, "", mysqlnd_mbcharlen_sjis_id, check_mb_sjis_id, LOWEST_MB_SJIS},
	{  89, "tis620", "tis620_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  90, "ucs2", "ucs2_bin", 2, 2, "UCS-2 Unicode", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{  91, "ujis", "ujis_bin", 1, 3, "", mysqlnd_mbcharlen_ujis_id, check_mb_ujis_id, LOWEST_MB_UJIS},
	{  92, "geostd8", "geostd8_general_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  93, "geostd8", "geostd8_bin", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  94, "latin1", "latin1_spanish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{  95, "cp932", "cp932_japanese_ci", 1, 2, "", mysqlnd_mbcharlen_cp932_id, check_mb_cp932_id, LOWEST_MB_CP932},
	{  96, "cp932", "cp932_bin", 1, 2, "", mysqlnd_mbcharlen_cp932_id, check_mb_cp932_id, LOWEST_MB_CP932},
	{  97, "eucjpms", "eucjpms_japanese_ci", 1, 3, "", mysqlnd_mbcharlen_eucjpms_id, check_mb_eucjpms_id, LOWEST_MB_EUCJPMS},
	{  98, "eucjpms", "eucjpms_bin", 1, 3, "", mysqlnd_mbcharlen_eucjpms_id, check_mb_eucjpms_id, LOWEST_MB_EUCJPMS},
	{  99, "cp1250", "cp1250_polish_ci", 1, 1, "", mysqlnd_mbcharlen_null_id, check_null_id, 0x100},
	{ 128, "ucs2", "ucs2_unicode_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 129, "ucs2", "ucs2_icelandic_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 130, "ucs2", "ucs2_latvian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 131, "ucs2", "ucs2_romanian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 132, "ucs2", "ucs2_slovenian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 133, "ucs2", "ucs2_polish_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 134, "ucs2", "ucs2_estonian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 135, "ucs2", "ucs2_spanish_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 136, "ucs2", "ucs2_swedish_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 137, "ucs2", "ucs2_turkish_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 138, "ucs2", "ucs2_czech_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 139, "ucs2", "ucs2_danish_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 140, "ucs2", "ucs2_lithuanian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 141, "ucs2", "ucs2_slovak_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 142, "ucs2", "ucs2_spanish2_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 143, "ucs2", "ucs2_roman_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 144, "ucs2", "ucs2_persian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 145, "ucs2", "ucs2_esperanto_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 146, "ucs2", "ucs2_hungarian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 147, "ucs2", "ucs2_sinhala_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 148, "ucs2", "ucs2_german2_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 149, "ucs2", "ucs2_croatian_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 150, "ucs2", "ucs2_unicode_520_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},
	{ 151, "ucs2", "ucs2_vietnamese_ci", 2, 2, "", mysqlnd_mbcharlen_ucs2_id, check_mb_ucs2_id, LOWEST_MB_UCS2},

/*56*/{160, "utf32", "utf32_unicode_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{161, "utf32", "utf32_icelandic_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{162, "utf32", "utf32_latvian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{163, "utf32", "utf32_romanian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{164, "utf32", "utf32_slovenian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{165, "utf32", "utf32_polish_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{166, "utf32", "utf32_estonian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{167, "utf32", "utf32_spanish_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{168, "utf32", "utf32_swedish_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{169, "utf32", "utf32_turkish_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{170, "utf32", "utf32_czech_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{171, "utf32", "utf32_danish_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{172, "utf32", "utf32_lithuanian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{173, "utf32", "utf32_slovak_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{174, "utf32", "utf32_spanish2_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{175, "utf32", "utf32_roman_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{176, "utf32", "utf32_persian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{177, "utf32", "utf32_esperanto_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{178, "utf32", "utf32_hungarian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{179, "utf32", "utf32_sinhala_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{180, "utf32", "utf32_german2_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{181, "utf32", "utf32_croatian_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{182, "utf32", "utf32_unicode_520_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},
/*56*/{183, "utf32", "utf32_vietnamese_ci", 4, 4, "UTF-32 Unicode", mysqlnd_mbcharlen_utf32_id, check_mb_utf32_id, LOWEST_MB_UTF32},

	{ 192, UTF8_MB3, UTF8_MB3"_unicode_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 193, UTF8_MB3, UTF8_MB3"_icelandic_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 194, UTF8_MB3, UTF8_MB3"_latvian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id,  check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 195, UTF8_MB3, UTF8_MB3"_romanian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 196, UTF8_MB3, UTF8_MB3"_slovenian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 197, UTF8_MB3, UTF8_MB3"_polish_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 198, UTF8_MB3, UTF8_MB3"_estonian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 199, UTF8_MB3, UTF8_MB3"_spanish_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 200, UTF8_MB3, UTF8_MB3"_swedish_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 201, UTF8_MB3, UTF8_MB3"_turkish_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 202, UTF8_MB3, UTF8_MB3"_czech_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 203, UTF8_MB3, UTF8_MB3"_danish_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 204, UTF8_MB3, UTF8_MB3"_lithuanian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 205, UTF8_MB3, UTF8_MB3"_slovak_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 206, UTF8_MB3, UTF8_MB3"_spanish2_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 207, UTF8_MB3, UTF8_MB3"_roman_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 208, UTF8_MB3, UTF8_MB3"_persian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 209, UTF8_MB3, UTF8_MB3"_esperanto_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 210, UTF8_MB3, UTF8_MB3"_hungarian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 211, UTF8_MB3, UTF8_MB3"_sinhala_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 212, UTF8_MB3, UTF8_MB3"_german2_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 213, UTF8_MB3, UTF8_MB3"_croatian_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 214, UTF8_MB3, UTF8_MB3"_unicode_520_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},
	{ 215, UTF8_MB3, UTF8_MB3"_vietnamese_ci", 1, 3, "", mysqlnd_mbcharlen_utf8mb3_id, check_mb_utf8mb3_valid_id, LOWEST_MB_UTF8MB3},

	{ 224, UTF8_MB4, UTF8_MB4"_unicode_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 225, UTF8_MB4, UTF8_MB4"_icelandic_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 226, UTF8_MB4, UTF8_MB4"_latvian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 227, UTF8_MB4, UTF8_MB4"_romanian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 228, UTF8_MB4, UTF8_MB4"_slovenian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 229, UTF8_MB4, UTF8_MB4"_polish_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 230, UTF8_MB4, UTF8_MB4"_estonian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 231, UTF8_MB4, UTF8_MB4"_spanish_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 232, UTF8_MB4, UTF8_MB4"_swedish_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 233, UTF8_MB4, UTF8_MB4"_turkish_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 234, UTF8_MB4, UTF8_MB4"_czech_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 235, UTF8_MB4, UTF8_MB4"_danish_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 236, UTF8_MB4, UTF8_MB4"_lithuanian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 237, UTF8_MB4, UTF8_MB4"_slovak_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 238, UTF8_MB4, UTF8_MB4"_spanish2_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 239, UTF8_MB4, UTF8_MB4"_roman_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 240, UTF8_MB4, UTF8_MB4"_persian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 241, UTF8_MB4, UTF8_MB4"_esperanto_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 242, UTF8_MB4, UTF8_MB4"_hungarian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 243, UTF8_MB4, UTF8_MB4"_sinhala_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 244, UTF8_MB4, UTF8_MB4"_german2_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 245, UTF8_MB4, UTF8_MB4"_croatian_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 246, UTF8_MB4, UTF8_MB4"_unicode_520_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 247, UTF8_MB4, UTF8_MB4"_vietnamese_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 248, "gb18030", "gb18030_chinese_ci", 1, 4, "", mysqlnd_mbcharlen_gb18030_id, my_ismbchar_gb18030_id, LOWEST_MB_GB18030},
	{ 249, "gb18030", "gb18030_bin", 1, 4, "", mysqlnd_mbcharlen_gb18030_id, my_ismbchar_gb18030_id, LOWEST_MB_GB18030},

	{ 254, UTF8_MB3, UTF8_MB3"_general_cs", 1, 3, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 255, UTF8_MB4, UTF8_MB4"_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 256, UTF8_MB4, UTF8_MB4"_de_pb_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 257, UTF8_MB4, UTF8_MB4"_is_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 258, UTF8_MB4, UTF8_MB4"_lv_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 259, UTF8_MB4, UTF8_MB4"_ro_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 260, UTF8_MB4, UTF8_MB4"_sl_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 261, UTF8_MB4, UTF8_MB4"_pl_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 262, UTF8_MB4, UTF8_MB4"_et_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 263, UTF8_MB4, UTF8_MB4"_es_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 264, UTF8_MB4, UTF8_MB4"_sv_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 265, UTF8_MB4, UTF8_MB4"_tr_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 266, UTF8_MB4, UTF8_MB4"_cs_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 267, UTF8_MB4, UTF8_MB4"_da_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 268, UTF8_MB4, UTF8_MB4"_lt_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 269, UTF8_MB4, UTF8_MB4"_sk_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 270, UTF8_MB4, UTF8_MB4"_es_trad_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 271, UTF8_MB4, UTF8_MB4"_la_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 272, UTF8_MB4, UTF8_MB4"_fa_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 273, UTF8_MB4, UTF8_MB4"_eo_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 274, UTF8_MB4, UTF8_MB4"_hu_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 275, UTF8_MB4, UTF8_MB4"_hr_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 276, UTF8_MB4, UTF8_MB4"_si_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 277, UTF8_MB4, UTF8_MB4"_vi_0900_ai_ci", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 278, UTF8_MB4, UTF8_MB4"_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 279, UTF8_MB4, UTF8_MB4"_de_pb_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 280, UTF8_MB4, UTF8_MB4"_is_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 281, UTF8_MB4, UTF8_MB4"_lv_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 282, UTF8_MB4, UTF8_MB4"_ro_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 283, UTF8_MB4, UTF8_MB4"_sl_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 284, UTF8_MB4, UTF8_MB4"_pl_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 285, UTF8_MB4, UTF8_MB4"_et_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 286, UTF8_MB4, UTF8_MB4"_es_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 287, UTF8_MB4, UTF8_MB4"_sv_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 288, UTF8_MB4, UTF8_MB4"_tr_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 289, UTF8_MB4, UTF8_MB4"_cs_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 290, UTF8_MB4, UTF8_MB4"_da_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 291, UTF8_MB4, UTF8_MB4"_lt_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 292, UTF8_MB4, UTF8_MB4"_sk_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 293, UTF8_MB4, UTF8_MB4"_es_trad_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 294, UTF8_MB4, UTF8_MB4"_la_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 295, UTF8_MB4, UTF8_MB4"_fa_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 296, UTF8_MB4, UTF8_MB4"_eo_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 297, UTF8_MB4, UTF8_MB4"_hu_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 298, UTF8_MB4, UTF8_MB4"_hr_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 299, UTF8_MB4, UTF8_MB4"_si_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 300, UTF8_MB4, UTF8_MB4"_vi_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{ 303, UTF8_MB4, UTF8_MB4"_ja_0900_as_cs", 1, 4, "", mysqlnd_mbcharlen_utf8_id, check_mb_utf8_valid_id, LOWEST_MB_UTF8},
	{   0, NULL, NULL, 0, 0, NULL, mysqlnd_mbcharlen_null_id, check_null_id, 0x100}
};
/* }}} */


/* {{{ mysqlnd_find_charset_nr */
PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_nr(const unsigned int charsetnr)
{
	const MYSQLND_CHARSET * c = mysqlnd_charsets;

	do {
		if (c->nr == charsetnr) {
			return c;
		}
		++c;
	} while (c[0].nr != 0);
	return NULL;
}
/* }}} */


/* {{{ mysqlnd_find_charset_name */
PHPAPI const MYSQLND_CHARSET * mysqlnd_find_charset_name(const char * const name)
{
	if (name) {
		const MYSQLND_CHARSET * c = mysqlnd_charsets;
		do {
			if (!strcasecmp(c->name, name)) {
				return c;
			}
			++c;
		} while (c[0].nr != 0);
	}
	return NULL;
}
/* }}} */

/* {{{ mysqlnd_cset_escape_quotes */
PHPAPI zend_ulong mysqlnd_cset_escape_quotes(const MYSQLND_CHARSET * const cset, char * newstr,
											 const char * escapestr, const size_t escapestr_len)
{
	const char 	*newstr_s = newstr;
	const char 	*newstr_e = newstr + 2 * escapestr_len;
	const char 	*end = escapestr + escapestr_len;

	DBG_ENTER("mysqlnd_cset_escape_quotes");

	for (;escapestr < end; escapestr++) {
		unsigned int len = 0;
		/* check unicode characters */

		if (*((zend_uchar *) escapestr) >= cset->lowest_mb_byte && (len = mysqlnd_mbvalid_dispatch(cset->mb_valid, escapestr, end))) {
			ZEND_ASSERT(newstr + len <= newstr_e);
			/* copy mb char without escaping it */
			while (len--) {
				*newstr++ = *escapestr++;
			}
			escapestr--;
			continue;
		}
		if (*escapestr == '\'') {
			ZEND_ASSERT(newstr + 2 <= newstr_e);
			*newstr++ = '\'';
			*newstr++ = '\'';
		} else {
			ZEND_ASSERT(newstr + 1 <= newstr_e);
			*newstr++ = *escapestr;
		}
	}
	*newstr = '\0';

	DBG_RETURN((zend_ulong)(newstr - newstr_s));
}
/* }}} */


/* {{{ mysqlnd_cset_escape_slashes */
PHPAPI zend_ulong mysqlnd_cset_escape_slashes(const MYSQLND_CHARSET * const cset, char *newstr,
											  const char * escapestr, const size_t escapestr_len)
{
	const char 	*newstr_s = newstr;
	const char 	*newstr_e = newstr + 2 * escapestr_len;
	const char 	*end = escapestr + escapestr_len;

	DBG_ENTER("mysqlnd_cset_escape_slashes");
	DBG_INF_FMT("charset=%s", cset->name);

	for (;escapestr < end; escapestr++) {
		char esc = '\0';

		/* check unicode characters */
		if (*((zend_uchar *) escapestr) >= cset->lowest_mb_byte) {
			unsigned int len = mysqlnd_mbvalid_dispatch(cset->mb_valid, escapestr, end);
			if (len) {
				ZEND_ASSERT(newstr + len <= newstr_e);
				/* copy mb char without escaping it */
				while (len--) {
					*newstr++ = *escapestr++;
				}
				escapestr--;
				continue;
			} else if (mysqlnd_mbcharlen_dispatch(cset->mb_charlen, *escapestr) > 1) {
				esc = *escapestr;
			}
		}
		if (!esc) {
			switch (*escapestr) {
				case 0:
					esc = '0';
					break;
				case '\n':
					esc = 'n';
					break;
				case '\r':
					esc = 'r';
					break;
				case '\\':
				case '\'':
				case '"':
					esc = *escapestr;
					break;
				case '\032':
					esc = 'Z';
					break;
			}
		}
		if (esc) {
			ZEND_ASSERT(newstr + 2 <= newstr_e);
			/* copy escaped character */
			*newstr++ = '\\';
			*newstr++ = esc;
		} else {
			ZEND_ASSERT(newstr + 1 <= newstr_e);
			/* copy non escaped character */
			*newstr++ = *escapestr;
		}
	}
	*newstr = '\0';

	DBG_RETURN((zend_ulong)(newstr - newstr_s));
}
/* }}} */


static struct st_mysqlnd_plugin_charsets mysqlnd_plugin_charsets_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"charsets",
		MYSQLND_VERSION_ID,
		PHP_MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@php.net>,  Ulf Wendel <uw@php.net>, Georg Richter <georg@php.net>",
		{
			NULL, /* no statistics , will be filled later if there are some */
			NULL, /* no statistics */
		},
		{
			NULL /* plugin shutdown */
		}
	},
	{/* methods */
		mysqlnd_find_charset_nr,
		mysqlnd_find_charset_name,
		mysqlnd_cset_escape_quotes,
		mysqlnd_cset_escape_slashes
	}
};


/* {{{ mysqlnd_charsets_plugin_register */
void
mysqlnd_charsets_plugin_register(void)
{
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_plugin_charsets_plugin);
}
/* }}} */

#if MYSQLND_CHARSETS_SANITY_CHECK
void mysqlnd_charsets_sanity_check(void)
{
	/* Sanity check fast path checks for encoding handling. */
	for (size_t i = 0; i < sizeof(mysqlnd_charsets) / sizeof(*mysqlnd_charsets); i++) {
		const MYSQLND_CHARSET *c = &mysqlnd_charsets[i];
		printf("charset nr %u\n", c->nr);

		/* Charset configuration consistency. */
		if (c->mb_valid != check_null_id) {
			ZEND_ASSERT(c->mb_valid != mysqlnd_mbcharlen_null_id);
		}
		if (c->mb_charlen != mysqlnd_mbcharlen_null_id) {
			ZEND_ASSERT(c->mb_charlen != check_null_id);
		}
		ZEND_ASSERT(c->char_minlen <= c->char_maxlen);
		if (c->char_maxlen == 1) {
			ZEND_ASSERT(c->lowest_mb_byte == 0x100);
		}
		if (c->char_minlen > 1) {
			ZEND_ASSERT(c->mb_valid != check_null_id);
			ZEND_ASSERT(c->mb_charlen != mysqlnd_mbcharlen_null_id);
			ZEND_ASSERT(c->lowest_mb_byte == 0x00);
		} else if (c->mb_valid != check_null_id) {
			/* Charset behavioural consistency w.r.t. charlen. */
			for (unsigned int ch = 0; ch < 255; ch++) {
				if (mysqlnd_mbcharlen_dispatch(c->mb_charlen, ch) > 1) {
					ZEND_ASSERT(ch >= c->lowest_mb_byte);
				}
			}

			/* Charset behavioural consistency w.r.t. validity. */
			union {
				zend_uchar c[4];
				uint32_t l;
			} input;
			for (uint32_t i = 0; i < 0xFFFFFFFF; i++) {
				input.l = i;
				if (mysqlnd_mbvalid_dispatch(c->mb_valid, (char *)input.c, (char *)input.c + 4) > 1) {
					if (input.c[0] < c->lowest_mb_byte) {
						printf("Input %u (%u %u)\n", input.l, input.c[0], c->lowest_mb_byte);
						ZEND_ASSERT(false);
					}
				}
			}
		}
	}
}
#endif
