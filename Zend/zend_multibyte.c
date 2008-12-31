/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at                              |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Masaki Fujimoto <fujimoto@php.net>                          |
   |          Rui Hirokawa <hirokawa@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_compile.h"
#include "zend_operators.h"
#include "zend_multibyte.h"

#ifdef ZEND_MULTIBYTE
static int zend_multibyte_encoding_filter(char **to, int *to_length, const char *to_encoding, const char *from, int from_length, const char *from_encoding TSRMLS_DC);
int sjis_input_filter(char **buf, int *length, const char *sjis, int sjis_length TSRMLS_DC);
int sjis_output_filter(char **buf, int *length, const char *sjis, int sjis_length TSRMLS_DC);
static char* zend_multibyte_assemble_encoding_list(zend_encoding **encoding_list, int encoding_list_size);
static int zend_multibyte_parse_encoding_list(const char *encoding_list, int encoding_list_size, zend_encoding ***result, int *result_size);
static zend_encoding* zend_multibyte_find_script_encoding(zend_encoding *onetime_encoding TSRMLS_DC);
static zend_encoding* zend_multibyte_detect_unicode(TSRMLS_D);
static zend_encoding* zend_multibyte_detect_utf_encoding(char *script, int script_size TSRMLS_DC);

/*
 * encodings
 */
const char *ucs2_aliases[] = {"ISO-10646-UCS-2", "UCS2" , "UNICODE", NULL};
zend_encoding encoding_ucs2 = {
	NULL,
	NULL,
	"UCS-2",
	(const char *(*)[])&ucs2_aliases,
	0
};

zend_encoding encoding_ucs2be = {
	NULL,
	NULL,
	"UCS-2BE",
	NULL,
	0
};

zend_encoding encoding_ucs2le = {
	NULL,
	NULL,
	"UCS-2LE",
	NULL,
	0
};

const char *ucs4_aliases[] = {"ISO-10646-UCS-4", "UCS4", NULL};
zend_encoding encoding_ucs4 = {
	NULL,
	NULL,
	"UCS-4",
	(const char *(*)[])&ucs4_aliases,
	0
};

zend_encoding encoding_ucs4be = {
	NULL,
	NULL,
	"UCS-4BE",
	NULL,
	0
};

zend_encoding encoding_ucs4le = {
	NULL,
	NULL,
	"UCS-4LE",
	NULL,
	0
};

const char *utf32_aliases[] = {"utf32", NULL};
zend_encoding encoding_utf32 = {
	NULL,
	NULL,
	"UTF-32",
	(const char *(*)[])&utf32_aliases,
	0
};

zend_encoding encoding_utf32be = {
	NULL,
	NULL,
	"UTF-32BE",
	NULL,
	0
};

zend_encoding encoding_utf32le = {
	NULL,
	NULL,
	"UTF-32LE",
	NULL,
	0
};

const char *utf16_aliases[] = {"utf16", NULL};
zend_encoding encoding_utf16 = {
	NULL,
	NULL,
	"UTF-16",
	(const char *(*)[])&utf16_aliases,
	0
};

zend_encoding encoding_utf16be = {
	NULL,
	NULL,
	"UTF-16BE",
	NULL,
	0
};

zend_encoding encoding_utf16le = {
	NULL,
	NULL,
	"UTF-16LE",
	NULL,
	0
};

const char *utf8_aliases[] = {"utf8", NULL};
zend_encoding encoding_utf8 = {
	NULL,
	NULL,
	"UTF-8",
	(const char *(*)[])&utf8_aliases,
	1
};

const char *ascii_aliases[] = {"ANSI_X3.4-1968", "iso-ir-6", "ANSI_X3.4-1986", "ISO_646.irv:1991", "US-ASCII", "ISO646-US", "us", "IBM367", "cp367", "csASCII", NULL};
zend_encoding encoding_ascii = {
	NULL,
	NULL,
	"ASCII",
	(const char *(*)[])&ascii_aliases,
	1
};

const char *euc_jp_aliases[] = {"EUC", "EUC_JP", "eucJP", "x-euc-jp", NULL};
zend_encoding encoding_euc_jp = {
	NULL,
	NULL,
	"EUC-JP",
	(const char *(*)[])&euc_jp_aliases,
	1
};

const char *sjis_aliases[] = {"x-sjis", "SJIS", "SHIFT-JIS", NULL};
zend_encoding encoding_sjis = {
	sjis_input_filter,
	sjis_output_filter,
	"Shift_JIS",
	(const char *(*)[])&sjis_aliases,
	0
};

const char *eucjp_win_aliases[] = {"eucJP-open", NULL};
zend_encoding encoding_eucjp_win = {
	NULL,
	NULL,
	"eucJP-win",
	(const char *(*)[])&eucjp_win_aliases,
	1
};

const char *sjis_win_aliases[] = {"SJIS-open", "MS_Kanji", "Windows-31J", "CP932", NULL};
zend_encoding encoding_sjis_win = {
	/* sjis-filters does not care about diffs of Shift_JIS and CP932 */
	sjis_input_filter,
	sjis_output_filter,
	"SJIS-win",
	(const char *(*)[])&sjis_win_aliases,
	0
};

const char *jis_aliases[] = {"ISO-2022-JP", NULL};
zend_encoding encoding_jis = {
	NULL,
	NULL,
	"JIS",
	(const char *(*)[])&jis_aliases,
	0
};

const char *euc_cn_aliases[] = {"CN-GB", "EUC_CN", "eucCN", "x-euc-cn", "gb2312", NULL};
zend_encoding encoding_euc_cn = {
	NULL,
	NULL,
	"EUC-CN",
	(const char *(*)[])&euc_cn_aliases,
	1
};

const char *cp936_aliases[] = {"CP-936", NULL};
zend_encoding encoding_cp936 = {
	NULL,
	NULL,
	"CP936",
	(const char *(*)[])&cp936_aliases,
	0
};

const char *hz_aliases[] = {"HZ-GB-2312", NULL};
zend_encoding encoding_hz = {
	NULL,
	NULL,
	"HZ",
	(const char *(*)[])&hz_aliases,
	0
};

const char *euc_tw_aliases[] = {"EUC_TW", "eucTW", "x-euc-tw", NULL};
zend_encoding encoding_euc_tw = {
	NULL,
	NULL,
	"EUC-TW",
	(const char *(*)[])&euc_tw_aliases,
	1
};

const char *big5_aliases[] = {"BIG5", "CN-BIG5", "BIG-FIVE", "BIGFIVE", "CP950", NULL};
zend_encoding encoding_big5 = {
	NULL,
	NULL,
	"BIG-5",
	(const char *(*)[])&big5_aliases,
	0
};

const char *euc_kr_aliases[] = {"EUC_KR", "eucKR", "x-euc-kr", NULL};
zend_encoding encoding_euc_kr = {
	NULL,
	NULL,
	"EUC-KR",
	(const char *(*)[])&euc_kr_aliases,
	1
};

const char *uhc_aliases[] = {"CP949", NULL};
zend_encoding encoding_uhc = {
	NULL,
	NULL,
	"UHC",
	(const char *(*)[])&uhc_aliases,
	1
};

zend_encoding encoding_2022kr = {
	NULL,
	NULL,
	"ISO-2022-KR",
	NULL,
	0
};

const char *cp1252_aliases[] = {"cp1252", NULL};
zend_encoding encoding_cp1252 = {
	NULL,
	NULL,
	"Windows-1252",
	(const char *(*)[])&cp1252_aliases,
	1
};

const char *iso_8859_1_aliases[] = {"ISO_8859-1", "latin1", NULL};
zend_encoding encoding_8859_1 = {
	NULL,
	NULL,
	"ISO-8859-1",
	(const char *(*)[])&iso_8859_1_aliases,
	1
};

const char *iso_8859_2_aliases[] = {"ISO_8859-2", "latin2", NULL};
zend_encoding encoding_8859_2 = {
	NULL,
	NULL,
	"ISO-8859-2",
	(const char *(*)[])&iso_8859_2_aliases,
	1
};

const char *iso_8859_3_aliases[] = {"ISO_8859-3", "latin3", NULL};
zend_encoding encoding_8859_3 = {
	NULL,
	NULL,
	"ISO-8859-3",
	(const char *(*)[])&iso_8859_3_aliases,
	1
};

const char *iso_8859_4_aliases[] = {"ISO_8859-4", "latin4", NULL};
zend_encoding encoding_8859_4 = {
	NULL,
	NULL,
	"ISO-8859-4",
	(const char *(*)[])&iso_8859_4_aliases,
	1
};

const char *iso_8859_5_aliases[] = {"ISO_8859-5", "cyrillic", NULL};
zend_encoding encoding_8859_5 = {
	NULL,
	NULL,
	"ISO-8859-5",
	(const char *(*)[])&iso_8859_5_aliases,
	1
};

const char *iso_8859_6_aliases[] = {"ISO_8859-6", "arabic", NULL};
zend_encoding encoding_8859_6 = {
	NULL,
	NULL,
	"ISO-8859-6",
	(const char *(*)[])&iso_8859_6_aliases,
	1
};

const char *iso_8859_7_aliases[] = {"ISO_8859-7", "greek", NULL};
zend_encoding encoding_8859_7 = {
	NULL,
	NULL,
	"ISO-8859-7",
	(const char *(*)[])&iso_8859_7_aliases,
	1
};

const char *iso_8859_8_aliases[] = {"ISO_8859-8", "hebrew", NULL};
zend_encoding encoding_8859_8 = {
	NULL,
	NULL,
	"ISO-8859-8",
	(const char *(*)[])&iso_8859_8_aliases,
	1
};

const char *iso_8859_9_aliases[] = {"ISO_8859-9", "latin5", NULL};
zend_encoding encoding_8859_9 = {
	NULL,
	NULL,
	"ISO-8859-9",
	(const char *(*)[])&iso_8859_9_aliases,
	1
};

const char *iso_8859_10_aliases[] = {"ISO_8859-10", "latin6", NULL};
zend_encoding encoding_8859_10 = {
	NULL,
	NULL,
	"ISO-8859-10",
	(const char *(*)[])&iso_8859_10_aliases,
	1
};

const char *iso_8859_13_aliases[] = {"ISO_8859-13", NULL};
zend_encoding encoding_8859_13 = {
	NULL,
	NULL,
	"ISO-8859-13",
	(const char *(*)[])&iso_8859_13_aliases,
	1
};

const char *iso_8859_14_aliases[] = {"ISO_8859-14", "latin8", NULL};
zend_encoding encoding_8859_14 = {
	NULL,
	NULL,
	"ISO-8859-14",
	(const char *(*)[])&iso_8859_14_aliases,
	1
};

const char *iso_8859_15_aliases[] = {"ISO_8859-15", NULL};
zend_encoding encoding_8859_15 = {
	NULL,
	NULL,
	"ISO-8859-15",
	(const char *(*)[])&iso_8859_15_aliases,
	1
};

const char *cp1251_aliases[] = {"CP1251", "CP-1251", "WINDOWS-1251", NULL};
zend_encoding encoding_cp1251 = {
	NULL,
	NULL,
	"Windows-1251",
	(const char *(*)[])&cp1251_aliases,
	1
};

const char *cp866_aliases[] = {"CP866", "CP-866", "IBM-866", NULL};
zend_encoding encoding_cp866 = {
	NULL,
	NULL,
	"CP866",
	(const char *(*)[])&cp866_aliases,
	1
};

const char *koi8r_aliases[] = {"KOI8-R", "KOI8R", NULL};
zend_encoding encoding_koi8r = {
	NULL,
	NULL,
	"KOI8-R",
	(const char *(*)[])&koi8r_aliases,
	1
};

zend_encoding *zend_encoding_table[] = {
	&encoding_ucs4,
	&encoding_ucs4be,
	&encoding_ucs4le,
	&encoding_ucs2,
	&encoding_ucs2be,
	&encoding_ucs2le,
	&encoding_utf32,
	&encoding_utf32be,
	&encoding_utf32le,
	&encoding_utf16,
	&encoding_utf16be,
	&encoding_utf16le,
	&encoding_utf8,
	&encoding_ascii,
	&encoding_euc_jp,
	&encoding_sjis,
	&encoding_eucjp_win,
	&encoding_sjis_win,
	&encoding_jis,
	&encoding_cp1252,
	&encoding_8859_1,
	&encoding_8859_2,
	&encoding_8859_3,
	&encoding_8859_4,
	&encoding_8859_5,
	&encoding_8859_6,
	&encoding_8859_7,
	&encoding_8859_8,
	&encoding_8859_9,
	&encoding_8859_10,
	&encoding_8859_13,
	&encoding_8859_14,
	&encoding_8859_15,
	&encoding_euc_cn,
	&encoding_cp936,
	&encoding_hz,
	&encoding_euc_tw,
	&encoding_big5,
	&encoding_euc_kr,
	&encoding_uhc,
	&encoding_2022kr,
	&encoding_cp1251,
	&encoding_cp866,
	&encoding_koi8r,
	NULL
};



ZEND_API int zend_multibyte_set_script_encoding(char *encoding_list, int encoding_list_size TSRMLS_DC)
{
	if (CG(script_encoding_list)) {
		efree(CG(script_encoding_list));
		CG(script_encoding_list) = NULL;
	}
	CG(script_encoding_list_size) = 0;

	if (!encoding_list) {
		return 0;
	}

	zend_multibyte_parse_encoding_list(encoding_list, encoding_list_size, &(CG(script_encoding_list)), &(CG(script_encoding_list_size)));

	return 0;
}


ZEND_API int zend_multibyte_set_internal_encoding(char *encoding_name, int encoding_name_size TSRMLS_DC)
{
	CG(internal_encoding) = zend_multibyte_fetch_encoding(encoding_name);
	return 0;
}

ZEND_API int zend_multibyte_set_functions(zend_encoding_detector encoding_detector, zend_encoding_converter encoding_converter, zend_encoding_oddlen encoding_oddlen TSRMLS_DC)
{
	CG(encoding_detector) = encoding_detector;
	CG(encoding_converter) = encoding_converter;
	CG(encoding_oddlen) = encoding_oddlen;
	return 0;
}


ZEND_API int zend_multibyte_set_filter(zend_encoding *onetime_encoding TSRMLS_DC)
{
	LANG_SCNG(script_encoding) = zend_multibyte_find_script_encoding(onetime_encoding TSRMLS_CC);
	LANG_SCNG(internal_encoding) = CG(internal_encoding);

	/* judge input/output filter */
	LANG_SCNG(input_filter) = NULL;
	LANG_SCNG(output_filter) = NULL;

	if (!LANG_SCNG(script_encoding)) {
		return 0;
	}

	if (!LANG_SCNG(internal_encoding) || LANG_SCNG(script_encoding) == LANG_SCNG(internal_encoding)) {
		/* if encoding specfic filters exist, use them */
		if (LANG_SCNG(script_encoding)->input_filter && LANG_SCNG(script_encoding)->output_filter) {
			LANG_SCNG(input_filter) = LANG_SCNG(script_encoding)->input_filter;
			LANG_SCNG(output_filter) = LANG_SCNG(script_encoding)->output_filter;
			return 0;
		}

		if (!LANG_SCNG(script_encoding)->compatible) {
			/* and if not, work around w/ script_encoding -> utf-8 -> script_encoding conversion */
			LANG_SCNG(internal_encoding) = LANG_SCNG(script_encoding);
			LANG_SCNG(input_filter) = zend_multibyte_script_encoding_filter;
			LANG_SCNG(output_filter) = zend_multibyte_internal_encoding_filter;
			return 0;
		} else {
			/* nothing to do in this case */
			return 0;
		}
	}

	/* LANG_SCNG(internal_encoding) cannot be NULL here */
	if (LANG_SCNG(internal_encoding)->compatible) {
		LANG_SCNG(input_filter) = zend_multibyte_script_encoding_filter;
		return 0;
	} else if (LANG_SCNG(script_encoding)->compatible) {
		LANG_SCNG(output_filter) = zend_multibyte_internal_encoding_filter;
		return 0;
	}

	/* both script and internal encodings are incompatible w/ flex */
	LANG_SCNG(input_filter) = zend_multibyte_script_encoding_filter;
	LANG_SCNG(output_filter) = zend_multibyte_internal_encoding_filter;

	return 0;
}


ZEND_API zend_encoding* zend_multibyte_fetch_encoding(char *encoding_name)
{
	int i, j;
	zend_encoding *encoding;

	if (!encoding_name) {
		return NULL;
	}

	for (i = 0; (encoding = zend_encoding_table[i]) != NULL; i++) {
		if (zend_binary_strcasecmp((char*)encoding->name, strlen(encoding->name), encoding_name, strlen(encoding_name)) == 0) {
			return encoding;
		}
	}

	for (i = 0; (encoding = zend_encoding_table[i]) != NULL; i++) {
		if (encoding->aliases != NULL) {
			for (j = 0; (*encoding->aliases)[j] != NULL; j++) {
				if (zend_binary_strcasecmp((char*)(*encoding->aliases)[j], strlen((*encoding->aliases)[j]), encoding_name, strlen(encoding_name)) == 0) {
					return encoding;
				}
			}
		}
	}

	return NULL;
}


ZEND_API int zend_multibyte_script_encoding_filter(char **to, int *to_length, const char *from, int from_length TSRMLS_DC)
{
	const char *name;

	if (LANG_SCNG(internal_encoding) == NULL || LANG_SCNG(internal_encoding)->compatible == 0) {
		name = "UTF-8";
	} else {
		name = LANG_SCNG(internal_encoding)->name;
	}

	return zend_multibyte_encoding_filter(to, to_length, name, from, from_length, LANG_SCNG(script_encoding)->name TSRMLS_CC);
}

ZEND_API int zend_multibyte_internal_encoding_filter(char **to, int *to_length, const char *from, int from_length TSRMLS_DC)
{
	const char *name;

	if (LANG_SCNG(script_encoding)->compatible == 0) {
		name = "UTF-8";
	} else {
		name = LANG_SCNG(script_encoding)->name;
	}

	return zend_multibyte_encoding_filter(to, to_length, LANG_SCNG(internal_encoding)->name, from, from_length, name TSRMLS_CC);
}

static int zend_multibyte_encoding_filter(char **to, int *to_length, const char *to_encoding, const char *from, int from_length, const char *from_encoding TSRMLS_DC)
{
	int oddlen;

	if (!CG(encoding_converter)) {
		return 0;
	}

	if (CG(encoding_oddlen)) {
		oddlen = CG(encoding_oddlen)(from, from_length, from_encoding TSRMLS_CC);
		if (oddlen > 0) {
			from_length -= oddlen;
		}
	}

	if (CG(encoding_converter)(to, to_length, from, from_length, to_encoding, from_encoding TSRMLS_CC) != 0) {
		return 0;
	}

	return from_length;
}


/*
 *	Shift_JIS Input/Output Filter
 */
static const unsigned char table_sjis[] = { /* 0x80-0x9f,0xE0-0xEF */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 0, 0, 0
};

int sjis_input_filter(char **buf, int *length, const char *sjis, int sjis_length TSRMLS_DC)
{
	unsigned char *p, *q;
	unsigned char  c1, c2;

	*buf = (char*)emalloc(sjis_length*3/2+1);
	if (!*buf)
		return 0;
	*length = 0;

	p = (unsigned char*)sjis;
	q = (unsigned char*)*buf;

	/* convert [SJIS -> EUC-JP] (for lex scan) -- some other better ways? */
	while (*p && (p-(unsigned char*)sjis) < sjis_length) {
		if (!(*p & 0x80)) {
			*q++ = *p++;
			continue;
		}

		/* handling 8 bit code */
		if (table_sjis[*p] == 1) {
			/* 1 byte kana */
			*q++ = 0x8e;
			*q++ = *p++;
			continue;
		}

		if (!*(p+1)) {
			*q++ = *p++;
			break;
		}

		if (table_sjis[*p] == 2) {
			/* 2 byte kanji code */
			c1 = *p++;
			if (!*p || (p-(unsigned char*)sjis) >= sjis_length) {
				break;
			}
			c2 = *p++;
			c1 -= (c1 <= 0x9f) ? 0x71 : 0xb1;
			c1 = (c1 << 1) + 1;
			if (c2 >= 0x9e) {
				c2 -= 0x7e;
				c1++;
			} else if (c2 > 0x7f) {
				c2 -= 0x20;
			} else {
				c2 -= 0x1f;
			}

			c1 |= 0x80;
			c2 |= 0x80;

			*q++ = c1;
			*q++ = c2;
		} else {
			/*
			 * for user defined chars (ATTENTION)
			 *
			 * THESE ARE NOT CODE FOR CONVERSION! :-P
			 * (using *ILLEGALLY* 3byte EUC-JP space)
			 *
			 * we cannot perfectly (== 1 to 1)  convert these chars to EUC-JP.
			 * so, these code are for perfect RESTORING in sjis_output_filter()
			 */
			c1 = *p++;
			if (!*p || (p-(unsigned char*)sjis) >= sjis_length) {
				break;
			}
			c2 = *p++;
			*q++ = (char)0x8f;
			/*
			 * MAP TO (EUC-JP):
			 * type A: 0xeba1 - 0xf4fe
			 * type B: 0xf5a1 - 0xfefe
			 * type C: 0xa1a1 - 0xa6fe
			 */
			c1 -= (c1 > 0xf9) ? (0x79+0x71) : (0x0a+0xb1);
			c1 = (c1 << 1) + 1;
			if (c2 >= 0x9e) {
				c2 -= 0x7e;
				c1++;
			} else if (c2 > 0x7f) {
				c2 -= 0x20;
			} else {
				c2 -= 0x1f;
			}
			
			c1 |= 0x80;
			c2 |= 0x80;

			*q++ = c1;
			*q++ = c2;
		}
	}
	*q = (char)NULL;
	*length = (char*)q - *buf;

	return *length;
}

static const unsigned char table_eucjp[] = { /* 0xA1-0xFE */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

int sjis_output_filter(char **sjis, int *sjis_length, const char *buf, int length TSRMLS_DC)
{
	unsigned char c1, c2;
	char *p;
	const char *q;

	if (!sjis || !sjis_length) {
		return 0;
	}

	/* always Shift_JIS <= EUC-JP */
	*sjis = (char*)emalloc(length+1);
	if (!sjis) {
		return 0;
	}
	p = *sjis;
	q = buf;

	/* restore converted strings [EUC-JP -> Shift_JIS] */
	while (*q) {
		if (!(*q & 0x80)) {
			*p++ = *q++;
			continue;
		}

		/* hankaku kana */
		if (*q == (char)0x8e) {
			q++;
			if (*q) {
				*p++ = *q++;
			}
			continue;
		}

		/* 2 byte kanji code */
		if (table_eucjp[(unsigned char)*q] == 2) {
			c1 = (*q++ & ~0x80) & 0xff;
			if (*q) {
				c2 = (*q++ & ~0x80) & 0xff;
			} else {
				q--;
				break;
			}

			c2 += (c1 & 0x01) ? 0x1f : 0x7d;
			if (c2 >= 0x7f) {
				c2++;
			}
			c1 = ((c1 - 0x21) >> 1) + 0x81;
			if (c1 > 0x9f) {
				c1 += 0x40;
			}
			
			*p++ = c1;
			*p++ = c2;
			continue;
		}

		if (*q == (char)0x8f) {
			q++;
			if (*q) {
				c1 = (*q++ & ~0x80) & 0xff;
			} else {
				q--;
				break;
			}
			if (*q) {
				c2 = (*q++ & ~0x80) & 0xff;
			} else {
				q -= 2;
				break;
			}
			
			c2 += (c1 & 0x01) ? 0x1f : 0x7d;
			if (c2 >= 0x7f) {
				c2++;
			}
			c1 = ((c1 - 0x21) >> 1) + 0x81;
			if (c1 > 0x9f) {
				c1 += 0x40;
			}
			
			if (c1 >= 0x81 && c1 <= 0x9f) {
				c1 += 0x79;
			} else {
				c1 += 0x0a;
			}
			
			*p++ = c1;
			*p++ = c2;
			continue;
		}

		/* some other chars (may not happen) */
		*p++ = *q++;
	}
	*p = '\0';
	*sjis_length = p - *sjis;

	return q-buf;	/* return length we actually read */
}


static char* zend_multibyte_assemble_encoding_list(zend_encoding **encoding_list, int encoding_list_size)
{
	int i, list_size = 0;
	const char *name;
	char *list = NULL;

	if (!encoding_list || !encoding_list_size) {
		return NULL;
	}

	for (i = 0; i < encoding_list_size; i++) {
		name = (*(encoding_list+i))->name;
		if (name) {
			list_size += strlen(name) + 1;
			if (!list) {
				list = (char*)emalloc(list_size);
				if (!list) {
					return NULL;
				}
				*list = (char)NULL;
			} else {
				list = (char*)erealloc(list, list_size);
				if (!list) {
					return NULL;
				}
				strcat(list, ",");
			}
			strcat(list, name);
		}
	}
	return list;
}


static int zend_multibyte_parse_encoding_list(const char *encoding_list, int encoding_list_size, zend_encoding ***result, int *result_size)
{
	int n, size;
	char *p, *p1, *p2, *endp, *tmpstr;
	zend_encoding **list, **entry, *encoding;

	list = NULL;
	if (encoding_list == NULL || encoding_list_size <= 0) {
		return -1;
	} else {
		/* copy the encoding_list string for work */
		tmpstr = (char *)estrndup(encoding_list, encoding_list_size);
		if (tmpstr == NULL) {
			return -1;
		}
		/* count the number of listed encoding names */
		endp = tmpstr + encoding_list_size;
		n = 1;
		p1 = tmpstr;
		while ((p2 = zend_memnstr(p1, ",", 1, endp)) != NULL) {
			p1 = p2 + 1;
			n++;
		}
		size = n;
		/* make list */
		list = (zend_encoding**)ecalloc(size, sizeof(zend_encoding*));
		if (list != NULL) {
			entry = list;
			n = 0;
			p1 = tmpstr;
			do {
				p2 = p = zend_memnstr(p1, ",", 1, endp);
				if (p == NULL) {
					p = endp;
				}
				*p = '\0';
				/* trim spaces */
				while (p1 < p && (*p1 == ' ' || *p1 == '\t')) {
					p1++;
				}
				p--;
				while (p > p1 && (*p == ' ' || *p == '\t')) {
					*p = '\0';
					p--;
				}
				/* convert to the encoding number and check encoding */
				encoding = zend_multibyte_fetch_encoding(p1);
				if (encoding)
				{
					*entry++ = encoding;
					n++;
				}
				p1 = p2 + 1;
			} while (n < size && p2 != NULL);
			*result = list;
			*result_size = n;
		}
		efree(tmpstr);
	}

	if (list == NULL) {
		return -1;
	}

	return 0;
}


static zend_encoding* zend_multibyte_find_script_encoding(zend_encoding *onetime_encoding TSRMLS_DC)
{
	zend_encoding *script_encoding;
	char *name, *list;

	/* onetime_encoding is prior to everything */
	if (onetime_encoding != NULL) {
		return onetime_encoding;
	}

	if (CG(detect_unicode)) {
		/* check out bom(byte order mark) and see if containing wchars */
		script_encoding = zend_multibyte_detect_unicode(TSRMLS_C);
		if (script_encoding != NULL) {
			/* bom or wchar detection is prior to 'script_encoding' option */
			return script_encoding;
		}
	}

	/* if no script_encoding specified, just leave alone */
	if (!CG(script_encoding_list) || !CG(script_encoding_list_size)) {
		return NULL;
	}

	/* if multiple encodings specified, detect automagically */
	if (CG(script_encoding_list_size) > 1 && CG(encoding_detector)) {
		list = zend_multibyte_assemble_encoding_list(CG(script_encoding_list),
				CG(script_encoding_list_size));
		name = CG(encoding_detector)(LANG_SCNG(script_org), 
				LANG_SCNG(script_org_size), list TSRMLS_CC);
		if (list) {
			efree(list);
		}
		if (name) {
			script_encoding = zend_multibyte_fetch_encoding(name);
			efree(name);
		} else {
			script_encoding = NULL;
		}
		return script_encoding;
	}

	return *(CG(script_encoding_list));
}


static zend_encoding* zend_multibyte_detect_unicode(TSRMLS_D)
{
	zend_encoding *script_encoding = NULL;
	int bom_size;
	char *script;

	if (LANG_SCNG(script_org_size) < sizeof(BOM_UTF32_LE)-1) {
		return NULL;
	}

	/* check out BOM */
	if (!memcmp(LANG_SCNG(script_org), BOM_UTF32_BE, sizeof(BOM_UTF32_BE)-1)) {
		script_encoding = &encoding_utf32be;
		bom_size = sizeof(BOM_UTF32_BE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF32_LE, sizeof(BOM_UTF32_LE)-1)) {
		script_encoding = &encoding_utf32le;
		bom_size = sizeof(BOM_UTF32_LE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF16_BE, sizeof(BOM_UTF16_BE)-1)) {
		script_encoding = &encoding_utf16be;
		bom_size = sizeof(BOM_UTF16_BE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF16_LE, sizeof(BOM_UTF16_LE)-1)) {
		script_encoding = &encoding_utf16le;
		bom_size = sizeof(BOM_UTF16_LE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF8, sizeof(BOM_UTF8)-1)) {
		script_encoding = &encoding_utf8;
		bom_size = sizeof(BOM_UTF8)-1;
	}

	if (script_encoding) {
		/* remove BOM */
		script = (char*)emalloc(LANG_SCNG(script_org_size)+1-bom_size);
		memcpy(script, LANG_SCNG(script_org)+bom_size, LANG_SCNG(script_org_size)+1-bom_size);
		efree(LANG_SCNG(script_org));
		LANG_SCNG(script_org) = script;
		LANG_SCNG(script_org_size) -= bom_size;

		return script_encoding;
	}

	/* script contains NULL bytes -> auto-detection */
	if (memchr(LANG_SCNG(script_org), 0, LANG_SCNG(script_org_size))) {
		/* make best effort if BOM is missing */
		return zend_multibyte_detect_utf_encoding(LANG_SCNG(script_org), LANG_SCNG(script_org_size) TSRMLS_CC);
	}

	return NULL;
}

static zend_encoding* zend_multibyte_detect_utf_encoding(char *script, int script_size TSRMLS_DC)
{
	char *p;
	int wchar_size = 2;
	int le = 0;

	/* utf-16 or utf-32? */
	p = script;
	while ((p-script) < script_size) {
		p = memchr(p, 0, script_size-(p-script)-2);
		if (!p) {
			break;
		}
		if (*(p+1) == (char)NULL && *(p+2) == (char)NULL) {
			wchar_size = 4;
			break;
		}

		/* searching for UTF-32 specific byte orders, so this will do */
		p += 4;
	}

	/* BE or LE? */
	p = script;
	while ((p-script) < script_size) {
		if (*p == (char)NULL && *(p+wchar_size-1) != (char)NULL) {
			/* BE */
			le = 0;
			break;
		} else if (*p != (char)NULL && *(p+wchar_size-1) == (char)NULL) {
			/* LE* */
			le = 1;
			break;
		}
		p += wchar_size;
	}

	if (wchar_size == 2) {
		return le ? &encoding_utf16le : &encoding_utf16be;
	} else {
		return le ? &encoding_utf32le : &encoding_utf32be;
	}

	return NULL;
}
#endif /* ZEND_MULTIBYTE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78
 * vim<600: sw=4 ts=4 tw=78
 */
