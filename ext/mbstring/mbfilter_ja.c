/* charset=UTF-8 */

/*
 * "streamable kanji code filter and converter"
 *
 * Copyright (c) 1998,1999,2000,2001 HappySize, Inc. All rights reserved.
 *
 * This software is released under the GNU Lesser General Public License.
 * (Version 2.1, February 1999)
 * Please read the following detail of the licence (in japanese).
 *
 * ◆使用許諾条件◆
 *
 * このソフトウェアは株式会社ハッピーサイズによって開発されました。株式会社ハッ
 * ピーサイズは、著作権法および万国著作権条約の定めにより、このソフトウェアに関
 * するすべての権利を留保する権利を持ち、ここに行使します。株式会社ハッピーサイ
 * ズは以下に明記した条件に従って、このソフトウェアを使用する排他的ではない権利
 * をお客様に許諾します。何人たりとも、以下の条件に反してこのソフトウェアを使用
 * することはできません。
 *
 * このソフトウェアを「GNU Lesser General Public License (Version 2.1, February
 * 1999)」に示された条件で使用することを、全ての方に許諾します。「GNU Lesser
 * General Public License」を満たさない使用には、株式会社ハッピーサイズから書面
 * による許諾を得る必要があります。
 *
 * 「GNU Lesser General Public License」の全文は以下のウェブページから取得でき
 * ます。「GNU Lesser General Public License」とは、これまでLibrary General
 * Public Licenseと呼ばれていたものです。
 *     http://www.gnu.org/ --- GNUウェブサイト
 *     http://www.gnu.org/copyleft/lesser.html --- ライセンス文面
 * このライセンスの内容がわからない方、守れない方には使用を許諾しません。
 *
 * しかしながら、当社とGNUプロジェクトとの特定の関係を示唆または主張するもので
 * はありません。
 *
 * ◆保証内容◆
 *
 * このソフトウェアは、期待された動作・機能・性能を持つことを目標として設計され
 * 開発されていますが、これを保証するものではありません。このソフトウェアは「こ
 * のまま」の状態で提供されており、たとえばこのソフトウェアの有用性ないし特定の
 * 目的に合致することといった、何らかの保証内容が、明示されたり暗黙に示されてい
 * る場合であっても、その保証は無効です。このソフトウェアを使用した結果ないし使
 * 用しなかった結果によって、直接あるいは間接に受けた身体的な傷害、財産上の損害
 * 、データの損失あるいはその他の全ての損害については、その損害の可能性が使用者
 * 、当社あるいは第三者によって警告されていた場合であっても、当社はその損害の賠
 * 償および補填を行いません。この規定は他の全ての、書面上または書面に無い保証・
 * 契約・規定に優先します。
 *
 * ◆著作権者の連絡先および使用条件についての問い合わせ先◆
 *
 * 〒102-0073
 * 東京都千代田区九段北1-13-5日本地所第一ビル4F
 * 株式会社ハッピーサイズ
 * Phone: 03-3512-3655, Fax: 03-3512-3656
 * Email: sales@happysize.co.jp
 * Web: http://happysize.com/
 *
 * ◆著者◆
 *
 * 金本　茂 <sgk@happysize.co.jp>
 *
 * ◆履歴◆
 *
 * 1998/11/10 sgk implementation in C++
 * 1999/4/25  sgk Cで書きなおし。
 * 1999/4/26  sgk 入力フィルタを実装。漢字コードを推定しながらフィルタを追加。
 * 1999/6/??      Unicodeサポート。
 * 1999/6/22  sgk ライセンスをLGPLに変更。
 *
 */

/* 
 * Unicode support
 *
 * Portions copyright (c) 1999,2000,2001 by the PHP3 internationalization team.
 * All rights reserved.
 *
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_globals.h"

#if defined(HAVE_MBSTR_JA)

#include "mbfilter.h"
#include "mbfilter_ja.h"

#include "cp932_table.h"
#include "unicode_table_ja.h"

#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

#define SJIS_ENCODE(c1,c2,s1,s2)	\
		do {						\
			s1 = c1;				\
			s1--;					\
			s1 >>= 1;				\
			if ((c1) < 0x5f) {		\
				s1 += 0x71;			\
			} else {				\
				s1 += 0xb1;			\
			}						\
			s2 = c2;				\
			if ((c1) & 1) {			\
				if ((c2) < 0x60) {	\
					s2--;			\
				}					\
				s2 += 0x20;			\
			} else {				\
				s2 += 0x7e;			\
			}						\
		} while (0)

#define SJIS_DECODE(c1,c2,s1,s2)	\
		do {						\
			s1 = c1;				\
			if (s1 < 0xa0) {		\
				s1 -= 0x81;			\
			} else {				\
				s1 -= 0xc1;			\
			}						\
			s1 <<= 1;				\
			s1 += 0x21;				\
			s2 = c2;				\
			if (s2 < 0x9f) {		\
				if (s2 < 0x7f) {	\
					s2++;			\
				}					\
				s2 -= 0x20;			\
			} else {				\
				s1++;				\
				s2 -= 0x7e;			\
			}						\
		} while (0)


/*
 * EUC-JP => wchar
 */
int
mbfl_filt_conv_eucjp_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xff) {	/* X 0208 first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* kana first char */
			filter->status = 2;
		} else if (c == 0x8f) {	/* X 0212 first char */
			filter->status = 3;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[s];
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_JIS0208;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 2:	/* got 0x8e */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = 0x8e00 | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 3:	/* got 0x8f,  X 0212 first char */
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
			filter->status = 0;
		} else {
			filter->status++;
			filter->cache = c;
		}
		break;
	case 4:	/* got 0x8f,  X 0212 second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c1 > 0xa0 && c1 < 0xff && c > 0xa0 && c < 0xff) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0212_ucs_table_size) {
				w = jisx0212_ucs_table[s];
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_JIS0212;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c | 0x8f0000;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => EUC-JP
 */
int
mbfl_filt_conv_wchar_eucjp(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s;

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
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_JIS0208) {
			s = c & MBFL_WCSPLANE_MASK;
		} else if (c1 == MBFL_WCSPLANE_JIS0212) {
			s = c & MBFL_WCSPLANE_MASK;
			s |= 0x8080;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s = 0x2141;
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
		if (s < 0x80) {	/* latin */
			CK((*filter->output_function)(s, filter->data TSRMLS_CC));
		} else if (s < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s, filter->data TSRMLS_CC));
		} else if (s < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s >> 8) & 0xff) | 0x80, filter->data TSRMLS_CC));
			CK((*filter->output_function)((s & 0xff) | 0x80, filter->data TSRMLS_CC));
		} else {	/* X 0212 */
			CK((*filter->output_function)(0x8f, filter->data TSRMLS_CC));
			CK((*filter->output_function)(((s >> 8) & 0xff) | 0x80, filter->data TSRMLS_CC));
			CK((*filter->output_function)((s & 0xff) | 0x80, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}


/*
 * eucJP-win => wchar
 */
int
mbfl_filt_conv_eucjpwin_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s, w, n;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xff) {	/* CP932 first char */
			filter->status = 1;
			filter->cache = c;
		} else if (c == 0x8e) {	/* kana first char */
			filter->status = 2;
		} else if (c == 0x8f) {	/* X 0212 first char */
			filter->status = 3;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 1:	/* got first half */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0xa0 && c < 0xff) {
			w = 0;
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s <= 137) {
				if (s == 31) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xff5e;			/* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225;			/* PARALLEL TO */
				} else if (s == 60) {
					w = 0xff0d;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xffe0;			/* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xffe1;			/* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xffe2;			/* FULLWIDTH NOT SIGN */
				}
			}
			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= (84*94)) {		/* user (85ku - 94ku) */
					w = s - (84*94) + 0xe000;
				}
			}
			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_WINCP932;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 2:	/* got 0x8e, X0201 kana */
		filter->status = 0;
		if (c > 0xa0 && c < 0xe0) {
			w = 0xfec0 + c;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = 0x8e00 | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 3:	/* got 0x8f,  X 0212 first char */
		if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
			filter->status = 0;
		} else {
			filter->status++;
			filter->cache = c;
		}
		break;
	case 4:	/* got 0x8f,  X 0212 second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c1 > 0xa0 && c1 < 0xff && c > 0xa0 && c < 0xff) {
			s = (c1 - 0xa1)*94 + c - 0xa1;
			if (s >= 0 && s < jisx0212_ucs_table_size) {
				w = jisx0212_ucs_table[s];
			} else if (s >= (82*94) && s < (84*94)) {	/* vender ext3 (83ku - 84ku) <-> CP932 (115ku -120ku) */
				s = (c1<< 8) | c;
				w = 0;
				n = 0;
				while (n < cp932ext3_eucjp_table_size) {
					if (s == cp932ext3_eucjp_table[n]) {
						if (n < (cp932ext3_ucs_table_max - cp932ext3_ucs_table_min)) {
							w = cp932ext3_ucs_table[n];
						}
						break;
					}
					n++;
				}
			} else if (s >= (84*94)) {		/* user (85ku - 94ku) */
				w = s - (84*94) + (0xe000 + (94*10));
			} else {
				w = 0;
			}
			if (w <= 0) {
				w = ((c1 & 0x7f) << 8) | (c & 0x7f);
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_JIS0212;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c | 0x8f0000;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => eucJP-win
 */
int
mbfl_filt_conv_wchar_eucjpwin(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, c2, s1;

	s1 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 10*94)) {	/* user  (X0208  85ku - 94ku) */
		s1 = c - 0xe000;
		c1 = s1/94 + 0x75;
		c2 = s1%94 + 0x21;
		s1 = (c1 << 8) | c2;
	} else if (c >= (0xe000 + 10*94) && c < (0xe000 + 20*94)) {	/* user  (X0212  85ku - 94ku) */
		s1 = c - (0xe000 + 10*94);
		c1 = s1/94 + 0xf5;
		c2 = s1%94 + 0xa1;
		s1 = (c1 << 8) | c2;
	}
	if (s1 <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_WINCP932) {
			s1 = c & MBFL_WCSPLANE_MASK;
			if (s1 >= ((85 + 0x20) << 8)) {	/* 85ku - 120ku */
				s1 = -1;
			}
		} else if (c1 == MBFL_WCSPLANE_JIS0208) {
			s1 = c & MBFL_WCSPLANE_MASK;
			if (s1 >= ((85 + 0x20) << 8)) {	/* 85ku - 94ku */
				s1 = -1;
			}
		} else if (c1 == MBFL_WCSPLANE_JIS0212) {
			s1 = c & MBFL_WCSPLANE_MASK;
			if (s1 >= ((83 + 0x20) << 8)) {	/* 83ku - 94ku */
				s1 = -1;
			} else {
				s1 |= 0x8080;
			}
		} else if (c == 0xa5) {		/* YEN SIGN */
			s1 = 0x216f;			/* FULLWIDTH YEN SIGN */
		} else if (c == 0x203e) {	/* OVER LINE */
			s1 = 0x2131;			/* FULLWIDTH MACRON */
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s1 = 0x2141;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		} else {
			s1 = -1;
			c1 = 0;
			c2 = cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext1 (13ku) */
				if (c == cp932ext1_ucs_table[c1]) {
					s1 = ((c1/94 + 0x2d) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
			if (s1 < 0) {
				c1 = 0;
				c2 = cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
				while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
					if (c == cp932ext3_ucs_table[c1]) {
						if (c1 < cp932ext3_eucjp_table_size) {
							s1 = cp932ext3_eucjp_table[c1];
						}
						break;
					}
					c1++;
				}
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}

	if (s1 >= 0) {
		if (s1 < 0x80) {	/* latin */
			CK((*filter->output_function)(s1, filter->data TSRMLS_CC));
		} else if (s1 < 0x100) {	/* kana */
			CK((*filter->output_function)(0x8e, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s1, filter->data TSRMLS_CC));
		} else if (s1 < 0x8080)  {	/* X 0208 */
			CK((*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data TSRMLS_CC));
			CK((*filter->output_function)((s1 & 0xff) | 0x80, filter->data TSRMLS_CC));
		} else {	/* X 0212 */
			CK((*filter->output_function)(0x8f, filter->data TSRMLS_CC));
			CK((*filter->output_function)(((s1 >> 8) & 0xff) | 0x80, filter->data TSRMLS_CC));
			CK((*filter->output_function)((s1 & 0xff) | 0x80, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}


/*
 * SJIS => wchar
 */
int
mbfl_filt_conv_sjis_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data TSRMLS_CC));
		} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 1:		/* kanji second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0x39 && c < 0xfd && c != 0x7f) {
			SJIS_DECODE(c1, c, s1, s2);
			w = (s1 - 0x21)*94 + s2 - 0x21;
			if (w >= 0 && w < jisx0208_ucs_table_size) {
				w = jisx0208_ucs_table[w];
			} else {
				w = 0;
			}
			if (w <= 0) {
				if (s1 < 0x7f && s2 < 0x7f) {
					w = (s1 << 8) | s2;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0208;
				} else {
					w = (c1 << 8) | c;
					w &= MBFL_WCSGROUP_MASK;
					w |= MBFL_WCSGROUP_THROUGH;
				}
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => SJIS
 */
int
mbfl_filt_conv_wchar_sjis(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, c2, s1, s2;

	s1 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	}
	if (s1 <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_JIS0208) {
			s1 = c & MBFL_WCSPLANE_MASK;
		} else if (c == 0xa5) {		/* YEN SIGN */
			s1 = 0x216f;	/* FULLWIDTH YEN SIGN */
		} else if (c == 0x203e) {	/* OVER LINE */
			s1 = 0x2131;	/* FULLWIDTH MACRON */
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s1 = 0x2141;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	} else if (s1 >= 0x8080) {
		s1 = -1;
	}
	if (s1 >= 0) {
		if (s1 < 0x100) { /* latin or kana */
			CK((*filter->output_function)(s1, filter->data TSRMLS_CC));
		} else { /* kanji */
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s2, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}


/*
 * SJIS-win => wchar
 */
int
mbfl_filt_conv_sjiswin_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s, s1, s2, w;

	switch (filter->status) {
	case 0:
		if (c >= 0 && c < 0x80) {	/* latin */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xe0) {	/* kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data TSRMLS_CC));
		} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
			filter->status = 1;
			filter->cache = c;
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	case 1:		/* kanji second char */
		filter->status = 0;
		c1 = filter->cache;
		if (c > 0x39 && c < 0xfd && c != 0x7f) {
			w = 0;
			SJIS_DECODE(c1, c, s1, s2);
			s = (s1 - 0x21)*94 + s2 - 0x21;
			if (s <= 137) {
				if (s == 31) {
					w = 0xff3c;			/* FULLWIDTH REVERSE SOLIDUS */
				} else if (s == 32) {
					w = 0xff5e;			/* FULLWIDTH TILDE */
				} else if (s == 33) {
					w = 0x2225;			/* PARALLEL TO */
				} else if (s == 60) {
					w = 0xff0d;			/* FULLWIDTH HYPHEN-MINUS */
				} else if (s == 80) {
					w = 0xffe0;			/* FULLWIDTH CENT SIGN */
				} else if (s == 81) {
					w = 0xffe1;			/* FULLWIDTH POUND SIGN */
				} else if (s == 137) {
					w = 0xffe2;			/* FULLWIDTH NOT SIGN */
				}
			}
			if (w == 0) {
				if (s >= cp932ext1_ucs_table_min && s < cp932ext1_ucs_table_max) {		/* vendor ext1 (13ku) */
					w = cp932ext1_ucs_table[s - cp932ext1_ucs_table_min];
				} else if (s >= 0 && s < jisx0208_ucs_table_size) {		/* X 0208 */
					w = jisx0208_ucs_table[s];
				} else if (s >= cp932ext2_ucs_table_min && s < cp932ext2_ucs_table_max) {		/* vendor ext2 (89ku - 92ku) */
					w = cp932ext2_ucs_table[s - cp932ext2_ucs_table_min];
				} else if (s >= cp932ext3_ucs_table_min && s < cp932ext3_ucs_table_max) {		/* vendor ext3 (115ku - 119ku) */
					w = cp932ext3_ucs_table[s - cp932ext3_ucs_table_min];
				} else if (s >= (94*94) && s < (114*94)) {		/* user (95ku - 114ku) */
					w = s - (94*94) + 0xe000;
				}
			}
			if (w <= 0) {
				w = (s1 << 8) | s2;
				w &= MBFL_WCSPLANE_MASK;
				w |= MBFL_WCSPLANE_WINCP932;
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => SJIS-win
 */
int
mbfl_filt_conv_wchar_sjiswin(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, c2, s1, s2;

	s1 = 0;
	s2 = 0;
	if (c >= ucs_a1_jis_table_min && c < ucs_a1_jis_table_max) {
		s1 = ucs_a1_jis_table[c - ucs_a1_jis_table_min];
	} else if (c >= ucs_a2_jis_table_min && c < ucs_a2_jis_table_max) {
		s1 = ucs_a2_jis_table[c - ucs_a2_jis_table_min];
	} else if (c >= ucs_i_jis_table_min && c < ucs_i_jis_table_max) {
		s1 = ucs_i_jis_table[c - ucs_i_jis_table_min];
	} else if (c >= ucs_r_jis_table_min && c < ucs_r_jis_table_max) {
		s1 = ucs_r_jis_table[c - ucs_r_jis_table_min];
	} else if (c >= 0xe000 && c < (0xe000 + 20*94)) {	/* user  (95ku - 114ku) */
		s1 = c - 0xe000;
		c1 = s1/94 + 0x7f;
		c2 = s1%94 + 0x21;
		s1 = (c1 << 8) | c2;
		s2 = 1;
	}
	if (s1 <= 0) {
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_WINCP932) {
			s1 = c & MBFL_WCSPLANE_MASK;
			s2 = 1;
		} else if (c1 == MBFL_WCSPLANE_JIS0208) {
			s1 = c & MBFL_WCSPLANE_MASK;
		} else if (c1 == MBFL_WCSPLANE_JIS0212) {
			s1 = c & MBFL_WCSPLANE_MASK;
			s1 |= 0x8080;
		} else if (c == 0xa5) {		/* YEN SIGN */
			s1 = 0x216f;	/* FULLWIDTH YEN SIGN */
		} else if (c == 0x203e) {	/* OVER LINE */
			s1 = 0x2131;	/* FULLWIDTH MACRON */
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s1 = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s1 = 0x2141;
		} else if (c == 0x2225) {	/* PARALLEL TO */
			s1 = 0x2142;
		} else if (c == 0xff0d) {	/* FULLWIDTH HYPHEN-MINUS */
			s1 = 0x215d;
		} else if (c == 0xffe0) {	/* FULLWIDTH CENT SIGN */
			s1 = 0x2171;
		} else if (c == 0xffe1) {	/* FULLWIDTH POUND SIGN */
			s1 = 0x2172;
		} else if (c == 0xffe2) {	/* FULLWIDTH NOT SIGN */
			s1 = 0x224c;
		}
	}
	if ((s1 <= 0) || (s1 >= 0x8080 && s2 == 0)) {	/* not found or X 0212 */
		s1 = -1;
		c1 = 0;
		c2 = cp932ext1_ucs_table_max - cp932ext1_ucs_table_min;
		while (c1 < c2) {		/* CP932 vendor ext1 (13ku) */
			if (c == cp932ext1_ucs_table[c1]) {
				s1 = ((c1/94 + 0x2d) << 8) + (c1%94 + 0x21);
				break;
			}
			c1++;
		}
		if (s1 <= 0) {
			c1 = 0;
			c2 = cp932ext3_ucs_table_max - cp932ext3_ucs_table_min;
			while (c1 < c2) {		/* CP932 vendor ext3 (115ku - 119ku) */
				if (c == cp932ext3_ucs_table[c1]) {
					s1 = ((c1/94 + 0x93) << 8) + (c1%94 + 0x21);
					break;
				}
				c1++;
			}
		}
		if (c == 0) {
			s1 = 0;
		} else if (s1 <= 0) {
			s1 = -1;
		}
	}
	if (s1 >= 0) {
		if (s1 < 0x100) { /* latin or kana */
			CK((*filter->output_function)(s1, filter->data TSRMLS_CC));
		} else { /* kanji */
			c1 = (s1 >> 8) & 0xff;
			c2 = s1 & 0xff;
			SJIS_ENCODE(c1, c2, s1, s2);
			CK((*filter->output_function)(s1, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s2, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}


/*
 * JIS => wchar
 */
int
mbfl_filt_conv_jis_wchar(int c, mbfl_convert_filter *filter TSRMLS_DC)
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
			CK((*filter->output_function)(0xa5, filter->data TSRMLS_CC));
		} else if (filter->status == 0x10 && c == 0x7e) {	/* OVER LINE */
			CK((*filter->output_function)(0x203e, filter->data TSRMLS_CC));
		} else if (filter->status == 0x20 && c > 0x20 && c < 0x60) {		/* kana */
			CK((*filter->output_function)(0xff40 + c, filter->data TSRMLS_CC));
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->cache = c;
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else if (c > 0xa0 && c < 0xe0) {	/* GR kana */
			CK((*filter->output_function)(0xfec0 + c, filter->data TSRMLS_CC));
		} else {
			w = c & MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
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
					w = (c1 << 8) | c;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0208;
				}
			} else {
				if (s >= 0 && s < jisx0212_ucs_table_size) {
					w = jisx0212_ucs_table[s];
				} else {
					w = 0;
				}
				if (w <= 0) {
					w = (c1 << 8) | c;
					w &= MBFL_WCSPLANE_MASK;
					w |= MBFL_WCSPLANE_JIS0212;
				}
			}
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
		} else if (c == 0x1b) {
			filter->status += 2;
		} else if ((c >= 0 && c < 0x21) || c == 0x7f) {		/* CTLs */
			CK((*filter->output_function)(c, filter->data TSRMLS_CC));
		} else {
			w = (c1 << 8) | c;
			w &= MBFL_WCSGROUP_MASK;
			w |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(w, filter->data TSRMLS_CC));
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
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
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
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));
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
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));
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
			CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));
			CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => JIS
 */
int
mbfl_filt_conv_wchar_jis(int c, mbfl_convert_filter *filter TSRMLS_DC)
{
	int c1, s;

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
		c1 = c & ~MBFL_WCSPLANE_MASK;
		if (c1 == MBFL_WCSPLANE_JIS0208) {
			s = c & MBFL_WCSPLANE_MASK;
		} else if (c1 == MBFL_WCSPLANE_JIS0212) {
			s = c & MBFL_WCSPLANE_MASK;
			s |= 0x8080;
		} else if (c == 0xa5) {		/* YEN SIGN */
			s = 0x1005c;
		} else if (c == 0x203e) {	/* OVER LINE */
			s = 0x1007e;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s = 0x2141;
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
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data TSRMLS_CC));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data TSRMLS_CC));
		} else if (s < 0x100) { /* kana */
			if ((filter->status & 0xff00) != 0x100) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
				CK((*filter->output_function)(0x49, filter->data TSRMLS_CC));		/* 'I' */
			}
			filter->status = 0x100;
			CK((*filter->output_function)(s & 0x7f, filter->data TSRMLS_CC));
		} else if (s < 0x8080) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data TSRMLS_CC));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s & 0x7f, filter->data TSRMLS_CC));
		} else if (s < 0x10000) { /* X 0212 */
			if ((filter->status & 0xff00) != 0x300) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));		/* '$' */
				CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
				CK((*filter->output_function)(0x44, filter->data TSRMLS_CC));		/* 'D' */
			}
			filter->status = 0x300;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s & 0x7f, filter->data TSRMLS_CC));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data TSRMLS_CC));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}


/*
 * wchar => ISO-2022-JP
 */
int
mbfl_filt_conv_wchar_2022jp(int c, mbfl_convert_filter *filter TSRMLS_DC)
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
		} else if (c == 0x203e) {	/* OVER LINE */
			s = 0x1007e;
		} else if (c == 0xff3c) {	/* FULLWIDTH REVERSE SOLIDUS */
			s = 0x2140;
		} else if (c == 0xff5e) {	/* FULLWIDTH TILDE */
			s = 0x2141;
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
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
				CK((*filter->output_function)(0x42, filter->data TSRMLS_CC));		/* 'B' */
			}
			filter->status = 0;
			CK((*filter->output_function)(s, filter->data TSRMLS_CC));
		} else if (s < 0x10000) { /* X 0208 */
			if ((filter->status & 0xff00) != 0x200) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x24, filter->data TSRMLS_CC));		/* '$' */
				CK((*filter->output_function)(0x42, filter->data TSRMLS_CC));		/* 'B' */
			}
			filter->status = 0x200;
			CK((*filter->output_function)((s >> 8) & 0x7f, filter->data TSRMLS_CC));
			CK((*filter->output_function)(s & 0x7f, filter->data TSRMLS_CC));
		} else { /* X 0201 latin */
			if ((filter->status & 0xff00) != 0x400) {
				CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
				CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
				CK((*filter->output_function)(0x4a, filter->data TSRMLS_CC));		/* 'J' */
			}
			filter->status = 0x400;
			CK((*filter->output_function)(s & 0x7f, filter->data TSRMLS_CC));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter TSRMLS_CC));
		}
	}

	return c;
}

int
mbfl_filt_conv_any_jis_flush(mbfl_convert_filter *filter TSRMLS_DC)
{
	/* back to latin */
	if ((filter->status & 0xff00) != 0) {
		CK((*filter->output_function)(0x1b, filter->data TSRMLS_CC));		/* ESC */
		CK((*filter->output_function)(0x28, filter->data TSRMLS_CC));		/* '(' */
		CK((*filter->output_function)(0x42, filter->data TSRMLS_CC));		/* 'B' */
	}
	filter->status &= 0xff;
	return 0;
}

#endif /* HAVE_MBSTR_JA */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
