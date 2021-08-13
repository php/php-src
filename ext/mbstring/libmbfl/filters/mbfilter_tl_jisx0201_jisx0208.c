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

#include <stdint.h>
#include <stdbool.h>
#include "mbfilter_tl_jisx0201_jisx0208.h"
#include "translit_kana_jisx0201_jisx0208.h"

/* Apply various transforms to input codepoint, such as converting halfwidth katakana
 * to fullwidth katakana. `mode` is a bitfield which controls which transforms are
 * actually performed. The bit values are defined in mbfilter_tl_jisx0201_jix0208.h.
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
int mbfl_convert_kana(int c, int next, bool *consumed, int *second, int mode)
{
	if ((mode & MBFL_FILT_TL_HAN2ZEN_ALL) && c >= 0x21 && c <= 0x7d && c != '"' && c != '\'' && c != '\\') {
		return c + 0xfee0;
	} else if ((mode & MBFL_FILT_TL_HAN2ZEN_ALPHA) && ((c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a))) { /* alphabetic */
		return c + 0xfee0;
	} else if ((mode & MBFL_FILT_TL_HAN2ZEN_NUMERIC) && c >= 0x30 && c <= 0x39) { /* num */
		return c + 0xfee0;
	} else if ((mode & MBFL_FILT_TL_HAN2ZEN_SPACE) && c == 0x20) { /* space */
		return 0x3000;
	}

	if (mode & (MBFL_FILT_TL_HAN2ZEN_KATAKANA | MBFL_FILT_TL_HAN2ZEN_HIRAGANA)) {
		/* Convert Hankaku kana to Zenkaku kana
		 * Either all Hankaku kana (including katakana and hiragana) will be converted
		 * to Zenkaku katakana, or to Zenkaku hiragana */
		if ((mode & MBFL_FILT_TL_HAN2ZEN_KATAKANA) && (mode & MBFL_FILT_TL_HAN2ZEN_GLUE)) {
			if (c >= 0xff61 && c <= 0xff9f) {
				int n = c - 0xff60;
				if (next >= 0xff61 && next <= 0xff9f) {
					if (next == 0xff9e && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						*consumed = true;
						return 0x3001 + hankana2zenkana_table[n];
					} else if (next == 0xff9e && n == 19) {
						*consumed = true;
						return 0x30f4;
					} else if (next == 0xff9f && n >= 42 && n <= 46) {
						*consumed = true;
						return 0x3002 + hankana2zenkana_table[n];
					}
				}

				return 0x3000 + hankana2zenkana_table[n];
			}
		} else if ((mode & MBFL_FILT_TL_HAN2ZEN_HIRAGANA) && (mode & MBFL_FILT_TL_HAN2ZEN_GLUE)) {
			if (c >= 0xff61 && c <= 0xff9f) {
				int n = c - 0xff60;
				if (next >= 0xff61 && next <= 0xff9f) {
					if (next == 0xff9e && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						*consumed = true;
						return 0x3001 + hankana2zenhira_table[n];
					} else if (next == 0xff9f && n >= 42 && n <= 46) {
						*consumed = true;
						return 0x3002 + hankana2zenhira_table[n];
					}
				}

				return 0x3000 + hankana2zenhira_table[n];
			}
		} else if ((mode & MBFL_FILT_TL_HAN2ZEN_KATAKANA) && c >= 0xff61 && c <= 0xff9f) {
			return 0x3000 + hankana2zenkana_table[c - 0xff60];
		} else if ((mode & MBFL_FILT_TL_HAN2ZEN_HIRAGANA) && c >= 0xff61 && c <= 0xff9f) {
			return 0x3000 + hankana2zenhira_table[c - 0xff60];
		}
	}

	if (mode & MBFL_FILT_TL_HAN2ZEN_COMPAT1) { /* special ascii to symbol */
		if (c == 0x5c) {
			return 0xffe5; /* FULLWIDTH YEN SIGN */
		} else if (c == 0xa5) { /* YEN SIGN */
			return 0xffe5; /* FULLWIDTH YEN SIGN */
		} else if (c == 0x7e) {
			return 0xffe3; /* FULLWIDTH MACRON */
		} else if (c == 0x203e) { /* OVERLINE */
			return 0xffe3; /* FULLWIDTH MACRON */
		} else if (c == 0x27) {
			return 0x2019; /* RIGHT SINGLE QUOTATION MARK */
		} else if (c == 0x22) {
			return 0x201d; /* RIGHT DOUBLE QUOTATION MARK */
		}
	} else if (mode & MBFL_FILT_TL_HAN2ZEN_COMPAT2) { /* special ascii to symbol */
		if (c == 0x5c) {
			return 0xff3c; /* FULLWIDTH REVERSE SOLIDUS */
		} else if (c == 0x7e) {
			return 0xff5e; /* FULLWIDTH TILDE */
		} else if (c == 0x27) {
			return 0xff07; /* FULLWIDTH APOSTROPHE */
		} else if (c == 0x22) {
			return 0xff02; /* FULLWIDTH QUOTATION MARK */
		}
	}

	if (mode & (MBFL_FILT_TL_ZEN2HAN_ALL | MBFL_FILT_TL_ZEN2HAN_ALPHA | MBFL_FILT_TL_ZEN2HAN_NUMERIC | MBFL_FILT_TL_ZEN2HAN_SPACE)) {
		/* Zenkaku to Hankaku */
		if ((mode & MBFL_FILT_TL_ZEN2HAN_ALL) && c >= 0xff01 && c <= 0xff5d && c != 0xff02 && c != 0xff07 && c!= 0xff3c) {
			/* all except " ' \ ~ */
			return c - 0xfee0;
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_ALPHA) && ((c >= 0xff21 && c <= 0xff3a) || (c >= 0xff41 && c <= 0xff5a))) {
			return c - 0xfee0;
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_NUMERIC) && (c >= 0xff10 && c <= 0xff19)) {
			return c - 0xfee0;
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_SPACE) && (c == 0x3000)) {
			return 0x20;
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_ALL) && (c == 0x2212)) { /* MINUS SIGN */
			return 0x2d;
		}
	}

	if (mode & (MBFL_FILT_TL_ZEN2HAN_KATAKANA | MBFL_FILT_TL_ZEN2HAN_HIRAGANA)) {
		/* Zenkaku kana to hankaku kana */
		if ((mode & MBFL_FILT_TL_ZEN2HAN_KATAKANA) && c >= 0x30a1 && c <= 0x30f4) {
			/* Zenkaku katakana to hankaku kana */
			int n = c - 0x30a1;
			if (zenkana2hankana_table[n][1]) {
				*second = 0xff00 + zenkana2hankana_table[n][1];
			}
			return 0xff00 + zenkana2hankana_table[n][0];
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_HIRAGANA) && c >= 0x3041 && c <= 0x3093) {
			/* Zenkaku hiragana to hankaku kana */
			int n = c - 0x3041;
			if (zenkana2hankana_table[n][1]) {
				*second = 0xff00 + zenkana2hankana_table[n][1];
			}
			return 0xff00 + zenkana2hankana_table[n][0];
		} else if (c == 0x3001) {
			return 0xff64; /* HALFWIDTH IDEOGRAPHIC COMMA */
		} else if (c == 0x3002) {
			return 0xff61; /* HALFWIDTH IDEOGRAPHIC FULL STOP */
		} else if (c == 0x300c) {
			return 0xff62; /* HALFWIDTH LEFT CORNER BRACKET */
		} else if (c == 0x300d) {
			return 0xff63; /* HALFWIDTH RIGHT CORNER BRACKET */
		} else if (c == 0x309b) {
			return 0xff9e; /* HALFWIDTH KATAKANA VOICED SOUND MARK */
		} else if (c == 0x309c) {
			return 0xff9f; /* HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK */
		} else if (c == 0x30fc) {
			return 0xff70; /* HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK */
		} else if (c == 0x30fb) {
			return 0xff65; /* HALFWIDTH KATAKANA MIDDLE DOT */
		}
	} else if (mode & (MBFL_FILT_TL_ZEN2HAN_HIRA2KANA | MBFL_FILT_TL_ZEN2HAN_KANA2HIRA)) {
		if ((mode & MBFL_FILT_TL_ZEN2HAN_HIRA2KANA) && ((c >= 0x3041 && c <= 0x3093) || c == 0x309d || c == 0x309e)) {
			/* Zenkaku hiragana to Zenkaku katakana */
			return c + 0x60;
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_KANA2HIRA) && ((c >= 0x30a1 && c <= 0x30f3) || c == 0x30fd || c == 0x30fe)) {
			/* Zenkaku katakana to Zenkaku hiragana */
			return c - 0x60;
		}
	}

	if (mode & MBFL_FILT_TL_ZEN2HAN_COMPAT1) { /* special symbol to ascii */
		if (c == 0xffe5) { /* FULLWIDTH YEN SIGN */
			return 0x5c;
		} else if (c == 0xff3c) { /* FULLWIDTH REVERSE SOLIDUS */
			return 0x5c;
		} else if (c == 0xffe3) { /* FULLWIDTH MACRON */
			return 0x7e;
		} else if (c == 0x203e) { /* OVERLINE */
			return 0x7e;
		} else if (c == 0x2018) { /* LEFT SINGLE QUOTATION MARK*/
			return 0x27;
		} else if (c == 0x2019) { /* RIGHT SINGLE QUOTATION MARK */
			return 0x27;
		} else if (c == 0x201c) { /* LEFT DOUBLE QUOTATION MARK */
			return 0x22;
		} else if (c == 0x201d) { /* RIGHT DOUBLE QUOTATION MARK */
			return 0x22;
		}
	}

	if (mode & MBFL_FILT_TL_ZEN2HAN_COMPAT2) { /* special symbol to ascii */
		if (c == 0xff3c) { /* FULLWIDTH REVERSE SOLIDUS */
			return 0x5c;
		} else if (c == 0xff5e) { /* FULLWIDTH TILDE */
			return 0x7e;
		} else if (c == 0xff07) { /* FULLWIDTH APOSTROPHE */
			return 0x27;
		} else if (c == 0xff02) { /* FULLWIDTH QUOTATION MARK */
			return 0x22;
		}
	}

	return c;
}

int mbfl_filt_tl_jisx0201_jisx0208(int c, mbfl_convert_filter *filt)
{
	int mode = (intptr_t)filt->opaque, second = 0;
	bool consumed = false;

	if (filt->cache) {
		int s = mbfl_convert_kana(filt->cache, c, &consumed, &second, mode);
		filt->cache = consumed ? 0 : c;
		(*filt->output_function)(s, filt->data);
		if (second) {
			(*filt->output_function)(second, filt->data);
		}
	} else if (c == 0) {
		/* This case has to be handled separately, since `filt->cache == 0` means no
		 * codepoint is cached */
		(*filt->output_function)(0, filt->data);
	} else {
		filt->cache = c;
	}

	return 0;
}

int mbfl_filt_tl_jisx0201_jisx0208_flush(mbfl_convert_filter *filt)
{
	int mode = (intptr_t)filt->opaque, second = 0;

	if (filt->cache) {
		int s = mbfl_convert_kana(filt->cache, 0, NULL, &second, mode);
		(*filt->output_function)(s, filt->data);
		if (second) {
			(*filt->output_function)(second, filt->data);
		}
		filt->cache = 0;
	}

	if (filt->flush_function) {
		return (*filt->flush_function)(filt->data);
	}

	return 0;
}

const struct mbfl_convert_vtbl vtbl_tl_jisx0201_jisx0208 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_tl_jisx0201_jisx0208,
	mbfl_filt_tl_jisx0201_jisx0208_flush,
	NULL,
};
