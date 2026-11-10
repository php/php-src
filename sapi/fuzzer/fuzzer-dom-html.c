/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   | (drafted by the oss-fuzz-coverage-pipeline run 2026-11-09)           |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   +----------------------------------------------------------------------+
   | Authors: arthurscchan <github@arthurscchan>                          |
   +----------------------------------------------------------------------+
*/

#include "fuzzer.h"

#include "main/php.h"
#include <main/php_config.h>
#include "main/php_main.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "fuzzer-sapi.h"

/* Large documents mostly slow down fuzzing without reaching new codepaths. */
#define MAX_SIZE (1 * 1024 * 1024)

/* createFromString()'s $options accept HTML_PARSE_* bits (LIBXML_NOERROR is
 * 1<<5 = 32 in libxml2); 0 is the neutral default and HTML5 parsing is
 * error-tolerant, so no suppression bit is needed. */
#define FUZZER_HTML_OPTIONS 0

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	if (Size > MAX_SIZE || Size < 2) {
		return 0;
	}

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	fuzzer_setup_dummy_frame();

	zend_string *class_name = zend_string_init(ZEND_STRL("Dom\\HTMLDocument"), 0);
	zend_class_entry *ce = zend_lookup_class(class_name);
	zend_string_release(class_name);

	if (ce) {
		/* Method table entries are stored lowercased; lookups are case-sensitive. */
		zend_function *create = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("createfromstring"));
		zend_function *save = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("savehtml"));
		ZEND_ASSERT(create != NULL);
		ZEND_ASSERT(save != NULL);

		zval retval, args[3];
		ZVAL_STRINGL(&args[0], (const char *) Data, Size);
		ZVAL_LONG(&args[1], FUZZER_HTML_OPTIONS);
		ZVAL_NULL(&args[2]);

		/* Parse untrusted bytes into an HTML5 document (lexbor engine). */
		zend_call_known_function(create, NULL, ce, &retval, 3, args, NULL);

		/* Round-trip direction: serialize the parsed document back to HTML. */
		if (Z_TYPE(retval) == IS_OBJECT && save != NULL) {
			zval retval2;
			zend_call_known_instance_method_with_0_params(save, Z_OBJ_P(&retval), &retval2);
			zval_ptr_dtor(&retval2);
		}
		zval_ptr_dtor(&retval);
		zval_ptr_dtor(&args[0]);

		if (EG(exception)) {
			zend_clear_exception();
		}
	}

	fuzzer_request_shutdown();
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);
	return 0;
}
