/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */


#include "fuzzer.h"

#include "Zend/zend.h"
#include <main/php_config.h>
#include "main/php_main.h"
#include "oniguruma.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
#ifdef HAVE_MBREGEX

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	fuzzer_setup_dummy_frame();

	/* Pass the input with its length, as strlen() would cut it off at the first NUL byte. */
	zval args[2];
	ZVAL_STRINGL(&args[0], (const char *) Data, Size);
	ZVAL_STRING(&args[1], "test123");
	fuzzer_call_php_func_zval("mb_ereg", 2, args);
	fuzzer_call_php_func_zval("mb_eregi", 2, args);
	zval_ptr_dtor(&args[1]);

	ZVAL_COPY(&args[1], &args[0]);
	fuzzer_call_php_func_zval("mb_ereg", 2, args);
	fuzzer_call_php_func_zval("mb_eregi", 2, args);
	zval_ptr_dtor(&args[0]);
	zval_ptr_dtor(&args[1]);

	fuzzer_request_shutdown();
#else
	fprintf(stderr, "\n\nERROR:\nPHP built without mbstring, recompile with --enable-mbstring to use this fuzzer\n");
	exit(1);
#endif
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* The default parse depth limit allows stack overflows under asan. */
	onig_set_parse_depth_limit(512);

	/* fuzzer_shutdown_php(); */
	return 0;
}
