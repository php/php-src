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
   | Authors: Johannes Schlüter <johanes@php.net>                         |
   +----------------------------------------------------------------------+
 */


#include "fuzzer.h"

#include "Zend/zend.h"
#include "main/php_config.h"
#include "main/php_main.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

#include "ext/standard/php_var.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	unsigned char *orig_data = malloc(Size+1);
	zend_execute_data execute_data;
	zend_function func;

	memcpy(orig_data, Data, Size);
	orig_data[Size] = '\0';

	if (fuzzer_request_startup()==FAILURE) {
		return 0;
	}

	/* Set up a dummy stack frame so that exceptions may be thrown. */
	{
		memset(&execute_data, 0, sizeof(zend_execute_data));
		memset(&func, 0, sizeof(zend_function));

		func.type = ZEND_INTERNAL_FUNCTION;
		func.common.function_name = ZSTR_EMPTY_ALLOC();
		execute_data.func = &func;
		EG(current_execute_data) = &execute_data;
	}

	{
		const unsigned char *data = orig_data;
		zval result;
		ZVAL_UNDEF(&result);

		php_unserialize_data_t var_hash;
		PHP_VAR_UNSERIALIZE_INIT(var_hash);
		php_var_unserialize(&result, (const unsigned char **) &data, data + Size, &var_hash);
		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

		zval_ptr_dtor(&result);

		/* Destroy any thrown exception. */
		if (EG(exception)) {
			zend_object_release(EG(exception));
			EG(exception) = NULL;
		}
	}

	/* Unserialize may create circular structure. Make sure we free them.
	 * Two calls are performed to handle objects with destructors. */
	zend_gc_collect_cycles();
	zend_gc_collect_cycles();
	php_request_shutdown(NULL);

	free(orig_data);

	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php();

	/* fuzzer_shutdown_php(); */
	return 0;
}
