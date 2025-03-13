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
   | Authors: Saki Takamachi <saki@php.net>                               |
   +----------------------------------------------------------------------+
 */



#include "fuzzer.h"

#include "Zend/zend.h"
#include <main/php_config.h>
#include "main/php_main.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

zend_long char_to_size_t(char *c) {
	zend_long ret = 0;
	if (*c >= '0' && *c <= '9') {
		ret *= 10;
		ret += *c - '0';
	}
	return ret;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	/* num1,num2,scale */
	const uint8_t *Comma1 = memchr(Data, ',', Size);
	if (!Comma1) {
		return 0;
	}

	size_t dividend_len = Comma1 - Data;
	char *dividend_str = estrndup((char *) Data, dividend_len);
	Data = Comma1 + 1;
	Size -= dividend_len + 1;

	const uint8_t *Comma2 = memchr(Data, ',', Size);
	if (!Comma2) {
		efree(dividend_str);
		return 0;
	}

	size_t divisor_len = Comma2 - Data;
	char *divisor_str = estrndup((char *) Data, divisor_len);
	Data = Comma2 + 1;
	Size -= divisor_len + 1;

	char *scale_str = malloc(Size + 1);
	memcpy(scale_str, Data, Size);
	scale_str[Size] = '\0';

	zend_long scale = char_to_size_t(scale_str);
	free(scale_str);

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	fuzzer_setup_dummy_frame();

	zval result;
	ZVAL_UNDEF(&result);

	zval args[4];
	ZVAL_COPY_VALUE(&args[0], &result);
	ZVAL_STRINGL(&args[1], dividend_str, dividend_len);
	ZVAL_STRINGL(&args[2], divisor_str, divisor_len);
	ZVAL_LONG(&args[3], scale);

	fuzzer_call_php_func_zval("bcdiv", 4, args);

	zval_ptr_dtor(&result);
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[2]);
	efree(dividend_str);
	efree(divisor_str);

	fuzzer_request_shutdown();

	return 0;
}

size_t LLVMFuzzerCustomMutator(uint8_t *Data, size_t Size, size_t MaxSize, unsigned int Seed) {
	char buf[64];
	size_t len = 0;

	/*  num1 integer */
	len += snprintf(buf + len, sizeof(buf) - len, "%ld", random());
	if (rand() & 1) {
		/* num1 fraction */
		buf[len] = '.';
		len++;
		len += snprintf(buf + len, sizeof(buf) - len, "%ld", random());
	}

	buf[len] = ',';
	len++;
	/* num2 integer */
	len += snprintf(buf + len, sizeof(buf) - len, "%ld", random());
	if (rand() & 1) {
		/* num2 fraction */
		buf[len] = '.';
		len++;
		len += snprintf(buf + len, sizeof(buf) - len, "%ld", random());
	}

	buf[len] = ',';
	len++;
	/* scale */
	len += snprintf(buf + len, sizeof(buf) - len, "%d", rand() % 10);

	if (len > MaxSize) {
		return 0;
	}
	memcpy(Data, buf, len);
	return len;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}
