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
   | Authors: Johannes Schlüter <johanes@php.net>                         |
   |          Stanislav Malyshev <stas@php.net>                           |
   +----------------------------------------------------------------------+
 */

#include <main/php.h>
#include <main/php_main.h>
#include <main/SAPI.h>
#include <ext/standard/info.h>
#include <ext/standard/php_var.h>
#include <main/php_variables.h>

#include "fuzzer.h"
#include "fuzzer-sapi.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	if (Size > 6 * 1024) {
		/* Large inputs have a large impact on fuzzer performance and may cause stack overflows,
		 * but are unlikely to be necessary to reach new codepaths. */
		return 0;
	}

	fuzzer_do_request_from_buffer(
		"fuzzer.php", (const char *) Data, Size, /* execute */ 0, /* before_shutdown */ NULL);

	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	/* Compilation will often trigger fatal errors.
	 * Use tracked allocation mode to avoid leaks in that case. */
	putenv("USE_TRACKED_ALLOC=1");

	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}
