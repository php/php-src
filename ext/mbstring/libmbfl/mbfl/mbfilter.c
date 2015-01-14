/*
 * charset=UTF-8
 * vim600: encoding=utf-8
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "mbfl_filter_output.h"
#include "mbfilter_pass.h"
#include "filters/mbfilter_tl_jisx0201_jisx0208.h"

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
    enum mbfl_no_encoding from,
    enum mbfl_no_encoding to,
    int buf_initsz)
{
	const mbfl_encoding *_from = mbfl_no2encoding(from);
	const mbfl_encoding *_to = mbfl_no2encoding(to);

	return mbfl_buffer_converter_new2(_from ? _from: &mbfl_encoding_pass, _to ? _to: &mbfl_encoding_pass, buf_initsz);
}

mbfl_buffer_converter *
mbfl_buffer_converter_new2(
	const mbfl_encoding *from,
	const mbfl_encoding *to,
    int buf_initsz)
{
	mbfl_buffer_converter *convd;

	/* allocate */
	convd = (mbfl_buffer_converter*)mbfl_malloc(sizeof (mbfl_buffer_converter));
	if (convd == NULL) {
		return NULL;
	}

	/* initialize */
	convd->from = from;
	convd->to = to;

	/* create convert filter */
	convd->filter1 = NULL;
	convd->filter2 = NULL;
	if (mbfl_convert_filter_get_vtbl(convd->from->no_encoding, convd->to->no_encoding) != NULL) {
		convd->filter1 = mbfl_convert_filter_new(convd->from->no_encoding, convd->to->no_encoding, mbfl_memory_device_output, NULL, &convd->device);
	} else {
		convd->filter2 = mbfl_convert_filter_new(mbfl_no_encoding_wchar, convd->to->no_encoding, mbfl_memory_device_output, NULL, &convd->device);
		if (convd->filter2 != NULL) {
			convd->filter1 = mbfl_convert_filter_new(convd->from->no_encoding,
					mbfl_no_encoding_wchar,
					(int (*)(int, void*))convd->filter2->filter_function,
					(int (*)(void*))convd->filter2->filter_flush,
					convd->filter2);
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
	return mbfl_buffer_converter_feed2(convd, string, NULL);
}

int
mbfl_buffer_converter_feed2(mbfl_buffer_converter *convd, mbfl_string *string, int *loc)
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
				if (loc) {
					*loc = p - string->val;
				}
				return -1;
			}
			n--;
		}
	}
	if (loc) {
		*loc = p - string->val;
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
	if (convd->filter2 != NULL) {
		mbfl_convert_filter_flush(convd->filter2);
	}
	result->no_encoding = convd->to->no_encoding;
	return mbfl_memory_device_result(&convd->device, result);
}

int mbfl_buffer_illegalchars(mbfl_buffer_converter *convd)
{
	int num_illegalchars = 0;

	if (convd == NULL) {
		return 0;
	}

	if (convd->filter1 != NULL) {
		num_illegalchars += convd->filter1->num_illegalchar;
	}

	if (convd->filter2 != NULL) {
		num_illegalchars += convd->filter2->num_illegalchar;
	}

	return (num_illegalchars);
}

/*
 * encoding detector
 */
mbfl_encoding_detector *
mbfl_encoding_detector_new(enum mbfl_no_encoding *elist, int elistsz, int strict)
{
	mbfl_encoding_detector *identd;

	int i, num;
	mbfl_identify_filter *filter;

	if (elist == NULL || elistsz <= 0) {
		return NULL;
	}

	/* allocate */
	identd = (mbfl_encoding_detector*)mbfl_malloc(sizeof(mbfl_encoding_detector));
	if (identd == NULL) {
		return NULL;
	}
	identd->filter_list = (mbfl_identify_filter **)mbfl_calloc(elistsz, sizeof(mbfl_identify_filter *));
	if (identd->filter_list == NULL) {
		mbfl_free(identd);
		return NULL;
	}

	/* create filters */
	i = 0;
	num = 0;
	while (i < elistsz) {
		filter = mbfl_identify_filter_new(elist[i]);
		if (filter != NULL) {
			identd->filter_list[num] = filter;
			num++;
		}
		i++;
	}
	identd->filter_list_size = num;

	/* set strict flag */
	identd->strict = strict;

	return identd;
}

mbfl_encoding_detector *
mbfl_encoding_detector_new2(const mbfl_encoding **elist, int elistsz, int strict)
{
	mbfl_encoding_detector *identd;

	int i, num;
	mbfl_identify_filter *filter;

	if (elist == NULL || elistsz <= 0) {
		return NULL;
	}

	/* allocate */
	identd = (mbfl_encoding_detector*)mbfl_malloc(sizeof(mbfl_encoding_detector));
	if (identd == NULL) {
		return NULL;
	}
	identd->filter_list = (mbfl_identify_filter **)mbfl_calloc(elistsz, sizeof(mbfl_identify_filter *));
	if (identd->filter_list == NULL) {
		mbfl_free(identd);
		return NULL;
	}

	/* create filters */
	i = 0;
	num = 0;
	while (i < elistsz) {
		filter = mbfl_identify_filter_new2(elist[i]);
		if (filter != NULL) {
			identd->filter_list[num] = filter;
			num++;
		}
		i++;
	}
	identd->filter_list_size = num;

	/* set strict flag */
	identd->strict = strict;

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
		bad = 0;
		while (n > 0) {
			for (i = 0; i < num; i++) {
				filter = identd->filter_list[i];
				if (!filter->flag) {
					(*filter->filter_function)(*p, filter);
					if (filter->flag) {
						bad++;
					}
				}
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

const mbfl_encoding *mbfl_encoding_detector_judge2(mbfl_encoding_detector *identd)
{
	mbfl_identify_filter *filter;
	const mbfl_encoding *encoding = NULL;
	int n;

	/* judge */
	if (identd != NULL) {
		n = identd->filter_list_size - 1;
		while (n >= 0) {
			filter = identd->filter_list[n];
			if (!filter->flag) {
				if (!identd->strict || !filter->status) {
					encoding = filter->encoding;
				}
			}
			n--;
		}

		/* fallback judge */
		if (!encoding) {
			n = identd->filter_list_size - 1;
			while (n >= 0) {
				filter = identd->filter_list[n];
				if (!filter->flag) {
					encoding = filter->encoding;
				}
				n--;
 			}
		}
	}

	return encoding;
}

enum mbfl_no_encoding mbfl_encoding_detector_judge(mbfl_encoding_detector *identd)
{
	const mbfl_encoding *encoding = mbfl_encoding_detector_judge2(identd);
	return !encoding ? mbfl_no_encoding_invalid: encoding->no_encoding;
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
	const mbfl_encoding *encoding;
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
const mbfl_encoding *
mbfl_identify_encoding(mbfl_string *string, enum mbfl_no_encoding *elist, int elistsz, int strict)
{
	int i, n, num, bad;
	unsigned char *p;
	mbfl_identify_filter *flist, *filter;
	const mbfl_encoding *encoding;

	/* flist is an array of mbfl_identify_filter instances */
	flist = (mbfl_identify_filter *)mbfl_calloc(elistsz, sizeof(mbfl_identify_filter));
	if (flist == NULL) {
		return NULL;
	}

	num = 0;
	if (elist != NULL) {
		for (i = 0; i < elistsz; i++) {
			if (!mbfl_identify_filter_init(&flist[num], elist[i])) {
				num++;
			}
		}
	}

	/* feed data */
	n = string->len;
	p = string->val;

	if (p != NULL) {
		bad = 0;
		while (n > 0) {
			for (i = 0; i < num; i++) {
				filter = &flist[i];
				if (!filter->flag) {
					(*filter->filter_function)(*p, filter);
					if (filter->flag) {
						bad++;
					}
				}
			}
			if ((num - 1) <= bad && !strict) {
				break;
			}
			p++;
			n--;
		}
	}

	/* judge */
	encoding = NULL;

	for (i = 0; i < num; i++) {
		filter = &flist[i];
		if (!filter->flag) {
			if (strict && filter->status) {
 				continue;
 			}
			encoding = filter->encoding;
			break;
		}
	}

	/* fall-back judge */
	if (!encoding) {
		for (i = 0; i < num; i++) {
			filter = &flist[i];
			if (!filter->flag && (!strict || !filter->status)) {
				encoding = filter->encoding;
				break;
			}
		}
	}

	/* cleanup */
	/* dtors should be called in reverse order */
	i = num; while (--i >= 0) {
		mbfl_identify_filter_cleanup(&flist[i]);
	}

	mbfl_free((void *)flist);

	return encoding;
}

const mbfl_encoding *
mbfl_identify_encoding2(mbfl_string *string, const mbfl_encoding **elist, int elistsz, int strict)
{
	int i, n, num, bad;
	unsigned char *p;
	mbfl_identify_filter *flist, *filter;
	const mbfl_encoding *encoding;

	/* flist is an array of mbfl_identify_filter instances */
	flist = (mbfl_identify_filter *)mbfl_calloc(elistsz, sizeof(mbfl_identify_filter));
	if (flist == NULL) {
		return NULL;
	}

	num = 0;
	if (elist != NULL) {
		for (i = 0; i < elistsz; i++) {
			if (!mbfl_identify_filter_init2(&flist[num], elist[i])) {
				num++;
			}
		}
	}

	/* feed data */
	n = string->len;
	p = string->val;

	if (p != NULL) {
		bad = 0;
		while (n > 0) {
			for (i = 0; i < num; i++) {
				filter = &flist[i];
				if (!filter->flag) {
					(*filter->filter_function)(*p, filter);
					if (filter->flag) {
						bad++;
					}
				}
			}
			if ((num - 1) <= bad && !strict) {
				break;
			}
			p++;
			n--;
		}
	}

	/* judge */
	encoding = NULL;

	for (i = 0; i < num; i++) {
		filter = &flist[i];
		if (!filter->flag) {
			if (strict && filter->status) {
 				continue;
 			}
			encoding = filter->encoding;
			break;
		}
	}

	/* fall-back judge */
	if (!encoding) {
		for (i = 0; i < num; i++) {
			filter = &flist[i];
			if (!filter->flag && (!strict || !filter->status)) {
				encoding = filter->encoding;
				break;
			}
		}
	}

	/* cleanup */
	/* dtors should be called in reverse order */
	i = num; while (--i >= 0) {
		mbfl_identify_filter_cleanup(&flist[i]);
	}

	mbfl_free((void *)flist);

	return encoding;
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
	const mbfl_encoding *encoding;

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

/*
 *	oddlen
 */
int
mbfl_oddlen(mbfl_string *string)
{
	int len, n, m, k;
	unsigned char *p;
	const unsigned char *mbtab;
	const mbfl_encoding *encoding;


	if (string == NULL) {
		return -1;
	}
	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL) {
		return -1;
	}

	len = 0;
	if (encoding->flag & MBFL_ENCTYPE_SBCS) {
		return 0;
	} else if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
		return len % 2;
	} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
		return len % 4;
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
			};
		}
		return n-k;
	} else {
		/* how can i do ? */
		return 0;
	}
	/* NOT REACHED */
}

int
mbfl_strpos(
    mbfl_string *haystack,
    mbfl_string *needle,
    int offset,
    int reverse)
{
	int result;
	mbfl_string _haystack_u8, _needle_u8;
	const mbfl_string *haystack_u8, *needle_u8 = NULL;
	const unsigned char *u8_tbl;

	if (haystack == NULL || haystack->val == NULL || needle == NULL || needle->val == NULL) {
		return -8;
	}

	{
		const mbfl_encoding *u8_enc;
		u8_enc = mbfl_no2encoding(mbfl_no_encoding_utf8);
		if (u8_enc == NULL || u8_enc->mblen_table == NULL) {
			return -8;
		}
		u8_tbl = u8_enc->mblen_table;
	}

	if (haystack->no_encoding != mbfl_no_encoding_utf8) {
		mbfl_string_init(&_haystack_u8);
		haystack_u8 = mbfl_convert_encoding(haystack, &_haystack_u8, mbfl_no_encoding_utf8);
		if (haystack_u8 == NULL) {
			result = -4;
			goto out;
		}
	} else {
		haystack_u8 = haystack;
	}

	if (needle->no_encoding != mbfl_no_encoding_utf8) {
		mbfl_string_init(&_needle_u8);
		needle_u8 = mbfl_convert_encoding(needle, &_needle_u8, mbfl_no_encoding_utf8);
		if (needle_u8 == NULL) {
			result = -4;
			goto out;
		}
	} else {
		needle_u8 = needle;
	}

	if (needle_u8->len < 1) {
		result = -8;
		goto out;
	}

	result = -1;
	if (haystack_u8->len < needle_u8->len) {
		goto out;
	}

	if (!reverse) {
		unsigned int jtbl[1 << (sizeof(unsigned char) * 8)];
		unsigned int needle_u8_len = needle_u8->len;
		unsigned int i;
		const unsigned char *p, *q, *e;
		const unsigned char *haystack_u8_val = haystack_u8->val,
		                    *needle_u8_val = needle_u8->val;
		for (i = 0; i < sizeof(jtbl) / sizeof(*jtbl); ++i) {
			jtbl[i] = needle_u8_len + 1;
		}
		for (i = 0; i < needle_u8_len - 1; ++i) {
			jtbl[needle_u8_val[i]] = needle_u8_len - i;
		}
		e = haystack_u8_val + haystack_u8->len;
		p = haystack_u8_val;
		while (--offset >= 0) {
			if (p >= e) {
				result = -16;
				goto out;
			}
			p += u8_tbl[*p];
		}
		p += needle_u8_len;
		if (p > e) {
			goto out;
		}
		while (p <= e) {
			const unsigned char *pv = p;
			q = needle_u8_val + needle_u8_len;
			for (;;) {
				if (q == needle_u8_val) {
					result = 0;
					while (p > haystack_u8_val) {
						unsigned char c = *--p;
						if (c < 0x80) {
							++result;
						} else if ((c & 0xc0) != 0x80) {
							++result;
						}
					}
					goto out;
				}
				if (*--q != *--p) {
					break;
				}
			}
			p += jtbl[*p];
			if (p <= pv) {
				p = pv + 1;
			}
		}
	} else {
		unsigned int jtbl[1 << (sizeof(unsigned char) * 8)];
		unsigned int needle_u8_len = needle_u8->len, needle_len = 0;
		unsigned int i;
		const unsigned char *p, *e, *q, *qe;
		const unsigned char *haystack_u8_val = haystack_u8->val,
		                    *needle_u8_val = needle_u8->val;
		for (i = 0; i < sizeof(jtbl) / sizeof(*jtbl); ++i) {
			jtbl[i] = needle_u8_len;
		}
		for (i = needle_u8_len - 1; i > 0; --i) {
			unsigned char c = needle_u8_val[i];
			jtbl[c] = i;
			if (c < 0x80) {
				++needle_len;
			} else if ((c & 0xc0) != 0x80) {
				++needle_len;
			}
		}
		{
			unsigned char c = needle_u8_val[0];
			if (c < 0x80) {
				++needle_len;
			} else if ((c & 0xc0) != 0x80) {
				++needle_len;
			}
		}
		e = haystack_u8_val;
		p = e + haystack_u8->len;
		qe = needle_u8_val + needle_u8_len;
		if (offset < 0) {
			if (-offset > needle_len) {
				offset += needle_len;
				while (offset < 0) {
					unsigned char c;
					if (p <= e) {
						result = -16;
						goto out;
					}
					c = *(--p);
					if (c < 0x80) {
						++offset;
					} else if ((c & 0xc0) != 0x80) {
						++offset;
					}
				}
			}
		} else {
			const unsigned char *ee = haystack_u8_val + haystack_u8->len;
			while (--offset >= 0) {
				if (e >= ee) {
					result = -16;
					goto out;
				}
				e += u8_tbl[*e];
			}
		}
		if (p < e + needle_u8_len) {
			goto out;
		}
		p -= needle_u8_len;
		while (p >= e) {
			const unsigned char *pv = p;
			q = needle_u8_val;
			for (;;) {
				if (q == qe) {
					result = 0;
					p -= needle_u8_len;
					while (p > haystack_u8_val) {
						unsigned char c = *--p;
						if (c < 0x80) {
							++result;
						} else if ((c & 0xc0) != 0x80) {
							++result;
						}
					}
					goto out;
				}
				if (*q != *p) {
					break;
				}
				++p, ++q;
			}
			p -= jtbl[*p];
			if (p >= pv) {
				p = pv - 1;
			}
		}
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

int
mbfl_substr_count(
    mbfl_string *haystack,
    mbfl_string *needle
   )
{
	int n, result = 0;
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
	pc.start = 0;
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
			if (pc.matched_pos >= 0) {
				++result;
				pc.matched_pos = -1;
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
	const mbfl_encoding *encoding;
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
		if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
			start *= 2;
			end = start + length*2;
		} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
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
	const mbfl_encoding *encoding;
	mbfl_memory_device device;

	/* validate the parameters */
	if (string == NULL || string->val == NULL || result == NULL) {
		return NULL;
	}

	if (from < 0 || length < 0) {
		return NULL;
	}

	if (from >= string->len) {
		from = string->len;
	}

	encoding = mbfl_no2encoding(string->no_encoding);
	if (encoding == NULL) {
		return NULL;
	}

	mbfl_string_init(result);
	result->no_language = string->no_language;
	result->no_encoding = string->no_encoding;

	if ((encoding->flag & (MBFL_ENCTYPE_SBCS
				| MBFL_ENCTYPE_WCS2BE
				| MBFL_ENCTYPE_WCS2LE
				| MBFL_ENCTYPE_WCS4BE
				| MBFL_ENCTYPE_WCS4LE))
			|| encoding->mblen_table != NULL) {
		const unsigned char *start = NULL;
		const unsigned char *end = NULL;
		unsigned char *w;
		unsigned int sz;

		if (encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
			from &= -2;

			if (from + length >= string->len) {
				length = string->len - from;
			}

			start = string->val + from;
			end   = start + (length & -2);
		} else if (encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
			from &= -4;

			if (from + length >= string->len) {
				length = string->len - from;
			}

			start = string->val + from;
			end   = start + (length & -4);
		} else if ((encoding->flag & MBFL_ENCTYPE_SBCS)) {
			if (from + length >= string->len) {
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
			if ((start - string->val) + length >= (int)string->len) {
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
		if ((w = (unsigned char*)mbfl_calloc(sz + 8,
				sizeof(unsigned char))) == NULL) {
			return NULL;
		}

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
			int pos;
		} bk, _bk;

		/* output code filter */
		if (!(decoder = mbfl_convert_filter_new(
				mbfl_no_encoding_wchar,
				string->no_encoding,
				mbfl_memory_device_output, 0, &device))) {
			return NULL;
		}

		/* wchar filter */
		if (!(encoder = mbfl_convert_filter_new(
				string->no_encoding,
				mbfl_no_encoding_wchar,
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
		encoder->output_function = (int(*)(int,void *))decoder->filter_function;
		encoder->flush_function = (int(*)(void *))decoder->filter_flush;
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
				decoder->filter_dtor(decoder);
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
					bk.decoder.filter_dtor(&bk.decoder);
					bk.encoder.filter_dtor(&bk.encoder);

					p = _bk.p;
					device.pos = _bk.pos;
					decoder->filter_dtor(decoder);
					encoder->filter_dtor(encoder);
					mbfl_convert_filter_copy(&_bk.decoder, decoder);
					mbfl_convert_filter_copy(&_bk.encoder, encoder);
					bk = _bk;
				} else {
					_bk.decoder.filter_dtor(&_bk.decoder);
					_bk.encoder.filter_dtor(&_bk.encoder);

					p = bk.p;
					device.pos = bk.pos;
					decoder->filter_dtor(decoder);
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
				decoder->filter_dtor(decoder);
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
				_bk.decoder.filter_dtor(&_bk.decoder);
				_bk.encoder.filter_dtor(&_bk.encoder);

				/* restore filter */
				p = bk.p;
				device.pos = bk.pos;
				decoder->filter_dtor(decoder);
				encoder->filter_dtor(encoder);
				mbfl_convert_filter_copy(&bk.decoder, decoder);
				mbfl_convert_filter_copy(&bk.encoder, encoder);
				break;
			}

			bk.decoder.filter_dtor(&bk.decoder);
			bk.encoder.filter_dtor(&bk.encoder);

			p = _bk.p;
			device.pos = _bk.pos;
			decoder->filter_dtor(decoder);
			encoder->filter_dtor(encoder);
			mbfl_convert_filter_copy(&_bk.decoder, decoder);
			mbfl_convert_filter_copy(&_bk.encoder, encoder);

			bk = _bk;
		}

		(*encoder->filter_flush)(encoder);

		bk.decoder.filter_dtor(&bk.decoder);
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
static int is_fullwidth(int c)
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
	(*(int *)data) += (is_fullwidth(c) ? 2: 1);
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
			n--;
			if ((*encoder->filter_function)(*p++, encoder) < 0) {
				break;
			}
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

mbfl_string *
mbfl_ja_jp_hantozen(
    mbfl_string *string,
    mbfl_string *result,
    int mode)
{
	int n;
	unsigned char *p;
	const mbfl_encoding *encoding;
	mbfl_memory_device device;
	mbfl_convert_filter *decoder = NULL;
	mbfl_convert_filter *encoder = NULL;
	mbfl_convert_filter *tl_filter = NULL;
	mbfl_convert_filter *next_filter = NULL;
	mbfl_filt_tl_jisx0201_jisx0208_param *param = NULL;

	/* validate parameters */
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
	if (decoder == NULL) {
		goto out;
	}
	next_filter = decoder;

	param =
		(mbfl_filt_tl_jisx0201_jisx0208_param *)mbfl_malloc(sizeof(mbfl_filt_tl_jisx0201_jisx0208_param));
	if (param == NULL) {
		goto out;
	}

	param->mode = mode;

	tl_filter = mbfl_convert_filter_new2(
		&vtbl_tl_jisx0201_jisx0208,
		(int(*)(int, void*))next_filter->filter_function,
		(int(*)(void*))next_filter->filter_flush,
		next_filter);
	if (tl_filter == NULL) {
		mbfl_free(param);
		goto out;
	}

	tl_filter->opaque = param;
	next_filter = tl_filter;

	encoder = mbfl_convert_filter_new(
		string->no_encoding,
		mbfl_no_encoding_wchar,
		(int(*)(int, void*))next_filter->filter_function,
		(int(*)(void*))next_filter->filter_flush,
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
			mbfl_free(tl_filter->opaque);
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

	int n;
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
    enum mbfl_no_encoding incode,
    enum mbfl_no_encoding outcode,
    enum mbfl_no_encoding transenc)
{
	int n;
	const char *s;
	const mbfl_encoding *outencoding;
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
	if (type == 0) { /* decimal output */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_encode_htmlnumericentity, 0, &pc);
	} else if (type == 2) { /* hex output */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_encode_hex_htmlnumericentity, 0, &pc);
	} else { /* type == 1: decimal/hex input */
		encoder = mbfl_convert_filter_new(
		    string->no_encoding,
		    mbfl_no_encoding_wchar,
		    collector_decode_htmlnumericentity,
			(int (*)(void*))mbfl_filt_decode_htmlnumericentity_flush, &pc);
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
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
