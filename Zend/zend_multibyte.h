/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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

#ifndef ZEND_MULTIBYTE_H
#define ZEND_MULTIBYTE_H

typedef struct _zend_encoding zend_encoding;

typedef size_t (*zend_encoding_filter)(unsigned char **str, size_t *str_length, const unsigned char *buf, size_t length TSRMLS_DC);

typedef const zend_encoding* (*zend_encoding_fetcher)(const char *encoding_name TSRMLS_DC);
typedef const char* (*zend_encoding_name_getter)(const zend_encoding *encoding);
typedef int (*zend_encoding_lexer_compatibility_checker)(const zend_encoding *encoding);
typedef const zend_encoding *(*zend_encoding_detector)(const unsigned char *string, size_t length, const zend_encoding **list, size_t list_size TSRMLS_DC);
typedef size_t (*zend_encoding_converter)(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from TSRMLS_DC);
typedef int (*zend_encoding_list_parser)(const char *encoding_list, size_t encoding_list_len, const zend_encoding ***return_list, size_t *return_size, int persistent TSRMLS_DC);
typedef const zend_encoding *(*zend_encoding_internal_encoding_getter)(TSRMLS_D);
typedef int (*zend_encoding_internal_encoding_setter)(const zend_encoding *encoding TSRMLS_DC);

typedef struct _zend_multibyte_functions {
    const char *provider_name;
    zend_encoding_fetcher encoding_fetcher;
    zend_encoding_name_getter encoding_name_getter;
    zend_encoding_lexer_compatibility_checker lexer_compatibility_checker;
    zend_encoding_detector encoding_detector;
    zend_encoding_converter encoding_converter;
    zend_encoding_list_parser encoding_list_parser;
    zend_encoding_internal_encoding_getter internal_encoding_getter;
    zend_encoding_internal_encoding_setter internal_encoding_setter;
} zend_multibyte_functions;

/*
 * zend multibyte APIs
 */
BEGIN_EXTERN_C()

ZEND_API extern const zend_encoding *zend_multibyte_encoding_utf32be;
ZEND_API extern const zend_encoding *zend_multibyte_encoding_utf32le;
ZEND_API extern const zend_encoding *zend_multibyte_encoding_utf16be;
ZEND_API extern const zend_encoding *zend_multibyte_encoding_utf16le;
ZEND_API extern const zend_encoding *zend_multibyte_encoding_utf8;

/* multibyte utility functions */
ZEND_API int zend_multibyte_set_functions(const zend_multibyte_functions *functions TSRMLS_DC);
ZEND_API const zend_multibyte_functions *zend_multibyte_get_functions(TSRMLS_D);

ZEND_API const zend_encoding *zend_multibyte_fetch_encoding(const char *name TSRMLS_DC);
ZEND_API const char *zend_multibyte_get_encoding_name(const zend_encoding *encoding);
ZEND_API int zend_multibyte_check_lexer_compatibility(const zend_encoding *encoding);
ZEND_API const zend_encoding *zend_multibyte_encoding_detector(const unsigned char *string, size_t length, const zend_encoding **list, size_t list_size TSRMLS_DC);
ZEND_API size_t zend_multibyte_encoding_converter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from TSRMLS_DC);
ZEND_API int zend_multibyte_parse_encoding_list(const char *encoding_list, size_t encoding_list_len, const zend_encoding ***return_list, size_t *return_size, int persistent TSRMLS_DC);

ZEND_API const zend_encoding *zend_multibyte_get_internal_encoding(TSRMLS_D);
ZEND_API const zend_encoding *zend_multibyte_get_script_encoding(TSRMLS_D);
ZEND_API int zend_multibyte_set_script_encoding(const zend_encoding **encoding_list, size_t encoding_list_size TSRMLS_DC);
ZEND_API int zend_multibyte_set_internal_encoding(const zend_encoding *encoding TSRMLS_DC);
ZEND_API int zend_multibyte_set_script_encoding_by_string(const char *new_value, size_t new_value_length TSRMLS_DC);

END_EXTERN_C()

#endif /* ZEND_MULTIBYTE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78
 * vim<600: sw=4 ts=4 tw=78
 */
