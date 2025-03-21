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

bool char_to_zend_long(const char *c, size_t scale_len, zend_long *ret) {
	*ret = 0;
	zend_long old_ret = 0;
	for (size_t i = 0; i < scale_len; i++) {
		if (*c >= '0' && *c <= '9') {
			*ret *= 10;
			*ret += *c - '0';
		}
		if (*ret > old_ret) {
			old_ret = *ret;
		} else {
			return false;
		}
	}
	return true;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	/* num1,num2,scale */
	const uint8_t *comma1 = memchr(Data, ',', Size);
	if (!comma1) {
		return 0;
	}

	size_t dividend_len = comma1 - Data;
	char *dividend_str = estrndup((char *) Data, dividend_len);
	Data = comma1 + 1;
	Size -= dividend_len + 1;

	const uint8_t *comma2 = memchr(Data, ',', Size);
	if (!comma2) {
		efree(dividend_str);
		return 0;
	}

	size_t divisor_len = comma2 - Data;
	char *divisor_str = estrndup((char *) Data, divisor_len);
	Data = comma2 + 1;
	Size -= divisor_len + 1;

	zend_long scale = 0;
	if (!char_to_zend_long((char *) Data, Size, &scale)) {
		efree(dividend_str);
		efree(divisor_str);
		return 0;
	}

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	fuzzer_setup_dummy_frame();

	zval args[3];
	ZVAL_STRINGL(&args[0], dividend_str, dividend_len);
	ZVAL_STRINGL(&args[1], divisor_str, divisor_len);
	ZVAL_LONG(&args[2], scale);

	fuzzer_call_php_func_zval("bcdiv", 3, args);

	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&args[1]);
	efree(dividend_str);
	efree(divisor_str);

	fuzzer_request_shutdown();

	return 0;
}

#define BUF_SIZE 128

static inline bool rand_bool() {
	return rand() & 1;
}

static inline size_t generate_random_num_fraction(char *buf, size_t len) {
	int zeros = rand() % 10;
	for (int i = 0; i < zeros; i++) {
		buf[len] = '0';
		len++;
	}
	len += snprintf(buf + len, BUF_SIZE - len, "%ld", random());
	return len;
}

static inline size_t generate_random_num(char *buf, size_t len) {
	if (rand_bool()) {
		/* num < 1 */
		buf[len] = '0';
		buf[len + 1]= '.';
		len += 2;
		/* fraction */
		len = generate_random_num_fraction(buf, len);
	} else {
		/* integer */
		len += snprintf(buf + len, BUF_SIZE - len, "%ld", random());
		if (rand_bool()) {
			/* fraction */
			buf[len] = '.';
			len++;
			len = generate_random_num_fraction(buf, len);
		}
	}

	return len;
}

size_t LLVMFuzzerCustomMutator(uint8_t *Data, size_t Size, size_t MaxSize, unsigned int Seed) {
	char buf[BUF_SIZE];
	size_t len = 0;

	/*  num1 */
	len = generate_random_num(buf, len);
	buf[len] = ',';
	len++;

	/* num2 */
	len = generate_random_num(buf, len);
	buf[len] = ',';
	len++;

	/* scale */
	len += snprintf(buf + len, BUF_SIZE - len, "%d", rand() % 10);

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
