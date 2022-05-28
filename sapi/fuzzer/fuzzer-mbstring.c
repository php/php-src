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

	if (!ToEncoding || !FromEncoding || fuzzer_request_startup() == FAILURE) {
		efree(ToEncodingName);
		efree(FromEncodingName);
		return 0;
	}

	zend_string *Result = php_mb_convert_encoding_ex((char *) Data, Size, ToEncoding, FromEncoding);
	zend_string_release(Result);
	efree(ToEncodingName);
	efree(FromEncodingName);

	fuzzer_request_shutdown();
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);
	return 0;
}
