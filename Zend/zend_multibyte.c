/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available through the world-wide-web at                              |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Masaki Fujimoto <fujimoto@php.net>                          |
   |          Rui Hirokawa <hirokawa@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend.h"
#include "zend_compile.h"
#include "zend_operators.h"
#include "zend_multibyte.h"
#include "zend_ini.h"

static const zend_encoding *dummy_encoding_fetcher(const char *encoding_name TSRMLS_DC)
{
	return NULL;
}

static const char *dummy_encoding_name_getter(const zend_encoding *encoding)
{
	return (const char*)encoding;
}

static int dummy_encoding_lexer_compatibility_checker(const zend_encoding *encoding)
{
	return 0;
}

static const zend_encoding *dummy_encoding_detector(const unsigned char *string, size_t length, const zend_encoding **list, size_t list_size TSRMLS_DC)
{
	return NULL;
}

static size_t dummy_encoding_converter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from TSRMLS_DC)
{
	return (size_t)-1;
}

static int dummy_encoding_list_parser(const char *encoding_list, size_t encoding_list_len, const zend_encoding ***return_list, size_t *return_size, int persistent TSRMLS_DC)
{
	*return_list = pemalloc(0, persistent);
	return_size = 0;
	return SUCCESS;
}

static const zend_encoding *dummy_internal_encoding_getter(TSRMLS_D)
{
	return NULL;
}

static int dummy_internal_encoding_setter(const zend_encoding *encoding TSRMLS_DC)
{
	return FAILURE;
}

static zend_multibyte_functions multibyte_functions = {
	NULL,
	dummy_encoding_fetcher,
	dummy_encoding_name_getter,
	dummy_encoding_lexer_compatibility_checker,
	dummy_encoding_detector,
	dummy_encoding_converter,
	dummy_encoding_list_parser,
	dummy_internal_encoding_getter,
	dummy_internal_encoding_setter
};

ZEND_API const zend_encoding *zend_multibyte_encoding_utf32be = (const zend_encoding*)"UTF-32BE";
ZEND_API const zend_encoding *zend_multibyte_encoding_utf32le = (const zend_encoding*)"UTF-32LE";
ZEND_API const zend_encoding *zend_multibyte_encoding_utf16be = (const zend_encoding*)"UTF-16BE";
ZEND_API const zend_encoding *zend_multibyte_encoding_utf16le = (const zend_encoding*)"UTF-32LE";
ZEND_API const zend_encoding *zend_multibyte_encoding_utf8 = (const zend_encoding*)"UTF-8";

ZEND_API int zend_multibyte_set_functions(const zend_multibyte_functions *functions TSRMLS_DC)
{
	zend_multibyte_encoding_utf32be = functions->encoding_fetcher("UTF-32BE" TSRMLS_CC);
	if (!zend_multibyte_encoding_utf32be) {
		return FAILURE;
	}
	zend_multibyte_encoding_utf32le = functions->encoding_fetcher("UTF-32LE" TSRMLS_CC);
	if (!zend_multibyte_encoding_utf32le) {
		return FAILURE;
	}
	zend_multibyte_encoding_utf16be = functions->encoding_fetcher("UTF-16BE" TSRMLS_CC);
	if (!zend_multibyte_encoding_utf16be) {
		return FAILURE;
	}
	zend_multibyte_encoding_utf16le = functions->encoding_fetcher("UTF-16LE" TSRMLS_CC);
	if (!zend_multibyte_encoding_utf16le) {
		return FAILURE;
	}
	zend_multibyte_encoding_utf8 = functions->encoding_fetcher("UTF-8" TSRMLS_CC);
	if (!zend_multibyte_encoding_utf8) {
		return FAILURE;
	}

	multibyte_functions = *functions;

	/* As zend_multibyte_set_functions() gets called after ini settings were
	 * populated, we need to reinitialize script_encoding here.
	 */
	{
		const char *value = zend_ini_string("zend.script_encoding", sizeof("zend.script_encoding"), 0);
		zend_multibyte_set_script_encoding_by_string(value, strlen(value) TSRMLS_CC);
	}
	return SUCCESS;
}

ZEND_API const zend_multibyte_functions *zend_multibyte_get_functions(TSRMLS_D)
{
	return multibyte_functions.provider_name ? &multibyte_functions: NULL;
}

ZEND_API const zend_encoding *zend_multibyte_fetch_encoding(const char *name TSRMLS_DC)
{
	return multibyte_functions.encoding_fetcher(name TSRMLS_CC);
}

ZEND_API const char *zend_multibyte_get_encoding_name(const zend_encoding *encoding)
{
	return multibyte_functions.encoding_name_getter(encoding);
}

ZEND_API int zend_multibyte_check_lexer_compatibility(const zend_encoding *encoding)
{
	return multibyte_functions.lexer_compatibility_checker(encoding);
}

ZEND_API const zend_encoding *zend_multibyte_encoding_detector(const unsigned char *string, size_t length, const zend_encoding **list, size_t list_size TSRMLS_DC)
{
	return multibyte_functions.encoding_detector(string, length, list, list_size TSRMLS_CC);
}

ZEND_API size_t zend_multibyte_encoding_converter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from TSRMLS_DC)
{
	return multibyte_functions.encoding_converter(to, to_length, from, from_length, encoding_to, encoding_from TSRMLS_CC);
}

ZEND_API int zend_multibyte_parse_encoding_list(const char *encoding_list, size_t encoding_list_len, const zend_encoding ***return_list, size_t *return_size, int persistent TSRMLS_DC)
{
	return multibyte_functions.encoding_list_parser(encoding_list, encoding_list_len, return_list, return_size, persistent TSRMLS_CC);
}

ZEND_API const zend_encoding *zend_multibyte_get_internal_encoding(TSRMLS_D)
{
	return multibyte_functions.internal_encoding_getter(TSRMLS_C);
}

ZEND_API const zend_encoding *zend_multibyte_get_script_encoding(TSRMLS_D)
{
	return LANG_SCNG(script_encoding);
}

ZEND_API int zend_multibyte_set_script_encoding(const zend_encoding **encoding_list, size_t encoding_list_size TSRMLS_DC)
{
	if (CG(script_encoding_list)) {
		free(CG(script_encoding_list));
	}
	CG(script_encoding_list) = encoding_list;
	CG(script_encoding_list_size) = encoding_list_size;
	return SUCCESS;
}

ZEND_API int zend_multibyte_set_internal_encoding(const zend_encoding *encoding TSRMLS_DC)
{
	return multibyte_functions.internal_encoding_setter(encoding TSRMLS_CC);
}

ZEND_API int zend_multibyte_set_script_encoding_by_string(const char *new_value, size_t new_value_length TSRMLS_DC)
{
	const zend_encoding **list = 0;
	size_t size = 0;

	if (!new_value) {
		zend_multibyte_set_script_encoding(NULL, 0 TSRMLS_CC);
		return SUCCESS;
	}

	if (FAILURE == zend_multibyte_parse_encoding_list(new_value, new_value_length, &list, &size, 1 TSRMLS_CC)) {
		return FAILURE;
	}

	if (size == 0) {
		pefree(list, 1);
		return FAILURE;
	}

	if (FAILURE == zend_multibyte_set_script_encoding(list, size TSRMLS_CC)) {
		return FAILURE;
	}

	return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78
 * vim<600: sw=4 ts=4 tw=78
 */
