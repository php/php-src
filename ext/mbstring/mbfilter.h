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

/*
 *
 * streamable kanji code filter and converter
 *    mbfl : Multi Byte FiLter Liblary
 *
 */

/* $Id$ */


#ifndef MBFL_MBFILTER_H
#define MBFL_MBFILTER_H

enum mbfl_no_language {
	mbfl_no_language_invalid = -1,
	mbfl_no_language_uni,
	mbfl_no_language_min,
	mbfl_no_language_catalan,		/* ca */
	mbfl_no_language_danish,		/* da */
	mbfl_no_language_german,		/* de */
	mbfl_no_language_english,		/* en */
	mbfl_no_language_estonian,		/* et */
	mbfl_no_language_greek,			/* el */
	mbfl_no_language_spanish,		/* es */
	mbfl_no_language_french,		/* fr */
	mbfl_no_language_italian,		/* it */
	mbfl_no_language_japanese,		/* ja */
	mbfl_no_language_korean,		/* ko */
	mbfl_no_language_dutch,			/* nl */
	mbfl_no_language_polish,		/* pl */
	mbfl_no_language_portuguese,	        /* pt */
	mbfl_no_language_swedish,		/* sv */
	mbfl_no_language_simplified_chinese,		/* zh-cn */
	mbfl_no_language_traditional_chinese,		/* zh-tw */
	mbfl_no_language_russian,		/* ru */
	mbfl_no_language_max
};

enum mbfl_no_encoding {
	mbfl_no_encoding_invalid = -1,
	mbfl_no_encoding_pass,
	mbfl_no_encoding_auto,
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_byte2be,
	mbfl_no_encoding_byte2le,
	mbfl_no_encoding_byte4be,
	mbfl_no_encoding_byte4le,
	mbfl_no_encoding_base64,
	mbfl_no_encoding_uuencode,
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
	mbfl_no_encoding_utf7,
	mbfl_no_encoding_utf7imap,
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_sjis,
	mbfl_no_encoding_eucjp_win,
	mbfl_no_encoding_sjis_win,
	mbfl_no_encoding_sjis_mac,
	mbfl_no_encoding_jis,
	mbfl_no_encoding_2022jp,
	mbfl_no_encoding_cp1252,
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
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_2022kr,
	mbfl_no_encoding_uhc,
	mbfl_no_encoding_hz,
	mbfl_no_encoding_cp1251,
	mbfl_no_encoding_cp866,
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_charset_max
};


/*
 * language
 */
typedef struct _mbfl_language {
	enum mbfl_no_language no_language;
	const char *name;
	const char *short_name;
	const char *(*aliases)[];
	enum mbfl_no_encoding mail_charset;
	enum mbfl_no_encoding mail_header_encoding;
	enum mbfl_no_encoding mail_body_encoding;
} mbfl_language;


/*
 * encoding
 */
typedef struct _mbfl_encoding {
	enum mbfl_no_encoding no_encoding;
	const char *name;
	const char *mime_name;
	const char *(*aliases)[];
	const unsigned char *mblen_table;
	unsigned int flag;
} mbfl_encoding;


#define MBFL_ENCTYPE_SBCS		0x00000001
#define MBFL_ENCTYPE_MBCS		0x00000002
#define MBFL_ENCTYPE_WCS2BE		0x00000010
#define MBFL_ENCTYPE_WCS2LE		0x00000020
#define MBFL_ENCTYPE_MWC2BE		0x00000040
#define MBFL_ENCTYPE_MWC2LE		0x00000080
#define MBFL_ENCTYPE_WCS4BE		0x00000100
#define MBFL_ENCTYPE_WCS4LE		0x00000200
#define MBFL_ENCTYPE_MWC4BE		0x00000400
#define MBFL_ENCTYPE_MWC4LE		0x00000800
#define MBFL_ENCTYPE_SHFTCODE	0x00001000 

/* wchar plane, special charactor */
#define MBFL_WCSPLANE_MASK			0xffff
#define MBFL_WCSPLANE_UCS2MAX		0x00010000
#define MBFL_WCSPLANE_SUPMIN		0x00010000
#define MBFL_WCSPLANE_SUPMAX		0x00200000
#define MBFL_WCSPLANE_JIS0208		0x70e10000		/* JIS HEX : 2121h - 7E7Eh */
#define MBFL_WCSPLANE_JIS0212		0x70e20000		/* JIS HEX : 2121h - 7E7Eh */
#define MBFL_WCSPLANE_WINCP932		0x70e30000		/* JIS HEX : 2121h - 9898h */
#define MBFL_WCSPLANE_8859_1		0x70e40000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_2		0x70e50000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_3		0x70e60000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_4		0x70e70000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_5		0x70e80000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_6		0x70e90000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_7		0x70ea0000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_8		0x70eb0000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_9		0x70ec0000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_10		0x70ed0000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_13		0x70ee0000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_14		0x70ef0000		/*  00h - FFh */
#define MBFL_WCSPLANE_8859_15		0x70f00000		/*  00h - FFh */
#define MBFL_WCSPLANE_KSC5601		0x70f10000		/*  2121h - 7E7Eh */
#define MBFL_WCSPLANE_GB2312		0x70f20000		/*  2121h - 7E7Eh */
#define MBFL_WCSPLANE_WINCP936		0x70f30000		/*  2121h - 9898h */
#define MBFL_WCSPLANE_BIG5		0x70f40000		/*  2121h - 9898h */
#define MBFL_WCSPLANE_CNS11643		0x70f50000		/*  2121h - 9898h */
#define MBFL_WCSPLANE_UHC		0x70f60000		/*  8141h - fefeh */
#define MBFL_WCSPLANE_CP1251		0x70f70000	
#define MBFL_WCSPLANE_CP866			0x70f80000	
#define MBFL_WCSPLANE_KOI8R 		0x70f90000	
#define MBFL_WCSGROUP_MASK                0xffffff
#define MBFL_WCSGROUP_UCS4MAX		0x70000000
#define MBFL_WCSGROUP_WCHARMAX		0x78000000
#define MBFL_WCSGROUP_THROUGH		0x78000000		/* 000000h - FFFFFFh */


/*
 * string object
 */
typedef struct _mbfl_string {
	enum mbfl_no_language no_language;
	enum mbfl_no_encoding no_encoding;
	unsigned char *val;
	unsigned int len;
} mbfl_string;

void mbfl_string_init(mbfl_string *string);
void mbfl_string_init_set(mbfl_string *string, enum mbfl_no_language no_language, enum mbfl_no_encoding no_encoding);
void mbfl_string_clear(mbfl_string *string);


/*
 * language resolver
 */
const mbfl_language * mbfl_name2language(const char *name);
const mbfl_language * mbfl_no2language(enum mbfl_no_language no_language);
enum mbfl_no_language mbfl_name2no_language(const char *name);
const char * mbfl_no_language2name(enum mbfl_no_language no_language);


/*
 * encoding resolver
 */
const mbfl_encoding * mbfl_name2encoding(const char *name);
const mbfl_encoding * mbfl_no2encoding(enum mbfl_no_encoding no_encoding);
enum mbfl_no_encoding mbfl_name2no_encoding(const char *name);
const char * mbfl_no_encoding2name(enum mbfl_no_encoding no_encoding);
const char * mbfl_no2preferred_mime_name(enum mbfl_no_encoding no_encoding);
int mbfl_is_support_encoding(const char *name);


/*
 * memory output function
 */
#define MBFL_MEMORY_DEVICE_ALLOC_SIZE	64

typedef struct _mbfl_memory_device {
	unsigned char *buffer;
	int length;
	int pos;
	int allocsz;
} mbfl_memory_device;

typedef struct _mbfl_wchar_device {
	unsigned int *buffer;
	int length;
	int pos;
	int allocsz;
} mbfl_wchar_device;

void mbfl_memory_device_init(mbfl_memory_device *device, int initsz, int allocsz TSRMLS_DC);
void mbfl_memory_device_realloc(mbfl_memory_device *device, int initsz, int allocsz TSRMLS_DC);
void mbfl_memory_device_clear(mbfl_memory_device *device TSRMLS_DC);
void mbfl_memory_device_reset(mbfl_memory_device *device TSRMLS_DC);
mbfl_string * mbfl_memory_device_result(mbfl_memory_device *device, mbfl_string *result TSRMLS_DC);
int mbfl_memory_device_output(int c, void *data TSRMLS_DC);
int mbfl_memory_device_output2(int c, void *data TSRMLS_DC);
int mbfl_memory_device_output4(int c, void *data TSRMLS_DC);
int mbfl_memory_device_strcat(mbfl_memory_device *device, const char *psrc TSRMLS_DC);
int mbfl_memory_device_strncat(mbfl_memory_device *device, const char *psrc, int len TSRMLS_DC);
int mbfl_memory_device_devcat(mbfl_memory_device *dest, mbfl_memory_device *src TSRMLS_DC);

void mbfl_wchar_device_init(mbfl_wchar_device *device TSRMLS_DC);
int mbfl_wchar_device_output(int c, void *data TSRMLS_DC);


/*
 * convert filter
 */
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE 0
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR 1
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG 2

typedef struct _mbfl_convert_filter mbfl_convert_filter;

struct _mbfl_convert_filter {
	void (*filter_ctor)(mbfl_convert_filter *filter TSRMLS_DC);
	void (*filter_dtor)(mbfl_convert_filter *filter TSRMLS_DC);
	int (*filter_function)(int c, mbfl_convert_filter *filter TSRMLS_DC);
	int (*filter_flush)(mbfl_convert_filter *filter TSRMLS_DC);
	int (*output_function)(int c, void *data TSRMLS_DC);
	int (*flush_function)(void *data TSRMLS_DC);
	void *data;
	int status;
	int cache;
	const mbfl_encoding *from;
	const mbfl_encoding *to;
	int illegal_mode;
	int illegal_substchar;
};

struct mbfl_convert_vtbl {
	enum mbfl_no_encoding from;
	enum mbfl_no_encoding to;
	void (*filter_ctor)(mbfl_convert_filter *filter TSRMLS_DC);
	void (*filter_dtor)(mbfl_convert_filter *filter TSRMLS_DC);
	int (*filter_function)(int c, mbfl_convert_filter *filter TSRMLS_DC);
	int (*filter_flush)(mbfl_convert_filter *filter TSRMLS_DC);
};

mbfl_convert_filter *
mbfl_convert_filter_new(
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int (*output_function)(int, void * TSRMLS_DC),
    int (*flush_function)(void * TSRMLS_DC),
    void *data TSRMLS_DC);
void mbfl_convert_filter_delete(mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter TSRMLS_DC);
int mbfl_convert_filter_flush(mbfl_convert_filter *filter TSRMLS_DC);
void mbfl_convert_filter_reset(mbfl_convert_filter *filter, enum mbfl_no_encoding from, enum mbfl_no_encoding to TSRMLS_DC);
void mbfl_convert_filter_copy(mbfl_convert_filter *src, mbfl_convert_filter *dist TSRMLS_DC);
int mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter TSRMLS_DC);


/*
 * identify filter
 */
typedef struct _mbfl_identify_filter mbfl_identify_filter;

struct _mbfl_identify_filter {
	void (*filter_ctor)(mbfl_identify_filter *filter TSRMLS_DC);
	void (*filter_dtor)(mbfl_identify_filter *filter TSRMLS_DC);
	int (*filter_function)(int c, mbfl_identify_filter *filter TSRMLS_DC);
	int status;
	int flag;
	int score;
	const mbfl_encoding *encoding;
};

struct mbfl_identify_vtbl {
	enum mbfl_no_encoding encoding;
	void (*filter_ctor)(mbfl_identify_filter *filter TSRMLS_DC);
	void (*filter_dtor)(mbfl_identify_filter *filter TSRMLS_DC);
	int (*filter_function)(int c, mbfl_identify_filter *filter TSRMLS_DC);
};

mbfl_identify_filter * mbfl_identify_filter_new(enum mbfl_no_encoding encoding TSRMLS_DC);
void mbfl_identify_filter_delete(mbfl_identify_filter *filter TSRMLS_DC);


/*
 * buffering converter
 */
typedef struct _mbfl_buffer_converter mbfl_buffer_converter;

struct _mbfl_buffer_converter {
	mbfl_convert_filter *filter1;
	mbfl_convert_filter *filter2;
	mbfl_memory_device device;
	const mbfl_encoding *from;
	const mbfl_encoding *to;
};

mbfl_buffer_converter * mbfl_buffer_converter_new(enum mbfl_no_encoding from, enum mbfl_no_encoding to, int buf_initsz TSRMLS_DC);
void mbfl_buffer_converter_delete(mbfl_buffer_converter *convd TSRMLS_DC);
void mbfl_buffer_converter_reset(mbfl_buffer_converter *convd TSRMLS_DC);
int mbfl_buffer_converter_illegal_mode(mbfl_buffer_converter *convd, int mode TSRMLS_DC);
int mbfl_buffer_converter_illegal_substchar(mbfl_buffer_converter *convd, int substchar TSRMLS_DC);
int mbfl_buffer_converter_strncat(mbfl_buffer_converter *convd, const unsigned char *p, int n TSRMLS_DC);
int mbfl_buffer_converter_feed(mbfl_buffer_converter *convd, mbfl_string *string TSRMLS_DC);
int mbfl_buffer_converter_flush(mbfl_buffer_converter *convd TSRMLS_DC);
mbfl_string * mbfl_buffer_converter_getbuffer(mbfl_buffer_converter *convd, mbfl_string *result TSRMLS_DC);
mbfl_string * mbfl_buffer_converter_result(mbfl_buffer_converter *convd, mbfl_string *result TSRMLS_DC);
mbfl_string * mbfl_buffer_converter_feed_result(mbfl_buffer_converter *convd, mbfl_string *string, mbfl_string *result TSRMLS_DC);


/*
 * encoding detector
 */
typedef struct _mbfl_encoding_detector mbfl_encoding_detector;

struct _mbfl_encoding_detector {
	mbfl_identify_filter **filter_list;
	int filter_list_size;
};

mbfl_encoding_detector * mbfl_encoding_detector_new(enum mbfl_no_encoding *elist, int eliztsz TSRMLS_DC);
void mbfl_encoding_detector_delete(mbfl_encoding_detector *identd TSRMLS_DC);
int mbfl_encoding_detector_feed(mbfl_encoding_detector *identd, mbfl_string *string TSRMLS_DC);
enum mbfl_no_encoding mbfl_encoding_detector_judge(mbfl_encoding_detector *identd TSRMLS_DC);


/*
 * encoding converter
 */
mbfl_string *
mbfl_convert_encoding(mbfl_string *string, mbfl_string *result, enum mbfl_no_encoding toenc TSRMLS_DC);


/*
 * identify encoding
 */
const mbfl_encoding *
mbfl_identify_encoding(mbfl_string *string, enum mbfl_no_encoding *elist, int eliztsz TSRMLS_DC);

const char *
mbfl_identify_encoding_name(mbfl_string *string, enum mbfl_no_encoding *elist, int eliztsz TSRMLS_DC);

const enum mbfl_no_encoding
mbfl_identify_encoding_no(mbfl_string *string, enum mbfl_no_encoding *elist, int eliztsz TSRMLS_DC);

/*
 * strlen
 */
int
mbfl_strlen(mbfl_string *string TSRMLS_DC);

#ifdef ZEND_MULTIBYTE
/*
 * oddlen
 */
int
mbfl_oddlen(mbfl_string *string);
#endif /* ZEND_MULTIBYTE */

/*
 * strpos
 */
int
mbfl_strpos(mbfl_string *haystack, mbfl_string *needle, int offset, int reverse TSRMLS_DC);

/*
 * substr
 */
mbfl_string *
mbfl_substr(mbfl_string *string, mbfl_string *result, int from, int length TSRMLS_DC);

/*
 * strcut
 */
mbfl_string *
mbfl_strcut(mbfl_string *string, mbfl_string *result, int from, int length TSRMLS_DC);

/*
 *  strwidth
 */
int
mbfl_strwidth(mbfl_string *string TSRMLS_DC);

/*
 *  strimwidth
 */
mbfl_string *
mbfl_strimwidth(mbfl_string *string, mbfl_string *marker, mbfl_string *result, int from, int width TSRMLS_DC);

/*
 * MIME header encode
 */
struct mime_header_encoder_data;	/* forward declaration */

struct mime_header_encoder_data *
mime_header_encoder_new(
    enum mbfl_no_encoding incode,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding encoding TSRMLS_DC);

void
mime_header_encoder_delete(struct mime_header_encoder_data *pe TSRMLS_DC);

int
mime_header_encoder_feed(int c, struct mime_header_encoder_data *pe TSRMLS_DC);

mbfl_string *
mime_header_encoder_result(struct mime_header_encoder_data *pe, mbfl_string *result TSRMLS_DC);

mbfl_string *
mbfl_mime_header_encode(
    mbfl_string *string, mbfl_string *result,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding encoding,
    const char *linefeed,
    int indent TSRMLS_DC);

/*
 * MIME header decode
 */
struct mime_header_decoder_data;	/* forward declaration */

struct mime_header_decoder_data *
mime_header_decoder_new(enum mbfl_no_encoding outcode TSRMLS_DC);

void
mime_header_decoder_delete(struct mime_header_decoder_data *pd TSRMLS_DC);

int
mime_header_decoder_feed(int c, struct mime_header_decoder_data *pd TSRMLS_DC);

mbfl_string *
mime_header_decoder_result(struct mime_header_decoder_data *pd, mbfl_string *result TSRMLS_DC);

mbfl_string *
mbfl_mime_header_decode(
    mbfl_string *string,
    mbfl_string *result,
    enum mbfl_no_encoding outcode TSRMLS_DC);


/*
 * convert HTML numeric entity
 */
mbfl_string *
mbfl_html_numeric_entity(mbfl_string *string, mbfl_string *result, int *convmap, int mapsize, int type TSRMLS_DC);


/*
 * convert of harfwidth and fullwidth for japanese
 */
mbfl_string *
mbfl_ja_jp_hantozen(mbfl_string *string, mbfl_string *result, int mode TSRMLS_DC);

#endif	/* MBFL_MBFILTER_H */
