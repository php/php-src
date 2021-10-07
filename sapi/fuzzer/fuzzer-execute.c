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
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
 */

#include "fuzzer-execute-common.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	if (Size > MAX_SIZE) {
		/* Large inputs have a large impact on fuzzer performance,
		 * but are unlikely to be necessary to reach new codepaths. */
		return 0;
	}

	steps_left = MAX_STEPS;
	fuzzer_do_request_from_buffer(
		FILE_NAME, (const char *) Data, Size, /* execute */ 1, /* before_shutdown */ NULL);

	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php_for_execute(NULL);
	return 0;
}
