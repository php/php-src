/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter.h
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.h is included in this package .
 *
 */

#ifndef MBFL_ENCODING_H
#define MBFL_ENCODING_H

#include "mbfl_defs.h"
#include "mbfl_consts.h"
#include "zend.h"

enum mbfl_no_encoding {
	mbfl_no_encoding_invalid = -1,
	mbfl_no_encoding_pass,
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_uuencode,
	mbfl_no_encoding_html_ent,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_7bit,
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_charset_min,
	mbfl_no_encoding_ucs4,
	mbfl_no_encoding_ucs4be,
	mbfl_no_encoding_ucs4le,
	mbfl_no_encoding_ucs2,
	mbfl_no_encoding_ucs2be,
	mbfl_no_encoding_ucs2le,
	mbfl_no_encoding_utf32,
	mbfl_no_encoding_utf32be,
	mbfl_no_encoding_utf32le,
	mbfl_no_encoding_utf16,
	mbfl_no_encoding_utf16be,
	mbfl_no_encoding_utf16le,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_utf8_docomo,
	mbfl_no_encoding_utf8_kddi_a,
	mbfl_no_encoding_utf8_kddi_b,
	mbfl_no_encoding_utf8_sb,
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_utf7imap,
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_eucjp2004,
	mbfl_no_encoding_sjis,
	mbfl_no_encoding_eucjp_win,
 	mbfl_no_encoding_sjis_docomo,
 	mbfl_no_encoding_sjis_kddi,
 	mbfl_no_encoding_sjis_sb,
 	mbfl_no_encoding_sjis_mac,
	mbfl_no_encoding_sjis2004,
	mbfl_no_encoding_cp932,
	mbfl_no_encoding_sjiswin,
	mbfl_no_encoding_cp51932,
	mbfl_no_encoding_jis,
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_2022jp_kddi,
	mbfl_no_encoding_2022jpms,
	mbfl_no_encoding_gb18030,
	mbfl_no_encoding_gb18030_2022,
	mbfl_no_encoding_cp1252,
	mbfl_no_encoding_cp1254,
	mbfl_no_encoding_8859_1,
	mbfl_no_encoding_8859_2,
	mbfl_no_encoding_8859_3,
	mbfl_no_encoding_8859_4,
	mbfl_no_encoding_8859_5,
	mbfl_no_encoding_8859_6,
	mbfl_no_encoding_8859_7,
	mbfl_no_encoding_8859_8,
	mbfl_no_encoding_8859_9,
	mbfl_no_encoding_8859_10,
	mbfl_no_encoding_8859_13,
	mbfl_no_encoding_8859_14,
	mbfl_no_encoding_8859_15,
	mbfl_no_encoding_euc_cn,
	mbfl_no_encoding_cp936,
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_big5,
	mbfl_no_encoding_cp950,
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_uhc,
	mbfl_no_encoding_hz,
	mbfl_no_encoding_cp1251,
	mbfl_no_encoding_cp866,
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_koi8u,
	mbfl_no_encoding_8859_16,
	mbfl_no_encoding_armscii8,
	mbfl_no_encoding_cp850,
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_charset_max
};

struct _mbfl_convert_filter;
struct mbfl_convert_vtbl {
	enum mbfl_no_encoding from;
	enum mbfl_no_encoding to;
	void (*filter_ctor)(struct _mbfl_convert_filter *filter);
	void (*filter_dtor)(struct _mbfl_convert_filter *filter);
	int (*filter_function)(int c, struct _mbfl_convert_filter *filter);
	int (*filter_flush)(struct _mbfl_convert_filter *filter);
	void (*filter_copy)(struct _mbfl_convert_filter *src, struct _mbfl_convert_filter *dest);
};

typedef struct {
	unsigned char *out;
	unsigned char *limit;
	uint32_t state;
	uint32_t errors;
	uint32_t replacement_char;
	unsigned int error_mode;
	zend_string *str;
} mb_convert_buf;

typedef size_t (*mb_to_wchar_fn)(unsigned char **in, size_t *in_len, uint32_t *out, size_t out_len, unsigned int *state);
typedef void (*mb_from_wchar_fn)(uint32_t *in, size_t in_len, mb_convert_buf *out, bool end);
typedef bool (*mb_check_fn)(unsigned char *in, size_t in_len);
typedef zend_string* (*mb_cut_fn)(unsigned char *str, size_t from, size_t len, unsigned char *end);

/* When converting encoded text to a buffer of wchars (Unicode codepoints) using `mb_to_wchar_fn`,
 * the buffer must be at least this size (to work with all supported text encodings) */
#define MBSTRING_MIN_WCHAR_BUFSIZE 5

static inline void mb_convert_buf_init(mb_convert_buf *buf, size_t initsize, uint32_t repl_char, unsigned int err_mode)
{
	buf->state = buf->errors = 0;
	buf->str = emalloc(_ZSTR_STRUCT_SIZE(initsize));
	buf->out = (unsigned char*)ZSTR_VAL(buf->str);
	buf->limit = buf->out + initsize;
	buf->replacement_char = repl_char;
	buf->error_mode = err_mode;
}

#define MB_CONVERT_BUF_ENSURE(buf, out, limit, needed) \
	ZEND_ASSERT(out <= limit); \
	if ((limit - out) < (needed)) { \
		size_t oldsize = limit - (unsigned char*)ZSTR_VAL((buf)->str); \
		size_t newsize = oldsize + MAX(oldsize >> 1, needed); \
		zend_string *newstr = erealloc((buf)->str, _ZSTR_STRUCT_SIZE(newsize)); \
		out = (unsigned char*)ZSTR_VAL(newstr) + (out - (unsigned char*)ZSTR_VAL((buf)->str)); \
		limit = (unsigned char*)ZSTR_VAL(newstr) + newsize; \
		(buf)->str = newstr; \
	}

#define MB_CONVERT_BUF_STORE(buf, _out, _limit) (buf)->out = _out; (buf)->limit = _limit

#define MB_CONVERT_BUF_LOAD(buf, _out, _limit) _out = (buf)->out; _limit = (buf)->limit

#define MB_CONVERT_ERROR(buf, out, limit, bad_cp, conv_fn) \
	MB_CONVERT_BUF_STORE(buf, out, limit); \
	mb_illegal_output(bad_cp, conv_fn, buf); \
	MB_CONVERT_BUF_LOAD(buf, out, limit)

static inline unsigned char* mb_convert_buf_add(unsigned char *out, char c)
{
	*out++ = c;
	return out;
}

static inline unsigned char* mb_convert_buf_add2(unsigned char *out, char c1, char c2)
{
	*out++ = c1;
	*out++ = c2;
	return out;
}

static inline unsigned char* mb_convert_buf_add3(unsigned char *out, char c1, char c2, char c3)
{
	*out++ = c1;
	*out++ = c2;
	*out++ = c3;
	return out;
}

static inline unsigned char* mb_convert_buf_add4(unsigned char *out, char c1, char c2, char c3, char c4)
{
	*out++ = c1;
	*out++ = c2;
	*out++ = c3;
	*out++ = c4;
	return out;
}

static inline unsigned char* mb_convert_buf_appends(unsigned char *out, const char *s)
{
	while (*s) {
		*out++ = *s++;
	}
	return out;
}

static inline unsigned char* mb_convert_buf_appendn(unsigned char *out, const char *s, size_t n)
{
	while (n--) {
		*out++ = *s++;
	}
	return out;
}

static inline zend_string* mb_convert_buf_result_raw(mb_convert_buf *buf)
{
	ZEND_ASSERT(buf->out <= buf->limit);
	zend_string *ret = buf->str;
	/* See `zend_string_alloc` in zend_string.h */
	GC_SET_REFCOUNT(ret, 1);
	GC_TYPE_INFO(ret) = GC_STRING;
	ZSTR_H(ret) = 0;
	ZSTR_LEN(ret) = buf->out - (unsigned char*)ZSTR_VAL(ret);
	*(buf->out) = '\0';
	return ret;
}

typedef struct {
	enum mbfl_no_encoding no_encoding;
	const char *name;
	const char *mime_name;
	const char **aliases;
	const unsigned char *mblen_table;
	unsigned int flag;
	const struct mbfl_convert_vtbl *input_filter;
	const struct mbfl_convert_vtbl *output_filter;
	mb_to_wchar_fn to_wchar;
	mb_from_wchar_fn from_wchar;
	mb_check_fn check;
	mb_cut_fn cut;
} mbfl_encoding;

extern const mbfl_encoding mbfl_encoding_utf8;

static inline zend_string* mb_convert_buf_result(mb_convert_buf *buf, const mbfl_encoding *enc)
{
	zend_string *ret = mb_convert_buf_result_raw(buf);
	if (enc == &mbfl_encoding_utf8 && buf->error_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8) {
		GC_ADD_FLAGS(ret, IS_STR_VALID_UTF8);
	}
	return ret;
}

/* Used if we initialize an `mb_convert_buf` but then discover we don't actually
 * want to return `zend_string` */
static inline void mb_convert_buf_free(mb_convert_buf *buf)
{
	efree(buf->str);
}

static inline size_t mb_convert_buf_len(mb_convert_buf *buf)
{
	return buf->out - (unsigned char*)ZSTR_VAL(buf->str);
}

static inline void mb_convert_buf_reset(mb_convert_buf *buf, size_t len)
{
	buf->out = (unsigned char*)ZSTR_VAL(buf->str) + len;
	ZEND_ASSERT(buf->out <= buf->limit);
}

MBFLAPI extern const mbfl_encoding *mbfl_name2encoding(const char *name);
MBFLAPI extern const mbfl_encoding *mbfl_name2encoding_ex(const char *name, size_t name_len);
MBFLAPI extern const mbfl_encoding *mbfl_no2encoding(enum mbfl_no_encoding no_encoding);
MBFLAPI extern const mbfl_encoding **mbfl_get_supported_encodings(void);
MBFLAPI extern const char *mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding);
MBFLAPI extern const char *mbfl_encoding_preferred_mime_name(const mbfl_encoding *encoding);

#endif /* MBFL_ENCODING_H */
