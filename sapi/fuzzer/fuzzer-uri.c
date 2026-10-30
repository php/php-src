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
  | Author: Yuma Kakei <yumasansansan@gmail.com>                         |
  +----------------------------------------------------------------------+
*/

#include <main/php.h>
#include <zend_exceptions.h>

#include "fuzzer.h"
#include "fuzzer-sapi.h"

#define MAX_SIZE (8 * 1024)

static const char *rfc3986_getters[] = {
	"getUriType", "getScheme", "getRawScheme", "getUserInfo", "getRawUserInfo",
	"getUsername", "getRawUsername", "getPassword", "getRawPassword", "getHost",
	"getRawHost", "getHostType", "getPort", "getPath", "getRawPath", "getQuery",
	"getRawQuery", "getFragment", "getRawFragment", "toString", "toRawString",
	"__serialize", "__debugInfo", NULL
};

static const char *rfc3986_withers[] = {
	"withScheme", "withUserInfo", "withHost", "withPath", "withQuery", "withFragment", NULL
};

static const char *whatwg_getters[] = {
	"getScheme", "isSpecialScheme", "getUsername", "getPassword", "getAsciiHost",
	"getUnicodeHost", "getHostType", "getPort", "getPath", "getQuery", "getFragment",
	"toAsciiString", "toUnicodeString", "__serialize", "__debugInfo", NULL
};

static const char *whatwg_withers[] = {
	"withScheme", "withUsername", "withPassword", "withHost", "withPath", "withQuery",
	"withFragment", NULL
};

/* Call a method, discarding its result and any exception it throws. If the result is an
 * object, call to_string_method on it. */
static void call_method(
		zend_object *obj, const char *name, uint32_t argc, zval *argv, const char *to_string_method)
{
	zend_function *fn = zend_hash_str_find_ptr_lc(&obj->ce->function_table, name, strlen(name));
	ZEND_ASSERT(fn != NULL);

	zval retval;
	ZVAL_UNDEF(&retval);
	zend_call_known_instance_method(fn, obj, &retval, argc, argv);
	zend_clear_exception();

	if (to_string_method && Z_TYPE(retval) == IS_OBJECT) {
		call_method(Z_OBJ(retval), to_string_method, 0, NULL, NULL);
	}
	zval_ptr_dtor(&retval);
}

static void fuzz_uri_class(
		const char *class_name, size_t class_name_len, zval *uri, zval *ref,
		const char **getters, const char **withers, const char *to_string_method)
{
	zend_class_entry *ce = zend_hash_str_find_ptr(CG(class_table), class_name, class_name_len);
	ZEND_ASSERT(ce != NULL);
	zend_function *parse = zend_hash_str_find_ptr(&ce->function_table, ZEND_STRL("parse"));
	ZEND_ASSERT(parse != NULL);

	/* Url::parse() reports errors through its third, by-reference argument. */
	zval args[3], retval;
	ZVAL_COPY_VALUE(&args[0], uri);
	ZVAL_NULL(&args[1]);
	ZVAL_NEW_EMPTY_REF(&args[2]);
	ZVAL_NULL(Z_REFVAL(args[2]));
	ZVAL_UNDEF(&retval);
	zend_call_known_function(parse, NULL, ce, &retval, parse->common.num_args, args, NULL);
	zend_clear_exception();
	zval_ptr_dtor(&args[2]);

	if (Z_TYPE(retval) != IS_OBJECT) {
		zval_ptr_dtor(&retval);
		return;
	}

	zend_object *obj = Z_OBJ(retval);
	for (const char **getter = getters; *getter; getter++) {
		call_method(obj, *getter, 0, NULL, NULL);
	}
	call_method(obj, "equals", 1, &retval, NULL);

	if (!Z_ISUNDEF_P(ref)) {
		call_method(obj, "resolve", 1, ref, to_string_method);
		for (const char **wither = withers; *wither; wither++) {
			call_method(obj, *wither, 1, ref, to_string_method);
		}
	}

	zval_ptr_dtor(&retval);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
	if (Size > MAX_SIZE) {
		/* Large inputs have a large impact on fuzzer performance,
		 * but are unlikely to be necessary to reach new codepaths. */
		return 0;
	}

	if (fuzzer_request_startup() == FAILURE) {
		return 0;
	}

	fuzzer_setup_dummy_frame();

	/* The input is a URI, optionally followed by a newline and a string that is passed to
	 * resolve() and the with*() methods. The WHATWG parser strips newlines from its input, so
	 * using them as separator loses little. */
	const uint8_t *newline = memchr(Data, '\n', Size);
	size_t uri_len = newline ? (size_t) (newline - Data) : Size;
	zval uri, ref;
	ZVAL_STRINGL(&uri, (const char *) Data, uri_len);
	if (newline) {
		ZVAL_STRINGL(&ref, (const char *) newline + 1, Size - uri_len - 1);
	} else {
		ZVAL_UNDEF(&ref);
	}

	fuzz_uri_class(ZEND_STRL("uri\\rfc3986\\uri"), &uri, &ref,
		rfc3986_getters, rfc3986_withers, "toRawString");
	fuzz_uri_class(ZEND_STRL("uri\\whatwg\\url"), &uri, &ref,
		whatwg_getters, whatwg_withers, "toAsciiString");
	fuzzer_call_php_func_zval("parse_url", 1, &uri);

	zval_ptr_dtor(&uri);
	zval_ptr_dtor(&ref);

	fuzzer_request_shutdown();
	return 0;
}

int LLVMFuzzerInitialize(int *argc, char ***argv) {
	fuzzer_init_php(NULL);

	/* fuzzer_shutdown_php(); */
	return 0;
}
