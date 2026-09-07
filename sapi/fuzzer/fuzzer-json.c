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
   |          Stanislav Malyshev <stas@php.net>                           |
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
#include "ext/json/php_json_parser.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

	if (fuzzer_request_startup() == FAILURE){
		return 0;
	}

	char *data = malloc(Size + 1);
	memcpy(data, Data, Size);
	data[Size] = '\0';

	for (int option = 0; option <=1; ++option) {
		zval result;
		php_json_parser parser;
		php_json_parser_init(&parser, &result, data, Size, option, 10);
		if (php_json_yyparse(&parser) == SUCCESS) {
			zval_ptr_dtor(&result);
		}
	}

	php_request_shutdown(NULL);

	free(data);
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}
