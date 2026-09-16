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
   | Authors: Johannes Schlüter <johanes@php.net>                         |
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

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	unsigned char *orig_data = malloc(Size+1);
	memcpy(orig_data, Data, Size);
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
