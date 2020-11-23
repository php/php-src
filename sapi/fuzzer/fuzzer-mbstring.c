/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */


#include "fuzzer.h"

#include "Zend/zend.h"
#include "main/php_config.h"
#include "main/php_main.h"
#include "oniguruma.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
#ifdef HAVE_MBREGEX
	char *args[2];
	char *data = malloc(Size+1);
	memcpy(data, Data, Size);
	data[Size] = '\0';

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	fuzzer_setup_dummy_frame();

	args[0] = data;
	args[1] = "test123";
	fuzzer_call_php_func("mb_ereg", 2, args);

	args[0] = data;
	args[1] = "test123";
	fuzzer_call_php_func("mb_eregi", 2, args);

	args[0] = data;
	args[1] = data;
	fuzzer_call_php_func("mb_ereg", 2, args);

	args[0] = data;
	args[1] = data;
	fuzzer_call_php_func("mb_eregi", 2, args);

	fuzzer_request_shutdown();

	free(data);
#else
	fprintf(stderr, "\n\nERROR:\nPHP built without mbstring, recompile with --enable-mbstring to use this fuzzer\n");
	exit(1);
#endif
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php();

	/* The default parse depth limit allows stack overflows under asan. */
	onig_set_parse_depth_limit(512);

	/* fuzzer_shutdown_php(); */
	return 0;
}
