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

#include "mbfl_allocators.h"
#include "mbfilter_tl_jisx0201_jisx0208.h"
#include "translit_kana_jisx0201_jisx0208.h"

void
mbfl_filt_tl_jisx0201_jisx0208_init(mbfl_convert_filter *filt)
{
	mbfl_filt_conv_common_ctor(filt);
}

void
mbfl_filt_tl_jisx0201_jisx0208_cleanup(mbfl_convert_filter *filt)
{
}

int
mbfl_filt_tl_jisx0201_jisx0208(int c, mbfl_convert_filter *filt)
{
	int s, n;
	int mode = ((mbfl_filt_tl_jisx0201_jisx0208_param *)filt->opaque)->mode;

	s = c;

	if ((mode & MBFL_FILT_TL_HAN2ZEN_ALL)
			&& c >= 0x21 && c <= 0x7d && c != 0x22 && c != 0x27 && c != 0x5c) {
		/* all except <"> <'> <\> <~> */
		s = c + 0xfee0;
	} else if ((mode & MBFL_FILT_TL_HAN2ZEN_ALPHA) &&
			((c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a))) {
		/* alpha */
		s = c + 0xfee0;
	} else if ((mode & MBFL_FILT_TL_HAN2ZEN_NUMERIC) &&
			c >= 0x30 && c <= 0x39) {
		/* num */
		s = c + 0xfee0;
	} else if ((mode & MBFL_FILT_TL_HAN2ZEN_SPACE) && c == 0x20) {
		/* space */
		s = 0x3000;
	}

	if (mode &
			(MBFL_FILT_TL_HAN2ZEN_KATAKANA | MBFL_FILT_TL_HAN2ZEN_HIRAGANA)) {
		/* hankaku kana to zenkaku kana */
		if ((mode & MBFL_FILT_TL_HAN2ZEN_KATAKANA) &&
				(mode & MBFL_FILT_TL_HAN2ZEN_GLUE)) {
			/* hankaku kana to zenkaku katakana and glue voiced sound mark */
			if (c >= 0xff61 && c <= 0xff9f) {
				if (filt->status) {
					n = (filt->cache - 0xff60) & 0x3f;
					if (c == 0xff9e && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						filt->status = 0;
						s = 0x3001 + hankana2zenkana_table[n];
					} else if (c == 0xff9e && n == 19) {
						filt->status = 0;
						s = 0x30f4;
					} else if (c == 0xff9f && (n >= 42 && n <= 46)) {
						filt->status = 0;
						s = 0x3002 + hankana2zenkana_table[n];
					} else {
						filt->status = 1;
						filt->cache = c;
						s = 0x3000 + hankana2zenkana_table[n];
					}
				} else {
					filt->status = 1;
					filt->cache = c;
					return c;
				}
			} else {
				if (filt->status) {
					n = (filt->cache - 0xff60) & 0x3f;
					filt->status = 0;
					(*filt->output_function)(0x3000 + hankana2zenkana_table[n], filt->data);
				}
			}
		} else if ((mode & MBFL_FILT_TL_HAN2ZEN_HIRAGANA) &&
				(mode & MBFL_FILT_TL_HAN2ZEN_GLUE)) {
			/* hankaku kana to zenkaku hirangana and glue voiced sound mark */
			if (c >= 0xff61 && c <= 0xff9f) {
				if (filt->status) {
					n = (filt->cache - 0xff60) & 0x3f;
					if (c == 0xff9e && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						filt->status = 0;
						s = 0x3001 + hankana2zenhira_table[n];
					} else if (c == 0xff9f && (n >= 42 && n <= 46)) {
						filt->status = 0;
						s = 0x3002 + hankana2zenhira_table[n];
					} else {
						filt->status = 1;
						filt->cache = c;
						s = 0x3000 + hankana2zenhira_table[n];
					}
				} else {
					filt->status = 1;
					filt->cache = c;
					return c;
				}
			} else {
				if (filt->status) {
					n = (filt->cache - 0xff60) & 0x3f;
					filt->status = 0;
					(*filt->output_function)(0x3000 + hankana2zenhira_table[n], filt->data);
				}
			}
		} else if ((mode & MBFL_FILT_TL_HAN2ZEN_KATAKANA) &&
				c >= 0xff61 && c <= 0xff9f) {
			/* hankaku kana to zenkaku katakana */
			s = 0x3000 + hankana2zenkana_table[c - 0xff60];
		} else if ((mode & MBFL_FILT_TL_HAN2ZEN_HIRAGANA)
				&& c >= 0xff61 && c <= 0xff9f) {
			/* hankaku kana to zenkaku hirangana */
			s = 0x3000 + hankana2zenhira_table[c - 0xff60];
		}
	}

	if (mode & MBFL_FILT_TL_HAN2ZEN_COMPAT1) {
		/* special ascii to symbol */
		if (c == 0x5c) {
			s = 0xffe5;				/* FULLWIDTH YEN SIGN */
		} else if (c == 0xa5) {		/* YEN SIGN */
			s = 0xffe5;				/* FULLWIDTH YEN SIGN */
		} else if (c == 0x7e) {
			s = 0xffe3;				/* FULLWIDTH MACRON */
		} else if (c == 0x203e) {	/* OVERLINE */
			s = 0xffe3;				/* FULLWIDTH MACRON */
		} else if (c == 0x27) {
			s = 0x2019;				/* RIGHT SINGLE QUOTATION MARK */
		} else if (c == 0x22) {
			s = 0x201d;				/* RIGHT DOUBLE QUOTATION MARK */
		}
	} else if (mode & MBFL_FILT_TL_HAN2ZEN_COMPAT2) {
		/* special ascii to symbol */
		if (c == 0x5c) {
			s = 0xff3c;				/* FULLWIDTH REVERSE SOLIDUS */
		} else if (c == 0x7e) {
			s = 0xff5e;				/* FULLWIDTH TILDE */
		} else if (c == 0x27) {
			s = 0xff07;				/* FULLWIDTH APOSTROPHE */
		} else if (c == 0x22) {
			s = 0xff02;				/* FULLWIDTH QUOTATION MARK */
		}
	}

	if (mode & 0xf0) { /* zenkaku to hankaku */
		if ((mode & 0x10) && c >= 0xff01 && c <= 0xff5d && c != 0xff02 && c != 0xff07 && c!= 0xff3c) {	/* all except <"> <'> <\> <~> */
			s = c - 0xfee0;
		} else if ((mode & 0x20) && ((c >= 0xff21 && c <= 0xff3a) || (c >= 0xff41 && c <= 0xff5a))) {	/* alpha */
			s = c - 0xfee0;
		} else if ((mode & 0x40) && (c >= 0xff10 && c <= 0xff19)) {	/* num */
			s = c - 0xfee0;
		} else if ((mode & 0x80) && (c == 0x3000)) {	/* spase */
			s = 0x20;
		} else if ((mode & 0x10) && (c == 0x2212)) {	/* MINUS SIGN */
			s = 0x2d;
		}
	}

	if (mode &
			(MBFL_FILT_TL_ZEN2HAN_KATAKANA | MBFL_FILT_TL_ZEN2HAN_HIRAGANA)) {
		/* Zenkaku kana to hankaku kana */
		if ((mode & MBFL_FILT_TL_ZEN2HAN_KATAKANA) &&
				c >= 0x30a1 && c <= 0x30f4) {
			/* Zenkaku katakana to hankaku kana */
			n = c - 0x30a1;
			if (zenkana2hankana_table[n][1] != 0) {
				(filt->output_function)(0xff00 + zenkana2hankana_table[n][0], filt->data);
				s = 0xff00 + zenkana2hankana_table[n][1];
			} else {
				s = 0xff00 + zenkana2hankana_table[n][0];
			}
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_HIRAGANA) &&
				c >= 0x3041 && c <= 0x3093) {
			/* Zenkaku hirangana to hankaku kana */
			n = c - 0x3041;
			if (zenkana2hankana_table[n][1] != 0) {
				(filt->output_function)(0xff00 + zenkana2hankana_table[n][0], filt->data);
				s = 0xff00 + zenkana2hankana_table[n][1];
			} else {
				s = 0xff00 + zenkana2hankana_table[n][0];
			}
		} else if (c == 0x3001) {
			s = 0xff64;				/* HALFWIDTH IDEOGRAPHIC COMMA */
		} else if (c == 0x3002) {
			s = 0xff61;				/* HALFWIDTH IDEOGRAPHIC FULL STOP */
		} else if (c == 0x300c) {
			s = 0xff62;				/* HALFWIDTH LEFT CORNER BRACKET */
		} else if (c == 0x300d) {
			s = 0xff63;				/* HALFWIDTH RIGHT CORNER BRACKET */
		} else if (c == 0x309b) {
			s = 0xff9e;				/* HALFWIDTH KATAKANA VOICED SOUND MARK */
		} else if (c == 0x309c) {
			s = 0xff9f;				/* HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK */
		} else if (c == 0x30fc) {
			s = 0xff70;				/* HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK */
		} else if (c == 0x30fb) {
			s = 0xff65;				/* HALFWIDTH KATAKANA MIDDLE DOT */
		}
	} else if (mode & (MBFL_FILT_TL_ZEN2HAN_HIRA2KANA
			| MBFL_FILT_TL_ZEN2HAN_KANA2HIRA)) { 
		if ((mode & MBFL_FILT_TL_ZEN2HAN_HIRA2KANA) &&
				c >= 0x3041 && c <= 0x3093) {
			/* Zenkaku hirangana to Zenkaku katakana */
			s = c + 0x60;
		} else if ((mode & MBFL_FILT_TL_ZEN2HAN_KANA2HIRA) &&
				c >= 0x30a1 && c <= 0x30f3) {
			/* Zenkaku katakana to Zenkaku hirangana */
			s = c - 0x60;
		}
	}

	if (mode & MBFL_FILT_TL_ZEN2HAN_COMPAT1) {	/* special symbol to ascii */
		if (c == 0xffe5) {			/* FULLWIDTH YEN SIGN */
			s = 0x5c;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x5c;
		} else if (c == 0xffe3) {	/* FULLWIDTH MACRON */
			s = 0x7e;
		} else if (c == 0x203e) {	/* OVERLINE */
			s = 0x7e;
		} else if (c == 0x2018) {	/* LEFT SINGLE QUOTATION MARK*/
			s = 0x27;
		} else if (c == 0x2019) {	/* RIGHT SINGLE QUOTATION MARK */
			s = 0x27;
		} else if (c == 0x201c) {	/* LEFT DOUBLE QUOTATION MARK */
			s = 0x22;
		} else if (c == 0x201d) {	/* RIGHT DOUBLE QUOTATION MARK */
			s = 0x22;
		}
	}

	if (mode & MBFL_FILT_TL_ZEN2HAN_COMPAT2) {	/* special symbol to ascii */
		if (c == 0xff3c) {			/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x5c;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s = 0x7e;
		} else if (c == 0xff07) {	/* FULLWIDTH APOSTROPHE */
			s = 0x27;
		} else if (c == 0xff02) {	/* FULLWIDTH QUOTATION MARK */
			s = 0x22;
		}
	}

	return (*filt->output_function)(s, filt->data);
}

int
mbfl_filt_tl_jisx0201_jisx0208_flush(mbfl_convert_filter *filt)
{
	int ret, n;
	int mode = ((mbfl_filt_tl_jisx0201_jisx0208_param *)filt->opaque)->mode;

	ret = 0;
	if (filt->status) {
		n = (filt->cache - 0xff60) & 0x3f;
		if (mode & 0x100) {	/* hankaku kana to zenkaku katakana */
			ret = (*filt->output_function)(0x3000 + hankana2zenkana_table[n], filt->data);
		} else if (mode & 0x200) {	/* hankaku kana to zenkaku hirangana */
			ret = (*filt->output_function)(0x3000 + hankana2zenhira_table[n], filt->data);
		}
		filt->status = 0;
	}

	if (filt->flush_function != NULL) {
		return (*filt->flush_function)(filt->data);
	}

	return ret;
}

const struct mbfl_convert_vtbl vtbl_tl_jisx0201_jisx0208 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_wchar,
	mbfl_filt_tl_jisx0201_jisx0208_init,
	mbfl_filt_tl_jisx0201_jisx0208_cleanup,
	mbfl_filt_tl_jisx0201_jisx0208,
	mbfl_filt_tl_jisx0201_jisx0208_flush
};

