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

#ifndef MBFL_MBFILTER_H
#define MBFL_MBFILTER_H

#include "zend.h"

#include "mbfl_defs.h"
#include "mbfl_consts.h"
#include "mbfl_encoding.h"
#include "mbfl_language.h"
#include "mbfl_string.h"
#include "mbfl_convert.h"

/* Prefer local fix, otherwise need to include too much. */
#ifndef ssize_t
#if defined(_WIN64)
#define ssize_t __int64
#elif defined(_WIN32)
#define ssize_t __int32
#elif defined(__GNUC__) && __GNUC__ >= 4
#define ssize_t long
#endif
#endif

/*
 * version information
 */
#define MBFL_VERSION_MAJOR 1
#define MBFL_VERSION_MINOR 3
#define MBFL_VERSION_TEENY 2

/*
 * convert filter
 */
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE 0
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR 1
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG 2
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY 3

/*
 * convenience macros
 */
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/*
 * buffering converter
 */
typedef struct _mbfl_buffer_converter mbfl_buffer_converter;

struct _mbfl_buffer_converter {
	mbfl_convert_filter *filter1;
	mbfl_convert_filter *filter2;
	mbfl_memory_device device;
	const mbfl_encoding *to;
};

MBFLAPI extern mbfl_buffer_converter * mbfl_buffer_converter_new(const mbfl_encoding *from, const mbfl_encoding *to, size_t buf_initsz);
MBFLAPI extern void mbfl_buffer_converter_delete(mbfl_buffer_converter *convd);
MBFLAPI extern void mbfl_buffer_converter_illegal_mode(mbfl_buffer_converter *convd, int mode);
MBFLAPI extern void mbfl_buffer_converter_illegal_substchar(mbfl_buffer_converter *convd, int substchar);
MBFLAPI extern size_t mbfl_buffer_converter_feed(mbfl_buffer_converter *convd, mbfl_string *string);
MBFLAPI extern void mbfl_buffer_converter_flush(mbfl_buffer_converter *convd);
MBFLAPI extern mbfl_string * mbfl_buffer_converter_result(mbfl_buffer_converter *convd, mbfl_string *result);
MBFLAPI extern mbfl_string * mbfl_buffer_converter_feed_result(mbfl_buffer_converter *convd, mbfl_string *string, mbfl_string *result);
MBFLAPI extern size_t mbfl_buffer_illegalchars(mbfl_buffer_converter *convd);

/*
 * encoding detector
 */
typedef struct _mbfl_encoding_detector mbfl_encoding_detector;

typedef struct {
	size_t num_illegalchars;
	size_t score;
} mbfl_encoding_detector_data;

struct _mbfl_encoding_detector {
	mbfl_convert_filter **filter_list;
	mbfl_encoding_detector_data *filter_data;
	int filter_list_size;
	int strict;
};

MBFLAPI extern mbfl_encoding_detector * mbfl_encoding_detector_new(const mbfl_encoding **elist, int elistsz, int strict);
MBFLAPI extern void mbfl_encoding_detector_delete(mbfl_encoding_detector *identd);
MBFLAPI extern int mbfl_encoding_detector_feed(mbfl_encoding_detector *identd, mbfl_string *string);
MBFLAPI extern const mbfl_encoding *mbfl_encoding_detector_judge(mbfl_encoding_detector *identd);


/*
 * encoding converter
 */
MBFLAPI extern mbfl_string *
mbfl_convert_encoding(mbfl_string *string, mbfl_string *result, const mbfl_encoding *toenc);


/*
 * identify encoding
 */
MBFLAPI extern const mbfl_encoding *
mbfl_identify_encoding(mbfl_string *string, const mbfl_encoding **elist, int elistsz, int strict);

/* Lengths -1 through -16 are reserved for error return values */
static inline int mbfl_is_error(size_t len) {
	return len >= (size_t) -16;
}

/*
 * strlen
 */
MBFLAPI extern size_t
mbfl_strlen(const mbfl_string *string);

#define MBFL_ERROR_NOT_FOUND ((size_t) -1)
#define MBFL_ERROR_ENCODING ((size_t) -4)
#define MBFL_ERROR_EMPTY ((size_t) -8)
#define MBFL_ERROR_OFFSET ((size_t) -16)

/*
 * strpos.
 * Errors: MBFL_ERROR_NOT_FOUND, MBFL_ERROR_ENCODING, MBFL_ERROR_OFFSET
 */
MBFLAPI extern size_t
mbfl_strpos(mbfl_string *haystack, mbfl_string *needle, ssize_t offset, int reverse);

/*
 * substr_count
 */
MBFLAPI extern size_t
mbfl_substr_count(mbfl_string *haystack, mbfl_string *needle);

/*
 * If specified as length, the substr until the end of the string is taken.
 */
#define MBFL_SUBSTR_UNTIL_END ((size_t) -1)

/*
 * substr
 */
MBFLAPI extern mbfl_string *
mbfl_substr(mbfl_string *string, mbfl_string *result, size_t from, size_t length);

/*
 * strcut
 */
MBFLAPI extern mbfl_string *
mbfl_strcut(mbfl_string *string, mbfl_string *result, size_t from, size_t length);

/*
 * MIME header encode
 */
struct mime_header_encoder_data;	/* forward declaration */

MBFLAPI extern struct mime_header_encoder_data *
mime_header_encoder_new(
    const mbfl_encoding *incode,
    const mbfl_encoding *outcode,
    const mbfl_encoding *encoding);

MBFLAPI extern void
mime_header_encoder_delete(struct mime_header_encoder_data *pe);

MBFLAPI extern mbfl_string *
mime_header_encoder_result(struct mime_header_encoder_data *pe, mbfl_string *result);

MBFLAPI extern mbfl_string *
mbfl_mime_header_encode(
    mbfl_string *string, mbfl_string *result,
    const mbfl_encoding *outcode,
    const mbfl_encoding *encoding,
    const char *linefeed,
    int indent);

/*
 * MIME header decode
 */
struct mime_header_decoder_data;	/* forward declaration */

MBFLAPI extern struct mime_header_decoder_data *
mime_header_decoder_new(const mbfl_encoding *outcode);

MBFLAPI extern void
mime_header_decoder_delete(struct mime_header_decoder_data *pd);

MBFLAPI extern mbfl_string *
mime_header_decoder_result(struct mime_header_decoder_data *pd, mbfl_string *result);

MBFLAPI extern mbfl_string *
mbfl_mime_header_decode(
    mbfl_string *string,
    mbfl_string *result,
    const mbfl_encoding *outcode);

#endif	/* MBFL_MBFILTER_H */
