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
 * The source code included in this files was separated from mbfilter_ja.c
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#include "mbfilter.h"
#include "mbfilter_jis.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"

static int mbfl_filt_conv_jis_wchar_flush(mbfl_convert_filter *filter);
static size_t mb_iso2022jp_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_iso2022jp(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static void mb_wchar_to_jis(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static bool mb_check_iso2022jp(unsigned char *in, size_t in_len);
static bool mb_check_jis(unsigned char *in, size_t in_len);

const mbfl_encoding mbfl_encoding_jis = {
	mbfl_no_encoding_jis,
	"JIS",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_jis_wchar,
	&vtbl_wchar_jis,
	mb_iso2022jp_to_wchar,
	mb_wchar_to_jis,
	mb_check_jis
};

const mbfl_encoding mbfl_encoding_2022jp = {
	mbfl_no_encoding_2022jp,
	"ISO-2022-JP",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_wchar,
	&vtbl_wchar_2022jp,
	mb_iso2022jp_to_wchar,
	mb_wchar_to_iso2022jp,
	mb_check_iso2022jp
};

const struct mbfl_convert_vtbl vtbl_jis_wchar = {
	mbfl_no_encoding_jis,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis_wchar,
	mbfl_filt_conv_jis_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_jis = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_jis,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_2022jp_wchar = {
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis_wchar,
	mbfl_filt_conv_jis_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jp = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_2022jp,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * JIS => wchar
 */
int
mbfl_filt_conv_jis_wchar(int c, mbfl_convert_filter *filter)
{
	int c1, s, w;

retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x20:	 X 0201 kana */
/*	case 0x80:	 X 0208 */
/*	case 0x90:	 X 0212 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (c == 0x0e) {		/* "kana in" */
			filter->status = 0x20;
		} else if (c == 0x0f) {		/* "kana out" */
			filter->status = 0;
		} else if (filter->status == 0x10 && c == 0x5c) {	/* YEN SIGN */
			CK((*filter->output_function)(0xa5, filter->data));
		} else if (filter->status == 0x10 && c == 0x7e) {	/* OVER LINE */
			CK((*filter->output_function)(0x203e, filter->data));
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x60) {		/* kana */
			CK((*filter->output_function)(0xff40 + c, filter->data));
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data));
		} else if (c > 0xa0 && c < 0xe0) {	/* GR kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

/*	case 0x81:	 X 0208 second char */
/*	case 0x91:	 X 0212 second char */
	case 1:
		filter->status &= ~0xf;
		c1 = filter->cache;
		if (c > 0x20 && c < 0x7f) {
			s = (c1 - 0x21)*94 + c - 0x21;
			if (filter->status == 0x80) {
				if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else {
					w = 0;
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			} else {
				if (s >= 0 && s < jisx0212_ucs_table_size) {
					w = jisx0212_ucs_table[s];
				} else {
					w = 0;
				}

				if (w <= 0) {
					w = MBFL_BAD_INPUT;
				}
			}
			CK((*filter->output_function)(w, filter->data));
		} else {
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
		}
		break;

	/* ESC */
/*	case 0x02:	*/
/*	case 0x12:	*/
/*	case 0x22:	*/
/*	case 0x82:	*/
/*	case 0x92:	*/
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			goto retry;
		}
		break;

	/* ESC $ */
/*	case 0x03:	*/
/*	case 0x13:	*/
/*	case 0x23:	*/
/*	case 0x83:	*/
/*	case 0x93:	*/
	case 3:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {			/* '(' */
			filter->status++;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			goto retry;
		}
		break;

	/* ESC $ ( */
/*	case 0x04:	*/
/*	case 0x14:	*/
/*	case 0x24:	*/
/*	case 0x84:	*/
/*	case 0x94:	*/
	case 4:
		if (c == 0x40 || c == 0x42) {	/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x44) {			/* 'D' */
			filter->status = 0x90;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x24, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

	/* ESC ( */
/*	case 0x05:	*/
/*	case 0x15:	*/
/*	case 0x25:	*/
/*	case 0x85:	*/
/*	case 0x95:	*/
	case 5:
		if (c == 0x42 || c == 0x48) {		/* 'B' or 'H' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->status &= ~0xf;
			CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
			CK((*filter->output_function)(0x28, filter->data));
			goto retry;
		}
		break;

		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return 0;
}

static int mbfl_filt_conv_jis_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* 2-byte (JIS X 0208 or 0212) character was truncated,
		 * or else escape sequence was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

/*
 * wchar => JIS
 */
int
mbfl_filt_conv_wchar_jis(int c, mbfl_convert_filter *filter)
{
	int s = 0;

	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c == 0x203E) { /* OVERLINE */
		s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s <= 0) {
		if (c == 0xa5) {		/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}
	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x8080) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else if (s < 0x10000) { /* X 0212 */
			if ((filter->status & 0xff00) != 0x300) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x44, filter->data));		/* 'D' */
			}
			filter->status = 0x300;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}


/*
 * wchar => ISO-2022-JP
 */
int
mbfl_filt_conv_wchar_2022jp(int c, mbfl_convert_filter *filter)
{
	int s;

	s = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}

	if (s <= 0) {
		if (c == 0xa5) {			/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s = 0x224c;
		}
		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
		s = -1;
	}
	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data));
		} else if (s < 0x10000) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data));
			CK((*filter->output_function)(s & 0x7f, filter->data));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data));
		}
	} else {
		CK(mbfl_filt_conv_illegal_output(c, filter));
	}

	return 0;
}

int
mbfl_filt_conv_any_jis_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
	}
	filter->status = 0;

	if (filter->flush_function != NULL) {
		return (*filter->flush_function)(filter->data);
	}

	return 0;
}

#define ASCII 0
#define JISX_0201_LATIN 1
#define JISX_0201_KANA 2
#define JISX_0208 3
#define JISX_0212 4

static size_t mb_iso2022jp_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			/* ESC seen; this is an escape sequence */
			if ((e - p) < 2) {
				*out++ = MBFL_BAD_INPUT;
				if (p != e && (*p == '$' || *p == '('))
					p++;
				continue;
			}

			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					*state = JISX_0208;
				} else if (c3 == '(') {
					if (p == e) {
						*out++ = MBFL_BAD_INPUT;
						break;
					}
					unsigned char c4 = *p++;
					if (c4 == '@' || c4 == 'B') {
						*state = JISX_0208;
					} else if (c4 == 'D') {
						*state = JISX_0212;
					} else {
						if ((limit - out) < 3) {
							p -= 4;
							break;
						}
						*out++ = MBFL_BAD_INPUT;
						*out++ = '$';
						*out++ = '(';
						p--;
					}
				} else {
					if ((limit - out) < 2) {
						p -= 3;
						break;
					}
					*out++ = MBFL_BAD_INPUT;
					*out++ = '$';
					p--;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				if (c3 == 'B' || c3 == 'H') {
					*state = ASCII;
				} else if (c3 == 'J') {
					*state = JISX_0201_LATIN;
				} else if (c3 == 'I') {
					*state = JISX_0201_KANA;
				} else {
					if ((limit - out) < 2) {
						p -= 3;
						break;
					}
					*out++ = MBFL_BAD_INPUT;
					*out++ = '(';
					p--;
				}
			} else {
				*out++ = MBFL_BAD_INPUT;
				p--;
			}
		} else if (c == 0xE) {
			/* "Kana In" marker; this is just for JIS-7/8, but we also accept it for ISO-2022-JP */
			*state = JISX_0201_KANA;
		} else if (c == 0xF) {
			/* "Kana Out" marker */
			*state = ASCII;
		} else if (*state == JISX_0201_LATIN && c == 0x5C) { /* YEN SIGN */
			*out++ = 0xA5;
		} else if (*state == JISX_0201_LATIN && c == 0x7E) { /* OVER LINE */
			*out++ = 0x203E;
		} else if (*state == JISX_0201_KANA && c > 0x20 && c < 0x60) {
			*out++ = 0xFF40 + c;
		} else if (*state >= JISX_0208 && c > 0x20 && c < 0x7F) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				uint32_t w = 0;
				if (*state == JISX_0208) {
					if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					}
					if (!w) {
						w = MBFL_BAD_INPUT;
					}
				} else {
					if (s < jisx0212_ucs_table_size) {
						w = jisx0212_ucs_table[s];
					}
					if (!w) {
						w = MBFL_BAD_INPUT;
					}
				}
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* GR-invoked Kana; "GR" stands for "graphics right" and refers to bytes
			 * with the MSB bit (in the context of ISO-2022 encoding).
			 *
			 * In this regard, Wikipedia states:
			 * "Other, older variants known as JIS7 and JIS8 build directly on the 7-bit and 8-bit
			 * encodings defined by JIS X 0201 and allow use of JIS X 0201 kana from G1 without
			 * escape sequences, using Shift Out and Shift In or setting the eighth bit
			 * (GR-invoked), respectively."
			 *
			 * Note that we support both the 'JIS7' use of 0xE/0xF Shift In/Shift Out codes
			 * and the 'JIS8' use of GR-invoked Kana */
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static void mb_wchar_to_iso2022jp(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x1005C;
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w != 0) {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		} else if ((s >= 0x80 && s < 0x2121) || (s > 0x8080)) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp);
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
			continue;
		}

		if (s < 0x80) { /* ASCII */
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s < 0x8080) { /* JIS X 0208 */
			if (buf->state != JISX_0208) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else if (s < 0x10000) { /* JIS X 0212 */
			if (buf->state != JISX_0212) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'D');
				buf->state = JISX_0212;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else { /* X 0201 Latin */
			if (buf->state != JISX_0201_LATIN) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_jis(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = 0;

		if (w >= ucs_a1_jis_table_min && w < ucs_a1_jis_table_max) {
			s = ucs_a1_jis_table[w - ucs_a1_jis_table_min];
		} else if (w == 0x203E) { /* OVERLINE */
			s = 0x1007E; /* Convert to JISX 0201 OVERLINE */
		} else if (w >= ucs_a2_jis_table_min && w < ucs_a2_jis_table_max) {
			s = ucs_a2_jis_table[w - ucs_a2_jis_table_min];
		} else if (w >= ucs_i_jis_table_min && w < ucs_i_jis_table_max) {
			s = ucs_i_jis_table[w - ucs_i_jis_table_min];
		} else if (w >= ucs_r_jis_table_min && w < ucs_r_jis_table_max) {
			s = ucs_r_jis_table[w - ucs_r_jis_table_min];
		}

		if (s == 0) {
			if (w == 0xA5) { /* YEN SIGN */
				s = 0x1005C;
			} else if (w == 0xFF3C) { /* FULLWIDTH REVERSE SOLIDUS */
				s = 0x2140;
			} else if (w == 0x2225) { /* PARALLEL TO */
				s = 0x2142;
			} else if (w == 0xFF0D) { /* FULLWIDTH HYPHEN-MINUS */
				s = 0x215D;
			} else if (w == 0xFFE0) { /* FULLWIDTH CENT SIGN */
				s = 0x2171;
			} else if (w == 0xFFE1) { /* FULLWIDTH POUND SIGN */
				s = 0x2172;
			} else if (w == 0xFFE2) { /* FULLWIDTH NOT SIGN */
				s = 0x224C;
			} else if (w != 0) {
				MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_iso2022jp);
				MB_CONVERT_BUF_ENSURE(buf, out, limit, len * 2);
				continue;
			}
		}

		if (s < 0x80) { /* ASCII */
			if (buf->state != ASCII) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA1 && s <= 0xDF) {
			if (buf->state != JISX_0201_KANA) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else if (s < 0x8080) { /* JIS X 0208 */
			if (buf->state != JISX_0208) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 5);
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else if (s < 0x10000) { /* JIS X 0212 */
			if (buf->state != JISX_0212) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 6);
				out = mb_convert_buf_add4(out, 0x1B, '$', '(', 'D');
				buf->state = JISX_0212;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0x7F, s & 0x7F);
		} else { /* X 0201 Latin */
			if (buf->state != JISX_0201_LATIN) {
				MB_CONVERT_BUF_ENSURE(buf, out, limit, (len * 2) + 4);
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

#define JISX_0201_KANA_SO 5

static bool mb_check_jis(unsigned char *in, size_t in_len)
{
	unsigned char *p = in, *e = p + in_len;
	unsigned int state = ASCII;

	while (p < e) {
		unsigned char c = *p++;
		if (c == 0x1B) {
			/* ESC seen; this is an escape sequence */
			if (state == JISX_0201_KANA_SO) {
				return false;
			}
			if ((e - p) < 2) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					state = JISX_0208;
				} else if (c3 == '(') {
					if (p == e) {
						return false;
					}
					unsigned char c4 = *p++;
					if (c4 == '@' || c4 == 'B') {
						state = JISX_0208;
					} else if (c4 == 'D') {
						state = JISX_0212;
					} else {
						return false;
					}
				} else {
					return false;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				/* ESC ( H is treated as a sequence transitioning to ASCII for historical reasons.
				 * see https://github.com/php/php-src/pull/10828#issuecomment-1478342432. */
				if (c3 == 'B' || c3 == 'H') {
					state = ASCII;
				} else if (c3 == 'J') {
					state = JISX_0201_LATIN;
				} else if (c3 == 'I') {
					state = JISX_0201_KANA;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if (c == 0xE) {
			/* "Kana In" marker */
			if (state != ASCII) {
				return false;
			}
			state = JISX_0201_KANA_SO;
		} else if (c == 0xF) {
			/* "Kana Out" marker */
			if (state != JISX_0201_KANA_SO) {
				return false;
			}
			state = ASCII;
		} else if ((state == JISX_0208 || state == JISX_0212) && (c > 0x20 && c < 0x7F)) {
			if (p == e) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				if (state == JISX_0208) {
					if (s < jisx0208_ucs_table_size && jisx0208_ucs_table[s]) {
						continue;
					}
				} else {
					if (s < jisx0212_ucs_table_size && jisx0212_ucs_table[s]) {
						continue;
					}
				}
				return false;
			} else {
				return false;
			}
		} else if (c < 0x80) {
			continue;
		} else if (c >= 0xA1 && c <= 0xDF) {
			/* GR-invoked Kana */
			continue;
		} else {
			return false;
		}
	}

	return state == ASCII;
}


static bool mb_check_iso2022jp(unsigned char *in, size_t in_len)
{
	unsigned char *p = in, *e = p + in_len;
	unsigned int state = ASCII;

	while (p < e) {
		unsigned char c = *p++;
		if (c == 0x1B) {
			/* ESC seen; this is an escape sequence */
			if ((e - p) < 2) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 == '$') {
				unsigned char c3 = *p++;
				if (c3 == '@' || c3 == 'B') {
					state = JISX_0208;
				} else {
					return false;
				}
			} else if (c2 == '(') {
				unsigned char c3 = *p++;
				if (c3 == 'B') {
					state = ASCII;
				} else if (c3 == 'J') {
					state = JISX_0201_LATIN;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if (c == 0xE || c == 0xF) {
			/* "Kana In" or "Kana Out" marker; ISO-2022-JP is not accepted. */
			return false;
		} else if (state == JISX_0208 && (c > 0x20 && c < 0x7F)) {
			if (p == e) {
				return false;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				if (s < jisx0208_ucs_table_size && jisx0208_ucs_table[s]) {
					continue;
				}
				return false;
			} else {
				return false;
			}
		} else if (c < 0x80) {
			continue;
		} else {
			return false;
		}
	}

	return state == ASCII;
}
