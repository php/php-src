/* charset=UTF-8
 * vim: encoding=utf-8:
 * */

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


#include <stdlib.h>
#include "mbfilter.h"
#include "mbfilter_ja.h"
#include "zend.h"

#ifdef PHP_WIN32
#include "win95nt.h"
#endif

#define	mbfl_malloc		emalloc
#define	mbfl_realloc	erealloc
#define	mbfl_calloc		ecalloc
#define	mbfl_free		efree

#define	mbfl_pmalloc	malloc
#define	mbfl_prealloc	realloc
#define	mbfl_pfree		free

/* unicode table */
extern const unsigned short cp1252_ucs_table[];
extern const unsigned short iso8859_2_ucs_table[];
extern const unsigned short iso8859_3_ucs_table[];
extern const unsigned short iso8859_4_ucs_table[];
extern const unsigned short iso8859_5_ucs_table[];
extern const unsigned short iso8859_6_ucs_table[];
extern const unsigned short iso8859_7_ucs_table[];
extern const unsigned short iso8859_8_ucs_table[];
extern const unsigned short iso8859_9_ucs_table[];
extern const unsigned short iso8859_10_ucs_table[];
extern const unsigned short iso8859_13_ucs_table[];
extern const unsigned short iso8859_14_ucs_table[];
extern const unsigned short iso8859_15_ucs_table[];

/* charactor property table */
#define MBFL_CHP_CTL		0x01
#define MBFL_CHP_DIGIT		0x02
#define MBFL_CHP_UALPHA		0x04
#define MBFL_CHP_LALPHA		0x08
#define MBFL_CHP_MMHQENC	0x10	/* must Q-encoding in MIME Header encoded-word */
#define MBFL_CHP_MSPECIAL	0x20	/* RFC822 Special characters */

extern const unsigned char mbfl_charprop_table[];


/* language structure */
static const char *mbfl_language_uni_aliases[] = {"universal", "none", NULL};

static mbfl_language mbfl_language_uni = {
	mbfl_no_language_uni,
	"uni",
	"uni",
	&mbfl_language_uni_aliases,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_base64
};

static mbfl_language mbfl_language_japanese = {
	mbfl_no_language_japanese,
	"Japanese",
	"ja",
	NULL,
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_7bit
};

static mbfl_language mbfl_language_english = {
	mbfl_no_language_english,
	"English",
	"en",
	NULL,
	mbfl_no_encoding_8859_1,
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit
};

static mbfl_language *mbfl_language_ptr_table[] = {
	&mbfl_language_uni,
	&mbfl_language_japanese,
	&mbfl_language_english,
	NULL
};


/* encoding byte width table */
static const unsigned char mblen_table_utf8[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
};

static const unsigned char mblen_table_eucjp[] = { /* 0xA1-0xFE */
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

static const unsigned char mblen_table_sjis[] = { /* 0x80-0x9f,0xE0-0xFF */
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
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};


/* encoding structure */
static const char *mbfl_encoding_pass_aliases[] = {"none", NULL};

static mbfl_encoding mbfl_encoding_pass = {
	mbfl_no_encoding_pass,
	"pass",
	NULL,
	&mbfl_encoding_pass_aliases,
	NULL,
	0
};

static const char *mbfl_encoding_auto_aliases[] = {"unknown", NULL};

static mbfl_encoding mbfl_encoding_auto = {
	mbfl_no_encoding_auto,
	"auto",
	NULL,
	&mbfl_encoding_auto_aliases,
	NULL,
	0
};

static mbfl_encoding mbfl_encoding_wchar = {
	mbfl_no_encoding_wchar,
	"wchar",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4BE
};

static mbfl_encoding mbfl_encoding_byte2be = {
	mbfl_no_encoding_byte2be,
	"byte2be",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_byte2le = {
	mbfl_no_encoding_byte2le,
	"byte2le",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_byte4be = {
	mbfl_no_encoding_byte4be,
	"byte4be",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_byte4le = {
	mbfl_no_encoding_byte4le,
	"byte4le",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_base64 = {
	mbfl_no_encoding_base64,
	"BASE64",
	"BASE64",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_uuencode = {
	mbfl_no_encoding_uuencode,
	"UUENCODE",
	"x-uuencode",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_qprint_aliases[] = {"qprint", NULL};

static mbfl_encoding mbfl_encoding_qprint = {
	mbfl_no_encoding_qprint,
	"Quoted-Printable",
	"Quoted-Printable",
	&mbfl_encoding_qprint_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_7bit = {
	mbfl_no_encoding_7bit,
	"7bit",
	"7bit",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding mbfl_encoding_8bit = {
	mbfl_no_encoding_8bit,
	"8bit",
	"8bit",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_ucs2_aliases[] = {"ISO-10646-UCS-2", "UCS2" , "UNICODE", NULL};

static mbfl_encoding mbfl_encoding_ucs2 = {
	mbfl_no_encoding_ucs2,
	"UCS-2",
	"UCS-2",
	&mbfl_encoding_ucs2_aliases,
	NULL,
	MBFL_ENCTYPE_WCS2BE
};

static mbfl_encoding mbfl_encoding_ucs2be = {
	mbfl_no_encoding_ucs2be,
	"UCS-2BE",
	"UCS-2BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS2BE
};

static mbfl_encoding mbfl_encoding_ucs2le = {
	mbfl_no_encoding_ucs2le,
	"UCS-2LE",
	"UCS-2LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS2LE
};

static const char *mbfl_encoding_ucs4_aliases[] = {"ISO-10646-UCS-4", "UCS4", NULL};

static mbfl_encoding mbfl_encoding_ucs4 = {
	mbfl_no_encoding_ucs4,
	"UCS-4",
	"UCS-4",
	&mbfl_encoding_ucs4_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4BE
};

static mbfl_encoding mbfl_encoding_ucs4be = {
	mbfl_no_encoding_ucs4be,
	"UCS-4BE",
	"UCS-4BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4BE
};

static mbfl_encoding mbfl_encoding_ucs4le = {
	mbfl_no_encoding_ucs4le,
	"UCS-4LE",
	"UCS-4LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4LE
};

static const char *mbfl_encoding_utf32_aliases[] = {"utf32", NULL};

static mbfl_encoding mbfl_encoding_utf32 = {
	mbfl_no_encoding_utf32,
	"UTF-32",
	"UTF-32",
	&mbfl_encoding_utf32_aliases,
	NULL,
	MBFL_ENCTYPE_WCS4BE
};

static mbfl_encoding mbfl_encoding_utf32be = {
	mbfl_no_encoding_utf32be,
	"UTF-32BE",
	"UTF-32BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4BE
};

static mbfl_encoding mbfl_encoding_utf32le = {
	mbfl_no_encoding_utf32le,
	"UTF-32LE",
	"UTF-32LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_WCS4LE
};

static const char *mbfl_encoding_utf16_aliases[] = {"utf16", NULL};

static mbfl_encoding mbfl_encoding_utf16 = {
	mbfl_no_encoding_utf16,
	"UTF-16",
	"UTF-16",
	&mbfl_encoding_utf16_aliases,
	NULL,
	MBFL_ENCTYPE_MWC2BE
};

static mbfl_encoding mbfl_encoding_utf16be = {
	mbfl_no_encoding_utf16be,
	"UTF-16BE",
	"UTF-16BE",
	NULL,
	NULL,
	MBFL_ENCTYPE_MWC2BE
};

static mbfl_encoding mbfl_encoding_utf16le = {
	mbfl_no_encoding_utf16le,
	"UTF-16LE",
	"UTF-16LE",
	NULL,
	NULL,
	MBFL_ENCTYPE_MWC2LE
};

static const char *mbfl_encoding_utf8_aliases[] = {"utf8", NULL};

static mbfl_encoding mbfl_encoding_utf8 = {
	mbfl_no_encoding_utf8,
	"UTF-8",
	"UTF-8",
	&mbfl_encoding_utf8_aliases,
	mblen_table_utf8,
	MBFL_ENCTYPE_MBCS
};

static const char *mbfl_encoding_utf7_aliases[] = {"utf7", NULL};

static mbfl_encoding mbfl_encoding_utf7 = {
	mbfl_no_encoding_utf7,
	"UTF-7",
	"UTF-7",
	&mbfl_encoding_utf7_aliases,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE
};

static mbfl_encoding mbfl_encoding_utf7imap = {
	mbfl_no_encoding_utf7imap,
	"UTF7-IMAP",
	NULL,
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE
};

static mbfl_encoding mbfl_encoding_ascii = {
	mbfl_no_encoding_ascii,
	"ASCII",
	"US-ASCII",
	NULL,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_euc_jp_aliases[] = {"EUC", "EUC_JP", "eucJP", "x-euc-jp", NULL};

static mbfl_encoding mbfl_encoding_euc_jp = {
	mbfl_no_encoding_euc_jp,
	"EUC-JP",
	"EUC-JP",
	&mbfl_encoding_euc_jp_aliases,
	mblen_table_eucjp,
	MBFL_ENCTYPE_MBCS
};

static const char *mbfl_encoding_sjis_aliases[] = {"x-sjis", "SHIFT-JIS", NULL};

static mbfl_encoding mbfl_encoding_sjis = {
	mbfl_no_encoding_sjis,
	"SJIS",
	"Shift_JIS",
	&mbfl_encoding_sjis_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_MBCS
};

static const char *mbfl_encoding_eucjp_win_aliases[] = {"eucJP-open", NULL};

static mbfl_encoding mbfl_encoding_eucjp_win = {
	mbfl_no_encoding_eucjp_win,
	"eucJP-win",
	"EUC-JP",
	&mbfl_encoding_eucjp_win_aliases,
	mblen_table_eucjp,
	MBFL_ENCTYPE_MBCS
};

static const char *mbfl_encoding_sjis_win_aliases[] = {"SJIS-open", "MS_Kanji", "Windows-31J", NULL};

static mbfl_encoding mbfl_encoding_sjis_win = {
	mbfl_no_encoding_sjis_win,
	"SJIS-win",
	"Shift_JIS",
	&mbfl_encoding_sjis_win_aliases,
	mblen_table_sjis,
	MBFL_ENCTYPE_MBCS
};

static mbfl_encoding mbfl_encoding_jis = {
	mbfl_no_encoding_jis,
	"JIS",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE
};

static mbfl_encoding mbfl_encoding_2022jp = {
	mbfl_no_encoding_2022jp,
	"ISO-2022-JP",
	"ISO-2022-JP",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_SHFTCODE
};

static const char *mbfl_encoding_cp1252_aliases[] = {"cp1252", NULL};

static mbfl_encoding mbfl_encoding_cp1252 = {
	mbfl_no_encoding_cp1252,
	"Windows-1252",
	"Windows-1252",
	&mbfl_encoding_cp1252_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_1_aliases[] = {"ISO_8859-1", "latin1", NULL};

static mbfl_encoding mbfl_encoding_8859_1 = {
	mbfl_no_encoding_8859_1,
	"ISO-8859-1",
	"ISO-8859-1",
	&mbfl_encoding_8859_1_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_2_aliases[] = {"ISO_8859-2", "latin2", NULL};

static mbfl_encoding mbfl_encoding_8859_2 = {
	mbfl_no_encoding_8859_2,
	"ISO-8859-2",
	"ISO-8859-2",
	&mbfl_encoding_8859_2_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_3_aliases[] = {"ISO_8859-3", "latin3", NULL};

static mbfl_encoding mbfl_encoding_8859_3 = {
	mbfl_no_encoding_8859_3,
	"ISO-8859-3",
	"ISO-8859-3",
	&mbfl_encoding_8859_3_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_4_aliases[] = {"ISO_8859-4", "latin4", NULL};

static mbfl_encoding mbfl_encoding_8859_4 = {
	mbfl_no_encoding_8859_4,
	"ISO-8859-4",
	"ISO-8859-4",
	&mbfl_encoding_8859_4_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_5_aliases[] = {"ISO_8859-5", "cyrillic", NULL};

static mbfl_encoding mbfl_encoding_8859_5 = {
	mbfl_no_encoding_8859_5,
	"ISO-8859-5",
	"ISO-8859-5",
	&mbfl_encoding_8859_5_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_6_aliases[] = {"ISO_8859-6", "arabic", NULL};

static mbfl_encoding mbfl_encoding_8859_6 = {
	mbfl_no_encoding_8859_6,
	"ISO-8859-6",
	"ISO-8859-6",
	&mbfl_encoding_8859_6_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_7_aliases[] = {"ISO_8859-7", "greek", NULL};

static mbfl_encoding mbfl_encoding_8859_7 = {
	mbfl_no_encoding_8859_7,
	"ISO-8859-7",
	"ISO-8859-7",
	&mbfl_encoding_8859_7_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_8_aliases[] = {"ISO_8859-8", "hebrew", NULL};

static mbfl_encoding mbfl_encoding_8859_8 = {
	mbfl_no_encoding_8859_8,
	"ISO-8859-8",
	"ISO-8859-8",
	&mbfl_encoding_8859_8_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_9_aliases[] = {"ISO_8859-9", "latin5", NULL};

static mbfl_encoding mbfl_encoding_8859_9 = {
	mbfl_no_encoding_8859_9,
	"ISO-8859-9",
	"ISO-8859-9",
	&mbfl_encoding_8859_9_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_10_aliases[] = {"ISO_8859-10", "latin6", NULL};

static mbfl_encoding mbfl_encoding_8859_10 = {
	mbfl_no_encoding_8859_10,
	"ISO-8859-10",
	"ISO-8859-10",
	&mbfl_encoding_8859_10_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_13_aliases[] = {"ISO_8859-13", NULL};

static mbfl_encoding mbfl_encoding_8859_13 = {
	mbfl_no_encoding_8859_13,
	"ISO-8859-13",
	"ISO-8859-13",
	&mbfl_encoding_8859_13_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_14_aliases[] = {"ISO_8859-14", "latin8", NULL};

static mbfl_encoding mbfl_encoding_8859_14 = {
	mbfl_no_encoding_8859_14,
	"ISO-8859-14",
	"ISO-8859-14",
	&mbfl_encoding_8859_14_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static const char *mbfl_encoding_8859_15_aliases[] = {"ISO_8859-15", NULL};

static mbfl_encoding mbfl_encoding_8859_15 = {
	mbfl_no_encoding_8859_15,
	"ISO-8859-15",
	"ISO-8859-15",
	&mbfl_encoding_8859_15_aliases,
	NULL,
	MBFL_ENCTYPE_SBCS
};

static mbfl_encoding *mbfl_encoding_ptr_list[] = {
	&mbfl_encoding_pass,
	&mbfl_encoding_auto,
	&mbfl_encoding_wchar,
	&mbfl_encoding_byte2be,
	&mbfl_encoding_byte2le,
	&mbfl_encoding_byte4be,
	&mbfl_encoding_byte4le,
	&mbfl_encoding_base64,
	&mbfl_encoding_uuencode,
	&mbfl_encoding_qprint,
	&mbfl_encoding_7bit,
	&mbfl_encoding_8bit,
	&mbfl_encoding_ucs4,
	&mbfl_encoding_ucs4be,
	&mbfl_encoding_ucs4le,
	&mbfl_encoding_ucs2,
	&mbfl_encoding_ucs2be,
	&mbfl_encoding_ucs2le,
	&mbfl_encoding_utf32,
	&mbfl_encoding_utf32be,
	&mbfl_encoding_utf32le,
	&mbfl_encoding_utf16,
	&mbfl_encoding_utf16be,
	&mbfl_encoding_utf16le,
	&mbfl_encoding_utf8,
	&mbfl_encoding_utf7,
	&mbfl_encoding_utf7imap,
	&mbfl_encoding_ascii,
	&mbfl_encoding_euc_jp,
	&mbfl_encoding_sjis,
	&mbfl_encoding_eucjp_win,
	&mbfl_encoding_sjis_win,
	&mbfl_encoding_jis,
	&mbfl_encoding_2022jp,
	&mbfl_encoding_cp1252,
	&mbfl_encoding_8859_1,
	&mbfl_encoding_8859_2,
	&mbfl_encoding_8859_3,
	&mbfl_encoding_8859_4,
	&mbfl_encoding_8859_5,
	&mbfl_encoding_8859_6,
	&mbfl_encoding_8859_7,
	&mbfl_encoding_8859_8,
	&mbfl_encoding_8859_9,
	&mbfl_encoding_8859_10,
	&mbfl_encoding_8859_13,
	&mbfl_encoding_8859_14,
	&mbfl_encoding_8859_15,
	NULL
};

/* forward */
static void mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter);
static int mbfl_filt_conv_common_flush(mbfl_convert_filter *filter);
static void mbfl_filt_conv_common_dtor(mbfl_convert_filter *filter);

static int mbfl_filt_conv_pass(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_byte2be(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_byte2be_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_byte2le(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_byte2le_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_byte4be(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_byte4be_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_byte4le(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_byte4le_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_any_7bit(int c, mbfl_convert_filter *filter);

static int mbfl_filt_conv_base64enc(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_base64enc_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_base64dec(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_base64dec_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_uudec(int c, mbfl_convert_filter *filter);

static int mbfl_filt_conv_qprintenc(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_qprintenc_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_qprintdec(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_qprintdec_flush(mbfl_convert_filter *filter);

static int mbfl_filt_conv_ucs4_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_ucs4be(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_ucs4le(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_ucs2_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_ucs2be(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_ucs2le(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf16_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf16be_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf16be(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf16le_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf16le(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf8_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf8(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf7_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf7(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf7_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_utf7imap_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf7imap(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_utf7imap_flush(mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_ascii(int c, mbfl_convert_filter *filter);

static int mbfl_filt_conv_wchar_cp1252(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_cp1252_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_1(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_2_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_2(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_3_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_3(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_4_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_4(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_5_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_5(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_6_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_6(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_7_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_7(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_8_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_8(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_9_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_9(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_10_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_10(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_13_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_13(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_14_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_14(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_8859_15_wchar(int c, mbfl_convert_filter *filter);
static int mbfl_filt_conv_wchar_8859_15(int c, mbfl_convert_filter *filter);

static void mbfl_filt_ident_common_ctor(mbfl_identify_filter *filter);
static void mbfl_filt_ident_common_dtor(mbfl_identify_filter *filter);
static void mbfl_filt_ident_false_ctor(mbfl_identify_filter *filter);
static int mbfl_filt_ident_utf8(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_utf7(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_ascii(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_eucjp(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_sjis(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_sjiswin(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_jis(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_cp1252(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_2022jp(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_false(int c, mbfl_identify_filter *filter);
static int mbfl_filt_ident_true(int c, mbfl_identify_filter *filter);

/* convert filter function table */
static struct mbfl_convert_vtbl vtbl_pass = {
	mbfl_no_encoding_pass,
	mbfl_no_encoding_pass,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_pass,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_byte2be_wchar = {
	mbfl_no_encoding_byte2be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte2be_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_byte2be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte2be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_byte2be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_byte2le_wchar = {
	mbfl_no_encoding_byte2le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte2le_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_byte2le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte2le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_byte2le,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_byte4be_wchar = {
	mbfl_no_encoding_byte4be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte4be_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_byte4be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte4be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_byte4be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_byte4le_wchar = {
	mbfl_no_encoding_byte4le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte4le_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_byte4le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte4le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_byte4le,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8bit_b64 = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_base64,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_base64enc,
	mbfl_filt_conv_base64enc_flush };

static struct mbfl_convert_vtbl vtbl_b64_8bit = {
	mbfl_no_encoding_base64,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_base64dec,
	mbfl_filt_conv_base64dec_flush };

static struct mbfl_convert_vtbl vtbl_uuencode_8bit = {
	mbfl_no_encoding_uuencode,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_uudec,
	mbfl_filt_conv_common_flush
};


static struct mbfl_convert_vtbl vtbl_8bit_qprint = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_qprint,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_qprintenc,
	mbfl_filt_conv_qprintenc_flush };

static struct mbfl_convert_vtbl vtbl_qprint_8bit = {
	mbfl_no_encoding_qprint,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_qprintdec,
	mbfl_filt_conv_qprintdec_flush };

static struct mbfl_convert_vtbl vtbl_8bit_7bit = {
	mbfl_no_encoding_8bit,
	mbfl_no_encoding_7bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_any_7bit,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_7bit_8bit = {
	mbfl_no_encoding_7bit,
	mbfl_no_encoding_8bit,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_pass,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_ucs4_wchar = {
	mbfl_no_encoding_ucs4,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_ucs4_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ucs4 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_ucs4be_wchar = {
	mbfl_no_encoding_ucs4be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte4be_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ucs4be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_ucs4le_wchar = {
	mbfl_no_encoding_ucs4le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte4le_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ucs4le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs4le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs4le,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_ucs2_wchar = {
	mbfl_no_encoding_ucs2,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_ucs2_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ucs2 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs2be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_ucs2be_wchar = {
	mbfl_no_encoding_ucs2be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte2be_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ucs2be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs2be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_ucs2le_wchar = {
	mbfl_no_encoding_ucs2le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte2le_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ucs2le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ucs2le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs2le,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf32_wchar = {
	mbfl_no_encoding_utf32,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_ucs4_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf32 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf32be_wchar = {
	mbfl_no_encoding_utf32be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte4be_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf32be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs4be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf32le_wchar = {
	mbfl_no_encoding_utf32le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_byte4le_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf32le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf32le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ucs4le,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf16_wchar = {
	mbfl_no_encoding_utf16,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf16_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf16 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf16be_wchar = {
	mbfl_no_encoding_utf16be,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf16be_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf16be = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16be,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf16be,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf16le_wchar = {
	mbfl_no_encoding_utf16le,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf16le_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf16le = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf16le,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf16le,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf8_wchar = {
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf8_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf8 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf8,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf8,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_utf7_wchar = {
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf7_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf7 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf7,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf7,
	mbfl_filt_conv_wchar_utf7_flush };

static struct mbfl_convert_vtbl vtbl_utf7imap_wchar = {
	mbfl_no_encoding_utf7imap,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_utf7imap_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_utf7imap = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_utf7imap,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_utf7imap,
	mbfl_filt_conv_wchar_utf7imap_flush };

static struct mbfl_convert_vtbl vtbl_ascii_wchar = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_pass,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_ascii = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_ascii,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_ascii,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_eucjp_wchar = {
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_eucjp_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_eucjp = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_euc_jp,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_eucjp,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_sjis_wchar = {
	mbfl_no_encoding_sjis,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_sjis_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_sjis = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_sjis,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_jis_wchar = {
	mbfl_no_encoding_jis,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_jis = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_jis,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_jis,
	mbfl_filt_conv_any_jis_flush };

static struct mbfl_convert_vtbl vtbl_2022jp_wchar = {
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_2022jp = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_2022jp,
	mbfl_filt_conv_any_jis_flush };

static struct mbfl_convert_vtbl vtbl_eucjpwin_wchar = {
	mbfl_no_encoding_eucjp_win,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_eucjpwin_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_eucjpwin = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_eucjp_win,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_eucjpwin,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_sjiswin_wchar = {
	mbfl_no_encoding_sjis_win,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_sjiswin_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_sjiswin = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_sjis_win,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_sjiswin,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_cp1252_wchar = {
	mbfl_no_encoding_cp1252,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_cp1252_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_cp1252 = {
	mbfl_no_encoding_cp1252,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_cp1252,
	mbfl_filt_conv_common_flush };


static struct mbfl_convert_vtbl vtbl_8859_1_wchar = {
	mbfl_no_encoding_8859_1,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_pass,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_1 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_1,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_1,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_2_wchar = {
	mbfl_no_encoding_8859_2,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_2_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_2 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_2,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_2,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_3_wchar = {
	mbfl_no_encoding_8859_3,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_3_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_3 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_3,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_3,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_4_wchar = {
	mbfl_no_encoding_8859_4,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_4_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_4 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_4,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_4,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_5_wchar = {
	mbfl_no_encoding_8859_5,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_5_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_5 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_5,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_5,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_6_wchar = {
	mbfl_no_encoding_8859_6,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_6_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_6 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_6,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_6,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_7_wchar = {
	mbfl_no_encoding_8859_7,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_7_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_7 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_7,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_7,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_8_wchar = {
	mbfl_no_encoding_8859_8,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_8_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_8 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_8,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_8,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_9_wchar = {
	mbfl_no_encoding_8859_9,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_9_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_9 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_9,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_9,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_10_wchar = {
	mbfl_no_encoding_8859_10,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_10_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_10 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_10,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_10,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_13_wchar = {
	mbfl_no_encoding_8859_13,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_13_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_13 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_13,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_13,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_14_wchar = {
	mbfl_no_encoding_8859_14,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_14_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_14 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_14,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_14,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_8859_15_wchar = {
	mbfl_no_encoding_8859_15,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_8859_15_wchar,
	mbfl_filt_conv_common_flush };

static struct mbfl_convert_vtbl vtbl_wchar_8859_15 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_8859_15,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_8859_15,
	mbfl_filt_conv_common_flush };


static struct mbfl_convert_vtbl *mbfl_convert_filter_list[] = {
	&vtbl_utf8_wchar,
	&vtbl_wchar_utf8,
	&vtbl_eucjp_wchar,
	&vtbl_wchar_eucjp,
	&vtbl_sjis_wchar,
	&vtbl_wchar_sjis,
	&vtbl_jis_wchar,
	&vtbl_wchar_jis,
	&vtbl_2022jp_wchar,
	&vtbl_wchar_2022jp,
	&vtbl_eucjpwin_wchar,
	&vtbl_wchar_eucjpwin,
	&vtbl_sjiswin_wchar,
	&vtbl_wchar_sjiswin,
	&vtbl_cp1252_wchar,
	&vtbl_wchar_cp1252,
	&vtbl_ascii_wchar,
	&vtbl_wchar_ascii,
	&vtbl_8859_1_wchar,
	&vtbl_wchar_8859_1,
	&vtbl_8859_2_wchar,
	&vtbl_wchar_8859_2,
	&vtbl_8859_3_wchar,
	&vtbl_wchar_8859_3,
	&vtbl_8859_4_wchar,
	&vtbl_wchar_8859_4,
	&vtbl_8859_5_wchar,
	&vtbl_wchar_8859_5,
	&vtbl_8859_6_wchar,
	&vtbl_wchar_8859_6,
	&vtbl_8859_7_wchar,
	&vtbl_wchar_8859_7,
	&vtbl_8859_8_wchar,
	&vtbl_wchar_8859_8,
	&vtbl_8859_9_wchar,
	&vtbl_wchar_8859_9,
	&vtbl_8859_10_wchar,
	&vtbl_wchar_8859_10,
	&vtbl_8859_13_wchar,
	&vtbl_wchar_8859_13,
	&vtbl_8859_14_wchar,
	&vtbl_wchar_8859_14,
	&vtbl_8859_15_wchar,
	&vtbl_wchar_8859_15,
	&vtbl_8bit_b64,
	&vtbl_b64_8bit,
	&vtbl_uuencode_8bit,
	&vtbl_8bit_qprint,
	&vtbl_qprint_8bit,
	&vtbl_8bit_7bit,
	&vtbl_7bit_8bit,
	&vtbl_utf7_wchar,
	&vtbl_wchar_utf7,
	&vtbl_utf7imap_wchar,
	&vtbl_wchar_utf7imap,
	&vtbl_utf16_wchar,
	&vtbl_wchar_utf16,
	&vtbl_utf16be_wchar,
	&vtbl_wchar_utf16be,
	&vtbl_utf16le_wchar,
	&vtbl_wchar_utf16le,
	&vtbl_utf32_wchar,
	&vtbl_wchar_utf32,
	&vtbl_utf32be_wchar,
	&vtbl_wchar_utf32be,
	&vtbl_utf32le_wchar,
	&vtbl_wchar_utf32le,
	&vtbl_ucs4_wchar,
	&vtbl_wchar_ucs4,
	&vtbl_ucs4be_wchar,
	&vtbl_wchar_ucs4be,
	&vtbl_ucs4le_wchar,
	&vtbl_wchar_ucs4le,
	&vtbl_ucs2_wchar,
	&vtbl_wchar_ucs2,
	&vtbl_ucs2be_wchar,
	&vtbl_wchar_ucs2be,
	&vtbl_ucs2le_wchar,
	&vtbl_wchar_ucs2le,
	&vtbl_byte4be_wchar,
	&vtbl_wchar_byte4be,
	&vtbl_byte4le_wchar,
	&vtbl_wchar_byte4le,
	&vtbl_byte2be_wchar,
	&vtbl_wchar_byte2be,
	&vtbl_byte2le_wchar,
	&vtbl_wchar_byte2le,
	&vtbl_pass,
	NULL
};


/* identify filter function table */
static struct mbfl_identify_vtbl vtbl_identify_ascii = {
	mbfl_no_encoding_ascii,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_ascii };

static struct mbfl_identify_vtbl vtbl_identify_utf8 = {
	mbfl_no_encoding_utf8,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_utf8 };

static struct mbfl_identify_vtbl vtbl_identify_utf7 = {
	mbfl_no_encoding_utf7,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_utf7 };

static struct mbfl_identify_vtbl vtbl_identify_eucjp = {
	mbfl_no_encoding_euc_jp,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_eucjp };

static struct mbfl_identify_vtbl vtbl_identify_eucjpwin = {
	mbfl_no_encoding_eucjp_win,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_eucjp };

static struct mbfl_identify_vtbl vtbl_identify_sjis = {
	mbfl_no_encoding_sjis,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjis };

static struct mbfl_identify_vtbl vtbl_identify_sjiswin = {
	mbfl_no_encoding_sjis_win,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_sjiswin };

static struct mbfl_identify_vtbl vtbl_identify_jis = {
	mbfl_no_encoding_jis,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_jis };

static struct mbfl_identify_vtbl vtbl_identify_2022jp = {
	mbfl_no_encoding_2022jp,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_2022jp };

static struct mbfl_identify_vtbl vtbl_identify_cp1252 = {
	mbfl_no_encoding_cp1252,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_cp1252 };

static struct mbfl_identify_vtbl vtbl_identify_8859_1 = {
	mbfl_no_encoding_8859_1,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_2 = {
	mbfl_no_encoding_8859_2,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_3 = {
	mbfl_no_encoding_8859_3,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_4 = {
	mbfl_no_encoding_8859_4,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_5 = {
	mbfl_no_encoding_8859_5,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_6 = {
	mbfl_no_encoding_8859_6,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_7 = {
	mbfl_no_encoding_8859_7,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_8 = {
	mbfl_no_encoding_8859_8,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_9 = {
	mbfl_no_encoding_8859_9,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_10 = {
	mbfl_no_encoding_8859_10,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_13 = {
	mbfl_no_encoding_8859_13,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_14 = {
	mbfl_no_encoding_8859_14,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_8859_15 = {
	mbfl_no_encoding_8859_15,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_true };

static struct mbfl_identify_vtbl vtbl_identify_false = {
	mbfl_no_encoding_pass,
	mbfl_filt_ident_false_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_false };

static struct mbfl_identify_vtbl *mbfl_identify_filter_list[] = {
	&vtbl_identify_utf8,
	&vtbl_identify_utf7,
	&vtbl_identify_ascii,
	&vtbl_identify_eucjp,
	&vtbl_identify_sjis,
	&vtbl_identify_eucjpwin,
	&vtbl_identify_sjiswin,
	&vtbl_identify_jis,
	&vtbl_identify_2022jp,
	&vtbl_identify_cp1252,
	&vtbl_identify_8859_1,
	&vtbl_identify_8859_2,
	&vtbl_identify_8859_3,
	&vtbl_identify_8859_4,
	&vtbl_identify_8859_5,
	&vtbl_identify_8859_6,
	&vtbl_identify_8859_7,
	&vtbl_identify_8859_8,
	&vtbl_identify_8859_9,
	&vtbl_identify_8859_10,
	&vtbl_identify_8859_13,
	&vtbl_identify_8859_14,
	&vtbl_identify_8859_15,
	&vtbl_identify_false,
	NULL
};


/* language resolver */
mbfl_language *
mbfl_name2language(const char *name)
{
	mbfl_language *language;
	int i, j;

	if (name == NULL) {
		return NULL;
	}

	i = 0;
	while ((language = mbfl_language_ptr_table[i++]) != NULL){
		if (strcasecmp(language->name, name) == 0) {
			return language;
		}
	}

	i = 0;
	while ((language = mbfl_language_ptr_table[i++]) != NULL){
		if (strcasecmp(language->short_name, name) == 0) {
			return language;
		}
	}

	/* serch aliases */
	i = 0;
	while ((language = mbfl_language_ptr_table[i++]) != NULL) {
		if (language->aliases != NULL) {
			j = 0;
			while ((*language->aliases)[j] != NULL) {
				if (strcasecmp((*language->aliases)[j], name) == 0) {
					return language;
				}
				j++;
			}
		}
	}

	return NULL;
}

mbfl_language *
mbfl_no2language(enum mbfl_no_language no_language)
{
	mbfl_language *language;
	int i;

	i = 0;
	while ((language = mbfl_language_ptr_table[i++]) != NULL){
		if (language->no_language == no_language) {
			return language;
		}
	}

	return NULL;
}

enum mbfl_no_language
mbfl_name2no_language(const char *name)
{
	mbfl_language *language;

	language = mbfl_name2language(name);
	if (language == NULL) {
		return mbfl_no_language_invalid;
	} else {
		return language->no_language;
	}
}

const char *
mbfl_no_language2name(enum mbfl_no_language no_language)
{
	mbfl_language *language;

	language = mbfl_no2language(no_language);
	if (language == NULL) {
		return "";
	} else {
		return language->name;
	}
}



/* encoding resolver */
mbfl_encoding *
mbfl_name2encoding(const char *name)
{
	mbfl_encoding *encoding;
	int i, j;

	if (name == NULL) {
		return NULL;
	}

	i = 0;
	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL){
		if (strcasecmp(encoding->name, name) == 0) {
			return encoding;
		}
	}

	/* serch MIME charset name */
	i = 0;
	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL) {
		if (encoding->mime_name != NULL) {
			if (strcasecmp(encoding->mime_name, name) == 0) {
				return encoding;
			}
		}
	}

	/* serch aliases */
	i = 0;
	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL) {
		if (encoding->aliases != NULL) {
			j = 0;
			while ((*encoding->aliases)[j] != NULL) {
				if (strcasecmp((*encoding->aliases)[j], name) == 0) {
					return encoding;
				}
				j++;
			}
		}
	}

	return NULL;
}

mbfl_encoding *
mbfl_no2encoding(enum mbfl_no_encoding no_encoding)
{
	mbfl_encoding *encoding;
	int i;

	i = 0;
	while ((encoding = mbfl_encoding_ptr_list[i++]) != NULL){
		if (encoding->no_encoding == no_encoding) {
			return encoding;
		}
	}

	return NULL;
}

enum mbfl_no_encoding
mbfl_name2no_encoding(const char *name)
{
	mbfl_encoding *encoding;

	encoding = mbfl_name2encoding(name);
	if (encoding == NULL) {
		return mbfl_no_encoding_invalid;
	} else {
		return encoding->no_encoding;
	}
}

const char *
mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding)
{
	mbfl_encoding *encoding;

	encoding = mbfl_no2encoding(no_encoding);
	if (encoding == NULL) {
		return "";
	} else {
		return encoding->name;
	}
}

const char *
mbfl_no2preferred_mime_name(enum mbfl_no_encoding no_encoding)
{
	mbfl_encoding *encoding;

	encoding = mbfl_no2encoding(no_encoding);
	if (encoding != NULL && encoding->mime_name != NULL && encoding->mime_name[0] != '\0') {
		return encoding->mime_name;
	} else {
		return NULL;
	}
}

int
mbfl_is_support_encoding(const char *name)
{
	mbfl_encoding *encoding;

	encoding = mbfl_name2encoding(name);
	if (encoding == NULL) {
		return 0;
	} else {
		return 1;
	}
}



/*
 * memory device output functions
 */
void
mbfl_memory_device_init(mbfl_memory_device *device, int initsz, int allocsz)
{
	if (device) {
		device->length = 0;
		device->buffer = (unsigned char *)0;
		if (initsz > 0) {
			device->buffer = (unsigned char *)mbfl_malloc(initsz*sizeof(unsigned char));
			if (device->buffer != NULL) {
				device->length = initsz;
			}
		}
		device->pos= 0;
		if (allocsz > MBFL_MEMORY_DEVICE_ALLOC_SIZE) {
			device->allocsz = allocsz;
		} else {
			device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		}
	}
}

void
mbfl_memory_device_realloc(mbfl_memory_device *device, int initsz, int allocsz)
{
	unsigned char *tmp;

	if (device) {
		if (initsz > device->length) {
			tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, initsz*sizeof(unsigned char));
			if (tmp != NULL) {
				device->buffer = tmp;
				device->length = initsz;
			}
		}
		if (allocsz > MBFL_MEMORY_DEVICE_ALLOC_SIZE) {
			device->allocsz = allocsz;
		} else {
			device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		}
	}
}

void
mbfl_memory_device_clear(mbfl_memory_device *device)
{
	if (device) {
		if (device->buffer) {
			mbfl_free(device->buffer);
		}
		device->buffer = (unsigned char *)0;
		device->length = 0;
		device->pos = 0;
	}
}

void
mbfl_memory_device_reset(mbfl_memory_device *device)
{
	if (device) {
		device->pos = 0;
	}
}

void
mbfl_memory_device_unput(mbfl_memory_device *device)
{
	if (device->pos > 0) {
		device->pos--;
	}
}

mbfl_string *
mbfl_memory_device_result(mbfl_memory_device *device, mbfl_string *result)
{
	if (device && result) {
		result->len = device->pos;
		mbfl_memory_device_output4('\0', device);
		result->val = device->buffer;
		device->buffer = (unsigned char *)0;
		device->length = 0;
		device->pos= 0;
		if (result->val == NULL) {
			result->len = 0;
			result = NULL;
		}
	} else {
		result = NULL;
	}

	return result;
}

int
mbfl_memory_device_output(int c, void *data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if (device->pos >= device->length) {
		/* reallocate buffer */
		int newlen;
		unsigned char *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = (unsigned char)c;
	return c;
}

int
mbfl_memory_device_output2(int c, void *data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if ((device->pos + 2) >= device->length) {
		/* reallocate buffer */
		int newlen;
		unsigned char *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = (unsigned char)((c >> 8) & 0xff);
	device->buffer[device->pos++] = (unsigned char)(c & 0xff);

	return c;
}

int
mbfl_memory_device_output4(int c, void* data)
{
	mbfl_memory_device *device = (mbfl_memory_device *)data;

	if ((device->pos + 4) >= device->length) {
		/* reallocate buffer */
		int newlen;
		unsigned char *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = (unsigned char)((c >> 24) & 0xff);
	device->buffer[device->pos++] = (unsigned char)((c >> 16) & 0xff);
	device->buffer[device->pos++] = (unsigned char)((c >> 8) & 0xff);
	device->buffer[device->pos++] = (unsigned char)(c & 0xff);

	return c;
}

int
mbfl_memory_device_strcat(mbfl_memory_device *device, const char *psrc)
{
	int len;
	unsigned char *w;
	const unsigned char *p;

	len = 0;
	p = psrc;
	while (*p) {
		p++;
		len++;
	}

	if ((device->pos + len) >= device->length) {
		/* reallocate buffer */
		int newlen = device->length + (len + MBFL_MEMORY_DEVICE_ALLOC_SIZE)*sizeof(unsigned char);
		unsigned char *tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	p = psrc;
	w = &device->buffer[device->pos];
	device->pos += len;
	while (len > 0) {
		*w++ = *p++;
		len--;
	}

	return len;
}

int
mbfl_memory_device_strncat(mbfl_memory_device *device, const char *psrc, int len)
{
	unsigned char *w;

	if ((device->pos + len) >= device->length) {
		/* reallocate buffer */
		int newlen = device->length + len + MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		unsigned char *tmp = (unsigned char *)mbfl_realloc((void *)device->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	w = &device->buffer[device->pos];
	device->pos += len;
	while (len > 0) {
		*w++ = *psrc++;
		len--;
	}

	return len;
}

int
mbfl_memory_device_devcat(mbfl_memory_device *dest, mbfl_memory_device *src)
{
	int n;
	unsigned char *p, *w;

	if ((dest->pos + src->pos) >= dest->length) {
		/* reallocate buffer */
		int newlen = dest->length + src->pos + MBFL_MEMORY_DEVICE_ALLOC_SIZE;
		unsigned char *tmp = (unsigned char *)mbfl_realloc((void *)dest->buffer, newlen*sizeof(unsigned char));
		if (tmp == NULL) {
			return -1;
		}
		dest->length = newlen;
		dest->buffer = tmp;
	}

	p = src->buffer;
	w = &dest->buffer[dest->pos];
	n = src->pos;
	dest->pos += n;
	while (n > 0) {
		*w++ = *p++;
		n--;
	}

	return n;
}

void
mbfl_wchar_device_init(mbfl_wchar_device *device)
{
	if (device) {
		device->buffer = (unsigned int *)0;
		device->length = 0;
		device->pos= 0;
		device->allocsz = MBFL_MEMORY_DEVICE_ALLOC_SIZE;
	}
}

void
mbfl_wchar_device_clear(mbfl_wchar_device *device)
{
	if (device) {
		if (device->buffer) {
			mbfl_free(device->buffer);
		}
		device->buffer = (unsigned int*)0;
		device->length = 0;
		device->pos = 0;
	}
}

int
mbfl_wchar_device_output(int c, void *data)
{
	mbfl_wchar_device *device = (mbfl_wchar_device *)data;

	if (device->pos >= device->length) {
		/* reallocate buffer */
		int newlen;
		unsigned int *tmp;

		newlen = device->length + device->allocsz;
		tmp = (unsigned int *)mbfl_realloc((void *)device->buffer, newlen*sizeof(int));
		if (tmp == NULL) {
			return -1;
		}
		device->length = newlen;
		device->buffer = tmp;
	}

	device->buffer[device->pos++] = c;

	return c;
}


/*
 * string object
 */
void
mbfl_string_init(mbfl_string *string)
{
	if (string) {
		string->no_language = mbfl_no_language_uni;
		string->no_encoding = mbfl_no_encoding_pass;
		string->val = (unsigned char*)0;
		string->len = 0;
	}
}

void
mbfl_string_init_set(mbfl_string *string, enum mbfl_no_language no_language, enum mbfl_no_encoding no_encoding)
{
	if (string) {
		string->no_language = no_language;
		string->no_encoding = no_encoding;
		string->val = (unsigned char*)0;
		string->len = 0;
	}
}

void
mbfl_string_clear(mbfl_string *string)
{
	if (string) {
		if (string->val != (unsigned char*)0) {
			mbfl_free(string->val);
		}
		string->val = (unsigned char*)0;
		string->len = 0;
	}
}



/*
 * encoding filter
 */
#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)

/*
 * commonly used constructor and destructor
 */
static void
mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;
}

static int
mbfl_filt_conv_common_flush(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;
	return 0;
}

static void
mbfl_filt_conv_common_dtor(mbfl_convert_filter *filter)
{
	filter->status = 0;
	filter->cache = 0;
}

static int
mbfl_filt_conv_pass(int c, mbfl_convert_filter *filter)
{
	return (*filter->output_function)(c, filter->data);
}

static int
mbfl_filt_conv_any_7bit(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x80) {
		CK((*filter->output_function)(c, filter->data));
	} 
	return c;
}

static int
mbfl_filt_conv_byte2be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = (c & 0xff) << 8;
		filter->cache = n;
	} else {
		filter->status = 0;
		n = (c & 0xff) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return c;
}

static int
mbfl_filt_conv_wchar_byte2be(int c, mbfl_convert_filter *filter)
{
	CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	CK((*filter->output_function)(c & 0xff, filter->data));
	return c;
}

static int
mbfl_filt_conv_byte2le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = c & 0xff;
		filter->cache = n;
	} else {
		filter->status = 0;
		n = ((c & 0xff) << 8) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return c;
}

static int
mbfl_filt_conv_wchar_byte2le(int c, mbfl_convert_filter *filter)
{
	CK((*filter->output_function)(c & 0xff, filter->data));
	CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	return c;
}

static int
mbfl_filt_conv_byte4be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = (c & 0xff) << 24;
		filter->cache = n;
	} else if (filter->status == 1) {
		filter->status = 2;
		n = (c & 0xff) << 16;
		filter->cache |= n;
	} else if (filter->status == 2) {
		filter->status = 3;
		n = (c & 0xff) << 8;
		filter->cache |= n;
	} else {
		filter->status = 0;
		n = (c & 0xff) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return c;
}

static int
mbfl_filt_conv_wchar_byte4be(int c, mbfl_convert_filter *filter)
{
	CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
	CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
	CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	CK((*filter->output_function)(c & 0xff, filter->data));
	return c;
}

static int
mbfl_filt_conv_byte4le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	if (filter->status == 0) {
		filter->status = 1;
		n = (c & 0xff);
		filter->cache = n;
	} else if (filter->status == 1) {
		filter->status = 2;
		n = (c & 0xff) << 8;
		filter->cache |= n;
	} else if (filter->status == 2) {
		filter->status = 3;
		n = (c & 0xff) << 16;
		filter->cache |= n;
	} else {
		filter->status = 0;
		n = ((c & 0xff) << 24) | filter->cache;
		CK((*filter->output_function)(n, filter->data));
	}
	return c;
}

static int
mbfl_filt_conv_wchar_byte4le(int c, mbfl_convert_filter *filter)
{
	CK((*filter->output_function)(c & 0xff, filter->data));
	CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
	CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
	return c;
}


/* uuencode => any */
#define UUDEC(c)	(char)(((c)-' ')&077)
static const char * uuenc_begin_text = "begin ";
enum { uudec_state_ground=0, uudec_state_inbegin,
	uudec_state_until_newline,
	uudec_state_size, uudec_state_a, uudec_state_b, uudec_state_c, uudec_state_d,
	uudec_state_skip_newline};
static int
mbfl_filt_conv_uudec(int c, mbfl_convert_filter * filter)
{
	int n;
	
	switch(filter->status)	{
		case uudec_state_ground:
			/* looking for "begin 0666 filename\n" line */
			if (filter->cache == 0 && c == 'b')
			{
				filter->status = uudec_state_inbegin;
				filter->cache = 1; /* move to 'e' */
			}
			else if (c == '\n')
				filter->cache = 0;
			else
				filter->cache++;
			break;
		case uudec_state_inbegin:
			if (uuenc_begin_text[filter->cache++] != c)	{
				/* doesn't match pattern */
				filter->status = uudec_state_ground;
				break;
			}
			if (filter->cache == 5)
			{
				/* thats good enough - wait for a newline */
				filter->status = uudec_state_until_newline;
				filter->cache = 0;
			}
			break;
		case uudec_state_until_newline:
			if (c == '\n')
				filter->status = uudec_state_size;
			break;
		case uudec_state_size:
			/* get "size" byte */
			n = UUDEC(c);
			filter->cache = n << 24;
			filter->status = uudec_state_a;
			break;
		case uudec_state_a:
			/* get "a" byte */
			n = UUDEC(c);
			filter->cache |= (n << 16);
			filter->status = uudec_state_b;
			break;
		case uudec_state_b:
			/* get "b" byte */
			n = UUDEC(c);
			filter->cache |= (n << 8);
			filter->status = uudec_state_c;
			break;
		case uudec_state_c:
			/* get "c" byte */
			n = UUDEC(c);
			filter->cache |= n;
			filter->status = uudec_state_d;
			break;
		case uudec_state_d:
			/* get "d" byte */
			{
				int A, B, C, D = UUDEC(c);
				A = (filter->cache >> 16) & 0xff;
				B = (filter->cache >> 8) & 0xff;
				C = (filter->cache) & 0xff;
				n = (filter->cache >> 24) & 0xff;
				if (n-- > 0)
					CK((*filter->output_function)( (A << 2) | (B >> 4), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (B << 4) | (C >> 2), filter->data));
				if (n-- > 0)
					CK((*filter->output_function)( (C << 6) | D, filter->data));
				filter->cache = n << 24;

				if (n == 0)
					filter->status = uudec_state_skip_newline;	/* skip next byte (newline) */
				else
					filter->status = uudec_state_a; /* go back to fetch "A" byte */
			}
			break;
		case uudec_state_skip_newline:
			/* skip newline */
			filter->status = 0;
	}
	return c;
}

/*
 * any => BASE64
 */
static const unsigned char mbfl_base64_table[] =
{
 /* 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', */
   0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,
 /* 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
   0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
 /* 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', */
   0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
 /* 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
   0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
 /* '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0' */
   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2b,0x2f,0x00
};

#define MBFL_BASE64_STS_MIME_HEADER 0x1000000

static int
mbfl_filt_conv_base64enc(int c, mbfl_convert_filter *filter)
{
	int n;

	n = (filter->status & 0xff);
	if (n == 0) {
		filter->status++;
		filter->cache = (c & 0xff) << 16;
	} else if (n == 1) {
		filter->status++;
		filter->cache |= (c & 0xff) << 8;
	} else {
		filter->status &= ~0xff;
		if ((filter->status & MBFL_BASE64_STS_MIME_HEADER) == 0) {
			n = (filter->status & 0xff00) >> 8;
			if (n > 72) {
				CK((*filter->output_function)(0x0d, filter->data));		/* CR */
				CK((*filter->output_function)(0x0a, filter->data));		/* LF */
				filter->status &= ~0xff00;
			}
			filter->status += 0x400;
		}
		n = filter->cache | (c & 0xff);
		CK((*filter->output_function)(mbfl_base64_table[(n >> 18) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(n >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(n >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[n & 0x3f], filter->data));
	}

	return c;
}

static int
mbfl_filt_conv_base64enc_flush(mbfl_convert_filter *filter)
{
	int status, cache, len;

	status = filter->status & 0xff;
	cache = filter->cache;
	len = (filter->status & 0xff00) >> 8;
	filter->status &= ~0xffff;
	filter->cache = 0;
	/* flush fragments */
	if (status >= 1) {
		if ((filter->status & MBFL_BASE64_STS_MIME_HEADER) == 0) {
			if (len > 72){
				CK((*filter->output_function)(0x0d, filter->data));		/* CR */
				CK((*filter->output_function)(0x0a, filter->data));		/* LF */
			}
		}
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 18) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 12) & 0x3f], filter->data));
		if (status == 1) {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		} else {
			CK((*filter->output_function)(mbfl_base64_table[(cache >> 6) & 0x3f], filter->data));
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		}
	}
	return 0;
}

/*
 * BASE64 => any
 */
static int
mbfl_filt_conv_base64dec(int c, mbfl_convert_filter *filter)
{
	int n;

	if (c == 0x0d || c == 0x0a || c == 0x20 || c == 0x09 || c == 0x3d) {	/* CR or LF or SPACE or HTAB or '=' */
		return c;
	}

	n = 0;
	if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
		n = c - 65;
	} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
		n = c - 71;
	} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
		n = c + 4;
	} else if (c == 0x2b) {			/* '+' */
		n = 62;
	} else if (c == 0x2f) {			/* '/' */
		n = 63;
	}
	n &= 0x3f;

	switch (filter->status) {
	case 0:
		filter->status = 1;
		filter->cache = n << 18;
		break;
	case 1:
		filter->status = 2;
		filter->cache |= n << 12;
		break;
	case 2:
		filter->status = 3;
		filter->cache |= n << 6;
		break;
	default:
		filter->status = 0;
		n |= filter->cache;
		CK((*filter->output_function)((n >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((n >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(n & 0xff, filter->data));
		break;
	}

	return c;
}

static int
mbfl_filt_conv_base64dec_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	if (status >= 2) {
		CK((*filter->output_function)((cache >> 16) & 0xff, filter->data));
		if (status >= 3) {
			CK((*filter->output_function)((cache >> 8) & 0xff, filter->data));
		}
	}
	return 0;
}

/*
 * any => Quoted-Printable
 */
#define MBFL_QPRINT_STS_MIME_HEADER 0x1000000

static int
mbfl_filt_conv_qprintenc(int c, mbfl_convert_filter *filter)
{
	int s, n;

	switch (filter->status & 0xff) {
	case 0:
		filter->cache = c;
		filter->status++;
		break;
	default:
		s = filter->cache;
		filter->cache = c;
		n = (filter->status & 0xff00) >> 8;

		if (s == 0) {		/* null */
			CK((*filter->output_function)(s, filter->data));
			filter->status &= ~0xff00;
			break;
		}

		if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
			if (s == 0x0a || (s == 0x0d && c != 0x0a)) {	/* line feed */
				CK((*filter->output_function)(0x0d, filter->data));		/* CR */
				CK((*filter->output_function)(0x0a, filter->data));		/* LF */
				filter->status &= ~0xff00;
				break;
			} else if (s == 0x0d) {
				break;
			}
		}

		if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0  && n >= 72) {	/* soft line feed */
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(0x0d, filter->data));		/* CR */
			CK((*filter->output_function)(0x0a, filter->data));		/* LF */
			filter->status &= ~0xff00;
		}

		if (s <= 0 || s >= 0x80 || s == 0x3d		/* not ASCII or '=' */
		   || ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) != 0 && 
		       (mbfl_charprop_table[s] & MBFL_CHP_MMHQENC) != 0)) {
			/* hex-octet */
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			n = (s >> 4) & 0xf;
			if (n < 10) {
				n += 48;		/* '0' */
			} else {
				n += 55;		/* 'A' - 10 */
			}
			CK((*filter->output_function)(n, filter->data));
			n = s & 0xf;
			if (n < 10) {
				n += 48;
			} else {
				n += 55;
			}
			CK((*filter->output_function)(n, filter->data));
			if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
				filter->status += 0x300;
			}
		} else {
			CK((*filter->output_function)(s, filter->data));
			if ((filter->status & MBFL_QPRINT_STS_MIME_HEADER) == 0) {
				filter->status += 0x100;
			}
		}
		break;
	}

	return c;
}

static int
mbfl_filt_conv_qprintenc_flush(mbfl_convert_filter *filter)
{
	/* flush filter cache */
	(*filter->filter_function)('\0', filter);
	filter->status &= ~0xffff;
	filter->cache = 0;
	return 0;
}

/*
 * Quoted-Printable => any
 */
static int
mbfl_filt_conv_qprintdec(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 1:
		if ((c >= 0x30 && c <= 0x39) || (c >= 0x41 && c <= 0x46)) {	/* 0 - 9 or A - F */
			filter->cache = c;
			filter->status = 2;
		} else if (c == 0x0d) {	/* soft line feed */
			filter->status = 3;
		} else if (c == 0x0a) {	/* soft line feed */
			filter->status = 0;
		} else {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		}
		break;
	case 2:
		n = filter->cache;
		if (n >= 0x30 && n <= 0x39) {		/* '0' - '9' */
			n -= 48;		/* 48 = '0' */
		} else {
			n -= 55;		/* 55 = 'A' - 10 */
		}
		n <<= 4;
		if (c >= 0x30 && c <= 0x39) {		/* '0' - '9' */
			n += (c - 48);
		} else if (c >= 0x41 && c <= 0x46) {	/* 'A' - 'F' */
			n += (c - 55);
		} else {
			CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
			CK((*filter->output_function)(filter->cache, filter->data));
			n = c;
		}
		CK((*filter->output_function)(n, filter->data));
		filter->status = 0;
		break;
	case 3:
		if (c != 0x0a) {		/* LF */
			CK((*filter->output_function)(c, filter->data));
		}
		filter->status = 0;
		break;
	default:
		if (c == 0x3d) {		/* '=' */
			filter->status = 1;
		} else {
			CK((*filter->output_function)(c, filter->data));
		}
		break;
	}

	return c;
}

static int
mbfl_filt_conv_qprintdec_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	if (status == 1) {
		CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
	} else if (status == 2) {
		CK((*filter->output_function)(0x3d, filter->data));		/* '=' */
		CK((*filter->output_function)(cache, filter->data));
	}

	return 0;
}


/*
 * UCS-4 => wchar
 */
static int
mbfl_filt_conv_ucs4_wchar(int c, mbfl_convert_filter *filter)
{
	int n, endian;

	endian = filter->status & 0xff00;
	switch (filter->status & 0xff) {
	case 0:
		if (endian) {
			n = c & 0xff;
		} else {
			n = (c & 0xff) << 24;
		}
		filter->cache = n;
		filter->status++;
		break;
	case 1:
		if (endian) {
			n = (c & 0xff) << 8;
		} else {
			n = (c & 0xff) << 16;
		}
		filter->cache |= n;
		filter->status++;
		break;
	case 2:
		if (endian) {
			n = (c & 0xff) << 16;
		} else {
			n = (c & 0xff) << 8;
		}
		filter->cache |= n;
		filter->status++;
		break;
	default:
		if (endian) {
			n = (c & 0xff) << 24;
		} else {
			n = c & 0xff;
		}
		n |= filter->cache;
		if ((n & 0xffff) == 0 && ((n >> 16) & 0xffff) == 0xfffe) {
			if (endian) {
				filter->status = 0;		/* big-endian */
			} else {
				filter->status = 0x100;		/* little-endian */
			}
			CK((*filter->output_function)(0xfeff, filter->data));
		} else {
			filter->status &= ~0xff;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * wchar => UCS-4BE
 */
static int
mbfl_filt_conv_wchar_ucs4be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * wchar => UCS-4LE
 */
static int
mbfl_filt_conv_wchar_ucs4le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 16) & 0xff, filter->data));
		CK((*filter->output_function)((c >> 24) & 0xff, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * UCS-2 => wchar
 */
static int
mbfl_filt_conv_ucs2_wchar(int c, mbfl_convert_filter *filter)
{
	int n, endian;

	endian = filter->status & 0xff00;
	switch (filter->status & 0xff) {
	case 0:
		if (endian) {
			n = c & 0xff;
		} else {
			n = (c & 0xff) << 8;
		}
		filter->cache = n;
		filter->status++;
		break;
	default:
		if (endian) {
			n = (c & 0xff) << 8;
		} else {
			n = c & 0xff;
		}
		n |= filter->cache;
		if (n == 0xfffe) {
			if (endian) {
				filter->status = 0;		/* big-endian */
			} else {
				filter->status = 0x100;		/* little-endian */
			}
			CK((*filter->output_function)(0xfeff, filter->data));
		} else {
			filter->status &= ~0xff;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * wchar => UCS-2BE
 */
static int
mbfl_filt_conv_wchar_ucs2be(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
		CK((*filter->output_function)(c & 0xff, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * wchar => UCS-2LE
 */
static int
mbfl_filt_conv_wchar_ucs2le(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		CK((*filter->output_function)(c & 0xff, filter->data));
		CK((*filter->output_function)((c >> 8) & 0xff, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}


/*
 * UTF-16 => wchar
 */
static int
mbfl_filt_conv_utf16_wchar(int c, mbfl_convert_filter *filter)
{
	int n, endian;

	endian = filter->status & 0xff00;
	switch (filter->status & 0xff) {
	case 0:
		if (endian) {
			n = c & 0xff;
		} else {
			n = (c & 0xff) << 8;
		}
		filter->cache |= n;
		filter->status++;
		break;
	default:
		if (endian) {
			n = (c & 0xff) << 8;
		} else {
			n = c & 0xff;
		}
		n |= filter->cache & 0xffff;
		filter->status &= ~0xff;
		if (n == 0xfffe) {
			if (endian) {
				filter->status = 0;		/* big-endian */
			} else {
				filter->status = 0x100;		/* little-endian */
			}
			CK((*filter->output_function)(0xfeff, filter->data));
		} else if (n >= 0xd800 && n < 0xdc00) {
			filter->cache = ((n & 0x3ff) << 16) + 0x400000;
		} else if (n >= 0xdc00 && n < 0xe000) {
			n &= 0x3ff;
			n |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (n >= MBFL_WCSPLANE_SUPMIN && n < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(n, filter->data));
			} else {		/* illegal character */
				n &= MBFL_WCSGROUP_MASK;
				n |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(n, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * UTF-16BE => wchar
 */
static int
mbfl_filt_conv_utf16be_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0:
		filter->status = 1;
		n = (c & 0xff) << 8;
		filter->cache |= n;
		break;
	default:
		filter->status = 0;
		n = (filter->cache & 0xff00) | (c & 0xff);
		if (n >= 0xd800 && n < 0xdc00) {
			filter->cache = ((n & 0x3ff) << 16) + 0x400000;
		} else if (n >= 0xdc00 && n < 0xe000) {
			n &= 0x3ff;
			n |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (n >= MBFL_WCSPLANE_SUPMIN && n < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(n, filter->data));
			} else {		/* illegal character */
				n &= MBFL_WCSGROUP_MASK;
				n |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(n, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * wchar => UTF-16BE
 */
static int
mbfl_filt_conv_wchar_utf16be(int c, mbfl_convert_filter *filter)
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * UTF-16LE => wchar
 */
static int
mbfl_filt_conv_utf16le_wchar(int c, mbfl_convert_filter *filter)
{
	int n;

	switch (filter->status) {
	case 0:
		filter->status = 1;
		n = c & 0xff;
		filter->cache |= n;
		break;
	default:
		filter->status = 0;
		n = (filter->cache & 0xff) | ((c & 0xff) << 8);
		if (n >= 0xd800 && n < 0xdc00) {
			filter->cache = ((n & 0x3ff) << 16) + 0x400000;
		} else if (n >= 0xdc00 && n < 0xe000) {
			n &= 0x3ff;
			n |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (n >= MBFL_WCSPLANE_SUPMIN && n < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(n, filter->data));
			} else {		/* illegal character */
				n &= MBFL_WCSGROUP_MASK;
				n |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(n, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(n, filter->data));
		}
		break;
	}

	return c;
}

/*
 * wchar => UTF-16LE
 */
static int
mbfl_filt_conv_wchar_utf16le(int c, mbfl_convert_filter *filter)
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
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}


/*
 * UTF-8 => wchar
 */
static int
mbfl_filt_conv_utf8_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c < 0x80) {
		if (c >= 0) {
			CK((*filter->output_function)(c, filter->data));
		}
		filter->status = 0;
	} else if (c < 0xc0) {
		switch (filter->status & 0xff) {
		case 0x10: /* 2byte code 2nd char */
		case 0x21: /* 3byte code 3rd char */
		case 0x32: /* 4byte code 4th char */
		case 0x43: /* 5byte code 5th char */
		case 0x54: /* 6byte code 6th char */
			filter->status = 0;
			s = filter->cache | (c & 0x3f);
			if (s >= 0x80) {
				CK((*filter->output_function)(s, filter->data));
			}
			break;
		case 0x20: /* 3byte code 2nd char */
		case 0x31: /* 4byte code 3rd char */
		case 0x42: /* 5byte code 4th char */
		case 0x53: /* 6byte code 5th char */
			filter->cache |= ((c & 0x3f) << 6);
			filter->status++;
			break;
		case 0x30: /* 4byte code 2nd char */
		case 0x41: /* 5byte code 3rd char */
		case 0x52: /* 6byte code 4th char */
			filter->cache |= ((c & 0x3f) << 12);
			filter->status++;
			break;
		case 0x40: /* 5byte code 2nd char */
		case 0x51: /* 6byte code 3rd char */
			filter->cache |= ((c & 0x3f) << 18);
			filter->status++;
			break;
		case 0x50: /* 6byte code 2nd char */
			filter->cache |= ((c & 0x3f) << 24);
			filter->status++;
			break;
		default:
			filter->status = 0;
			break;
		}
	} else if (c < 0xe0) { /* 2byte code first char */
		filter->status = 0x10;
		filter->cache = (c & 0x1f) << 6;
	} else if (c < 0xf0) { /* 3byte code first char */
		filter->status = 0x20;
		filter->cache = (c & 0xf) << 12;
	} else if (c < 0xf8) { /* 4byte code first char */
		filter->status = 0x30;
		filter->cache = (c & 0x7) << 18;
	} else if (c < 0xfc) { /* 5byte code first char */
		filter->status = 0x40;
		filter->cache = (c & 0x3) << 24;
	} else if (c < 0xfe)  { /* 6 byte code first char */
		filter->status = 0x50;
		filter->cache = (c & 0x1) << 30;
	} else {
		filter->status = 0;
		filter->cache = 0;
	}

	return c;
}

/*
 * wchar => UTF-8
 */
static int
mbfl_filt_conv_wchar_utf8(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < MBFL_WCSGROUP_UCS4MAX) {
		if (c < 0x80) {
			CK((*filter->output_function)(c, filter->data));
		} else if (c < 0x800) {
			CK((*filter->output_function)(((c >> 6) & 0x1f) | 0xc0, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else if (c < 0x10000) {
			CK((*filter->output_function)(((c >> 12) & 0x0f) | 0xe0, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else if (c < 0x200000) {
			CK((*filter->output_function)(((c >> 18) & 0x07) | 0xf0, filter->data));
			CK((*filter->output_function)(((c >> 12) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else if (c < 0x4000000) {
			CK((*filter->output_function)(((c >> 24) & 0x03) | 0xf8, filter->data));
			CK((*filter->output_function)(((c >> 18) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 12) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		} else {
			CK((*filter->output_function)(((c >> 30) & 0x01) | 0xfc, filter->data));
			CK((*filter->output_function)(((c >> 24) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 18) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 12) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)(((c >> 6) & 0x3f) | 0x80, filter->data));
			CK((*filter->output_function)((c & 0x3f) | 0x80, filter->data));
		}
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}


/*
 * UTF-7 => wchar
 */
static int
mbfl_filt_conv_utf7_wchar(int c, mbfl_convert_filter *filter)
{
	int s, n;

	n = -1;
	if (filter->status != 0) {		/* Modified Base64 */
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = c - 65;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = c - 71;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = c + 4;
		} else if (c == 0x2b) {			/* '+' */
			n = 62;
		} else if (c == 0x2f) {			/* '/' */
			n = 63;
		}
		if (n < 0 || n > 63) {
			if (c == 0x2d) {
				if (filter->status == 1) {		/* "+-" -> "+" */
					CK((*filter->output_function)(0x2b, filter->data));
				}
			} else if (c >= 0 && c < 0x80) {	/* ASCII exclude '-' */
				CK((*filter->output_function)(c, filter->data));
			} else {		/* illegal character */
				s = c & MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
			filter->cache = 0;
			filter->status = 0;
			return c;
		}
	}

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == 0x2b) {	/* '+'  shift character */
			filter->status = 1;
		} else if (c >= 0 && c < 0x80) {	/* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else {		/* illegal character */
			s = c & MBFL_WCSGROUP_MASK;
			s |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	/* decode Modified Base64 */
	case 1:
	case 2:
		filter->cache |= n << 10;
		filter->status = 3;
		break;
	case 3:
		filter->cache |= n << 4;
		filter->status = 4;
		break;
	case 4:
		s = ((n >> 2) & 0xf) | (filter->cache & 0xffff);
		n = (n & 0x3) << 14;
		filter->status = 5;
		if (s >= 0xd800 && s < 0xdc00) {
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = n;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	case 5:
		filter->cache |= n << 8;
		filter->status = 6;
		break;
	case 6:
		filter->cache |= n << 2;
		filter->status = 7;
		break;
	case 7:
		s = ((n >> 4) & 0x3) | (filter->cache & 0xffff);
		n = (n & 0xf) << 12;
		filter->status = 8;
		if (s >= 0xd800 && s < 0xdc00) {
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = n;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	case 8:
		filter->cache |= n << 6;
		filter->status = 9;
		break;
	case 9:
		s = n | (filter->cache & 0xffff);
		filter->status = 2;
		if (s >= 0xd800 && s < 0xdc00) {
			s = (((s & 0x3ff) << 16) + 0x400000);
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/*
 * wchar => UTF-7
 */
static int
mbfl_filt_conv_wchar_utf7(int c, mbfl_convert_filter *filter)
{
	int s, n;

	n = 0;
	if (c >= 0 && c < 0x80) {	/* ASCII */
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = 1;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = 1;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = 1;
		} else if (c == '\0') {			/* '\0' */
			n = 1;
		} else if (c == 0x2f) {			/* '/' */
			n = 1;
		} else if (c == 0x2d) {			/* '-' */
			n = 1;
		} else if (c == 0x20) {			/* SPACE */
			n = 2;
		} else if (c == 0x09) {			/* HTAB */
			n = 2;
		} else if (c == 0x0d) {			/* CR */
			n = 2;
		} else if (c == 0x0a) {			/* LF */
			n = 2;
		} else if (c == 0x27) {			/* "'" */
			n = 2;
		} else if (c == 0x28) {			/* '(' */
			n = 2;
		} else if (c == 0x29) {			/* ')' */
			n = 2;
		} else if (c == 0x2c) {			/* ',' */
			n = 2;
		} else if (c == 0x2e) {			/* '.' */
			n = 2;
		} else if (c == 0x3a) {			/* ':' */
			n = 2;
		} else if (c == 0x3f) {			/* '?' */
			n = 2;
		}
	} else if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		;
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		s = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->filter_function)(s, filter));
		s = (c & 0x3ff) | 0xdc00;
		CK((*filter->filter_function)(s, filter));
		return c;
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
		return c;
	}

	switch (filter->status) {
	case 0:
		if (n != 0) {	/* directly encode characters */
			CK((*filter->output_function)(c, filter->data));
		} else {	/* Modified Base64 */
			CK((*filter->output_function)(0x2b, filter->data));		/* '+' */
			filter->status++;
			filter->cache = c;
		}
		break;

	/* encode Modified Base64 */
	case 1:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_base64_table[(s >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 4) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_base64_table[(s << 2) & 0x3c], filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status++;
			filter->cache = ((s & 0xf) << 16) | c;
		}
		break;

	case 2:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_base64_table[(s >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 2) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_base64_table[(s << 4) & 0x30], filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status++;
			filter->cache = ((s & 0x3) << 16) | c;
		}
		break;

	case 3:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_base64_table[(s >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(s >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[s & 0x3f], filter->data));
		if (n != 0) {
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			CK((*filter->output_function)(c, filter->data));
			filter->status = 0;
		} else {
			filter->status = 1;
			filter->cache = c;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;

}

static int
mbfl_filt_conv_wchar_utf7_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	switch (status) {
	case 1:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 4) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache << 2) & 0x3c], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 2:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 2) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache << 4) & 0x30], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 3:
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[(cache >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_base64_table[cache & 0x3f], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;
	}
	return 0;
}


/*
 * UTF7-IMAP => wchar
 */
static int
mbfl_filt_conv_utf7imap_wchar(int c, mbfl_convert_filter *filter)
{
	int s, n;

	n = -1;
	if (filter->status != 0) {		/* Modified Base64 */
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = c - 65;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = c - 71;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = c + 4;
		} else if (c == 0x2b) {			/* '+' */
			n = 62;
		} else if (c == 0x2c) {			/* ',' */
			n = 63;
		}
		if (n < 0 || n > 63) {
			if (c == 0x2d) {
				if (filter->status == 1) {		/* "&-" -> "&" */
					CK((*filter->output_function)(0x26, filter->data));
				}
			} else if (c >= 0 && c < 0x80) {	/* ASCII exclude '-' */
				CK((*filter->output_function)(c, filter->data));
			} else {		/* illegal character */
				s = c & MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
			filter->cache = 0;
			filter->status = 0;
			return c;
		}
	}

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == 0x26) {	/* '&'  shift character */
			filter->status++;
		} else if (c >= 0 && c < 0x80) {	/* ASCII */
			CK((*filter->output_function)(c, filter->data));
		} else {		/* illegal character */
			s = c & MBFL_WCSGROUP_MASK;
			s |= MBFL_WCSGROUP_THROUGH;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	/* decode Modified Base64 */
	case 1:
	case 2:
		filter->cache |= n << 10;
		filter->status = 3;
		break;
	case 3:
		filter->cache |= n << 4;
		filter->status = 4;
		break;
	case 4:
		s = ((n >> 2) & 0xf) | (filter->cache & 0xffff);
		n = (n & 0x3) << 14;
		filter->status = 5;
		if (s >= 0xd800 && s < 0xdc00) {
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = n;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	case 5:
		filter->cache |= n << 8;
		filter->status = 6;
		break;
	case 6:
		filter->cache |= n << 2;
		filter->status = 7;
		break;
	case 7:
		s = ((n >> 4) & 0x3) | (filter->cache & 0xffff);
		n = (n & 0xf) << 12;
		filter->status = 8;
		if (s >= 0xd800 && s < 0xdc00) {
			s = (((s & 0x3ff) << 16) + 0x400000) | n;
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = n;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = n;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	case 8:
		filter->cache |= n << 6;
		filter->status = 9;
		break;
	case 9:
		s = n | (filter->cache & 0xffff);
		filter->status = 2;
		if (s >= 0xd800 && s < 0xdc00) {
			s = (((s & 0x3ff) << 16) + 0x400000);
			filter->cache = s;
		} else if (s >= 0xdc00 && s < 0xe000) {
			s &= 0x3ff;
			s |= (filter->cache & 0xfff0000) >> 6;
			filter->cache = 0;
			if (s >= MBFL_WCSPLANE_SUPMIN && s < MBFL_WCSPLANE_SUPMAX) {
				CK((*filter->output_function)(s, filter->data));
			} else {		/* illegal character */
				s &= MBFL_WCSGROUP_MASK;
				s |= MBFL_WCSGROUP_THROUGH;
				CK((*filter->output_function)(s, filter->data));
			}
		} else {
			filter->cache = 0;
			CK((*filter->output_function)(s, filter->data));
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static const unsigned char mbfl_utf7imap_base64_table[] =
{
 /* 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', */
   0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,
 /* 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
   0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,
 /* 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', */
   0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
 /* 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
   0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
 /* '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', ',', '\0' */
   0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2b,0x2c,0x00
};

/*
 * wchar => UTF7-IMAP
 */
static int
mbfl_filt_conv_wchar_utf7imap(int c, mbfl_convert_filter *filter)
{
	int n, s;

	n = 0;
	if (c == 0x26) {
		n = 1;
	} else if ((c >= 0x20 && c <= 0x7e) || c == 0) {
		n = 2;
	} else if (c >= 0 && c < MBFL_WCSPLANE_UCS2MAX) {
		;
	} else if (c >= MBFL_WCSPLANE_SUPMIN && c < MBFL_WCSPLANE_SUPMAX) {
		s = ((c >> 10) - 0x40) | 0xd800;
		CK((*filter->filter_function)(s, filter));
		s = (c & 0x3ff) | 0xdc00;
		CK((*filter->filter_function)(s, filter));
		return c;
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
		return c;
	}

	switch (filter->status) {
	case 0:
		if (n != 0) {	/* directly encode characters */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
		} else {	/* Modified Base64 */
			CK((*filter->output_function)(0x26, filter->data));		/* '&' */
			filter->status = 1;
			filter->cache = c;
		}
		break;

	/* encode Modified Base64 */
	case 1:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 4) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s << 2) & 0x3c], filter->data));
			CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			filter->status = 0;
		} else {
			filter->status = 2;
			filter->cache = ((s & 0xf) << 16) | c;
		}
		break;

	case 2:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 2) & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s << 4) & 0x30], filter->data));
			CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			filter->status = 0;
		} else {
			filter->status = 3;
			filter->cache = ((s & 0x3) << 16) | c;
		}
		break;

	case 3:
		s = filter->cache;
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(s >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[s & 0x3f], filter->data));
		if (n != 0) {
			CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			CK((*filter->output_function)(c, filter->data));
			if (n == 1) {
				CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
			}
			filter->status = 0;
		} else {
			filter->status = 1;
			filter->cache = c;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;

}

static int
mbfl_filt_conv_wchar_utf7imap_flush(mbfl_convert_filter *filter)
{
	int status, cache;

	status = filter->status;
	cache = filter->cache;
	filter->status = 0;
	filter->cache = 0;
	/* flush fragments */
	switch (status) {
	case 1:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 10) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 4) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache << 2) & 0x3c], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 2:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 14) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 8) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 2) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache << 4) & 0x30], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;

	case 3:
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 12) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[(cache >> 6) & 0x3f], filter->data));
		CK((*filter->output_function)(mbfl_utf7imap_base64_table[cache & 0x3f], filter->data));
		CK((*filter->output_function)(0x2d, filter->data));		/* '-' */
		break;
	}
	return 0;
}


/*
 * wchar => ASCII
 */
static int
mbfl_filt_conv_wchar_ascii(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x80) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * wchar => cp1252
 */
static int
mbfl_filt_conv_wchar_cp1252(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0x100)	{
		/* look it up from the cp1252 table */
		s = -1;
		n = 30;
		while (n >= 0) {
			if (c == cp1252_ucs_table[n]) {
				s = 0x80 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_1)
		{
			s = c & MBFL_WCSPLANE_MASK;
		}
	}
	else if (c >= 0 && c < 0x100) {
		s = c;
	}
	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}
	return c;
}

/*
 * cp1252 => wchar
 */
static int
mbfl_filt_conv_cp1252_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0x80 && c < 0xa0) {
		s = cp1252_ucs_table[c - 0x80];
	} else {
		s = c;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-1
 */
static int
mbfl_filt_conv_wchar_8859_1(int c, mbfl_convert_filter *filter)
{
	if (c >= 0 && c < 0x100) {
		CK((*filter->output_function)(c, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-2 => wchar
 */
static int
mbfl_filt_conv_8859_2_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_2_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_2;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-2
 */
static int
mbfl_filt_conv_wchar_8859_2(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_2_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_2) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-3 => wchar
 */
static int
mbfl_filt_conv_8859_3_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_3_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_3;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-3
 */
static int
mbfl_filt_conv_wchar_8859_3(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_3_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_3) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-4 => wchar
 */
static int
mbfl_filt_conv_8859_4_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_4_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_4;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-4
 */
static int
mbfl_filt_conv_wchar_8859_4(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_4_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_4) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-5 => wchar
 */
static int
mbfl_filt_conv_8859_5_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_5_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_5;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-5
 */
static int
mbfl_filt_conv_wchar_8859_5(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_5_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_5) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-6 => wchar
 */
static int
mbfl_filt_conv_8859_6_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_6_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_6;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-6
 */
static int
mbfl_filt_conv_wchar_8859_6(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_6_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_6) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-7 => wchar
 */
static int
mbfl_filt_conv_8859_7_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_7_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_7;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-7
 */
static int
mbfl_filt_conv_wchar_8859_7(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_7_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_7) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-8 => wchar
 */
static int
mbfl_filt_conv_8859_8_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_8_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_8;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-8
 */
static int
mbfl_filt_conv_wchar_8859_8(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_8_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_8) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-9 => wchar
 */
static int
mbfl_filt_conv_8859_9_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_9_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_9;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-9
 */
static int
mbfl_filt_conv_wchar_8859_9(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_9_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_9) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-10 => wchar
 */
static int
mbfl_filt_conv_8859_10_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_10_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_10;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-10
 */
static int
mbfl_filt_conv_wchar_8859_10(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_10_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_10) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-13 => wchar
 */
static int
mbfl_filt_conv_8859_13_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_13_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_13;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-13
 */
static int
mbfl_filt_conv_wchar_8859_13(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_13_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_13) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-14 => wchar
 */
static int
mbfl_filt_conv_8859_14_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_14_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_14;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-14
 */
static int
mbfl_filt_conv_wchar_8859_14(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_14_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_14) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}

/*
 * ISO-8859-15 => wchar
 */
static int
mbfl_filt_conv_8859_15_wchar(int c, mbfl_convert_filter *filter)
{
	int s;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else if (c >= 0xa0 && c < 0x100) {
		s = iso8859_15_ucs_table[c - 0xa0];
		if (s <= 0) {
			s = c;
			s &= MBFL_WCSPLANE_MASK;
			s |= MBFL_WCSPLANE_8859_15;
		}
	} else {
		s = c;
		s &= MBFL_WCSGROUP_MASK;
		s |= MBFL_WCSGROUP_THROUGH;
	}

	CK((*filter->output_function)(s, filter->data));

	return c;
}

/*
 * wchar => ISO-8859-15
 */
static int
mbfl_filt_conv_wchar_8859_15(int c, mbfl_convert_filter *filter)
{
	int s, n;

	if (c >= 0 && c < 0xa0) {
		s = c;
	} else {
		s = -1;
		n = 95;
		while (n >= 0) {
			if (c == iso8859_15_ucs_table[n]) {
				s = 0xa0 + n;
				break;
			}
			n--;
		}
		if (s <= 0 && (c & ~MBFL_WCSPLANE_MASK) == MBFL_WCSPLANE_8859_15) {
			s = c & MBFL_WCSPLANE_MASK;
		}
	}

	if (s >= 0) {
		CK((*filter->output_function)(s, filter->data));
	} else {
		if (filter->illegal_mode != MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			CK(mbfl_filt_conv_illegal_output(c, filter));
		}
	}

	return c;
}



/*
 *
 * identify filter functions
 *
 */

static void
mbfl_filt_ident_common_ctor(mbfl_identify_filter *filter)
{
	filter->status = 0;
	filter->flag = 0;
}

static void
mbfl_filt_ident_common_dtor(mbfl_identify_filter *filter)
{
	filter->status = 0;
}

static int
mbfl_filt_ident_ascii(int c, mbfl_identify_filter *filter)
{
	if (c >= 0x20 && c < 0x80) {
		;
	} else if (c == 0x0d || c == 0x0a || c == 0x09 || c == 0) {	/* CR or LF or HTAB or null */
		;
	} else {
		filter->flag = 1;
	}

	return c;
}

static int
mbfl_filt_ident_utf8(int c, mbfl_identify_filter *filter)
{
	if (c < 0x80) {
		if (c < 0) { 
			filter->flag = 1;	/* bad */
		} else if (c != 0 && filter->status) {
			filter->flag = 1;	/* bad */
		}
		filter->status = 0;
	} else if (c < 0xc0) {
		switch (filter->status) {
		case 0x20: /* 3 byte code 2nd char */
		case 0x30: /* 4 byte code 2nd char */
		case 0x31: /* 4 byte code 3rd char */
		case 0x40: /* 5 byte code 2nd char */
		case 0x41: /* 5 byte code 3rd char */
		case 0x42: /* 5 byte code 4th char */
		case 0x50: /* 6 byte code 2nd char */
		case 0x51: /* 6 byte code 3rd char */
		case 0x52: /* 6 byte code 4th char */
		case 0x53: /* 6 byte code 5th char */
			filter->status++;
			break;
		case 0x10: /* 2 byte code 2nd char */
		case 0x21: /* 3 byte code 3rd char */
		case 0x32: /* 4 byte code 4th char */
		case 0x43: /* 5 byte code 5th char */
		case 0x54: /* 6 byte code 6th char */
			filter->status = 0;
			break;
		default:
			filter->flag = 1;	/* bad */
			filter->status = 0;
			break;
		}
	} else {
		if (filter->status) {
			filter->flag = 1;	/* bad */
		}
		filter->status = 0;
		if (c < 0xe0) {				/* 2 byte code first char */
			filter->status = 0x10;
		} else if (c < 0xf0) {		/* 3 byte code 1st char */
			filter->status = 0x20;
		} else if (c < 0xf8) {		/* 4 byte code 1st char */
			filter->status = 0x30;
		} else if (c < 0xfc) {		/* 5 byte code 1st char */
			filter->status = 0x40;
		} else if (c < 0xfe) {		/* 6 byte code 1st char */
			filter->status = 0x50;
		} else {
			filter->flag = 1;	/* bad */
		}
	}

	return c;
}

static int
mbfl_filt_ident_utf7(int c, mbfl_identify_filter *filter)
{
	int n;

	switch (filter->status) {
	/* directly encoded characters */
	case 0:
		if (c == 0x2b) {	/* '+'  shift character */
			filter->status++;
		} else if (c == 0x5c || c == 0x7e || c < 0 || c > 0x7f) {	/* illegal character */
			filter->flag = 1;	/* bad */
		}
		break;

	/* Modified Base64 */
	case 1:
	case 2:
		n = 0;
		if (c >= 0x41 && c <= 0x5a) {		/* A - Z */
			n = 1;
		} else if (c >= 0x61 && c <= 0x7a) {	/* a - z */
			n = 1;
		} else if (c >= 0x30 && c <= 0x39) {	/* 0 - 9 */
			n = 1;
		} else if (c == 0x2b) {			/* '+' */
			n = 1;
		} else if (c == 0x2f) {			/* '/' */
			n = 1;
		}
		if (n <= 0) {
			if (filter->status == 1 && c != 0x2d) {
				filter->flag = 1;	/* bad */
			} else if (c < 0 || c > 0x7f) {
				filter->flag = 1;	/* bad */
			}
			filter->status = 0;
		} else {
			filter->status = 2;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int
mbfl_filt_ident_eucjp(int c, mbfl_identify_filter *filter)
{
	switch (filter->status) {
	case  0:	/* latin */
		if (c >= 0 && c < 0x80) {	/* ok */
			;
		} else if (c > 0xa0 && c < 0xff) {	/* kanji first char */
			filter->status = 1;
		} else if (c == 0x8e) {				/* kana first char */
			filter->status = 2;
		} else if (c == 0x8f) {				/* X 0212 first char */
			filter->status = 3;
		} else {							/* bad */
			filter->flag = 1;
		}
		break;

	case  1:	/* got first half */
		if (c < 0xa1 || c > 0xfe) {		/* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;

	case  2:	/* got 0x8e */
		if (c < 0xa1 || c > 0xdf) {		/* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;

	case  3:	/* got 0x8f */
		if (c < 0xa1 || c > 0xfe) {		/* bad */
			filter->flag = 1;
		}
		filter->status++;
		break;
	case  4:	/* got 0x8f */
		if (c < 0xa1 || c > 0xfe) {		/* bad */
			filter->flag = 1;
		}
		filter->status = 0;
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static int
mbfl_filt_ident_sjis(int c, mbfl_identify_filter *filter)
{
	if (filter->status) {		/* kanji second char */
		if (c < 0x40 || c > 0xfc || c == 0x7f) {	/* bad */
		    filter->flag = 1;
		}
		filter->status = 0;
	} else if (c >= 0 && c < 0x80) {	/* latin  ok */
		;
	} else if (c > 0xa0 && c < 0xe0) {	/* kana  ok */
		;
	} else if (c > 0x80 && c < 0xf0 && c != 0xa0) {	/* kanji first char */
		filter->status = 1;
	} else {							/* bad */
		filter->flag = 1;
	}

	return c;
}

static int
mbfl_filt_ident_sjiswin(int c, mbfl_identify_filter *filter)
{
	if (filter->status) {		/* kanji second char */
		if (c < 0x40 || c > 0xfc || c == 0x7f) {	/* bad */
		    filter->flag = 1;
		}
		filter->status = 0;
	} else if (c >= 0 && c < 0x80) {	/* latin  ok */
		;
	} else if (c > 0xa0 && c < 0xe0) {	/* kana  ok */
		;
	} else if (c > 0x80 && c < 0xfd && c != 0xa0) {	/* kanji first char */
		filter->status = 1;
	} else {							/* bad */
		filter->flag = 1;
	}

	return c;
}

static int
mbfl_filt_ident_jis(int c, mbfl_identify_filter *filter)
{
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
		} else if (c == 0x0e) {			/* "kana in" */
			filter->status = 0x20;
		} else if (c == 0x0f) {			/* "kana out" */
			filter->status = 0;
		} else if ((filter->status == 0x80 || filter->status == 0x90) && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
/*	case 0x91:	 X 0212 second char */
	case 1:
		filter->status &= ~0xf;
		if (c == 0x1b) {
			goto retry;
		} else if (c < 0x21 || c > 0x7e) {		/* bad */
			filter->flag = 1;
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x28) {		/* '(' */
			filter->status++;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ ( */
	case 4:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else if (c == 0x44) {		/* 'D' */
			filter->status = 0x90;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42 || c == 0x48) {		/* 'B' or 'H' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else if (c == 0x49) {		/* 'I' */
			filter->status = 0x20;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

/* We only distinguish the MS extensions to ISO-8859-1.
 * Actually, this is pretty much a NO-OP, since the identification
 * system doesn't allow us to discriminate between a positive match,
 * a possible match and a definite non-match.
 * The problem here is that cp1252 looks like SJIS for certain chars.
 * */
static int
mbfl_filt_ident_cp1252(int c, mbfl_identify_filter *filter)
{
	if (c >= 0x80 && c < 0xa0)
		filter->flag = 0;
	else
		filter->flag = 1; /* not it */
	return c;	
}

static int
mbfl_filt_ident_2022jp(int c, mbfl_identify_filter *filter)
{
retry:
	switch (filter->status & 0xf) {
/*	case 0x00:	 ASCII */
/*	case 0x10:	 X 0201 latin */
/*	case 0x80:	 X 0208 */
	case 0:
		if (c == 0x1b) {
			filter->status += 2;
		} else if (filter->status == 0x80 && c > 0x20 && c < 0x7f) {		/* kanji first char */
			filter->status += 1;
		} else if (c >= 0 && c < 0x80) {		/* latin, CTLs */
			;
		} else {
			filter->flag = 1;	/* bad */
		}
		break;

/*	case 0x81:	 X 0208 second char */
	case 1:
		if (c == 0x1b) {
			filter->status++;
		} else {
			filter->status &= ~0xf;
			if (c < 0x21 || c > 0x7e) {		/* bad */
				filter->flag = 1;
			}
		}
		break;

	/* ESC */
	case 2:
		if (c == 0x24) {		/* '$' */
			filter->status++;
		} else if (c == 0x28) {		/* '(' */
			filter->status += 3;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC $ */
	case 3:
		if (c == 0x40 || c == 0x42) {		/* '@' or 'B' */
			filter->status = 0x80;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	/* ESC ( */
	case 5:
		if (c == 0x42) {		/* 'B' */
			filter->status = 0;
		} else if (c == 0x4a) {		/* 'J' */
			filter->status = 0x10;
		} else {
			filter->flag = 1;	/* bad */
			filter->status &= ~0xf;
			goto retry;
		}
		break;

	default:
		filter->status = 0;
		break;
	}

	return c;
}

static void
mbfl_filt_ident_false_ctor(mbfl_identify_filter *filter)
{
	filter->status = 0;
	filter->flag = 1;
}

static int
mbfl_filt_ident_false(int c, mbfl_identify_filter *filter)
{
	filter->flag = 1;	/* bad */
	return c;
}

static int
mbfl_filt_ident_true(int c, mbfl_identify_filter *filter)
{
	return c;
}



/*
 * convert filter
 */

/* setup filter function table */
static void
mbfl_convert_filter_set_vtbl(mbfl_convert_filter *filter, struct mbfl_convert_vtbl *vtbl)
{
	if (filter && vtbl) {
		filter->filter_ctor = vtbl->filter_ctor;
		filter->filter_dtor = vtbl->filter_dtor;
		filter->filter_function = vtbl->filter_function;
		filter->filter_flush = vtbl->filter_flush;
	}
}


static struct mbfl_convert_vtbl *
mbfl_convert_filter_get_vtbl(enum mbfl_no_encoding from, enum mbfl_no_encoding to)
{
	struct mbfl_convert_vtbl *vtbl;
	int i;

	if (to == mbfl_no_encoding_base64 ||
	    to == mbfl_no_encoding_qprint ||
	    to == mbfl_no_encoding_7bit) {
		from = mbfl_no_encoding_8bit;
	} else if (from == mbfl_no_encoding_base64 ||
	           from == mbfl_no_encoding_qprint ||
				  from == mbfl_no_encoding_uuencode) {
		to = mbfl_no_encoding_8bit;
	}

	i = 0;
	while ((vtbl = mbfl_convert_filter_list[i++]) != NULL){
		if (vtbl->from == from && vtbl->to == to) {
			return vtbl;
		}
	}

	return NULL;
}


static void
mbfl_convert_filter_select_vtbl(mbfl_convert_filter *filter)
{
	struct mbfl_convert_vtbl *vtbl;

	vtbl = mbfl_convert_filter_get_vtbl(filter->from->no_encoding, filter->to->no_encoding);
	if (vtbl == NULL) {
		vtbl = &vtbl_pass;
	}
	mbfl_convert_filter_set_vtbl(filter, vtbl);
}


/* filter pipe */
static int
mbfl_filter_output_pipe(int c, void* data)
{
	mbfl_convert_filter *filter = (mbfl_convert_filter*)data;
	return (*filter->filter_function)(c, filter);
}


/* null output */
static int
mbfl_filter_output_null(int c, void* data)
{
	return c;
}


mbfl_convert_filter *
mbfl_convert_filter_new(
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int (*output_function)(int, void*),
    int (*flush_function)(void*),
    void* data)
{
	mbfl_convert_filter * filter;

	/* allocate */
	filter = (mbfl_convert_filter *)mbfl_malloc(sizeof(mbfl_convert_filter));
	if (filter == NULL) {
		return NULL;
	}

	/* encoding structure */
	filter->from = mbfl_no2encoding(from);
	filter->to = mbfl_no2encoding(to);
	if (filter->from == NULL) {
		filter->from = &mbfl_encoding_pass;
	}
	if (filter->to == NULL) {
		filter->to = &mbfl_encoding_pass;
	}

	if (output_function != NULL) {
		filter->output_function = output_function;
	} else {
		filter->output_function = mbfl_filter_output_null;
	}
	filter->flush_function = flush_function;
	filter->data = data;
	filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	filter->illegal_substchar = 0x3f;		/* '?' */

	/* setup the function table */
	mbfl_convert_filter_select_vtbl(filter);

	/* constructor */
	(*filter->filter_ctor)(filter);

	return filter;
}

void
mbfl_convert_filter_delete(mbfl_convert_filter *filter)
{
	if (filter) {
		(*filter->filter_dtor)(filter);
		mbfl_free((void*)filter);
	}
}

int
mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter)
{
	return (*filter->filter_function)(c, filter);
}

int
mbfl_convert_filter_flush(mbfl_convert_filter *filter)
{
	(*filter->filter_flush)(filter);
	return (filter->flush_function ? (*filter->flush_function)(filter->data) : 0);
}

void
mbfl_convert_filter_reset(
    mbfl_convert_filter *filter,
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to)
{
	/* destruct old filter */
	(*filter->filter_dtor)(filter);

	/* resset filter member */
	filter->from = mbfl_no2encoding(from);
	filter->to = mbfl_no2encoding(to);

	/* set the vtbl */
	mbfl_convert_filter_select_vtbl(filter);

	/* construct new filter */
	(*filter->filter_ctor)(filter);
}

void
mbfl_convert_filter_copy(
    mbfl_convert_filter *src,
    mbfl_convert_filter *dist)
{
	dist->filter_ctor = src->filter_ctor;
	dist->filter_dtor = src->filter_dtor;
	dist->filter_function = src->filter_function;
	dist->filter_flush = src->filter_flush;
	dist->output_function = src->output_function;
	dist->flush_function = src->flush_function;
	dist->data = src->data;
	dist->status = src->status;
	dist->cache = src->cache;
	dist->from = src->from;
	dist->to = src->to;
	dist->illegal_mode = src->illegal_mode;
	dist->illegal_substchar = src->illegal_substchar;
}

static int
mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src)
{
	int n;
	unsigned char *p;

	p = src->buffer;
	n = src->pos;
	while (n > 0) {
		if ((*filter->filter_function)(*p++, filter) < 0) {
			return -1;
		}
		n--;
	}

	return n;
}

static int
mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p)
{
	int c;

	while ((c = *p++) != '\0') {
		if ((*filter->filter_function)(c, filter) < 0) {
			return -1;
		}
	}

	return 0;
}

#if 0
static int
mbfl_convert_filter_strncat(mbfl_convert_filter *filter, const unsigned char *p, int n)
{
	while (n > 0) {
		if ((*filter->filter_function)(*p++, filter) < 0) {
			return -1;
		}
		n--;
	}

	return n;
}
#endif

/* hex character table "0123456789ABCDEF" */
static char mbfl_hexchar_table[] = {
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46
};

/* illegal character output function for conv-filter */
int
mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter)
{
	int mode_backup, ret, n, m, r;

	ret = 0;
	mode_backup = filter->illegal_mode;
	filter->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
	switch (mode_backup) {
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR:
		ret = (*filter->filter_function)(filter->illegal_substchar, filter);
		break;
	case MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG:
		if (c >= 0) {
			if (c < MBFL_WCSGROUP_UCS4MAX) {	/* unicode */
				ret = mbfl_convert_filter_strcat(filter, "U+");
			} else {
				if (c < MBFL_WCSGROUP_WCHARMAX) {
					m = c & ~MBFL_WCSPLANE_MASK;
					switch (m) {
					case MBFL_WCSPLANE_JIS0208:
						ret = mbfl_convert_filter_strcat(filter, "JIS+");
						break;
					case MBFL_WCSPLANE_JIS0212:
						ret = mbfl_convert_filter_strcat(filter, "JIS2+");
						break;
					case MBFL_WCSPLANE_WINCP932:
						ret = mbfl_convert_filter_strcat(filter, "W932+");
						break;
					case MBFL_WCSPLANE_8859_1:
						ret = mbfl_convert_filter_strcat(filter, "I8859_1+");
						break;
					default:
						ret = mbfl_convert_filter_strcat(filter, "?+");
						break;
					}
					c &= MBFL_WCSPLANE_MASK;
				} else {
					ret = mbfl_convert_filter_strcat(filter, "BAD+");
					c &= MBFL_WCSGROUP_MASK;
				}
			}
			if (ret >= 0) {
				m = 0;
				r = 28;
				while (r >= 0) {
					n = (c >> r) & 0xf;
					if (n || m) {
						m = 1;
						ret = (*filter->filter_function)(mbfl_hexchar_table[n], filter);
						if (ret < 0) {
							break;
						}
					}
					r -= 4;
				}
				if (m == 0 && ret >= 0) {
					ret = (*filter->filter_function)(mbfl_hexchar_table[0], filter);
				}
			}
		}
		break;
	default:
		break;
	}
	filter->illegal_mode = mode_backup;

	return ret;
}


/*
 * identify filter
 */

static void
mbfl_identify_filter_set_vtbl(mbfl_identify_filter *filter, struct mbfl_identify_vtbl *vtbl)
{
	if (filter && vtbl) {
		filter->filter_ctor = vtbl->filter_ctor;
		filter->filter_dtor = vtbl->filter_dtor;
		filter->filter_function = vtbl->filter_function;
	}
}

static struct mbfl_identify_vtbl *
mbfl_identify_filter_get_vtbl(enum mbfl_no_encoding encoding)
{
	struct mbfl_identify_vtbl *vtbl;
	int i;

	i = 0;
	while ((vtbl = mbfl_identify_filter_list[i++]) != NULL) {
		if (vtbl->encoding == encoding) {
			break;
		}
	}

	return vtbl;
}

static void
mbfl_identify_filter_select_vtbl(mbfl_identify_filter *filter)
{
	struct mbfl_identify_vtbl *vtbl;

	vtbl = mbfl_identify_filter_get_vtbl(filter->encoding->no_encoding);
	if (vtbl == NULL) {
		vtbl = &vtbl_identify_false;
	}
	mbfl_identify_filter_set_vtbl(filter, vtbl);
}

mbfl_identify_filter *
mbfl_identify_filter_new(enum mbfl_no_encoding encoding)
{
	mbfl_identify_filter * filter;

	/* allocate */
	filter = (mbfl_identify_filter *)mbfl_malloc(sizeof(mbfl_identify_filter));
	if (filter == NULL) {
		return NULL;
	}

	/* encoding structure */
	filter->encoding = mbfl_no2encoding(encoding);
	if (filter->encoding == NULL) {
		filter->encoding = &mbfl_encoding_pass;
	}

	filter->status = 0;
	filter->flag = 0;
	filter->score = 0;

	/* setup the function table */
	mbfl_identify_filter_select_vtbl(filter);

	/* constructor */
	(*filter->filter_ctor)(filter);

	return filter;
}

void
mbfl_identify_filter_delete(mbfl_identify_filter *filter)
{
	if (filter) {
		(*filter->filter_dtor)(filter);
		mbfl_free((void*)filter);
	}
}



/*
 *  buffering converter
 */
mbfl_buffer_converter *
mbfl_buffer_converter_new(
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int buf_initsz)
{
	mbfl_buffer_converter *convd;

	/* allocate */
	convd = (mbfl_buffer_converter*)mbfl_malloc(sizeof (mbfl_buffer_converter));
	if (convd == NULL) {
		return NULL;
	}

	/* initialize */
	convd->from = mbfl_no2encoding(from);
	convd->to = mbfl_no2encoding(to);
	if (convd->from == NULL) {
		convd->from = &mbfl_encoding_pass;
	}
	if (convd->to == NULL) {
		convd->to = &mbfl_encoding_pass;
	}

	/* create convert filter */
	convd->filter1 = NULL;
	convd->filter2 = NULL;
	if (mbfl_convert_filter_get_vtbl(convd->from->no_encoding, convd->to->no_encoding) != NULL) {
		convd->filter1 = mbfl_convert_filter_new(convd->from->no_encoding, convd->to->no_encoding, mbfl_memory_device_output, 0, &convd->device);
	} else {
		convd->filter2 = mbfl_convert_filter_new(mbfl_no_encoding_wchar, convd->to->no_encoding, mbfl_memory_device_output, 0, &convd->device);
		if (convd->filter2 != NULL) {
			convd->filter1 = mbfl_convert_filter_new(convd->from->no_encoding, mbfl_no_encoding_wchar, (int (*)(int, void*))convd->filter2->filter_function, NULL, convd->filter2);
			if (convd->filter1 == NULL) {
				mbfl_convert_filter_delete(convd->filter2);
			}
		}
	}
	if (convd->filter1 == NULL) {
		return NULL;
	}

	mbfl_memory_device_init(&convd->device, buf_initsz, buf_initsz/4);

	return convd;
}

void
mbfl_buffer_converter_delete(mbfl_buffer_converter *convd)
{
	if (convd != NULL) {
		if (convd->filter1) {
			mbfl_convert_filter_delete(convd->filter1);
		}
		if (convd->filter2) {
			mbfl_convert_filter_delete(convd->filter2);
		}
		mbfl_memory_device_clear(&convd->device);
		mbfl_free((void*)convd);
	}
}

void
mbfl_buffer_converter_reset(mbfl_buffer_converter *convd)
{
	mbfl_memory_device_reset(&convd->device);
}

int
mbfl_buffer_converter_illegal_mode(mbfl_buffer_converter *convd, int mode)
{
	if (convd != NULL) {
		if (convd->filter2 != NULL) {
			convd->filter2->illegal_mode = mode;
		} else if (convd->filter1 != NULL) {
			convd->filter1->illegal_mode = mode;
		} else {
			return 0;
		}
	}

	return 1;
}

int
mbfl_buffer_converter_illegal_substchar(mbfl_buffer_converter *convd, int substchar)
{
	if (convd != NULL) {
		if (convd->filter2 != NULL) {
			convd->filter2->illegal_substchar = substchar;
		} else if (convd->filter1 != NULL) {
			convd->filter1->illegal_substchar = substchar;
		} else {
			return 0;
		}
	}

	return 1;
}

int
mbfl_buffer_converter_strncat(mbfl_buffer_converter *convd, const unsigned char *p, int n)
{
	mbfl_convert_filter *filter;
	int (*filter_function)(int c, mbfl_convert_filter *filter);

	if (convd != NULL && p != NULL) {
		filter = convd->filter1;
		if (filter != NULL) {
			filter_function = filter->filter_function;
			while (n > 0) {
				if ((*filter_function)(*p++, filter) < 0) {
					break;
				}
				n--;
			}
		}
	}

	return n;
}

int
mbfl_buffer_converter_feed(mbfl_buffer_converter *convd, mbfl_string *string)
{
	int n;
	unsigned char *p;
	mbfl_convert_filter *filter;
	int (*filter_function)(int c, mbfl_convert_filter *filter);

	if (convd == NULL || string == NULL) {
		return -1;
	}
	mbfl_memory_device_realloc(&convd->device, convd->device.pos + string->len, string->len/4);
	/* feed data */
	n = string->len;
	p = string->val;
	filter = convd->filter1;
	if (filter != NULL) {
		filter_function = filter->filter_function;
		while (n > 0) {
			if ((*filter_function)(*p++, filter) < 0) {
				return -1;
			}
			n--;
		}
	}

	return 0;
}

int
mbfl_buffer_converter_flush(mbfl_buffer_converter *convd)
{
	if (convd == NULL) {
		return -1;
	}

	if (convd->filter1 != NULL) {
		mbfl_convert_filter_flush(convd->filter1);
	}
	if (convd->filter2 != NULL) {
		mbfl_convert_filter_flush(convd->filter2);
	}

	return 0;
}

mbfl_string *
mbfl_buffer_converter_getbuffer(mbfl_buffer_converter *convd, mbfl_string *result)
{
	if (convd != NULL && result != NULL && convd->device.buffer != NULL) {
		result->no_encoding = convd->to->no_encoding;
		result->val = convd->device.buffer;
		result->len = convd->device.pos;
	} else {
		result = NULL;
	}

	return result;
}

mbfl_string *
mbfl_buffer_converter_result(mbfl_buffer_converter *convd, mbfl_string *result)
{
	if (convd == NULL || result == NULL) {
		return NULL;
	}
	result->no_encoding = convd->to->no_encoding;
	return mbfl_memory_device_result(&convd->device, result);
}

mbfl_string *
mbfl_buffer_converter_feed_result(mbfl_buffer_converter *convd, mbfl_string *string, mbfl_string *result)
{
	if (convd == NULL || string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_buffer_converter_feed(convd, string);
	if (convd->filter1 != NULL) {
		mbfl_convert_filter_flush(convd->filter1);
	}
	if (convd->filter2 != NULL) {
		mbfl_convert_filter_flush(convd->filter2);
	}
	result->no_encoding = convd->to->no_encoding;
	return mbfl_memory_device_result(&convd->device, result);
}


/*
 * encoding detector
 */
mbfl_encoding_detector *
mbfl_encoding_detector_new(enum mbfl_no_encoding *elist, int eliztsz)
{
	mbfl_encoding_detector *identd;

	int i, num;
	mbfl_identify_filter *filter;

	if (elist == NULL || eliztsz <= 0) {
		return NULL;
	}

	/* allocate */
	identd = (mbfl_encoding_detector*)mbfl_malloc(sizeof(mbfl_encoding_detector));
	if (identd == NULL) {
		return NULL;
	}
	identd->filter_list = (mbfl_identify_filter **)mbfl_calloc(eliztsz, sizeof(mbfl_identify_filter *));
	if (identd->filter_list == NULL) {
		mbfl_free(identd);
		return NULL;
	}

	/* create filters */
	i = 0;
	num = 0;
	while (i < eliztsz) {
		filter = mbfl_identify_filter_new(elist[i]);
		if (filter != NULL) {
			identd->filter_list[num] = filter;
			num++;
		}
		i++;
	}
	identd->filter_list_size = num;

	return identd;
}

void
mbfl_encoding_detector_delete(mbfl_encoding_detector *identd)
{
	int i;

	if (identd != NULL) {
		if (identd->filter_list != NULL) {
			i = identd->filter_list_size;
			while (i > 0) {
				i--;
				mbfl_identify_filter_delete(identd->filter_list[i]);
			}
			mbfl_free((void *)identd->filter_list);
		}
		mbfl_free((void *)identd);
	}
}

int
mbfl_encoding_detector_feed(mbfl_encoding_detector *identd, mbfl_string *string)
{
	int i, n, num, bad, res;
	unsigned char *p;
	mbfl_identify_filter *filter;

	res = 0;
	/* feed data */
	if (identd != NULL && string != NULL && string->val != NULL) {
		num = identd->filter_list_size;
		n = string->len;
		p = string->val;
		while (n > 0) {
			i = 0;
			bad = 0;
			while (i < num) {
				filter = identd->filter_list[i];
				(*filter->filter_function)(*p, filter);
				if (filter->flag) {
					bad++;
				}
				i++;
			}
			if ((num - 1) <= bad) {
				res = 1;
				break;
			}
			p++;
			n--;
		}
	}

	return res;
}

enum mbfl_no_encoding mbfl_encoding_detector_judge(mbfl_encoding_detector *identd)
{
	mbfl_identify_filter *filter;
	enum mbfl_no_encoding encoding;
	int n;

	/* judge */
	encoding = mbfl_no_encoding_invalid;
	if (identd != NULL) {
		n = identd->filter_list_size - 1;
		while (n >= 0) {
			filter = identd->filter_list[n];
			if (!filter->flag) {
				encoding = filter->encoding->no_encoding;
			}
			n--;
		}
	}

	return encoding;
}


/*
 * encoding converter
 */
mbfl_string *
mbfl_convert_encoding(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding toenc)
{
	int n;
	unsigned char *p;
	mbfl_encoding *encoding;
	mbfl_memory_device device;
	mbfl_convert_filter *filter1;
	mbfl_convert_filter *filter2;

	/* initialize */
	encoding = mbfl_no2encoding(toenc);
	if (encoding == NULL || string == NULL || result == NULL) {
		return NULL;
	}

	filter1 = NULL;
	filter2 = NULL;
	if (mbfl_convert_filter_get_vtbl(string->no_encoding, toenc) != NULL) {
		filter1 = mbfl_convert_filter_new(string->no_encoding, toenc, mbfl_memory_device_output, 0, &device);
	} else {
		filter2 = mbfl_convert_filter_new(mbfl_no_encoding_wchar, toenc, mbfl_memory_device_output, 0, &device);
		if (filter2 != NULL) {
			filter1 = mbfl_convert_filter_new(string->no_encoding, mbfl_no_encoding_wchar, (int (*)(int, void*))filter2->filter_function, NULL, filter2);
			if (filter1 == NULL) {
				mbfl_convert_filter_delete(filter2);
			}
		}
	}
	if (filter1 == NULL) {
		return NULL;
	}
	filter2->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	filter2->illegal_substchar = 0x3f;		/* '?' */
	mbfl_memory_device_init(&device, string->len, (string->len >> 2) + 8);

	/* feed data */
	n = string->len;
	p = string->val;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter1->filter_function)(*p++, filter1) < 0) {
				break;
			}
			n--;
		}
	}

	mbfl_convert_filter_flush(filter1);
	mbfl_convert_filter_delete(filter1);
	if (filter2 != NULL) {
		mbfl_convert_filter_flush(filter2);
		mbfl_convert_filter_delete(filter2);
	}

	return mbfl_memory_device_result(&device, result);
}


/*
 * identify encoding
 */
mbfl_encoding *
mbfl_identify_encoding(mbfl_string *string, enum mbfl_no_encoding *elist, int eliztsz)
{
	int i, n, num, bad;
	unsigned char *p;
	struct mbfl_identify_vtbl *vtbl;
	mbfl_identify_filter *flist, *filter;
	mbfl_encoding *encoding;

	/* initialize */
	flist = (mbfl_identify_filter *)mbfl_calloc(eliztsz, sizeof(mbfl_identify_filter));
	if (flist == NULL) {
		return NULL;
	}
	i = 0;
	num = 0;
	if (elist != NULL) {
		while (i < eliztsz) {
			vtbl = mbfl_identify_filter_get_vtbl(elist[i]);
			if (vtbl != NULL) {
				filter = &flist[num];
				mbfl_identify_filter_set_vtbl(filter, vtbl);
				filter->encoding = mbfl_no2encoding(vtbl->encoding);
				(*filter->filter_ctor)(filter);
				num++;
			}
			i++;
		}
	}

	/* feed data */
	n = string->len;
	p = string->val;
	if (p != NULL) {
		while (n > 0) {
			i = 0;
			bad = 0;
			while (i < num) {
				filter = &flist[i];
				(*filter->filter_function)(*p, filter);
				if (filter->flag) {
					bad++;
				}
				i++;
			}
			if ((num - 1) <= bad) {
				break;
			}
			p++;
			n--;
		}
	}

	/* judge */
	i = num - 1;
	bad = 1;
	encoding = NULL;
	while (i >= 0) {
		filter = &flist[i];
		if (filter->flag) {
			bad++;
		} else {
			encoding = filter->encoding;
		}
		i--;
	}
#if 0
	if (bad < num) {
		encoding = NULL;
	}
#endif

	i = 0;
	while (i < num) {
		filter = &flist[i];
		(*filter->filter_dtor)(filter);
		i++;
	}
	mbfl_free((void *)flist);

	return encoding;
}

const char*
mbfl_identify_encoding_name(mbfl_string *string, enum mbfl_no_encoding *elist, int eliztsz)
{
	mbfl_encoding *encoding;

	encoding = mbfl_identify_encoding(string, elist, eliztsz);
	if (encoding != NULL &&
	    encoding->no_encoding > mbfl_no_encoding_charset_min &&
	    encoding->no_encoding < mbfl_no_encoding_charset_max) {
		return encoding->name;
	} else {
		return NULL;
	}
}

enum mbfl_no_encoding
mbfl_identify_encoding_no(mbfl_string *string, enum mbfl_no_encoding *elist, int eliztsz)
{
	mbfl_encoding *encoding;

	encoding = mbfl_identify_encoding(string, elist, eliztsz);
	if (encoding != NULL &&
	    encoding->no_encoding > mbfl_no_encoding_charset_min &&
	    encoding->no_encoding < mbfl_no_encoding_charset_max) {
		return encoding->no_encoding;
	} else {
		return mbfl_no_encoding_invalid;
	}
}


/*
 *  strlen
 */
static int
filter_count_output(int c, void *data)
{
	(*(int *)data)++;
	return c;
}

int
mbfl_strlen(mbfl_string *string)
{
	int len, n, m, k;
	unsigned char *p;
	const unsigned char *mbtab;
	mbfl_encoding *encoding;

	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL || string == NULL) {
		return -1;
	}

	len = 0;
	if (encoding->flag & MBFL_ENCTYPE_SBCS) {
		len = string->len;
	} else if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
		len = string->len/2;
	} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
		len = string->len/4;
	} else if (encoding->mblen_table != NULL) {
		mbtab = encoding->mblen_table;
		n = 0;
		p = string->val;
		k = string->len;
		/* count */
		if (p != NULL) {
			while (n < k) {
				m = mbtab[*p];
				n += m;
				p += m;
				len++;
			};
		}
	} else {
		/* wchar filter */
		mbfl_convert_filter *filter = mbfl_convert_filter_new(
		  string->no_encoding, 
		  mbfl_no_encoding_wchar,
		  filter_count_output, 0, &len);
		if (filter == NULL) {
			return -1;
		}
		/* count */
		n = string->len;
		p = string->val;
		if (p != NULL) {
			while (n > 0) {
				(*filter->filter_function)(*p++, filter);
				n--;
			}
		}
		mbfl_convert_filter_delete(filter);
	}

	return len;
}


/*
 *  strpos
 */
struct collector_strpos_data {
	mbfl_convert_filter *next_filter;
	mbfl_wchar_device needle;
	int needle_len;
	int start;
	int output;
	int found_pos;
	int needle_pos;
	int matched_pos;
};

static int
collector_strpos(int c, void* data)
{
	int *p, *h, *m, n;
	struct collector_strpos_data *pc = (struct collector_strpos_data*)data;

	if (pc->output >= pc->start) {
		if (c == pc->needle.buffer[pc->needle_pos]) {
			if (pc->needle_pos == 0) {
				pc->found_pos = pc->output;			/* found position */
			}
			pc->needle_pos++;						/* needle pointer */
			if (pc->needle_pos >= pc->needle_len) {
				pc->matched_pos = pc->found_pos;	/* matched position */
				pc->needle_pos--;
				goto retry;
			}
		} else if (pc->needle_pos != 0) {
retry:
			h = pc->needle.buffer;
			h++;
			for (;;) {
				pc->found_pos++;
				p = h;
				m = pc->needle.buffer;
				n = pc->needle_pos - 1;
				while (n > 0 && *p == *m) {
					n--;
					p++;
					m++;
				}
				if (n <= 0) {
					if (*m != c) {
						pc->needle_pos = 0;
					}
					break;
				} else {
					h++;
					pc->needle_pos--;
				}
			}
		}
	}

	pc->output++;
	return c;
}

int
mbfl_strpos(
    mbfl_string *haystack,
    mbfl_string *needle,
    int offset,
    int reverse)
{
	int n, result;
	unsigned char *p;
	mbfl_convert_filter *filter;
	struct collector_strpos_data pc;

	if (haystack == NULL || needle == NULL) {
		return -8;
	}
	/* needle is converted into wchar */
	mbfl_wchar_device_init(&pc.needle);
	filter = mbfl_convert_filter_new(
	  needle->no_encoding,
	  mbfl_no_encoding_wchar,
	  mbfl_wchar_device_output, 0, &pc.needle);
	if (filter == NULL) {
		return -4;
	}
	p = needle->val;
	n = needle->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter->filter_function)(*p++, filter) < 0) {
				break;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(filter);
	mbfl_convert_filter_delete(filter);
	pc.needle_len = pc.needle.pos;
	if (pc.needle.buffer == NULL) {
		return -4;
	}
	if (pc.needle_len <= 0) {
		mbfl_wchar_device_clear(&pc.needle);
		return -2;
	}
	/* initialize filter and collector data */
	filter = mbfl_convert_filter_new(
	  haystack->no_encoding,
	  mbfl_no_encoding_wchar,
	  collector_strpos, 0, &pc);
	if (filter == NULL) {
		mbfl_wchar_device_clear(&pc.needle);
		return -4;
	}
	pc.start = offset;
	pc.output = 0;
	pc.needle_pos = 0;
	pc.found_pos = 0;
	pc.matched_pos = -1;

	/* feed data */
	p = haystack->val;
	n = haystack->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter->filter_function)(*p++, filter) < 0) {
				pc.matched_pos = -4;
				break;
			}
			if (pc.matched_pos >= 0 && !reverse) {
				break;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(filter);
	result = pc.matched_pos;
	mbfl_convert_filter_delete(filter);
	mbfl_wchar_device_clear(&pc.needle);

	return result;
}


/*
 *  substr
 */
struct collector_substr_data {
	mbfl_convert_filter *next_filter;
	int start;
	int stop;
	int output;
};

static int
collector_substr(int c, void* data)
{
	struct collector_substr_data *pc = (struct collector_substr_data*)data;

	if (pc->output >= pc->stop) {
		return -1;
	}

	if (pc->output >= pc->start) {
		(*pc->next_filter->filter_function)(c, pc->next_filter);
	}

	pc->output++;

	return c;
}

mbfl_string *
mbfl_substr(
    mbfl_string *string,
    mbfl_string *result,
    int from,
    int length)
{
	mbfl_encoding *encoding;
	int n, m, k, len, start, end;
	unsigned char *p, *w;
	const unsigned char *mbtab;

	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL || string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;

	if ((encoding->flag & (MBFL_ENCTYPE_SBCS | MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE | MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) ||
	   encoding->mblen_table != NULL) {
		len = string->len;
		start = from;
		end = from + length;
		if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_MWC2LE)) {
			start *= 2;
			end = start + length*2;
		} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_MWC4LE)) {
			start *= 4;
			end = start + length*4;
		} else if (encoding->mblen_table != NULL) {
			mbtab = encoding->mblen_table;
			start = 0;
			end = 0;
			n = 0;
			k = 0;
			p = string->val;
			if (p != NULL) {
				/* search start position */
				while (k <= from) {
					start = n;
					if (n >= len) {
						break;
					}
					m = mbtab[*p];
					n += m;
					p += m;
					k++;
				}
				/* detect end position */
				k = 0;
				end = start;
				while (k < length) {
					end = n;
					if (n >= len) {
						break;
					}
					m = mbtab[*p];
					n += m;
					p += m;
					k++;
				}
			}
		}

		if (start > len) {
			start = len;
		}
		if (start < 0) {
			start = 0;
		}
		if (end > len) {
			end = len;
		}
		if (end < 0) {
			end = 0;
		}
		if (start > end) {
			start = end;
		}

		/* allocate memory and copy */
		n = end - start;
		result->len = 0;
		result->val = w = (unsigned char*)mbfl_malloc((n + 8)*sizeof(unsigned char));
		if (w != NULL) {
			p = string->val;
			if (p != NULL) {
				p += start;
				result->len = n;
				while (n > 0) {
					*w++ = *p++;
					n--;
				}
			}
			*w++ = '\0';
			*w++ = '\0';
			*w++ = '\0';
			*w = '\0';
		} else {
			result = NULL;
		}
	} else {
		mbfl_memory_device device;
		struct collector_substr_data pc;
		mbfl_convert_filter *decoder;
		mbfl_convert_filter *encoder;

		mbfl_memory_device_init(&device, length + 1, 0);
		mbfl_string_init(result);
		result->no_language = string->no_language;
		result->no_encoding = string->no_encoding;
		/* output code filter */
		decoder = mbfl_convert_filter_new(
		    mbfl_no_encoding_wchar,
		    string->no_encoding,
		    mbfl_memory_device_output, 0, &device);
		/* wchar filter */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_substr, 0, &pc);
		if (decoder == NULL || encoder == NULL) {
			mbfl_convert_filter_delete(encoder);
			mbfl_convert_filter_delete(decoder);
			return NULL;
		}
		pc.next_filter = decoder;
		pc.start = from;
		pc.stop = from + length;
		pc.output = 0;

		/* feed data */
		p = string->val;
		n = string->len;
		if (p != NULL) {
			while (n > 0) {
				if ((*encoder->filter_function)(*p++, encoder) < 0) {
					break;
				}
				n--;
			}
		}

		mbfl_convert_filter_flush(encoder);
		mbfl_convert_filter_flush(decoder);
		result = mbfl_memory_device_result(&device, result);
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(decoder);
	}

	return result;
}


/*
 *  strcut
 */
mbfl_string *
mbfl_strcut(
    mbfl_string *string,
    mbfl_string *result,
    int from,
    int length)
{
	mbfl_encoding *encoding;
	int n, m, k, len, start, end;
	unsigned char *p, *w;
	const unsigned char *mbtab;
	mbfl_memory_device device;
	mbfl_convert_filter *encoder, *encoder_tmp, *decoder, *decoder_tmp;

	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL || string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;

	if ((encoding->flag & (MBFL_ENCTYPE_SBCS | MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE | MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) ||
	   encoding->mblen_table != NULL) {
		len = string->len;
		start = from;
		end = from + length;
		if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
			start /= 2;
			start *= 2;
			end = length/2;
			end *= 2;
			end += start;
		} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
			start /= 4;
			start *= 4;
			end = length/4;
			end *= 4;
			end += start;
		} else if (encoding->mblen_table != NULL) {
			mbtab = encoding->mblen_table;
			start = 0;
			end = 0;
			n = 0;
			p = string->val;
			if (p != NULL) {
				/* search start position */
				for (;;) {
					m = mbtab[*p];
					n += m;
					p += m;
					if (n > from) {
						break;
					}
					start = n;
				}
				/* search end position */
				k = start + length;
				if (k >= string->len) {
					end = string->len;
				} else {
					end = start;
					while (n <= k) {
						end = n;
						m = mbtab[*p];
						n += m;
						p += m;
					}
				}
			}
		}

		if (start > len) {
			start = len;
		}
		if (start < 0) {
			start = 0;
		}
		if (end > len) {
			end = len;
		}
		if (end < 0) {
			end = 0;
		}
		if (start > end) {
			start = end;
		}
		/* allocate memory and copy string */
		n = end - start;
		result->len = 0;
		result->val = w = (unsigned char*)mbfl_malloc((n + 8)*sizeof(unsigned char));
		if (w != NULL) {
			result->len = n;
			p = &(string->val[start]);
			while (n > 0) {
				*w++ = *p++;
				n--;
			}
			*w++ = '\0';
			*w++ = '\0';
			*w++ = '\0';
			*w = '\0';
		} else {
			result = NULL;
		}
	} else {
		/* wchar filter */
		encoder = mbfl_convert_filter_new(
		  string->no_encoding,
		  mbfl_no_encoding_wchar,
		  mbfl_filter_output_null, 0, 0);
		encoder_tmp = mbfl_convert_filter_new(
		  string->no_encoding,
		  mbfl_no_encoding_wchar,
		  mbfl_filter_output_null, 0, 0);
		/* output code filter */
		decoder = mbfl_convert_filter_new(
		  mbfl_no_encoding_wchar,
		  string->no_encoding,
		  mbfl_memory_device_output, 0, &device);
		decoder_tmp = mbfl_convert_filter_new(
		  mbfl_no_encoding_wchar,
		  string->no_encoding,
		  mbfl_memory_device_output, 0, &device);
		if (encoder == NULL || encoder_tmp == NULL || decoder == NULL || decoder_tmp == NULL) {
			mbfl_convert_filter_delete(encoder);
			mbfl_convert_filter_delete(encoder_tmp);
			mbfl_convert_filter_delete(decoder);
			mbfl_convert_filter_delete(decoder_tmp);
			return NULL;
		}
		mbfl_memory_device_init(&device, length + 8, 0);
		k = 0;
		n = 0;
		p = string->val;
		if (p != NULL) {
			/* seartch start position */
			while (n < from) {
				(*encoder->filter_function)(*p++, encoder);
				n++;
			}
			/* output a little shorter than "length" */
			encoder->output_function = mbfl_filter_output_pipe;
			encoder->data = decoder;
			k = length - 20;
			len = string->len;
			while (n < len && device.pos < k) {
				(*encoder->filter_function)(*p++, encoder);
				n++;
			}
			/* detect end position */
			for (;;) {
				/* backup current state */
				k = device.pos;
				mbfl_convert_filter_copy(encoder, encoder_tmp);
				mbfl_convert_filter_copy(decoder, decoder_tmp);
				if (n >= len) {
					break;
				}
				/* feed 1byte and flush */
				(*encoder->filter_function)(*p, encoder);
				(*encoder->filter_flush)(encoder);
				(*decoder->filter_flush)(decoder);
				if (device.pos > length) {
					break;
				}
				/* restore filter and re-feed data */
				device.pos = k;
				mbfl_convert_filter_copy(encoder_tmp, encoder);
				mbfl_convert_filter_copy(decoder_tmp, decoder);
				(*encoder->filter_function)(*p, encoder);
				p++;
				n++;
			}
			device.pos = k;
			mbfl_convert_filter_copy(encoder_tmp, encoder);
			mbfl_convert_filter_copy(decoder_tmp, decoder);
			mbfl_convert_filter_flush(encoder);
			mbfl_convert_filter_flush(decoder);
		}
		result = mbfl_memory_device_result(&device, result);
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(encoder_tmp);
		mbfl_convert_filter_delete(decoder);
		mbfl_convert_filter_delete(decoder_tmp);
	}

	return result;
}


/*
 *  strwidth
 */
static int
filter_count_width(int c, void* data)
{
	if (c >= 0x20) {
		if (c < 0x2000 || (c > 0xff60 && c < 0xffa0)) {
			(*(int *)data)++;
		} else {
			(*(int *)data) += 2;
		}
	}

	return c;
}

int
mbfl_strwidth(mbfl_string *string)
{
	int len, n;
	unsigned char *p;
	mbfl_convert_filter *filter;

	len = 0;
	if (string->len > 0 && string->val != NULL) {
		/* wchar filter */
		filter = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    filter_count_width, 0, &len);
		if (filter == NULL) {
			mbfl_convert_filter_delete(filter);
			return -1;
		}

		/* feed data */
		p = string->val;
		n = string->len;
		while (n > 0) {
			(*filter->filter_function)(*p++, filter);
			n--;
		}

		mbfl_convert_filter_flush(filter);
		mbfl_convert_filter_delete(filter);
	}

	return len;
}


/*
 *  strimwidth
 */
struct collector_strimwidth_data {
	mbfl_convert_filter *decoder;
	mbfl_convert_filter *decoder_backup;
	mbfl_memory_device device;
	int from;
	int width;
	int outwidth;
	int outchar;
	int status;
	int endpos;
};

static int
collector_strimwidth(int c, void* data)
{
	struct collector_strimwidth_data *pc = (struct collector_strimwidth_data*)data;

	switch (pc->status) {
	case 10:
		(*pc->decoder->filter_function)(c, pc->decoder);
		break;
	default:
		if (pc->outchar >= pc->from) {
			if (c >= 0x20) {
				if (c < 0x2000 || (c > 0xff60 && c < 0xffa0)) {
					pc->outwidth++;
				} else {
					pc->outwidth += 2;
				}
			}
			if (pc->outwidth > pc->width) {
				if (pc->status == 0) {
					pc->endpos = pc->device.pos;
					mbfl_convert_filter_copy(pc->decoder, pc->decoder_backup);
				}
				pc->status++;
				(*pc->decoder->filter_function)(c, pc->decoder);
				c = -1;
			} else {
				(*pc->decoder->filter_function)(c, pc->decoder);
			}
		}
		pc->outchar++;
		break;
	}

	return c;
}

mbfl_string *
mbfl_strimwidth(
    mbfl_string *string,
    mbfl_string *marker,
    mbfl_string *result,
    int from,
    int width)
{
	struct collector_strimwidth_data pc;
	mbfl_convert_filter *encoder;
	int n, mkwidth;
	unsigned char *p;

	if (string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;
	mbfl_memory_device_init(&pc.device, width, 0);

	/* output code filter */
	pc.decoder = mbfl_convert_filter_new(
	    mbfl_no_encoding_wchar,
	    string->no_encoding,
	    mbfl_memory_device_output, 0, &pc.device);
	pc.decoder_backup = mbfl_convert_filter_new(
	    mbfl_no_encoding_wchar,
	    string->no_encoding,
	    mbfl_memory_device_output, 0, &pc.device);
	/* wchar filter */
	encoder = mbfl_convert_filter_new(
	    string->no_encoding,
	    mbfl_no_encoding_wchar,
	    collector_strimwidth, 0, &pc);
	if (pc.decoder == NULL || pc.decoder_backup == NULL || encoder == NULL) {
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(pc.decoder);
		mbfl_convert_filter_delete(pc.decoder_backup);
		return NULL;
	}
	mkwidth = 0;
	if (marker) {
		mkwidth = mbfl_strwidth(marker);
	}
	pc.from = from;
	pc.width = width - mkwidth;
	pc.outwidth = 0;
	pc.outchar = 0;
	pc.status = 0;
	pc.endpos = 0;

	/* feed data */
	p = string->val;
	n = string->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
			n--;
		}
		mbfl_convert_filter_flush(encoder);
		if (pc.status != 0 && mkwidth > 0) {
			pc.width += mkwidth;
			while (n > 0) {
				if ((*encoder->filter_function)(*p++, encoder) < 0) {
					break;
				}
				n--;
			}
			mbfl_convert_filter_flush(encoder);
			if (pc.status != 1) {
				pc.status = 10;
				pc.device.pos = pc.endpos;
				mbfl_convert_filter_copy(pc.decoder_backup, pc.decoder);
				mbfl_convert_filter_reset(encoder, marker->no_encoding, mbfl_no_encoding_wchar);
				p = marker->val;
				n = marker->len;
				while (n > 0) {
					if ((*encoder->filter_function)(*p++, encoder) < 0) {
						break;
					}
					n--;
				}
				mbfl_convert_filter_flush(encoder);
			}
		} else if (pc.status != 0) {
			pc.device.pos = pc.endpos;
			mbfl_convert_filter_copy(pc.decoder_backup, pc.decoder);
		}
		mbfl_convert_filter_flush(pc.decoder);
	}
	result = mbfl_memory_device_result(&pc.device, result);
	mbfl_convert_filter_delete(encoder);
	mbfl_convert_filter_delete(pc.decoder);
	mbfl_convert_filter_delete(pc.decoder_backup);

	return result;
}



/*
 *  convert Hankaku and Zenkaku
 */
struct collector_hantozen_data {
	mbfl_convert_filter *next_filter;
	int mode;
	int status;
	int cache;
};

static const unsigned char hankana2zenkata_table[64] = {
	0x00,0x02,0x0C,0x0D,0x01,0xFB,0xF2,0xA1,0xA3,0xA5,
	0xA7,0xA9,0xE3,0xE5,0xE7,0xC3,0xFC,0xA2,0xA4,0xA6,
	0xA8,0xAA,0xAB,0xAD,0xAF,0xB1,0xB3,0xB5,0xB7,0xB9,
	0xBB,0xBD,0xBF,0xC1,0xC4,0xC6,0xC8,0xCA,0xCB,0xCC,
	0xCD,0xCE,0xCF,0xD2,0xD5,0xD8,0xDB,0xDE,0xDF,0xE0,
	0xE1,0xE2,0xE4,0xE6,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,
	0xEF,0xF3,0x9B,0x9C
};
static const unsigned char hankana2zenhira_table[64] = {
	0x00,0x02,0x0C,0x0D,0x01,0xFB,0x92,0x41,0x43,0x45,
	0x47,0x49,0x83,0x85,0x87,0x63,0xFC,0x42,0x44,0x46,
	0x48,0x4A,0x4B,0x4D,0x4F,0x51,0x53,0x55,0x57,0x59,
	0x5B,0x5D,0x5F,0x61,0x64,0x66,0x68,0x6A,0x6B,0x6C,
	0x6D,0x6E,0x6F,0x72,0x75,0x78,0x7B,0x7E,0x7F,0x80,
	0x81,0x82,0x84,0x86,0x88,0x89,0x8A,0x8B,0x8C,0x8D,
	0x8F,0x93,0x9B,0x9C
};
static const unsigned char zenkana2hankana_table[84][2] = {
	{0x67,0x00},{0x71,0x00},{0x68,0x00},{0x72,0x00},{0x69,0x00},
	{0x73,0x00},{0x6A,0x00},{0x74,0x00},{0x6B,0x00},{0x75,0x00},
	{0x76,0x00},{0x76,0x9E},{0x77,0x00},{0x77,0x9E},{0x78,0x00},
	{0x78,0x9E},{0x79,0x00},{0x79,0x9E},{0x7A,0x00},{0x7A,0x9E},
	{0x7B,0x00},{0x7B,0x9E},{0x7C,0x00},{0x7C,0x9E},{0x7D,0x00},
	{0x7D,0x9E},{0x7E,0x00},{0x7E,0x9E},{0x7F,0x00},{0x7F,0x9E},
	{0x80,0x00},{0x80,0x9E},{0x81,0x00},{0x81,0x9E},{0x6F,0x00},
	{0x82,0x00},{0x82,0x9E},{0x83,0x00},{0x83,0x9E},{0x84,0x00},
	{0x84,0x9E},{0x85,0x00},{0x86,0x00},{0x87,0x00},{0x88,0x00},
	{0x89,0x00},{0x8A,0x00},{0x8A,0x9E},{0x8A,0x9F},{0x8B,0x00},
	{0x8B,0x9E},{0x8B,0x9F},{0x8C,0x00},{0x8C,0x9E},{0x8C,0x9F},
	{0x8D,0x00},{0x8D,0x9E},{0x8D,0x9F},{0x8E,0x00},{0x8E,0x9E},
	{0x8E,0x9F},{0x8F,0x00},{0x90,0x00},{0x91,0x00},{0x92,0x00},
	{0x93,0x00},{0x6C,0x00},{0x94,0x00},{0x6D,0x00},{0x95,0x00},
	{0x6E,0x00},{0x96,0x00},{0x97,0x00},{0x98,0x00},{0x99,0x00},
	{0x9A,0x00},{0x9B,0x00},{0x9C,0x00},{0x9C,0x00},{0x72,0x00},
	{0x74,0x00},{0x66,0x00},{0x9D,0x00},{0x73,0x9E}
};

static int
collector_hantozen(int c, void* data)
{
	int s, mode, n;
	struct collector_hantozen_data *pc = (struct collector_hantozen_data*)data;

	s = c;
	mode = pc->mode;

	if (mode & 0xf) { /* hankaku to zenkaku */
		if ((mode & 0x1) && c >= 0x21 && c <= 0x7d && c != 0x22 && c != 0x27 && c != 0x5c) {	/* all except <"> <'> <\> <~> */
			s = c + 0xfee0;
		} else if ((mode & 0x2) && ((c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a))) {	/* alpha */
			s = c + 0xfee0;
		} else if ((mode & 0x4) && c >= 0x30 && c <= 0x39) {	/* num */
			s = c + 0xfee0;
		} else if ((mode & 0x8) && c == 0x20) {	/* spase */
			s = 0x3000;
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

	if (mode & 0x300) {	/* hankaku kana to zenkaku kana */
		if ((mode & 0x100) && (mode & 0x800)) {	/* hankaku kana to zenkaku katakana and glue voiced sound mark */
			if (c >= 0xff61 && c <= 0xff9f) {
				if (pc->status) {
					n = (pc->cache - 0xff60) & 0x3f;
					if (c == 0xff9e && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						pc->status = 0;
						s = 0x3001 + hankana2zenkata_table[n];
					} else if (c == 0xff9e && n == 19) {
						pc->status = 0;
						s = 0x30f4;
					} else if (c == 0xff9f && (n >= 42 && n <= 46)) {
						pc->status = 0;
						s = 0x3002 + hankana2zenkata_table[n];
					} else {
						pc->status = 1;
						pc->cache = c;
						s = 0x3000 + hankana2zenkata_table[n];
					}
				} else {
					pc->status = 1;
					pc->cache = c;
					return c;
				}
			} else {
				if (pc->status) {
					n = (pc->cache - 0xff60) & 0x3f;
					pc->status = 0;
					(*pc->next_filter->filter_function)(0x3000 + hankana2zenkata_table[n], pc->next_filter);
				}
			}
		} else if ((mode & 0x200) && (mode & 0x800)) {	/* hankaku kana to zenkaku hirangana and glue voiced sound mark */
			if (c >= 0xff61 && c <= 0xff9f) {
				if (pc->status) {
					n = (pc->cache - 0xff60) & 0x3f;
					if (c == 0xff9e && ((n >= 22 && n <= 36) || (n >= 42 && n <= 46))) {
						pc->status = 0;
						s = 0x3001 + hankana2zenhira_table[n];
					} else if (c == 0xff9f && (n >= 42 && n <= 46)) {
						pc->status = 0;
						s = 0x3002 + hankana2zenhira_table[n];
					} else {
						pc->status = 1;
						pc->cache = c;
						s = 0x3000 + hankana2zenhira_table[n];
					}
				} else {
					pc->status = 1;
					pc->cache = c;
					return c;
				}
			} else {
				if (pc->status) {
					n = (pc->cache - 0xff60) & 0x3f;
					pc->status = 0;
					(*pc->next_filter->filter_function)(0x3000 + hankana2zenhira_table[n], pc->next_filter);
				}
			}
		} else if ((mode & 0x100) && c >= 0xff61 && c <= 0xff9f) {	/* hankaku kana to zenkaku katakana */
			s = 0x3000 + hankana2zenkata_table[c - 0xff60];
		} else if ((mode & 0x200) && c >= 0xff61 && c <= 0xff9f) {	/* hankaku kana to zenkaku hirangana */
			s = 0x3000 + hankana2zenhira_table[c - 0xff60];
		}
	}

	if (mode & 0x3000) {	/* Zenkaku kana to hankaku kana */
		if ((mode & 0x1000) && c >= 0x30a1 && c <= 0x30f4) {	/* Zenkaku katakana to hankaku kana */
			n = c - 0x30a1;
			if (zenkana2hankana_table[n][1] != 0) {
				(*pc->next_filter->filter_function)(0xff00 + zenkana2hankana_table[n][0], pc->next_filter);
				s = 0xff00 + zenkana2hankana_table[n][1];
			} else {
				s = 0xff00 + zenkana2hankana_table[n][0];
			}
		} else if ((mode & 0x2000) && c >= 0x3041 && c <= 0x3093) {	/* Zenkaku hirangana to hankaku kana */
			n = c - 0x3041;
			if (zenkana2hankana_table[n][1] != 0) {
				(*pc->next_filter->filter_function)(0xff00 + zenkana2hankana_table[n][0], pc->next_filter);
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
	} else if (mode & 0x30000) { 
		if ((mode & 0x10000) && c >= 0x3041 && c <= 0x3093) {	/* Zenkaku hirangana to Zenkaku katakana */
			s = c + 0x60;
		} else if ((mode & 0x20000) && c >= 0x30a1 && c <= 0x30f3) {	/* Zenkaku katakana to Zenkaku hirangana */
			s = c - 0x60;
		}
	}

	if (mode & 0x100000) {	/* special ascii to symbol */
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
	} else if (mode & 0x200000) {	/* special symbol to ascii */
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

	if (mode & 0x400000) {	/* special ascii to symbol */
		if (c == 0x5c) {
			s = 0xff3c;				/* FULLWIDTH REVERSE SOLIDUS */
		} else if (c == 0x7e) {
			s = 0xff5e;				/* FULLWIDTH TILDE */
		} else if (c == 0x27) {
			s = 0xff07;				/* FULLWIDTH APOSTROPHE */
		} else if (c == 0x22) {
			s = 0xff02;				/* FULLWIDTH QUOTATION MARK */
		}
	} else if (mode & 0x800000) {	/* special symbol to ascii */
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

	return (*pc->next_filter->filter_function)(s, pc->next_filter);
}

static int
collector_hantozen_flush(struct collector_hantozen_data *pc)
{
	int ret, n;

	ret = 0;
	if (pc->status) {
		n = (pc->cache - 0xff60) & 0x3f;
		if (pc->mode & 0x100) {	/* hankaku kana to zenkaku katakana */
			ret = (*pc->next_filter->filter_function)(0x3000 + hankana2zenkata_table[n], pc->next_filter);
		} else if (pc->mode & 0x200) {	/* hankaku kana to zenkaku hirangana */
			ret = (*pc->next_filter->filter_function)(0x3000 + hankana2zenhira_table[n], pc->next_filter);
		}
		pc->status = 0;
	}

	return ret;
}

mbfl_string *
mbfl_ja_jp_hantozen(
    mbfl_string *string,
    mbfl_string *result,
    int mode)
{
	int n;
	unsigned char *p;
	mbfl_encoding *encoding;
	mbfl_memory_device device;
	struct collector_hantozen_data pc;
	mbfl_convert_filter *decoder;
	mbfl_convert_filter *encoder;

	/* initialize */
	if (string == NULL || result == NULL) {
		return NULL;
	}
	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL) {
		return NULL;
	}
	mbfl_memory_device_init(&device, string->len, 0);
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;
	decoder = mbfl_convert_filter_new(
	  mbfl_no_encoding_wchar,
	  string->no_encoding,
	  mbfl_memory_device_output, 0, &device);
	encoder = mbfl_convert_filter_new(
	  string->no_encoding,
	  mbfl_no_encoding_wchar,
	  collector_hantozen, 0, &pc);
	if (decoder == NULL || encoder == NULL) {
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(decoder);
		return NULL;
	}
	pc.next_filter = decoder;
	pc.mode = mode;
	pc.status = 0;
	pc.cache = 0;

	/* feed data */
	p = string->val;
	n = string->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
			n--;
		}
	}

	mbfl_convert_filter_flush(encoder);
	collector_hantozen_flush(&pc);
	mbfl_convert_filter_flush(decoder);
	result = mbfl_memory_device_result(&device, result);
	mbfl_convert_filter_delete(encoder);
	mbfl_convert_filter_delete(decoder);

	return result;
}


/*
 *  MIME header encode
 */
struct mime_header_encoder_data {
	mbfl_convert_filter *conv1_filter;
	mbfl_convert_filter *block_filter;
	mbfl_convert_filter *conv2_filter;
	mbfl_convert_filter *conv2_filter_backup;
	mbfl_convert_filter *encod_filter;
	mbfl_convert_filter *encod_filter_backup;
	mbfl_memory_device outdev;
	mbfl_memory_device tmpdev;
	int status1;
	int status2;
	int prevpos;
	int linehead;
	int firstindent;
	int encnamelen;
	int lwsplen;
	char encname[128];
	char lwsp[16];
};

static int
mime_header_encoder_block_collector(int c, void *data)
{
	int n;
	struct mime_header_encoder_data *pe = (struct mime_header_encoder_data *)data;

	switch (pe->status2) {
	case 1:	/* encoded word */
		pe->prevpos = pe->outdev.pos;
		mbfl_convert_filter_copy(pe->conv2_filter, pe->conv2_filter_backup);
		mbfl_convert_filter_copy(pe->encod_filter, pe->encod_filter_backup);
		(*pe->conv2_filter->filter_function)(c, pe->conv2_filter);
		(*pe->conv2_filter->filter_flush)(pe->conv2_filter);
		(*pe->encod_filter->filter_flush)(pe->encod_filter);
		n = pe->outdev.pos - pe->linehead + pe->firstindent;
		pe->outdev.pos = pe->prevpos;
		mbfl_convert_filter_copy(pe->conv2_filter_backup, pe->conv2_filter);
		mbfl_convert_filter_copy(pe->encod_filter_backup, pe->encod_filter);
		if (n >= 74) {
			(*pe->conv2_filter->filter_flush)(pe->conv2_filter);
			(*pe->encod_filter->filter_flush)(pe->encod_filter);
			mbfl_memory_device_strncat(&pe->outdev, "\x3f\x3d", 2);			/* ?= */
			mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);
			pe->linehead = pe->outdev.pos;
			pe->firstindent = 0;
			mbfl_memory_device_strncat(&pe->outdev, pe->encname, pe->encnamelen);
			c = (*pe->conv2_filter->filter_function)(c, pe->conv2_filter);
		} else {
			c = (*pe->conv2_filter->filter_function)(c, pe->conv2_filter);
		}
		break;

	default:
		mbfl_memory_device_strncat(&pe->outdev, pe->encname, pe->encnamelen);
		c = (*pe->conv2_filter->filter_function)(c, pe->conv2_filter);
		pe->status2 = 1;
		break;
	}

	return c;
}

static int
mime_header_encoder_collector(int c, void *data)
{
	int n;
	struct mime_header_encoder_data *pe = (struct mime_header_encoder_data *)data;

	switch (pe->status1) {
	case 11:	/* encoded word */
		(*pe->block_filter->filter_function)(c, pe->block_filter);
		break;

	default:	/* ASCII */
		if (c >= 0x21 && c < 0x7f) {	/* ASCII exclude SPACE and CTLs */
			mbfl_memory_device_output(c, &pe->tmpdev);
			pe->status1 = 1;
		} else if (pe->status1 == 0 && c == 0x20) {	/* repeat SPACE */
			mbfl_memory_device_output(c, &pe->tmpdev);
		} else {
			if (pe->tmpdev.pos < 74 && c == 0x20) {
				n = pe->outdev.pos - pe->linehead + pe->tmpdev.pos + pe->firstindent;
				if (n > 74) {
					mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);		/* LWSP */
					pe->linehead = pe->outdev.pos;
					pe->firstindent = 0;
				} else if (pe->outdev.pos > 0) {
					mbfl_memory_device_output(0x20, &pe->outdev);
				}
				mbfl_memory_device_devcat(&pe->outdev, &pe->tmpdev);
				mbfl_memory_device_reset(&pe->tmpdev);
				pe->status1 = 0;
			} else {
				n = pe->outdev.pos - pe->linehead + pe->encnamelen + pe->firstindent;
				if (n > 60)  {
					mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);		/* LWSP */
					pe->linehead = pe->outdev.pos;
					pe->firstindent = 0;
				} else if (pe->outdev.pos > 0)  {
					mbfl_memory_device_output(0x20, &pe->outdev);
				}
				mbfl_convert_filter_devcat(pe->block_filter, &pe->tmpdev);
				mbfl_memory_device_reset(&pe->tmpdev);
				(*pe->block_filter->filter_function)(c, pe->block_filter);
				pe->status1 = 11;
			}
		}
		break;
	}

	return c;
}

mbfl_string *
mime_header_encoder_result(struct mime_header_encoder_data *pe, mbfl_string *result)
{
	if (pe->status1 >= 10) {
		(*pe->conv2_filter->filter_flush)(pe->conv2_filter);
		(*pe->encod_filter->filter_flush)(pe->encod_filter);
		mbfl_memory_device_strncat(&pe->outdev, "\x3f\x3d", 2);		/* ?= */
	} else if (pe->tmpdev.pos > 0) {
		if (pe->outdev.pos > 0) {
			if ((pe->outdev.pos - pe->linehead + pe->tmpdev.pos) > 74) {
				mbfl_memory_device_strncat(&pe->outdev, pe->lwsp, pe->lwsplen);
			} else {
				mbfl_memory_device_output(0x20, &pe->outdev);
			}
		}
		mbfl_memory_device_devcat(&pe->outdev, &pe->tmpdev);
	}
	mbfl_memory_device_reset(&pe->tmpdev);
	pe->prevpos = 0;
	pe->linehead = 0;
	pe->status1 = 0;
	pe->status2 = 0;

	return mbfl_memory_device_result(&pe->outdev, result);
}

struct mime_header_encoder_data*
mime_header_encoder_new(
    enum mbfl_no_encoding incode,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding transenc)
{
	int n;
	const char *s;
	mbfl_encoding *outencoding;
	struct mime_header_encoder_data *pe;

	/* get output encoding and check MIME charset name */
	outencoding = mbfl_no2encoding(outcode);
	if (outencoding == NULL || outencoding->mime_name == NULL || outencoding->mime_name[0] == '\0') {
		return NULL;
	}

	pe = (struct mime_header_encoder_data*)mbfl_malloc(sizeof(struct mime_header_encoder_data));
	if (pe == NULL) {
		return NULL;
	}

	mbfl_memory_device_init(&pe->outdev, 0, 0);
	mbfl_memory_device_init(&pe->tmpdev, 0, 0);
	pe->prevpos = 0;
	pe->linehead = 0;
	pe->firstindent = 0;
	pe->status1 = 0;
	pe->status2 = 0;

	/* make the encoding description string  exp. "=?ISO-2022-JP?B?" */
	n = 0;
	pe->encname[n++] = 0x3d;
	pe->encname[n++] = 0x3f;
	s = outencoding->mime_name;
	while (*s) {
		pe->encname[n++] = *s++;
	}
	pe->encname[n++] = 0x3f;
	if (transenc == mbfl_no_encoding_qprint) {
		pe->encname[n++] = 0x51;
	} else {
		pe->encname[n++] = 0x42;
		transenc = mbfl_no_encoding_base64;
	}
	pe->encname[n++] = 0x3f;
	pe->encname[n] = '\0';
	pe->encnamelen = n;

	n = 0;
	pe->lwsp[n++] = 0x0d;
	pe->lwsp[n++] = 0x0a;
	pe->lwsp[n++] = 0x20;
	pe->lwsp[n] = '\0';
	pe->lwsplen = n;

	/* transfer encode filter */
	pe->encod_filter = mbfl_convert_filter_new(outcode, transenc, mbfl_memory_device_output, 0, &(pe->outdev));
	pe->encod_filter_backup = mbfl_convert_filter_new(outcode, transenc, mbfl_memory_device_output, 0, &(pe->outdev));

	/* Output code filter */
	pe->conv2_filter = mbfl_convert_filter_new(mbfl_no_encoding_wchar, outcode, mbfl_filter_output_pipe, 0, pe->encod_filter);
	pe->conv2_filter_backup = mbfl_convert_filter_new(mbfl_no_encoding_wchar, outcode, mbfl_filter_output_pipe, 0, pe->encod_filter);

	/* encoded block filter */
	pe->block_filter = mbfl_convert_filter_new(mbfl_no_encoding_wchar, mbfl_no_encoding_wchar, mime_header_encoder_block_collector, 0, pe);

	/* Input code filter */
	pe->conv1_filter = mbfl_convert_filter_new(incode, mbfl_no_encoding_wchar, mime_header_encoder_collector, 0, pe);

	if (pe->encod_filter == NULL ||
	    pe->encod_filter_backup == NULL ||
	    pe->conv2_filter == NULL ||
	    pe->conv2_filter_backup == NULL ||
	    pe->conv1_filter == NULL) {
		mime_header_encoder_delete(pe);
		return NULL;
	}

	if (transenc == mbfl_no_encoding_qprint) {
		pe->encod_filter->status |= MBFL_QPRINT_STS_MIME_HEADER;
		pe->encod_filter_backup->status |= MBFL_QPRINT_STS_MIME_HEADER;
	} else {
		pe->encod_filter->status |= MBFL_BASE64_STS_MIME_HEADER;
		pe->encod_filter_backup->status |= MBFL_BASE64_STS_MIME_HEADER;
	}

	return pe;
}

void
mime_header_encoder_delete(struct mime_header_encoder_data *pe)
{
	if (pe) {
		mbfl_convert_filter_delete(pe->conv1_filter);
		mbfl_convert_filter_delete(pe->block_filter);
		mbfl_convert_filter_delete(pe->conv2_filter);
		mbfl_convert_filter_delete(pe->conv2_filter_backup);
		mbfl_convert_filter_delete(pe->encod_filter);
		mbfl_convert_filter_delete(pe->encod_filter_backup);
		mbfl_memory_device_clear(&pe->outdev);
		mbfl_memory_device_clear(&pe->tmpdev);
		mbfl_free((void*)pe);
	}
}

int
mime_header_encoder_feed(int c, struct mime_header_encoder_data *pe)
{
	return (*pe->conv1_filter->filter_function)(c, pe->conv1_filter);
}

mbfl_string *
mbfl_mime_header_encode(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding encoding,
    const char *linefeed,
    int indent)
{
	int n;
	unsigned char *p;
	struct mime_header_encoder_data *pe;

	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = mbfl_no_encoding_ascii;

	pe = mime_header_encoder_new(string->no_encoding, outcode, encoding);
	if (pe == NULL) {
		return NULL;
	}

	if (linefeed != NULL) {
		n = 0;
		while (*linefeed && n < 8) {
			pe->lwsp[n++] = *linefeed++;
		}
		pe->lwsp[n++] = 0x20;
		pe->lwsp[n] = '\0';
		pe->lwsplen = n;
	}
	if (indent > 0 && indent < 74) {
		pe->firstindent = indent;
	}

	n = string->len;
	p = string->val;
	while (n > 0) {
		(*pe->conv1_filter->filter_function)(*p++, pe->conv1_filter);
		n--;
	}

	result = mime_header_encoder_result(pe, result);
	mime_header_encoder_delete(pe);

	return result;
}


/*
 *  MIME header decode
 */
struct mime_header_decoder_data {
	mbfl_convert_filter *deco_filter;
	mbfl_convert_filter *conv1_filter;
	mbfl_convert_filter *conv2_filter;
	mbfl_memory_device outdev;
	mbfl_memory_device tmpdev;
	int cspos;
	int status;
	enum mbfl_no_encoding encoding;
	enum mbfl_no_encoding incode;
	enum mbfl_no_encoding outcode;
};

static int
mime_header_decoder_collector(int c, void* data)
{
	mbfl_encoding *encoding;
	struct mime_header_decoder_data *pd = (struct mime_header_decoder_data*)data;

	switch (pd->status) {
	case 1:
		if (c == 0x3f) {		/* ? */
			mbfl_memory_device_output(c, &pd->tmpdev);
			pd->cspos = pd->tmpdev.pos;
			pd->status = 2;
		} else {
			mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
			mbfl_memory_device_reset(&pd->tmpdev);
			if (c == 0x3d) {		/* = */
				mbfl_memory_device_output(c, &pd->tmpdev);
			} else if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				pd->status = 9;
			} else {
				(*pd->conv1_filter->filter_function)(c, pd->conv1_filter);
				pd->status = 0;
			}
		}
		break;
	case 2:		/* store charset string */
		if (c == 0x3f) {		/* ? */
			/* identify charset */
			mbfl_memory_device_output('\0', &pd->tmpdev);
			encoding = mbfl_name2encoding(&pd->tmpdev.buffer[pd->cspos]);
			if (encoding != NULL) {
				pd->incode = encoding->no_encoding;
				pd->status = 3;
			}
			mbfl_memory_device_unput(&pd->tmpdev);
			mbfl_memory_device_output(c, &pd->tmpdev);
		} else {
			mbfl_memory_device_output(c, &pd->tmpdev);
			if (pd->tmpdev.pos > 100) {		/* too long charset string */
				pd->status = 0;
			} else if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				mbfl_memory_device_unput(&pd->tmpdev);
				pd->status = 9;
			}
			if (pd->status != 2) {
				mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
				mbfl_memory_device_reset(&pd->tmpdev);
			}
		}
		break;
	case 3:		/* identify encoding */
		mbfl_memory_device_output(c, &pd->tmpdev);
		if (c == 0x42 || c == 0x62) {		/* 'B' or 'b' */
			pd->encoding = mbfl_no_encoding_base64;
			pd->status = 4;
		} else if (c == 0x51 || c == 0x71) {	/* 'Q' or 'q' */
			pd->encoding = mbfl_no_encoding_qprint;
			pd->status = 4;
		} else {
			if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				mbfl_memory_device_unput(&pd->tmpdev);
				pd->status = 9;
			} else {
				pd->status = 0;
			}
			mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
			mbfl_memory_device_reset(&pd->tmpdev);
		}
		break;
	case 4:		/* reset filter */
		mbfl_memory_device_output(c, &pd->tmpdev);
		if (c == 0x3f) {		/* ? */
			/* charset convert filter */
			mbfl_convert_filter_reset(pd->conv1_filter, pd->incode, mbfl_no_encoding_wchar);
			/* decode filter */
			mbfl_convert_filter_reset(pd->deco_filter, pd->encoding, mbfl_no_encoding_8bit);
			pd->status = 5;
		} else {
			if (c == 0x0d || c == 0x0a) {	/* CR or LF */
				mbfl_memory_device_unput(&pd->tmpdev);
				pd->status = 9;
			} else {
				pd->status = 0;
			}
			mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
		}
		mbfl_memory_device_reset(&pd->tmpdev);
		break;
	case 5:		/* encoded block */
		if (c == 0x3f) {		/* ? */
			pd->status = 6;
		} else {
			(*pd->deco_filter->filter_function)(c, pd->deco_filter);
		}
		break;
	case 6:		/* check end position */
		if (c == 0x3d) {		/* = */
			/* flush and reset filter */
			(*pd->deco_filter->filter_flush)(pd->deco_filter);
			(*pd->conv1_filter->filter_flush)(pd->conv1_filter);
			mbfl_convert_filter_reset(pd->conv1_filter, mbfl_no_encoding_ascii, mbfl_no_encoding_wchar);
			pd->status = 7;
		} else {
			(*pd->deco_filter->filter_function)(0x3f, pd->deco_filter);
			if (c != 0x3f) {		/* ? */
				(*pd->deco_filter->filter_function)(c, pd->deco_filter);
				pd->status = 5;
			}
		}
		break;
	case 7:		/* after encoded block */
		if (c == 0x0d || c == 0x0a) {	/* CR LF */
			pd->status = 8;
		} else {
			mbfl_memory_device_output(c, &pd->tmpdev);
			if (c == 0x3d) {		/* = */
				pd->status = 1;
			} else if (c != 0x20 && c != 0x09) {		/* not space */
				mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
				mbfl_memory_device_reset(&pd->tmpdev);
				pd->status = 0;
			}
		}
		break;
	case 8:		/* folding */
	case 9:		/* folding */
		if (c != 0x0d && c != 0x0a && c != 0x20 && c != 0x09) {
			if (c == 0x3d) {		/* = */
				if (pd->status == 8) {
					mbfl_memory_device_output(0x20, &pd->tmpdev);	/* SPACE */
				} else {
					(*pd->conv1_filter->filter_function)(0x20, pd->conv1_filter);
				}
				mbfl_memory_device_output(c, &pd->tmpdev);
				pd->status = 1;
			} else {
				mbfl_memory_device_output(0x20, &pd->tmpdev);
				mbfl_memory_device_output(c, &pd->tmpdev);
				mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
				mbfl_memory_device_reset(&pd->tmpdev);
				pd->status = 0;
			}
		}
		break;
	default:		/* non encoded block */
		if (c == 0x0d || c == 0x0a) {	/* CR LF */
			pd->status = 9;
		} else if (c == 0x3d) {		/* = */
			mbfl_memory_device_output(c, &pd->tmpdev);
			pd->status = 1;
		} else {
			(*pd->conv1_filter->filter_function)(c, pd->conv1_filter);
		}
		break;
	}

	return c;
}

mbfl_string *
mime_header_decoder_result(struct mime_header_decoder_data *pd, mbfl_string *result)
{
	switch (pd->status) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 7:
	case 8:
	case 9:
		mbfl_convert_filter_devcat(pd->conv1_filter, &pd->tmpdev);
		break;
	case 5:
	case 6:
		(*pd->deco_filter->filter_flush)(pd->deco_filter);
		(*pd->conv1_filter->filter_flush)(pd->conv1_filter);
		break;
	}
	(*pd->conv2_filter->filter_flush)(pd->conv2_filter);
	mbfl_memory_device_reset(&pd->tmpdev);
	pd->status = 0;

	return mbfl_memory_device_result(&pd->outdev, result);
}

struct mime_header_decoder_data*
mime_header_decoder_new(enum mbfl_no_encoding outcode)
{
	struct mime_header_decoder_data *pd;

	pd = (struct mime_header_decoder_data*)mbfl_malloc(sizeof(struct mime_header_decoder_data));
	if (pd == NULL) {
		return NULL;
	}

	mbfl_memory_device_init(&pd->outdev, 0, 0);
	mbfl_memory_device_init(&pd->tmpdev, 0, 0);
	pd->cspos = 0;
	pd->status = 0;
	pd->encoding = mbfl_no_encoding_pass;
	pd->incode = mbfl_no_encoding_ascii;
	pd->outcode = outcode;
	/* charset convert filter */
	pd->conv2_filter = mbfl_convert_filter_new(mbfl_no_encoding_wchar, pd->outcode, mbfl_memory_device_output, 0, &pd->outdev);
	pd->conv1_filter = mbfl_convert_filter_new(pd->incode, mbfl_no_encoding_wchar, mbfl_filter_output_pipe, 0, pd->conv2_filter);
	/* decode filter */
	pd->deco_filter = mbfl_convert_filter_new(pd->encoding, mbfl_no_encoding_8bit, mbfl_filter_output_pipe, 0, pd->conv1_filter);

	if (pd->conv1_filter == NULL || pd->conv2_filter == NULL || pd->deco_filter == NULL) {
		mime_header_decoder_delete(pd);
		return NULL;
	}

	return pd;
}

void
mime_header_decoder_delete(struct mime_header_decoder_data *pd)
{
	if (pd) {
		mbfl_convert_filter_delete(pd->conv2_filter);
		mbfl_convert_filter_delete(pd->conv1_filter);
		mbfl_convert_filter_delete(pd->deco_filter);
		mbfl_memory_device_clear(&pd->outdev);
		mbfl_memory_device_clear(&pd->tmpdev);
		mbfl_free((void*)pd);
	}
}

int
mime_header_decoder_feed(int c, struct mime_header_decoder_data *pd)
{
	return mime_header_decoder_collector(c, pd);
}

mbfl_string *
mbfl_mime_header_decode(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding outcode)
{
	int n;
	unsigned char *p;
	struct mime_header_decoder_data *pd;

	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = outcode;

	pd = mime_header_decoder_new(outcode);
	if (pd == NULL) {
		return NULL;
	}

	/* feed data */
	n = string->len;
	p = string->val;
	while (n > 0) {
		mime_header_decoder_collector(*p++, pd);
		n--;
	}

	result = mime_header_decoder_result(pd, result);
	mime_header_decoder_delete(pd);

	return result;
}



/*
 *  convert HTML numeric entity
 */
struct collector_htmlnumericentity_data {
	mbfl_convert_filter *decoder;
	int status;
	int cache;
	int digit;
	int *convmap;
	int mapsize;
};

static int
collector_encode_htmlnumericentity(int c, void *data)
{
	struct collector_htmlnumericentity_data *pc = (struct collector_htmlnumericentity_data *)data;
	int f, n, s, r, d, size, *mapelm;

	size = pc->mapsize;
	f = 0;
	n = 0;
	while (n < size) {
		mapelm = &(pc->convmap[n*4]);
		if (c >= mapelm[0] && c <= mapelm[1]) {
			s = (c + mapelm[2]) & mapelm[3];
			if (s >= 0) {
				(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
				(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
				r = 100000000;
				s %= r;
				while (r > 0) {
					d = s/r;
					if (d || f) {
						f = 1;
						s %= r;
						(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
					}
					r /= 10;
				}
				if (!f) {
					f = 1;
					(*pc->decoder->filter_function)(mbfl_hexchar_table[0], pc->decoder);
				}
				(*pc->decoder->filter_function)(0x3b, pc->decoder);		/* ';' */
			}
		}
		if (f) {
			break;
		}
		n++;
	}
	if (!f) {
		(*pc->decoder->filter_function)(c, pc->decoder);
	}

	return c;
}

static int
collector_decode_htmlnumericentity(int c, void *data)
{
	struct collector_htmlnumericentity_data *pc = (struct collector_htmlnumericentity_data *)data;
	int f, n, s, r, d, size, *mapelm;

	switch (pc->status) {
	case 1:
		if (c == 0x23) {	/* '#' */
			pc->status = 2;
		} else {
			pc->status = 0;
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 2:
		if (c >= 0x30 && c <= 0x39) {	/* '0' - '9' */
			pc->cache = c - 0x30;
			pc->status = 3;
			pc->digit = 1;
		} else {
			pc->status = 0;
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 3:
		s = 0;
		f = 0;
		if (c >= 0x30 && c <= 0x39) {	/* '0' - '9' */
			if (pc->digit > 9) {
				pc->status = 0;
				s = pc->cache;
				f = 1;
			} else {
				s = pc->cache*10 + c - 0x30;
				pc->cache = s;
				pc->digit++;
			}
		} else {
			pc->status = 0;
			s = pc->cache;
			f = 1;
			n = 0;
			size = pc->mapsize;
			while (n < size) {
				mapelm = &(pc->convmap[n*4]);
				d = s - mapelm[2];
				if (d >= mapelm[0] && d <= mapelm[1]) {
					f = 0;
					(*pc->decoder->filter_function)(d, pc->decoder);
					if (c != 0x3b) {	/* ';' */
						(*pc->decoder->filter_function)(c, pc->decoder);
					}
					break;
				}
				n++;
			}
		}
		if (f) {
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			r = 1;
			n = pc->digit;
			while (n > 0) {
				r *= 10;
				n--;
			}
			s %= r;
			r /= 10;
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 10;
				(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
			}
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	default:
		if (c == 0x26) {	/* '&' */
			pc->status = 1;
		} else {
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	}

	return c;
}

mbfl_string *
mbfl_html_numeric_entity(
    mbfl_string *string,
    mbfl_string *result,
    int *convmap,
    int mapsize,
    int type)
{
	struct collector_htmlnumericentity_data pc;
	mbfl_memory_device device;
	mbfl_convert_filter *encoder;
	int n;
	unsigned char *p;

	if (string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;
	mbfl_memory_device_init(&device, string->len, 0);

	/* output code filter */
	pc.decoder = mbfl_convert_filter_new(
	    mbfl_no_encoding_wchar,
	    string->no_encoding,
	    mbfl_memory_device_output, 0, &device);
	/* wchar filter */
	if (type == 0) {
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_encode_htmlnumericentity, 0, &pc);
	} else {
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_decode_htmlnumericentity, 0, &pc);
	}
	if (pc.decoder == NULL || encoder == NULL) {
		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(pc.decoder);
		return NULL;
	}
	pc.status = 0;
	pc.cache = 0;
	pc.digit = 0;
	pc.convmap = convmap;
	pc.mapsize = mapsize;

	/* feed data */
	p = string->val;
	n = string->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(encoder);
	mbfl_convert_filter_flush(pc.decoder);
	result = mbfl_memory_device_result(&device, result);
	mbfl_convert_filter_delete(encoder);
	mbfl_convert_filter_delete(pc.decoder);

	return result;
}





/*
 * Unicode table
 */


/* Windows CodePage 1252 - it's the same as iso-8859-1 but
 * defines extra symbols in the range 0x80-0x9f.
 * This table differs from the rest of the unicode tables below
 * as it only covers this range, while the rest cover 0xa0 onwards */
static const unsigned short cp1252_ucs_table[] = {
 0xfffe,0xfffe,0x201a,0x0192,0x201e,0x2026,0x2020,0x2021,
 0x02c6,0x2030,0x0160,0x2039,0x0152,0xfffe,0xfffe,0xfffe,
 0xfffe,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x02dc,
 0x2122,0x0161,0x203a,0x0153,0xfffe,0xfffe,0x0178
};


static const unsigned short iso8859_2_ucs_table[] = {
 0x00A0,0x0104,0x02D8,0x0141,0x00A4,0x013D,0x015A,0x00A7,
 0x00A8,0x0160,0x015E,0x0164,0x0179,0x00AD,0x017D,0x017B,
 0x00B0,0x0105,0x02DB,0x0142,0x00B4,0x013E,0x015B,0x02C7,
 0x00B8,0x0161,0x015F,0x0165,0x017A,0x02DD,0x017E,0x017C,
 0x0154,0x00C1,0x00C2,0x0102,0x00C4,0x0139,0x0106,0x00C7,
 0x010C,0x00C9,0x0118,0x00CB,0x011A,0x00CD,0x00CE,0x010E,
 0x0110,0x0143,0x0147,0x00D3,0x00D4,0x0150,0x00D6,0x00D7,
 0x0158,0x016E,0x00DA,0x0170,0x00DC,0x00DD,0x0162,0x00DF,
 0x0155,0x00E1,0x00E2,0x0103,0x00E4,0x013A,0x0107,0x00E7,
 0x010D,0x00E9,0x0119,0x00EB,0x011B,0x00ED,0x00EE,0x010F,
 0x0111,0x0144,0x0148,0x00F3,0x00F4,0x0151,0x00F6,0x00F7,
 0x0159,0x016F,0x00FA,0x0171,0x00FC,0x00FD,0x0163,0x02D9
};


static const unsigned short iso8859_3_ucs_table[] = {
 0x00A0,0x0126,0x02D8,0x00A3,0x00A4,0x0000,0x0124,0x00A7,
 0x00A8,0x0130,0x015E,0x011E,0x0134,0x00AD,0x0000,0x017B,
 0x00B0,0x0127,0x00B2,0x00B3,0x00B4,0x00B5,0x0125,0x00B7,
 0x00B8,0x0131,0x015F,0x011F,0x0135,0x00BD,0x0000,0x017C,
 0x00C0,0x00C1,0x00C2,0x0000,0x00C4,0x010A,0x0108,0x00C7,
 0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
 0x0000,0x00D1,0x00D2,0x00D3,0x00D4,0x0120,0x00D6,0x00D7,
 0x011C,0x00D9,0x00DA,0x00DB,0x00DC,0x016C,0x015C,0x00DF,
 0x00E0,0x00E1,0x00E2,0x0000,0x00E4,0x010B,0x0109,0x00E7,
 0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
 0x0000,0x00F1,0x00F2,0x00F3,0x00F4,0x0121,0x00F6,0x00F7,
 0x011D,0x00F9,0x00FA,0x00FB,0x00FC,0x016D,0x015D,0x02D9
};


static const unsigned short iso8859_4_ucs_table[] = {
 0x00A0,0x0104,0x0138,0x0156,0x00A4,0x0128,0x013B,0x00A7,
 0x00A8,0x0160,0x0112,0x0122,0x0166,0x00AD,0x017D,0x00AF,
 0x00B0,0x0105,0x02DB,0x0157,0x00B4,0x0129,0x013C,0x02C7,
 0x00B8,0x0161,0x0113,0x0123,0x0167,0x014A,0x017E,0x014B,
 0x0100,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x012E,
 0x010C,0x00C9,0x0118,0x00CB,0x0116,0x00CD,0x00CE,0x012A,
 0x0110,0x0145,0x014C,0x0136,0x00D4,0x00D5,0x00D6,0x00D7,
 0x00D8,0x0172,0x00DA,0x00DB,0x00DC,0x0168,0x016A,0x00DF,
 0x0101,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x012F,
 0x010D,0x00E9,0x0119,0x00EB,0x0117,0x00ED,0x00EE,0x012B,
 0x0111,0x0146,0x014D,0x0137,0x00F4,0x00F5,0x00F6,0x00F7,
 0x00F8,0x0173,0x00FA,0x00FB,0x00FC,0x0169,0x016B,0x02D9
};


static const unsigned short iso8859_5_ucs_table[] = {
 0x00A0,0x0401,0x0402,0x0403,0x0404,0x0405,0x0406,0x0407,
 0x0408,0x0409,0x040A,0x040B,0x040C,0x00AD,0x040E,0x040F,
 0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
 0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
 0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
 0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
 0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
 0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
 0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
 0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
 0x2116,0x0451,0x0452,0x0453,0x0454,0x0455,0x0456,0x0457,
 0x0458,0x0459,0x045A,0x045B,0x045C,0x00A7,0x045E,0x045F
};


static const unsigned short iso8859_6_ucs_table[] = {
 0x00A0,0x0000,0x0000,0x0000,0x00A4,0x0000,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x060C,0x00AD,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x061B,0x0000,0x0000,0x0000,0x061F,
 0x0000,0x0621,0x0622,0x0623,0x0624,0x0625,0x0626,0x0627,
 0x0628,0x0629,0x062A,0x062B,0x062C,0x062D,0x062E,0x062F,
 0x0630,0x0631,0x0632,0x0633,0x0634,0x0635,0x0636,0x0637,
 0x0638,0x0639,0x063A,0x0000,0x0000,0x0000,0x0000,0x0000,
 0x0640,0x0641,0x0642,0x0643,0x0644,0x0645,0x0646,0x0647,
 0x0648,0x0649,0x064A,0x064B,0x064C,0x064D,0x064E,0x064F,
 0x0650,0x0651,0x0652,0x0000,0x0000,0x0000,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x060C,0x00AD,0x0000,0x0000
};


static const unsigned short iso8859_7_ucs_table[] = {
 0x00A0,0x2018,0x2019,0x00A3,0x0000,0x0000,0x00A6,0x00A7,
 0x00A8,0x00A9,0x0000,0x00AB,0x00AC,0x00AD,0x0000,0x2015,
 0x00B0,0x00B1,0x00B2,0x00B3,0x0384,0x0385,0x0386,0x00B7,
 0x0388,0x0389,0x038A,0x00BB,0x038C,0x00BD,0x038E,0x038F,
 0x0390,0x0391,0x0392,0x0393,0x0394,0x0395,0x0396,0x0397,
 0x0398,0x0399,0x039A,0x039B,0x039C,0x039D,0x039E,0x039F,
 0x03A0,0x03A1,0x0000,0x03A3,0x03A4,0x03A5,0x03A6,0x03A7,
 0x03A8,0x03A9,0x03AA,0x03AB,0x03AC,0x03AD,0x03AE,0x03AF,
 0x03B0,0x03B1,0x03B2,0x03B3,0x03B4,0x03B5,0x03B6,0x03B7,
 0x03B8,0x03B9,0x03BA,0x03BB,0x03BC,0x03BD,0x03BE,0x03BF,
 0x03C0,0x03C1,0x03C2,0x03C3,0x03C4,0x03C5,0x03C6,0x03C7,
 0x03C8,0x03C9,0x03CA,0x03CB,0x03CC,0x03CD,0x03CE,0x0000
};


static const unsigned short iso8859_8_ucs_table[] = {
 0x00A0,0x0000,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,
 0x00A8,0x00A9,0x00D7,0x00AB,0x00AC,0x00AD,0x00AE,0x203E,
 0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
 0x00B8,0x00B9,0x00F7,0x00BB,0x00BC,0x00BD,0x00BE,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x2017,
 0x05D0,0x05D1,0x05D2,0x05D3,0x05D4,0x05D5,0x05D6,0x05D7,
 0x05D8,0x05D9,0x05DA,0x05DB,0x05DC,0x05DD,0x05DE,0x05DF,
 0x05E0,0x05E1,0x05E2,0x05E3,0x05E4,0x05E5,0x05E6,0x05E7,
 0x05E8,0x05E9,0x05EA,0x0000,0x0000,0x0000,0x0000,0x0000
};


static const unsigned short iso8859_9_ucs_table[] = {
 0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,
 0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
 0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
 0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
 0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
 0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
 0x011E,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,
 0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x0130,0x015E,0x00DF,
 0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
 0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
 0x011F,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,
 0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x0131,0x015F,0x00FF
};


static const unsigned short iso8859_10_ucs_table[] = {
 0x00A0,0x0104,0x0112,0x0122,0x0124,0x0128,0x0136,0x00A7,
 0x013B,0x0110,0x0160,0x0166,0x017D,0x00AD,0x016A,0x014A,
 0x00B0,0x0105,0x0113,0x0123,0x012B,0x0129,0x0137,0x00B7,
 0x013C,0x0111,0x0161,0x0167,0x017E,0x2015,0x016B,0x014B,
 0x0100,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x012E,
 0x010C,0x00C9,0x0118,0x00CB,0x0116,0x00CD,0x00CE,0x00CF,
 0x00D0,0x0145,0x014C,0x00D3,0x00D4,0x00D5,0x00D6,0x0168,
 0x00D8,0x0172,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
 0x0101,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x012F,
 0x010D,0x00E9,0x0119,0x00EB,0x0117,0x00ED,0x00EE,0x00EF,
 0x00F0,0x0146,0x014D,0x00F3,0x00F4,0x00F5,0x00F6,0x0169,
 0x00F8,0x0173,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x0138
};


static const unsigned short iso8859_13_ucs_table[] = {
 0x00A0,0x201D,0x00A2,0x00A3,0x00A4,0x201E,0x00A6,0x00A7,
 0x00D8,0x00A9,0x0156,0x00AB,0x00AC,0x00AD,0x00AE,0x00C6,
 0x00B0,0x00B1,0x00B2,0x00B3,0x201C,0x00B5,0x00B6,0x00B7,
 0x00F8,0x00B9,0x0157,0x00BB,0x00BC,0x00BD,0x00BE,0x00E6,
 0x0104,0x012E,0x0100,0x0106,0x00C4,0x00C5,0x0118,0x0112,
 0x010C,0x00C9,0x0179,0x0116,0x0122,0x0136,0x012A,0x013B,
 0x0160,0x0143,0x0145,0x00D3,0x014C,0x00D5,0x00D6,0x00D7,
 0x0172,0x0141,0x015A,0x016A,0x00DC,0x017B,0x017D,0x00DF,
 0x0105,0x012F,0x0101,0x0107,0x00E4,0x00E5,0x0119,0x0113,
 0x010D,0x00E9,0x017A,0x0117,0x0123,0x0137,0x012B,0x013C,
 0x0161,0x0144,0x0146,0x00F3,0x014D,0x00F5,0x00F6,0x00F7,
 0x0173,0x0142,0x015B,0x016B,0x00FC,0x017C,0x017E,0x2019
};


static const unsigned short iso8859_14_ucs_table[] = {
 0x00A0,0x1E02,0x1E03,0x00A3,0x010A,0x010B,0x1E0A,0x00A7,
 0x1E80,0x00A9,0x1E82,0x1E0B,0x1EF2,0x00AD,0x00AE,0x0178,
 0x1E1E,0x1E1F,0x0120,0x0121,0x1E40,0x1E41,0x00B6,0x1E56,
 0x1E81,0x1E57,0x1E83,0x1E60,0x1EF3,0x1E84,0x1E85,0x1E61,
 0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
 0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
 0x0174,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x1E6A,
 0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x0176,0x00DF,
 0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
 0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
 0x0175,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x1E6B,
 0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x0177,0x00FF
};


static const unsigned short iso8859_15_ucs_table[] = {
 0x00A0,0x00A1,0x00A2,0x00A3,0x20AC,0x00A5,0x0160,0x00A7,
 0x0161,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
 0x00B0,0x00B1,0x00B2,0x00B3,0x017D,0x00B5,0x00B6,0x00B7,
 0x017E,0x00B9,0x00BA,0x00BB,0x0152,0x0153,0x0178,0x00BF,
 0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
 0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
 0x00D0,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,
 0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
 0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
 0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
 0x00F0,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,
 0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x00FF
};


static const unsigned char mbfl_charprop_table[] = {
/* NUL	0 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC,
/* SCH	1 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SIX	2 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EIX	3 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EOT	4 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* ENQ	5 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* ACK	6 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* BEL	7 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* BS	8 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* HI	9 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* LF	10 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* VI	11 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* FF	12 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* CR	13 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SO	14 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SI	15 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SLE	16 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* CSI	17 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* DC2	18 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* DC3	19 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* DC4	20 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* NAK	21 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SYN	22 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EIB	23 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* CAN	24 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* EM	25 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SLB	26 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* ESC	27 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* FS	28 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* GS	29 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* RS	30 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* US	31 */	MBFL_CHP_CTL | MBFL_CHP_MMHQENC ,
/* SP	32 */	MBFL_CHP_MMHQENC ,
/* !	33 */	0 ,
/* "	34 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* #	35 */	MBFL_CHP_MMHQENC ,
/* $	36 */	MBFL_CHP_MMHQENC ,
/* %	37 */	MBFL_CHP_MMHQENC ,
/* &	38 */	MBFL_CHP_MMHQENC ,
/* '	39 */	MBFL_CHP_MMHQENC ,
/* (	40 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* )	41 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* *	42 */	0 ,
/* +	43 */	0 ,
/* ,	44 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* -	45 */	0 ,
/* .	46 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* /	47 */	0 ,
/* 0	48 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 1	49 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 2	50 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 3	51 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 4	52 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 5	53 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 6	54 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 7	55 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 8	56 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* 9	57 */	MBFL_CHP_DIGIT | MBFL_CHP_MMHQENC ,
/* :	58 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ;	59 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* <	60 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* =	61 */	0 ,
/* >	62 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ?	63 */	MBFL_CHP_MMHQENC ,
/* @	64 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* A	65 */	MBFL_CHP_UALPHA ,
/* B	66 */	MBFL_CHP_UALPHA ,
/* C	67 */	MBFL_CHP_UALPHA ,
/* D	68 */	MBFL_CHP_UALPHA ,
/* E	69 */	MBFL_CHP_UALPHA ,
/* F	70 */	MBFL_CHP_UALPHA ,
/* G	71 */	MBFL_CHP_UALPHA ,
/* H	72 */	MBFL_CHP_UALPHA ,
/* I	73 */	MBFL_CHP_UALPHA ,
/* J	74 */	MBFL_CHP_UALPHA ,
/* K	75 */	MBFL_CHP_UALPHA ,
/* L	76 */	MBFL_CHP_UALPHA ,
/* M	77 */	MBFL_CHP_UALPHA ,
/* N	78 */	MBFL_CHP_UALPHA ,
/* O	79 */	MBFL_CHP_UALPHA ,
/* P	80 */	MBFL_CHP_UALPHA ,
/* Q	81 */	MBFL_CHP_UALPHA ,
/* R	82 */	MBFL_CHP_UALPHA ,
/* S	83 */	MBFL_CHP_UALPHA ,
/* T	84 */	MBFL_CHP_UALPHA ,
/* U	85 */	MBFL_CHP_UALPHA ,
/* V	86 */	MBFL_CHP_UALPHA ,
/* W	87 */	MBFL_CHP_UALPHA ,
/* X	88 */	MBFL_CHP_UALPHA ,
/* Y	89 */	MBFL_CHP_UALPHA ,
/* Z	90 */	MBFL_CHP_UALPHA ,
/* [	91 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* \	92 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ]	93 */	MBFL_CHP_MMHQENC | MBFL_CHP_MSPECIAL ,
/* ^	94 */	MBFL_CHP_MMHQENC ,
/* _	95 */	MBFL_CHP_MMHQENC ,
/* `	96 */	MBFL_CHP_MMHQENC ,
/* a	97 */	MBFL_CHP_LALPHA ,
/* b	98 */	MBFL_CHP_LALPHA ,
/* c	99 */	MBFL_CHP_LALPHA ,
/* d	100 */	MBFL_CHP_LALPHA ,
/* e	101 */	MBFL_CHP_LALPHA ,
/* f	102 */	MBFL_CHP_LALPHA ,
/* g	103 */	MBFL_CHP_LALPHA ,
/* h	104 */	MBFL_CHP_LALPHA ,
/* i	105 */	MBFL_CHP_LALPHA ,
/* j	106 */	MBFL_CHP_LALPHA ,
/* k	107 */	MBFL_CHP_LALPHA ,
/* l	108 */	MBFL_CHP_LALPHA ,
/* m	109 */	MBFL_CHP_LALPHA ,
/* n	110 */	MBFL_CHP_LALPHA ,
/* o	111 */	MBFL_CHP_LALPHA ,
/* p	112 */	MBFL_CHP_LALPHA ,
/* q	113 */	MBFL_CHP_LALPHA ,
/* r	114 */	MBFL_CHP_LALPHA ,
/* s	115 */	MBFL_CHP_LALPHA ,
/* t	116 */	MBFL_CHP_LALPHA ,
/* u	117 */	MBFL_CHP_LALPHA ,
/* v	118 */	MBFL_CHP_LALPHA ,
/* w	119 */	MBFL_CHP_LALPHA ,
/* x	120 */	MBFL_CHP_LALPHA ,
/* y	121 */	MBFL_CHP_LALPHA ,
/* z	122 */	MBFL_CHP_LALPHA ,
/* {	123 */	MBFL_CHP_MMHQENC ,
/* |	124 */	MBFL_CHP_MMHQENC ,
/* }	125 */	MBFL_CHP_MMHQENC ,
/* ~	126 */	MBFL_CHP_MMHQENC ,
/* DEL	127 */	MBFL_CHP_MMHQENC
};
