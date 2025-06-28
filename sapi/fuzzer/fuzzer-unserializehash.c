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
 */


#include "fuzzer.h"

#include "Zend/zend.h"
#include <main/php_config.h>
#include "main/php_main.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

#include "ext/standard/php_var.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t FullSize) {
	const uint8_t *Start = memchr(Data, '|', FullSize);
	if (!Start) {
		return 0;
	}
	++Start;

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	size_t Size = (Data + FullSize) - Start;
	unsigned char *orig_data = malloc(Size+1);
	memcpy(orig_data, Start, Size);
	orig_data[Size] = '\0';

	fuzzer_setup_dummy_frame();

	{
		const unsigned char *data = orig_data;
		zval result;
		ZVAL_UNDEF(&result);

		php_unserialize_data_t var_hash;
		PHP_VAR_UNSERIALIZE_INIT(var_hash);
		php_var_unserialize(&result, (const unsigned char **) &data, data + Size, &var_hash);
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

		if (Z_TYPE(result) == IS_OBJECT
			&& zend_string_equals_literal(Z_OBJCE(result)->name, "HashContext")) {
			zval args[2];
			ZVAL_COPY_VALUE(&args[0], &result);
			ZVAL_STRINGL(&args[1], (char *) Data, (Start - Data) - 1);
			fuzzer_call_php_func_zval("hash_update", 2, args);
			zval_ptr_dtor(&args[1]);
			fuzzer_call_php_func_zval("hash_final", 1, args);
		}

		zval_ptr_dtor(&result);
	}

	free(orig_data);

	fuzzer_request_shutdown();
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}
