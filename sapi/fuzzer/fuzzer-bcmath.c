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
	const char *n1ptr = (char *) Data;
	const char *comma1 = memchr(n1ptr, ',', Size);
	if (!comma1) {
		return 0;
	}
	size_t n1len = comma1 - n1ptr;
	Size -= n1len + 1;

	const char *n2ptr = comma1 + 1;
	const char *comma2 = memchr(n2ptr, ',', Size);
	if (!comma2) {
		return 0;
	}
	size_t n2len = comma2 - n2ptr;
	Size -= n2len + 1;

	zend_long scale = 0;
	if (!char_to_zend_long((char *) comma2 + 1, Size, &scale)) {
		return 0;
	}

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	char func_name[6];
	switch (rand() % 6) {
		case 0:
			sprintf(func_name, "%s", "bcadd");
			break;
		case 1:
			sprintf(func_name, "%s", "bcsub");
			break;
		case 2:
			sprintf(func_name, "%s", "bcmul");
			break;
		case 3:
			sprintf(func_name, "%s", "bcdiv");
			break;
		case 4:
			sprintf(func_name, "%s", "bcmod");
			break;
		case 5:
			sprintf(func_name, "%s", "bcpow");
			break;
	}

	fuzzer_setup_dummy_frame();

	zval args[3];
	ZVAL_STRINGL(&args[0], n1ptr, n1len);
	ZVAL_STRINGL(&args[1], n2ptr, n2len);
	ZVAL_LONG(&args[2], scale);

	fuzzer_call_php_func_zval(func_name, 3, args);

	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&args[1]);

	fuzzer_request_shutdown();

	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}
