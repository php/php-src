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
#include "ext/standard/php_var.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fuzzer-sapi.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
#ifdef HAVE_EXIF
	php_stream *stream;
	zval args[4];

	if (Size > 256 * 1024) {
		/* Large inputs have a large impact on fuzzer performance,
		 * but are unlikely to be necessary to reach new codepaths. */
		return 0;
	}

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	stream = php_stream_memory_create(TEMP_STREAM_DEFAULT);
	php_stream_write(stream, (const char *) Data, Size);
	php_stream_to_zval(stream, &args[0]);

	/* exif_read_data($stream, null, false, true): the thumbnail is only read if requested. */
	ZVAL_NULL(&args[1]);
	ZVAL_FALSE(&args[2]);
	ZVAL_TRUE(&args[3]);
	fuzzer_call_php_func_zval("exif_read_data", 4, args);

	/* exif_thumbnail($stream, $width, $height, $type): the thumbnail is only scanned for its
	 * dimensions if they are requested. */
	for (int i = 1; i < 4; i++) {
		ZVAL_NEW_EMPTY_REF(&args[i]);
		ZVAL_NULL(Z_REFVAL(args[i]));
	}
	fuzzer_call_php_func_zval("exif_thumbnail", 4, args);
	for (int i = 0; i < 4; i++) {
		zval_ptr_dtor(&args[i]);
	}

	/* cleanup */
	php_request_shutdown(NULL);

	return 0;
#else
	fprintf(stderr, "\n\nERROR:\nPHP built without EXIF, recompile with --enable-exif to use this fuzzer\n");
	exit(1);
#endif
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}

