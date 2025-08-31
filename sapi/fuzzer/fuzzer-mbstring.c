/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */


#include "zend.h"
#include "fuzzer.h"
#include "fuzzer-sapi.h"
#include "ext/mbstring/mbstring.h"

zend_string* convert_encoding(const uint8_t *Data, size_t Size, const mbfl_encoding *FromEncoding, const mbfl_encoding *ToEncoding, size_t BufSize, unsigned int *NumErrors)
{
	uint32_t *wchar_buf = ecalloc(BufSize, sizeof(uint32_t));
	unsigned int state = 0;

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, Size, '?', MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR);

	while (Size) {
		size_t out_len = FromEncoding->to_wchar((unsigned char**)&Data, &Size, wchar_buf, BufSize, &state);
		ZEND_ASSERT(out_len <= BufSize);
		ToEncoding->from_wchar(wchar_buf, out_len, &buf, !Size);
	}

	*NumErrors = buf.errors;
	zend_string *result = mb_convert_buf_result(&buf, ToEncoding);
	efree(wchar_buf);
	return result;
}

void assert_zend_string_eql(zend_string *str1, zend_string *str2)
{
	ZEND_ASSERT(ZSTR_LEN(str1) == ZSTR_LEN(str2));
	for (int i = 0; i < ZSTR_LEN(str1); i++) {
		ZEND_ASSERT(ZSTR_VAL(str1)[i] == ZSTR_VAL(str2)[i]);
	}
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	const uint8_t *Comma1 = memchr(Data, ',', Size);
	if (!Comma1) {
		return 0;
	}

	size_t ToEncodingNameLen = Comma1 - Data;
	char *ToEncodingName = estrndup((char *) Data, ToEncodingNameLen);
	Data = Comma1 + 1;
	Size -= ToEncodingNameLen + 1;

	const uint8_t *Comma2 = memchr(Data, ',', Size);
	if (!Comma2) {
		efree(ToEncodingName);
		return 0;
	}

	size_t FromEncodingNameLen = Comma2 - Data;
	char *FromEncodingName = estrndup((char *) Data, FromEncodingNameLen);
	Data = Comma2 + 1;
	Size -= FromEncodingNameLen + 1;

	const mbfl_encoding *ToEncoding = mbfl_name2encoding(ToEncodingName);
	const mbfl_encoding *FromEncoding = mbfl_name2encoding(FromEncodingName);

	if (!ToEncoding || !FromEncoding || Size < 2 || fuzzer_request_startup() == FAILURE) {
		efree(ToEncodingName);
		efree(FromEncodingName);
		return 0;
	}

	/* Rather than converting an entire (possibly very long) string at once, mbstring converts
	 * strings 'chunk by chunk'; the decoder will run until it fills up its output buffer with
	 * wchars, then the encoder will process those wchars, then the decoder runs again until it
	 * again fills up its output buffer, and so on
	 *
	 * The most error-prone part of the decoder/encoder code is where we exit a decoder/encoder
	 * function and save its state to allow later resumption
	 * To stress-test that aspect of the decoders/encoders, try performing an encoding conversion
	 * operation with different, random buffer sizes
	 * If the code is correct, the result should always be the same either way */
	size_t bufsize1 = *Data++;
	size_t bufsize2 = *Data++;
	bufsize1 = MAX(bufsize1, MBSTRING_MIN_WCHAR_BUFSIZE);
	bufsize2 = MAX(bufsize2, MBSTRING_MIN_WCHAR_BUFSIZE);
	Size -= 2;

	unsigned int errors1 = 0, errors2 = 0;

	zend_string *Result1 = convert_encoding(Data, Size, FromEncoding, ToEncoding, bufsize1, &errors1);
	zend_string *Result2 = convert_encoding(Data, Size, FromEncoding, ToEncoding, bufsize2, &errors2);

	assert_zend_string_eql(Result1, Result2);
	ZEND_ASSERT(errors1 == errors2);

	/* For some text encodings, we have specialized validation functions. These should always be
	 * stricter than the conversion functions; if the conversion function receives invalid input
	 * and emits an error marker (MBFL_BAD_INPUT), then the validation function should always
	 * return false. However, if the conversion function does not emit any error marker, it may
	 * still happen in some cases that the validation function returns false. */
	if (FromEncoding->check != NULL) {
		bool good = FromEncoding->check((unsigned char*)Data, Size);
		if (errors1 > 0) {
			/* If the conversion function emits an error marker, that may or may not mean the input
			 * was invalid; it could also be that the input was valid, but it contains codepoints
			 * which cannot be represented in the output encoding.
			 * To confirm if that is the case, try converting to UTF-8, which can represent any
			 * Unicode codepoint. */
			unsigned int errors3 = 0;
			zend_string *Temp = convert_encoding(Data, Size, FromEncoding, &mbfl_encoding_utf8, 128, &errors3);
			if (errors3 > 0) {
				ZEND_ASSERT(!good);
			}
			zend_string_release(Temp);
		}
	}

	zend_string_release(Result1);
	zend_string_release(Result2);
	efree(ToEncodingName);
	efree(FromEncodingName);

	fuzzer_request_shutdown();
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);
	return 0;
}
