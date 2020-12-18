/*
 * charset=UTF-8
 */

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

#include <stddef.h>
#include <string.h>

#include "mbfilter.h"
#include "mbfl_filter_output.h"
#include "mbfilter_8bit.h"
#include "mbfilter_wchar.h"
#include "mbstring.h"
#include "php_unicode.h"
#include "filters/mbfilter_base64.h"
#include "filters/mbfilter_qprint.h"
#include "filters/mbfilter_singlebyte.h"
#include "filters/mbfilter_tl_jisx0201_jisx0208.h"
#include "filters/mbfilter_utf8.h"

#include "eaw_table.h"

/* hex character table "0123456789ABCDEF" */
static char mbfl_hexchar_table[] = {
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46
};



/*
 * encoding filter
 */
#define CK(statement)	do { if ((statement) < 0) return (-1); } while (0)


/*
 *  buffering converter
 */
mbfl_buffer_converter *
mbfl_buffer_converter_new(
	const mbfl_encoding *from,
	const mbfl_encoding *to,
    size_t buf_initsz)
{
	mbfl_buffer_converter *convd = emalloc(sizeof(mbfl_buffer_converter));
	convd->to = to;

	/* create convert filter */
	convd->filter1 = NULL;
	convd->filter2 = NULL;
	if (mbfl_convert_filter_get_vtbl(from, to) != NULL) {
		convd->filter1 = mbfl_convert_filter_new(from, to, mbfl_memory_device_output, NULL, &convd->device);
	} else {
		convd->filter2 = mbfl_convert_filter_new(&mbfl_encoding_wchar, to, mbfl_memory_device_output, NULL, &convd->device);
		if (convd->filter2 != NULL) {
			convd->filter1 = mbfl_convert_filter_new(from,
					&mbfl_encoding_wchar,
					(output_function_t)convd->filter2->filter_function,
					(flush_function_t)convd->filter2->filter_flush,
					convd->filter2);
			if (convd->filter1 == NULL) {
				mbfl_convert_filter_delete(convd->filter2);
			}
		}
	}
	if (convd->filter1 == NULL) {
		efree(convd);
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
		efree((void*)convd);
	}
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

size_t mbfl_buffer_converter_feed(mbfl_buffer_converter *convd, mbfl_string *string)
{
	size_t n;
	unsigned char *p;
	mbfl_convert_filter *filter;

	ZEND_ASSERT(convd);
	ZEND_ASSERT(string);

	mbfl_memory_device_realloc(&convd->device, convd->device.pos + string->len, string->len/4);
	/* feed data */
	n = string->len;
	p = string->val;

	filter = convd->filter1;
	if (filter != NULL) {
		while (n > 0) {
			if ((*filter->filter_function)(*p++, filter) < 0) {
				return p - string->val;
			}
			n--;
		}
	}
	return p - string->val;
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

	return 0;
}

mbfl_string *
mbfl_buffer_converter_result(mbfl_buffer_converter *convd, mbfl_string *result)
{
	if (convd == NULL || result == NULL) {
		return NULL;
	}
	result->encoding = convd->to;
	return mbfl_memory_device_result(&convd->device, result);
}

mbfl_string *
mbfl_buffer_converter_feed_result(mbfl_buffer_converter *convd, mbfl_string *string,
				  mbfl_string *result)
{
	if (convd == NULL || string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_buffer_converter_feed(convd, string);
	if (convd->filter1 != NULL) {
		mbfl_convert_filter_flush(convd->filter1);
	}
	result->encoding = convd->to;
	return mbfl_memory_device_result(&convd->device, result);
}

size_t mbfl_buffer_illegalchars(mbfl_buffer_converter *convd)
{
	size_t num_illegalchars = 0;

	if (convd == NULL) {
		return 0;
	}

	if (convd->filter1 != NULL) {
		num_illegalchars += convd->filter1->num_illegalchar;
	}

	if (convd->filter2 != NULL) {
		num_illegalchars += convd->filter2->num_illegalchar;
	}

	return num_illegalchars;
}

/*
 * encoding detector
 */
static int mbfl_estimate_encoding_likelihood(int c, void* data)
{
	mbfl_convert_filter *filter = *((mbfl_convert_filter**)data);
	uintptr_t *score = (uintptr_t*)(&filter->opaque);

	/* Receive wchars decoded from test string using candidate encoding
	 * If the test string was invalid in the candidate encoding, we assume
	 * it's the wrong one. */
	if (c & MBFL_WCSGROUP_THROUGH) {
		filter->num_illegalchar++;
	} else if (php_unicode_is_cntrl(c) || php_unicode_is_private(c)) {
		/* Otherwise, count how many control characters and 'private use'
		 * codepoints we see. Those are rarely used and may indicate that
		 * the candidate encoding is not the right one. */
		*score += 10;
	} else if (php_unicode_is_punct(c)) {
		/* Punctuation is also less common than letters/digits */
		(*score)++;
	}
	return c;
}

mbfl_encoding_detector *mbfl_encoding_detector_new(const mbfl_encoding **elist, int elistsz, int strict)
{
	if (!elistsz) {
		return NULL;
	}

	mbfl_encoding_detector *identd = emalloc(sizeof(mbfl_encoding_detector));
	identd->filter_list = ecalloc(elistsz, sizeof(mbfl_convert_filter*));
	for (int i = 0; i < elistsz; i++) {
		identd->filter_list[i] = mbfl_convert_filter_new(elist[i], &mbfl_encoding_wchar,
			mbfl_estimate_encoding_likelihood, NULL, &identd->filter_list[i]);
		identd->filter_list[i]->opaque = (void*)0;
	}
	identd->filter_list_size = elistsz;
	identd->strict = strict;
	return identd;
}

void mbfl_encoding_detector_delete(mbfl_encoding_detector *identd)
{
	for (int i = 0; i < identd->filter_list_size; i++) {
		mbfl_convert_filter_delete(identd->filter_list[i]);
	}
	efree(identd->filter_list);
	efree(identd);
}

int mbfl_encoding_detector_feed(mbfl_encoding_detector *identd, mbfl_string *string)
{
	int num = identd->filter_list_size;
	size_t n = string->len;
	unsigned char *p = string->val;
	int bad = 0;

	while (n--) {
		for (int i = 0; i < num; i++) {
			mbfl_convert_filter *filter = identd->filter_list[i];
			if (!filter->num_illegalchar) {
				(*filter->filter_function)(*p, filter);
				if (filter->num_illegalchar) {
					bad++;
				}
			}
		}
		if ((num - 1) <= bad && !identd->strict) {
			return 1;
		}
		p++;
	}

	if (identd->strict) {
		for (int i = 0; i < num; i++) {
			mbfl_convert_filter *filter = identd->filter_list[i];
			(filter->filter_flush)(filter);
		}
	}

	return 0;
}

const mbfl_encoding *mbfl_encoding_detector_judge(mbfl_encoding_detector *identd)
{
	uintptr_t best_score = UINT_MAX; /* Low score is 'better' */
	const mbfl_encoding *enc = NULL;

	for (int i = 0; i < identd->filter_list_size; i++) {
		mbfl_convert_filter *filter = identd->filter_list[i];
		if (!filter->num_illegalchar && (uintptr_t)filter->opaque < best_score) {
			enc = filter->from;
			best_score = (uintptr_t)filter->opaque;
		}
	}

	return enc;
}

/*
 * encoding converter
 */
mbfl_string *
mbfl_convert_encoding(
    mbfl_string *string,
    mbfl_string *result,
    const mbfl_encoding *toenc)
{
	size_t n;
	unsigned char *p;
	mbfl_memory_device device;
	mbfl_convert_filter *filter1;
	mbfl_convert_filter *filter2;

	/* initialize */
	if (toenc == NULL || string == NULL || result == NULL) {
		return NULL;
	}

	filter1 = NULL;
	filter2 = NULL;
	if (mbfl_convert_filter_get_vtbl(string->encoding, toenc) != NULL) {
		filter1 = mbfl_convert_filter_new(string->encoding, toenc, mbfl_memory_device_output, 0, &device);
	} else {
		filter2 = mbfl_convert_filter_new(&mbfl_encoding_wchar, toenc, mbfl_memory_device_output, 0, &device);
		if (filter2 != NULL) {
			filter1 = mbfl_convert_filter_new(string->encoding, &mbfl_encoding_wchar, (int (*)(int, void*))filter2->filter_function, NULL, filter2);
			if (filter1 == NULL) {
				mbfl_convert_filter_delete(filter2);
			}
		}
	}
	if (filter1 == NULL) {
		return NULL;
	}

	if (filter2 != NULL) {
		filter2->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
		filter2->illegal_substchar = 0x3f;		/* '?' */
	}

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
const mbfl_encoding *mbfl_identify_encoding(mbfl_string *string, const mbfl_encoding **elist, int elistsz, int strict)
{
	if (!elistsz) {
		return NULL;
	}
	mbfl_encoding_detector *identd = mbfl_encoding_detector_new(elist, elistsz, strict);
	mbfl_encoding_detector_feed(identd, string);
	const mbfl_encoding *enc = mbfl_encoding_detector_judge(identd);
	mbfl_encoding_detector_delete(identd);
	return enc;
}

/*
 *  strlen
 */
static int
filter_count_output(int c, void *data)
{
	(*(size_t *)data)++;
	return c;
}

size_t
mbfl_strlen(const mbfl_string *string)
{
	size_t len, n, k;
	unsigned char *p;
	const mbfl_encoding *encoding = string->encoding;

	len = 0;
	if (encoding->flag & MBFL_ENCTYPE_SBCS) {
		len = string->len;
	} else if (encoding->flag & MBFL_ENCTYPE_WCS2) {
		len = string->len/2;
	} else if (encoding->flag & MBFL_ENCTYPE_WCS4) {
		len = string->len/4;
	} else if (encoding->mblen_table != NULL) {
		const unsigned char *mbtab = encoding->mblen_table;
		n = 0;
		p = string->val;
		k = string->len;
		/* count */
		if (p != NULL) {
			while (n < k) {
				unsigned m = mbtab[*p];
				n += m;
				p += m;
				len++;
			}
		}
	} else {
		/* wchar filter */
		mbfl_convert_filter *filter = mbfl_convert_filter_new(
		  string->encoding,
		  &mbfl_encoding_wchar,
		  filter_count_output, 0, &len);
		if (filter == NULL) {
			return (size_t) -1;
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
	size_t needle_len;
	size_t start;
	size_t output;
	size_t found_pos;
	size_t needle_pos;
	size_t matched_pos;
};

static int
collector_strpos(int c, void* data)
{
	int *p, *h, *m;
	ssize_t n;
	struct collector_strpos_data *pc = (struct collector_strpos_data*)data;

	if (pc->output >= pc->start) {
		if (c == (int)pc->needle.buffer[pc->needle_pos]) {
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
			h = (int *)pc->needle.buffer;
			h++;
			for (;;) {
				pc->found_pos++;
				p = h;
				m = (int *)pc->needle.buffer;
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

static const unsigned char *mbfl_find_offset_utf8(
		const unsigned char *str, const unsigned char *end, ssize_t offset) {
	if (offset < 0) {
		const unsigned char *pos = end;
		while (offset < 0) {
			if (pos <= str) {
				return NULL;
			}

			unsigned char c = *(--pos);
			if (c < 0x80) {
				++offset;
			} else if ((c & 0xc0) != 0x80) {
				++offset;
			}
		}
		return pos;
	} else {
		const unsigned char *u8_tbl = mbfl_encoding_utf8.mblen_table;
		const unsigned char *pos = str;
		while (offset-- > 0) {
			if (pos >= end) {
				return NULL;
			}
			pos += u8_tbl[*pos];
		}
		return pos;
	}
}

static size_t mbfl_pointer_to_offset_utf8(const unsigned char *start, const unsigned char *pos) {
	size_t result = 0;
	while (pos > start) {
		unsigned char c = *--pos;
		if (c < 0x80) {
			++result;
		} else if ((c & 0xc0) != 0x80) {
			++result;
		}
	}
	return result;
}

size_t
mbfl_strpos(
    mbfl_string *haystack,
    mbfl_string *needle,
    ssize_t offset,
    int reverse)
{
	size_t result;
	mbfl_string _haystack_u8, _needle_u8;
	const mbfl_string *haystack_u8, *needle_u8 = NULL;
	const unsigned char *offset_pointer;

	if (haystack->encoding->no_encoding != mbfl_no_encoding_utf8) {
		mbfl_string_init(&_haystack_u8);
		haystack_u8 = mbfl_convert_encoding(haystack, &_haystack_u8, &mbfl_encoding_utf8);
		if (haystack_u8 == NULL) {
			result = MBFL_ERROR_ENCODING;
			goto out;
		}
	} else {
		haystack_u8 = haystack;
	}

	if (needle->encoding->no_encoding != mbfl_no_encoding_utf8) {
		mbfl_string_init(&_needle_u8);
		needle_u8 = mbfl_convert_encoding(needle, &_needle_u8, &mbfl_encoding_utf8);
		if (needle_u8 == NULL) {
			result = MBFL_ERROR_ENCODING;
			goto out;
		}
	} else {
		needle_u8 = needle;
	}

	offset_pointer = mbfl_find_offset_utf8(
		haystack_u8->val, haystack_u8->val + haystack_u8->len, offset);
	if (!offset_pointer) {
		result = MBFL_ERROR_OFFSET;
		goto out;
	}

	result = MBFL_ERROR_NOT_FOUND;
	if (haystack_u8->len < needle_u8->len) {
		goto out;
	}

	const char *found_pos;
	if (!reverse) {
		found_pos = zend_memnstr(
			(const char *) offset_pointer,
			(const char *) needle_u8->val, needle_u8->len,
			(const char *) haystack_u8->val + haystack_u8->len);
	} else {
		if (offset >= 0) {
			found_pos = zend_memnrstr(
				(const char *) offset_pointer,
				(const char *) needle_u8->val, needle_u8->len,
				(const char *) haystack_u8->val + haystack_u8->len);
		} else {
			size_t needle_len = mbfl_strlen(needle_u8);
			offset_pointer = mbfl_find_offset_utf8(
				offset_pointer, haystack_u8->val + haystack_u8->len, needle_len);
			if (!offset_pointer) {
				offset_pointer = haystack_u8->val + haystack_u8->len;
			}

			found_pos = zend_memnrstr(
				(const char *) haystack_u8->val,
				(const char *) needle_u8->val, needle_u8->len,
				(const char *) offset_pointer);
		}
	}

	if (found_pos) {
		result = mbfl_pointer_to_offset_utf8(haystack_u8->val, (const unsigned char *) found_pos);
	}

out:
	if (haystack_u8 == &_haystack_u8) {
		mbfl_string_clear(&_haystack_u8);
	}
	if (needle_u8 == &_needle_u8) {
		mbfl_string_clear(&_needle_u8);
	}
	return result;
}

/*
 *  substr_count
 */

size_t
mbfl_substr_count(
    mbfl_string *haystack,
    mbfl_string *needle
   )
{
	size_t n, result = 0;
	unsigned char *p;
	mbfl_convert_filter *filter;
	struct collector_strpos_data pc;

	/* needle is converted into wchar */
	mbfl_wchar_device_init(&pc.needle);
	filter = mbfl_convert_filter_new(
	  needle->encoding,
	  &mbfl_encoding_wchar,
	  mbfl_wchar_device_output, 0, &pc.needle);
	if (filter == NULL) {
		return MBFL_ERROR_ENCODING;
	}
	mbfl_convert_filter_feed_string(filter, needle->val, needle->len);
	mbfl_convert_filter_flush(filter);
	mbfl_convert_filter_delete(filter);
	pc.needle_len = pc.needle.pos;
	if (pc.needle.buffer == NULL) {
		return MBFL_ERROR_ENCODING;
	}
	if (pc.needle_len == 0) {
		mbfl_wchar_device_clear(&pc.needle);
		return MBFL_ERROR_EMPTY;
	}
	/* initialize filter and collector data */
	filter = mbfl_convert_filter_new(
	  haystack->encoding,
	  &mbfl_encoding_wchar,
	  collector_strpos, 0, &pc);
	if (filter == NULL) {
		mbfl_wchar_device_clear(&pc.needle);
		return MBFL_ERROR_ENCODING;
	}
	pc.start = 0;
	pc.output = 0;
	pc.needle_pos = 0;
	pc.found_pos = 0;
	pc.matched_pos = MBFL_ERROR_NOT_FOUND;

	/* feed data */
	p = haystack->val;
	n = haystack->len;
	if (p != NULL) {
		while (n > 0) {
			if ((*filter->filter_function)(*p++, filter) < 0) {
				pc.matched_pos = MBFL_ERROR_ENCODING;
				break;
			}
			if (pc.matched_pos != MBFL_ERROR_NOT_FOUND) {
				++result;
				pc.matched_pos = MBFL_ERROR_NOT_FOUND;
				pc.needle_pos = 0;
			}
			n--;
		}
	}
	mbfl_convert_filter_flush(filter);
	mbfl_convert_filter_delete(filter);
	mbfl_wchar_device_clear(&pc.needle);

	return result;
}

/*
 *  substr
 */
struct collector_substr_data {
	mbfl_convert_filter *next_filter;
	size_t start;
	size_t stop;
	size_t output;
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
    size_t from,
    size_t length)
{
	const mbfl_encoding *encoding = string->encoding;
	size_t n, k, len, start, end;
	unsigned m;
	unsigned char *p, *w;

	mbfl_string_init(result);
	result->encoding = string->encoding;

	if ((encoding->flag & (MBFL_ENCTYPE_SBCS | MBFL_ENCTYPE_WCS2 | MBFL_ENCTYPE_WCS4)) ||
	   encoding->mblen_table != NULL) {
		len = string->len;
		if (encoding->flag & MBFL_ENCTYPE_SBCS) {
			start = from;
		} else if (encoding->flag & MBFL_ENCTYPE_WCS2) {
			start = from*2;
		} else if (encoding->flag & MBFL_ENCTYPE_WCS4) {
			start = from*4;
		} else {
			const unsigned char *mbtab = encoding->mblen_table;
			start = 0;
			n = 0;
			k = 0;
			p = string->val;
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
		}

		if (length == MBFL_SUBSTR_UNTIL_END) {
			end = len;
		} else if (encoding->flag & MBFL_ENCTYPE_SBCS) {
			end = start + length;
		} else if (encoding->flag & MBFL_ENCTYPE_WCS2) {
			end = start + length*2;
		} else if (encoding->flag & MBFL_ENCTYPE_WCS4) {
			end = start + length*4;
		} else {
			const unsigned char *mbtab = encoding->mblen_table;
			end = start;
			n = start;
			k = 0;
			p = string->val + start;
			/* detect end position */
			while (k <= length) {
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

		if (start > len) {
			start = len;
		}
		if (end > len) {
			end = len;
		}
		if (start > end) {
			start = end;
		}

		/* allocate memory and copy */
		n = end - start;
		result->len = 0;
		result->val = w = (unsigned char*)emalloc(n + 1);
		result->len = n;
		memcpy(w, string->val + start, n);
		w[n] = '\0';
	} else {
		mbfl_memory_device device;
		struct collector_substr_data pc;
		mbfl_convert_filter *decoder;
		mbfl_convert_filter *encoder;

		if (length == MBFL_SUBSTR_UNTIL_END) {
			length = mbfl_strlen(string) - from;
		}

		mbfl_memory_device_init(&device, length + 1, 0);
		mbfl_string_init(result);
		result->encoding = string->encoding;
		/* output code filter */
		decoder = mbfl_convert_filter_new(
		    &mbfl_encoding_wchar,
		    string->encoding,
		    mbfl_memory_device_output, 0, &device);
		/* wchar filter */
		encoder = mbfl_convert_filter_new(
		    string->encoding,
		    &mbfl_encoding_wchar,
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
    size_t from,
    size_t length)
{
	const mbfl_encoding *encoding = string->encoding;
	mbfl_memory_device device;

	if (from >= string->len) {
		from = string->len;
	}

	mbfl_string_init(result);
	result->encoding = string->encoding;

	if ((encoding->flag & (MBFL_ENCTYPE_SBCS | MBFL_ENCTYPE_WCS2 | MBFL_ENCTYPE_WCS4)) || encoding->mblen_table != NULL) {
		const unsigned char *start = NULL;
		const unsigned char *end = NULL;
		unsigned char *w;
		size_t sz;

		if (encoding->flag & MBFL_ENCTYPE_WCS2) {
			from &= -2;

			if (length >= string->len - from) {
				length = string->len - from;
			}

			start = string->val + from;
			end   = start + (length & -2);
		} else if (encoding->flag & MBFL_ENCTYPE_WCS4) {
			from &= -4;

			if (length >= string->len - from) {
				length = string->len - from;
			}

			start = string->val + from;
			end   = start + (length & -4);
		} else if ((encoding->flag & MBFL_ENCTYPE_SBCS)) {
			if (length >= string->len - from) {
				length = string->len - from;
			}

			start = string->val + from;
			end = start + length;
		} else if (encoding->mblen_table != NULL) {
			const unsigned char *mbtab = encoding->mblen_table;
			const unsigned char *p, *q;
			int m;

			/* search start position */
			for (m = 0, p = string->val, q = p + from;
					p < q; p += (m = mbtab[*p]));

			if (p > q) {
				p -= m;
			}

			start = p;

			/* search end position */
			if (length >= string->len - (start - string->val)) {
				end = string->val + string->len;
			} else {
				for (q = p + length; p < q; p += (m = mbtab[*p]));

				if (p > q) {
					p -= m;
				}
				end = p;
			}
		} else {
			/* never reached */
			return NULL;
		}

		/* allocate memory and copy string */
		sz = end - start;
		w = ecalloc(sz + 8, sizeof(unsigned char));

		memcpy(w, start, sz);
		w[sz] = '\0';
		w[sz + 1] = '\0';
		w[sz + 2] = '\0';
		w[sz + 3] = '\0';

		result->val = w;
		result->len = sz;
	} else {
		mbfl_convert_filter *encoder     = NULL;
		mbfl_convert_filter *decoder     = NULL;
		const unsigned char *p, *q, *r;
		struct {
			mbfl_convert_filter encoder;
			mbfl_convert_filter decoder;
			const unsigned char *p;
			size_t pos;
		} bk, _bk;

		/* output code filter */
		if (!(decoder = mbfl_convert_filter_new(
				&mbfl_encoding_wchar,
				string->encoding,
				mbfl_memory_device_output, 0, &device))) {
			return NULL;
		}

		/* wchar filter */
		if (!(encoder = mbfl_convert_filter_new(
				string->encoding,
				&mbfl_encoding_wchar,
				mbfl_filter_output_null,
				NULL, NULL))) {
			mbfl_convert_filter_delete(decoder);
			return NULL;
		}

		mbfl_memory_device_init(&device, length + 8, 0);

		p = string->val;

		/* search start position */
		for (q = string->val + from; p < q; p++) {
			(*encoder->filter_function)(*p, encoder);
		}

		/* switch the drain direction */
		encoder->output_function = (output_function_t)decoder->filter_function;
		encoder->flush_function = (flush_function_t)decoder->filter_flush;
		encoder->data = decoder;

		q = string->val + string->len;

		/* save the encoder, decoder state and the pointer */
		mbfl_convert_filter_copy(decoder, &_bk.decoder);
		mbfl_convert_filter_copy(encoder, &_bk.encoder);
		_bk.p = p;
		_bk.pos = device.pos;

		if (length > q - p) {
			length = q - p;
		}

		if (length >= 20) {
			/* output a little shorter than "length" */
			/* XXX: the constant "20" was determined purely on the heuristics. */
			for (r = p + length - 20; p < r; p++) {
				(*encoder->filter_function)(*p, encoder);
			}

			/* if the offset of the resulting string exceeds the length,
			 * then restore the state */
			if (device.pos > length) {
				p = _bk.p;
				device.pos = _bk.pos;
				if (decoder->filter_dtor)
					decoder->filter_dtor(decoder);
				if (encoder->filter_dtor)
					encoder->filter_dtor(encoder);
				mbfl_convert_filter_copy(&_bk.decoder, decoder);
				mbfl_convert_filter_copy(&_bk.encoder, encoder);
				bk = _bk;
			} else {
				/* save the encoder, decoder state and the pointer */
				mbfl_convert_filter_copy(decoder, &bk.decoder);
				mbfl_convert_filter_copy(encoder, &bk.encoder);
				bk.p = p;
				bk.pos = device.pos;

				/* flush the stream */
				(*encoder->filter_flush)(encoder);

				/* if the offset of the resulting string exceeds the length,
				 * then restore the state */
				if (device.pos > length) {
					if (bk.decoder.filter_dtor)
						bk.decoder.filter_dtor(&bk.decoder);
					if (bk.encoder.filter_dtor)
						bk.encoder.filter_dtor(&bk.encoder);

					p = _bk.p;
					device.pos = _bk.pos;
					if (decoder->filter_dtor)
						decoder->filter_dtor(decoder);
					if (encoder->filter_dtor)
						encoder->filter_dtor(encoder);
					mbfl_convert_filter_copy(&_bk.decoder, decoder);
					mbfl_convert_filter_copy(&_bk.encoder, encoder);
					bk = _bk;
				} else {
					if (_bk.decoder.filter_dtor)
						_bk.decoder.filter_dtor(&_bk.decoder);
					if (_bk.encoder.filter_dtor)
						_bk.encoder.filter_dtor(&_bk.encoder);

					p = bk.p;
					device.pos = bk.pos;
					if (decoder->filter_dtor)
						decoder->filter_dtor(decoder);
					if (encoder->filter_dtor)
						encoder->filter_dtor(encoder);
					mbfl_convert_filter_copy(&bk.decoder, decoder);
					mbfl_convert_filter_copy(&bk.encoder, encoder);
				}
			}
		} else {
			bk = _bk;
		}

		/* detect end position */
		while (p < q) {
			(*encoder->filter_function)(*p, encoder);

			if (device.pos > length) {
				/* restore filter */
				p = bk.p;
				device.pos = bk.pos;
				if (decoder->filter_dtor)
					decoder->filter_dtor(decoder);
				if (encoder->filter_dtor)
					encoder->filter_dtor(encoder);
				mbfl_convert_filter_copy(&bk.decoder, decoder);
				mbfl_convert_filter_copy(&bk.encoder, encoder);
				break;
			}

			p++;

			/* backup current state */
			mbfl_convert_filter_copy(decoder, &_bk.decoder);
			mbfl_convert_filter_copy(encoder, &_bk.encoder);
			_bk.pos = device.pos;
			_bk.p = p;

			(*encoder->filter_flush)(encoder);

			if (device.pos > length) {
				if (_bk.decoder.filter_dtor)
					_bk.decoder.filter_dtor(&_bk.decoder);
				if (_bk.encoder.filter_dtor)
					_bk.encoder.filter_dtor(&_bk.encoder);

				/* restore filter */
				p = bk.p;
				device.pos = bk.pos;
				if (decoder->filter_dtor)
					decoder->filter_dtor(decoder);
				if (encoder->filter_dtor)
					encoder->filter_dtor(encoder);
				mbfl_convert_filter_copy(&bk.decoder, decoder);
				mbfl_convert_filter_copy(&bk.encoder, encoder);
				break;
			}

			if (bk.decoder.filter_dtor)
				bk.decoder.filter_dtor(&bk.decoder);
			if (bk.encoder.filter_dtor)
				bk.encoder.filter_dtor(&bk.encoder);

			p = _bk.p;
			device.pos = _bk.pos;
			if (decoder->filter_dtor)
				decoder->filter_dtor(decoder);
			if (encoder->filter_dtor)
				encoder->filter_dtor(encoder);
			mbfl_convert_filter_copy(&_bk.decoder, decoder);
			mbfl_convert_filter_copy(&_bk.encoder, encoder);

			bk = _bk;
		}

		(*encoder->filter_flush)(encoder);

		if (bk.decoder.filter_dtor)
			bk.decoder.filter_dtor(&bk.decoder);
		if (bk.encoder.filter_dtor)
			bk.encoder.filter_dtor(&bk.encoder);

		result = mbfl_memory_device_result(&device, result);

		mbfl_convert_filter_delete(encoder);
		mbfl_convert_filter_delete(decoder);
	}

	return result;
}


/*
 *  strwidth
 */
static size_t is_fullwidth(int c)
{
	int i;

	if (c < mbfl_eaw_table[0].begin) {
		return 0;
	}

	for (i = 0; i < sizeof(mbfl_eaw_table) / sizeof(mbfl_eaw_table[0]); i++) {
		if (mbfl_eaw_table[i].begin <= c && c <= mbfl_eaw_table[i].end) {
			return 1;
		}
	}

	return 0;
}

static int
filter_count_width(int c, void* data)
{
	(*(size_t *)data) += (is_fullwidth(c) ? 2: 1);
	return c;
}

size_t
mbfl_strwidth(mbfl_string *string)
{
	size_t len, n;
	unsigned char *p;
	mbfl_convert_filter *filter;

	len = 0;
	if (string->len > 0 && string->val != NULL) {
		/* wchar filter */
		filter = mbfl_convert_filter_new(
		    string->encoding,
		    &mbfl_encoding_wchar,
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
	size_t from;
	size_t width;
	size_t outwidth;
	size_t outchar;
	size_t endpos;
	int status;
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
			pc->outwidth += (is_fullwidth(c) ? 2: 1);

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
    size_t from,
    size_t width)
{
	struct collector_strimwidth_data pc;
	mbfl_convert_filter *encoder;
	size_t n, mkwidth;
	unsigned char *p;

	if (string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->encoding = string->encoding;
	mbfl_memory_device_init(&pc.device, MIN(string->len, width), 0);

	/* output code filter */
	pc.decoder = mbfl_convert_filter_new(
	    &mbfl_encoding_wchar,
	    string->encoding,
	    mbfl_memory_device_output, 0, &pc.device);
	pc.decoder_backup = mbfl_convert_filter_new(
	    &mbfl_encoding_wchar,
	    string->encoding,
	    mbfl_memory_device_output, 0, &pc.device);
	/* wchar filter */
	encoder = mbfl_convert_filter_new(
	    string->encoding,
	    &mbfl_encoding_wchar,
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
			n--;
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
		}
		mbfl_convert_filter_flush(encoder);
		if (pc.status != 0 && mkwidth > 0) {
			pc.width += mkwidth;
			if (n > 0) {
				while (n > 0) {
					if ((*encoder->filter_function)(*p++, encoder) < 0) {
						break;
					}
					n--;
				}
				mbfl_convert_filter_flush(encoder);
			} else if (pc.outwidth > pc.width) {
				pc.status++;
			}
			if (pc.status != 1) {
				pc.status = 10;
				pc.device.pos = pc.endpos;
				mbfl_convert_filter_copy(pc.decoder_backup, pc.decoder);
				mbfl_convert_filter_reset(encoder, marker->encoding, &mbfl_encoding_wchar);
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

mbfl_string *
mbfl_ja_jp_hantozen(
    mbfl_string *string,
    mbfl_string *result,
    int mode)
{
	size_t n;
	unsigned char *p;
	mbfl_memory_device device;
	mbfl_convert_filter *decoder = NULL;
	mbfl_convert_filter *encoder = NULL;
	mbfl_convert_filter *tl_filter = NULL;
	mbfl_convert_filter *next_filter = NULL;
	mbfl_filt_tl_jisx0201_jisx0208_param *param = NULL;

	mbfl_memory_device_init(&device, string->len, 0);
	mbfl_string_init(result);

	result->encoding = string->encoding;

	decoder = mbfl_convert_filter_new(
		&mbfl_encoding_wchar,
		string->encoding,
		mbfl_memory_device_output, 0, &device);
	if (decoder == NULL) {
		goto out;
	}
	next_filter = decoder;

	param = emalloc(sizeof(mbfl_filt_tl_jisx0201_jisx0208_param));
	param->mode = mode;

	tl_filter = mbfl_convert_filter_new2(
		&vtbl_tl_jisx0201_jisx0208,
		(int(*)(int, void*))next_filter->filter_function,
		(flush_function_t)next_filter->filter_flush,
		next_filter);
	if (tl_filter == NULL) {
		efree(param);
		goto out;
	}

	tl_filter->opaque = param;
	next_filter = tl_filter;

	encoder = mbfl_convert_filter_new(
		string->encoding,
		&mbfl_encoding_wchar,
		(int(*)(int, void*))next_filter->filter_function,
		(flush_function_t)next_filter->filter_flush,
		next_filter);
	if (encoder == NULL) {
		goto out;
	}

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
	result = mbfl_memory_device_result(&device, result);
out:
	if (tl_filter != NULL) {
		if (tl_filter->opaque != NULL) {
			efree(tl_filter->opaque);
		}
		mbfl_convert_filter_delete(tl_filter);
	}

	if (decoder != NULL) {
		mbfl_convert_filter_delete(decoder);
	}

	if (encoder != NULL) {
		mbfl_convert_filter_delete(encoder);
	}

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
	size_t prevpos;
	size_t linehead;
	size_t firstindent;
	int encnamelen;
	int lwsplen;
	char encname[128];
	char lwsp[16];
};

static int
mime_header_encoder_block_collector(int c, void *data)
{
	size_t n;
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
			mbfl_memory_device_strncat(&pe->outdev, "\x3f\x3d", 2);	/* ?= */
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
	static int qp_table[256] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00 */
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x20 */
		0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 1, 0, 1, /* 0x10 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x40 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, /* 0x50 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x60 */
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, /* 0x70 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x80 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x90 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xA0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xB0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xC0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xD0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0xE0 */
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  /* 0xF0 */
	};

	size_t n;
	struct mime_header_encoder_data *pe = (struct mime_header_encoder_data *)data;

	switch (pe->status1) {
	case 11:	/* encoded word */
		(*pe->block_filter->filter_function)(c, pe->block_filter);
		break;

	default:	/* ASCII */
		if (c <= 0x00ff && !qp_table[(c & 0xff)]) { /* ordinary characters */
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
    const mbfl_encoding *incode,
    const mbfl_encoding *outcode,
    const mbfl_encoding *transenc)
{
	size_t n;
	const char *s;
	struct mime_header_encoder_data *pe;

	/* get output encoding and check MIME charset name */
	if (outcode->mime_name == NULL || outcode->mime_name[0] == '\0') {
		return NULL;
	}

	pe = emalloc(sizeof(struct mime_header_encoder_data));
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
	s = outcode->mime_name;
	while (*s) {
		pe->encname[n++] = *s++;
	}
	pe->encname[n++] = 0x3f;
	if (transenc->no_encoding == mbfl_no_encoding_qprint) {
		pe->encname[n++] = 0x51;
	} else {
		pe->encname[n++] = 0x42;
		transenc = &mbfl_encoding_base64;
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
	pe->conv2_filter = mbfl_convert_filter_new(&mbfl_encoding_wchar, outcode, mbfl_filter_output_pipe, 0, pe->encod_filter);
	pe->conv2_filter_backup = mbfl_convert_filter_new(&mbfl_encoding_wchar, outcode, mbfl_filter_output_pipe, 0, pe->encod_filter);

	/* encoded block filter */
	pe->block_filter = mbfl_convert_filter_new(&mbfl_encoding_wchar, &mbfl_encoding_wchar, mime_header_encoder_block_collector, 0, pe);

	/* Input code filter */
	pe->conv1_filter = mbfl_convert_filter_new(incode, &mbfl_encoding_wchar, mime_header_encoder_collector, 0, pe);

	if (pe->encod_filter == NULL ||
	    pe->encod_filter_backup == NULL ||
	    pe->conv2_filter == NULL ||
	    pe->conv2_filter_backup == NULL ||
	    pe->conv1_filter == NULL) {
		mime_header_encoder_delete(pe);
		return NULL;
	}

	if (transenc->no_encoding == mbfl_no_encoding_qprint) {
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
		efree((void*)pe);
	}
}

mbfl_string *
mbfl_mime_header_encode(
    mbfl_string *string,
    mbfl_string *result,
    const mbfl_encoding *outcode,
    const mbfl_encoding *encoding,
    const char *linefeed,
    int indent)
{
	size_t n;
	unsigned char *p;
	struct mime_header_encoder_data *pe;

	mbfl_string_init(result);
	result->encoding = &mbfl_encoding_ascii;

	pe = mime_header_encoder_new(string->encoding, outcode, encoding);
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
	size_t cspos;
	int status;
	const mbfl_encoding *encoding;
	const mbfl_encoding *incode;
	const mbfl_encoding *outcode;
};

static int
mime_header_decoder_collector(int c, void* data)
{
	const mbfl_encoding *encoding;
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
			encoding = mbfl_name2encoding((const char *)&pd->tmpdev.buffer[pd->cspos]);
			if (encoding != NULL) {
				pd->incode = encoding;
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
			pd->encoding = &mbfl_encoding_base64;
			pd->status = 4;
		} else if (c == 0x51 || c == 0x71) {	/* 'Q' or 'q' */
			pd->encoding = &mbfl_encoding_qprint;
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
			mbfl_convert_filter_reset(pd->conv1_filter, pd->incode, &mbfl_encoding_wchar);
			/* decode filter */
			mbfl_convert_filter_reset(pd->deco_filter, pd->encoding, &mbfl_encoding_8bit);
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
			mbfl_convert_filter_reset(pd->conv1_filter, &mbfl_encoding_ascii, &mbfl_encoding_wchar);
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
mime_header_decoder_new(const mbfl_encoding *outcode)
{
	struct mime_header_decoder_data *pd = emalloc(sizeof(struct mime_header_decoder_data));

	mbfl_memory_device_init(&pd->outdev, 0, 0);
	mbfl_memory_device_init(&pd->tmpdev, 0, 0);
	pd->cspos = 0;
	pd->status = 0;
	pd->encoding = &mbfl_encoding_8bit;
	pd->incode = &mbfl_encoding_ascii;
	pd->outcode = outcode;
	/* charset convert filter */
	pd->conv2_filter = mbfl_convert_filter_new(&mbfl_encoding_wchar, pd->outcode, mbfl_memory_device_output, 0, &pd->outdev);
	pd->conv1_filter = mbfl_convert_filter_new(pd->incode, &mbfl_encoding_wchar, mbfl_filter_output_pipe, 0, pd->conv2_filter);
	/* decode filter */
	pd->deco_filter = mbfl_convert_filter_new(pd->encoding, &mbfl_encoding_8bit, mbfl_filter_output_pipe, 0, pd->conv1_filter);

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
		efree((void*)pd);
	}
}

mbfl_string *
mbfl_mime_header_decode(
    mbfl_string *string,
    mbfl_string *result,
    const mbfl_encoding *outcode)
{
	size_t n;
	unsigned char *p;
	struct mime_header_decoder_data *pd;

	mbfl_string_init(result);
	result->encoding = outcode;

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
				(*pc->decoder->filter_function)(0x26, pc->decoder);	/* '&' */
				(*pc->decoder->filter_function)(0x23, pc->decoder);	/* '#' */
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
		if (c == 0x78) {	/* 'x' */
			pc->status = 4;
		} else if (c >= 0x30 && c <= 0x39) { /* '0' - '9' */
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
			s = pc->cache;
			if (pc->digit > 9 || s > INT_MAX/10) {
				pc->status = 0;
				f = 1;
			} else {
				s = s*10 + (c - 0x30);
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
			while (n > 1) {
				r *= 10;
				n--;
			}
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 10;
				(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
			}
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 4:
		if (c >= 0x30 && c <= 0x39) { /* '0' - '9' */
			pc->cache = c - 0x30;
			pc->status = 5;
			pc->digit = 1;
		} else if (c >= 0x41 && c <= 0x46) { /* 'A' - 'F'  */
			pc->cache = c - 0x41 + 10;
			pc->status = 5;
			pc->digit = 1;
		} else if (c >= 0x61 && c <= 0x66) { /* 'a' - 'f'  */
			pc->cache = c - 0x61 + 10;
			pc->status = 5;
			pc->digit = 1;
		} else {
			pc->status = 0;
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */
			(*pc->decoder->filter_function)(c, pc->decoder);
		}
		break;
	case 5:
		s = 0;
		f = 0;
		if ((c >= 0x30 && c <= 0x39) ||
			(c >= 0x41 && c <= 0x46) ||
			(c >= 0x61 && c <= 0x66)) {	/* '0' - '9' or 'a' - 'f'  */
			if (pc->digit > 9) {
				pc->status = 0;
				s = pc->cache;
				f = 1;
			} else {
				if (c >= 0x30 && c <= 0x39) {
					s = pc->cache*16 + (c - 0x30);
				} else if (c >= 0x41 && c <= 0x46)  {
					s = pc->cache*16 + (c - 0x41 + 10);
				} else {
					s = pc->cache*16 + (c - 0x61 + 10);
				}
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
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */
			r = 1;
			n = pc->digit;
			while (n > 0) {
				r *= 16;
				n--;
			}
			s %= r;
			r /= 16;
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 16;
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

static int
collector_encode_hex_htmlnumericentity(int c, void *data)
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
				(*pc->decoder->filter_function)(0x26, pc->decoder);	/* '&' */
				(*pc->decoder->filter_function)(0x23, pc->decoder);	/* '#' */
				(*pc->decoder->filter_function)(0x78, pc->decoder);	/* 'x' */
				r = 0x1000000;
				s %= r;
				while (r > 0) {
					d = s/r;
					if (d || f) {
						f = 1;
						s %= r;
						(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
					}
					r /= 16;
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

int mbfl_filt_decode_htmlnumericentity_flush(mbfl_convert_filter *filter)
{
	struct collector_htmlnumericentity_data *pc = (struct collector_htmlnumericentity_data *)filter;
	int n, s, r, d;

	if (pc->status) {
		switch (pc->status) {
		case 1: /* '&' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			break;
		case 2: /* '#' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			break;
		case 3: /* '0'-'9' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */

			s = pc->cache;
			r = 1;
			n = pc->digit;
			while (n > 1) {
				r *= 10;
				n--;
			}
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 10;
				(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
			}

			break;
		case 4: /* 'x' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */
			break;
		case 5: /* '0'-'9','a'-'f' */
			(*pc->decoder->filter_function)(0x26, pc->decoder);		/* '&' */
			(*pc->decoder->filter_function)(0x23, pc->decoder);		/* '#' */
			(*pc->decoder->filter_function)(0x78, pc->decoder);		/* 'x' */

			s = pc->cache;
			r = 1;
			n = pc->digit;
			while (n > 0) {
				r *= 16;
				n--;
			}
			s %= r;
			r /= 16;
			while (r > 0) {
				d = s/r;
				s %= r;
				r /= 16;
				(*pc->decoder->filter_function)(mbfl_hexchar_table[d], pc->decoder);
			}
			break;
		default:
			break;
		}
	}

	pc->status = 0;
	pc->cache = 0;
	pc->digit = 0;

	return 0;
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
	size_t n;
	unsigned char *p;

	if (string == NULL || result == NULL) {
		return NULL;
	}
	mbfl_string_init(result);
	result->encoding = string->encoding;
	mbfl_memory_device_init(&device, string->len, 0);

	/* output code filter */
	pc.decoder = mbfl_convert_filter_new(
	    &mbfl_encoding_wchar,
	    string->encoding,
	    mbfl_memory_device_output, 0, &device);
	/* wchar filter */
	if (type == 0) { /* decimal output */
		encoder = mbfl_convert_filter_new(
		    string->encoding,
		    &mbfl_encoding_wchar,
		    collector_encode_htmlnumericentity, 0, &pc);
	} else if (type == 2) { /* hex output */
		encoder = mbfl_convert_filter_new(
		    string->encoding,
		    &mbfl_encoding_wchar,
		    collector_encode_hex_htmlnumericentity, 0, &pc);
	} else { /* type == 1: decimal/hex input */
		encoder = mbfl_convert_filter_new(
		    string->encoding,
		    &mbfl_encoding_wchar,
		    collector_decode_htmlnumericentity,
		    (flush_function_t)mbfl_filt_decode_htmlnumericentity_flush, &pc);
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
