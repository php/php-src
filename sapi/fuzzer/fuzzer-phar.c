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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fuzzer-sapi.h"

int phar_create_or_parse_filename(char *fname, size_t fname_len, char *alias, size_t alias_len, zend_bool is_data, uint32_t options, void** pphar, char **error);
void phar_request_initialize(void);

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	void *phar_data;
	char *tmp_filename;
	int tmpfd;

	if (Size > 256 * 1024) {
		/* Large inputs have a large impact on fuzzer performance,
		 * but are unlikely to be necessary to reach new codepaths. */
		return 0;
	}

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}
	phar_request_initialize();

	tmp_filename = estrdup("/tmp/phar-fuzz.temp.XXXXXX");
	tmpfd = mkstemp(tmp_filename);
	write(tmpfd, Data, Size);
	close(tmpfd);

	zend_first_try {
		phar_create_or_parse_filename(tmp_filename, strlen(tmp_filename), NULL, 0, 1, REPORT_ERRORS, &phar_data, NULL);
	} zend_end_try();

	/* cleanup */
	unlink(tmp_filename);
	efree(tmp_filename);
	php_request_shutdown(NULL);

	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	/* Gracefully handle bailouts. */
	putenv("USE_TRACKED_ALLOC=1");

	fuzzer_init_php();

	/* fuzzer_shutdown_php(); */
	return 0;
}

