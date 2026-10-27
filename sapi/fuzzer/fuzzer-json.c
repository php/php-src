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
#include "ext/json/php_json.h"
#include "ext/json/php_json_parser.h"

/* Lower than the default depth of 512, so that deeply nested inputs fail quickly and the
 * depth limit is reached with small inputs. */
#define MAX_DEPTH 10

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

	if (fuzzer_request_startup() == FAILURE){
		return 0;
	}

	char *data = malloc(Size + 1);
	memcpy(data, Data, Size);
	data[Size] = '\0';

	int options[12] = {
		0,
		PHP_JSON_OBJECT_AS_ARRAY,
		PHP_JSON_OBJECT_AS_ARRAY | PHP_JSON_BIGINT_AS_STRING,
		PHP_JSON_OBJECT_AS_ARRAY | PHP_JSON_BIGINT_AS_STRING | PHP_JSON_INVALID_UTF8_IGNORE,
		PHP_JSON_OBJECT_AS_ARRAY | PHP_JSON_BIGINT_AS_STRING | PHP_JSON_INVALID_UTF8_SUBSTITUTE,
		PHP_JSON_OBJECT_AS_ARRAY | PHP_JSON_INVALID_UTF8_IGNORE,
		PHP_JSON_OBJECT_AS_ARRAY | PHP_JSON_INVALID_UTF8_SUBSTITUTE,
		PHP_JSON_BIGINT_AS_STRING,
		PHP_JSON_BIGINT_AS_STRING | PHP_JSON_INVALID_UTF8_IGNORE,
		PHP_JSON_BIGINT_AS_STRING | PHP_JSON_INVALID_UTF8_SUBSTITUTE,
		PHP_JSON_INVALID_UTF8_IGNORE,
		PHP_JSON_INVALID_UTF8_SUBSTITUTE
	};

	for (int index = 0; index < 12; ++index) {
		zval result;
		php_json_parser parser;
		php_json_parser_init(&parser, &result, data, Size, options[index], MAX_DEPTH);
		bool decoded = php_json_yyparse(&parser) == SUCCESS;
		if (decoded) {
			zval_ptr_dtor(&result);
		}

		/* json_validate() must agree with json_decode(). Only compare when decoding to arrays,
		 * as decoding to objects additionally rejects property names starting with a NUL byte. */
		if (options[index] == PHP_JSON_OBJECT_AS_ARRAY
				|| options[index] == (PHP_JSON_OBJECT_AS_ARRAY | PHP_JSON_INVALID_UTF8_IGNORE)) {
			bool valid = php_json_validate_ex(data, Size,
				options[index] & PHP_JSON_INVALID_UTF8_IGNORE, MAX_DEPTH);
			ZEND_ASSERT(valid == decoded);
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
