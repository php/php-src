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
 * The source code included in this file was separated from mbfilter.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "zend_bitset.h"
#include "mbfilter.h"
#include "mbfilter_utf16.h"

#ifdef ZEND_INTRIN_AVX2_NATIVE

/* We are building AVX2-only binary */
# include <immintrin.h>
# define mb_utf16be_to_wchar mb_utf16be_to_wchar_avx2
# define mb_utf16le_to_wchar mb_utf16le_to_wchar_avx2
# define mb_wchar_to_utf16be mb_wchar_to_utf16be_avx2
# define mb_wchar_to_utf16le mb_wchar_to_utf16le_avx2

static size_t mb_utf16be_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16be_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf16le_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16le_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

#elif defined(ZEND_INTRIN_AVX2_RESOLVER)

/* We are building binary which works with or without AVX2; whether or not to use
 * AVX2-accelerated functions will be determined at runtime */
# include <immintrin.h>
# include "Zend/zend_cpuinfo.h"

static size_t mb_utf16be_to_wchar_default(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16be_default(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf16le_to_wchar_default(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16le_default(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

# ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
/* Dynamic linker will decide whether or not to use AVX2-based functions and
 * resolve symbols accordingly */

ZEND_INTRIN_AVX2_FUNC_DECL(size_t mb_utf16be_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state));
ZEND_INTRIN_AVX2_FUNC_DECL(void mb_wchar_to_utf16be_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end));
ZEND_INTRIN_AVX2_FUNC_DECL(size_t mb_utf16le_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state));
ZEND_INTRIN_AVX2_FUNC_DECL(void mb_wchar_to_utf16le_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end));

size_t mb_utf16be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state) __attribute__((ifunc("resolve_utf16be_wchar")));
void mb_wchar_to_utf16be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end) __attribute__((ifunc("resolve_wchar_utf16be")));
size_t mb_utf16le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state) __attribute__((ifunc("resolve_utf16le_wchar")));
void mb_wchar_to_utf16le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end) __attribute__((ifunc("resolve_wchar_utf16le")));

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED
static mb_to_wchar_fn resolve_utf16be_wchar(void)
{
	return zend_cpu_supports_avx2() ? mb_utf16be_to_wchar_avx2 : mb_utf16be_to_wchar_default;
}

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED
static mb_from_wchar_fn resolve_wchar_utf16be(void)
{
	return zend_cpu_supports_avx2() ? mb_wchar_to_utf16be_avx2 : mb_wchar_to_utf16be_default;
}

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED
static mb_to_wchar_fn resolve_utf16le_wchar(void)
{
	return zend_cpu_supports_avx2() ? mb_utf16le_to_wchar_avx2 : mb_utf16le_to_wchar_default;
}

ZEND_NO_SANITIZE_ADDRESS
ZEND_ATTRIBUTE_UNUSED
static mb_from_wchar_fn resolve_wchar_utf16le(void)
{
	return zend_cpu_supports_avx2() ? mb_wchar_to_utf16le_avx2 : mb_wchar_to_utf16le_default;
}

# else /* ZEND_INTRIN_AVX2_FUNC_PTR */
/* We are compiling for a target where the dynamic linker will not be able to
 * resolve symbols according to whether the host supports AVX2 or not; so instead,
 * we can make calls go through a function pointer and set the function pointer
 * on module load */

#ifdef HAVE_FUNC_ATTRIBUTE_TARGET
static size_t mb_utf16be_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state) __attribute__((target("avx2")));
static void mb_wchar_to_utf16be_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end) __attribute__((target("avx2")));
static size_t mb_utf16le_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state) __attribute__((target("avx2")));
static void mb_wchar_to_utf16le_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end) __attribute__((target("avx2")));
#else
static size_t mb_utf16be_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16be_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf16le_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16le_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
#endif

static mb_to_wchar_fn utf16be_to_wchar_ptr = NULL;
static mb_from_wchar_fn wchar_to_utf16be_ptr = NULL;
static mb_to_wchar_fn utf16le_to_wchar_ptr = NULL;
static mb_from_wchar_fn wchar_to_utf16le_ptr = NULL;

static size_t mb_utf16be_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	return utf16be_to_wchar_ptr(in, in_len, buf, bufsize, NULL);
}

static void mb_wchar_to_utf16be(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	wchar_to_utf16be_ptr(in, len, buf, end);
}

static size_t mb_utf16le_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	return utf16le_to_wchar_ptr(in, in_len, buf, bufsize, NULL);
}

static void mb_wchar_to_utf16le(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	wchar_to_utf16le_ptr(in, len, buf, end);
}

void init_convert_utf16(void)
{
	if (zend_cpu_supports_avx2()) {
		utf16be_to_wchar_ptr = mb_utf16be_to_wchar_avx2;
		wchar_to_utf16be_ptr = mb_wchar_to_utf16be_avx2;
		utf16le_to_wchar_ptr = mb_utf16le_to_wchar_avx2;
		wchar_to_utf16le_ptr = mb_wchar_to_utf16le_avx2;
	} else {
		utf16be_to_wchar_ptr = mb_utf16be_to_wchar_default;
		wchar_to_utf16be_ptr = mb_wchar_to_utf16be_default;
		utf16le_to_wchar_ptr = mb_utf16le_to_wchar_default;
		wchar_to_utf16le_ptr = mb_wchar_to_utf16le_default;
	}
}
# endif

#else

/* No AVX2 support */
# define mb_utf16be_to_wchar mb_utf16be_to_wchar_default
# define mb_utf16le_to_wchar mb_utf16le_to_wchar_default
# define mb_wchar_to_utf16be mb_wchar_to_utf16be_default
# define mb_wchar_to_utf16le mb_wchar_to_utf16le_default

static size_t mb_utf16be_to_wchar_default(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16be_default(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static size_t mb_utf16le_to_wchar_default(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_utf16le_default(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

#endif

static int mbfl_filt_conv_utf16_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_utf16_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);

static const char *mbfl_encoding_utf16_aliases[] = {"utf16", NULL};

const mbfl_encoding mbfl_encoding_utf16 = {
	mbfl_no_encoding_utf16,
	"UTF-16",
	"UTF-16",
	mbfl_encoding_utf16_aliases,
	NULL,
	0,
	&vtbl_utf16_wchar,
	&vtbl_wchar_utf16,
	mb_utf16_to_wchar,
	mb_wchar_to_utf16be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_utf16be = {
	mbfl_no_encoding_utf16be,
	"UTF-16BE",
	"UTF-16BE",
	NULL,
	NULL,
	0,
	&vtbl_utf16be_wchar,
	&vtbl_wchar_utf16be,
	mb_utf16be_to_wchar,
	mb_wchar_to_utf16be,
	NULL,
	NULL,
};

const mbfl_encoding mbfl_encoding_utf16le = {
	mbfl_no_encoding_utf16le,
	"UTF-16LE",
	"UTF-16LE",
	NULL,
	NULL,
	0,
	&vtbl_utf16le_wchar,
	&vtbl_wchar_utf16le,
	mb_utf16le_to_wchar,
	mb_wchar_to_utf16le,
	NULL,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf16_wchar = {
	mbfl_no_encoding_utf16,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf16_wchar,
	mbfl_filt_conv_utf16_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf16be_wchar = {
	mbfl_no_encoding_utf16be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf16be_wchar,
	mbfl_filt_conv_utf16_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16be,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_utf16le_wchar = {
	mbfl_no_encoding_utf16le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_utf16le_wchar,
	mbfl_filt_conv_utf16_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_utf16le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16le,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_utf16le,
	mbfl_filt_conv_common_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_utf16_wchar(int c, mbfl_convert_filter *filter)
{
	/* Start with the assumption that the string is big-endian;
	 * If we find a little-endian BOM, then we will change that assumption */
	if (filter->status == 0) {
		filter->cache = c & 0xFF;
		filter->status = 1;
	} else {
		int n = (filter->cache << 8) | (c & 0xFF);
		filter->cache = filter->status = 0;
		if (n == 0xFFFE) {
			/* Switch to little-endian mode */
			filter->filter_function = mbfl_filt_conv_utf16le_wchar;
		} else {
			filter->filter_function = mbfl_filt_conv_utf16be_wchar;
			if (n >= 0xD800 && n <= 0xDBFF) {
				filter->cache = n & 0x3FF; /* Pick out 10 data bits */
				filter->status = 2;
				return 0;
			} else if (n >= 0xDC00 && n <= 0xDFFF) {
				/* This is wrong; second part of surrogate pair has come first */
				CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			} else if (n != 0xFEFF) {
				CK((*filter->output_function)(n, filter->data));
			}
		}
	}

	return 0;
}

int mbfl_filt_conv_utf16be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0: /* First byte */
		filter->cache = c & 0xFF;
		filter->status = 1;
		break;

	case 1: /* Second byte */
		n = (filter->cache << 8) | (c & 0xFF);
		if (n >= 0xD800 && n <= 0xDBFF) {
			filter->cache = n & 0x3FF; /* Pick out 10 data bits */
			filter->status = 2;
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			/* This is wrong; second part of surrogate pair has come first */
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else {
			filter->status = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;

	case 2: /* Second part of surrogate, first byte */
		filter->cache = (filter->cache << 8) | (c & 0xFF);
		filter->status = 3;
		break;

	case 3: /* Second part of surrogate, second byte */
		n = ((filter->cache & 0xFF) << 8) | (c & 0xFF);
		if (n >= 0xD800 && n <= 0xDBFF) {
			/* Wrong; that's the first half of a surrogate pair, not the second */
			filter->cache = n & 0x3FF;
			filter->status = 2;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			filter->status = 0;
			n = ((filter->cache & 0x3FF00) << 2) + (n & 0x3FF) + 0x10000;
			CK((*filter->output_function)(n, filter->data));
		} else {
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(n, filter->data));
		}
	}

	return 0;
}

int mbfl_filt_conv_wchar_utf16be(int c, mbfl_convert_filter *filter)
{
	int n;

	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		n = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(n & 0xff, filter->data));
		n = (c & 0x3ff) | 0xdc00;
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(n & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int mbfl_filt_conv_utf16le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0:
		filter->cache = c & 0xff;
		filter->status = 1;
		break;

	case 1:
		if ((c & 0xfc) == 0xd8) {
			/* Looks like we have a surrogate pair here */
			filter->cache += ((c & 0x3) << 8);
			filter->status = 2;
		} else if ((c & 0xfc) == 0xdc) {
			/* This is wrong; the second part of the surrogate pair has come first */
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else {
			filter->status = 0;
			CK((*filter->output_function)(filter->cache + ((c & 0xff) << 8), filter->data));
		}
		break;

	case 2:
		filter->cache = (filter->cache << 10) + (c & 0xff);
		filter->status = 3;
		break;

	case 3:
		n = (filter->cache & 0xFF) | ((c & 0xFF) << 8);
		if (n >= 0xD800 && n <= 0xDBFF) {
			/* We previously saw the first part of a surrogate pair and were
			 * expecting the second part; this is another first part */
			filter->cache = n & 0x3FF;
			filter->status = 2;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			n = filter->cache + ((c & 0x3) << 8) + 0x10000;
			filter->status = 0;
			CK((*filter->output_function)(n, filter->data));
		} else {
			/* The first part of a surrogate pair was followed by some other codepoint
			 * which is not part of a surrogate pair at all */
			filter->status = 0;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return 0;
}

int mbfl_filt_conv_wchar_utf16le(int c, mbfl_convert_filter *filter)
{
	int n;

	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		n = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->output_function)(n & 0xff, filter->data));
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		n = (c & 0x3ff) | 0xdc00;
		CK((*filter->output_function)(n & 0xff, filter->data));
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

static int mbfl_filt_conv_utf16_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status) {
		/* Input string was truncated */
		filter->status = 0;
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define DETECTED_BE 1
#define DETECTED_LE 2

static size_t mb_utf16_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	if (*state == DETECTED_BE) {
		return mb_utf16be_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*state == DETECTED_LE) {
		return mb_utf16le_to_wchar(in, in_len, buf, bufsize, NULL);
	} else if (*in_len >= 2) {
		unsigned char *p = *in;
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint16_t n = (c1 << 8) | c2;

		if (n == 0xFFFE) {
			/* Little-endian BOM */
			*in = p;
			*in_len -= 2;
			*state = DETECTED_LE;
			return mb_utf16le_to_wchar(in, in_len, buf, bufsize, NULL);
		} if (n == 0xFEFF) {
			/* Big-endian BOM; don't send to output */
			*in = p;
			*in_len -= 2;
		}
	}

	*state = DETECTED_BE;
	return mb_utf16be_to_wchar(in, in_len, buf, bufsize, NULL);
}

static size_t mb_utf16be_to_wchar_default(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	/* We only want to read 16-bit words out of `str`; any trailing byte will be handled at the end */
	unsigned char *p = *in, *e = p + (*in_len & ~1);
	/* Set `limit` to one less than the actual amount of space in the buffer; this is because
	 * on some iterations of the below loop, we might produce two output words */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint16_t n = (c1 << 8) | c2;

		if (n >= 0xD800 && n <= 0xDBFF) {
			/* Handle surrogate */
			if (p < e) {
				unsigned char c3 = *p++;
				unsigned char c4 = *p++;
				uint16_t n2 = (c3 << 8) | c4;

				if (n2 >= 0xD800 && n2 <= 0xDBFF) {
					/* Wrong; that's the first half of a surrogate pair, when we were expecting the second */
					*out++ = MBFL_BAD_INPUT;
					p -= 2;
				} else if (n2 >= 0xDC00 && n2 <= 0xDFFF) {
					*out++ = (((n & 0x3FF) << 10) | (n2 & 0x3FF)) + 0x10000;
				} else {
					/* The first half of a surrogate pair was followed by a 'normal' codepoint */
					*out++ = MBFL_BAD_INPUT;
					*out++ = n2;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			/* This is wrong; second part of surrogate pair has come first */
			*out++ = MBFL_BAD_INPUT;
		} else {
			*out++ = n;
		}
	}

	if (p == e && (*in_len & 0x1) && out < limit) {
		/* There is an extra trailing byte (which shouldn't be there) */
		*out++ = MBFL_BAD_INPUT;
		p++;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf16be_default(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;

		if (w < MBFL_WCSPLANE_UCS2MAX) {
			out = mb_convert_buf_add2(out, (w >> 8) & 0xFF, w & 0xFF);
		} else if (w < MBFL_WCSPLANE_UTF32MAX) {
			uint16_t n1 = ((w >> 10) - 0x40) | 0xD800;
			uint16_t n2 = (w & 0x3FF) | 0xDC00;
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
			out = mb_convert_buf_add4(out, (n1 >> 8) & 0xFF, n1 & 0xFF, (n2 >> 8) & 0xFF, n2 & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf16be_default);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static size_t mb_utf16le_to_wchar_default(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	/* We only want to read 16-bit words out of `str`; any trailing byte will be handled at the end */
	unsigned char *p = *in, *e = p + (*in_len & ~1);
	/* Set `limit` to one less than the actual amount of space in the buffer; this is because
	 * on some iterations of the below loop, we might produce two output words */
	uint32_t *out = buf, *limit = buf + bufsize - 1;

	while (p < e && out < limit) {
		unsigned char c1 = *p++;
		unsigned char c2 = *p++;
		uint16_t n = (c2 << 8) | c1;

		if (n >= 0xD800 && n <= 0xDBFF) {
			/* Handle surrogate */
			if (p < e) {
				unsigned char c3 = *p++;
				unsigned char c4 = *p++;
				uint16_t n2 = (c4 << 8) | c3;

				if (n2 >= 0xD800 && n2 <= 0xDBFF) {
					/* Wrong; that's the first half of a surrogate pair, when we were expecting the second */
					*out++ = MBFL_BAD_INPUT;
					p -= 2;
				} else if (n2 >= 0xDC00 && n2 <= 0xDFFF) {
					*out++ = (((n & 0x3FF) << 10) | (n2 & 0x3FF)) + 0x10000;
				} else {
					/* The first half of a surrogate pair was followed by a 'normal' codepoint */
					*out++ = MBFL_BAD_INPUT;
					*out++ = n2;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (n >= 0xDC00 && n <= 0xDFFF) {
			/* This is wrong; second part of surrogate pair has come first */
			*out++ = MBFL_BAD_INPUT;
		} else {
			*out++ = n;
		}
	}

	if (p == e && (*in_len & 0x1) && out < limit) {
		/* There is an extra trailing byte (which shouldn't be there) */
		*out++ = MBFL_BAD_INPUT;
		p++;
	}

	*in_len -= (p - *in);
	*in = p;
	return out - buf;
}

static void mb_wchar_to_utf16le_default(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;

		if (w < MBFL_WCSPLANE_UCS2MAX) {
			out = mb_convert_buf_add2(out, w & 0xFF, (w >> 8) & 0xFF);
		} else if (w < MBFL_WCSPLANE_UTF32MAX) {
			uint16_t n1 = ((w >> 10) - 0x40) | 0xD800;
			uint16_t n2 = (w & 0x3FF) | 0xDC00;
			MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
			out = mb_convert_buf_add4(out, n1 & 0xFF, (n1 >> 8) & 0xFF, n2 & 0xFF, (n2 >> 8) & 0xFF);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf16le_default);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

#if defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER)

#ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
size_t mb_utf16be_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
#else
static size_t mb_utf16be_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
#endif
{
	size_t len = *in_len;

	if (len >= 32 && bufsize >= 16) {
		unsigned char *p = *in;
		uint32_t *out = buf;

		/* Used to determine if a block of input bytes contains any surrogates */
		const __m256i _f8 = _mm256_set1_epi16(0xF8);
		const __m256i _d8 = _mm256_set1_epi16(0xD8);
		/* wchars must be in host byte order, which is little-endian on x86;
		 * Since we are reading in (big-endian) UTF-16BE, use this vector to swap byte order for output */
		const __m256i swap_bytes = _mm256_set_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1, 14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);

		do {
			__m256i operand = _mm256_loadu_si256((__m256i*)p); /* Load 32 bytes */

			uint32_t surrogate_bitvec = _mm256_movemask_epi8(_mm256_cmpeq_epi16(_mm256_and_si256(operand, _f8), _d8));
			if (surrogate_bitvec == 0) {
				/* There are no surrogates among these 16 characters
				 * So converting the UTF-16 input to wchars is very simple; just extend each 16-bit value
				 * to a 32-bit value, filling in zero bits in the high end */
				operand = _mm256_shuffle_epi8(operand, swap_bytes);
				_mm256_storeu_si256((__m256i*)out, _mm256_cvtepu16_epi32(_mm256_castsi256_si128(operand)));
				_mm256_storeu_si256((__m256i*)(out + 8), _mm256_cvtepu16_epi32(_mm256_extracti128_si256(operand, 1)));
				out += 16;
				bufsize -= 16;
				p += sizeof(__m256i);
				len -= sizeof(__m256i);
			} else if ((surrogate_bitvec & 1) == 0) {
				/* Some prefix of the current block is non-surrogates; output those */
				uint8_t n_chars = zend_ulong_ntz(surrogate_bitvec) >> 1;
				operand = _mm256_shuffle_epi8(operand, swap_bytes);
				/* We know that the output buffer has at least 64 bytes of space available
				 * So don't bother trimming the output down to only include the non-surrogate prefix;
				 * rather, write out an entire block of 64 (or 32) bytes, then bump our output pointer
				 * forward just past the 'good part', so the 'bad part' will be overwritten on the next
				 * iteration of this loop */
				_mm256_storeu_si256((__m256i*)out, _mm256_cvtepu16_epi32(_mm256_castsi256_si128(operand)));
				if (n_chars > 8) {
					_mm256_storeu_si256((__m256i*)(out + 8), _mm256_cvtepu16_epi32(_mm256_extracti128_si256(operand, 1)));
				}
				out += n_chars;
				bufsize -= n_chars;
				p += n_chars * 2;
				len -= n_chars * 2;
			} else {
				/* Some prefix of the current block is (valid or invalid) surrogates
				 * Handle those using non-vectorized code */
				surrogate_bitvec = ~surrogate_bitvec;
				unsigned int n_chars = surrogate_bitvec ? zend_ulong_ntz(surrogate_bitvec) >> 1 : 16;
				do {
					unsigned char c1 = *p++;
					unsigned char c2 = *p++;

					if (c1 & 0x4 || len < 4) {
						/* 2nd part of surrogate pair has come first OR string ended abruptly
						 * after 1st part of surrogate pair */
						*out++ = MBFL_BAD_INPUT;
						bufsize--;
						n_chars--;
						len -= 2;
						continue;
					}

					uint16_t n = (c1 << 8) | c2;
					unsigned char c3 = *p++;
					unsigned char c4 = *p++;

					if ((c3 & 0xFC) == 0xDC) {
						/* Valid surrogate pair */
						uint16_t n2 = (c3 << 8) | c4;
						*out++ = (((n & 0x3FF) << 10) | (n2 & 0x3FF)) + 0x10000;
						bufsize--;
						len -= 4;
#if defined(PHP_HAVE_BUILTIN_USUB_OVERFLOW) && PHP_HAVE_BUILTIN_USUB_OVERFLOW
						/* Subtracting 2 from `n_chars` will automatically set the CPU's flags;
						 * branch directly off the appropriate flag (CF on x86) rather than using
						 * another instruction (CMP on x86) to check for underflow */
						if (__builtin_usub_overflow(n_chars, 2, &n_chars)) {
							/* The last 2 bytes of this block and the first 2 bytes of the following
							 * block form a valid surrogate pair; now just make sure we don't get
							 * stuck in this loop due to underflow of the loop index */
							break;
						}
#else
						n_chars -= 2;
						if (n_chars == UINT_MAX) {
							break;
						}
#endif
					} else {
						/* First half of surrogate pair was followed by another first half
						 * OR by a non-surrogate character */
						*out++ = MBFL_BAD_INPUT;
						bufsize--;
						n_chars--;
						len -= 2;
						p -= 2; /* Back up so the last 2 bytes will be processed again */
					}
				} while (n_chars);
			}
		} while (len >= 32 && bufsize >= 16);

		if (len && bufsize >= 4) {
			/* Finish up trailing bytes which don't fill a 32-byte block */
			out += mb_utf16be_to_wchar_default(&p, &len, out, bufsize, NULL);
		}

		*in = p;
		*in_len = len;
		return out - buf;
	} else if (len) {
		return mb_utf16be_to_wchar_default(in, in_len, buf, bufsize, NULL);
	} else {
		return 0;
	}
}

#ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
void mb_wchar_to_utf16be_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
#else
static void mb_wchar_to_utf16be_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
#endif
{
	if (len >= 8) {
		unsigned char *out, *limit;
		MB_CONVERT_BUF_LOAD(buf, out, limit);
		MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

		/* Used to find wchars which are outside the Unicode BMP (Basic Multilingual Plane) */
		const __m256i bmp_mask = _mm256_set1_epi32(0xFFFF);
		/* Used to extract 16 bits which we want from each of eight 32-bit values */
		const __m256i pack_8x16 = _mm256_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 8, 9, 4, 5, 0, 1, 12, 13, 8, 9, 4, 5, 0, 1, -1, -1, -1, -1, -1, -1, -1, -1);

		do {
			__m256i operand = _mm256_loadu_si256((__m256i*)in); /* Load 32 bytes */

			uint32_t bmp_bitvec = _mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_and_si256(operand, bmp_mask), operand));
			if (bmp_bitvec == 0xFFFFFFFF) {
				/* All eight wchars are in the BMP
				 * Shuffle bytes around to get the 16 bytes we want into the low 16 bytes of YMM register
				 * (which is equivalent to an XMM register) */
				operand = _mm256_shuffle_epi8(operand, pack_8x16);
				__m256i operand2 = _mm256_permute2x128_si256(operand, operand, 1);
				operand = _mm256_alignr_epi8(operand2, operand, 8);
				_mm_storeu_si128((__m128i*)out, _mm256_castsi256_si128(operand)); /* Store 16 bytes */
				out += 16;
				len -= 8;
				in += 8;
			} else if (bmp_bitvec & 1) {
				/* Some prefix of this block are codepoints in the BMP */
				unsigned int n_bytes = zend_ulong_ntz(~bmp_bitvec);
				operand = _mm256_shuffle_epi8(operand, pack_8x16);
				__m256i operand2 = _mm256_permute2x128_si256(operand, operand, 1);
				operand = _mm256_alignr_epi8(operand2, operand, 8);
				/* Store 16 bytes, but bump output pointer forward just past the 'good part',
				 * so the 'bad part' will be overwritten on the next iteration of this loop */
				_mm_storeu_si128((__m128i*)out, _mm256_castsi256_si128(operand));
				out += n_bytes >> 1;
				len -= n_bytes >> 2;
				in += n_bytes >> 2;
			} else {
				/* Some prefix of this block is codepoints outside the BMP OR error markers
				 * Handle them using non-vectorized code */
				unsigned int n_words = bmp_bitvec ? zend_ulong_ntz(bmp_bitvec) >> 2 : 8;
				do {
					uint32_t w = *in++;
					n_words--;
					len--;

					if (w < MBFL_WCSPLANE_UTF32MAX) {
						uint16_t n1 = ((w >> 10) - 0x40) | 0xD800;
						uint16_t n2 = (w & 0x3FF) | 0xDC00;
						MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
						out = mb_convert_buf_add4(out, (n1 >> 8) & 0xFF, n1 & 0xFF, (n2 >> 8) & 0xFF, n2 & 0xFF);
					} else {
						MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf16be_default);
						MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
					}
				} while (n_words);
			}
		} while (len >= 8);

		MB_CONVERT_BUF_STORE(buf, out, limit);
	}

	if (len) {
		mb_wchar_to_utf16be_default(in, len, buf, end);
	}
}

#ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
size_t mb_utf16le_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
#else
static size_t mb_utf16le_to_wchar_avx2(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
#endif
{
	/* Most of this function is the same as `mb_utf16be_to_wchar_avx2`, above;
	 * See it for more detailed code comments */

	size_t len = *in_len;

	if (len >= 32 && bufsize >= 16) {
		unsigned char *p = *in;
		uint32_t *out = buf;

		const __m256i _f8 = _mm256_set1_epi16(0xF800);
		const __m256i _d8 = _mm256_set1_epi16(0xD800);

		do {
			__m256i operand = _mm256_loadu_si256((__m256i*)p);

			uint32_t surrogate_bitvec = _mm256_movemask_epi8(_mm256_cmpeq_epi16(_mm256_and_si256(operand, _f8), _d8));
			if (surrogate_bitvec == 0) {
				/* There are no surrogates among these 16 characters */
				_mm256_storeu_si256((__m256i*)out, _mm256_cvtepu16_epi32(_mm256_castsi256_si128(operand)));
				_mm256_storeu_si256((__m256i*)(out + 8), _mm256_cvtepu16_epi32(_mm256_extracti128_si256(operand, 1)));
				out += 16;
				bufsize -= 16;
				p += sizeof(__m256i);
				len -= sizeof(__m256i);
			} else if ((surrogate_bitvec & 1) == 0) {
				/* Some prefix of the current block is non-surrogates */
				uint8_t n_chars = zend_ulong_ntz(surrogate_bitvec) >> 1;
				_mm256_storeu_si256((__m256i*)out, _mm256_cvtepu16_epi32(_mm256_castsi256_si128(operand)));
				if (n_chars > 8) {
					_mm256_storeu_si256((__m256i*)(out + 8), _mm256_cvtepu16_epi32(_mm256_extracti128_si256(operand, 1)));
				}
				out += n_chars;
				bufsize -= n_chars;
				p += n_chars * 2;
				len -= n_chars * 2;
			} else {
				/* Some prefix of the current block is (valid or invalid) surrogates */
				surrogate_bitvec = ~surrogate_bitvec;
				unsigned int n_chars = surrogate_bitvec ? zend_ulong_ntz(surrogate_bitvec) >> 1 : 16;
				do {
					unsigned char c1 = *p++;
					unsigned char c2 = *p++;

					if (c2 & 0x4 || len < 4) {
						/* 2nd part of surrogate pair has come first OR string ended abruptly
						 * after 1st part of surrogate pair */
						*out++ = MBFL_BAD_INPUT;
						bufsize--;
						n_chars--;
						len -= 2;
						continue;
					}

					uint16_t n = (c2 << 8) | c1;
					unsigned char c3 = *p++;
					unsigned char c4 = *p++;

					if ((c4 & 0xFC) == 0xDC) {
						/* Valid surrogate pair */
						uint16_t n2 = (c4 << 8) | c3;
						*out++ = (((n & 0x3FF) << 10) | (n2 & 0x3FF)) + 0x10000;
						bufsize--;
						len -= 4;
#if defined(PHP_HAVE_BUILTIN_USUB_OVERFLOW) && PHP_HAVE_BUILTIN_USUB_OVERFLOW
						if (__builtin_usub_overflow(n_chars, 2, &n_chars)) {
							break;
						}
#else
						n_chars -= 2;
						if (n_chars == UINT_MAX) {
							break;
						}
#endif
					} else {
						/* First half of surrogate pair was followed by another first half
						 * OR by a non-surrogate character */
						*out++ = MBFL_BAD_INPUT;
						bufsize--;
						n_chars--;
						len -= 2;
						p -= 2; /* Back up so the last 2 bytes will be processed again */
					}
				} while (n_chars);
			}
		} while (len >= 32 && bufsize >= 16);

		if (len && bufsize >= 4) {
			out += mb_utf16le_to_wchar_default(&p, &len, out, bufsize, NULL);
		}

		*in = p;
		*in_len = len;
		return out - buf;
	} else if (len) {
		return mb_utf16le_to_wchar_default(in, in_len, buf, bufsize, NULL);
	} else {
		return 0;
	}
}

#ifdef ZEND_INTRIN_AVX2_FUNC_PROTO
void mb_wchar_to_utf16le_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
#else
static void mb_wchar_to_utf16le_avx2(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
#endif
{
	if (len >= 8) {
		unsigned char *out, *limit;
		MB_CONVERT_BUF_LOAD(buf, out, limit);
		MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

		/* Used to find wchars which are outside the Unicode BMP (Basic Multilingual Plane) */
		const __m256i bmp_mask = _mm256_set1_epi32(0xFFFF);
		/* Used to extract 16 bits which we want from each of eight 32-bit values */
		const __m256i pack_8x16 = _mm256_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 13, 12, 9, 8, 5, 4, 1, 0, 13, 12, 9, 8, 5, 4, 1, 0, -1, -1, -1, -1, -1, -1, -1, -1);

		do {
			__m256i operand = _mm256_loadu_si256((__m256i*)in);

			uint32_t bmp_bitvec = _mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_and_si256(operand, bmp_mask), operand));
			if (bmp_bitvec == 0xFFFFFFFF) {
				/* All eight wchars are in the BMP
				 * Shuffle bytes around to get the 16 bytes we want into the low 16 bytes of YMM register
				 * (which is equivalent to an XMM register) */
				operand = _mm256_shuffle_epi8(operand, pack_8x16);
				__m256i operand2 = _mm256_permute2x128_si256(operand, operand, 1);
				operand = _mm256_alignr_epi8(operand2, operand, 8);
				_mm_storeu_si128((__m128i*)out, _mm256_castsi256_si128(operand));
				out += 16;
				len -= 8;
				in += 8;
			} else if (bmp_bitvec & 1) {
				/* Some prefix of this block are codepoints in the BMP */
				unsigned int n_bytes = zend_ulong_ntz(~bmp_bitvec);
				operand = _mm256_shuffle_epi8(operand, pack_8x16);
				__m256i operand2 = _mm256_permute2x128_si256(operand, operand, 1);
				operand = _mm256_alignr_epi8(operand2, operand, 8);
				_mm_storeu_si128((__m128i*)out, _mm256_castsi256_si128(operand));
				out += n_bytes >> 1;
				len -= n_bytes >> 2;
				in += n_bytes >> 2;
			} else {
				/* Some prefix of this block is codepoints outside the BMP OR error markers */
				unsigned int n_words = bmp_bitvec ? zend_ulong_ntz(bmp_bitvec) >> 2 : 8;
				do {
					uint32_t w = *in++;
					n_words--;
					len--;

					if (w < MBFL_WCSPLANE_UTF32MAX) {
						uint16_t n1 = ((w >> 10) - 0x40) | 0xD800;
						uint16_t n2 = (w & 0x3FF) | 0xDC00;
						MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
						out = mb_convert_buf_add4(out, n1 & 0xFF, (n1 >> 8) & 0xFF, n2 & 0xFF, (n2 >> 8) & 0xFF);
					} else {
						MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_utf16le_default);
						MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
					}
				} while (n_words);
			}
		} while (len >= 8);

		MB_CONVERT_BUF_STORE(buf, out, limit);
	}

	if (len) {
		mb_wchar_to_utf16le_default(in, len, buf, end);
	}
}

#endif /* defined(ZEND_INTRIN_AVX2_NATIVE) || defined(ZEND_INTRIN_AVX2_RESOLVER) */
