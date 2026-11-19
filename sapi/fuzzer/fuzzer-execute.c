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
	char ini_buf[512];
	snprintf(ini_buf, sizeof(ini_buf),
		"opcache.enable=0\n");

	/* Hint: If opcache is ever enabled, opcache.validate_timestamps=0 needs to
	 * be set. Otherwise, obtaining the timestamp will fail and the script will
	 * not be cached. */

	fuzzer_init_php_for_execute(ini_buf);
	return 0;
}
