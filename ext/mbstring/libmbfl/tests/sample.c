/**
 * this is a small sample script to use libmbfl.
 * Rui Hirokawa <hirokawa@php.net>
 *
 * this file is encoded in EUC-JP.
 */

#include <stdio.h>
#include "../mbfl/mbfilter.h"

int main()
{
	enum mbfl_no_encoding from_encoding, to_encoding;
	enum mbfl_no_language no_language;
	mbfl_buffer_converter *convd = NULL;
	mbfl_string string, result, *ret;
	mbfl_language lang;
	unsigned char input[] = "日本語文字列"; /* EUC-JP kanji string */

	no_language = mbfl_name2no_language("Japanese");
	from_encoding = mbfl_name2no_encoding("EUC-JP");
	to_encoding = mbfl_name2no_encoding("SJIS");

	mbfl_string_init(&string);
	mbfl_string_init(&result);

	string.no_encoding = from_encoding;
	string.no_language = no_language;
	string.val = (unsigned char *)input;
	string.len = strlen(input);

	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
	mbfl_buffer_converter_delete(convd);

	puts(ret->val);
	return 1;
}
