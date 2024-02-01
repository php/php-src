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
 * The author of this file: Moriyoshi Koizumi <koizumi@gree.co.jp>
 *
 */

#include "mbfilter.h"
#include "mbfilter_cp5022x.h"
#include "mbfilter_jis.h"

#include "unicode_table_cp932_ext.h"
#include "unicode_table_jis.h"
#include "cp932_table.h"
#include "translit_kana_jisx0201_jisx0208.h"

static int mbfl_filt_conv_cp5022x_wchar_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_cp50220_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_cp50222_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_cp50220(int c, mbfl_convert_filter *filter);
static size_t mb_cp5022x_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state);
static void mb_wchar_to_cp50220(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static void mb_wchar_to_cp50221(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);
static void mb_wchar_to_cp50222(uint32_t *in, size_t len, mb_convert_buf *buf, bool end);

/* Previously, a dubious 'encoding' called 'cp50220raw' was supported
 * This was just CP50220, but the implementation was less strict regarding
 * invalid characters; it would silently pass some through
 * This 'encoding' only existed in mbstring. In case some poor, lost soul is
 * still using it, retain minimal support by aliasing it to CP50220
 *
 * Further, mbstring also had a made-up encoding called "JIS-ms"
 * This was the same as CP5022{0,1,2}, but without their special ways of
 * handling conversion of Unicode half-width katakana */
static const char *cp50220_aliases[] = {"cp50220raw", "cp50220-raw", "JIS-ms", NULL};

const mbfl_encoding mbfl_encoding_cp50220 = {
	mbfl_no_encoding_cp50220,
	"CP50220",
	"ISO-2022-JP",
	cp50220_aliases,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50220_wchar,
	&vtbl_wchar_cp50220,
	mb_cp5022x_to_wchar,
	mb_wchar_to_cp50220,
	NULL
};

const mbfl_encoding mbfl_encoding_cp50221 = {
	mbfl_no_encoding_cp50221,
	"CP50221",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50221_wchar,
	&vtbl_wchar_cp50221,
	mb_cp5022x_to_wchar,
	mb_wchar_to_cp50221,
	NULL
};

const mbfl_encoding mbfl_encoding_cp50222 = {
	mbfl_no_encoding_cp50222,
	"CP50222",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_cp50222_wchar,
	&vtbl_wchar_cp50222,
	mb_cp5022x_to_wchar,
	mb_wchar_to_cp50222,
	NULL
};

const struct mbfl_convert_vtbl vtbl_cp50220_wchar = {
	mbfl_no_encoding_cp50220,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50220 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50220,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50220,
	mbfl_filt_conv_wchar_cp50220_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_cp50221_wchar = {
	mbfl_no_encoding_cp50221,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50221 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50221,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50221,
	mbfl_filt_conv_any_jis_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_cp50222_wchar = {
	mbfl_no_encoding_cp50222,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_cp5022x_wchar,
	mbfl_filt_conv_cp5022x_wchar_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_cp50222 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_cp50222,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_cp50222,
	mbfl_filt_conv_wchar_cp50222_flush,
	NULL,
};

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

int mbfl_filt_conv_cp5022x_wchar(int c, mbfl_convert_filter *filter)
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
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c <= 0x97) { /* kanji first char */
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
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= 94 * 94 && s < 114 * 94) {
					/* user-defined => PUA (Microsoft extended) */
					w = s - 94*94 + 0xe000;
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

static int mbfl_filt_conv_cp5022x_wchar_flush(mbfl_convert_filter *filter)
{
	if (filter->status & 0xF) {
		/* 2-byte (JIS X 0208 or 0212) character was truncated, or else
		 * escape sequence was truncated */
		CK((*filter->output_function)(MBFL_BAD_INPUT, filter->data));
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

/* Apply various transforms to input codepoint, such as converting halfwidth katakana
 * to fullwidth katakana. `mode` is a bitfield which controls which transforms are
 * actually performed. The bit values are defined in translit_kana_jisx0201_jisx0208.h.
 * `mode` must not call for transforms which are inverses (i.e. which would cancel
 * each other out).
 *
 * In some cases, successive input codepoints may be merged into one output codepoint.
 * (That is the purpose of the `next` parameter.) If the `next` codepoint is consumed
 * and should be skipped over, `*consumed` will be set to true. Otherwise, `*consumed`
 * will not be modified. If there is no following codepoint, `next` should be zero.
 *
 * Again, in some cases, one input codepoint may convert to two output codepoints.
 * If so, the second output codepoint will be stored in `*second`.
 *
 * Return the resulting codepoint. If none of the requested transforms apply, return
 * the input codepoint unchanged.
 */
uint32_t mb_convert_kana_codepoint(uint32_t c, uint32_t next, bool *consumed, uint32_t *second, unsigned int mode)
{
	if ((mode & MBFL_HAN2ZEN_ALL) && c >= 0x21 && c <= 0x7D && c != '"' && c != '\'' && c != '\\') {
		return c + 0xFEE0;
	}
	if ((mode & MBFL_HAN2ZEN_ALPHA) && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
		return c + 0xFEE0;
	}
	if ((mode & MBFL_HAN2ZEN_NUMERIC) && c >= '0' && c <= '9') {
		return c + 0xFEE0;
	}
	if ((mode & MBFL_HAN2ZEN_SPACE) && c == ' ') {
		return 0x3000;
	}

	if (mode & (MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_HIRAGANA)) {
		/* Convert Hankaku kana to Zenkaku kana
		 * Either all Hankaku kana (including katakana and hiragana) will be converted
		 * to Zenkaku katakana, or to Zenkaku hiragana */
		if ((mode & MBFL_HAN2ZEN_KATAKANA) && (mode & MBFL_HAN2ZEN_GLUE)) {
			if (c >= 0xFF61 && c <= 0xFF9F) {
				int n = c - 0xFF60;

				if (next >= 0xFF61 && next <= 0xFF9F) {
					if (next == 0xFF9E && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						*consumed = true;
						return 0x3001 + hankana2zenkana_table[n];
					}
					if (next == 0xFF9E && n == 19) {
						*consumed = true;
						return 0x30F4;
					}
					if (next == 0xFF9F && n >= 42 && n <= 46) {
						*consumed = true;
						return 0x3002 + hankana2zenkana_table[n];
					}
				}

				return 0x3000 + hankana2zenkana_table[n];
			}
		}
		if ((mode & MBFL_HAN2ZEN_HIRAGANA) && (mode & MBFL_HAN2ZEN_GLUE)) {
			if (c >= 0xFF61 && c <= 0xFF9F) {
				int n = c - 0xFF60;

				if (next >= 0xFF61 && next <= 0xFF9F) {
					if (next == 0xFF9E && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						*consumed = true;
						return 0x3001 + hankana2zenhira_table[n];
					}
					if (next == 0xFF9F && n >= 42 && n <= 46) {
						*consumed = true;
						return 0x3002 + hankana2zenhira_table[n];
					}
				}

				return 0x3000 + hankana2zenhira_table[n];
			}
		}
		if ((mode & MBFL_HAN2ZEN_KATAKANA) && c >= 0xFF61 && c <= 0xFF9F) {
			return 0x3000 + hankana2zenkana_table[c - 0xFF60];
		}
		if ((mode & MBFL_HAN2ZEN_HIRAGANA) && c >= 0xFF61 && c <= 0xFF9F) {
			return 0x3000 + hankana2zenhira_table[c - 0xFF60];
		}
	}

	if (mode & MBFL_HAN2ZEN_SPECIAL) { /* special ascii to symbol */
		if (c == '\\' || c == 0xA5) { /* YEN SIGN */
			return 0xFFE5; /* FULLWIDTH YEN SIGN */
		}
		if (c == 0x7E || c == 0x203E) {
			return 0xFFE3; /* FULLWIDTH MACRON */
		}
		if (c == '\'') {
			return 0x2019; /* RIGHT SINGLE QUOTATION MARK */
		}
		if (c == '"') {
			return 0x201D; /* RIGHT DOUBLE QUOTATION MARK */
		}
	}

	if (mode & (MBFL_ZEN2HAN_ALL | MBFL_ZEN2HAN_ALPHA | MBFL_ZEN2HAN_NUMERIC | MBFL_ZEN2HAN_SPACE)) {
		/* Zenkaku to Hankaku */
		if ((mode & MBFL_ZEN2HAN_ALL) && c >= 0xFF01 && c <= 0xFF5D && c != 0xFF02 && c != 0xFF07 && c != 0xFF3C) {
			/* all except " ' \ ~ */
			return c - 0xFEE0;
		}
		if ((mode & MBFL_ZEN2HAN_ALPHA) && ((c >= 0xFF21 && c <= 0xFF3A) || (c >= 0xFF41 && c <= 0xFF5A))) {
			return c - 0xFEE0;
		}
		if ((mode & MBFL_ZEN2HAN_NUMERIC) && (c >= 0xFF10 && c <= 0xFF19)) {
			return c - 0xFEE0;
		}
		if ((mode & MBFL_ZEN2HAN_SPACE) && (c == 0x3000)) {
			return ' ';
		}
		if ((mode & MBFL_ZEN2HAN_ALL) && (c == 0x2212)) { /* MINUS SIGN */
			return '-';
		}
	}

	if (mode & (MBFL_ZEN2HAN_KATAKANA | MBFL_ZEN2HAN_HIRAGANA)) {
		/* Zenkaku kana to hankaku kana */
		if ((mode & MBFL_ZEN2HAN_KATAKANA) && c >= 0x30A1 && c <= 0x30F4) {
			/* Zenkaku katakana to hankaku kana */
			int n = c - 0x30A1;
			if (zenkana2hankana_table[n][1]) {
				*second = 0xFF00 + zenkana2hankana_table[n][1];
			}
			return 0xFF00 + zenkana2hankana_table[n][0];
		}
		if ((mode & MBFL_ZEN2HAN_HIRAGANA) && c >= 0x3041 && c <= 0x3093) {
			/* Zenkaku hiragana to hankaku kana */
			int n = c - 0x3041;
			if (zenkana2hankana_table[n][1]) {
				*second = 0xFF00 + zenkana2hankana_table[n][1];
			}
			return 0xFF00 + zenkana2hankana_table[n][0];
		}
		if (c == 0x3001) {
			return 0xFF64; /* HALFWIDTH IDEOGRAPHIC COMMA */
		}
		if (c == 0x3002) {
			return 0xFF61; /* HALFWIDTH IDEOGRAPHIC FULL STOP */
		}
		if (c == 0x300C) {
			return 0xFF62; /* HALFWIDTH LEFT CORNER BRACKET */
		}
		if (c == 0x300D) {
			return 0xFF63; /* HALFWIDTH RIGHT CORNER BRACKET */
		}
		if (c == 0x309B) {
			return 0xFF9E; /* HALFWIDTH KATAKANA VOICED SOUND MARK */
		}
		if (c == 0x309C) {
			return 0xff9f; /* HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK */
		}
		if (c == 0x30FC) {
			return 0xFF70; /* HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK */
		}
		if (c == 0x30FB) {
			return 0xFF65; /* HALFWIDTH KATAKANA MIDDLE DOT */
		}
	}

	if (mode & (MBFL_ZENKAKU_HIRA2KATA | MBFL_ZENKAKU_KATA2HIRA)) {
		if ((mode & MBFL_ZENKAKU_HIRA2KATA) && ((c >= 0x3041 && c <= 0x3093) || c == 0x309D || c == 0x309E)) {
			/* Zenkaku hiragana to Zenkaku katakana */
			return c + 0x60;
		}
		if ((mode & MBFL_ZENKAKU_KATA2HIRA) && ((c >= 0x30A1 && c <= 0x30F3) || c == 0x30FD || c == 0x30FE)) {
			/* Zenkaku katakana to Zenkaku hiragana */
			return c - 0x60;
		}
	}

	if (mode & MBFL_ZEN2HAN_SPECIAL) { /* special symbol to ascii */
		if (c == 0xFFE5 || c == 0xFF3C) { /* FULLWIDTH YEN SIGN/FULLWIDTH REVERSE SOLIDUS */
			return '\\';
		}
		if (c == 0xFFE3 || c == 0x203E) { /* FULLWIDTH MACRON/OVERLINE */
			return '~';
		}
		if (c == 0x2018 || c == 0x2019) { /* LEFT/RIGHT SINGLE QUOTATION MARK*/
			return '\'';
		}
		if (c == 0x201C || c == 0x201D) { /* LEFT/RIGHT DOUBLE QUOTATION MARK */
			return '"';
		}
	}

	return c;
}

static int mbfl_filt_conv_wchar_cp50220(int c, mbfl_convert_filter *filter)
{
	int mode = MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE;
	bool consumed = false;

	if (filter->cache) {
		int s = mb_convert_kana_codepoint(filter->cache, c, &consumed, NULL, mode);
		filter->cache = consumed ? 0 : c;
		/* Terrible hack to get CP50220 to emit error markers in the proper
		 * position, not reordering them with subsequent characters */
		filter->filter_function = mbfl_filt_conv_wchar_cp50221;
		mbfl_filt_conv_wchar_cp50221(s, filter);
		filter->filter_function = mbfl_filt_conv_wchar_cp50220;
		if (c == 0 && !consumed) {
			(*filter->output_function)(0, filter->data);
		}
	} else if (c == 0) {
		/* This case has to be handled separately, since `filter->cache == 0` means
		 * no codepoint is cached */
		(*filter->output_function)(0, filter->data);
	} else {
		filter->cache = c;
	}

	return 0;
}

static int mbfl_filt_conv_wchar_cp50220_flush(mbfl_convert_filter *filter)
{
	int mode = MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE;

	if (filter->cache) {
		int s = mb_convert_kana_codepoint(filter->cache, 0, NULL, NULL, mode);
		filter->filter_function = mbfl_filt_conv_wchar_cp50221;
		mbfl_filt_conv_wchar_cp50221(s, filter);
		filter->filter_function = mbfl_filt_conv_wchar_cp50220;
		filter->cache = 0;
	}

	return mbfl_filt_conv_any_jis_flush(filter);
}

int mbfl_filt_conv_wchar_cp50221(int c, mbfl_convert_filter *filter)
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
	} else if (c >= 0xE000 && c <= 0xE757) {
		/* 'private'/'user' codepoints */
		s = c - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
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
	}

	/* Above, we do a series of lookups in `ucs_*_jis_table` to find a
	 * corresponding kuten code for this Unicode codepoint
	 * If we get zero, that means the codepoint is not in JIS X 0208
	 * On the other hand, if we get a result with the high bits set on both
	 * upper and lower bytes, that is not a code in JIS X 0208 but rather
	 * in JIS X 0213
	 * In either case, check if this codepoint is one of the extensions added
	 * to JIS X 0208 by MicroSoft (to make CP932) */
	if (s == 0 || ((s & 0x8000) && (s & 0x80))) {
		int i;
		s = -1;

		for (i = 0;
				i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
				i++) {
			const int oh = cp932ext1_ucs_table_min / 94;

			if (c == cp932ext1_ucs_table[i]) {
				s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
				break;
			}
		}

		if (s < 0) {
			const int oh = cp932ext2_ucs_table_min / 94;
			const int cp932ext2_ucs_table_size =
					cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			for (i = 0; i < cp932ext2_ucs_table_size; i++) {
				if (c == cp932ext2_ucs_table[i]) {
					s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
					break;
				}
			}
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
				filter->status = 0;
			}
			CK((*filter->output_function)(s, filter->data));
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x49, filter->data));		/* 'I' */
				filter->status = 0x500;
			}
			CK((*filter->output_function)(s - 0x80, filter->data));
		} else if (s <= 0x927E) { /* X 0208 + extensions */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			CK(mbfl_filt_conv_illegal_output(c, filter));
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
 * wchar => CP50222
 */
int mbfl_filt_conv_wchar_cp50222(int c, mbfl_convert_filter *filter)
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
	} else if (c >= 0xE000 && c <= 0xE757) {
		/* 'private'/'user' codepoints */
		s = c - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
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
	}
	if (s == 0 || ((s & 0x8000) && (s & 0x80))) {
		int i;
		s = -1;

		for (i = 0;
				i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
			const int oh = cp932ext1_ucs_table_min / 94;

			if (c == cp932ext1_ucs_table[i]) {
				s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
				break;
			}
		}

		if (s <= 0) {
			const int oh = cp932ext2_ucs_table_min / 94;
			const int cp932ext2_ucs_table_size =
					cp932ext2_ucs_table_max - cp932ext2_ucs_table_min;
			for (i = 0; i < cp932ext2_ucs_table_size; i++) {
				if (c == cp932ext2_ucs_table[i]) {
					s = ((i / 94 + oh + 0x21) << 8) + (i % 94 + 0x21);
					break;
				}
			}
		}

		if (c == 0) {
			s = 0;
		} else if (s <= 0) {
			s = -1;
		}
	}

	if (s >= 0) {
		if (s < 0x80) { /* ASCII */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			} else if ((filter->status & 0xff00) != 0) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0;
			}
			CK((*filter->output_function)(s, filter->data));
		} else if (s >= 0xa0 && s < 0xe0) { /* X 0201 kana */
			if ((filter->status & 0xff00) != 0x500) {
				CK((*filter->output_function)(0x0e, filter->data));		/* SI */
				filter->status = 0x500;
			}
			CK((*filter->output_function)(s - 0x80, filter->data));
		} else if (s <= 0x927E) { /* X 0208 */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			}
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
				filter->status = 0x200;
			}
			CK((*filter->output_function)((s >> 8) & 0xff, filter->data));
			CK((*filter->output_function)(s & 0xff, filter->data));
		} else if (s < 0x10000) { /* X0212 */
			CK(mbfl_filt_conv_illegal_output(c, filter));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) == 0x500) {
				CK((*filter->output_function)(0x0f, filter->data));		/* SO */
				filter->status = 0;
			}
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

static int mbfl_filt_conv_wchar_cp50222_flush(mbfl_convert_filter *filter)
{
	/* back to latin */
	if ((filter->status & 0xff00) == 0x500) {
		CK((*filter->output_function)(0x0f, filter->data));		/* SO */
	} else if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data));		/* 'B' */
	}
	filter->status = 0;

	if (filter->flush_function) {
		(*filter->flush_function)(filter->data);
	}

	return 0;
}

#define ASCII 0
#define JISX_0201_LATIN 1
#define JISX_0201_KANA 2
#define JISX_0208 3
#define JISX_0212 4

static size_t mb_cp5022x_to_wchar(unsigned char **in, size_t *in_len, uint32_t *buf, size_t bufsize, unsigned int *state)
{
	ZEND_ASSERT(bufsize >= 3);

	unsigned char *p = *in, *e = p + *in_len;
	uint32_t *out = buf, *limit = buf + bufsize;

	while (p < e && out < limit) {
		unsigned char c = *p++;

		if (c == 0x1B) {
			/* Escape sequence */
			if ((e - p) < 2) {
				*out++ = MBFL_BAD_INPUT;
				/* Duplicate error-handling behavior of legacy code */
				if (p < e && (*p == '(' || *p == '$'))
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
			*state = JISX_0201_KANA;
		} else if (c == 0xF) {
			*state = ASCII;
		} else if (*state == JISX_0201_LATIN && c == 0x5C) { /* YEN SIGN */
			*out++ = 0xA5;
		} else if (*state == JISX_0201_LATIN && c == 0x7E) { /* OVER LINE */
			*out++ = 0x203E;
		} else if (*state == JISX_0201_KANA && c > 0x20 && c < 0x60) {
			*out++ = 0xFF40 + c;
		} else if (*state >= JISX_0208 && c > 0x20 && c <= 0x97) {
			if (p == e) {
				*out++ = MBFL_BAD_INPUT;
				break;
			}
			unsigned char c2 = *p++;
			if (c2 > 0x20 && c2 < 0x7F) {
				unsigned int s = (c - 0x21)*94 + c2 - 0x21;
				uint32_t w = 0;
				if (*state == JISX_0208) {
					if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {
						w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
					} else if (s < jisx0208_ucs_table_size) {
						w = jisx0208_ucs_table[s];
					} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {
						w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
					} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {
						w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
					} else if (s >= 94*94 && s < 114*94) {
						/* MicroSoft extension */
						w = s - 94*94 + 0xE000;
					}
					if (!w)
						w = MBFL_BAD_INPUT;
				} else {
					if (s < jisx0212_ucs_table_size) {
						w = jisx0212_ucs_table[s];
					}
					if (!w)
						w = MBFL_BAD_INPUT;
				}
				*out++ = w;
			} else {
				*out++ = MBFL_BAD_INPUT;
			}
		} else if (c < 0x80) {
			*out++ = c;
		} else if (c >= 0xA1 && c <= 0xDF) {
			*out++ = 0xFEC0 + c;
		} else {
			*out++ = MBFL_BAD_INPUT;
		}
	}

	*in_len = e - p;
	*in = p;
	return out - buf;
}

static unsigned int lookup_wchar(uint32_t w)
{
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
	} else if (w >= 0xE000 && w <= 0xE757) {
		/* Private Use Area codepoints */
		s = w - 0xE000;
		s = ((s / 94) + 0x7F) << 8 | ((s % 94) + 0x21);
	}

	if (!s) {
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
		} else if (w == 0) {
			return 0;
		}
	}

	/* Above, we do a series of lookups in `ucs_*_jis_table` to find a
	 * corresponding kuten code for this Unicode codepoint
	 * If we get zero, that means the codepoint is not in JIS X 0208
	 * On the other hand, if we get a result with the high bits set on both
	 * upper and lower bytes, that is not a code in JIS X 0208 but rather
	 * in JIS X 0213
	 * In either case, check if this codepoint is one of the extensions added
	 * to JIS X 0208 by MicroSoft (to make CP932) */
	if (!s || s >= 0x8080) {
		for (int i = 0; i < cp932ext1_ucs_table_max - cp932ext1_ucs_table_min; i++) {
			if (w == cp932ext1_ucs_table[i]) {
				return (((i / 94) + (cp932ext1_ucs_table_min / 94) + 0x21) << 8) + (i % 94) + 0x21;
			}
		}

		for (int i = 0; i < cp932ext2_ucs_table_max - cp932ext2_ucs_table_min; i++) {
			if (w == cp932ext2_ucs_table[i]) {
				return (((i / 94) + (cp932ext2_ucs_table_min / 94) + 0x21) << 8) + (i % 94) + 0x21;
			}
		}
	}

	return s;
}

static void mb_wchar_to_cp50220(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	uint32_t w;

	if (buf->state & 0xFFFF00) {
		/* Reprocess cached codepoint */
		w = buf->state >> 8;
		buf->state &= 0xFF;
		goto reprocess_codepoint;
	}

	while (len--) {
		w = *in++;
reprocess_codepoint:

		if (w >= 0xFF61 && w <= 0xFF9F && !len && !end) {
			/* This codepoint may need to combine with the next one,
			 * but the 'next one' will come in a separate buffer */
			buf->state |= w << 8;
			break;
		}

		bool consumed = false;
		w = mb_convert_kana_codepoint(w, len ? *in : 0, &consumed, NULL, MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE);
		if (consumed) {
			/* Two successive codepoints were converted into one */
			in++; len--; consumed = false;
		}

		unsigned int s = lookup_wchar(w);

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		} else if (s < 0x80) {
			/* ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != ASCII) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA0 && s < 0xE0) {
			/* JISX 0201 Kana */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_KANA) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s - 0x80);
		} else if (s <= 0x927E) {
			/* JISX 0208 Kanji */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
			if (buf->state != JISX_0208) {
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else if (s >= 0x10000) {
			/* JISX 0201 Latin; we 'tag' these by adding 0x10000 */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_LATIN) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_cp50221(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = lookup_wchar(w);

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		} else if (s < 0x80) {
			/* ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != ASCII) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA0 && s < 0xE0) {
			/* JISX 0201 Kana */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_KANA) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'I');
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s - 0x80);
		} else if (s <= 0x927E) {
			/* JISX 0208 Kanji */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
			if (buf->state != JISX_0208) {
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else if (s >= 0x10000) {
			/* JISX 0201 Latin; we 'tag' these by adding 0x10000 */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state != JISX_0201_LATIN) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50221);
		}
	}

	if (end && buf->state != ASCII) {
		MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
		out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}

static void mb_wchar_to_cp50222(uint32_t *in, size_t len, mb_convert_buf *buf, bool end)
{
	unsigned char *out, *limit;
	MB_CONVERT_BUF_LOAD(buf, out, limit);
	MB_CONVERT_BUF_ENSURE(buf, out, limit, len);

	while (len--) {
		uint32_t w = *in++;
		unsigned int s = lookup_wchar(w);

		if (!s && w) {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50222);
		} else if (s < 0x80) {
			/* ASCII */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 4);
			if (buf->state == JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xF);
				buf->state = ASCII;
			} else if (buf->state != ASCII) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
				buf->state = ASCII;
			}
			out = mb_convert_buf_add(out, s);
		} else if (s >= 0xA0 && s < 0xE0) {
			/* JISX 0201 Kana */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 2);
			if (buf->state != JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xE);
				buf->state = JISX_0201_KANA;
			}
			out = mb_convert_buf_add(out, s - 0x80);
		} else if (s <= 0x927E) {
			/* JISX 0208 Kanji */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 6);
			if (buf->state == JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xF);
			}
			if (buf->state != JISX_0208) {
				out = mb_convert_buf_add3(out, 0x1B, '$', 'B');
				buf->state = JISX_0208;
			}
			out = mb_convert_buf_add2(out, (s >> 8) & 0xFF, s & 0xFF);
		} else if (s >= 0x10000) {
			/* JISX 0201 Latin; we 'tag' these by adding 0x10000 */
			MB_CONVERT_BUF_ENSURE(buf, out, limit, len + 5);
			if (buf->state == JISX_0201_KANA) {
				out = mb_convert_buf_add(out, 0xF);
			}
			if (buf->state != JISX_0201_LATIN) {
				out = mb_convert_buf_add3(out, 0x1B, '(', 'J');
				buf->state = JISX_0201_LATIN;
			}
			out = mb_convert_buf_add(out, s & 0x7F);
		} else {
			MB_CONVERT_ERROR(buf, out, limit, w, mb_wchar_to_cp50222);
		}
	}

	if (end) {
		if (buf->state == JISX_0201_KANA) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, 1);
			out = mb_convert_buf_add(out, 0xF);
		} else if (buf->state != ASCII) {
			MB_CONVERT_BUF_ENSURE(buf, out, limit, 3);
			out = mb_convert_buf_add3(out, 0x1B, '(', 'B');
		}
	}

	MB_CONVERT_BUF_STORE(buf, out, limit);
}
