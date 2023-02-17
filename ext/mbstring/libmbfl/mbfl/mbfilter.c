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
#include "filters/mbfilter_utf8.h"

#include "rare_cp_bitvec.h"

/*
 * encoding detector
 */
static int mbfl_estimate_encoding_likelihood(int input_cp, void *void_data)
{
	mbfl_encoding_detector_data *data = void_data;
	unsigned int c = input_cp;

	/* Receive wchars decoded from input string using candidate encoding.
	 * If the string was invalid in the candidate encoding, we assume
	 * it's the wrong one. Otherwise, give the candidate many 'demerits'
	 * for each 'rare' codepoint found, a smaller number for each ASCII
	 * punctuation character, and 1 for all other codepoints.
	 *
	 * The 'common' codepoints should cover the vast majority of
	 * codepoints we are likely to see in practice, while only covering
	 * a small minority of the entire Unicode encoding space. Why?
	 * Well, if the test string happens to be valid in an incorrect
	 * candidate encoding, the bogus codepoints which it decodes to will
	 * be more or less random. By treating the majority of codepoints as
	 * 'rare', we ensure that in almost all such cases, the bogus
	 * codepoints will include plenty of 'rares', thus giving the
	 * incorrect candidate encoding lots of demerits. See
	 * common_codepoints.txt for the actual list used.
	 *
	 * So, why give extra demerits for ASCII punctuation characters? It's
	 * because there are some text encodings, like UTF-7, HZ, and ISO-2022,
	 * which deliberately only use bytes in the ASCII range. When
	 * misinterpreted as ASCII/UTF-8, strings in these encodings will
	 * have an unusually high number of ASCII punctuation characters.
	 * So giving extra demerits for such characters will improve
	 * detection accuracy for UTF-7 and similar encodings.
	 *
	 * Finally, why 1 demerit for all other characters? That penalizes
	 * long strings, meaning we will tend to choose a candidate encoding
	 * in which the test string decodes to a smaller number of
	 * codepoints. That prevents single-byte encodings in which almost
	 * every possible input byte decodes to a 'common' codepoint from
	 * being favored too much. */
	if (c == MBFL_BAD_INPUT) {
		data->num_illegalchars++;
	} else if (c > 0xFFFF) {
		data->score += 40;
	} else if (c >= 0x21 && c <= 0x2F) {
		data->score += 6;
	} else if ((rare_codepoint_bitvec[c >> 5] >> (c & 0x1F)) & 1) {
		data->score += 30;
	} else {
		data->score += 1;
	}
	return 0;
}

mbfl_encoding_detector *mbfl_encoding_detector_new(const mbfl_encoding **elist, int elistsz, int strict)
{
	if (!elistsz) {
		return NULL;
	}

	mbfl_encoding_detector *identd = emalloc(sizeof(mbfl_encoding_detector));
	identd->filter_list = ecalloc(elistsz, sizeof(mbfl_convert_filter*));
	identd->filter_data = ecalloc(elistsz, sizeof(mbfl_encoding_detector_data));

	int filter_list_size = 0;
	for (int i = 0; i < elistsz; i++) {
		mbfl_convert_filter *filter = mbfl_convert_filter_new(elist[i], &mbfl_encoding_wchar,
			mbfl_estimate_encoding_likelihood, NULL, &identd->filter_data[filter_list_size]);
		if (filter) {
			identd->filter_list[filter_list_size++] = filter;
		}
	}
	identd->filter_list_size = filter_list_size;
	identd->strict = strict;
	return identd;
}

void mbfl_encoding_detector_delete(mbfl_encoding_detector *identd)
{
	for (int i = 0; i < identd->filter_list_size; i++) {
		mbfl_convert_filter_delete(identd->filter_list[i]);
	}
	efree(identd->filter_list);
	efree(identd->filter_data);
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
			mbfl_encoding_detector_data *data = &identd->filter_data[i];
			if (!data->num_illegalchars) {
				(*filter->filter_function)(*p, filter);
				if (data->num_illegalchars) {
					bad++;
				}
			}
		}
		if ((num - 1) <= bad && !identd->strict) {
			return 1;
		}
		p++;
	}

	for (int i = 0; i < num; i++) {
		mbfl_convert_filter *filter = identd->filter_list[i];
		(filter->filter_flush)(filter);
	}

	return 0;
}

const mbfl_encoding *mbfl_encoding_detector_judge(mbfl_encoding_detector *identd)
{
	size_t best_score = SIZE_MAX; /* Low score is 'better' */
	const mbfl_encoding *enc = NULL;

	for (int i = 0; i < identd->filter_list_size; i++) {
		mbfl_convert_filter *filter = identd->filter_list[i];
		mbfl_encoding_detector_data *data = &identd->filter_data[i];
		if (!data->num_illegalchars && data->score < best_score) {
			enc = filter->from;
			best_score = data->score;
		}
	}

	return enc;
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

		decoder->illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
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
